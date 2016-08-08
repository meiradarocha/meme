/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme network interface routines.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* These routines are the "standard connector" routines that plug
 * the platform-specific network library into Meme.
 *
 * I want all net-related data structures to be hidden from the core
 * code in Meme, so "void *" is used as the data type for socket
 * descriptors that are passed to these routines.
 *
 * There are two flavors of sockets in the MSock array: regular sockets
 * and message sockets.  Regular sockets are polled and controlled from
 * high-level Meme code.  Message sockets are used by SendMemeMessage()
 * and DispatchIncomingMessages().  They are polled and controlled by
 * a state machine called from NetOften().  The "valid" field in the MSock
 * struct is zero for a free socket, one for an allocated regular socket, 
 * and three for an allocated message socket.
 *
 * This file interfaces to the Windows Sockets API.
 */




/* New messaging:
 * The old design suffers from two flaws.
 *
 * 1) The source IP address and source module ID are not sent to the 
 * destination in the header of the message.
 *
 * 2) The source IP address was erroneously considered to be a unique
 * identifier for the machine from which it originated, as though it
 * were never one of many interfaces.
 *
 * The fixes implemented today are of two major types:
 *
 * 1) The message header has been expanded to include the source host ID
 *    and module ID.
 * 2) The practice of formatting a return message to oneself in the body
 *    of a transmitted message has been eliminated.
 *
 * In addition, the meme NetGetMyHostID has been removed.
 *
 * Ongoing flaws:
 * The system still suffers from easily falsifiable module IDs.  This is
 * a security hole since tasks can impersonate code in other modules and
 * interfere with them.  This can be fixed by forcing tasks to register
 * with the low-level interpreter code.  Any task that has not registered
 * will not be able to transmit a message.  Any task that has registered
 * will be identifiable as belonging to a particular module.
 *
 * Practices regarding the new messaging scheme:
 * 
 * Messages will have source host ID and source module ID fields.  The
 * source host ID field is filled by the low-level code.  The module ID
 * field will be filled by the module code for now.  Eventually this will
 * change.
 *
 * Unlike previous implementations, the message base address shall always
 * be the same, regardless of where the message is encountered.  Previously,
 * the message started at the header when seen by low-level code, but was
 * delivered to the modules with a pointer to the body.  This will no longer
 * be true.
 *
 * The message length has been moved and now represents the total byte count
 * of the message, including the header and the length itself.  This will
 * reduce some confusion.
 *
 */

#include <windows.h>
#include <windowsx.h>	// Preprocessor definitions for things like _fmalloc() etc
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <sys/timeb.h>
#include "config.h"
#include "forth.h"
#include "externs.h"
#include "errors.h"
#include "memenet.h"
#include "prims.h"
#include "messages.h"
#include "rfile.h"
#include "protos.h"
#include "winextrn.h"
#include "mstimer.h"


/************************************************************/
/* Define this symbol to turn on the net monitoring console */

// #define NETMON

/************************************************************/

/***** Value for "no signature" in MSock *******/
#define	INVALID_SIGNATURE	(0)

/* Data structure for the array of sockets.
 * Note: state, buff, and remaining are only used for message sockets.
 */
typedef struct _MSock {
	cell			event;			/* Which event woke the task	*/
	cell			valid;			/* Bit 0 set if used, bit 1 set if
									 * a message socket
									 */
	SOCKET			sock;			/* The socket descriptor		*/
	struct sockaddr_in	saddr;		/* The socket address struct	*/
	cell *			task_active;	/* Pointer to a task's "on" flag */
	HOSTID			myIP;			/* Local IP number				*/
	cell			myPort;			/* Local port number			*/
	HOSTID			herIP;			/* Remote IP number				*/
	cell			herPort;		/* Remote port number			*/
	cell			state;			/* Message socket connect state	*/
	char *			buff;			/* Message buffer				*/
	cell			remaining;		/* # bytes left in message buff */
	mstimer			timeout;		/* The time at which the socket times out */
	cell			ticket;			/* Array index of each MSocket	*/
	cell			signature;		/* Unique ID for each MSocket allocation */
	struct _MSock *	next;			/* Ptr to next socket in linked list */
} MSock;

/* This is the structure of the list of host IDs */
struct  {
	cell			length;
	cell			*addrs;
} HostIDList;

/* States for message sockets */
#define		msDISCONNECTED		(0)
#define		msCONNECTING		(1)
#define		msLISTENING			(2)
#define		msCONNECTED			(3)
#define		msDISCONNECTING		(4)
#define		msDEALLOCATED		(5)

/* Timeout in seconds for message port sockets */
#define		MEMEPORTLISTENTIMEOUT	(0)			/* 0 = Forever */
#define		MEMEPORTCONNECTTIMEOUT	(20)		/* 0 = Forever */

#define		LISTENBACKLOG		(5)

static MSock *	AllocMSock(void);
static void		FreeMSock(MSock *msp);
void			DispatchIncomingMessages(MSock *m);

#ifndef NULL
#define	NULL	( (void *) 0 )
#endif


/* Variables local to stuff in this file */
static MSock			MSockets[NUMSOCKETS];	/* The socket data structure */
static struct linger	dontlinger = {0, 0};
static WSADATA			wsaData;				/* Winsock startup data */

#define	STBUFLEN	(4096)
static char FAR *	statusbuf;					/* Buffer for NetStatus() to pass to recv() */

/* Networking-related globals */
char myhostname[50];

/* The Meme sockets linked lists */
MSock *MSockIdleList;
MSock *MSockActiveList;

/* Externs */
extern int 			NetworkType;			/* Set in forth.c			*/


/*
 * Time routines 
 */
void
setsocktimeout(MSock *s, time_t seconds)
{
	if (seconds <= 0) {
		zerotimer(&s->timeout);
		return;
	}
	settimer(&s->timeout, 1000 * seconds);
}

cell
socktimedout(MSock *s)
{
	return timedout(&s->timeout) ? 1 : 0;
}


/* AllocMSock - Allocate a Meme socket.  Returns pointer if 
 * successful, NULL if failure
 */
static MSock *
AllocMSock(void)
{
	/* The signature is a unique number identifying this allocation of the MSock.
	 * Prevents problems with MSocks getting errors and then being reallocated
	 * while some piece of code busywaits (like SendRemoteMemeMessage).
	 */
	static cell	signature_counter = INVALID_SIGNATURE;
	MSock		*msp;

	if (MSockIdleList == NULL) return NULL;
	msp = MSockIdleList;
	MSockIdleList = msp->next;
	msp->next = MSockActiveList;
	MSockActiveList = msp;

	msp->sock = INVALID_SOCKET;		/* Window's constant for an unused socket */
	msp->task_active = (cell *)0;	/* No task recorded if zero */
	msp->valid = 1;
	msp->state = msDISCONNECTED;
	msp->buff = NULL;
	msp->remaining = 0;

	/* Done in SetUpNetwork() */
	/* msp->ticket = i;				/* Store the array index in the socket structure
									 * for quick lookup
									 */

	if (++signature_counter == INVALID_SIGNATURE) ++signature_counter;
	msp->signature = signature_counter;

	msp->event = 0;
	msp->myIP = msp->myPort = msp->herIP = msp->herPort = 0;
	return msp;
}

static void
FreeMSock(MSock *msp)
{
	struct linger donot;
	MSock **app = &MSockActiveList;
	while (*app && *app != msp) app = &((*app)->next);
	if (*app == NULL) {
		error("Can't free MSock, not on list");
	} else {
		*app = msp->next;
		msp->next = MSockIdleList;
		MSockIdleList = msp;
	}
	if (msp->valid == 1 || msp->valid == 3) {
		msp->valid = 0;
		msp->state = msDEALLOCATED;
		if (msp->sock != INVALID_SOCKET) {	// the initial, or unused, value
			donot.l_onoff = donot.l_linger = 0;
			setsockopt(msp->sock, SOL_SOCKET, SO_LINGER, 
									(const char *)&donot, sizeof(donot));
			closesocket(msp->sock);
		}
	}
	msp->signature = INVALID_SIGNATURE;
}

#define      WS_VERSION_REQD    0x0101
#define      WS_VERSION_MAJOR   HIBYTE(WS_VERSION_REQD)
#define      WS_VERSION_MINOR   LOBYTE(WS_VERSION_REQD)
#define      MIN_SOCKETS_REQD   4	
/* SetUpNetwork - Initialize the network code */
void
SetUpNetwork(void)
{
	int          err;
	int i;
	struct hostent FAR * myhostent;

	MSockActiveList = MSockIdleList = NULL;
	for (i = NUMSOCKETS - 1; i >= 0; i--) {
		MSockets[i].next = MSockIdleList;
		MSockIdleList = &MSockets[i];
		MSockIdleList->valid = 0;
		MSockIdleList->state = msDEALLOCATED;
		MSockIdleList->ticket = i;
		MSockIdleList->signature = INVALID_SIGNATURE;
	}

	/* If we're configured for no networking, don't even try to set up */
	if (NetworkType == netNoNetwork) {
		NetFail = -1;
		return;
	}

	err=WSAStartup(WS_VERSION_REQD,&wsaData);
	if (err != 0) {
	    /* Report that Windows Sockets did not respond to the WSAStartup() call */
		NetFail = 1;
		return;
	} else	if (( LOBYTE (wsaData.wVersion) < WS_VERSION_MAJOR) || 
	      ( LOBYTE (wsaData.wVersion) == WS_VERSION_MAJOR &&
	        HIBYTE (wsaData.wVersion) < WS_VERSION_MINOR)) {

	      /* Report that app requires Windows Sockets version WS_VERSION_REQD  */
	      /* compliance and that the WINSOCK.DLL on system doesn't support it. */
		WSACleanup();
		NetFail = 2;
		return;
	} else	if (wsaData.iMaxSockets < MIN_SOCKETS_REQD ) {
      
	      /* Report that winsock.dll was unable to support the minimum number of  */
	      /* sockets (MIN_SOCKETS_REQD) for the application                       */
		WSACleanup();
		NetFail = 3;
		return;
	/* Allocate the scratch buffer for NetStatus() */
	} else if ( (statusbuf = (char FAR *)_fmalloc(STBUFLEN)) == NULL) {
		error("Couldn't allocate the buffer for NetStatus()\n");
		WSACleanup();
		NetFail = 4;
		return;
	}

	/* Get our hostname and IP number(s) */
	gethostname(myhostname, 50);
	if (*myhostname) {
		myhostent = gethostbyname(myhostname);
		if (myhostent) {
			int i = 0;
			HOSTID **p = (HOSTID **)myhostent->h_addr_list;
			while (*p++) i++;
			HostIDList.length = i;
			HostIDList.addrs = (cell *)malloc(i * sizeof(cell));
			if (HostIDList.addrs == NULL) {
				NetFail = 5;
				*myhostname = '\0';
				WSACleanup();
				return;
			}
			for (i = 0, p = (HOSTID **)myhostent->h_addr_list; i < HostIDList.length; i++, p++) {
				HostIDList.addrs[i] = ntohl( **p );
			}
			NetFail = 0;
			return;
		} else {
			/* Couldn't get myhostent -- we haven't gotten an IP number */
			NetFail = 5;
			*myhostname = '\0';
			WSACleanup();
			return;
		}
	} else {
		/* Couldn't get myhostname */
		NetFail = 6;
		*myhostname = '\0';
		WSACleanup();
		return;
	}
}

/* This routine is called from title() in IO.C to print
 * part of the startup banner
 */
void
NetTitle(void)
{
	char NetFailString[100];
	char DummyIPstring[40];
	unsigned char *dp;

	/* This is not an error, and should not be reported as such (in the switch statement) */

#ifdef BIG_ENDIAN
	dp = (unsigned char *) &DummyIP;
	sprintf(DummyIPstring, "%d.%d.%d.%d", dp[0], dp[1], dp[2], dp[3]);
#else
	dp = (unsigned char *) &DummyIP;
	sprintf(DummyIPstring, "%d.%d.%d.%d", dp[3], dp[2], dp[1], dp[0]);
#endif

	switch (NetFail) {
		case -1:
			sprintf(NetFailString, "Network: disabled in Meme's user configuration.  Dummy IP is %s\n",
					DummyIPstring);
			break;
		case 0:
			DummyIP = htonl(HostIDList.addrs[0]);
			dp = (unsigned char *)&DummyIP;
			sprintf(DummyIPstring, "%d.%d.%d.%d", dp[0], dp[1], dp[2], dp[3]);
			if (httpProxy) {
				sprintf(NetFailString, 
					"Network: Winsock %c.%c  Host: \"%s\" [%s]  Proxy: %s:%d\n",
					LOBYTE(wsaData.wVersion) + '0', HIBYTE(wsaData.wVersion) + '0', 
					myhostname, DummyIPstring, httpProxyHost, httpProxyPort);
			} else {
				sprintf(NetFailString, 
					"Network: Winsock %c.%c  Host: \"%s\" [%s]\n",
					LOBYTE(wsaData.wVersion) + '0', HIBYTE(wsaData.wVersion) + '0', 
					myhostname, DummyIPstring);
			}
			break;
		case 1:
			sprintf(NetFailString, "Network failed: Windows sockets didn't start. Dummy IP is %s\n",
					DummyIPstring);
			break;
		case 2:
			sprintf(NetFailString, "Network failed: need Winsock version %d.%d or later.  Dummy IP is %s\n",
					WS_VERSION_MAJOR, WS_VERSION_MINOR, DummyIPstring);
			break;
		case 3:
			sprintf(NetFailString, "Network failed: too few sockets available, need %d.  Dummy IP is %s\n",
					MIN_SOCKETS_REQD, DummyIPstring);
			break;
		case 4:
			sprintf(NetFailString, "Network failed: couldn't allocate buffer for NetStatus().  Dummy IP is %s\n",
					DummyIPstring);
			break;
		case 5:
			sprintf(NetFailString, "Network failed: local host's IP number not found.  Dummy IP is %s\n",
					DummyIPstring);
			break;
		case 6:
			sprintf(NetFailString, "Network failed: local host's name not found.  Dummy IP is %s\n",
					DummyIPstring);
			break;
	}
	if (NetFail <= 0) {		/* If not an error */
		cprint(NetFailString);	/* Just put it in the edit window if it's around */
	} else {				/* If a real error */
		error(NetFailString);	/* Display a dialog box if the editor window is gone */
	}
}

/* CleanUpNetwork - Shut down the network code */
void
CleanUpNetwork(void)
{
	if (NetFail) return;
	free(statusbuf);
	WSACleanup();
#ifdef REMOVE
	int i;

	for(i = 0; i < NUMSOCKETS; i++) {
		if (MSockets[i].valid && MSockets[i].state == msCONNECTED) {
			(void)NetDisconnect(&MSockets[i], 2);
			while (NetDisconnected(&MSockets[i]) == NETPENDING)
				;
		}
	}
#endif
}

/* NetConnect - Initiate an active open of a TCP connection
 *
 *  *** host is an IP number in network byte order ***
 * port is the port number to connect to
 * timeout is the timeout in seconds
 *
 * Returns:		socket descriptor	if successful.
 * On error:	NETERROR
 * If error, NetError() returns:
 *				NETUNINITIALIZED	if no net driver was initialized.
 *				NETNOSOCKSPACE		if unable to allocate a socket.
 *				NETUNRESOLVEDHOST	if hostname resolution failed.
 *				NETUNRESOLVEDARP	if ARP resolution failed.
 */
cell
NetConnect(host, port, timeout)
unsigned long host;
cell port;
cell timeout;
{
	MSock *s;
//	LPHOSTENT host_info;
	cell err, err2;
	u_long true = 1;
//	u_long ip_addr;
	struct sockaddr_in dest;

	/* If the network isn't up, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return NETERROR;
	}
	if ( !(s = AllocMSock()) ) {
		XV_ERRNO = NETNOSOCKSPACE;
		return NETERROR;
	}
	
	/* Set the timeout */
	setsocktimeout(s, timeout);

	/* Try to allocate a socket */
	s->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (s->sock == INVALID_SOCKET) {
		FreeMSock(s);
		err = WSAGetLastError();
		switch(err) {
			case WSANOTINITIALISED:
			case WSAENETDOWN:
			case WSAEAFNOSUPPORT:
				XV_ERRNO = NETUNINITIALIZED;
				return NETERROR;
			case WSAEMFILE:
			case WSAENOBUFS:
				XV_ERRNO = NETNOSOCKSPACE;
				return NETERROR;
			default:
				XV_ERRNO = err; /* Random choice */
				return NETERROR;
		}
	}
	if (ioctlsocket(s->sock, FIONBIO, (u_long FAR *)&true) != 0) {
		FreeMSock(s);
		XV_ERRNO = NETNOSOCKSPACE;
		return NETERROR;
	}

	/* Bind the socket to our local connection point.  We don't care
	 * what either our IP # or our port # is.
	 */
	s->saddr.sin_family = AF_INET;
	s->saddr.sin_addr.s_addr = INADDR_ANY;
	s->saddr.sin_port = 0;
	if (bind(s->sock, (LPSOCKADDR)&s->saddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		XV_ERRNO = NETNOSOCKSPACE;
		FreeMSock(s);
		return NETERROR;
	}

	/* Set up the destination sockaddr struct */
	dest.sin_family = AF_INET;

	/* The host ID (IP number) is in host byte order */	
	*(u_long *)&(dest.sin_addr.s_addr) = htonl(host);
	
	/* The port number is NOT in network byte order.. */
	dest.sin_port = htons((unsigned short)port);
	
	err = connect(s->sock, (struct sockaddr FAR *)&dest, sizeof(struct sockaddr_in));
	if (err == SOCKET_ERROR) {
		if ( (err2 = WSAGetLastError()) == WSAEWOULDBLOCK) {
			s->state = msCONNECTING;
			/* Important!  Set the herIP and herPort fields for the message socket service
			 * loops in MemeOften() and SendRemoteMemeMessage().
			 */
			s->herIP = host;			/* herIP field is in host byte order */
			s->herPort = port;			/* herPort field is in host byte order */
			return (cell)s;
		} else {
			FreeMSock(s);
			switch(err2) {
				case WSANOTINITIALISED:
				case WSAENETDOWN:
					XV_ERRNO = NETUNINITIALIZED;
					return NETERROR;
				case WSAETIMEDOUT:
					XV_ERRNO = NETTIMEDOUT;
					return NETERROR;
				case WSAEINVAL:
					XV_ERRNO = NETINVALIDSOCK;
					return NETERROR;
				case WSAECONNREFUSED:
					XV_ERRNO = NETCONNECTREFUSED;
					return NETERROR;
				default:
					XV_ERRNO = err2;
					return NETERROR;
			}
		}
	}
}


/* NetListen - Passively open a TCP connection
 *
 * timeout is the timeout in seconds
 *
 * Returns:		socket descriptor	if successful.
 * On error:	NETERROR			if error.
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if no net driver initialized.
 *				NETNOSOCKSPACE		if couldn't malloc a socket.
 *				NETINVALIDSOCK		if the operation couldn't be performed
 *									on this socket.
 */
cell
NetListen(port, timeout)
cell port;
cell timeout;
{
	MSock *s;
	int err;
	u_long true = 1;

	/* If the network isn't our type, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return NETERROR;
	}

	/* Allocate the tcp socket struct */
	s = AllocMSock();
	if (!s) {
		XV_ERRNO = NETNOSOCKSPACE;
		return NETERROR;
	}
		
	/* Set the timeout */
	setsocktimeout(s, timeout);

	/* Try to allocate a socket */
	s->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (s->sock == INVALID_SOCKET) {
		FreeMSock(s);
		err = WSAGetLastError();
		switch(err) {
			case WSANOTINITIALISED:
			case WSAENETDOWN:
			case WSAEAFNOSUPPORT:
				XV_ERRNO = NETUNINITIALIZED;
				return NETERROR;
			case WSAEMFILE:
			case WSAENOBUFS:
				XV_ERRNO = NETNOSOCKSPACE;
				return NETERROR;
			default:
				XV_ERRNO = err;
				return NETERROR;
		}
	}
	if ( ioctlsocket(s->sock, FIONBIO, (u_long FAR *)&true) ) {
		FreeMSock(s);
		XV_ERRNO = NETNOSOCKSPACE;
		return NETERROR;
	}
	/* Bind the socket to our local connection point.
	 * 
	 */
	s->saddr.sin_family = AF_INET;
	s->saddr.sin_addr.s_addr = INADDR_ANY;
	s->saddr.sin_port = htons((unsigned short)port);
	if (bind(s->sock, (struct sockaddr FAR *)&s->saddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		XV_ERRNO = NETNOSOCKSPACE;
		FreeMSock(s);
		return NETERROR;
	}
	if (listen(s->sock, LISTENBACKLOG) == SOCKET_ERROR) {
		switch(WSAGetLastError()) {
			case WSANOTINITIALISED:
			case WSAENETDOWN:
				FreeMSock(s);
				XV_ERRNO = NETUNINITIALIZED;
				return NETERROR;
			case WSAEINPROGRESS:
			case WSAEADDRINUSE:
			case WSAEINVAL:
			case WSAENOTSOCK:
				FreeMSock(s);
				XV_ERRNO = NETINVALIDSOCK;
				return NETERROR;
			case WSAEMFILE:
			case WSAENOBUFS:
				FreeMSock(s);
				XV_ERRNO = NETNOSOCKSPACE;
				return NETERROR;
			default:
				FreeMSock(s);
				XV_ERRNO = WSAGetLastError();
				return NETERROR;
		}
	} else {
		s->state = msLISTENING;
		NetDispatch(s, FD_ACCEPT, timeout * 1000);
		return (cell)s;
	}
}

/* NetConnected - Wait for the connection to be established.
 * Returns:		NETESTABLISHED		if connection established
 *				NETPENDING			if connection still pending
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 *				NETCONNECTREFUSED	if remote host refused connection
 *				NETTIMEDOUT			if the socket timed out
 */
cell
NetConnected(void *v)
{
	MSock *s = v;
	int err;
	int namelen;
	struct sockaddr_in dest;
	fd_set FAR rd[FD_SETSIZE], wr[FD_SETSIZE], ex[FD_SETSIZE];
	struct timeval FAR seltimeout;

	int True = -1;

	/* If the network isn't our type, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	/* If the socket is no longer valid, signal an error */
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return (cell)NETERROR;
	}
	if (socktimedout(s)) {
		FreeMSock(s);
		XV_ERRNO = NETTIMEDOUT;
		return NETERROR;
	}
	if (s->state == msCONNECTING) {	
		FD_ZERO(rd);
		FD_ZERO(wr);
		FD_ZERO(ex);
		FD_SET(s->sock, wr);
		seltimeout.tv_sec = 0L;
		seltimeout.tv_usec = 0L;
		err = select(0, rd, wr, ex, &seltimeout);
		if (err == SOCKET_ERROR) {
			err = WSAGetLastError();
			switch (err) {
				case WSAEWOULDBLOCK:
				case WSAEINPROGRESS:	// Blocking socket operation in progress
					return NETPENDING;
				case WSAENOTSOCK:		// Disconnect finished before we got here
					FreeMSock(s);
					XV_ERRNO = NETCLOSEDEARLY;
					return NETERROR;
			}
		} else if (err == 1) {
			namelen = sizeof(struct sockaddr_in);
			getsockname(s->sock, (struct sockaddr FAR *)&dest,
														(int FAR *)&namelen);
			s->myIP = ntohl(dest.sin_addr.S_un.S_addr);
			s->myPort = ntohs((unsigned short)dest.sin_port);
			getpeername(s->sock, (struct sockaddr FAR *)&dest,
														(int FAR *)&namelen);
			s->herIP = ntohl(dest.sin_addr.S_un.S_addr);
			s->herPort = ntohs((unsigned short)dest.sin_port);
			s->state = msCONNECTED;

#ifdef MEMENET_NODELAY
			setsockopt(s->sock, IPPROTO_TCP, TCP_NODELAY,
								(const char FAR *)&True, sizeof(int));
#endif

			return NETESTABLISHED;
		} else if (err == 0) { /* May be a pending connection, but check exceptions */
			FD_ZERO(wr);
			FD_SET(s->sock, ex);
			err = select(0, rd, wr, ex, &seltimeout);
			if (err == 1) {
				XV_ERRNO = NETCONNECTREFUSED;
				return NETERROR;
			} else if (err == 0) {
				return NETPENDING;
			} else if (err == SOCKET_ERROR) {
				XV_ERRNO = WSAGetLastError();
				return NETERROR;
			}
		}
	} else if (s->state == msCONNECTED) {

#ifdef MEMENET_NODELAY
		setsockopt(s->sock, IPPROTO_TCP, TCP_NODELAY,
							(const char FAR *)&True, sizeof(int));
#endif


		return NETESTABLISHED;
	}
}

/* NetAccepted - Return a *new* socket if the 'listen'ed port makes a connection
 *
 * Input argument is a MSock pointer to a socket that has has NetListen called for it.
 *
 * Returns:		NETPENDING		if there is no connection yet
 *				MSock pointer	if there is a new connection
 * On error:	NETERROR		...and XV_ERRNO is set to the error code
 */

#ifdef REMOVE

cell
NetAccepted(void *v)
{
	MSock *s = v;
	MSock *new;
	SOCKET newsocket;
	int err2;
	int namelen;

	char errMsg[80];

	int True = -1;

	/* If the network isn't our type, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	namelen = sizeof(struct sockaddr_in);

	if (socktimedout(s)) {
		FreeMSock(s);
		XV_ERRNO = NETTIMEDOUT;
		return NETERROR;
	}

	if (s->state == msLISTENING) {
		newsocket = accept(s->sock, (struct sockaddr FAR *)&s->saddr, (int FAR *)&namelen);
#ifdef REMOVE
sprintf(errMsg, "accept(%08x, %08x, %08x) returned %08x\n", s->sock, &s->saddr, &namelen, newsocket);
error(errMsg);
#endif
		if (newsocket == INVALID_SOCKET) {
			err2 = WSAGetLastError();
			switch(err2) {
				case WSAEWOULDBLOCK:
					return NETPENDING;
				case WSAECONNREFUSED:
					FreeMSock(s);
					XV_ERRNO = NETCONNECTREFUSED;
					return NETERROR;
				case WSAETIMEDOUT:
					FreeMSock(s);
					XV_ERRNO = NETTIMEDOUT;
					return NETERROR;
				case WSAENOTSOCK:
					FreeMSock(s);
					XV_ERRNO = NETINVALIDSOCK;
					return NETERROR;
				default:
					FreeMSock(s);
					XV_ERRNO = err2;
					return NETERROR;
			}
		} else {
			/* Trade the socket in the current MSock for the new one */
			new = AllocMSock();
			if (!new) {
				setsockopt(newsocket, SOL_SOCKET, SO_LINGER, 
							(char FAR *)&dontlinger, sizeof(struct linger));
				closesocket(newsocket);
				XV_ERRNO = NETNOSOCKSPACE;
				return NETERROR;
			}
			new->sock = newsocket;
			getsockname(new->sock, (struct sockaddr FAR *)&new->saddr,
													(int FAR *)&namelen);
			new->myIP = ntohl(new->saddr.sin_addr.S_un.S_addr);
			new->myPort = ntohs(new->saddr.sin_port);
			getpeername(new->sock, (struct sockaddr FAR *)&new->saddr,
													(int FAR *)&namelen);
			new->herIP = ntohl(new->saddr.sin_addr.S_un.S_addr);
			new->herPort = ntohs(new->saddr.sin_port);
			new->state = msCONNECTED;

#ifdef MEMENET_NODELAY
			setsockopt(new->sock , IPPROTO_TCP, TCP_NODELAY, 
							(const char *)&True, sizeof(int));
#endif

			return (cell)new;
		}
	} else {
		XV_ERRNO = NETINVALIDSOCK;
		return NETERROR;
	}
}
#else

cell
NetAccepted(void *v)
{
	MSock *s = v;
	MSock *newmsock;
	SOCKET newsocket;
	int err;
	int err2;
	int namelen;
	fd_set FAR rd[FD_SETSIZE];
	fd_set FAR wr[FD_SETSIZE];
	fd_set FAR ex[FD_SETSIZE];
	TIMEVAL seltimeout;
	// char errMsg[80];

	int True = -1;

	/* If the network isn't up, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	namelen = sizeof(struct sockaddr_in);

	if (socktimedout(s)) {
		FreeMSock(s);
		XV_ERRNO = NETTIMEDOUT;
		return NETERROR;
	}
	if (s->state == msLISTENING) {		/* If socket is in listening state */
		FD_ZERO(rd);
		FD_ZERO(wr);
		FD_ZERO(ex);
		FD_SET(s->sock, rd);
		seltimeout.tv_sec = 0;
		seltimeout.tv_usec = 0;
		err = select(s->sock + 1, rd, wr, ex, &seltimeout);


		/* If no bits set by select(), nothing happened so far. */
		if (!err) return NETPENDING;
		if (err == 1) {		/* Incoming conn on sock */
			if (FD_ISSET(s->sock, rd)) {
				newsocket = accept(s->sock, (struct sockaddr FAR *)&s->saddr, (int FAR *)&namelen);
#ifdef REMOVE
sprintf(errMsg, "accept(%08x, %08x, %08x) returned %08x\n", s->sock, &s->saddr, &namelen, newsocket);
error(errMsg);
#endif
				if (newsocket == INVALID_SOCKET) {
					err2 = WSAGetLastError();
					switch(err2) {
						case WSAEWOULDBLOCK:
							return NETPENDING;
						case WSAECONNREFUSED:
							FreeMSock(s);
							XV_ERRNO = NETCONNECTREFUSED;
							return NETERROR;
						case WSAETIMEDOUT:
							FreeMSock(s);
							XV_ERRNO = NETTIMEDOUT;
							return NETERROR;
						case WSAENOTSOCK:
							FreeMSock(s);
							XV_ERRNO = NETINVALIDSOCK;
							return NETERROR;
						default:
							FreeMSock(s);
							XV_ERRNO = err2;
							return NETERROR;
					}
				} else {
					/* Trade the socket in the current MSock for the new one */
					newmsock = AllocMSock();
					if (!newmsock) {
						setsockopt(newsocket, SOL_SOCKET, SO_LINGER, 
									(char FAR *)&dontlinger, sizeof(struct linger));
						closesocket(newsocket);
						XV_ERRNO = NETNOSOCKSPACE;
						return NETERROR;
					}
					newmsock->sock = newsocket;
					getsockname(newmsock->sock, (struct sockaddr FAR *)&newmsock->saddr,
															(int FAR *)&namelen);
					newmsock->myIP = ntohl(newmsock->saddr.sin_addr.S_un.S_addr);
					newmsock->myPort = ntohs(newmsock->saddr.sin_port);
					getpeername(newmsock->sock, (struct sockaddr FAR *)&newmsock->saddr,
															(int FAR *)&namelen);
					newmsock->herIP = ntohl(newmsock->saddr.sin_addr.S_un.S_addr);
					newmsock->herPort = ntohs(newmsock->saddr.sin_port);
					newmsock->state = msCONNECTED;

#ifdef MEMENET_NODELAY
					setsockopt(newmsock->sock, IPPROTO_TCP, TCP_NODELAY, 
						(const char *)&True, sizeof(int));
#endif

					return (cell)newmsock;
				}
			} else {		/* If err is 1, and our socket didn't get the "hit", then we got a bug! */
				FreeMSock(s);
				XV_ERRNO = NETINVALIDSOCK;
				return NETERROR;
			}
		} else if (err == SOCKET_ERROR) {
			err = WSAGetLastError();
			switch (err) {
				case WSANOTINITIALISED:
				case WSAENETDOWN:	// Blocking socket operation in progress
					XV_ERRNO = NETUNINITIALIZED;
					return (cell)NETERROR;
				case WSAENOTSOCK:		// Disconnect finished before we got here
					FreeMSock(s);
					XV_ERRNO = NETINVALIDSOCK;
					return NETERROR;
			}
		}
	} else {		/* Socket's state is not msLISTENING */
		XV_ERRNO = NETINVALIDSOCK;
		return NETERROR;
	}
}

#endif

/* NetStatus - Return number of characters waiting on net connection
 *
 * s is a socket descriptor (ptr to MSock struct).
 *
 * Returns:		number of bytes waiting
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 *				NETCLOSEDEARLY		if connection closed
 */
cell
NetStatus(void *v)
{
	MSock *s = v;
	cell actual;
	cell err;

	/* If the network isn't running, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	/* If the socket is no longer valid, signal an error */
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return (cell)NETERROR;
	}
	
	actual = recv(s->sock, statusbuf, STBUFLEN, MSG_PEEK);
	if (actual == SOCKET_ERROR) {
	 	err = WSAGetLastError();
		switch(err) {
			case WSAEWOULDBLOCK:
				return 0;
			case WSAENOTCONN:
			case WSAECONNRESET:
				XV_ERRNO = NETCLOSEDEARLY;
				FreeMSock( s );
				return NETERROR;
			default:
				XV_ERRNO = err;
				FreeMSock( s );
				return NETERROR;
		}
	} else {
		return actual;
	}
}

/* NetRead - Read bytes from the network connection
 *
 * s is a socket descriptor (ptr to MSock struct).
 * buffer is a pointer to a space for the read data
 * len is the maximum length that will be read
 *
 * Returns:		actual number of bytes read
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 *				NETCLOSEDEARLY		if connection closed
 */
cell
NetRead(v, buffer, len)
void *v;
char FAR *buffer;
cell len;
{
	MSock *s = v;
	cell actual;
	int err;

	/* If the network isn't running, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	/* If the socket is no longer valid, signal an error */
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return (cell)NETERROR;
	}
	
	/* This code works fine on 32-bit versions of Windows, but under
	 * Windows 3.x, some winsock implementations will choke if they
	 * get a length > 32767 bytes.
	 */
	actual = recv(s->sock, buffer, len > 32767 ? 32767 : len, 0);
	switch (actual) {
		case 0:
			/* Connection has closed */
			XV_ERRNO = NETCLOSEDEARLY;
			FreeMSock(s);
			return NETERROR;

		case SOCKET_ERROR:
			err = WSAGetLastError();
			switch(err) {
				case WSAEWOULDBLOCK:
					return (cell)0;
				case WSAENOTCONN:
				case WSAECONNRESET:
					XV_ERRNO = NETCLOSEDEARLY;
					FreeMSock(s);
					return NETERROR;
				default:
					XV_ERRNO = WSAGetLastError();
					FreeMSock(s);
					return NETERROR;
			}
		default:
			return actual;
	}
}

/* NetWrite - Write bytes to the network connection
 *
 * buffer is a pointer to the data to write
 * len is the number of bytes to write
 *
 * Returns:		actual number of bytes written
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 *				NETCLOSEDEARLY		if connection is closed
 */
cell
NetWrite(v, buffer, len)
void *v;
char FAR *buffer;
cell len;
{
	MSock *s = v;
	cell actual;

	/* If the network isn't running, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return (cell)NETERROR;
	}

	/* If the socket is no longer valid, signal an error */
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return (cell)NETERROR;
	}

	/* Under Windows 3.x, some winsock implementations will choke
	 * if they see a length > 32767 bytes.
	 */
//	actual = send(s->sock, buffer, len, 0);
	actual = send(s->sock, buffer, len > 32767 ? 32767 : len, 0);
	if (actual == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			return (cell)0;
		} else {
			FreeMSock(s);
			XV_ERRNO = WSAGetLastError();
			switch (XV_ERRNO) {
				case WSAECONNRESET:
				case WSAENOTCONN:
					XV_ERRNO = NETCLOSEDEARLY;
					return NETERROR;
				default:
					return NETERROR;
			}
		}
	} else {
		return actual;
	}
}

/* NetDisconnect - Initiate closing of the net connection
 *
 * s is the socket descriptor
 *
 * Returns:		NETNOERROR
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 */
cell
NetDisconnect(void *v, cell timeout)
{
	MSock *s = v;
	struct linger lingerstruct;		/* Graceful shutdown timer */
	int err;

	/* If the network isn't our type, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return NETERROR;
	}
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return NETERROR;
	}
	setsocktimeout(s, timeout);
	/* Linger and use non-zero interval for graceful disconnect. We're non-blocking
	 * so we should get WSAEWOULDBLOCK
	 */
	lingerstruct.l_onoff = 0;
	lingerstruct.l_linger = (unsigned short)timeout;
	setsockopt(s->sock, SOL_SOCKET, SO_LINGER, (void FAR *)&lingerstruct, sizeof(lingerstruct));
	err = closesocket(s->sock);
	if (err == 0) {
		s->state = msDISCONNECTING;
		return NETPENDING;
	}
	if (err == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
		s->state = msDISCONNECTING;
		return NETPENDING;
	}
	s->sock = INVALID_SOCKET;	/* Make sure FreeMSock doesn't try to close it again */
	FreeMSock(s);
	XV_ERRNO = NETINVALIDSOCK;
	return NETERROR;
}

/* NetDisconnected - Wait for disconnect
 *
 * s is the socket descriptor
 *
 * Returns:		NETPENDING			if still connected
 *				NETDISCONNECTED		if disconnection
 * On error:	NETERROR
 * On error, NetError() returns:
 *				NETUNINITIALIZED	if net driver not initialized
 *				NETINVALIDSOCK		if not a valid socket
 *				NETCLOSEDEARLY		if the socket is already closed
 *				NETTIMEDOUT			if no disconnection before timeout
 */
cell
NetDisconnected(void *v)
{
	fd_set FAR rd[FD_SETSIZE], wr[FD_SETSIZE], ex[FD_SETSIZE];
	struct timeval FAR seltimeout;

	MSock *s = v;
	cell err;
	/* If the network isn't our type, get out */
	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return NETERROR;
	}
	if (s->valid != 1 && s->valid != 3) {
		XV_ERRNO = NETINVALIDSOCK;
		return NETERROR;
	}
	if (socktimedout(s)) {
		FreeMSock(s);
		XV_ERRNO = NETTIMEDOUT;
		return NETERROR;
	}
	FD_ZERO(rd);
	FD_ZERO(wr);
	FD_ZERO(ex);
	FD_SET(s->sock, rd);
	seltimeout.tv_sec = 0L;
	seltimeout.tv_usec = 0L;
	err = select(0, rd, wr, ex, &seltimeout);
	if (err == SOCKET_ERROR) {
		err = WSAGetLastError();
		switch (err) {
			case WSAEWOULDBLOCK:
			case WSAEINPROGRESS:	// Blocking socket operation in progress
				return NETPENDING;
			case WSAENOTSOCK:		// Disconnect finished before we got here
				s->sock = INVALID_SOCKET;	/* Make sure FreeMSock() doesn't reclose the socket */
				FreeMSock(s);
				return NETDISCONNECTED;
		}
	} else if (err == 0) {
		return NETPENDING;
	} else if (err == 1) {
		s->sock = INVALID_SOCKET;	/* Make sure FreeMSock() doesn't reclose the socket */
		FreeMSock(s);
		return NETDISCONNECTED;
	}
}

/*
 * NetOften - Service the network.  Called from MemeOften().
 */
static	MSock * listenSocket = NULL;	/* Meme port-listening socket */
void
NetOften(void)
{
	MSock *msp;
	char str[80];
	cell rtn;
	MSock *newmsp;
	MSock *nextmsp;


#ifdef NETMON

	void NetMon(void);

	NetMon();

#endif
	/* Walk through the socket array, servicing each active socket */
	for (msp = MSockActiveList; msp;) {
		if (msp->valid == 3) {

			/* Make sure, if the MSock that msp points at is unlinked (freed), that
			 * we have the link to the next active socket
			 */
			nextmsp = msp->next;
			switch(msp->state) {
				case msLISTENING:
					rtn = NetAccepted(msp);
					if (rtn == NETERROR) {
						if (msp == listenSocket) listenSocket = NULL;
						break;
					}
					if (rtn == NETPENDING) break;
					/* Arrive here if we've accepted a new connection. */
					/* Service the newly connected message socket */
					newmsp = (MSock *)rtn;
					newmsp->valid |= 2;			/* Mark as msg socket */
					DispatchIncomingMessages(newmsp);
					break;
				case msCONNECTING:
					rtn = NetConnected(msp);
					if (rtn == NETERROR || rtn == NETPENDING) break;
					/* If not NETPENDING or NETERROR, it must be NETESTABLISHED.  Drop through. */
				case msCONNECTED:
					DispatchIncomingMessages(msp);
					break;
				case msDISCONNECTING:
					(void)NetDisconnected(msp);
					break;
				case msDISCONNECTED:
					sprintf(str, "NetOften(): valid = 3 but state = msDISCONNECTED. msp = 0x%x\n", msp);
					error(str);
					break;
				case msDEALLOCATED:
					sprintf(str, "NetOften(): valid = 3 but state = msDEALLOCATED. msp = 0x%x\n", msp);
					error(str);
					break;
				default:
					sprintf(str, "Bad state in NetOften(): %d\n", msp->state);
					error(str);
			}

			/* Link to the next active MSock */
			msp = nextmsp;
		} else {
			msp = msp->next;
		}
	}
	/* Make sure there is a listening socket */
	if (listenSocket == NULL) {
		rtn = NetListen(MEMEPORT, MEMEPORTLISTENTIMEOUT);
		if (rtn != NETERROR) {
			listenSocket = (MSock *)rtn;
			listenSocket->valid |= 2;
		}
	}
}

unsigned long
NetName2HostID(char *name)
{
	LPHOSTENT host_info;
	unsigned long ip_addr;
	unsigned int b1, b2, b3, b4;
	unsigned char *p = (unsigned char *)&ip_addr;
	int rtn;

	rtn = sscanf(name, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
	if (rtn == 4) {
		p[0] = (unsigned char)b4; p[1] = (unsigned char)b3;
		p[2] = (unsigned char)b2; p[3] = (unsigned char)b1;
		return ip_addr;
	}

	/* If networking isn't running, only check whether the name is "localhost",
	 * otherwise call gethostbyname().
	 */
	if (NetFail) {
		char lchost[80];
		char *p1 = name, *p2 = lchost;
		while (*p1) {
			*p2 = isupper((*p1)) ? tolower((*p1)) : *p1;
			p1++, p2++;
		}
		*p2 = '\0';
		if (!strncmp(lchost, "localhost", 9)) {
			return (unsigned long) 0x7f000001;
		} else {
			return DummyIP;
		}
	}

	/* Check with the networking name resolution routine */
	ip_addr = inet_addr(name);
	if (ip_addr == INADDR_NONE) {

		/* Try to resolve name string as a hostname */
		host_info = gethostbyname(name);
		if (host_info == NULL) {
			return 0;
		} else {	// The host info is in network byte order, convert it
			return ntohl( *(u_long *)(host_info->h_addr_list[0]) );
		}
	} else {		// The resolved IP number is in network byte order, convert it
			return ntohl( ip_addr );
	}
}

/* Return a flag if hostid is one of IP numbers associated with
 * an interface on this machine.
 */
cell
IsMyHostID(HOSTID hostid)
{
	int i = HostIDList.length;

	while (--i >= 0) {
		if (hostid == (unsigned cell)HostIDList.addrs[i]) return -1;
	}
	return 0;
}

/* Return this host's IP number, in host byte order, that's associated
 * with a socket.
 */
HOSTID
SockMyIP(void *s)
{
    return ((MSock *)s)->myIP;
}

/* Return the local port number associated with a socket */
cell
SockMyPort(void *s)
{
	return ((MSock *)s)->myPort;
}

/* Return the remote IP number associated with a socket */
HOSTID
SockHerIP(void *s)
{
	return((MSock *)s)->herIP;
}

/* Return the remote port number associated with a socket */
cell
SockHerPort(void *s)
{
	return((MSock *)s)->herPort;
}


/****************************************************************************
 * Following code is for processing Meme messages.
 */

/*
 * Handle messages sent to the main task.  Non-zero if failure.
 */
cell
HandleMaintaskReceiverMessage(Message *msg)
{
	if (maintaskReceiver) {
		Xexecute_one_with_one_arg((token_t)maintaskReceiver, (cell)msg);
		return (cell)0;
	} else {
		return (cell)1;
	}
}	



/*
#define	NOISYSENDMESSAGE
 */

/* SendLocalMemeMessage delivers a message on the local host. Returns
 * 0 if successful, 1 if error
 */
cell
SendLocalMemeMessage(Message *msg, cell force)
{
    CModule *cmp;
#ifdef NOISYSENDMESSAGE
    char s[80];
#endif

	/* If dest host ID is localhost, msg originated locally.  Set
	 * source host ID to localhost.
	 */
	if (msg->destHostID == LOCALHOST) {
			msg->srcHostID = LOCALHOST;
	}
	if (msg->destModuleID == FILESERVERID) {
		HandleFileServerMessage(msg);
		return 0;
	} else if (msg->destModuleID == MAINTASKID) {
		HandleMaintaskReceiverMessage(msg);
		return 0;
	}
/* I can't really remember what this is for.  As far as I can tell,
 * if force = TRUE, delivery of the message to the deck module is forced.
 * This must have been for security, so the deck module can act as a sentinel
 * guard against BAAAAD messages.  For right now, I'm taking it out.
 */
#ifdef REMOVE
	if (force) {
		cmp = WhichModule((module *)DECKMODULEID);
	} else {
		cmp = WhichModule((module *)(msg->destModuleID));
	}
#else
	cmp = WhichModule((module *)(msg->destModuleID));
#endif

	if (cmp) {
#ifdef NOISYSENDMESSAGE
sprintf(s, "CModule ptr %x\n", cmp);
cprint(s);
#endif
		/* If there's no receiver meme, return immediately */
		if (cmp->ptr->CFAOfReceiver == 0) {
#ifdef NOISYSENDMESSAGE
cprint("CFA of rcvr was 0\n");
#endif
			return (cell)1;
		}
#ifdef NOISYSENDMESSAGE
sprintf(s, "CFA of rcvr %x msg addr %x msg code %x\n",
cmp->ptr->CFAOfReceiver, msg, *(cell *)msg->body);
cprint(s);
#endif
		Xexecute_one_with_one_arg((token_t)cmp->ptr->CFAOfReceiver,
											(cell)msg);
		return (cell)0;
	} else {
		return (cell)1;
	}
}

/* SendRemoteMemeMessage delivers a message to a remote host.  Returns
 * 0 if successful, 1 if error, and -1 if pending.
 *
 * This code used to hang if the output buffer for the socket filled up,
 * because the loop containing the NetWrite only exited if the message
 * had been compeletely sent.
 *
 * There is now a set of five static variable that holds the state in
 * case the buffer fills.
 * in_progress - Flag.  Non-zero if the buffer filled and the routine exited
 * 				without sending the whole message.
 * i_psock - Pointer to the MSock we're writing on.
 * socksignature - Unique ID per allocation of an MSock.  Catches
 * 				situations where MSock is deallocated and reallocated before we get back.
 * sendbuffer - Pointer to base of malloc'd buffer.
 * sendptr - Pointer to the first unsent byte in buffer.
 * bytes_undone - Number of unsent bytes.
 * NOTE that if in_progress is zero, the other variables have no meaning.
 */
cell
SendRemoteMemeMessage(Message *msg)
{
	/* State for unfinished transmissions */
	static		cell	in_progress = 0;
	static		MSock	*i_psock;
	static		cell	socksignature;
	static		char	*sendbuffer;
	static		char 	*sendptr;
	static		cell	bytes_undone;

#ifdef NOISYSENDMESSAGE
    char s[80];
#endif
	cell remaining;
	char *msgp;
	cell actual;
	MSock *tmp;

	if (NetFail) {
		XV_ERRNO = NETUNINITIALIZED;
		return 1;
	}

	/* If there is an unfinished message, try to finish sending it. */
	if (in_progress) {
		if (i_psock->signature == socksignature) {	/* Still the same socket as before. */
			/* Write the message on the net connection */
			do {
				actual = NetWrite(i_psock, sendptr, bytes_undone);

				/* If net error, drop this message on the floor. */
				if (actual == NETERROR) {
					if (i_psock->remaining) {
						free(i_psock->buff);	/* Deallocate receive buffer if necessary */
					}
					break;
				}

				/* Buffer's full again!  Return -1 cuz we haven't touched the calling arg message. */
				if (actual == 0) return -1;

				bytes_undone -= actual;			/* Decrement # of bytes left */
				sendptr += actual;				/* Move pointer to first unsent byte */
			} while (bytes_undone) ;			/* Loop till message is sent */
		}
		/* Arrive here if either net error or message sent.  In either case,
		 * shut off in_progress flag and free the buffer.
		 */
		in_progress = 0;
		free(sendbuffer);
	}	

	/* Arrive here ready to send calling arg message. */

	/* Look for the destination host's IP number in the list of
	 * message sockets
	 */
	for (tmp = MSockActiveList; tmp; tmp = tmp->next) {
		if (tmp->valid == 3 && tmp->herIP == msg->destHostID &&
			(tmp->state == msCONNECTING || tmp->state == msCONNECTED) ) break;
	}
	if (!tmp) {		/* If message socket not found */
		tmp = (MSock *)NetConnect(msg->destHostID, MEMEPORT, MEMEPORTCONNECTTIMEOUT);
		if ((cell)tmp == NETERROR) {
			return (cell)1;	/* Return error status */
		}
		tmp->valid |= 2;	/* Mark as message socket - start state machine */
		return (cell)-1;	/* Return pending status */
	} else {				/* If message socket found */
		if (tmp->state != msCONNECTED) {
			return (cell)-1;	/* Return pending status */
		}

		/* Set the source host ID in the message header */
		msg->srcHostID = SockMyIP(tmp);

		/* Write the message on the net connection */
		remaining = msg->len;
		msgp = (char *)msg;
		do {
			actual = NetWrite(tmp, msgp, remaining);
			if (actual == NETERROR) {
#ifdef NOISYSENDMESSAGE
sprintf(s, "NetWrite() failed. Msg addr %lx msg number %x\n",
msg, *(cell *)msg->body);
#endif
				if (tmp->remaining) {
					free(tmp->buff);
					tmp->remaining = 0;
				}
				return (cell)1;
			}

			/* If the output buffer is full, save the partial message
			 * for the next time we enter this routine.
			 */
			if (actual == 0) {
				sendbuffer = malloc(remaining);
				if (!sendbuffer) {	/* Oh, well.  Can't preserve the data for later */
					return 1;	/* Return error */
				}
				memcpy(sendbuffer, msgp, remaining);
				bytes_undone = remaining;
				i_psock = tmp;
				socksignature = tmp->signature;
				sendptr = sendbuffer;
				in_progress = 1;
				return 0;			/* Return 0 so we don't get passed this calling arg again */
			}
			
			/* Arrive here if the last NetWrite was normal. */
			remaining -= actual;
			msgp += actual;
		} while (remaining) ;
		return (cell)0;
	}
}

/* SendMemeMessage delivers a message according to the host ID and 
 * module ID in the header.  msg is a pointer to the message.  
 * force = TRUE if delivery to the deck module is to be forced.
 * It returns 0 if successful, 1 if error, and -1 if pending.
 */
cell
SendMemeMessage(void *vp, cell force)
{
	Message *msg = (Message *)vp;
#ifdef NOISYSENDMESSAGE
    char s[80];

sprintf(s, "DestIP %x Mod ID %x Msg # %x\n", msg->destHostID,
msg->destModuleID, *(cell *)msg->body);
cprint(s);
#endif
    if (msg->destHostID == LOCALHOST) {
		return SendLocalMemeMessage(msg, force);
	}
	return SendRemoteMemeMessage(msg);
}

/* Return TRUE if the header on msg does not pass a check for
 * reasonable values for its fields
 */
cell
InsaneHeader(Message *m, MSock *sock)
{
	if (m->srcHostID != SockHerIP(sock)) return -1;
	if (m->destHostID != SockMyIP(sock)) return -1;
	if (WhichModule(m->destModuleID) == 0 &&
		m->destModuleID != FILESERVERID &&
		m->destModuleID != MAINTASKID &&
		m->destModuleID != DECKMODULEID) return -1;
	if (m->len < MSGHEADERLEN) return -1;
	return 0;
}

/* This is the maximum number of messages that will be dispatched
 * on a single call to DispatchIncomingMessages().
 */
// #define	MAXDISPATCH	(4)

/*
 * Number of consecutive NetReads done before returning
 */
// #define MAXREADS	(8)

/*
 * Longest interval, in milliseconds, that we stay in this routine.
 */
#define	MAXTARRY	(50000)

/* This code has several fallacies in it.  First, it is not possible to check the
 * destination IP number on a packet to decide whether it's mine.  There are too
 * many possible interfaces to check.  Second, I cannot write my return IP number
 * into a packet to have the remote host route it back to me; that IP number
 * must be read at the remote end after the packet has arrived at a particular
 * interface.
 *
 * I must assume that any packet arriving at the local host is here because it
 * has been routed to me.  I cannot route packets at the Meme messaging level;
 * I must assume that it is handled for me lower down.
 *
 * I have been assuming that the important thing to write in the header is the
 * destination host and destination module.  I must also have the source host
 * and source module inserted after a module sends the message, and available
 * on the far end.  That is the only way to assure that the remote host can
 * (a) verify my identity, and (b) return a message to me.
 *
 * In order to find out my module ID, I must have a non-spoofable way of finding
 * the module address corresponding to the task that is sending the message.
 * This isn't hard for tasks allocated in the dictionary, since their address
 * is inside the module.  It is harder for tasks allocated on the heap. 
 */

#ifdef REMOVE	/******* OLD ************/

static void
DispatchIncomingMessages(MSock *m)
{
	cell actual;
	cell msglen;

	void *tmp;
	Message *msg;
	cell rtn;

	mstimer		tm;		/* Millisecond timer */


	for (;;) {
		if (m->remaining) {
			settimer(&tm, 20);		/* Set 20ms timer */
			while (m->remaining < MSGHEADERLEN && !timedout(&tm)) {
				actual = NetRead(m, m->buff + m->remaining,
												MSGHEADERLEN - m->remaining);
// sprintf(errMsg, "%d ", actual); cprint(errMsg);
				if (actual == NETERROR) {
					free(m->buff);
					m->remaining = 0;
					return;
				}
				m->remaining += actual;
			}
			if (m->remaining < MSGHEADERLEN) {
				return;
			}
			msg = (Message *)(m->buff);

			/* Sanity-check the header */
			if (InsaneHeader(msg, m)) {
				sprintf(errMsg, "Insane msg header %08x %08x %08x %08x %08x\n",
								msg->srcHostID, msg->srcModuleID, msg->destHostID,
								msg->destModuleID, msg->len);
				error(errMsg);
				m->remaining = 0;
				free(m->buff);
				(void)NetDisconnect(m, 1);
				return;
			}
			msglen = msg->len;
			tmp = realloc(m->buff, msglen);
			if (!tmp) return;
			m->buff = tmp;
			actual = NetRead(m, m->buff + m->remaining, msglen - m->remaining);
// sprintf(errMsg, "%d ", actual); cprint(errMsg);
			if (actual == NETERROR) {
				free(m->buff);
				m->remaining = 0;
				return;
			}
			m->remaining += actual;
			if (m->remaining == msglen) {
				rtn = SendLocalMemeMessage((Message *)m->buff, 1);
				free(m->buff);
				m->remaining = 0;
			}
		} else {
			rtn = NetStatus(m);
			if (rtn == NETERROR) return;
			if (rtn >= MSGHEADERLEN) {
				m->buff = (char *)malloc(MSGHEADERLEN);
				if (m->buff == NULL) return;
				settimer(&tm, 20);		/* Set 20ms timer */
				while (m->remaining < MSGHEADERLEN && !timedout(&tm)) {
					actual = NetRead(m, m->buff, MSGHEADERLEN);
// sprintf(errMsg, "%d ", actual); cprint(errMsg);
					if (actual == NETERROR) {
						free(m->buff);
						m->remaining = 0;
						return;
					}
					m->remaining += actual;
				}
				if (m->remaining < MSGHEADERLEN) {
					return;
				}
				msg = (Message *)(m->buff);

				/* Sanity-check the header */
				if (InsaneHeader(msg, m)) {
				sprintf(errMsg, "Insane msg header %08x %08x %08x %08x %08x\n",
								msg->srcHostID, msg->srcModuleID, msg->destHostID,
								msg->destModuleID, msg->len);
					error(errMsg);
					m->remaining = 0;
					free(m->buff);
					(void)NetDisconnect(m, 1);
					return;
				}
				msglen = msg->len;
				tmp = realloc(m->buff, msglen);
				if (!tmp) return;
				m->buff = tmp;
				actual = NetRead(m, m->buff + m->remaining,
														msglen - m->remaining);
// sprintf(errMsg, "%d ", actual); cprint(errMsg);
				if (actual == NETERROR) {
					free(m->buff);
					m->remaining = 0;
					return;
				}
				m->remaining += actual;
				if (m->remaining == msglen) {
					rtn = SendLocalMemeMessage((Message *)m->buff, 1);
					free(m->buff);
					m->remaining = 0;
				}

			} else {
				return;
			}
		}
	}
}


#else	/*********** END OLD    START NEW ***************/


static void
DispatchIncomingMessages(MSock *m)
{
	cell actual;
	cell msglen;
	Message *msg;
	void *tmp;

	for(;;) {
		if (m->remaining == 0) {
			if (m->buff == 0) {
				m->buff = (char *)malloc(MSGHEADERLEN);
				if (m->buff == 0) return;
			}
		}
		while (m->remaining < MSGHEADERLEN) {
			actual = NetRead(m, m->buff + m->remaining,
												MSGHEADERLEN - m->remaining);
			if (actual == 0) return;
			if (actual == NETERROR) {
				m->remaining = 0;
				free(m->buff);
				m->buff = 0;
				return;
			}
			m->remaining += actual;
		}
		if (m->remaining >= MSGHEADERLEN) {
			msg = (Message *)m->buff;
			if (InsaneHeader(msg, m)) {
				sprintf(errMsg, "Insane msg header %08x %08x %08x %08x %08x\n",
								msg->srcHostID, msg->srcModuleID,
								msg->destHostID, msg->destModuleID,
								msg->len);
				error(errMsg);
				m->remaining = 0;
				free(m->buff);
				m->buff = 0;
				(void)NetDisconnect(m, 1);
				return;
			}
			msglen = msg->len;
			tmp = realloc(m->buff, msglen);
			if (!tmp) return;
			m->buff = tmp;
			while (m->remaining < msglen) {
				actual = NetRead(m, m->buff + m->remaining,
												msglen - m->remaining);
				if (actual == 0) return;
				if (actual == NETERROR) {
					m->remaining = 0;
					free(m->buff);
					m->buff = 0;
					return;
				}
				m->remaining += actual;
			}
			if (m->remaining == msglen) {
#ifdef REMOVE
/* This code doesn't work.  If the call to SendLocalMemeMessage reenters
 * DispatchIncomingMessages (by running a receiver meme that calls PAUSE)
 * the MSock's state hasn't changed, and the same message is resent and
 * causes infinite recursion.
 * By storing the message buffer pointer in sendingbuff (which is an auto
 * variable) and by clearing the MSock buffer pointer and count BEFORE
 * delivering the message, I make certain that a re-entry to the code will
 * not try to redeliver the same message.  Note that each re-entry will get
 * its own copy of sendingbuff.
 */
				(void)SendLocalMemeMessage((Message *)m->buff, 1);
				free(m->buff);
				m->buff = 0;
				m->remaining = 0;
#else
				Message *sendingbuff = (Message *)m->buff;
				m->buff = 0;
				m->remaining = 0;
				(void)SendLocalMemeMessage(sendingbuff, 1);
				free((char *)sendingbuff);
#endif
			} else if (m->remaining > msglen) {
				sprintf(errMsg, "Incoming msg too long: msglen = %d, remaining = %d\n");
				error(errMsg);
			}
		}
	}
}

#endif  /************** END NEW *************/


/*****************************************************************************/
/*
	#define FD_READ         0x01
	#define FD_WRITE        0x02
	#define FD_OOB          0x04
	#define FD_ACCEPT       0x08
	#define FD_CONNECT      0x10
	#define FD_CLOSE        0x20
*/

#define	FD__TIMER			0x1000

cell
NetDispatch(void *v, cell flags, cell timeoutms)
{
	int rtn;
	MSock *s = v;
	SetTimer(theAsyncWindow, s->ticket, timeoutms, NULL);
	s->task_active = &XV_TASK_ACTIVE;
	rtn = WSAAsyncSelect(s->sock, theAsyncWindow, 
		(unsigned short)s->ticket + WM_USER, flags);
	if (rtn) {
		rtn = WSAGetLastError();
		switch(rtn) {
			case WSANOTINITIALISED:
			case WSAENETDOWN:
				XV_ERRNO = NETUNINITIALIZED;
				break;
			case WSAEINPROGRESS:
			case WSAEADDRINUSE:
			case WSAEADDRNOTAVAIL:
			case WSAEDESTADDRREQ:
			case WSAEFAULT:
			case WSAEINVAL:
			case WSAEISCONN:
			case WSAENOTCONN:
				XV_ERRNO = NETINVALIDSOCK;
				break;
			case WSAECONNREFUSED:
			case WSAENETUNREACH:
				XV_ERRNO = NETCONNECTREFUSED;
				break;
			case WSAEMFILE:
			case WSAENOBUFS:
				XV_ERRNO = NETNOSOCKSPACE;
				break;
			case WSAETIMEDOUT:
				XV_ERRNO  = NETTIMEDOUT;
				break;
			case WSAECONNRESET:
			case WSAECONNABORTED:
				XV_ERRNO = NETCLOSEDEARLY;
		}
		return NETERROR;	
	}
	return NETNOERROR;
}

/*
 * Dispatch network-related messages from Windows to Meme tasks
 */
void
DispatchAsyncMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	int index;

	/* The index into the MSocket array is computed differently for
	 * async messages and timer messages.
	 */
	if (message == WM_TIMER) {
		index = wParam;
	} else if (message >= WM_USER && message < WM_USER + NUMSOCKETS) {
		index = message - WM_USER;
	} else {	// Bogus message
		return;
	}

	/* Kill the timeout timer associated with this socket, since we got here
	 * one way or the other
	 */
	KillTimer(theAsyncWindow, MSockets[index].ticket);

	/* Make sure we don't get any more messages about this socket unless
	 * we ask for them again.
	 */
	WSAAsyncSelect(MSockets[index].sock, theAsyncWindow, 0, 0);

	/* If there is a task associated with the socket and
	 * task is sleeping, wake it up 
	 */
	if (MSockets[index].task_active) {					// If there's an associated task
		*(cell *)(MSockets[index].task_active) = 1;		// Wake it		
	}

	/* Set the proper event in the socket's event field */
	if (message == WM_TIMER) {							// If it's a timer event
		MSockets[index].event |= FD__TIMER;
	} else {											// If it's a network event
		MSockets[index].event |= lParam;
	}
}







/********************************************************************************************
 * NetMon
 */

#ifdef NETMON	/* NETMON is defined at the top of the file */

#define	PERIOD	(100)	/* Number of milliseconds between updates of the console */

double
fnow(void)
{
	struct timeb t;
	ftime(&t);
	return (double)((unsigned long)t.time) + (double)t.millitm / 1000.0;
}

void
NetMon(void)
{
	static HANDLE stdoutput;
	static int firstime = 1;
	int i;
	MSock *mp;
	DWORD written;
	char msg[150];
	COORD startpos = {0, 0};
	static HANDLE currbuff;
	static HANDLE nextbuff;
	static HANDLE swapstorage;	/* For swapping the values of currbuff and nextbuff */
	static SECURITY_ATTRIBUTES foo;
	static int NetMonError = 0;
	static double nexttime = 0.0;
	static char *statenames[] = {
		"msDISCONNECTED ",
		"msCONNECTING   ",
		"msLISTENING    ",
		"msCONNECTED    ",
		"msDISCONNECTING",
		"msDEALLOCATED  "
	};


	if (NetMonError) return;

	if (fnow() < nexttime) return;

	if (firstime) {
		firstime = 0;

		if (!AllocConsole()) {
			MessageBox(NULL, "Failed to allocate console", "NetMon Error", MB_OK);
			NetMonError = 1;
		}

		stdoutput = GetStdHandle(STD_OUTPUT_HANDLE);

		foo.nLength = 4000;
		foo.lpSecurityDescriptor = NULL;
		foo.bInheritHandle = FALSE;
		currbuff = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, &foo,
			CONSOLE_TEXTMODE_BUFFER, 0);
		nextbuff = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, &foo,
			CONSOLE_TEXTMODE_BUFFER, 0);
				

	}

	/* Clear buffer */
	FillConsoleOutputCharacter(currbuff, ' ', 800, startpos, &written);

	/* Display active sockets */
	i = 0;
	mp = MSockActiveList;
	while (mp) {
		i++;
		mp = mp->next;
	}
	SetConsoleCursorPosition(currbuff, startpos);

	sprintf(msg, "Active MSockets: %3d   listenSocket=%08x\n\n", i, listenSocket);
	WriteConsole(currbuff, msg, strlen(msg), &written, NULL);

	mp = MSockActiveList;

	while(mp) {
		sprintf(msg, "Addr 0x%08x  next %08x  state %s  valid %1d\n",
					mp, mp->next, statenames[mp->state], mp->valid);
		WriteConsole(currbuff, msg, strlen(msg), &written, NULL);

		sprintf(msg, "myIP:port %08x:%05d  herIP:port %08x:%05d\n\n",
					mp->myIP, mp->myPort, mp->herIP, mp->herPort);
		WriteConsole(currbuff, msg, strlen(msg), &written, NULL);

		mp = mp->next;
	}

	/* Display idle sockets */
	i = 0;
	mp = MSockIdleList;
	while (mp) {
		i++;
		mp = mp->next;
	}
	sprintf(msg, "Idle MSockets: %3d\n", i);
	WriteConsole(currbuff, msg, strlen(msg), &written, NULL);

	mp = MSockIdleList;
	while (mp) {
		sprintf(msg, "0x%08x      ", mp);
		WriteConsole(currbuff, msg, strlen(msg), &written, NULL);

		mp = mp->next;
	}

	/* Swap buffers */
	SetConsoleActiveScreenBuffer(currbuff);
	swapstorage = currbuff;  currbuff = nextbuff;  nextbuff = swapstorage;

	/* Update the timer */
	nexttime = fnow() + PERIOD / 1000.0;
}

#endif	/* NETMON */

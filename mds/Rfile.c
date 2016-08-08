/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Remote file request code for Meme.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "forth.h"
#include "meme.h"
#include "messages.h"
#include "rfile.h"
#include "memenet.h"
#include "externs.h"
#include "protos.h"

#define		 MAXRFILE		(32)	/* # of elems in remote file req array	*/

#ifdef		_DEBUG
# define	RFTIMEOUT		(3600)	/* # seconds to let file requests pend	*/
#elif		defined(MEMEVIEW)
# define	RFTIMEOUT		(300)
#else
# define	RFTIMEOUT		(20)	/* # seconds to let file requests pend	*/
#endif

/* States for a RFile struct */
#define	rfIDLE				(0)		/* Nuthin happenin						*/
#define rfINITIATED			(1)		/* Request needs to be sent				*/
#define	rfREQUESTED			(2)		/* Request pending						*/
#define	rfSUCCEEDED			(3)		/* Request done, file found				*/
#define	rfTIMEDOUT			(4)		/* Timed out once						*/
#define	rfFAILED			(5)		/* Request done, not found or timed out	*/
#define	rfREPLYING			(6)		/* Replying to another host's request	*/
#define	rfREMOVE			(7)		/* RFile needs to be freed by RFileOften */
#define	rfHCONNECTING		(100)	/* http only: connect to host pending	*/
#define	rfHREQUESTING		(101)	/* http only: writing filename to host	*/
#define	rfHREQUESTED		(102)	/* http only: waiting for reply			*/
#define	rfHREPLYING			(103)	/* http only: reading header in reply	*/
#define	rfHFILEREPLYING		(104)	/* http only: reading file in reply		*/

/* Filetypes for a RFile struct */
#define	rfNoFiletype		(0)		/* RFile struct not in use				*/
#define	rfHTTPFiletype		(1)		/* http: filetype						*/
#define	rfMEMEFiletype		(2)		/* meme: filetype						*/

#define	HTTPPORTNUMBER		(80)
#define HTTPHEADERLENGTH	(400)
/****************************************************************************
 * The following structs are only used by routines in this file.
 */

/* The data structure for a remote file request */
typedef struct _RFile {
	struct _RFile *next;		/* Ptr to next RFile in the linked list	*/
	cell	active;				/* == 0 if RFile not in use				*/			
	cell	tag;				/* Unique identifier for the transaction*/
	cell	state;				/* State of the request					*/
	cell	filetype;			/* Filetype; meme: or http:				*/
	void *	socket;				/* Meme socket (for http: only)			*/
	HOSTID	hostid;				/* Host ID (for http: only)				*/
	cell	port;				/* TCP port (for http: only)			*/
	cell	remaining;			/* # bytes left of filename (http only)	*/
	time_t	time;				/* Time request was made				*/
	char *	filename;			/* Name file to read					*/
	cell	errcode;			/* Error code returned in http header	*/
	cell	filelen;			/* File len returned in http header		*/
	void *	buff;				/* Pointer to data buffer				*/
} RFile;


/****************************************************************************
 * Data that's local to routines in this file
 */
/* Array of remote file structs */
static	RFile	RFiles[MAXRFILE];

/* Holds a tag for each file transaction */
static	cell	nextag = 0;

/* Linked list of the active RFile requests */
static RFile *RFActiveList;

/* Linked list of the idle RFile requests */
static RFile *RFIdleList;

void
SetUpRFiles(void)
{
	int i = MAXRFILE;

	SetUpRFCache();

	RFActiveList = RFIdleList = NULL;

	while (--i >= 0) {
		RFiles[i].next = RFIdleList;
		RFIdleList = &RFiles[i];
	}
}

void
CleanUpRFiles(void)
{
	CleanUpRFCache();
}

void *
AllocRFile(void)
{
	RFile *newguy;

	if (RFIdleList == NULL) return NULL;
	newguy = RFIdleList;
	RFIdleList = RFIdleList->next;
	newguy->next = RFActiveList;
	RFActiveList = newguy;
	newguy->active = 1;
	if (nextag == -1) nextag++;	/* Don't let it be -1 */
	newguy->tag = nextag++;
	newguy->state = rfIDLE;
	newguy->filetype = rfNoFiletype;	/* Uninitialized */
	newguy->socket = 0;
	newguy->hostid = 0;
	newguy->time = time(NULL);
	newguy->buff = NULL;
	newguy->filename = NULL;
	return newguy;
}

void
FreeRFile(void *vp)
{
	RFile *rp = vp;
	RFile *p = RFActiveList;
	RFile **last = &RFActiveList;
	while (p && p != rp) {
		last = &p->next;
		p = p->next;
	}
	if (p == NULL) {	/* rp was not found on the active list */
		error("Corrupted linked lists in the RFiles\n");
		return;
	}
	*last = rp->next;
	rp->next = RFIdleList;
	RFIdleList = rp;
	rp->active = 0;
	if (rp->buff) {
		free(rp->buff);
		rp->buff = NULL;
	}
	if (rp->socket) {
		if (NetDisconnect(rp->socket, 1) != NETERROR) NetDisconnected(rp->socket);
		rp->socket = NULL;
	}
	rp->filetype = rfNoFiletype;		
	if (rp->filename) {
		free(rp->filename);
		rp->filename = NULL;
	}
}

/* Parse a URL.  If error, the pathname portion of the struct holds an 
 * empty string.  This routine does not check for leading or trailing
 * whitespace or newlines.
 * If a URL is passed in the argument "name", the pathname will always have a leading
 * slash.
 * Note that the pathname may or may not have a leading slash if
 * a plain filename was passed as the URL.
 */
void
Name2URL(char *name, URL *up)
{
	char	*cp;
	char	*host;

	cp = name;

	/* Find '://'  in the string */
	while (!(*cp == ':' && cp[1] == '/' && cp[2] == '/') && *cp) cp++;

	if (*cp) {									/* If found */
		up->urltype = utURL;					/* Flag as well-formed URL */
		memcpy(up->filetype, name, cp - name);
		up->filetype[cp - name] = '\0';
	} else {									/* Poorly formed URL	*/
		/* Treat poorly formed URL like a plain filename.
		 * Determine abs vs. rel path name.
		 */
		up->urltype = abspathname(name) ? utABSFNAME : utRELFNAME;

		strcpy(up->filetype, "meme");			/* Default file type	*/
		strcpy(up->hostname, "127.0.0.1");		/* Local host IP number */
		strcpy(up->pathname, name);				/* Name is pathname		*/
		return;
	}

	/* Find the first slash after the "://" */
	cp += 3;				/* Point past "://" */
	host = cp;
	while (*cp && *cp != '/') cp++;
	if (*cp) {									/* If slash found 			*/
		memcpy(up->hostname, host, cp - host);	/* Copy host to struct 		*/
		up->hostname[cp - host] = '\0';			/* Null-terminate 			*/
		strcpy(up->pathname, cp);		/* Copy w/ leading slash to struct	*/
	} else {									/* If slash not found 		*/
		if (cp - host) {						/* If hostname > 0 chars	*/
			memcpy(up->hostname, host, cp - host);	/* Copy host to struct 	*/
			up->hostname[cp - host] = '\0';			/* Null-terminate 		*/
			strcpy(up->pathname, "/");			/* Pathname is slash		*/
		} else {								/* If hostname == 0 chars	*/
			up->urltype = utINVALID;			/* Indicate error			*/
		}
	}
}

/* This routine initiates a request for a "meme:" remote file.  It returns a tag
 * which is a unique identifier for the transaction.
 */
cell
MemeRFileRequest(cell hostid, char *urlname, cell port)
{
	
	RFile *rp;
	FileRequestMsg *mp = (FileRequestMsg *)malloc(256);
	if (!mp) {
		error("RFileRequest: malloc failed\n");
		return (cell)(-1);
	}
	mp->srcModuleID = (cell)FILESERVERID;
	mp->destHostID = hostid;
	mp->destModuleID =  (cell)FILESERVERID;
	mp->len = (sizeof(FileRequestMsg) + (strlen(urlname) + 1) + 4) & ~3;
	mp->msgcode = FILEREQUESTMSGCODE;
	strcpy( (char *)mp + sizeof(FileRequestMsg), urlname);
	rp = AllocRFile();
	if (!rp) return (cell)(-1);
	mp->tag = rp->tag;
	rp->buff = (void *)mp;
	rp->port = port;			/* TCP port to contact */
	rp->state = rfINITIATED;
	rp->filetype = rfMEMEFiletype;
	return rp->tag;
}

/* This routine initiates a request for a "http:" remote file.  It returns a tag
 * which is a unique identifier for the transaction.  It returns tag = -1 on error.
 */
cell
HttpRFileRequest(cell hostid, char *pathname, cell port)
{	
	RFile *rp;

	rp = AllocRFile();
	if (!rp) return (cell)(-1);
	rp->filename = strdup(pathname);
	if (!rp->filename) {
		FreeRFile(rp);
		return (cell)(-1);
	}
	rp->hostid = hostid;
	rp->port = port;
	rp->state = rfINITIATED;
	rp->filetype = rfHTTPFiletype;
	return rp->tag;
}
	

/* This routine is called by Meme primitives that need to open a URL.
 * It sets the fields in req as its return value.
 * req->status is 0 if successful.  It is 1 if error, and -1 if
 * request pending.  If req->status is -1, loop on WaitURL(&req) and wait
 * for status to become 0 or 1.
 *
 * urlname can be a URL of type meme: or http:, or it can be a pathname for
 * a local file.  In the last case, Name2URL() converts the pathname to a
 * URL of type meme: to simplify processing.
 *
 * The request type indicates to OpenURL and WaitURL whether a buffer
 * pointer (to a Meme message) is returned in buff, or a pathname to
 * a local file is returned in tmpname.
 *
 * If successful and the request type is rfrqFILE, then
 * tmpname is the name of a local file that can be opened.
 *
 * If successful and the request type is rfrqBUFFER, then
 * buff points to a malloc'd buffer containing a 
 * FileSendMsg whose filelen and file contents are valid.
 */
void
OpenURL(char *urlname, RFRequest *req, cell type)
{
	URL url;
	HOSTID hostid;
	FILE *fp;
	cell size;
	char cachetmpname[256];

	/* Parse the name into a URL struct.  If urlname is a pathname to a local
	 * file, it is converted to a URL of type meme:.
	 */
	Name2URL(urlname, &url);
	if (url.urltype == utINVALID) {
		req->status = rfrFAILURE;
		return;
	}

	/* If the net interface is disabled, all URLs should be read as local
	 * files.  The MEMEFILES pathname list is used to find the path to the
	 * file.
	 */
	if (NetFail) {

		/* If the "meme:" filetype, it could have been either a URL
		 * or a filename, so check which it was.  If it was a URL,
		 * we know that the first character is '/' so skip it
		 * when passing the pathname to MemeFilePathName().
		 */
		if (!strcmp(url.filetype, "meme")) {

			/* If it was really a URL */
			if (url.urltype == utURL) {
				MemeFilePathName(url.pathname + 1, req->tmpname);	/* Skip leading '/' */
			
			} else {	/* If it was a plain filename */
				MemeFilePathName(url.pathname, req->tmpname);		/* Leave as it was */
			}

			/* If we failed to find the file in one of the MEMEFILES paths */
			if (*req->tmpname == '\0') {
				req->status = rfrFAILURE;						/* Signal failure */
				return;											/* Git */
			}
			req->status = rfrSUCCESS;							/* Otherwise signal success */
			if (type == rfrqFILE) return;						/* If file read, return now */

			/* Buffer request, not file request.  Because this is
			 * a "meme:" filetype, the buffer is made to look
			 * like a Meme FileSendMsg
			 */
			size = FileSize(req->tmpname);
			req->buff = malloc(size + sizeof(FileSendMsg));
			if (req->buff == NULL) {							/* If can't alloc			*/
				req->status = rfrFAILURE;						/* Signal failure			*/
				return;											/* Git						*/
			}

			/* Read the file */
			fp = fopen(req->tmpname, READ_MODE);
			if (!fp) {											/* Can't open the file		*/
				req->status = rfrFAILURE;						/* Signal failure			*/
				return;											/* Git						*/
			}
			fread(req->buff + sizeof(FileSendMsg), 1, size, fp);
			fclose(fp);

			/* Set the file length param in the FileSendMsg */
			((FileSendMsg *)(req->buff))->filelen = size;
			req->status = rfrSUCCESS;							/* Signal success			*/
			return;												/* Git						*/

		/* If the "http:" filetype, it must have been a
		 * URL.  Pass the pathname to MemeFilePathName with
		 * the leading slash trimmed off, otherwise MemeFilePathName
		 * will not try to open the file relative to the MEMEFILES list.
		 */
		} else if (!strcmp(url.filetype, "http")) {
			MemeFilePathName(url.pathname + 1, req->tmpname);
			if (*req->tmpname == '\0') {						/* If file not found		*/
				req->status = rfrFAILURE;						/* Signal failure			*/
				return;											/* Git						*/
			}

			req->status = rfrSUCCESS;							/* Signal success			*/
			if (type == rfrqFILE) return;						/* If file request, git		*/

			/* Not a file request, but a buffer request.
			 * Allocate a buffer and read the file
			 * into it.  The "http:" filetype 
			 * gets a buffer formatted like a FileSendMsg.
			 * 
			 */
			size = FileSize(req->tmpname);						/* Get file size			*/
			req->buff = malloc(size + sizeof(FileSendMsg));		/* Alloc buffer				*/
			if (req->buff == NULL) {							/* If couldn't alloc		*/
				req->status = rfrFAILURE;						/* Signal failure			*/
				return;											/* Git						*/
			}

			fp = fopen(req->tmpname, READ_MODE);				/* Ready to read file		*/
			if (!fp) {											/* If file not opened		*/
				req->status = rfrFAILURE;						/* Signal failure			*/
				return;											/* Git						*/
			}
			fread(req->buff + sizeof(FileSendMsg), 1, size, fp); /* Read file				*/
			fclose(fp);

			/* Set the file length param in the FileSendMsg */
			((FileSendMsg *)(req->buff))->filelen = size;
			req->status = rfrSUCCESS;							/* Signal success			*/
			return;												/* Git						*/

		/* If the filetype is unknown */
		} else {								/* If neither "meme" nor "http" filetype	*/
			req->status = rfrFAILURE;							/* Signal failure			*/
			return;												/* Git						*/
		}
	}

	/* Arrive here if net interface enabled. */

	/* Copy the urlname into the req struct. */
	safestrcpy(req->urlname, urlname, RFREQUESTNAMELEN);


	/* Check whether the requested URL is already in the cache */
	IsCached(urlname, cachetmpname);

	/* If already cached, copy the filename to the req buffer. */
	if (*cachetmpname != '\0') {
		safestrcpy(req->tmpname, cachetmpname, RFREQUESTNAMELEN);

		/* If the req type is "file", we're done */
		if (type == rfrqFILE) {
			req->status = rfrSUCCESS;
			return;
		}

		/* Otherwise, read the file into a buffer.
		 * Read the file into a buffer formatted like a FileSendMsg.
		 */
		size = FileSize(req->tmpname);
		req->buff = malloc(size + sizeof(FileSendMsg));
		if (req->buff == NULL) {
			req->status = rfrFAILURE;
			return;
		}
		fp = fopen(req->tmpname, READ_MODE);
		if (!fp) {
			req->status = rfrFAILURE;
			return;
		}
		fread(req->buff + sizeof(FileSendMsg), 1, size, fp);
		fclose(fp);
		((FileSendMsg *)(req->buff))->filelen = size;
		req->status = rfrSUCCESS;
		return;
	}

	if (!strcmp("meme", url.filetype)) {			/* If the URL starts with "meme:" */
		if (url.urltype != utINVALID) {				/* If it really was a URL */
			hostid = NetName2HostID(url.hostname);
			if (hostid) {							/* If hostname resolution successful */
				if (IsMyHostID(hostid) || hostid == LOCALHOST) {	/* Local file */
					if (url.urltype == utURL) {	/* Really a URL, pathname always has '/' */
						MemeFilePathName(url.pathname + 1, req->tmpname);
					} else {					/* Plain filename, pathname abs or rel */
						MemeFilePathName(url.pathname, req->tmpname);
					}
					if (*req->tmpname == '\0') {
						req->status = rfrFAILURE;
						return;
					}
					req->status = rfrSUCCESS;
					if (type == rfrqFILE) return;
					size = FileSize(req->tmpname);
					req->buff = malloc(size + sizeof(FileSendMsg));
					if (req->buff == NULL) {
						req->status = rfrFAILURE;
						return;
					}
					fp = fopen(req->tmpname, READ_MODE);
					if (!fp) {
						req->status = rfrFAILURE;
						return;
					}
					fread(req->buff + sizeof(FileSendMsg), 1, size, fp);
					fclose(fp);
					((FileSendMsg *)(req->buff))->filelen = size;
					req->status = rfrSUCCESS;
					return;
				} else {			/* Remote file */
					req->type = type;								/* buffer or file	*/
					req->tag = MemeRFileRequest(hostid, urlname, MEMEPORT);	/* Unique id tag	*/
					if (req->tag == -1) {
						req->status = rfrFAILURE;
					} else {					/* Couldn't allocate RFile struct */
						req->status = rfrPENDING;					/* current state	*/
					}
				}
			} else {				/* Hostname resolution unsuccessful */
				req->status = rfrFAILURE;
			}
		} else {					/* Parse of URL was unsuccessful */
			req->status = rfrFAILURE;
		}
	} else if (!strcmp(url.filetype, "http")) {	/* If URL starts with "http:"	*/

		if (httpProxy) {		/* If a http proxy host has been specified */
			if (httpProxyIP == 0) {
				httpProxyIP = NetName2HostID(httpProxyHost);
				if (httpProxyIP == 0) {
					error("http proxy hostname resolution failed");
					req->status = rfrFAILURE;
					return;
				}
			}
			req->type = type;
			req->tag = HttpRFileRequest(httpProxyIP, urlname, httpProxyPort);
			if (req->tag == -1) {		/* If couldn't alloc RFile struct */
				req->status = rfrFAILURE;
			} else {
				req->status = rfrPENDING;
			}
			return;
		} else {				/* No http proxy host has been specified */
			if (*url.pathname) {					/* If URL parse successful		*/
				hostid = NetName2HostID(url.hostname);
				if (hostid) {			/* If hostname resolution successful		*/
					/* Treat http files on localhost the same as on a remote host */
					req->type = type;
					req->tag = HttpRFileRequest(hostid, url.pathname, HTTPPORTNUMBER);
					if (req->tag == -1) {		/* If couldn't alloc RFile struct */
						req->status = rfrFAILURE;
					} else {
						req->status = rfrPENDING;
					}
				} else {				/* If hostname resolution failed */
					req->status = rfrFAILURE;
				}
			} else {					/* Parse of URL was unsuccessful */
				req->status = rfrFAILURE;
			}
		}
	} else {						/* Unknown filetype */
		req->status = rfrFAILURE;
	}
}

/* This routine is called when the status field of a RFRequest is -1,
 * as returned by OpenURL().  It polls the RFile array to find out if
 * the file request has been satisfied.  It sets the status field to 0
 * for success, and 1 for error. The status field stays -1 if the request
 * is still pending.
 */
void
WaitURL(RFRequest *req)
{
	RFile *rp;
	FILE *fp;

	for (rp = RFActiveList; rp; rp = rp->next) {
		if (rp->tag == req->tag && rp->active) break;
	}
	if (rp == NULL) {	/* Couldn't find request */
		error("WaitURL(): Couldn't find request in RFiles\n");
		req->status = rfrFAILURE;
		return;
	}
	switch (rp->state) {
		case rfINITIATED:
		case rfREQUESTED:
		case rfHCONNECTING:
		case rfHREQUESTING:
		case rfHREQUESTED:
		case rfHREPLYING:
		case rfHFILEREPLYING:
			return;
		case rfFAILED:
			FreeRFile(rp);
			req->status = rfrFAILURE;
			return;
		case rfSUCCEEDED:
			req->status = rfrSUCCESS;
			req->buff = rp->buff;
			rp->buff = NULL;	/* Make sure FreeRFile doesn't free buffer */

			/* Create a filename for the cache copy of the file */
			Memetmpname(req->tmpname, req->tag);

			/* Attempt to write the file */
			fp = fopen(req->tmpname, WRITE_MODE);

			/* If not openable for writing */
			if (!fp) {
				strcpy(errMsg, "WaitURL(): Can't open temporary file ");
				strcat(errMsg, req->tmpname);
				strcat(errMsg, "\n");
				error(errMsg);
				free(req->buff);
				req->status = rfrFAILURE;
				FreeRFile(rp);
				return;
			}

			/* Write the file */
			fwrite(req->buff + sizeof(FileSendMsg), 1, 
										((FileSendMsg *)(req->buff))->filelen, fp);
			fclose(fp);

			/* Mark the request buffer as successfully completed */
			req->status = rfrSUCCESS;

			/* Add the temporary file to the cache map */
			AddRFileToCache(req->urlname, req->tmpname);

			/* If the request type is not for a buffer, free the requested buffer */
			if (req->type != rfrqBUFFER) {
				free(req->buff);
			}

			/* Free the rfile buffer */
			FreeRFile(rp);
			return;
	}
}

/* HandleFileServerMessage handles messages to and from the file server,
 * which has a module ID of -2.
 * 
 * The file server either receives a request for a file, or a reply
 * to a request for a file.
 * A request for a file looks like:
 * Source Host ID, Source Module ID, Dest Host ID, Dest Module ID, 
 * len, FileRequest msg code, filename.
 *
 * Source Host ID, Source Module ID, Dest Host ID, Dest Module ID, len,
 * FileSend msg code, filelen, file ...
 * If the filelen is -1, the file was not found.
 */
void
HandleFileServerMessage(Message *msg)
{
	FileRequestMsg *frm;
	FileSendMsg *fsm;
	URL url;
	char pathname[128];
	RFile *rp;
	cell size;
	FILE *fp;
	Message *mp;

	if (*(cell *)(msg->body) == FILEREQUESTMSGCODE) {
		frm = (FileRequestMsg *)msg;
		Name2URL((char *)msg + sizeof(FileRequestMsg), &url);
		if (url.urltype != utINVALID) {	/* If no conversion error */

			/* If this request was in the form of a URL, make sure the
			 * filename is searched for relative to MEMEFILES.  If it
			 * was a local file name, search for it according to its
			 * leading character.
			 */
			if (url.urltype == utURL) {
				MemeFilePathName(url.pathname + 1, pathname);
			} else {
				MemeFilePathName(url.pathname, pathname);
			}
			if (!*pathname) {	/* File not found */
notfound:
				fsm = (FileSendMsg *)malloc(sizeof(FileSendMsg));
				if (!fsm) {
					error("HandleFileServerMessage: can't malloc reply msg\n");
					return;
				}

				fsm->destHostID = frm->srcHostID;
				fsm->destModuleID = frm->srcModuleID;
				/* FIX: find a better way of expressing the msg length */
				fsm->len = sizeof(FileSendMsg);
				fsm->msgcode = FILESENDMSGCODE;
				fsm->tag = frm->tag;
				fsm->filelen = (cell) -1;

				rp = AllocRFile();
				if (!rp) {
					error("HandleFileServerMessage: AllocRFile failed\n");
					free(fsm);
					return;
				}
				rp->state = rfREPLYING;
				rp->buff = fsm;
				return;
			} else {			/* File found */
				size = FileSize(pathname);
				fsm = malloc(sizeof(FileSendMsg) + size);
				if (!fsm) {
					error("HandleFileServerMessage: Couldn't malloc reply\n");
					return;
				}
				fp = fopen(pathname, READ_MODE);
				if (!fp) {
					sprintf(errMsg, "HandleFileServerMessage: Couldn't open file %s\n", pathname);
					error(errMsg);
					return;
				}
				/* FIX: This won't run correctly if fread takes 16-bit ints */
				fread((char *)fsm + sizeof(FileSendMsg), 1, size, fp);
				fclose(fp);
				fsm->srcModuleID = (cell)FILESERVERID;
				fsm->destHostID = frm->srcHostID;
				fsm->destModuleID = frm->srcModuleID;
				/* FIX: find a better way of expressing the msg length */
				fsm->len = size + sizeof(FileSendMsg);
				fsm->msgcode = FILESENDMSGCODE;
				fsm->tag = frm->tag;
				fsm->filelen = size;
				rp = AllocRFile();
				if (!rp) {
					error("HandleFileServerMessage: Couldn't alloc RFile\n");
					free(fsm);
					return;
				}
				rp->state = rfREPLYING;
				if (rp->buff) free(rp->buff);
				rp->buff = fsm;

				/* Force this RFile struct to be handled by the proper case clause
				 * in RFileOften()
				 */
				rp->filetype = rfMEMEFiletype;

				return;
			}
		} else {	/* URL name conversion error */
			goto notfound;
		}
	} else if (*(cell *)(msg->body) == FILESENDMSGCODE) {
		fsm = (FileSendMsg *)msg;
		for (rp = RFActiveList; rp; rp = rp->next) {
			if (rp->tag == fsm->tag && rp->state == rfREQUESTED) {
				break;
			}
		}
		if (rp) {		/* Found the corresponding RFile struct */
			if (fsm->filelen == -1) { /* Requested file not found */
				rp->state = rfFAILED;
				return;
			}
			mp = (Message *)malloc(msg->len);
			if (!mp) {
				error("HandleFileServerMessage: can't alloc msg buff\n");
				rp->state = rfFAILED;
				return;
			}
			memcpy(mp, msg, msg->len);
			rp->state = rfSUCCEEDED;
			if (rp->buff) free(rp->buff);
			rp->buff = mp;
		}	/* Didn't find a corresponding RFile.  The request must
			 * have timed out.  Drop the file on the floor.
			 */
	}
}

/* Convert a string in place to lower-case */
void
LcString(char *s)
{
	int len = strlen(s) + 1;

	while (len--) {
		if (isupper(s[len])) {
			s[len] = tolower(s[len]);
		}
	}
}

/* Copy a string from src to dest, converting to lower-case at the destination */
char *
LcStrcpy(char *dest, const char *src)
{
	char *rtn = strcpy(dest, src);
	LcString(dest);
	return rtn;
}

/* Case-insensitive flavor of strstr() */
char *
Stristr(char *string, const char *strCharSet)
{
	int stringLen;			/* Length of string to search */
	int strCharSetLen;		/* Length of strCharSet */
	char *lcString;			/* Lower-case version of string */	
	char *lcStrCharSet;		/* Lower-case version of strCharSet */
	char *rtn;				/* The value returned by strstr() */

	/* Get string lengths of input args */
	stringLen = strlen(string) + 1;
	strCharSetLen = strlen(strCharSet) + 1;

	/* Allocate space for lower-case versions */
	lcString = alloca(stringLen);
	lcStrCharSet = alloca(strCharSetLen);

	/* Copy the input args and convert the copies to lower-case */
	LcStrcpy(lcString, string);
	LcStrcpy(lcStrCharSet, strCharSet);

	/* Call strstr() on the lower-case versions */
	rtn = strstr(lcString, lcStrCharSet);
	if (rtn) {
		rtn = string + ((uint32)rtn - (uint32)lcString);
		return rtn;
	} else {
		return rtn;
	}
}

void
CopyLine(char *dest, char *src)
{
	while (*src != '\015' && *src != '\012' && *src != '\0') {
		*dest++ = *src++;
	}
	*dest = '\0';
}

/* Interpret the string of digits at textp as an unsigned decimal number 
 * and store its value where rtnvalp points.
 */
void
ParseNumber(char *textp, cell *rtnvalp)
{
	char numberText[100];
	char *np = numberText;

	while (isdigit(*textp)) *np++ = *textp++;
	*np = '\0';
	*rtnvalp = (cell)atoi(numberText);
}

void
ParseHTTPHeader(char *hdr, cell *errcode, cell *filelen)
{
	char line[256];				/* One text line from the header */

	*errcode = *filelen = 0;	/* Initialize the error code and
								 * file length return values to zero
								 */
	for (;;) {

		/* Copy a line of text into the line buffer */
		while (*hdr == '\012' || *hdr == '\015') hdr++;	/* Skip newline chars */
		if (*hdr == '\0') {			/* If the header is empty, we're done */
			if (*filelen == 0) *errcode = -1;
			return;			
		}
		CopyLine(line, hdr);

		/* Check whether this is the line with the result code */
		if (Stristr(line, "HTTP/1.0 ") == line) {
			/* If this is the line with the result code, the numeric
			 * result immediately follows.  Null-terminate it and 
			 * convert it to an int
			 */
			ParseNumber(line + 9, errcode);
		
		/* Check whether this is the line with the file length */
		} else if (Stristr(line, "Content-length: ") == line) {
			ParseNumber(line + 16, filelen);
		}

		/* If we've found both the result code and the content length, return */
		if (*errcode && *filelen) return;

		/* Skip to the end of the header line */
		while (*hdr != '\012' && *hdr != '\015' && *hdr != '\0') hdr++;
	}
}
			
/* Called from MemeOften().  This is necessary because SendMemeMessage()
 * has to be called repeatedly while idling.
 *
 * FreeRFile() must be called with great care from this routine.  There are two
 * ways that a RFile struct is allocated.  One is by a call to OpenURL() and the
 * other is by a message sent by a remote host to request a file.  In the first
 * case, the code surrounding OpenURL() and WaitURL() will free the RFile once the
 * file read is finished.  In the second case, the RFile must be freed in this routine
 * because there is no wrapper code to free the file once finished.  The two cases
 * are differentiated by the rfREPLYING state, which is only entered when replying
 * to a message from a remote host.
 * Note that Meme only replies to "meme:" remote requests; there is no HTTP server in Meme.
 * Also note that FreeRFile is called in error cases, such as when a RFile struct
 * with the IDLE state is found on the active list.
 */
void
RFileOften(void)
{
	RFile 	*rp;
	int		rtn;

	if (!RFActiveList) return;
	for (rp = RFActiveList; rp; rp = rp->next) {

		/* If the socket has timed out, set its state to TIMEDOUT.
		 * Reset the timer.  If the socket times out again, force
		 * it to be freed.
		 */
		if (rp->time + RFTIMEOUT < time(NULL)) {	/* Timed out */
			if (rp->state == rfTIMEDOUT) {
				rp->state = rfFAILED;
				continue;
			}
			rp->state = rfTIMEDOUT;
			rp->time = time(NULL);
			continue;
		}

		/* The filetype is really a server type.  rfMEMEFiletype relies on the Meme remote file
		 * access code, which receives Meme messages as though it were a module. rfHTTPFiletype
		 * talks directly to an HTTP server (by default on port 80).
		 */
		switch(rp->filetype) {
			case rfMEMEFiletype:	/* Server is Meme remote file access */
				switch(rp->state) {
					case rfIDLE:
						error("Bad state rfIDLE for active RFile\n");
						rp->state = rfREMOVE;
						break;
					case rfINITIATED:
						rtn = SendMemeMessage((Message *)rp->buff, 0);
						if (rtn == -1) break;					/* Message send in progress	*/
						if (rtn == 0) rp->state = rfREQUESTED;	/* Message send succeeded	*/
						if (rtn == 1) rp->state = rfFAILED;		/* Message send failed		*/
						break;
					case rfREQUESTED:
					case rfSUCCEEDED:
					case rfFAILED:
					case rfTIMEDOUT:
						break;
					case rfREPLYING:
						rtn = SendMemeMessage((Message *)rp->buff, 0);
						if (rtn == -1) break;	/* If pending */
						rp->state = rfREMOVE;	/* If succeeded or failed */
						break;
					default:
						{
							char errMsg[200];
							sprintf(errMsg, "RFileOften: rfMEMEFiletype: bad state %d for RFile\n",
								rp->state);
							error(errMsg);
							rp->state = rfREMOVE;
						}
						break;
				}
				break;
			case rfHTTPFiletype:	/* Server is an HTTP server */
				switch(rp->state) { 
					case rfIDLE:
						error("Bad state rfIDLE for active RFile\n");
						rp->state = rfREMOVE;
						break;
					case rfINITIATED:
						/* FIX: Have to be able to handle alternate port
						 * numbers specified in the URL.
						 */
						rp->socket = (void *)NetConnect(rp->hostid, HTTPPORTNUMBER, 0);
						if (rp->socket == (void *)NETERROR) {
							rp->state = rfFAILED;
							break;
						}
						rp->state = rfHCONNECTING;
						break;
					case rfHCONNECTING:	
						switch (NetConnected(rp->socket)) {
							case NETPENDING:
								break;
							case NETERROR:
								rp->state = rfFAILED;
								break;
							case NETESTABLISHED:
								rp->buff = (char *) malloc(200);
								if (!rp->buff) {
									rp->state = rfFAILED;
									break;
								}
								strcpy(rp->buff, "GET ");
								strcat(rp->buff, rp->filename);
								strcat(rp->buff, " HTTP/1.0\015\012\015\012");
								rtn = NetWrite(rp->socket, rp->buff, strlen(rp->buff));
								if (rtn == NETERROR) {
									rp->state = rfFAILED;
									break;
								}
								if ((unsigned)rtn < strlen(rp->buff)) {
									rp->state = rfHREQUESTING;
									rp->remaining = strlen(rp->buff) - rtn;
								} else {
									rp->state = rfHREQUESTED;
								}
								break;
						}
						break;
					case rfHREQUESTING:
						rtn = NetWrite(rp->socket, (char *)rp->buff + (strlen(rp->buff) - rp->remaining),
																rp->remaining);
						if (rtn == NETERROR) {
							rp->state = rfFAILED;
							break;
						}
						if (rtn < rp->remaining) {
							rp->remaining += rtn;
							break;
						} else {
							rp->state = rfHREQUESTED;
						}
						break;
					case rfHREQUESTED:
						free(rp->buff);
						rp->buff = (char *)malloc(HTTPHEADERLENGTH);
						if (!rp->buff) {
							rp->state = rfFAILED;
							break;
						}
						rp->remaining = 0;
						rp->state = rfHREPLYING;
						break;
					/* In this state, the "remaining" variable is really the number
					 * of bytes read.  Read until a blank line is received.
					 */
					case rfHREPLYING:
						rtn = NetRead(rp->socket, (char *)rp->buff + rp->remaining,
														HTTPHEADERLENGTH - rp->remaining);
						
						if (rtn == NETERROR) {
							rp->state = rfFAILED;
							break;
						} else if (rtn) {		/* If characters got read */
							/* Search the buffer for a blank line.  p points at
							 * next char to compare.  last points at last char in buff.
							 */
							char *p = rp->buff;
							char *last = (char *)rp->buff + rp->remaining + rtn - 1;
							int foundblankline = 0;

							rp->remaining += rtn;	/* Bump count of chars in buff */

							while (p < last) {		/* While not pointing at last char */

								/* Jump out of loop if we've found two newlines in a row.
								 * Zero the second newline and leave p pointing at the 
								 * char before the first byte of the body.
								 */
								if (!memcmp(p, "\012\012", 2)) {
									foundblankline = 1;
									*++p = '\0';
									break;
								} else if (p < last - 2 && !memcmp(p, "\015\012\015\012", 4)) {
									foundblankline = 1;
									p += 2;
									*p++ = '\0';	/* Zero the second CR */
									*p = '\0';	/* Zero the second LF */
									break;
								} else if (p < last - 2 && !memcmp(p, "\012\015\012\015", 4)) {
									foundblankline = 1;
									p += 2;
									*p++ = '\0';	/* Zero the second CR */
									*p = '\0';	/* Zero the second LF */
									break;
								}
								++p;				/* Otherwise bump pointer */			
							}

							if (foundblankline) { /* If we found the blank line */
							
								/* Try parsing the header for its file length */
								ParseHTTPHeader(rp->buff, &rp->errcode, &rp->filelen);
								if (rp->errcode != 200) {	/* If http read wasn't successful */
									rp->state = rfFAILED;
									break;
								} else {					/* http server result code is "ok" */
									if (last - p) {			/* If any chars were read beyond the header */
										char *tempbuff = (char *)malloc(last - p);
										if (!tempbuff) {
											rp->state = rfFAILED;
											break;
										}
										memcpy(tempbuff, p + 1, last - p);
										rp->buff = (char *)realloc(rp->buff, rp->filelen + sizeof(FileSendMsg));
										if (!rp->buff) {
											rp->state = rfFAILED;
											free(tempbuff);
											break;
										}
										memcpy((char *)rp->buff + sizeof(FileSendMsg), tempbuff, last - p);
										free(tempbuff);
									} else { /* There were no extra characters read after the header. */
										rp->buff = (char *)realloc(rp->buff, rp->filelen + sizeof(FileSendMsg));
										if (!rp->buff) {
											rp->state = rfFAILED;
											break;
										}
									}
									rp->remaining = last - p;
								}
								rp->state = rfHFILEREPLYING;
								((FileSendMsg *)(rp->buff))->filelen = rp->filelen;
							}
						}				

						break;
					/* In this state, the "remaining" variable is really the number
					 * of bytes read.  Read until the remote host closes the connection.
					 */
					case rfHFILEREPLYING:
						if (rp->remaining < rp->filelen) {
							rtn = NetRead(rp->socket, (char *)rp->buff + sizeof(FileSendMsg) + rp->remaining,
															rp->filelen - rp->remaining);
							if (rtn == NETERROR) {
								rp->state = rfFAILED;
								break;
							} else {
								rp->remaining += rtn;
								if (rp->remaining == rp->filelen) {
									rp->state = rfSUCCEEDED;
								}
								break;
							}
						} else {
							rp->state = rfSUCCEEDED;
							break;
						}
					/* These states indicate that the remote file transaction is finished.  WaitURL()
					 * will remove the RFile struct from the active list, so do nothing for these states--
					 * the correct thing to do is to wait for WaitURL() to get called from inner_interpreter().
					 */
					case rfFAILED:
					case rfSUCCEEDED:

					/* Similar thing for rfTIMEDOUT.  The code at the start of the for loop will
					 * set the state to rfREMOVE if the request times out, and the code at the end of
					 * this routine will remove it.
					 */
					case rfTIMEDOUT:
						break;
					default:
						{
							char errMsg[200];
							sprintf(errMsg, "RFileOften: rfHTTPFiletype: bad state %d for RFile\n",
								rp->state);
							error(errMsg);
							rp->state = rfREMOVE;
						}
						break;

				}
				break;
			default:
				{
					char errMsg[200];
					sprintf(errMsg, "RFileOften: bad filetype %d for RFile\n", rp->filetype);
					error(errMsg);
					rp->state = rfREMOVE;
				}
				break;
		}
	}

	/* Remove any RFiles with rfREMOVE as their state.  This can't be done in the above for
	 * loop because it screws up the linked list of active RFiles.
	 * Note that the for loop exits and restarts each time it removes a file from the active
	 * list.
	 */
	do {
		for (rp = RFActiveList; rp; rp = rp->next) {
			if (rp->state == rfREMOVE) {
				FreeRFile(rp);
				break;
			}
		}
	} while (rp) ;
}

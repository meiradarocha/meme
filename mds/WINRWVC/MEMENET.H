/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Header file for Windows-specific network code.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#ifndef		_MEMENET_H_
# define	_MEMENET_H_

#include "meme.h"		/* Message */
/*
 * Maximum number of simultaneously available sockets
 */
#define	NUMSOCKETS			(64)

/*
 * Length of a hostname in the various data structures - note:
 * change the hostname length in cyberspc.mh if you change it here
 */
#define	HOSTNAMELEN			(128)

/* Error codes returned by these routines */
/* This list is also in memetcp.c (for the DOS implementation--change it
 * there if you change it here!
 */
#define	NETDISCONNECTED		(1)
#define	NETESTABLISHED		(1)
#define	NETPENDING			(0)
#define	NETNOERROR			(0)
#define	NETERROR			(-1)

/* Send-message queue structure */
typedef struct _sMsg {
	Message			*msg;			/* Pointer to the message							*/
	char			*sendptr;		/* Pointer to what's left to send in the message	*/
	cell			remaining;		/* Number of bytes left to write					*/
	cell			errcode;		/* Error flag, zero if no error						*/
	struct _sMsg	*fwd;			/* Ptr to next struct in queue, points toward front	*/
	struct _sMsg	*back;			/* Ptr to prev struct in queue, points toward back	*/
} SMsg;




#endif

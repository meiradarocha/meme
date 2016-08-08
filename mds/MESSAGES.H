/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme message code header file
 * by Marc de Groot
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* The message codes */
#define	MOTIONCONSTRAINTMSGCODE		(0)
#define	AVATARPOSITIONREQMSGCODE	(1)
#define	AVATARPOSITIONSENDMSGCODE	(2)
#define	CHANGEROOMMSGCODE			(3)
#define	TARGETROOMMSGCODE			(4)
#define	FILEREQUESTMSGCODE			(5)
#define	FILESENDMSGCODE				(6) 

typedef struct _FileRequestMsg {
	HOSTID	srcHostID;
	cell	srcModuleID;
	HOSTID	destHostID;
	cell	destModuleID;
	cell	len;
	cell	msgcode;
	cell	tag;
/*	char	filename[0]; 	Variable-length filename starts here */
} FileRequestMsg;

typedef struct _FileSendMsg {
	HOSTID	srcHostID;
	cell	srcModuleID;
	HOSTID	destHostID;
	cell	destModuleID;
	cell	len;
	cell	msgcode;
	cell	tag;
	cell	filelen;
/*	char	file[0]; 		Variable-length file starts here */
} FileSendMsg;

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Remote file request data structure
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* This struct is returned by OpenURL() and is passed to WaitURL(), both
 * declared in rfile.c.
 * status is rfrPENDING if the request is pending, rfrFAILURE if error,
 * and rfrSUCCESS if successful.
 *
 * tag is used internally to uniquely identify the remote file transaction.
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


#ifndef __RFILE_H__
# define __RFILE_H__

# define	RFREQUESTNAMELEN	(128)	/* File name lengths in struct */

typedef struct _URL {
	long	urltype;				/* Numeric flag telling what the URL was */
	char	filetype[32];
	char	hostname[256];
	char	pathname[256];
} URL;

/* Values for urltype */
# define	utINVALID		(-1)	/* Invalid URL specified */
# define	utRELFNAME		(0)		/* Plain relative-path filename */
# define	utABSFNAME		(1)		/* Plain absolute-path filename */
# define	utURL			(2)		/* Well-formed URL */


/* Request types */
# define	rfrqFILE	(0)
# define	rfrqBUFFER	(1)

typedef struct _RFRequest {
	cell	status;
	cell	tag;
	cell	type;
	char	*buff;
	char	tmpname[RFREQUESTNAMELEN];
	char	urlname[RFREQUESTNAMELEN];
} RFRequest;

/* Values for the RFRequest status field */
# define	rfrPENDING		(-1)
# define	rfrSUCCESS		(0)
# define	rfrFAILURE		(1)
	
#endif

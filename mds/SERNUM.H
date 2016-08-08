/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Serial number code header file.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
#ifndef	__SERNUM_H__
#define	__SERNUM_H__

#include "config.h"
#include "forth.h"

#define LEADER	{ 'Q', 'X', 'g', '\374', '\002', 'K', 'L', 'b', 't', 'm', '^', '\004', '\211', 'F', '}', '|' }

#define	LEADERLEN	(16)
#define	SERNUMLEN	(64)
#define	CRYPTSERLEN	(SERNUMLEN * 4)

/* Size of the serial number block plus the checksum.  See the struct below. */
#define	CRYPTSERANDSUMLEN	(CRYPTSERLEN + sizeof(cell))

#define	ADDSALT		(0x8b04372f)
#define	XORSALT		(0x92140c9b)

typedef struct _cfgblock {
   	char leader[LEADERLEN];		/* To locate the block in the exe */
/* NOTE  Do not separate these two fields */
	char serial[CRYPTSERLEN];	/* Encrypted serial number	*/
	cell checksum;				/* Serial number checksum	*/
/* NOTE */
	cell defaultNet;			/* Default Network type, from the enum */
	unsigned long dummyIP;		/* Dummy IP number for net=none. Stored in host byte order */
	cell defaultNoMouse;		/* Default mouse setting, = FALSE if mouse */
} ConfigBlock;

#endif

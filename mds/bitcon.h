/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Header file for bit concentrator library for Meme.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* First four bytes of a concentrated file contain this value, stored
 * with the most significant byte first.
 */
#define	BITCON_MAGIC	(0xC0F1C0DE)	/* Concentrator file code			*/

/* Return values from Concentrate() and Dilute() */
#define ERR_INITIO		(2)		/* Can't initialize buffer I/O				*/
#define	ERR_READ		(3)		/* Not enough input							*/
#define	ERR_WRITE		(4)		/* Can't malloc output buffer				*/
#define	ERR_BADJUJU		(5)		/* Bad magic number							*/
#define	ERR_BADTEXTSIZE	(6)		/* Compressed file has bad text size		*/


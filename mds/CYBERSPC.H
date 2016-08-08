/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Cyberspace routines for Meme - data structures
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */


#ifndef	__CYBERSPC_H__
#define __CYBERSPC_H__

#include "memenet.h"

#define	GLOBALREGIONDATABASE	"gregions"
#define	LOCALREGIONDATABASE		"regions"
#define REGIONDATABASE "foo"		/* FIX: Why is this here? */

/* The data structure for a rectangular region */
typedef struct _rprism {
	double xlow;
	double xhigh;
	double ylow;
	double yhigh;
	double zlow;
	double zhigh;
} RPrism;

/* The data structure for a region of cyberspace */
typedef struct _region {			/* global record 		local record	*/
	char	hostname[HOSTNAMELEN];	/* hostname				URL				*/
	char	reserved1[HOSTNAMELEN];	/* reserved				n/a				*/
	char	reserved2[HOSTNAMELEN];	/* reserved				n/a				*/
	char	reserved3[HOSTNAMELEN];	/* reserved				n/a				*/
	double	X;						/* n/a					X position		*/
	double	Y;						/* n/a					Y position		*/
	double	Z;						/* n/a					Z position		*/
	double	yaw;					/* n/a					yaw angle		*/
	double	pitch;					/* n/a					pitch angle		*/
	double	roll;					/* n/a					roll angle		*/
	double	xscale;					/* n/a					X scale factor	*/
	double	yscale;					/* n/a					Y scale factor	*/
	double	zscale;					/* n/a					Z scale factor	*/
	cell	count;					/* 		Prism count						*/
	cell	filler1;				/* (Because VC++ defaults to 8-byte packing) */
	RPrism	rprism[1];				/*		Prism 1, ...					*/
} Region;

/* Returns the size in bytes of a properly initialized region */
#define	REGIONSIZE(x)	( ( (x)->count - 1 ) * sizeof(RPrism) + sizeof(Region) )

#endif	/* __CYBERSPC_H__ */

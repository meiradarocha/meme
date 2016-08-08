/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* File I/O for Meme
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <stdio.h>
#include "config.h"
#include "forth.h"

/* If we don't do this, mfread and mfwrite will recurse infinitely */
#ifdef	fread
#undef	fread
#endif
#ifdef	fwrite
#undef	fwrite
#endif

size_t
mfread( void *p, size_t size, size_t n, FILE *fp )
{
#ifdef WINDOWS
	long	wantedsize = size * n;		/* Requested size to read */
	size_t	chunksize;			/* How much to read this fread */
	long	actualsize = 0;
	size_t	actual;
	
	
	/* Windows has some problem with reading more than 32767 characters per read */
	while (wantedsize) {
		chunksize = wantedsize > 32767 ? 32767 : wantedsize;
		actual = fread(p, 1, chunksize, fp);
		if (actual != chunksize) {
			return (actualsize + actual) / size;
		}
		actualsize += actual;
		wantedsize -= actual;
		p = (void *) ( (char *)p + actual );
	}
	return actualsize / size;
#else
	return fread(p, size, n, fp);
#endif
}

size_t
mfwrite( const void *p, size_t size, size_t n, FILE *fp )
{
#ifdef WINDOWS
	long	wantedsize = size * n;		/* Requested size to read */
	size_t	chunksize;			/* How much to read this fread */
	long	actualsize = 0;
	size_t	actual;
	
	
	/* Watcom has some problem with writing more than 32767 characters per read */
	while (wantedsize) {
		chunksize = wantedsize > 32767 ? 32767 : wantedsize;
		actual = fwrite(p, 1, chunksize, fp);
		if (actual != chunksize) {
			return (actualsize + actual) / size;
		}
		actualsize += actual;
		wantedsize -= actual;
		p = (void *) ( (char *)p + actual );
	}
	return actualsize / size;
#else
	return fwrite(p, size, n, fp);
#endif
}
	

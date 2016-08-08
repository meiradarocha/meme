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

/* malloca.c 1.2 90/03/11 */
/*
 * Getmem which cheats and statically allocates the dictionary array
 * in the bss segment.  This is good for Unix, which does not store
 * the BSS image in the program file, but bad for DOS, which does store
 * the BSS in the file, thus wasting tons of space.
 */

#include "config.h"


char *
getdictmem(unsigned cell size)
{
	static char dictionary[DICT_SIZE];

	size = size;	/* Quiet compiler warning */

    return( dictionary );
}

void
freedictmem(void)
{
}


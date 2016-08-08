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

/* mallocl.c 1.3 90/03/11 */
/*
 * Getmem which uses the C library malloc.
 *
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "forth.h"
#include "meme.h"
#include "protos.h"

static char *memedictmem = NULL;

char *
getdictmem(unsigned cell size)
{
	size = size;	/* Quiet compiler warning */

	if (memedictmem == NULL) {
		memedictmem = malloc((unsigned int)DICT_SIZE);
		if (memedictmem == NULL) {
			char msg[100];
			sprintf(msg, "getdictmem: Couldn't malloc %d bytes\n", DICT_SIZE);
			fatal(msg, MEMEMEMALLOCSTATUS);
		}
		return(memedictmem);
	} else {
		fatal("getdictmem: dictionary already allocated.\n", MEMEMEMALLOCSTATUS);
	}
}

void
freedictmem(void)
{
    if (memedictmem) {
		free(memedictmem);
		memedictmem = NULL;
	}
}

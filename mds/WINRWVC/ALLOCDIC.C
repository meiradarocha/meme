/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Dictionary allocation code.
 * Portions copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"
#include <malloc.h>
#include "forth.h"
#include "externs.h"
#include "protos.h"

static int dictallocated = 0;

static char *dictmalloc;		/* Rtn value from malloc() for the dictionary */
static char *relmapmalloc;		/* Rtn value from malloc() for the dict relmap */
static char *urelmapmalloc;		/* Rtn value from malloc() for the data relmap */

/* Allocate heap memory.  Return both the malloc return value, and a word-aligned
 * pointer for actual data storage.
 */
void
allocate_memory(mallocaddr, alignedaddr, size)
char **mallocaddr;
char **alignedaddr;
unsigned cell size;
{
	char *p;

	p = malloc( size + sizeof(cell) );
	if (p == NULL) {
		*mallocaddr = p;
		*alignedaddr = (char *)-1;
		return;
	}
	*mallocaddr = p;
	*alignedaddr = (char *) aligned( (u_char *)p );
}

/* Free heap memory.  Takes a value returned by malloc(). */
void
free_memory(char *mallocptr)
{
	free(mallocptr);
}


void
allocate_dictionary(needed)
unsigned cell needed;
{
	if (dictallocated) {
		fatal("allocate_dictionary: dictionary already allocated\n",
			MEMEDICTALLOCSTATUS);
	}

	/* Do the dictionary first */
	allocate_memory(&dictmalloc, (char **)&origin, needed);
    if ( origin == (cell *)-1 ) {
		fatal("Can't allocate memory for the dictionary\n", MEMEDICTALLOCSTATUS);
	}

    /* Fill the dictionary with zeroes so that the first cell will
     * be a zero.  Modules require a cell containing zero at their
     * origins, and the end of the link field list has been modified
     * to contain a 0 (rather than a pointer to the origin).
     */
    fill((u_char *)origin, needed, (u_char)0);

    /* Pointer alignment */
    dict_end = (cell *)((cell)origin + needed);

#ifdef RELOCATE

	/* Next, do the dict relmap */
    nrelbytes = (needed + 7) >> 3;
	allocate_memory(&relmapmalloc, &relmap, nrelbytes);
    if ( relmap == (u_char *)-1 ) {
		fatal("Can't allocate memory for the dictionary relocation map\n", MEMEDICTALLOCSTATUS);
    }
	fill(relmap, nrelbytes, (u_char)0);

	/* Next, do the user var relmap */
    nurelbytes = (MAXUSER + 7) >> 3;
	allocate_memory(&urelmapmalloc, &urelmap, nurelbytes);
    if ( urelmap == (u_char *)-1 ) {
		fatal("Can't allocate memory for the data relocation map\n", MEMEDICTALLOCSTATUS);
	}
	fill(urelmap, nurelbytes, (u_char)0);

#endif

	dictallocated = 1;
}

void
free_dictionary(void)
{
	if (!dictallocated) {
		fatal("free_dictionary: dictionary already freed\n", MEMEDICTALLOCSTATUS);
	}
	free_memory(dictmalloc);
	free_memory(relmapmalloc);
	free_memory(urelmapmalloc);

	dictallocated = 0;
}


/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

#include "config.h"
#include "forth.h"
#include <conio.h>
/*
 * Here is where you can add your own extensions.
 * Add entries to the ccalls table, and create Forth entry points
 * for them with ccall:
 */
#ifdef REMOVE
u_cell
pfetch(port)
cell port;
{
	return((u_cell)inpw((unsigned)port));
}

void
pstore(port,word)
cell port, word;
{
	(void)outpw((unsigned)port,(unsigned)word);
}

u_cell
pcfetch(port)
cell port;
{
	return ((u_cell)inp((unsigned)port));
}

void
pcstore(port,byte)
cell port, byte;
{
	(void)outp((unsigned)port,(int)byte);
}
#endif

#ifdef notdef
cell
example1(a,b)		/* Returns sum of a and b */
	cell a,b;
{
	return(a+b);
}

char *
example2(s)	/* Returns last 9 characters of string s in reverse order */
	char *s;
{
	register char *p;
	int i;
        static char result[10];

	p = &s[strlen(s)];
	for(i = 0; i < 9  &&  p > s; i++)
		result[i] = *--p;

	result[i] = '\0';

	return (result);
}
#endif

cell ((*ccalls[])()) = {
#ifdef notdef
	(cell (*)())example1,			/* Entry # 0 */
	(cell (*)())example2,			/* Entry # 1 */
	(cell (*)())pfetch,			/* Entry # 0 */
	(cell (*)())pstore,			/* Entry # 1 */
	(cell (*)())pcfetch,			/* Entry # 2 */
	(cell (*)())pcstore,			/* Entry # 3 */
#endif
	(cell (*) ())0
	/* Add your own routines here */
};

/*
 * Forth words to call the above routines may be created by:
 *
 *  system also
 *  0 ccall: sum      { i.a i.b -- i.sum }
 *  1 ccall: byterev  { s.in -- s.out }
 *
 * and could be used as follows:
 *
 *  5 6 sum .
 *  p" hello"  byterev  count type
 *
 */

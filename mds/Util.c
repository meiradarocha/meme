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

/* Utility routines
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
 
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <string.h>
#include <direct.h>
#include <time.h>
#include "forth.h"
#include "externs.h"


static	uint32	seed1 = 0x5324879f;
static	uint32	seed2 = 0xb78d0945;

uint32
myrand()
{
	uint32 tmp;
	tmp = seed1 + seed2;
	if (tmp < seed1 || tmp < seed2) tmp++;
	seed2 = seed1;
	seed1 = tmp;
	return tmp;
}

void
myrandomize()
{
	struct timeb t;
	ftime(&t);
	seed1 =(cell)(t.time + t.millitm);
}

/* Return non-zero if s names an absolute-path filename.
 * Return zero if s names a relative-path filename.
 * Names starting with "./" or "../" are, strictly speaking,
 * relative pathnames, but are tagged as absolute by this routine
 * so PathName() will not look in through the list of directories it's
 * given.
 */
int
abspathname(char *s)
{
	return (
			*s == '/' || 
			*s == '\\' || 
			s[1] == ':' || 
			(*s == '.' && (s[1] == '\\'  || s[1] == '/')) ||
			(*s == '.' && s[1] == '.' && (s[2] == '/' || s[2] == '\\'))
		);
}

cell
FileSize(char *name)
{
	long size;
	FILE *fp;

	if ( (fp = fopen(name, READ_MODE)) == NULL ) {
		return (cell) -1;
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	return (cell) size;
}

/* Generate a temporary filename with a path from MEMETMP. */
void
Memetmpname(char *p, cell tag)
{
	sprintf(p, "%s/%d.tmp", envMEMETMP, tag);
}


/* Search the path list pointed to by listp for the named file and generate
 * the full path name where it's found.  Set dest to the pathname.
 * Set dest to an empty string if file not found.
 */
void
PathName(char *file, char *dest, char *listp)
{
	char *cp;
	FILE *fp;

	/* If the filename is a fully-qualified pathname, try it by
	 * itself.
	 */
	if (abspathname(file)) {
		if (fp = fopen(file, READ_MODE)) {
			fclose(fp);
			/* RW can't handle filenames that start with ./ */
			if ( *file == '.' && (file[1] == '/' || file[1] == '\\') ) {
				getcwd(dest, 60);
				strcat(dest, &file[1]);
			} else {
				strcpy(dest, file);
			}		
			return;
		} else {
			*dest = '\0';
			return;
		}
	}
	cp = listp;
	for (;;) {
		if (!*cp) {
			*dest = '\0';
			return;
		}
#ifndef UNIX
		while (*cp && *cp != ';') cp++;
#else
		while (*cp && *cp != ':') cp++;
#endif
		memcpy(dest, listp, cp - listp);
		dest[cp - listp] = '\0';
		/* RW can't handle filenames that start with ./ */
		if ( *dest == '.' && dest[1] == '\0' ) {
			getcwd(dest, 60);
		}

		/* If the first character in the filename is not a slash, and the
		 * last character in the directory name is not a slash, append a slash
		 * to the directory name.
		 */
		if (*file != '/' && *file != '\\' &&
			dest[strlen(dest) - 1] != '/' && dest[strlen(dest) - 1] != '\\') {
			strcat(dest, "/");
			strcat(dest, file);

		/* Else, if the first character of the filename is a slash and the
		 * last character of the directory name is a slash, append the filename
		 * to the directory name starting at the second character of the filename.
		 */
		} else if ( (*file == '/' || *file == '\\') &&
			(dest[strlen(dest) - 1] == '/' || dest[strlen(dest) - 1] == '\\') ) {
			strcat(dest, &file[1]);

		/* Else, just append the filename to the directory name */
		} else {
			strcat(dest, file);
		}

		/* If the file is in the directory, we found it.  Return */
		if (fp = fopen(dest, READ_MODE)) {
			fclose(fp);
			return;
		}

		if (*cp) listp = ++cp;	/* Skip ';' */
	}
}

/* Search the MEMEFILES path list for the file. */
void
MemeFilePathName(char *file, char *dest)
{
	PathName(file, dest, envMEMEFILES);
}

void
safestrcpy(char *dest, char *src, int maxlen)
{
	if (strlen(src) < (unsigned int)maxlen) {
		strcpy(dest, src);
		return;
	}
	strncpy(dest, src, maxlen - 1);
	dest[maxlen - 1] = '\0';
}

static char yearRtnString[8];

char *
CurrentYear(void)
{
	struct tm *tp;
	time_t t;
	
	t = time(NULL);
	tp = localtime(&t);
	sprintf(yearRtnString, "%d", tp->tm_year + 1900);
	return yearRtnString;
}

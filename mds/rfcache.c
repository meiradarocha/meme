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

/* Remote file caching mechanism for Meme.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"

#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "forth.h"
#include "meme.h"
#include "messages.h"
#include "rfile.h"
#include "memenet.h"
#include "externs.h"
#include "protos.h"

/* Called from SetUpRFiles(). */
void
SetUpRFCache(void)
{
	FILE *fp;

	sprintf(cachefilename, "%s/cachemap", envMEMETMP);
	sprintf(cachetmpfilename, "%s/cachetmp", envMEMETMP);
	if (fp = fopen(cachefilename, WRITE_MODE)) {
		fclose(fp);
	} else {
		fatal("Couldn't open the cache map file for writing.", 1);
	}
}

/* Called from CleanUpRFiles(). */
void
CleanUpRFCache(void)
{

}

/* Adds an association between the two filenames to the cache map */
void
AddRFileToCache(char *urlname, char *tmpname)
{
	FILE *cachefp;
	FILE *tmpfp;
	char buff[256];

	if ( (cachefp = fopen(cachefilename, READ_MODE)) == NULL ) {
		fatal("Couldn't open cache file for reading", 1);
	}
	if ( (tmpfp = fopen(cachetmpfilename, WRITE_MODE)) == NULL ) {
		fatal("Couldn't open cache temp file for writing", 1);
	}

	/* Write the latest cache map entry to the temp file */
	sprintf(buff, "%s\t%s\n", urlname, tmpname);
	fputs(buff, tmpfp);
	if (ferror(tmpfp)) {
		fatal("Couldn't write the cache temp file", 1);
	}

	/* Copy the cache file to the cache temp file */
	for (;;) {
		fgets(buff, 256, cachefp);
		if (feof(cachefp)) break;
		if (ferror(cachefp)) {
			fatal("Couldn't read the cache file", 1);
		}
		fputs(buff, tmpfp);
		if (ferror(tmpfp)) {
			fatal("Couldn't write the cache temp file", 1);
		}
	}
	fclose(cachefp);
	fclose(tmpfp);

	/* Remove the cache file and rename the temp file to replace it */
	if (unlink(cachefilename) != 0) {
		fatal("Couldn't delete the old cache file", 1);
	}
	if (rename(cachetmpfilename, cachefilename) != 0) {
		fatal("Couldn't rename the cache temp file", 1);
	}
}

/* Search the cache map file for urlname.  If found, the temporary filename
 * tmpname contains the local cached filename.  If not found tmpname is empty.
 */
void
IsCached(char *urlname, char *tmpname)
{
	FILE *cachefp;
	char buff[256];				/* One line read from cache map file */
	char cacheurlname[256];		/* Name of cached file				*/
	char cachetmpname[256];		/* Name of counterpart in cache		*/
	char soughtname[256];		/* Name we're searching for in cache map */
	char *p;					/* For eliminating trailing blanks */

	/* Copy urlname to a local copy, and eliminate trailing blanks */
	safestrcpy(soughtname, urlname, 256);
	p = &soughtname[strlen(soughtname)];
	for (;;) {
		if (p == soughtname) break; /* Don't decrement past first char */

		--p;						/* Move left one char */

		/* If char is not whitespace, break */
		if (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') break;

		/* Change whitespace to null */
		*p = '\0';
	}

	/* Open the cache map file */
	if ( (cachefp = fopen(cachefilename, READ_MODE)) == NULL ) {
		fatal("Couldn't open cache file for reading", 1);
	}

	/* Read entries in the cache map until EOF or until urlname is found */
	for (;;) {
		fgets(buff, 256, cachefp);

		/* If EOF, set tmpname to indicate that urlname was not found, and return */
		if (feof(cachefp)) {
			*tmpname = '\0';
			fclose(cachefp);
			return;
		}

		/* Parse the input line into two separate pathnames */
		sscanf(buff, "%s\t%s\n", cacheurlname, cachetmpname);

		/* Compare the cache map urlname with the input arg urlname */
		if (!strcmp(cacheurlname, urlname)) {
			strcpy(tmpname, cachetmpname);
			fclose(cachefp);
			return;
		}
	}
}

	

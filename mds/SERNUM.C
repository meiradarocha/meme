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

/* Serial number code for Meme
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sernum.h"
#include "malloc.h"
#include "config.h"


uint32 myrand();
void myrandomize();

/* Encrypt the serial number in the plaintext string.  Put the
 * encrypted data in the buffer pointed to by cryptext.  The cryptext
 * buffer is CRYPTSERLEN bytes long.
 * Rtn value is 0 if successful, non-0 if not.
 */
int
EncryptSerNum(plaintext, cryptext, checksum)
char *plaintext, *cryptext;
cell *checksum;
{
	int i;
	char *pp = plaintext;
	char *cp = cryptext;
	cell *cellp;
	cell sum;

	/*  Check if the serial number is too long */
	if (strlen(plaintext) + 1 > SERNUMLEN) return -1;

	/* Fill the buffer with random numbers */
	myrandomize();
	for (i = 0; i < CRYPTSERLEN; i++) {
		cryptext[i] = (char)((myrand() & 0xff00) ^ 0x4700) >> 8;
	}

	/* Encrypt the plain text into the cryptext */
	for (;;) {
		cp[3] = *cp ^ cp[1] ^ cp[2] ^ *pp;
		if (*pp == '\0') break;
		pp++;
		cp += 4;
	}
	cellp = (cell *)cryptext;
	sum = 0;
	for (i = 0; i < SERNUMLEN; i++) {
		sum += cellp[i];
	}
	sum += ADDSALT;
	sum ^= XORSALT;
	*checksum = sum;
	return 0;
}

void
ModifyConfigBlock(file, cblock)
char *file;
ConfigBlock *cblock;
{
	char leader[LEADERLEN] = LEADER;
	long size;
	char *buff;
	char *buffp;
	FILE *f;
	int i;

	if ( (f = fopen(file, "r+b")) == NULL ) {
		printf("Can't open %s, exiting\n", file);
		exit(1);
	}
	fseek(f,0L,2); size = ftell(f); fseek(f, 0L, 0);
	buffp = buff = (char *)malloc(size);
	if (buff == NULL) {
		printf("Can't malloc enough space for %s, exiting.\n",
							file);
		exit(1);
	}
	fread(buff, (size_t)1, (size_t)size, f);
	if (ferror(f)) {
		printf("Can't read %s, exiting.\n", file);
		exit(1);
	}
	for (i = 0; i < size - LEADERLEN + 1; i++, buffp++) {
		if (!memcmp(buffp, leader, LEADERLEN)) {
			memcpy(buffp + LEADERLEN, cblock->serial, CRYPTSERANDSUMLEN);
			fseek(f, 0L, 0);
			fwrite(buff, (size_t)1, (size_t)size, f);
			if (ferror(f)) {
				printf("Error writing file, exiting\n");
				exit(1);
			}
			fclose(f);
			printf("Serialization successful.\n");
			exit(0);
		}
	}
	printf("Didn't find the config block leader, exiting.\n");
	exit(1);
}

void
main(argc, argv)
int argc;
char *argv[];
{
	char s[80];		/* Line from the user */
	ConfigBlock c;		/* The configuration block */
	char FileName[80];

	printf("Serialize v1.0\n");

#ifdef	UNIX
	strcpy(FileName, "meme");
#else
	strcpy(FileName, "meme.exe");
#endif

	argc--; argv++;
	if (argc) {
		if (!strcmp(*argv, "-f")) {
			if (argc == 1) {
				printf("No filename specified, exiting.\n");
				exit(1);
			}
			argc--; argv++;
			strcpy(FileName, *argv);
			argc--; argv++;
		}
	}
	*s = '\0';
	if (argc) {
		strcat(s, *argv); argc--; argv++;
		while(argc--) {
			strcat(s, " ");
			strcat(s, *argv++);
		}
		EncryptSerNum(s, &c.serial, &c.checksum);
	} else {
		printf("Serial number? ");
		fflush(stdout);
		gets(s);
		EncryptSerNum(s, &c.serial, &c.checksum);
	}
	ModifyConfigBlock(FileName, &c);
}

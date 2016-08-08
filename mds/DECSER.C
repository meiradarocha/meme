/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Serial number decryption routine for Meme.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.
 * All rights reserved.
 */
#include "sernum.h"

int
DecryptSernum(plaintext, cryptext, checksum)
char *plaintext;
char *cryptext;
cell *checksum;
{
	char *pp = plaintext;
	char *cp = cryptext;
	cell *cellp;
	unsigned cell sum;
	int i;

	/* Check the checksum.  If it's wrong, exit */
	cellp = (cell *)cryptext;
	sum = 0;
	for (i = 0; i < SERNUMLEN; i++) {
		sum += cellp[i];
	}
	if (sum != (*checksum ^ XORSALT) - ADDSALT) {
		return 1;
	}
	/* Decrypt the cryptext into the plaintext */
	for (;;) {
		*pp = *cp ^ cp[1] ^ cp[2] ^ cp[3];
		if (*pp == '\0') break;
		pp++;
		cp += 4;
	}
	return 0;

}

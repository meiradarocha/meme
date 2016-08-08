/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Character I/O for Meme.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <windows.h>
#include "winextrn.h"
#include <stdio.h>
#include "config.h"
#include "forth.h"
#include "externs.h"
#include "protos.h"


void linemode() {}
void keymode() {}
void restoremode() {}

int
key()
{
	keymode();
	return (int) DQ;
}

void
emit(c)
u_char c;
{
    if ( c == '\n' || c == '\r') {
        XV_NUM_OUT = 0;
		XV_NUM_LINE++;
    } else {
        XV_NUM_OUT++;
	}
	if (quietMode) return;
	if (!webMode) SendMessage(theEditWindow, WM_CHAR, (WPARAM)c, 0L);
}

int
key_avail()
{
	return KEYAVAIL;
}



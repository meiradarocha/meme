/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Provisional console output function for Meme.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
#include <windows.h>
#include <conio.h>
#include <dos.h>
#include <stdio.h>

#include "config.h"
#include "forth.h"
#include "winextrn.h"
#include "externs.h"

void
SetUpConsole(void)
{
}

void
EmitConsole(unsigned char c)
{
	if (quietMode) return;
	if (!webMode) SendMessage(theEditControl, WM_CHAR, (WPARAM)c, 0L);
}

void
CleanUpConsole(void)
{
}

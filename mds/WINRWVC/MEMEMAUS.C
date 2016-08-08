/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Interface routines for the mouse.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* These routines are the "standard connector" routines that plug
 * the platform-specific mouse routines into Meme.
 */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#include "renderer.h"
#if	RENDERING == RENDERWARE
# include "rwlib.h"
#endif

#include "config.h"
#include "forth.h"
#include "winextrn.h"

void
SetUpMouse(void)
{
}

void
CleanUpMouse(void)
{
}

cell
MemeMouseX()
{
#ifdef REMOVE
	POINT pt;
	RECT rt;
	GetCursorPos(&pt);
	GetClientRect(theGraphicsWindow, &rt);

		// NOT GetWindowRect!
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt); 
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt.right); 

	pt.x = (pt.x > rt.left) ? pt.x : rt.left;
	pt.x = (pt.x < rt.right) ? pt.x : rt.right;

	return (cell)pt.x;
#endif
	POINT pt;
	RECT rt;

	GetCursorPos(&pt);
	ScreenToClient(theGraphicsWindow, &pt);
	GetClientRect(theGraphicsWindow, &rt);
	pt.x = (pt.x > rt.left) ? pt.x : rt.left;
	pt.x = (pt.x < rt.right) ? pt.x : rt.right;
	return (cell)pt.x;
}

cell
MemeMouseY()
{
#ifdef REMOVE
	POINT pt;
	RECT rt;
	GetCursorPos(&pt);
	GetClientRect(theGraphicsWindow, &rt);

		// NOT GetWindowRect!
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt); 
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt.right); 

	pt.y = (pt.y > rt.top) ? pt.y : rt.top;
	pt.y = (pt.y < rt.bottom) ? pt.y : rt.bottom;
	
	return (cell)pt.y;
#endif
	POINT pt;
	RECT rt;

	GetCursorPos(&pt);
	ScreenToClient(theGraphicsWindow, &pt);
	GetClientRect(theGraphicsWindow, &rt);
	pt.y = (pt.y > rt.top) ? pt.y : rt.top;
	pt.y = (pt.y < rt.bottom) ? pt.y : rt.bottom;
	return (cell)pt.y;
}

cell
MemeMouseB()
{
	cell result = 0L;

	POINT pt;
	RECT rt;
	GetCursorPos(&pt);
	GetClientRect(theGraphicsWindow, &rt);

		// NOT GetWindowRect!
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt); 
	ClientToScreen(theGraphicsWindow,(LPPOINT)&rt.right); 

	if (!PtInRect(&rt, pt))
		return (cell)0; 
	
	
	if (GetKeyState(0x01) < 0) // high bit is on 
		result |= 0x01;

	if (GetKeyState(0x02) < 0) // high bit is on 
		result |= 0x02;

	if (GetKeyState(0x04) < 0) // high bit is on 
		result |= 0x04;
	
	return result;
}

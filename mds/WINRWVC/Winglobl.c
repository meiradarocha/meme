/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Global variables that are specific to the Windows platform
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <windows.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf		exitjmpbuff;
WPARAM		exitWParam;

// For the keyboard input routine--these vars are set in the EditWindowProc,
// and are read in nextchar().
// BOOL		keyReady; /* NOT USED NOW; REPLACED BY KEYSTROKE QUEUE */
// WPARAM		theKey;

#ifdef MEMEVIEW
char szAppName[] = "Meme(tm) Viewer for Windows";
#else
char szAppName[] = "Meme(tm) for Windows";
#endif

HWND hDlgModeless = NULL;

#ifndef _MSC_VER
/* These variables are supposed to be declared in Windows, but WATCOM can't find them... */
int		__argc;
char	**__argv;
#endif

//HWND 					theFrameWindow;
HWND					theEditWindow;
HWND					theGraphicsWindow;
HWND					theEditControl;
HWND					theAsyncWindow;
HANDLE					theInstanceHandle;
HANDLE					hAccel;

char					EditorClass[]= "MemeEditorClass";
char 					GraphicsClass[] = "MemeGraphicsClass";
char					AsyncClass[] = "MemeAsyncClass";

OSVERSIONINFO			winVersion;
int						win32s;	
DWORD					mainThreadId;		// main thread identifier
HANDLE					memeOneInstanceSem;
int						graphicsWindowIsIconified = 0;
int						webMode = 0;
BOOL					editWindowIsHidden;

char					GraphicsTitle[] = "Meme(tm) \xA9 Immersive Systems, Inc.";
char					GraphicsViewTitle[] = "Meme(tm) Viewer \xA9 Immersive Systems, Inc.";

/**************************************************************************/
/* Character queue for keystrokes */

WPARAM keystrokeq[256];
unsigned char nextkeyin = 0;
unsigned char nextkeyout = 0;

/**************************************************************************/
int isVoodoo = 0;		/* =TRUE if Voodoo chipset or other full-screen acceleration */

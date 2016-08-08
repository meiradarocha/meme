/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Extern declarations of global variables that are specific to the Windows platform
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */


#include <stdlib.h>
#include <setjmp.h>

extern	jmp_buf	exitjmpbuff;
extern	WPARAM	exitWParam;

// For the keyboard input routine--these vars are set in the EditWindowProc,
// and are read in nextchar(). /** NOT USED--SEE KEYSTROKE QUEUE BELOW */
// extern	BOOL		keyReady;
// extern	WPARAM		theKey;

extern	char szAppName[];
extern	HWND hDlgModeless;

extern	int	__argc;
extern	char **__argv;

// extern 	HWND 				theFrameWindow;
extern	HWND				theEditWindow;
extern	HWND				theGraphicsWindow;
extern	HWND				theEditControl;
extern	HWND				theAsyncWindow;
extern	HANDLE				theInstanceHandle;
extern	HANDLE				hAccel;
extern	char				EditorClass[];
extern	char 				GraphicsClass[];
extern	char				AsyncClass[];
extern OSVERSIONINFO		winVersion;	
extern	int					win32s;
extern	DWORD				mainThreadId;
extern	HANDLE				memeOneInstanceSem;
extern	int					graphicsWindowIsIconified;
extern	int					webMode;
extern	BOOL				editWindowIsHidden;

extern	char				GraphicsTitle[];
extern	char				GraphicsViewTitle[];

/**************************************************************************/
/* Character queue for keystrokes */

extern	WPARAM keystrokeq[];
extern	unsigned char nextkeyin;
extern	unsigned char nextkeyout;

#define	KEYAVAIL	(nextkeyin != nextkeyout)
#define NQ(x)		{ if ( (nextkeyin+1 & 0xff) != nextkeyout) { keystrokeq[nextkeyin++] = (x); } else { Beep(1400, 35); } }
#define	DQ			( KEYAVAIL ? keystrokeq[nextkeyout++] : 0 ) 

/**************************************************************************/
extern int isVoodoo;		/* =TRUE if Voodoo chipset or other full-screen acceleration */

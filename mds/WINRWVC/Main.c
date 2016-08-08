/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme entry point code
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"
#include <windows.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdio.h>
#include "forth.h"
#include "externs.h"
#include "winextrn.h"	/* External declarations of stuff in winglobl.c */
#include "resource.h"
#include "ascii.h"
#include "protos.h"
#include "psprotos.h"

#define	EDITCTRL_ID					102
#define SYSMENU_COMMANDVISIBLE 		0x1010
#define SYSMENU_COMMANDINVISIBLE 	0x1020

long FAR PASCAL GraphicsWndProc		(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL EditorWndProc		(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL AsyncWndProc		(HWND, UINT, WPARAM, LPARAM);

void								ShowView(HWND);
void								OnPaint(HWND);
void								OnSize(HWND, WPARAM, LPARAM);

static	HICON	hAppIcon;
static	HMENU	hEditSysMenu;
static	HMENU	hGraphicSysMenu;

/*
 *
 ***********************************************************************************
 * Window Procs
 ***********************************************************************************
 *
 */

////////////////////////////////
// Editor window proc
long FAR PASCAL EditorWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HFONT hFont;
	LRESULT charcount;
	extern BOOL pasting;

	switch (message) {
		case WM_CREATE:
			theEditControl = CreateWindow(
					"edit",				// window class
					NULL,				// no caption
					WS_CHILD | WS_VISIBLE |  WS_VSCROLL |
							ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
					0, 0, 0, 0,			// position and size
					hwnd, (HMENU)EDITCTRL_ID,
					((LPCREATESTRUCT) lParam)->hInstance, NULL);
			if (!theEditControl) {
				fatal("Couldn't create the edit control.", MEMEWINERRSTATUS);
			} else {
				if (win32s) {
					SendMessage (theEditControl, EM_LIMITTEXT, (WPARAM)0, (LPARAM)0L);	/* Limit is 64k bytes */
				}
			}
			hFont = CreateFont(	14,						// Logical height
								0,						// Logical width, 0 = don't care
								0,						// Angle of escapement
								0,						// Orientation
								FW_DONTCARE,			// Font weight
								0,						// Italic flag
								0,						// Underline flag
								0,						// Strikeout flag
								OEM_CHARSET,			// Character set
								OUT_DEFAULT_PRECIS,		// Output precision
								OUT_DEFAULT_PRECIS,		// Clipping precision
								DEFAULT_QUALITY,		// Output quality
								FIXED_PITCH | FF_DONTCARE | 0x4,	// Pitch and family
								NULL								// Font name
								);
			if (hFont) {
				SendMessage(theEditControl, WM_SETFONT, (WPARAM)hFont, (LPARAM)0L);
			}
			if (!webMode) {
				hEditSysMenu = GetSystemMenu(hwnd, FALSE);
				AppendMenu(hEditSysMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(hEditSysMenu, MF_STRING, SYSMENU_COMMANDINVISIBLE, "Hide Command Window");
			}
			return 0;

		case WM_SYSCOMMAND:	
			if ( (wParam & 0xfff0) == SYSMENU_COMMANDINVISIBLE) {
				ModifyMenu(	hEditSysMenu, SYSMENU_COMMANDINVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDVISIBLE, "Show Command Window");
				ModifyMenu(	hGraphicSysMenu, SYSMENU_COMMANDINVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDVISIBLE, "Show Command Window");
				ShowWindow(theEditWindow, SW_HIDE);
				editWindowIsHidden = TRUE;
			} else if ( (wParam & 0xfff0) == SYSMENU_COMMANDVISIBLE) {
				ModifyMenu(	hEditSysMenu, SYSMENU_COMMANDVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDINVISIBLE, "Hide Command Window"); 
				ModifyMenu(	hGraphicSysMenu, SYSMENU_COMMANDVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDINVISIBLE, "Hide Command Window"); 
				ShowWindow(theEditWindow, SW_SHOW);
				editWindowIsHidden = FALSE;
			}
			break;

		case WM_CHAR:
			if (wParam != CHAR_CTLC && wParam != CHAR_CTLH && !pasting) {
				charcount = SendMessage(theEditControl, WM_GETTEXTLENGTH, 0, 0);
				SendMessage(theEditControl, EM_SETSEL, charcount, charcount + 1);
			}
			SendMessage(theEditControl, WM_CHAR, wParam, lParam);
			return 0;

		case WM_SETFOCUS:
			SetFocus(theEditControl);
			return 0;

		case WM_SIZE:
			MoveWindow(theEditControl, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			return 0;

		case WM_COMMAND:
			if (wParam == EDITCTRL_ID && HIWORD(lParam) == EN_ERRSPACE || HIWORD(lParam) == EN_MAXTEXT) {
				MessageBox(NULL, "Edit control out of space.", szAppName,
						MB_OK | MB_ICONSTOP);
			}
			
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


//////////////////////////////////////
// Graphics window proc
long FAR PASCAL GraphicsWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_CREATE:
			if (!webMode) {
				hGraphicSysMenu = GetSystemMenu(hwnd, FALSE);
				AppendMenu(hGraphicSysMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(hGraphicSysMenu, MF_STRING, SYSMENU_COMMANDINVISIBLE, "Hide Command Window");
			}
			break;
#ifdef REMOVE
		case WM_COMMAND:
			// Messages from edit control
			if (LOWORD (lParam) && wParam == EDITCTRL_ID) {
				switch (HIWORD (lParam)) {
					case EN_ERRSPACE:
					case EN_MAXTEXT:
						MessageBox(hwnd, "Edit control out of space.", szAppName, MB_OK | MB_ICONSTOP);
						break;
				}
				break;
			}
			switch (wParam) {}
			return DefWindowProc(hwnd, message, wParam, lParam);
#endif
		case WM_SYSCOMMAND:	
			if ( (wParam & 0xfff0) == SYSMENU_COMMANDINVISIBLE && !webMode) {
				ModifyMenu(	hGraphicSysMenu, SYSMENU_COMMANDINVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDVISIBLE, "Show Command Window");
				ModifyMenu(	hEditSysMenu, SYSMENU_COMMANDINVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDVISIBLE, "Show Command Window");
				ShowWindow(theEditWindow, SW_HIDE);
				editWindowIsHidden = TRUE;
			} else if ( (wParam & 0xfff0) == SYSMENU_COMMANDVISIBLE && !webMode) {
				ModifyMenu(	hGraphicSysMenu, SYSMENU_COMMANDVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDINVISIBLE, "Hide Command Window"); 
				ModifyMenu(	hEditSysMenu, SYSMENU_COMMANDVISIBLE,
							MF_BYCOMMAND, SYSMENU_COMMANDINVISIBLE, "Hide Command Window"); 
				ShowWindow(theEditWindow, SW_SHOW);
				editWindowIsHidden = FALSE;
			} else if (wParam == SC_MINIMIZE) {
				SetClassLong( hwnd, GCL_HICON, (long)(currentCamera ? NULL : hAppIcon) );
			 	if (!editWindowIsHidden && !webMode) {
					ShowWindow(theEditWindow, SW_HIDE);
				}
			} else if ( (wParam == SC_RESTORE) && IsIconic(hwnd) && !editWindowIsHidden && !webMode) {
				ShowWindow(theEditWindow, SW_SHOW);
			}
			break;
		case WM_PAINT:
			if (currentCamera) {
				OnPaint(hwnd);
				return 0;
			}
			break;

		case WM_SIZE:
			if (currentCamera) {
				OnSize(hwnd, wParam, lParam);
				return 0;
			}
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
/*
		case WM_ACTIVATE:
			if (isVoodoo) {
				if (LOWORD(wParam) == WA_INACTIVE) {
					WindowsReleaseFocus();
				} else {
					WindowsGrabFocus(hwnd);
				}
			}
			break;

*/


/*** New ***/
        case WM_ACTIVATEAPP:
            if (isVoodoo) {
                if (wParam) {
                    WindowsGrabFocus(hwnd);
                    RwDeviceControl(rwSETPASSTHROUGHSTATE, FALSE, NULL, 0);
                } else  {
                    WindowsReleaseFocus();
                    RwDeviceControl(rwSETPASSTHROUGHSTATE, TRUE, NULL, 0);
                }
            }
            return 0L;
/*** New ***/

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//////////////////////////////////////
// Async window proc

long FAR PASCAL AsyncWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message >= WM_USER || message == WM_TIMER) {
		DispatchAsyncMessage(message, wParam, lParam);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 * FirstInstance - register window class for the application,
 *		   and do any other application initialization
 */
BOOL
FirstInstance( HANDLE inst )
{
    WNDCLASS	wc;
    BOOL		rc;

	if (!webMode) {
	    /* Set up and register the editor window class */
	    wc.style = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc = (LPVOID) EditorWndProc;
	    wc.cbClsExtra = 0;
	    wc.cbWndExtra = 0;
	    wc.hInstance = inst;
	    wc.hIcon = LoadIcon(inst, (LPCSTR)IDI_MEMEAPP2);
	    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	    wc.lpszMenuName = NULL;
	    wc.lpszClassName = EditorClass;
	    rc = RegisterClass(&wc);
	    if (!rc) return rc;
	}

    /* Set up and register the graphics window class */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) GraphicsWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon(inst, (LPCSTR)IDI_MEMEAPP2);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = GraphicsClass;
    rc = RegisterClass(&wc);
    if (!rc) return rc;
	
    /* Set up and register the async window class */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) AsyncWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon(inst, (LPCSTR)IDI_MEMEAPP2);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = AsyncClass;
    rc = RegisterClass(&wc);
    return rc;
}

/*
 * AnyInstance - do work required for every instance of the application:
 *		  create the window, initialize data
 */
BOOL
AnyInstance(HANDLE inst, int cmdshow)
{
	int x, y;
	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);

	if (!webMode) {
	    theEditWindow = CreateWindow(
	    	EditorClass,							/* class */
			"Meme(tm) Command Window",					/* caption */
			WS_VISIBLE | WS_OVERLAPPED | WS_THICKFRAME | WS_CAPTION |
			WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_SYSMENU,		/* style */
			x / 40,									/* init. x pos */
			y / 2,									/* init. y pos */
			x * 19 / 20,							/* init. x size */
			y * 3 / 10,								/* init. y size */
			NULL,									/* parent window */
			NULL,									/* use the class menu */
			inst,									/* program handle */
			NULL									/* create parms */
		);

	    if (!theEditWindow) {
			return(FALSE);
	    }
	} else {
		theEditWindow = 0;
	}
 
#ifdef MEMEVIEW
    theGraphicsWindow = CreateWindow(
    	GraphicsClass,				/* class */
		GraphicsViewTitle,
		WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | 
		WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_SYSMENU, 	/* style */
		x * 3 / 10,					/* init. x pos */
		y / 20,						/* init. y pos */
		x * 4 / 10,					/* init. x size  */
		y * 2 / 5,					/* init. y size */
		NULL,						/* parent window */
		NULL,						/* use the class menu */
		inst,						/* program handle */
		NULL						/* create parms */
	);
#else
    theGraphicsWindow = CreateWindow(
    	GraphicsClass,				/* class */
		GraphicsTitle,						/* caption */
		WS_VISIBLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | 
		WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_SYSMENU, 	/* style */
		x * 3 / 10,					/* init. x pos */
		y / 20,						/* init. y pos */
		x * 4 / 10,					/* init. x size  */
		y * 2 / 5,					/* init. y size */
		NULL,						/* parent window */
		NULL,						/* use the class menu */
		inst,						/* program handle */
		NULL						/* create parms */
	);
#endif

   /*
     * if this failed, then kill original windows
     */
    if(!theGraphicsWindow) {
		if (!webMode) DestroyWindow(theEditWindow);
		return(FALSE);
    }

    theAsyncWindow = CreateWindow(
    	AsyncClass,						/* class */
		NULL,							/* caption */
		0,							 	/* style */
		0,								/* init. x pos */
		0,								/* init. y pos */
		0,								/* init. x size  */
		0,								/* init. y size */
		NULL,							/* parent window */
		NULL,							/* i.d. */
		inst,							/* program handle */
		NULL							/* create parms */
	);

    /*
     * if this failed, then kill original windows
     */
    if(!theAsyncWindow) {
		if (!webMode) DestroyWindow(theEditWindow);
		DestroyWindow(theGraphicsWindow);
		return(FALSE);
    }

	/*
	 * Make the windows visible
	 */
	if (webMode) {
//		SetWindowPos(theGraphicsWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(theGraphicsWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(theGraphicsWindow, cmdshow);
		UpdateWindow(theGraphicsWindow);
	} else {
		editWindowIsHidden = FALSE;
		SetActiveWindow(theEditWindow);
		ShowWindow(theEditWindow, cmdshow);
		UpdateWindow(theEditWindow);
		ShowWindow(theGraphicsWindow, cmdshow);
		UpdateWindow(theGraphicsWindow);
	}

	hAccel = LoadAccelerators(inst, szAppName);
    return(TRUE);
			
} /* AnyInstance */

/*
 * WinMain - Application entry point
 *
 */
int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{

	/* Very first thing -- turn on the debug logging if necessary */
	if ( __argc > 1 && !stricmp(__argv[1], "debuglogging") ) {
		__argv++;
		__argc--;
		DebugLogging = 1;

L("Turned on debug logging in WinMain()")

	}

	/* Copy the first string in argv[] to argv0 */
	strcpy(argv0, __argv[0]);


L("Calling LoadIcon");

	hAppIcon = LoadIcon(hInstance, (LPCSTR)IDI_MEMEAPP2);

L("LoadIcon returned, calling setjmp")

	if (!setjmp(exitjmpbuff)) {

L("Between setjmp and WindowsSetUpMeme")

		// If successful, WindowsSetUpMeme() never returns
		if (WindowsSetUpMeme(&__argc, &__argv,
						hInstance, hPrevInstance, lpszCmdLine, nCmdShow) == 0) {

L("WindowsSetUpMeme returned to WinMain")

			return 0;
		}
	}

L("Returned from setjmp in WinMain, entering CleanUpMeme")

	CleanUpMeme();

L("Returned from CleanUpMeme. Exiting");

	return exitWParam;								// Set by the message loop
}

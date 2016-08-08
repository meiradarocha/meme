/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Platform-specific function prototypes
 * by Marc de Groot
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

 /* This file contains prototypes for platform-specific functions.  Functions
  * present on all platforms should have their prototypes in protos.h.
  */

#ifndef		_PSPROTOS_H_
# define	_PSPROTOS_H_

/* init.c */
int				WindowsSetUpMeme(int *argcp, char *(*argvp[]), 
					HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);

/* main.c */
long FAR PASCAL EditorWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
long FAR PASCAL GraphicsWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
long FAR PASCAL AsyncWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL FirstInstance( HANDLE inst );
BOOL AnyInstance(HANDLE inst, int cmdshow);
int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);


/* memenet.c? */
void DispatchAsyncMessage(UINT message, WPARAM wParam, LPARAM lParam);

/* memerend.c */
void	WindowsGrabFocus(HWND hwnd);
void	WindowsReleaseFocus(void);

#endif

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* MemeOften() - Called from PAUSE to execute things that
 * need to happen often.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/*
 * MemeOften() calls routines that, in turn, call the networking API.  The networking code
 * returns extended error codes in XV_ERRNO--a user variable.  Since MemeOften() runs during
 * context switches, it doesn't have a legitimate set of user variables available.  To
 * accommodate this need, the array "anti_task" was created.  On entry, the xup is saved
 * (though in most cases this is unnecesary) and xup is set to anti_task.  When the networking
 * memes try and store extended error information in XV_ERRNO, they will not write into the
 * user area of an unsuspecting task.
 */

#include <windows.h>
#include "config.h"
#include "externs.h"
#include "winextrn.h"
#include "mstimer.h"
#include "protos.h"

void
MemeOften(void)
{
	MSG msg;
	long *saved_xup = xup;	/* Save the value of xup. */

	/* Make sure that user variables set by the between-tasks code have a
	 * legitimate place to be written.
	 */
	xup = (long *)anti_task;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_CHAR) {
			NQ(msg.wParam);	/* Set the key's keycode */
		} else if (msg.message == WM_QUIT) {
			exitWParam = msg.wParam;
			longjmp(exitjmpbuff, 1);
		} else if (hDlgModeless == NULL || !IsDialogMessage(hDlgModeless, &msg)) {
			if (!TranslateAccelerator(msg.hwnd, hAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} 	
	}

	/* Send Meme messages */
	if (!NetFail) NetOften();

	/* Run the URL server */
	RFileOften(); 

	/* RESTORE XUP before returning */
	xup = saved_xup;
}

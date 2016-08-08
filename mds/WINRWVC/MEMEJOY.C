/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Interface routines for the joystick.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* These routines are the "standard connector" routines that plug
 * the platform-specific joystick routines into Meme.
 */

#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "config.h"
#include "forth.h"

JOYINFO joy;

cell
MemeJoystickX()
{
	return 0;
}

cell
MemeJoystickY()
{
	return 0;
}

cell
MemeJoystickB()
{
	return 0;
}

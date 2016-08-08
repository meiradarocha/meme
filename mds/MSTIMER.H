/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Millisecond timer header file.
 * by Marc de Groot
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */



#ifndef	__MSTIMER_H__
#define	__MSTIMER_H__



typedef struct _mstimer {
	unsigned long	sec;
	long			ms;
} mstimer;

void			normalizetimer(mstimer *t);
void			zerotimer(mstimer *t);
void			settimer(mstimer *t, unsigned long msdelay);
unsigned long	timedout(mstimer *t);

#endif

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Millisecond timer routines.
 * by Marc de Groot
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <sys/timeb.h>
#include "mstimer.h"


/* Normalize, i. e. make sure ms is between 0 and 999 inclusive */
void
normalizetimer(mstimer *t)
{
	if (t->ms < 0) {
		t->sec += (t->ms - 999) / 1000;
		t->ms = 1000 + (t->ms % 1000);
		return;
	}
	if (t->ms > 999) {
		t->sec += t->ms / 1000;
		t->ms = t->ms % 1000;
	}
}

/* Zero the timer. */
void
zerotimer(mstimer *t)
{
	t->sec = (unsigned long)t->ms = 0;
}

/* Set the timer t to the current time plus msdelay. */
void
settimer(mstimer *t, unsigned long msdelay)
{
	struct timeb tb;

	ftime(&tb);
	t->sec = tb.time + (msdelay / 1000);
	t->ms = tb.millitm + (msdelay % 1000);
	normalizetimer(t);
}

/* Returns 0 if not timed out, or the # of milliseconds after
 * the true timeout moment that this routine is called.  If
 * the timer t is zeroed, timedout will always return 0.
 */
unsigned long
timedout(mstimer *t)
{
	mstimer now;

	/* If the timer is zeroed, always return "false". */
	if ( (t->sec | (unsigned long)t->ms) == 0) return 0;

	/* Get current time in "now" */
	settimer(&now, 0);

	/* If curr seconds < timeout seconds, we haven't timed out yet. */
	if (now.sec < t->sec) return 0;

	/* If curr secs == timeout secs AND curr millisecs <= timeout millisecs,
	 * we haven't timed out yet.
	 */
	if (now.sec == t->sec && now.ms <= t->ms) return 0;

	/* Otherwise, we have timed out.  Return # of milliseconds past the real timeout
	 * time we are.
	 */
	return 1000 * (now.sec - t->sec) + (now.ms - t->ms);
}

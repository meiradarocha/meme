/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* check_expiry() checks whether the program should exit because the expiration
 * date has passed.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"

#include <windows.h>
#include "forth.h"
#include "externs.h"
#include "protos.h"

/* Define MEMEEVAL for evaluation copies of the development system.
 * Make sure the SECOND version of the expiration date, below, is correctly set.
 * Note that MEMEEVAL is ignored if MEMEVIEW is defined.
 */
// #define MEMEEVAL


/* Adjust the year value because some brain-dead puppy decided to only
 * return the last two digits of the year from localtime().
 * Since the epoch was January 1st, 1970, any year less than 70 must
 * be in the 21st century, so add 100, which will make the inequality
 * comparisons work--at least until 2070!
 */
#define	AYEAR	( (currentYear < 70) ? currentYear + 100 : currentYear )

#ifdef MEMEVIEW

/* The expiration date--NOTE: First day program doesn't work is day AFTER what's specified here */
/* Exp. date is 01Apr00 */
#define	EDAY	31		/* Day of month (1 = the 1st) */
#define	EMONTH	2		/* Month (0 = January!) */
#define	EYEAR	100		/* Last two digits of year.  Add 100 if the year 2000 or later */


void
expired(void)
{

MessageBox(NULL,
"This version of the Meme(tm) viewer has expired.  For\n\
information on the availability of this viewer and other\n\
Meme(tm) products, contact Immersive Systems, Inc.\n\
\n\
         WWW: http://www.immersive.com\n\
         E-mail: meme-info@immersive.com\n\
         Telephone: +1 415 641 VR4U",
		 "Software has expired!", MB_OK);

	exit(0);
}

void
check_expiry(void)
{
	if (EYEAR < AYEAR) expired();
	if (EYEAR == AYEAR && EMONTH < currentMonth) expired();
	if (EYEAR == AYEAR && EMONTH == currentMonth && EDAY < currentDay) expired();
}

void
cprint_evalcopymessage(void)
{
	cprint("\n");
}

#endif

// **************************************************************************

#if (!defined(MEMEVIEW) && defined(MEMEEVAL))

/* The expiration date--NOTE: First day program doesn't work is day AFTER what's specified here */
/* Exp. date is 25Sep96 */
#define	EDAY	24		/* Day of month (1 = the 1st) */
#define	EMONTH	8		/* Month (0 = January!) */
#define	EYEAR	96		/* Last two digits of year.  Add 100 if the year 2000 or later */

void
expired(void)
{

	MessageBox(NULL,

"This evaluation copy of the Meme(tm) Development System has expired.\n\
For information on the Meme(tm) Development System and other products,\n\
contact Immersive Systems, Inc.\n\
\n\
                            WWW: http://www.immersive.com\n\
                            E-mail: meme-info@immersive.com\n\
                            Telephone: +1 415 641 VR4U",
	
	"Software has expired!", MB_OK);

	exit(0);
}

void
check_expiry(void)
{
	if (EYEAR < AYEAR) expired();
	if (EYEAR == AYEAR && EMONTH < currentMonth) expired();
	if (EYEAR == AYEAR && EMONTH == currentMonth && EDAY < currentDay) expired();
}

void
cprint_evalcopymessage(void)
{
	char buf[100];

	sprintf(buf, "*** FOR EVALUATION PURPOSES ONLY.  GOOD UNTIL %d/%d/%2d. ***\n",
													EMONTH + 1, EDAY, EYEAR);
	cprint(buf);
}

#endif

// **************************************************************************

#if (!defined(MEMEVIEW) && !defined(MEMEEVAL))

void
check_expiry(void)
{
}

void
cprint_evalcopymessage(void)
{
}

#endif

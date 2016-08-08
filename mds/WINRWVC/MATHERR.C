/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

#include "config.h"

#include <stdio.h>
#include <math.h>
#include "forth.h"
#include "meme.h"
#include "protos.h"

static char *Msgs[] = {
        0,
        "Domain error",
        "Argument singularity",
        "Overflow range error",
        "Underflow range error",
        "Total loss of significance",
        "Partial loss of significance"
   };

int
matherr( struct _exception *excp )
{
	char s[256];
	sprintf(s, "%s in %s\n", Msgs[excp->type], excp->name);
	cprint(s);
	return 1;
}

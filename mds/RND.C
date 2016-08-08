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

/* Random number generator for Primordial soup.
 * by Marc de Groot.
 * Copyright (c) Marc de Groot.  All rights reserved.
 */

#include "soup.h"
#include "protos.h"

unsigned long		rnd_n1 = 0x43259871;
unsigned long		rnd_n2 = 0x1b5c903a;
short			carryflag;

/*
 * Set the seed for the random number generator
 */
void
rndseed(seed)
unsigned long seed;
{
	rnd_n2 = seed;
}

/*
 * Return a random number in the interval [0..limit).
 */
unsigned long
rnd(limit)
unsigned long limit;
{
#ifndef REMOVE
	return scale(awcrnd(), limit);
#else
	return scale(jrand48(), limit);
#endif
}

/*
 * Add-with-carry random number generator
 */
unsigned long
awcrnd()
{
	short		newcarryflag;
	unsigned long	lastn1;

	/* Set newcarryflag if rnd_n1 + rnd_n2 results in a carry.
	 * n1 = n1 + n2 + carry, n2 = old n1
	 */
	lastn1 = rnd_n1;
	rnd_n1 = addc(rnd_n1, rnd_n2, &newcarryflag);
	if (carryflag) {
		rnd_n1++;
	}
	rnd_n2 = lastn1;

	/* Set carry flag for next time */
	carryflag = newcarryflag;

	return rnd_n1;
}

unsigned long
addc(n1, n2, carry)
unsigned long n1;
unsigned long n2;
short *carry;
{
	unsigned long sum;

	sum = n1 + n2;
	*carry = sum < n1 || sum < n2;
	return sum;
}

/*
 * Unsigned rational multiply.  Multiply n1 by n2/(2^32).
 * Multiply n1 by n2 retaining a 64-bit result, then return the
 * high 32 bits.
 *
 * Use the following diagram to look at the multiply:
 *  - Each letter represents a hex digit.
 *				aaaabbbb
 *			  x ccccdddd
 *			 -----------
 *				eeeeffff
 *			gggghhhh
 *			iiiijjjj
 *		kkkkllll
 *	    ----------------
 *  	mmmmnnnnoooopppp
 *
 */
unsigned long
scale(n1, n2)
unsigned long n1;
unsigned long n2;
{
	unsigned long partial1;		/* Partial products for multiply */
	unsigned long partial2;
	unsigned long partial3;
	unsigned long partial4;

	unsigned long loprod;		/* The 64-bit product	*/
	unsigned long hiprod;

	short carry;			/* Carry flag returned by addc() */

	/* bbbb x dddd = eeeeffff */
	partial1 = (unsigned long) (n1 & 0xffffL) *
										(unsigned long ) (n2 & 0xffffL);

	/* aaaa x dddd = gggghhhh */
	partial2 = (unsigned long) (n1 >> 16) *
										(unsigned long) (n2 & 0xffffL);

	/* cccc x bbbb = iiiijjjj */
	partial3 = (unsigned long) (n1 & 0xffffL) *
										(unsigned long) (n2 >> 16);

	/* cccc x aaaa = kkkkllll */
	partial4 = (unsigned long) (n1 >> 16) * (unsigned long) (n2 >> 16);

	hiprod = 0;

	/* Do the add of eeeeffff + hhhh0000 */
	loprod = addc(partial1, partial2 << 16, &carry);

	/* If carry from eeeeffff + hhhh0000, increment hi part	*/
	if (carry) hiprod++;

	/* Do the add of (eeeeffff + hhhh00000) + jjjj00000 */
	loprod = addc(loprod, partial3 << 16, &carry);

	/* If carry from (eeeeffff + hhhh0000) + jjjj0000, inc hi part */
	if (carry) hiprod++;

	/* Do gggg + iiii + kkkkllll */
	hiprod += (partial2 >> 16) + (partial3 >> 16) + partial4;

	/* At this point, the 64-bit result of n1 * n2 is in {hiprod, loprod} */

	/* Return the result / 2^32 */
	return hiprod;
}

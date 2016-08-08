/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Error codes returned in the errno user variable, and by GetLastError.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */


/*************** CHANGE ERRORS.MH IF YOU CHANGE THIS FILE ******************/


/*
 * The network error codes are also in memetcp.c for the DOS platform,
 * and in errors.mh (the Meme-level code).
 * Change the codes there if you change them here.
 */

/* The 'evil' anti-error message */
#define	ENOERROR			(0)			/* Indicate success, or no error	*/

/*
 * Network error codes
 */
#define	NETCONNECTREFUSED	(-2)
#define	NETCLOSEDEARLY		(-3)
#define	NETTIMEDOUT			(-4)
#define	NETUNRESOLVEDHOST	(-5)
#define	NETUNRESOLVEDARP	(-6)
#define	NETUNINITIALIZED	(-7)
#define	NETNOSOCKSPACE		(-8)
#define	NETINVALIDSOCK		(-9)

/*
 * Serial comm error codes
 */
#define	SERBADOPEN			(-100)		/* Port failed to open				*/
#define	SERBADPARAMS		(-101)		/* Bad port config params			*/
#define	SERBADCLOSE			(-102)		/* SerialClose failed				*/
#define	SERCANTSETEVENTS	(-103)		/* SetCommMask failed				*/
#define	SERNOSPACE			(-104)		/* Failure to allocate data structs	*/
#define	SERREADFAILED		(-105)		/* Failure on read of serial port	*/
#define	SERWRITEFAILED		(-106)		/* Failure on read of serial port	*/

/*
 * RegisterDeckModule error codes
 */
#define	EDECKALREADYREGISTERED	(-200)	/* DeckModule is non-zero			*/
#define	EDECKINVALIDMODULE		(-201)	/* Invalid pointer to module		*/

/*
 * Adopts error codes
 */
#define	EADOPTSCHILDISWORLD		(-300)	/* Attempt to adopt the world obj	*/
#define	EADOPTSSELF				(-301)	/* Attempt to adopt self			*/

/*
 * Orphan error codes
 */
#define	EORPHANNULLPARENT		(-400)	/* Non-null backPtr, null parent	 */
#define	EORPHANWORLD			(-401)	/* Tried to orphan the world		*/
#define	EORPHANBADPTRS			(-402)	/* null backPtr&bad CheckFamilyPtrs	*/

 /*
 * CheckFamilyPtrs error codes
 */
#define	EFAMPTRSBADCHILDPTR		(-500)	/* Child ptr invalid */
#define	EFAMPTRSCHILDSBACKPTR	(-501)	/* Child's backptr invalid */
#define	EFAMPTRSBADSIBLINGPTR	(-502)	/* Sibling ptr invalid */
#define	EFAMPTRSSIBLINGSBACKPTR	(-503)	/* Sibling's backptr invalid */
#define EFAMPTRSBACKPTR			(-504)	/* Object's backptr invalid */
#define	EFAMPTRSNULLPARENT		(-505)	/* Backptr is non-null but parent is null */
#define	EFAMPTRSBADPARENT		(-506)	/* Invalid parent ptr */
#define	EFAMPTRSNONNULLPARENT	(-507)	/* Backptr is null but parent is non-null */
#define	EFAMPTRSNONNULLSIBLING	(-508)	/* Backptr is null but sibling is non-null */
#define	EFAMPTRSBADOBJECT		(-509)	/* Object pointer doesn't point to an object */
#define EFAMPTRSUNUSED			(-510)	/* Object ptr points to an unused object */
#define	EFAMPTRSCHILDUNUSED		(-511)	/* Object's child is unused */
#define	EFAMPTRSSIBLINGUNUSED	(-512)	/* Object's sibling is unused */
#define	EFAMPTRSBACKUNUSED		(-513)	/* Object's backPtr points to unused object */
#define	EFAMPTRSPARENTUNUSED	(-514)	/* Object's parent is unused */

/*
 * SendMessage error codes
 */
#define	ESENDMSGTIMEDOUT		(-600)	/* Timed out before delivery */

/*
 * Error codes from GetSubtreeMatrix and Transform
 */
#define	EGETSUBTMTREETOODEEP	(-700)	/* Lineage array not long enough to hold tree */
#define	ETRANSFORMINVERTFAILED	(-701)	/* Matrix inversion failed in Transform(). */
#define	ETRANSFORMCOMPOSEFAILED	(-702)	/* Composition of matrices failed in Transform(). */
#define	ETRANSFORMVECTORFAILED	(-703)	/* RwTransformVector in Transform() failed */
#define	ETRANSFORMPOINTFAILED	(-704)	/* RwTransformPoint in Transform() failed */
#define	ETRANSFORMBNOPARENT		(-705)	/* Second object has no parent */

/*
 * Error codes from NewTexture and NewMaskedTexture
 */
#define	ETEXINVALIDTEXFILE		(-800)	/* Texture file not found or invalid */
#define	ETEXINVALIDMASKFILE		(-801)	/* Mask file not found or invalid */

/*
 * Error codes from SetCameraBackdrop
 */
#define	ESCBINVALIDFILE			(-900)	/* File not found or invalid */
#define	ESCBNOSPACE				(-901)	/* malloc failed */
#define	ESCBBADOBJECT			(-902)	/* Either unused or not camera obj */

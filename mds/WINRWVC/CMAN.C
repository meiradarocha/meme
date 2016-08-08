/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/*----------------------------------------------------------------------*/
/*									*/
/* tcyber.c - Cyberman example code					*/
/*									*/
/*	Prints to the screen the current status of the Logitech 	*/
/*	Cyberman 6D controller, including button presses.		*/
/*	Columns give translatio, then rotation: X Y Z P Y R		*/
/*	(xtrans ytrans ztrans pitch yaw roll).				*/
/*									*/
/*----------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <dos.h>

/* Swift data struct */
/*----------------------*/
typedef struct {
	unsigned char device,		/* Really only need device here */
		major_version,
		minor_version,
		xtrans, ytrans, ztrans,
		xrot, yrot, zrot;
} cyberman_swift;


/* Actual cyberman data struct */
/*-----------------------------*/
typedef struct {
	short xtrans, ytrans, ztrans,	/* Translation		*/
		xrot, yrot, zrot,	/* Rotation		*/
		buttons;		/* Buttons		*/
} cyberman_data;

/* 
 * Globals 
 */

static int firsttime = -1;

/* Paragraph # of the low-memory data buffer */
static unsigned short lowmemseg;

cyberman_data *cm;	/* Ptr to data structure */

/* Ptr or NULL, this is returned by Cyberman(). */
static cyberman_data *cmanaddr; 

/* Allocated here to make sure unused fields are 0 */
/* intr() must have all unused seg regs set to 0   */
static union REGPACK regpack; 

unsigned char mouse_there()
{
	union REGS r;
	if (_dos_getvect(0x33)!=0L) {	/* If Int 33 installed-		*/
		r.w.ax=0;		/* Mouse function 0		*/
		int386(0x33,&r,&r);	/* Reset Mouse			*/
		if (r.w.ax!=0) return(1);	/* ax!=0, mouse there	*/
	}
	return(0);
}

void get_swift(cyberman_swift *cym)
{
	union REGS r;
	r.w.ax=0x53C1;			/* Extended Cyberman		*/
	int386(0x33,&r,&r);		/* device status call		*/
	cym->device=r.w.ax;		/* Device type in ax		*/
}

unsigned char cyberman_there()
{
	get_swift((cyberman_swift *)cm); /* Get swift data		*/
	if (((cyberman_swift *)cm)->device==1) return(1); /* If =1,  there */
	return(0);			/* else return 0		*/
}

void
get_cyberman(unsigned short segment)
{
	regpack.w.ax=0x5301;		/* Get Cyberman datacall	*/
	regpack.w.es=segment;		/* Set seg/off pointers to	*/
	regpack.w.dx=0;			/* data structure		*/
	intr(0x33,&regpack);		/* Make the call		*/
}

void
SetUpCyberMan()
{
}

void
CleanUpCyberMan()
{
    /* Free the allocated memory */
    _dos_freemem(lowmemseg);
}

cyberman_data *
CyberMan()
{
	if (firsttime) {
		firsttime = 0;
		cmanaddr = NULL;
		if (mouse_there()) {
			if (cyberman_there()){
				/* Allocate 4 paragraphs */
				_dos_allocmem(4, &lowmemseg);
				cm = (cyberman_data *)(lowmemseg * 16);
				cm->xtrans = cm->ytrans = cm->ztrans =
				cm->xrot = cm->yrot = cm->zrot =
				cm->buttons = 0;
				
				cmanaddr = cm;
			}
		}
	}
	if (cmanaddr) {
		get_cyberman(lowmemseg);
	}
	return cmanaddr;
}


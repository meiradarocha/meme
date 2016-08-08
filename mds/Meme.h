/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme object structure.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#ifndef	__MEME_H__
#define	__MEME_H__
/* Rendering engine-specific data structure */
#include "forth.h"
#include "memerend.h"

/* Types of objects */
#define	cameraObject			(0)
#define	lightObject				(1)
#define	visibleObject			(2)
#define	audibleObject			(3)
#define	imaginaryObject			(4)
#define	compoundObject			(5)
#define spriteObject			(6)

/* Subtypes of objects */
#define	directionalLight		(0)
#define	conicalLight			(1)
#define	pointLight				(2)
#define	parallelPointLight		(3)
#define	ambientLight			(4)

#define	plainVisible			(5)
#define	containerVisible		(6)

#define	monoCamera				(1 << 8)
#define	stereoCamera			(2 << 8)
#define stereoInterlacedLRCamera	(stereoCamera + 1)
#define stereoInterlacedRLCamera	(stereoCamera + 2)
#define stereoAdjacentLRCamera		(stereoCamera + 3)
#define stereoAdjacentRLCamera		(stereoCamera + 4)
#define	textureCamera			(3 << 8)

/* Texture map wrap types */
#define	noWrap					(0)
#define	sphericalWrap			(1)
#define	cylindricalWrap			(2)
#define	cubicalWrap				(3)
#define	flatWrap				(4)

/* Shading types */
#define	flatShadedVisible		(0)
#define	smoothShadedVisible		(1)
#define phongShadedVisible		(2)

/* Geometry types */
#define pointCloudVisible		(0<<8)
#define	wireFrameVisible		(1<<8)
#define solidVisible			(2<<8)

/* Types of links */
#define	lockedLink				(0)
#define	swivelLink				(1)

/* This is the size of the object array  */
#define	MAXOBJECTS				(1024)	/* Change maxobjects in memeutil.m when
										 * you change this.
										 */
#define	MAXMODULES				(256)	/* Size of the module array	*/

/* Local host's IP number */
#define	LOCALHOST				((HOSTID)0x7f000001)

/* Port numbers */
#define	MEMEPORT				(8765)		/* Also defined in memenet.c */
#define	SPACETOHOSTPORT			(8766)

#define	DECKMODULEID			((module *)0)		/* Deck Module ID number */
#define	MAINTASKID				((module *)-1)		/* Main task ID number	*/
#define	FILESERVERID			((module *)-2)		/* File server ID number */


/* Convert from radians to degrees */
#define		DEGREES(x)			((x) * 180.0 / 3.14159265358979323846)

/* Here's what a Meme object looks like */
typedef struct _Object {
	union			_EngineSpecificPtr ptr;	/* renderer-specific data	*/
	EngineSpecificFrame frame;				/* renderer-specific ref frame */
	double			x;						/* X position				*/
	double			y;						/* Y position				*/
	double			z;						/* Z position				*/
	double			yaw;					/* yaw angle in degrees		*/
	double			pitch;					/* pitch angle in degrees	*/
	double			roll;					/* roll angle in degrees	*/
	double			xscale;					/* X scale factor			*/
	double			yscale;					/* Y scale factor			*/
	double			zscale;					/* Z scale factor			*/
	struct _Object	*child;					/* ptr to first child		*/
	struct _Object	*sibling;				/* ptr to next sibling		*/
	struct _Object	**backPtr;				/* ptr to parent or sibling	*/
	struct _Object	*parent;				/* ptr to parent object		*/
	cell			objectType;
	cell			objectSubtype;
	cell			linkType;
	cell			used;					/* This object allocated	*/
} Object;


/* This is the magic number that goes in the ModuleMagic field. 
 * IF YOU CHANGE THIS VALUE HERE ALSO CHANGE IT IN MSTRUCTS.MH.
 */
#define		MODULEMAGICNUMBER	(0xC0DEBA5E)


/* This struct is used for relocatable modules.
 * When modifying this struct, also be sure to modify the struct
 * in module.m
 */
struct _module {
	cell	ModuleMagic;				/* Module magic # 0xC0DEBA5E */
	void	*LinkFieldPtrs[NTHREADS];	/* Like a vocabulary */
	void	*VocLinkPtr;				/* Like a vocabulary */
	cell	ModuleMaxFileSize;			/* Physical size of buffer */
	cell	ModuleDP;					/* Top of module's dict */
	void	*FirstTaskPtr;				/* Ptr to 1st task */
	void	*LastTaskLinkPtr;			/* Ptr to last task's link */
	cell	RelTableOffset;				/* Offs from struct adr to rel tbl */
	cell	ModuleLoaded;				/* Is-module-loaded flag */
	cell	CFAOfGo;					/* CFA of the "go" meme */
	cell	CFAOfReceiver;				/* CFA of the "receiver" meme*/
	cell	CompilationOrigin;			/* Address originally compiled at */
	cell	CompilationLibOrigin;		/* Address of std lib when originally compiled */
	cell	ModuleCreator;				/* Module that created this module */
	cell	ModuleTagPointer;			/* Pointer to module's ID string */
	cell	ModuleAuxPointer;			/* Pointer to module's aux data */
	char	ModuleDict[];				/* Start of dictionary */
};
typedef struct _module module;

typedef struct	_CModule {
	module		*ptr;
	cell		used;
} CModule;

/****************************************************************************
 * Stuff for messaging.
 */

/* The message transmitted by SendMessage().  There is a corresponding struct
 * in modcode/messages.mh.  If you change these, change MSGHEADERLEN below.
 */
typedef struct	_message {
	HOSTID		srcHostID;		/* Source host ID					*/
	module		*srcModuleID;	/* Source module ID					*/
	HOSTID		destHostID;		/* Destination host ID				*/
	module		*destModuleID;	/* Destination module ID			*/
	cell		len;			/* Total byte count in the message	*/
	char		body[];			/* The body of the message			*/
} Message;

/* Length of the message header, in bytes */
#define	MSGHEADERLEN	(20)	/* source hostID + source moduleID +
								 * dest host ID + dest module ID + 
								 * msg length
								 */

/* Length of the incoming data buffer for DispatchIncomingMessages() */
#define	DISPATCHBUFFLEN		(2000)

/****************************************************************************
 * Stuff for the debugger
 */

/* The global data structure for the anomaloscope */
struct AnomData {
	cell		_IP;
	cell		_starting_rp;
	cell		_debug_in_progress;
	cell		_non_colon_def[2];
	cell		_prp;
	cell		_private_rtn_stk[1024];
	cell		_slop[10];
};

/* A pointer to this record is returned by MemePickObject() */
/* If you change this here, change it in memeutil.m as well! */

/* I had to change this struct, moving the three doubles from */
/* the end of the struct to the start, because of VC++'s 8-byte */
/* padding of structs.  4 bytes of padding were being left between */
/* vindex and x.  When making structs be very wary of this! */
typedef struct _MemePickRecord {
	double		vx;		/* Nearest vertex's coords in object space */
	double		vy;
	double		vz;
	double		wx;		/* The spot exactly under the pixel in world coords */
	double		wy;
	double		wz;
	void		*clump;
	void		*polygon;
	cell		vindex;
} MemePickRecord;

/* Defines relating to the current camera struct */
#define	currentCamera	CurCam.cam
#define	curCamtype		CurCam.subtype

#endif

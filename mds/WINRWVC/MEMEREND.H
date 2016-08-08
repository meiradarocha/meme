/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Rendering engine-specific part of the Meme object structure.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#ifndef __MEMEREND_H__
#define __MEMEREND_H__

#include "rwlib.h"

/* This is the pointer to the rendering engine-specific data structure
 * for a Meme object.  This union is used by struct _Object.
 *
 * This version is for the RenderWare rendering engine.
 */
union _EngineSpecificPtr {
	RwClump		*clump;
	RwCamera	*camera;
	RwLight		*light;
	void		*module;
	void		*voidptr;
};

typedef RwMatrix4d * EngineSpecificFrame;

typedef struct	_MemeColorStruct {
	double r;
	double g;
	double b;
	double opacity;
} MemeColorStruct;

#endif

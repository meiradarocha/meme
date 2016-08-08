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

/* Meme materials code.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "config.h"
#include "forth.h"
#include "meme.h"
#include "mememat.h"
#include "protos.h"

/****************************************************************************
 * The following structs are only used by routines in this file.
 */

static mMat *mMaterials[MAXMATERIALS];

/****************************************************************************
 * Setup and cleanup routines
 */
void
SetUpMaterials(void)
{
	int i;
	for (i = 0; i < MAXMATERIALS; i++) {
		mMaterials[i] = NULL;
	}
}

void
CleanUpMaterials(void)
{
	int i;
	for (i = 0; i < MAXMATERIALS; i++) {
		if (mMaterials[i]) {
			free(mMaterials[i]);
			mMaterials[i] = NULL;
		}
	}
}

void *
NewMat(void)
{
	int i;
	mMat **p = mMaterials;
	mMat *mp;

	for (i = 0; i < MAXMATERIALS; i++) {
		if (*p == NULL) break;
		p++;
	}
	if (i == MAXMATERIALS) {
		error("Materials array is full.");
		return NULL;
	}
	mp = (mMat *)malloc(sizeof(mMat));
	if (mp == NULL) {
		error("Can't allocate a material.");
		return NULL;
	}

	mp->r = mp->g = mp->b = -1.0;
	mp->opacity = -1.0;
	mp->Ka = -1.0;
	mp->Kd = -1.0;
	mp->Ks = -1.0;
	mp->power = -1.0;
	mp->geometry = -1;
	mp->shading = -1;
	mp->wrap = -1;
	mp->tex = (RwTexture *)-1;
	*p = mp;
	return mp;
}

void
DestroyMat(void *mp)
{
	int i;
	mMat **ap = mMaterials;

	for (i = 0; i < MAXMATERIALS; i++) {
		if (*ap == (mMat *)mp) break;
		ap++;
	}
	if (i == MAXMATERIALS) {
		error("Tried to free a bogus material pointer.  ");
		return;
	}
#ifdef REMOVE
	if ( ((mMat *)mp)->tex ) {
		MemeDestroyTexture( ((mMat *)mp)->tex );
	}
#endif
	free(mp);
	*ap = NULL;
}

int
ValidMat(mMat *mp)
{
	int i;
	mMat **ap = mMaterials;

	for (i = 0; i < MAXMATERIALS; i++) {
		if (*ap == mp) break;
		ap++;
	}
	return i != MAXMATERIALS;
}

void
MatColor(void *p, double r, double g, double b)
{
	if (!ValidMat((mMat *)p)) {
		error("MatColor: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->r = r;
	((mMat *)p)->g = g;
	((mMat *)p)->b = b;
}

void
MatOpacity(void *p, double opacity)
{
	if (!ValidMat((mMat *)p)) {
		error("MatOpacity: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->opacity = (opacity > 0.9999) ? 1.0 : opacity;
}

void
MatSurface(void *p, double Ka, double Kd, double Ks, double power)
{
	if (!ValidMat((mMat *)p)) {
		error("MatSurface: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->Ka = Ka;
	((mMat *)p)->Kd = Kd;
	((mMat *)p)->Ks = Ks;
	((mMat *)p)->power = power;
}

void
MatGeometry(void *p, cell g)
{
	if (!ValidMat((mMat *)p)) {
		error("MatGeometry: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->geometry = g;
}

void
MatShading(void *p, cell s)
{
	if (!ValidMat((mMat *)p)) {
		error("MatShading: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->shading = s;
}

void
MatTexture(void *p, void *tex)
{
	if (!ValidMat((mMat *)p)) {
		error("MatTexture: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->tex = tex;
}

void
MatWrap(void *p, cell wrap)
{
	if (!ValidMat((mMat *)p)) {
		error("MatWrap: Bogus material pointer.  ");
		return;
	}
	((mMat *)p)->wrap = wrap;
}

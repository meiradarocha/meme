/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme materials header file.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#ifndef	_MEMEMAT_H_
# define	_MEMEMAT_H_

# define MAXMATERIALS	(256)	/* # of elems in materials array		*/

/* The data structure for a Meme material */
typedef struct _mMat {
	double	r;			/* Red coefficient */
	double	g;			/* Green coefficient */
	double	b;			/* Blue coefficient */
	double	opacity;	/* Opacity coefficient */
	double	Ka;			/* Ambient lighting coefficient */
	double	Kd;			/* Diffuse lighting coefficient */
	double	Ks;			/* Specular lighting coefficient */
	double	power;		/* Exponent for specular coefficient */
	cell	geometry;	/* Geometry type */
	cell	shading;	/* Shading type */
	cell	wrap;		/* Texture map wrap type */
	void*	tex;		/* Pointer to texture, NULL if no texture */
} mMat;

#endif

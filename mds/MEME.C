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

/* Routines that deal with Meme objects.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* Note on the object hierarchy:
 *   Objects have parents, siblings, and children in the hierarchy.  There
 * are three pointers that represent the object's position in the hierarchy:
 * the child pointer, the sibling pointer, and the back pointer.  If an
 * object has children, the object's child pointer points to the first child.
 * The first child's sibling pointer points to the second child,  the second
 * child's sibling pointer points to the third child, and so on.
 *   If an object is linked into the hierarchy, it is pointed to by either
 * a child pointer or a sibling pointer of another object.  The back pointer
 * is a backward-pointing pointer that points at the child or sibling field
 * that is, in turn, pointing at the object containing the back pointer.
 *   The back pointer is used by Orphan().  It points either at the child
 * field of the object's parent, or the sibling field of its sibling.  It always
 * contains the address of the pointer that points at its object.  Since
 * Orphan() must patch the either the child field of the object's parent, or the
 * sibling field of the object's sibling, the back pointer points at the
 * appropriate field to patch.
 */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#ifndef unix
#include <dos.h>
#endif
#include <math.h>
#include <string.h>

#include "config.h"
#include "forth.h"
#include "prims.h"
#include "meme.h"
#include "externs.h"
#include "protos.h"
#include "errors.h"

/* Initialize object fields */
void
InitializeObject(Object *obj, cell type, cell subtype, cell linktype)
{
	obj->xscale = obj->yscale = obj->zscale = 1.0;
	obj->yaw = obj->pitch = obj->roll = 0.0;
	obj->x = obj->y = obj->z = 0.0;
	obj->child = obj->sibling = (Object *) NULL;
	obj->backPtr = (Object **) NULL;
	obj->parent = (Object *) NULL;
	obj->objectType = type;
	obj->objectSubtype = subtype;
	obj->linkType = linktype;
	obj->used = -1;
}

/* Mark all objects as unused.  Alloc and init the world object.  This is
 * called once by SetUpRendering().
 */
void
InitializeObjects(void)
{
	int i;

	/* Mark all the objects as unused. */
	for (i = 0; i < MAXOBJECTS; i++) {
		Objects[i].used = 0;
	}

	/* World object is an imaginary object.  Its subtype is zero cuz
	 * imaginary objects have no subtype.  Its link is not used.  Its
	 * parent is NULL cuz its the root of the tree.
	 */
	InitializeObject(
		&Objects[0],		/* Pointer to the object	*/
		imaginaryObject,	/* Type						*/
		(cell) 0,			/* Subtype					*/
		lockedLink);		/* Link						*/
}

/* Return a pointer to an unused Meme object.  Return NULL if none found. */
Object *
FirstFreeObject(void)
{
	int	i;

	for (i = 0; i < MAXOBJECTS; i++) {
		if (Objects[i].used == 0) {
			return &Objects[i];
		}
	}
	return (Object *) NULL;
}

/* Return the number of Meme objects */
cell
NumberObjects(void)
{
	int i;
	cell used = 0;		/* Count of used Meme objects */

	for (i = 0; i < MAXOBJECTS; i++) {
		if ( Objects[i].used ) {
			used++;
		}
	}
	return used;
}

#ifdef REMOVE
Object *
ParentOf(Object *obj)
{
	Object *parent;
	Object *child;
	int i;

	/* For every object */
	for (i = 0, parent = Objects; i < MAXOBJECTS; i++, parent++) {
		/* If this object is in use and has a child,
		 * check if its first child or any of its first child's
		 * siblings are equal to the calling argument.  If so,
		 * this object is the parent.
	 	 */
		if (parent->used && parent->child) {
			child = parent->child;
			while (child) {
				if (child == obj) return parent;
				child = child->sibling;
			}
		}
	}
	return (Object *)NULL;
}
#endif

cell
CheckObjectPtr(Object *o)
{
	if ( (unsigned long) o  < (unsigned long) &Objects[0] ) return -1;
	if ( (unsigned long) o  > (unsigned long) &Objects[MAXOBJECTS - 1] ) return -1;
	if ( ( (unsigned long) o - (unsigned long) &Objects[0] ) % sizeof(Object) ) return -1;
	return 0;
}

/* CheckFamilyPtrs() return 0 for success, non-zero for failure */
/* Note that the code first checks for an invalid obj ptr, THEN for unused obj */
cell
CheckFamilyPtrs(Object *o)
{
	unsigned long tryobj1, tryobj2;
	if (CheckObjectPtr(o)) return EFAMPTRSBADOBJECT;
	if (o->used == 0) return EFAMPTRSUNUSED;
	if (o->child) {		/* If obj has child */
		if ( CheckObjectPtr(o->child) ) return EFAMPTRSBADCHILDPTR;	/* If child ptr invalid */
		if ( (o->child)->used == 0 ) return EFAMPTRSCHILDUNUSED;	/* Child obj unallocated */
		if ( (o->child)->backPtr != &o->child ) return EFAMPTRSCHILDSBACKPTR;
	}
	if (o->sibling) {
		if ( CheckObjectPtr(o->sibling) ) return EFAMPTRSBADSIBLINGPTR;
		if ( (o->sibling)->used == 0 ) return EFAMPTRSSIBLINGUNUSED;
		if ( (o->sibling)->backPtr != &o->sibling ) return EFAMPTRSSIBLINGSBACKPTR;
	}
	if (o->backPtr) { 
		tryobj1 = (unsigned long) o->backPtr - ( (unsigned long) &o->sibling - (unsigned long) o );
		tryobj2 = (unsigned long) o->backPtr - ( (unsigned long) &o->child - (unsigned long) o );
		if (CheckObjectPtr((Object *)tryobj1) == 0) {
			if (((Object *)tryobj1)->used == 0) return EFAMPTRSBACKUNUSED;
		} else if (CheckObjectPtr((Object *)tryobj2) == 0) {
			if (((Object *)tryobj2)->used == 0) return EFAMPTRSBACKUNUSED;
		} else {
			 return EFAMPTRSBACKPTR;
		}
		if (o->parent == NULL) return EFAMPTRSNULLPARENT;
		if (CheckObjectPtr(o->parent)) return EFAMPTRSBADPARENT;
		if ( (o->parent)->used == 0 ) return EFAMPTRSPARENTUNUSED;
	} else {
		if (o->parent) return EFAMPTRSNONNULLPARENT;
		if (o->sibling) return EFAMPTRSNONNULLSIBLING;
	}
	return 0;
}

/* Orphan() removes a Meme object from the hierarchy
 * of articulation.
 */
cell
Orphan(Object *o)
{
	Object *parent;
	Object *child;

	/* If the backPtr is NULL, this is either the world
	 * object, or an orphaned object, so don't try to orphan it.
	 */
	if (o->backPtr != (Object **) NULL) {

		/* Find my parent */
		if (! (parent = o->parent) ) {
			return EORPHANNULLPARENT;	/* Non-NULL backPtr, yet NULL parent */
		}

		/* Preserve a pointer to my child */
		child = o->child;
		o->child = (Object *)NULL;

		/* Patch either the child pointer of my parent, or the
		 * sibling pointer of my previous sibling, to point to
		 * my next sibling (which may be NULL).
		 */
		*(o->backPtr) = o->sibling;

		/* If my next sibling exists, patch her back pointer
		 * to point where mine points.  Patch my sibling pointer
		 * to point to nothing.
		 */
		if (o->sibling) {
			(o->sibling)->backPtr = o->backPtr;
			o->sibling = (Object *)NULL;
		}
		/* Patch my back pointer to point to nowhere, since
		 * I now have no parent or sibling.
		 */
		o->backPtr = (Object **) NULL;

		/* Patch my parent pointer to point to nowhere, since
		 * I have no parent.
		 */
		o->parent = NULL;

		/* If I have a child, make my parent adopt my child */
		//if (child) {
		//	child->parent = NULL;
		//	Adopts(parent, child);
		//}
		o->child = child;	/* Restore the children */
		return 0;
	} else {

		/* We got here if the backPtr is NULL */
		if (o == &Objects[0]) return EORPHANWORLD;	/* Tried to orphan the world */
		if (CheckFamilyPtrs(o)) return EORPHANBADPTRS;
		return 0;
	}
}

/* Adopts() causes one Meme object to become the child of another.
 * The first object becomes the parent of the second.
 */

cell
Adopts(Object *parent, Object *child)
{
	Object *oldsibling;
	Object *theSibling;
	cell rtn;

	if (rtn = CheckFamilyPtrs(parent)) return rtn;
	if (rtn = CheckFamilyPtrs(child)) return rtn;

	/* Don't let the world be adopted by another object */
	if (child == &Objects[0]) {
		return EADOPTSCHILDISWORLD;
	}

	/* Don't let an object adopt itself */
	if (child == parent) {
		return EADOPTSSELF;
	}

	if (child->parent) {
		(void)Orphan(child);	/* Don't check error rtn, only error
								 * returned by Orphan is null parent,
								 * which we just checked here.
								 */
	}

	/* Make parent's most recent child my sibling */
	if (parent->child) {
		oldsibling = child->sibling;
		child->sibling = parent->child;
		/* If I had a sibling before, walk the sibling tree
		 * till I find the last sibling I have now.  Patch that
		 * sibling's sibling field to point at my old sibling.
		 * Patch my old sibling's back pointer to point at that
		 * last sibling.
		 */
		if (oldsibling) {
			theSibling = child->sibling;
			while (theSibling->sibling) {
				theSibling = theSibling->sibling;
			}
			theSibling->sibling = oldsibling;
			oldsibling->backPtr = &theSibling->sibling;
		}
	}
	/* If I have a sibling, make its backPtr point at me */
	if (child->sibling) {
		(child->sibling)->backPtr = &(child->sibling);
	}

	/* Make parent's most recent child pointer point at me */
	parent->child = child;

	/* Make my backPtr point at parent */
	child->backPtr = &(parent->child);

	/* Set my parent pointer */
	child->parent = parent;

	return 0;	/* no error */
}

/* Allocate and initialize a new object, linking it to the parent
 * object specified.  Returns a pointer to the new object.
 */
Object *
NewObject(Object *parentobj, cell type, cell subtype, cell linktype)
{
	Object *obj;

	if (CheckObjectPtr(parentobj) || parentobj->used == 0) return NULL;
	if ( (obj = FirstFreeObject()) == NULL ) return NULL;

	/* Initialize various fields */
	InitializeObject(obj, type, subtype, linktype);

	/* Get parent object's first child, make it our sibling,
	 * then make ourself the first child.
	 */
	Adopts(parentobj, obj);
	if (type == cameraObject) NumberCameras++;
	return obj;
}

void
DestroyObject(Object *obj)
{
	if (obj == &Objects[0]) {	/* If the world object				*/
		return;					/* Don't destroy it					*/
	}
	if (obj->used == 0) return;
	while (obj->child) Adopts(obj->parent, obj->child);
	if (obj->objectType == cameraObject) NumberCameras--;
	if (obj->ptr.camera == currentCamera) currentCamera = NULL;
	EngineSpecificDestroyObject(obj); /* Deallocate renderer data	*/
	Orphan(obj);				/* Unlink from the hierarchy		*/
	obj->used = 0;				/* Mark object as unused			*/
}
/*****************************************************************************/
/* The following code is for modules. */
CModule *
FirstFreeModule(void)
{
	int i;
	CModule *mp = Modules;

	for (i = 0; i < MAXMODULES; i++, mp++) {
		if (mp->used == 0) return mp;
	}
	return NULL;
}

CModule *
WhichModule(module *mp)
{
	int i;
	CModule *cmp = Modules;

	/* If the module ID is zero, return the deck module address */
	if (!mp) {
		mp = (module *)DeckModule;

		/* If deck hasn't been registered, return 0 */
		if (!DeckModule) return NULL;
	}

	/* Find the CModule address corresponding to the module address */
	for (i = 0; i < MAXMODULES; i++, cmp++) {
		if ( (mp == cmp->ptr) && cmp->used) return cmp;
	}
	return NULL;
}

void *
NewModule(size)
cell size;
{
	CModule *mp;

	if ( (mp = FirstFreeModule()) == NULL ) return NULL;
	if ( (mp->ptr = (void *)malloc(size)) == NULL ) return NULL;
	memset((void *)mp->ptr, (int) 0, (size_t)size);
	mp->used = 1;
	return mp->ptr;
}

void
DestroyModule(void *p)
{
	CModule *cmp;
	if ( (cmp = WhichModule( (module *)p )) && cmp->used) {
		free((void *)cmp->ptr);
		cmp->used = 0;
		cmp->ptr = NULL;
	}
}

/*****************************************************************************/
/* The following two routines convert from interoperable float to float,
 * and vice-versa.
 */
void
Float2IFloat(f, exp, man)
double	f;
cell *exp, *man;
{
	double fexp, fman, fpow;
	if (f == 0.0) {
		*exp = *man = 0;
		return;
	}
	fexp = ceil(log10(fabs(f)) + 1.0);
	*exp = (cell)fexp;
	fpow = pow(10.0, fexp);
	fman = f / fpow;
	fman = fman * TWO_31ST;
	*man = (cell)fman;
}

void
IFloat2Float(exp, man, f)
cell exp, man;
double *f;
{
	double fpow;

	if (man == 0) {
		*f = 0.0;
		return;
	}
	fpow = pow(10.0, (double)exp);
	*f = (double)man / TWO_31ST;
	*f = *f * fpow;
}

Object *
MemePtrToObj(void* p)
{
	int i;
	Object *op;
	
	for (i = 0, op = Objects; i < MAXOBJECTS; i++, op++ ) {
		if (p == op->ptr.voidptr) return op;
	}
	return NULL;
}

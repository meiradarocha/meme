/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Cyberspace routines for Meme
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/timeb.h>
#include "config.h"
#include "forth.h"
#include "cyberspc.h"
#include "externs.h"
#include "protos.h"

/* Primitives in this file:
 * void DestroyRegion(Region *r)
 * cell PointInRegion(Region *r, double x, double y, double z)
 * cell PointInPrism(RPrism *rp, double x, double y, double z)
 * cell OverlappingPrisms(RPrism *rp1, RPrism *rp2)
 * cell OverlappingRegions(Region *r1, Region *r2)
 * Region *AddToRegion(Region *regionp, RPrism *prismp)
 * Region *NextDBRegion(void)
 * Region * FirstDBRegion(int gflag)
 * Region * AddDBRegion(Region *rp, int gflag)
 * Region * DeleteDBRegion(Region *rp, int gflag)
 * Region * RegisterDBRegion(Region *rp)
 * Region * UnregisterDBRegion(Region *rp)
 * Region * EnclosingDBRegion(double x, double y, double z)
 * Region * FirstOverlappingDBRegion(Region *rp)
 * Region * NextOverlappingDBRegion(Region *rp)
 */

/* The value stored in XV_REGIONVAR by the Meme programmer to indicate
 * a request of the global database
 */
#define GLOBFLAG	(-1)

/* Returns true if we should use the global database */
#define ISGLOB		(XV_REGIONVAR == GLOBFLAG)

/* Erase a string field in a region record */
#define ERASESTRFLD(x) \
{ \
	int __i = HOSTNAMELEN; \
 	char *__p = (x); \
 	while (__i--) *__p = '\0'; \
}

/* A pointer to this struct is stored in XV_REGIONVAR.  It is used
 * to hold the state of a region DB request.
 */
typedef struct _dbstate {
	FILE *fp;
	Region *rp;
} DBState;

#define	DBFP	( ((DBState *)XV_REGIONVAR)->fp )
#define	DBRP	( ((DBState *)XV_REGIONVAR)->rp )

/*******************************************************************************
 * The following are the primitive routines for cyberspace region management.
 */

void
DestroyRegion(Region *r)
{ 
	(void)free(r);
}

/* PointInRegion() - Return a boolean if a point is enclosed by a region */
cell
PointInRegion(Region *r, double x, double y, double z)
{
	int		i;
	RPrism	*rp;
        
	rp = &r->rprism[0];
	for (i = 0; i < r->count; i++, rp++) {
		if (rp->xlow <= x && rp->xhigh > x && rp->ylow <= y && rp->yhigh > y 
										&& rp->zlow <= z && rp->zhigh > z) {
			return (cell)-1;
		}
	}
	return (cell)0;
}

/* PointInPrism() - Return a boolean if a point is enclosed by a prism */
cell
PointInPrism(RPrism *rp, double x, double y, double z)
{
	if (rp->xlow <= x && rp->xhigh > x && rp->ylow <= y && rp->yhigh > y 
										&& rp->zlow <= z && rp->zhigh > z) {
		return (cell)-1;
	}
	return (cell)0;
}

/* OverlappingPrisms() - Return boolean indicating whether the intersection of
 * two prisms is non-null
 */
cell
OverlappingPrisms(RPrism *rp1, RPrism *rp2)
{
	if (PointInPrism(rp1, rp2->xlow, rp2->ylow, rp2->zlow)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xlow, rp2->ylow, rp2->zhigh)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xlow, rp2->yhigh, rp2->zlow)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xlow, rp2->yhigh, rp2->zhigh)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xhigh, rp2->ylow, rp2->zlow)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xhigh, rp2->ylow, rp2->zhigh)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xhigh, rp2->yhigh, rp2->zlow)) return (cell)-1;
	if (PointInPrism(rp1, rp2->xhigh, rp2->yhigh, rp2->zhigh)) return (cell)-1;

	if (PointInPrism(rp2, rp1->xlow, rp1->ylow, rp1->zlow)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xlow, rp1->ylow, rp1->zhigh)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xlow, rp1->yhigh, rp1->zlow)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xlow, rp1->yhigh, rp1->zhigh)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xhigh, rp1->ylow, rp1->zlow)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xhigh, rp1->ylow, rp1->zhigh)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xhigh, rp1->yhigh, rp1->zlow)) return (cell)-1;
	if (PointInPrism(rp2, rp1->xhigh, rp1->yhigh, rp1->zhigh)) return (cell)-1;

	return (cell)0;
}

/* OverlappingRegions()- Return boolean indicating whether the intersection of
 * two regions is non-null.
 */
cell
OverlappingRegions(Region *r1, Region *r2)
{
	int     i, j;
	RPrism *rp1, *rp2;
	
	for (i = 0, rp1 = &r1->rprism[0]; i < r1->count; i++, rp1++) {
		for (j = 0, rp2 = &r2->rprism[0]; i < r2->count; i++, rp2++) {
			if (OverlappingPrisms(rp1, rp2)) return (cell)-1;
		}
	}
	return (cell)0;
}

/* AddToRegion() - Add a rectangular prism to a region.
 * On entry:    regionp	Pointer to a region.  NULL if new region to be created.
 *				prismp	Pointer to a prism to be added to the region.
 * Returns:		regionp	If successful.
 * On error:    NULL
 */
Region *
AddToRegion(Region *regionp, RPrism *prismp)
{
	Region *rp;

	/* If allocating a new region */
	if (regionp == (Region *)NULL) {
		rp = (Region *)malloc(sizeof(Region));
		if (rp == (Region *)NULL) return (Region *)NULL;
		ERASESTRFLD(rp->hostname);
		ERASESTRFLD(rp->reserved1);
		ERASESTRFLD(rp->reserved2);
		ERASESTRFLD(rp->reserved3);
		rp->count = 0;
	} else {
		rp = (Region *)realloc( (char *)regionp,
			(size_t)((regionp->count) * sizeof(RPrism) + sizeof(Region)) );
		if (rp == (Region *)NULL) return (Region *)NULL;
	}
	memcpy(&rp->rprism[rp->count], prismp, sizeof(RPrism));
	rp->count++;
	return rp;
}

/* NextDBRegion() - Return a pointer to the next region database entry.  
 * Return NULL if error.
 * Note: a local copy must be made of the region before working on it.
 */
Region *
NextDBRegion(void)
{
	if (XV_REGIONVAR == 0 || XV_REGIONVAR == GLOBFLAG) return (Region *)NULL;
	if ( !fread((void *)DBRP, sizeof(Region), (size_t)1, DBFP) ) {
		free(DBRP);
		if (DBFP) fclose(DBFP);
		free((void *)XV_REGIONVAR);
		XV_REGIONVAR = 0;
		return NULL;
	}
	if (DBRP->count > 1) {
		if ( !(DBRP = realloc(DBRP, REGIONSIZE(DBRP))) ) {
			free(DBRP);
			if (DBFP) fclose(DBFP);
			free((void *)XV_REGIONVAR);
			XV_REGIONVAR = 0;
			return NULL;
		}
		if ( !fread((void *)&DBRP->rprism[1],
					(DBRP->count - 1) * sizeof(RPrism), (size_t)1, DBFP) ) {
			free(DBRP);
			if (DBFP) fclose(DBFP);
			free((void *)XV_REGIONVAR);
			XV_REGIONVAR = 0;
			return NULL;
		}
	}
	return DBRP;
}

/* FirstDBRegion() - Open the region database, read the first region, and
 * return a pointer to it.
 * Note: a local copy must be made of the region before working on it.
 */
Region *
FirstDBRegion(int gflag)
{
	char namep[128];

	if (gflag) {
		MemeFilePathName(GLOBALREGIONDATABASE, namep);
	} else {
		MemeFilePathName(LOCALREGIONDATABASE, namep);
	}
	if (XV_REGIONVAR != 0 && XV_REGIONVAR != GLOBFLAG) {
		free(DBRP);
		if (DBFP) fclose(DBFP);
		free((void *)XV_REGIONVAR);
	}
	XV_REGIONVAR = (cell)malloc(sizeof(DBState));
	if (!XV_REGIONVAR) return NULL;
	if ( !(DBFP = fopen(namep, READ_MODE)) ) {
		free((void *)XV_REGIONVAR);
		XV_REGIONVAR = 0;
		return NULL;
	}
        
	/* Read the host ID and count in the first region */
	if ( !(DBRP = (Region *)malloc(sizeof(Region))) ) {
		if (DBFP) fclose(DBFP);
		free((void *)XV_REGIONVAR);
		XV_REGIONVAR = 0;
		return NULL;
	}
	return NextDBRegion();
}

/* AddDBRegion() - Add a region to the database.  Returns the region pointer if
 * successful, NULL otherwise.
 */
Region *
AddDBRegion(Region *rp, int gflag)
{
	Region *temprp;
	char namep[128];
	FILE *db;
        
	temprp = FirstDBRegion(gflag);
	if (!temprp) return NULL;
	while (temprp) {
		if (OverlappingRegions(rp, temprp)) {
			return NULL;
		}
		temprp = NextDBRegion();
	}

	if (gflag) {
		MemeFilePathName(GLOBALREGIONDATABASE, namep);
	} else {
		MemeFilePathName(LOCALREGIONDATABASE, namep);
	}
	db = fopen(namep, MODIFY_MODE);
	if (!db) return NULL;
	fseek(db, 0L, SEEK_END);
	if (!fwrite(rp, REGIONSIZE(rp), (size_t)1, db)) {
		if (db) fclose(db);
		return NULL;
	}
	if (db) fclose(db);
	return rp;
}

/* DeleteDBRegion() - Delete a region from the database.  Returns the 
 * region pointer if successful, NULL otherwise.
 */
Region *
DeleteDBRegion(Region *rp, int gflag)
{
	FILE *tempdb;
	Region *temprp;
	int foundit = 0;
	char filename[10];
	struct timeb t;
	char namep[128];

	if (gflag) {
		MemeFilePathName(GLOBALREGIONDATABASE, namep);
	} else {
		MemeFilePathName(LOCALREGIONDATABASE, namep);
	}
	ftime(&t);
	sprintf(filename, "%x", t.time);
	tempdb = fopen(filename, WRITE_MODE);
	if (!tempdb) return NULL;
	temprp = FirstDBRegion(gflag);
	if (!temprp) {
		if (tempdb) fclose(tempdb);
		return NULL;
	}
	while (temprp) {
		if (memcmp(rp, temprp, REGIONSIZE(rp))) {
			if (!fwrite(temprp, REGIONSIZE(temprp), (size_t)1, tempdb)) {
				if (tempdb) fclose(tempdb);
				return NULL;
			}
		} else {
			foundit = 1;
		}
		temprp = NextDBRegion();
	}
	if (foundit) {
		remove(namep);
		rename(filename, namep);
	} else {
		remove(filename);
	}
	return (foundit ? rp : (Region *)NULL);
}

/*****************************************************************************
 * The following routines are the programmer interface to the cyberspace 
 * management code.
 */

/* RegisterDBRegion() - Add the region to the space-to-host database.  
 * It is assumed that the hostname field has been initialized to the 
 * owner's hostname.
 * Returns the region pointer if successful, NULL if not.
 */
Region *
RegisterDBRegion(Region *rp)
{
	Region *dbrp;
	int gflag;
	char namep[128];
	FILE *db;

        
	/* Find out whether its a global database record.  Set up vars
	 * for opening global vs local region database.
	 */
	gflag = ISGLOB;
	if (gflag) {
		MemeFilePathName(GLOBALREGIONDATABASE, namep);
	} else {
		MemeFilePathName(LOCALREGIONDATABASE, namep);
	}
	/* If the region database isn't there, create it */
	if (!(db = fopen(namep, READ_MODE))) {
		if (gflag) {
			strcpy(namep, GLOBALREGIONDATABASE);
		} else {
			strcpy(namep, LOCALREGIONDATABASE);
		}
		if (db = fopen(namep, WRITE_MODE)) {
			if (fwrite(rp, REGIONSIZE(rp), (size_t)1, db)) {
				fclose(db);
				return rp;
			} else {
				if (db) fclose(db);
				return (Region *)NULL;
			}
		} else {
			return (Region *)NULL;
		}
	}
	dbrp = FirstDBRegion(gflag);
	while (dbrp) {
		if (OverlappingRegions(rp, dbrp)) {
			fclose(db);
			return NULL;
		}
		dbrp = NextDBRegion();
	}
	return AddDBRegion(rp, gflag);
}

/* UnregisterDBRegion() - Delete the region from the database.  Returns 
 * the region pointer if successful, NULL otherwise
 */
Region *
UnregisterDBRegion(Region *rp)
{
	int gflag;

	gflag = ISGLOB;
	return DeleteDBRegion(rp, gflag);
}

/* EnclosingDBRegion() - Return a pointer to the database region that 
 * encloses the point.
 */
Region *
EnclosingDBRegion(double x, double y, double z)
{
	Region *rp, *rp2;
	int gflag;

	gflag = ISGLOB;
        
	rp = FirstDBRegion(gflag);
	if (!rp) return (Region *)NULL;
	while (rp) {
		if (PointInRegion(rp, x, y, z)) {
			if (rp2 = malloc(REGIONSIZE(rp))) {
				memcpy(rp2, rp, REGIONSIZE(rp));
			}
			while (rp) rp = NextDBRegion();
			return rp2;
		}
		rp = NextDBRegion();
	}
	return (Region *)NULL;
}

/* FirstOverlappingDBRegion() - Return pointer to first database region that 
 * overlaps the calling argument region.  Return NULL if none found or error.
 */
Region *
FirstOverlappingDBRegion(Region *rp)
{
	Region *temprp;
	int gflag;

	gflag = ISGLOB;
	temprp = FirstDBRegion(gflag);
	if (!temprp) return NULL;
	while (temprp) {
		if (OverlappingRegions(temprp, rp)) return temprp;
		temprp = NextDBRegion();
	}
	return NULL;
}

/* NextOverlappingDBRegion() - Return pointer to next database region that 
 * overlaps the calling argument region.  Return NULL if none found or error.
 */
Region *
NextOverlappingDBRegion(Region *rp)
{
	Region *temprp;
        
	temprp = NextDBRegion();
	if (!temprp) return NULL;
	while (temprp) {
		if (OverlappingRegions(temprp, rp)) return temprp;
			temprp = NextDBRegion();
	}
	return NULL;
}

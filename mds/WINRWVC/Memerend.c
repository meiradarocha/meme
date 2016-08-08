/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Interface routines for the rendering library.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* These routines are the "standard connector" routines that plug
 * the platform-specific rendering library into Meme.
 */

// #define	RWSTEREO_OVERLOAD

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <dos.h>
#include <float.h>

#include "renderer.h"
#include "rwlib.h"
#include "rwwin.h"

#include "config.h"
#include "forth.h"
#include "externs.h"
#include "winextrn.h"
#include "meme.h"
#include "memerend.h"
#include "mememat.h"
#include "protos.h"
#include "errors.h"

extern	Object	Objects[];
extern	char	errMsg[];

RwScene *theScene;			/* Meme uses one RenderWare scene (for now) */

cell	NumberFrames = 0;	/* For frame rate timing */

static int RWIsSetUp = 0;	/* This flag is true after RwOpen() is called. */
static RwPolygon3d *firstpoly;	/* Used by MemeGetSurface */
static RwDeviceCapabilities *capabilities;	/* Used by SetUpRendering() to find
											 * out if we're using an accelerator
											 */
/*
 * Global variable which are set at SetUpRendering time to tell whether we're using
 * DIBs or not.  If we're using DIBs then we don't use GDI to render into
 * the camera image buffer since GDI is very flakey when doing so.
 */
static long        UsingDibSections = 1L;
static long        UsingWinG = 1L;
static long        UsingDD = 1L;

static long        PassThroughDevice;   /* Non-zero for pass through device */
static long        DoubleBuffered;      /* Non-zero for double buffered device */
static RwInt32     FullScreen;          /* TRUE if we are full screen exclusive in DD */
static HWND        CaptureWindow;       /* Window handle that has captured mouse */
static RECT        OldWinPos;           /* Old window position before we went fullscreen (DD) */

/*
 * If we are using WinG or DIBSection bitmaps then allow stretching
 */

static int         StretchingAllowed = FALSE;


/*
 * Maximum size of the viewer's window.
 * (this is from the demo source code in RW 2.1 --
 * need to change this.)
 */
#define MAXIMUM_WINDOW_WIDTH    640	// Changed from 640 x 480 11Jul98
#define MAXIMUM_WINDOW_HEIGHT   480


/************************************************************************
 *
 *      Function:       WindowsReleaseFocus()
 *                      
 *      Description:    Releases mouse capture and removes the cursor
 *                      clip rectangle.
 *
 *      Parameters:     None
 *
 *      Return Value:   None
 *
 ************************************************************************/
void
WindowsReleaseFocus(void)
{
    ReleaseCapture();
    ClipCursor(NULL);
    CaptureWindow = (HWND)0;
}


/************************************************************************
 *
 *      Function:       WindowsGrabFocus(HWND window)
 *                      
 *      Description:    Captures mouse, and constrains it to the boundaries
 *                      of the application's window.
 *
 *      Parameters:     Window handle to grab focus
 *
 *      Return Value:   None
 *
 ************************************************************************/
void
WindowsGrabFocus(HWND window)
{
    SetCapture(window);
    CaptureWindow = window;

    if (PassThroughDevice | FullScreen)
    {
        RECT        Size;

        GetClientRect(window, &Size);
        ClientToScreen(window, (LPPOINT)&Size);
        ClientToScreen(window, ((LPPOINT)&Size) + 1);
        ClipCursor(&Size);
	}
}


/****************************************************************************/
/* The following code opens and closes the rendering library.               */
void
SetUpRendering(void *param)
{
	RwBool					rtn;
    RwVideoMode             modeinfo;
    // int                     i;
    // char                    buffer[128];
	RwOpenArgument			args[1];

	/* Initialize the array of objects */
	InitializeObjects();

	args[0].option = rwAPPCANUSEFULLSCREEN;
	
	rtn = RwOpenExt("MSWindows", param, 1, args);
	if (!rtn) {
		fatal("Failed to initialize rendering.  Set display to high color.\n", 
												MEMERENDERERRSTATUS);
	}

    /* We enquire what type of image the camera will return from
     * RwGetCameraImage().  This is important because unfortunately
     * GDI rendering to DIBs is rather flakey - we therefore don't
     * do GDI rendering to the camera image if we're using DIBs.
     */
    RwGetDeviceInfo(rwWINUSINGDIBSECTIONS, &UsingDibSections, sizeof(UsingDibSections));
    RwGetDeviceInfo(rwWINUSINGWING, &UsingWinG, sizeof(UsingWinG));
    RwGetDeviceInfo(rwWINUSINGDD, &UsingDD, sizeof(UsingDD));

    /* Get some info about device about whether it's double buffered,
     * and whether it's a pass through device.
     */
    DoubleBuffered = FALSE;
    PassThroughDevice = FALSE;
    if (RwGetDeviceInfo(rwGETDEVICECAPABILITIES, &capabilities, sizeof(capabilities))) {
        DoubleBuffered = (capabilities->features & rwFEATUREHWDOUBLEBUFFER);
        PassThroughDevice = (capabilities->features & rwFEATUREPASSTHROUGHDEVICE);
    }

    /* Check to see what mode we are in, and if fullscreen, resize the window to fill screen.
     * Don't forget to clamp to max window size though.  We assume that we started in mode zero.
     */
    if (RwDeviceControl(rwGETVIDEOMODEINFORMATION, 0, &modeinfo, sizeof(modeinfo))) {
        /* Device knows what we are talking about */
        if (!(modeinfo.flags & rwHWWINDOWED)) {
            int     window_width, window_height;

            /* We need to ensure that the screen is bigger than the camera we will create */
            if ((modeinfo.width < MAXIMUM_WINDOW_WIDTH) || (modeinfo.height < MAXIMUM_WINDOW_HEIGHT)) {
                RwInt32     numModes;
                int         found_good_mode = FALSE;

                if (RwGetDeviceInfo(rwGETNUMVIDEOMODES, &numModes, sizeof(numModes))) {
                    int     i;

                    for (i = 0; i < numModes && !found_good_mode; i++) {
                        if (RwDeviceControl(rwGETVIDEOMODEINFORMATION, i, &modeinfo, sizeof(modeinfo))) {
                            if ((modeinfo.width >= MAXIMUM_WINDOW_WIDTH) && 
														(modeinfo.height >= MAXIMUM_WINDOW_HEIGHT)) {
                                /* Great, this'll do */
                                found_good_mode = RwDeviceControl(rwUSEVIDEOMODE, i, NULL, 0);
                            }
                        }
                    }
                }

                if (!found_good_mode) {
                    /* We can't continue without a suitable mode, so fail */
					fatal("Failed to initialize the rendering subsystem.  No suitable video mode found.\n", 
												MEMERENDERERRSTATUS);
                }
            }

            /* Now we need to make sure that the window is not bigger than our camera */
            window_width = modeinfo.width;
            window_height = modeinfo.height;

            if (window_width > MAXIMUM_WINDOW_WIDTH)
                window_width = MAXIMUM_WINDOW_WIDTH;
            if (window_height > MAXIMUM_WINDOW_HEIGHT)
                window_height = MAXIMUM_WINDOW_HEIGHT;

            /* And size the window - doesn't matter about location - it can't be seen */
            SetWindowPos(param, HWND_TOP, 0, 0, window_width, window_height, 
											SWP_NOCOPYBITS + SWP_NOZORDER);
        }
    }

    /* Find out if we ended up in full screen mode,
       we assume that the default mode is mode 0 */
    FullScreen = FALSE;
    CaptureWindow = (HWND)0;

    if (UsingDD) {
        RwVideoMode     modeinfo;

        if (RwDeviceControl(rwGETVIDEOMODEINFORMATION, 0, &modeinfo, sizeof(modeinfo)))
        {
            if (!(modeinfo.flags & rwHWWINDOWED))
            {
                RECT    Size;

                /* Remember old window pos, and move window to surround new
                   screen position and size */

                GetWindowRect(param, &OldWinPos);

                Size.left = 0;
                Size.right = modeinfo.width;
                Size.top = 0;
                Size.bottom = modeinfo.height;
                AdjustWindowRect(&Size, WS_OVERLAPPEDWINDOW, FALSE);
                MoveWindow(param, Size.left, Size.top, Size.right - Size.left, Size.bottom - Size.top, TRUE);

                WindowsGrabFocus(param);

                FullScreen = TRUE;
            }
        }
    } else if (DoubleBuffered) {
        /* Chances are, if we are double buffered, we are also full screen, and
         * can't see the window.  Capture the mouse.
         */

        GetWindowRect(param, &OldWinPos);

        WindowsGrabFocus(param);
    }

	/* The isVoodoo flag really tells us that we have a full-screen
	 * graphical display and that the window is hidden
	 */
	isVoodoo = FullScreen || DoubleBuffered;

	/* Mark as set up for CleanUpRendering() */
	RWIsSetUp = 1;

	/* Create a scene */
	theScene = RwCreateScene();
	if ( (theScene = RwCreateScene()) == NULL) {
		fatal("Couldn't create rendering \042scene\042\n", MEMERENDERERRSTATUS);
	}
	currentCamera = NULL;
}

void
RenderingBanner()
{
	char buff[200];
	sprintf(buff, "RenderWare(tm) (c) 1993-%s Canon Inc.  All rights reserved.\n", CurrentYear());
	cprint(buff);

/********************* NOTE ******************/
	// The following lines are only necessary if RenderWare version 2 is linked with Meme
	sprintf(buff, "\n**** THIS COPY OF MEME USES RENDERWARE VERSION 2.  FOR NON-COMMERCIAL USE ONLY.\n\n");
	cprint(buff);
/*********************************************/

	sprintf(buff, "Graphics device \042%s\042  %d-bit pixels  %d-bit Zbuf  %dx%d bitmaps/%d tiles.\n", 
		capabilities->device_name,
		capabilities->display_depth,
		capabilities->zbuffer_depth,
		capabilities->default_map_size,
		capabilities->default_map_size,
		capabilities->max_map_tiling);
	cprint(buff);
}

void
CleanUpRendering()
{
	if (RWIsSetUp) {
		if (isVoodoo) {
			RwDeviceControl(rwSETPASSTHROUGHSTATE, TRUE, NULL, 0);
		}
		if (CaptureWindow)
		{
			SetWindowPos(CaptureWindow, HWND_TOPMOST, OldWinPos.left, OldWinPos.top,
						 OldWinPos.right - OldWinPos.left, OldWinPos.bottom - OldWinPos.top,
						 SWP_NOCOPYBITS | SWP_NOACTIVATE);

			WindowsReleaseFocus();
		}
		RwClose();	/* Close down the library */
	}
}

/****************************************************************************
 * This routine sets the view window on the camera in a unified way regardless
 * of the size or shape of the viewport.
 */
void _MemeSetCameraViewWindow(void *vcam, double xfactor, double yfactor)
{
	RwInt32 viewportx, viewporty;
	RwCamera *cam = vcam;

	/* get the viewport */
	RwGetCameraViewport(cam, (RwInt32 *) NULL, (RwInt32 *) NULL,
											&viewportx, &viewporty);

	/* Set the view window.  This gives a slightly wide-angle effect. */
	RwSetCameraViewwindow(cam,
			RMul(FL2REAL(xfactor), RDiv(INT2REAL(viewportx), FL2REAL(200.0) ) ),
			RMul(FL2REAL(yfactor), RDiv(INT2REAL(viewporty), FL2REAL(200.0) ) )
	);
}

void _MemeGetCameraViewWindow(void *vcam, double *xfactor, double *yfactor)
{
	RwInt32 viewportx, viewporty;
	RwReal localxfact, localyfact;
	RwCamera *cam = vcam;

	/* get the viewport */
	RwGetCameraViewport(cam, (RwInt32 *) NULL, (RwInt32 *) NULL,
											&viewportx, &viewporty);

	/* get the view window */
	RwGetCameraViewwindow(cam, &localxfact, &localyfact);
	
	*xfactor = REAL2FL( RDiv( FL2REAL(localxfact), RDiv(INT2REAL(viewportx), INT2REAL(200)) ) ); 
	*yfactor = REAL2FL( RDiv( FL2REAL(localyfact), RDiv(INT2REAL(viewporty), INT2REAL(200)) ) );
}

/****************************************************************************
 * This routine takes all the clumps in a clump hierarchy and makes one
 * clump out of them.  It destroys the clump hierarchy and returns the 
 * new clump.
 */

static RwClump *newclump;

RwClump * __stdcall _MemeFlattenClumpHierarchy(RwClump *clump);

RwClump * __stdcall
_MemeFlattenClumpHierarchy(clump)
RwClump *clump;
{
	return RwAddPolygonsToClump(newclump, clump);
}

RwClump * 
MemeFlattenClumpHierarchy(clump)
RwClump *clump;
{
	newclump = RwCreateClump(1, 1);
	RwForAllClumpsInHierarchy(clump, _MemeFlattenClumpHierarchy);
	RwDestroyClump(clump);
	return newclump;
}

/****************************************************************************
 * The following code is related to Meme objects.  It is the code about objects
 * that contains rendering engine-specific calls.  It creates camera, 
 * light, and visible objects, and destroys objects. 
 */
Object *
NewCameraObject(Object *parent, cell linktype, cell subtype, 
								cell x, cell y, cell width, cell height)
{
	RwCamera *camera;
	Object *obj;
	RwStereoMode mode;

	/* Ask RW to create a camera */
	if (subtype >> 8 == stereoCamera >> 8) {
		camera = RwCreateStereoCamera(MemeScreenWidth(), MemeScreenHeight(), NULL);
		if (camera == NULL) {
			error("Couldn't create camera\n");
			return (Object *)NULL;
		}
		switch (subtype) {
			case stereoInterlacedLRCamera: 
				mode = rwSTEREOINTERLACEDLR;
				break;
			case stereoInterlacedRLCamera: 
				mode = rwSTEREOINTERLACEDRL;
				break;
			case stereoAdjacentLRCamera: 
				mode = rwSTEREOADJACENTLR;
				break;
			case stereoCamera:	/* Default to cross-fusion image pairs */
			case stereoAdjacentRLCamera: 
				mode = rwSTEREOADJACENTRL;
				break;
			default:
				mode = rwSTEREONONE;
				break;
		}
		RwSetStereoCameraMode(camera, mode);

	} else if (subtype >> 8 == monoCamera >> 8) {
		camera = RwCreateCamera(MemeScreenWidth(), MemeScreenHeight(), NULL);

	} else if (subtype >> 8 == textureCamera >> 8) {
		camera = RwCreateCamera(capabilities->default_map_size, 
								capabilities->default_map_size, NULL);
	}

	if (camera == NULL) {
		error("Couldn't create camera\n");
		return (Object *)NULL;
	}
	RwSetCameraNearClipping(camera, CREAL(0.025));

	/* Set the viewport */
	RwSetCameraViewport(camera, x, y, width, height);

	/* Set the view window */
	_MemeSetCameraViewWindow(camera, 0.2, 0.2);

	obj = (Object *)NewObject(parent, cameraObject, subtype, linktype);
	obj->ptr.camera = camera;
	if (subtype >> 8 != textureCamera >> 8) {
		currentCamera = camera;
	}
	curCamtype = subtype;
	return obj;
}

Object *
NewLightObject(Object *parent, cell linktype, cell subtype)
{
	RwLight *light;
	Object *obj;

	/* Ask RW to create a light */
	switch (subtype) {
		case directionalLight:
			light = RwCreateLight(rwDIRECTIONAL, CREAL(0.0), CREAL(-1.0), CREAL(0.0), CREAL(1.0));
			break;
		case conicalLight:
			light = RwCreateLight(rwCONICAL, CREAL(0.0), CREAL(0.0), CREAL(0.0), CREAL(1.0));
			break;
		case pointLight:
		case parallelPointLight:
			light = RwCreateLight(rwPOINT, CREAL(0.0), CREAL(0.0), CREAL(0.0), CREAL(1.0));
			break;
	}
	if (light == NULL) {
		error("Couldn't create light\n");
		return (Object *)NULL;
	}
	RwSetLightRadius(light, CREAL(10.0));
	RwAddLightToScene(theScene, light);

	/* Create a new Meme object */
	obj = (Object *)NewObject(parent, lightObject, subtype, linktype);
	obj->ptr.light = light;
	return obj;
}

Object *
NewVisibleObject(Object *parent, cell linktype, cell subtype, char *filename)
{
	Object *obj;
	RwClump *clump;

	if (filename) {
		/* Get a clump from a file */
		clump = RwReadShape(filename); 
		if (clump == NULL) {
			strcpy(errMsg, "Couldn't read shape ");
			strcat(errMsg, filename);
			strcat(errMsg, "\n");
			error(errMsg);
			return (Object *)NULL;
		}
		clump = MemeFlattenClumpHierarchy(clump);
	} else {
		/* Create an empty clump */
		clump = RwCreateClump(1, 1);
		if (clump == NULL) {
			strcpy(errMsg, "Couldn't create empty shape\n");
			error(errMsg);
			return (Object *)NULL;
		}
	}
	switch (subtype) {
		case containerVisible:
			RwSetClumpHints(clump, (unsigned long)(rwHS | rwCONTAINER));
			break;
		case plainVisible:
		default:
			RwSetClumpHints(clump, (unsigned long)(rwHS | rwEDITABLE));
			break;
	}
	obj = (Object *)NewObject(parent, visibleObject, subtype, linktype);
	obj->ptr.clump = clump;
	RwAddClumpToScene(theScene, clump);
	return obj;
}

Object *
NewSpriteObject(Object *parent, cell linktype, cell texture)
{
	Object *obj;
	RwClump *clump;

	/* Get a clump from a file */
	clump = RwCreateSprite( (RwTexture *)texture );
	if (clump == NULL) {
		strcpy(errMsg, "Couldn't create sprite\n");
		error(errMsg);
		return (Object *)NULL;
	}
	RwSetClumpAxisAlignment(clump, rwALIGNAXISZORIENTY);
	obj = (Object *)NewObject(parent, spriteObject, (cell) 0, linktype);
	obj->ptr.clump = clump;
	RwAddClumpToScene(theScene, clump);
	return obj;	
}

Object *
NewImaginaryObject(Object *parent, cell linktype)
{
	Object *obj;

	obj = NewObject(parent, imaginaryObject, (cell) 0, linktype);
	if (!obj) {
		error("Couldn't create imaginary object\n");
	}
	return obj;
}

Object *
NewCompoundObject(Object *parent, cell linktype)
{
	Object *obj;

	obj = (Object *)NewObject(parent, compoundObject, (cell) 0, linktype);
	if (!obj) {
		error("Couldn't create compound object\n");
		return NULL;
	}
	obj->ptr.module = NULL;	/* Flag as having no associated module	*/
	return obj;				/* Return pointer to allocated object	*/
}

/* This removes an object from the scene and deallocates the memory
 * it occupies.  This code is called by DestroyObject() in meme.c.
 */
void
EngineSpecificDestroyObject(Object *p)
{
	switch (p->objectType) {
	    case cameraObject:
			_MemeRemoveCameraBackdrop(p);
			if (p->ptr.camera == currentCamera) currentCamera = NULL;
			if (p->objectSubtype == stereoCamera) {
				(void)RwDestroyStereoCamera(p->ptr.camera);
			} else {
				(void)RwDestroyCamera(p->ptr.camera);
			}
			break;
	    case lightObject:
			(void)RwRemoveLightFromScene(p->ptr.light);
			(void)RwDestroyLight(p->ptr.light); 
			break;
	    case visibleObject:
		case spriteObject:
			(void)RwRemoveClumpFromScene(p->ptr.clump);
			RwDestroyClump(p->ptr.clump); 
			break;
	    case compoundObject:
	    case audibleObject:	
	    case imaginaryObject: 
			break;
	    default:
			sprintf(errMsg, "EngineSpecificDestroyObject: bad object type\
			%d in object 0x%x\n", p->objectType, p);
			cprint(errMsg);
			break;
	}
}


/****************************************************************************
 * The following code is called by the Meme primitive "render".
 */

/* Recursively walk the object tree and transform each object
 * with respect to its parent's coordinate system.  The transformation
 * stack in RenderWare is used to hold the transformation matrices.
 * On entry, the current matrix (returned by RwScratchMatrix()) 
 * is the parent's local coordinate system transformation matrix.
 * pxscale, pyscale, and pzscale are the parent's scale factors.  It's
 * necessary to pass these in because of the cockamamie way that swivel
 * links are computed.
 */
void
TransformTree(Object *objarg, double pxscale, double pyscale, double pzscale)
{
	RwMatrix4d *matrix;		/* Transformation matrix */
	Object *obj = objarg;	/* Local copy of calling arg */

	/* Loop until this object and all its siblings have
	 * been transformed.
	 */
	for (;;) {
		/* Save a copy of the parent matrix, then get a local
		 * copy of the current matrix pointer.  This is for
		 * reduced execution time (no call to RwScratchMatrix())
		 * and for fewer typed characters.
		 */
		RwPushScratchMatrix();
		matrix = RwScratchMatrix();

		/* Compose the transformation matrix:
		 * Compose the scale.
		 * Compose the yaw, then pitch, then roll.
		 * Compose the translation.  If the object is locked
		 * to its parent, preconcatenate the translation.
		 * If the object is only swivel-bound, postconcatenate
		 * the translation.
		 */
		if (obj->linkType == lockedLink) {
			RwTranslateMatrix(matrix, FL2REAL(obj->x), FL2REAL(obj->y), FL2REAL(obj->z), rwPRECONCAT);
			RwRotateMatrix(matrix, CREAL(0), CREAL(0), CREAL(1), FL2REAL(obj->roll), rwPRECONCAT);
			RwRotateMatrix(matrix, CREAL(1), CREAL(0), CREAL(0), FL2REAL(obj->pitch), rwPRECONCAT);
#ifdef REMOVE
			if (obj->objectType == cameraObject) {
				RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw + 180.0),
																rwPRECONCAT);
			} else {
				RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
			}
#else
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
#endif
			RwScaleMatrix(matrix, FL2REAL(obj->xscale), FL2REAL(obj->yscale),
														FL2REAL(obj->zscale), rwPRECONCAT);
		} else /* if (obj->linkType == swivelLink) */ {
			RwTranslateMatrix(matrix, FL2REAL(obj->x * pxscale), FL2REAL(obj->y * pyscale), 
										FL2REAL(obj->z * pzscale), rwPOSTCONCAT);
			RwRotateMatrix(matrix, CREAL(0), CREAL(0), CREAL(1), FL2REAL(obj->roll), rwPRECONCAT);
			RwRotateMatrix(matrix, CREAL(1), CREAL(0), CREAL(0), FL2REAL(obj->pitch), rwPRECONCAT);
#ifdef REMOVE
			if (obj->objectType == cameraObject) {
				RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw + 180.0),
																rwPRECONCAT);
			} else {
				RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
			}
#else
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
#endif
			RwScaleMatrix(matrix,
						FL2REAL(obj->xscale), FL2REAL(obj->yscale), FL2REAL(obj->zscale), rwPRECONCAT);
		}

		/* Transform the object according to its type.
		 * For audible objects, code still needs to be
		 * written.
		 * Imaginary objects have no behavior in this switch
		 * statement.  They are only used as local coordinate
		 * systems relative to which their children are transformed.
		 */
		switch (obj->objectType) {
			case spriteObject:
			case visibleObject:		RwTransformClump(obj->ptr.clump, matrix, rwREPLACE);		break;
			case cameraObject:
				RwPushScratchMatrix();
				matrix = RwScratchMatrix();
				RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(180.0), rwPRECONCAT);
				RwTransformCamera(obj->ptr.camera, matrix, rwREPLACE);
				RwPopScratchMatrix();
				break;
			case lightObject:
				RwPushScratchMatrix();
				matrix = RwScratchMatrix();
				RwRotateMatrix(matrix, CREAL(1), CREAL(0), CREAL(0), FL2REAL(90.0), rwPRECONCAT);
				RwTransformLight(obj->ptr.light, matrix, rwREPLACE);
				RwPopScratchMatrix();
				break;
			case audibleObject:		/* Audio rendering code here */								break;
		}

		/* If this object has any children, recurse pointing
		 * at the head of the list of children.
		 */
		if (obj->child)	TransformTree(obj->child, 
			pxscale * obj->xscale, pyscale * obj->yscale, pzscale * obj->zscale);

		/* The parent's matrix was saved on the stack on entry to
		 * this loop.  Restore it now by popping the 
		 * current object's matrix off the stack.
		 */
		RwPopScratchMatrix();

		/* If there is a next sibling object, point at it
		 * and loop back, otherwise we're done so return.
		 */
		if ( (obj = obj->sibling) == NULL) return;
	}
}

#ifdef REMOVE

/* Walk the object tree and compose transformations into a matrix,
 * then apply the matrix to the point (0, 0, 0).
 * The object's origin in absolute coordinates is returned in (x, y, z).
 */
#define	LINEAGELEN	100
void
AbsolutePosition(Object *obj, double *x, double *y, double *z)
{
	Object	*lineage[LINEAGELEN];
	Object	*o = obj;
	RwV3d	point;
	RwMatrix4d *mat;
	int i;
	Object *r;

	mat = RwCreateMatrix();
	RwIdentityMatrix(mat);

	point.x = point.y = point.z = CREAL(0.0);

	/* Find the lineage (path of descendents) of the object */
	lineage[0] = NULL;
	for (i = 1; i < LINEAGELEN; i++) {
		lineage[i] = o;
		if (o->parent) {
			o = o->parent;
		} else {
			break;
		}
	}
	if (i == LINEAGELEN) {
		error("AbsolutePosition: ran out of lineage\n");
		return;
	}

	/* Transform this branch of the tree. */
	r = lineage[i];
	TransformBranch(&lineage[i], mat, 1.0, 1.0, 1.0);
	RwTransformPoint(&point, mat);

	/* Set the return values */
	*x = REAL2FL(point.x);
	*y = REAL2FL(point.y);
	*z = REAL2FL(point.z);

	/* Free the matrix storage */
	RwDestroyMatrix(mat);
}

#endif

/* This routine recusively walks the branch of the tree found by
 * AbsolutePosition(), and computes the transformation matrix.
 * Note that the code was lifted out of TransformTree().
 */
void
TransformBranch(Object **lineage, RwMatrix4d *matrix,
							double pxscale, double pyscale, double pzscale)
{
	Object *obj = *lineage;	/* Pointer to the current object */

	/* Compose the transformation matrix:
	 * Compose the scale.
	 * Compose the yaw, then pitch, then roll.
	 * Compose the translation.  If the object is locked
	 * to its parent, preconcatenate the translation.
	 * If the object is only swivel-bound, postconcatenate
	 * the translation.
	 */
	if (obj->linkType == lockedLink) {
		RwTranslateMatrix(matrix, FL2REAL(obj->x), FL2REAL(obj->y), FL2REAL(obj->z), rwPRECONCAT);
		RwRotateMatrix(matrix, CREAL(0), CREAL(0), CREAL(1), FL2REAL(obj->roll), rwPRECONCAT);
		RwRotateMatrix(matrix, CREAL(1), CREAL(0), CREAL(0), FL2REAL(obj->pitch), rwPRECONCAT);
#ifdef REMOVE
		if (obj->objectType == cameraObject) {
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw + 180.0),
															rwPRECONCAT);
		} else {
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
		}
#else
		RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
#endif
		if (obj != &Objects[0]) {
			RwScaleMatrix(matrix,
					FL2REAL(obj->xscale), FL2REAL(obj->yscale), FL2REAL(obj->zscale), rwPRECONCAT);
		}
	} else if (obj->linkType == swivelLink) {
		RwTranslateMatrix(matrix, FL2REAL(obj->x * pxscale), FL2REAL(obj->y * pyscale), 
									FL2REAL(obj->z * pzscale), rwPOSTCONCAT);
		RwRotateMatrix(matrix, CREAL(0), CREAL(0), CREAL(1), FL2REAL(obj->roll), rwPRECONCAT);
		RwRotateMatrix(matrix, CREAL(1), CREAL(0), CREAL(0), FL2REAL(obj->pitch), rwPRECONCAT);
#ifdef REMOVE
		if (obj->objectType == cameraObject) {
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw + 180.0),
															rwPRECONCAT);
		} else {
			RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
		}
#else
		RwRotateMatrix(matrix, CREAL(0), CREAL(1), CREAL(0), FL2REAL(obj->yaw), rwPRECONCAT);
#endif
		if (obj != &Objects[0]) {
			RwScaleMatrix(matrix,
					FL2REAL(obj->xscale), FL2REAL(obj->yscale), FL2REAL(obj->zscale), rwPRECONCAT);
		}
	}

	if (*--lineage != NULL) {
		if (obj != &Objects[0]) {
			TransformBranch(lineage, matrix, pxscale * obj->xscale, 
								pyscale * obj->yscale, pzscale * obj->zscale);
		} else {
			TransformBranch(lineage, matrix, pxscale, pyscale, pzscale);
		}
	}
}


/****************** START OF TRANSFORM ***********************/

#define LINEAGELEN	(100)

/* Walk the object tree and compose transformations into a matrix.
 * Return zero if successful.  If non-zero, error code is in XV_ERRNO.
 */
cell
GetSubtreeMatrix(Object *obj, RwMatrix4d **mat)
{
	Object	*lineage[LINEAGELEN];
	Object	*o = obj;
	int i;
	Object *r;

	RwIdentityMatrix(*mat);

	/* Find the lineage (path of descendents) of the object */
	lineage[0] = NULL;
	for (i = 1; i < LINEAGELEN; i++) {
		lineage[i] = o;
		if (o->parent) {
			o = o->parent;
		} else {
			break;
		}
	}
	if (i == LINEAGELEN) {
		XV_ERRNO = EGETSUBTMTREETOODEEP;
		return -1;
	}

	/* Transform this branch of the tree. */
	r = lineage[i];
	TransformBranch(&lineage[i], *mat, 1.0, 1.0, 1.0);

	return 0;
}

/* Find the transformation from one object to another, and write the
 * equivalent parameters into the second object's translation,
 * rotation, and scaling fields. Return zero if no error.  If error,
 * return non-zero, and error code is in XV_ERRNO.
 */
cell
Transform(Object *obj, Object *destnode, Object *result)
{	
	RwMatrix4d *mata;			/* Object a's transformation matrix */
	RwMatrix4d *matb;			/* Object b's transformation matrix */
	RwMatrix4d *invmat;			/* Inverse of object a's matrix		*/
	RwV3d		point;			/* A 3-D point						*/
	RwReal		elems[4][4];	/* Matrix elements					*/
	RwReal		magsq;			/* Square of magnitude of a vector	*/


	mata = RwCreateMatrix();
	matb = RwCreateMatrix();
	invmat = RwCreateMatrix();

	/* Get the transformation matrix for the object */
	if (GetSubtreeMatrix(obj, &mata)) {
		goto errexit;
	}

	/* Get the transformation matrix for the destination node */
	if (GetSubtreeMatrix(destnode, &matb)) {
		goto errexit;
	}

	/* Invmat is the inverse matrix of the destination node. */
	if (RwInvertMatrix(matb, invmat) == NULL) {
		XV_ERRNO = ETRANSFORMINVERTFAILED;
		goto errexit;
	}

	/* Compose matrix A and inverse of matrix B.
	 * Args are dest, source, op.  Result in mata.
	 */
	if (RwTransformMatrix(mata, invmat, rwPOSTCONCAT) == NULL) {
		XV_ERRNO = ETRANSFORMCOMPOSEFAILED;
		goto errexit;
	}

	/* Now mata has the combined matrix.  Extract translation,
	 * rotation, and scaling information.
	 */
	point.x = point.y = point.z = CREAL(0.0);									/* Translation */
	if (RwTransformPoint(&point, mata) == NULL) {
		XV_ERRNO = ETRANSFORMPOINTFAILED;
		goto errexit;
	}

	result->x = REAL2FL(point.x);
	result->y = REAL2FL(point.y);
	result->z = REAL2FL(point.z);

	RwTranslateMatrix(mata, FL2REAL(-result->x), FL2REAL(-result->y), FL2REAL(-result->z), rwPOSTCONCAT);



	point.x = CREAL(1); point.y = point.z = CREAL(0);							/* Scaling */
	RwTransformVector(&point, mata);
	magsq = point.x * point.x + point.y * point.y + point.z * point.z;
	if (magsq != CREAL(1)) {
		result->xscale = result->yscale = result->zscale = sqrt(REAL2FL(magsq));
		RwScaleMatrix(mata, FL2REAL(1.0 / result->xscale), FL2REAL(1.0 / result->yscale),
						FL2REAL(1.0 / result->zscale), rwPOSTCONCAT);
	}


	/* Get the matrix elements */
	RwGetMatrixElements(mata, elems);														/* Rotation */

	{
		double yaw, pitch, roll;
		double cy;

		cy = sqrt(REAL2FL(elems[1][1]) * REAL2FL(elems[1][1]) + 
				REAL2FL(elems[1][0]) * REAL2FL(elems[1][0]) );

		if (cy > 16 * DBL_EPSILON) {
			yaw = -atan2( REAL2FL(elems[0][2]), REAL2FL(elems[2][2]) );
			pitch = -atan2( -REAL2FL(elems[1][2]), cy);
			roll = -atan2( REAL2FL(elems[1][0]), REAL2FL(elems[1][1]) );
		} else {
			yaw = -atan2(-REAL2FL(elems[2][0]), REAL2FL(elems[0][0]) );
			pitch = -atan2(-REAL2FL(elems[1][2]), cy);
			roll = 0.0;
		}

		result->yaw = DEGREES(yaw);
		result->pitch = DEGREES(pitch);
		result->roll = DEGREES(roll);
	}

	RwDestroyMatrix(mata);
	RwDestroyMatrix(matb);
	RwDestroyMatrix(invmat);
	return 0;

errexit:
	RwDestroyMatrix(mata);
	RwDestroyMatrix(matb);
	RwDestroyMatrix(invmat);
	return -1;

}

#ifdef REMOVE

/* This code does the extraction of Euler angles from the matrix.  It
 * is adopted from Graphics Gems IV.
 */
{
	double cy = sqrt(m[1][1]*m[1][1] + m[1][0]*m[1][0]);
	double yaw, pitch, roll;

	if (cy > 16 * FLT_EPSILON) {
		yaw = -atan2(m[0][2], m[2][2]);
		pitch = -atan2(-m[1][2], cy);
		roll = -atan2(m[1][0], m[1][1]);
	} else {
		yaw = -atan2(-m[2][0], m[0][0]);
		pitch = -atan2(-m[1][2], cy);
		roll = 0.0;
	}
}

#endif




/***************** END OF TRANSFORM *************************/

void 
ShowView(HWND hwnd)
{
    HDC hdc;

	if (currentCamera) {
		if (curCamtype >> 8 == monoCamera >> 8 || curCamtype >> 8 == textureCamera >> 8) {
			// MemeOften();
			RwBeginCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearCameraViewport(currentCamera);
			RwRenderScene(theScene);
			RwEndCameraUpdate(currentCamera);

			// MemeOften();

			hdc = GetDC(hwnd);
			RwShowCameraImage(currentCamera, (void *)(DWORD)hdc);
			ReleaseDC(hwnd, hdc);
			// MemeOften();
		} else if (curCamtype >> 8 == stereoCamera >> 8) {
			// MemeOften();
			RwBeginStereoCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearStereoCameraViewport(currentCamera);
			RwRenderStereoScene(theScene);
			RwEndStereoCameraUpdate(currentCamera);
			// MemeOften();
			hdc = GetDC(hwnd);
			RwShowStereoCameraImage(currentCamera, (void *)(DWORD)hdc);
			ReleaseDC(hwnd, hdc);
			// MemeOften();
		}
	}
}

void
OnEraseBackground(HWND hwnd)
{
	HDC hdc;

	if (currentCamera) {
		if (curCamtype >> 8 == monoCamera >> 8 || curCamtype >> 8 == textureCamera >> 8) {
			RwBeginCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearCameraViewport(currentCamera);
			RwEndCameraUpdate(currentCamera);
			hdc = GetDC(hwnd);
			RwShowCameraImage(currentCamera, (void *)(DWORD)hdc);
			ReleaseDC(hwnd, hdc);
		} else if (curCamtype >> 8 == stereoCamera >> 8) {
			RwBeginStereoCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearStereoCameraViewport(currentCamera);
			RwEndStereoCameraUpdate(currentCamera);
			hdc = GetDC(hwnd);
			RwShowStereoCameraImage(currentCamera, (void *)(DWORD)hdc);
			ReleaseDC(hwnd, hdc);
		}
	}
}

void 
OnPaint(HWND hwnd)
{
    HDC hdc;
    PAINTSTRUCT ps;

	if (currentCamera) {
	    hdc = BeginPaint(hwnd, &ps);
		RwDamageCameraViewport(currentCamera,
                        (RwInt32) ps.rcPaint.left, (RwInt32) ps.rcPaint.top,
                           (RwInt32) (ps.rcPaint.right - ps.rcPaint.left),
                           (RwInt32) (ps.rcPaint.bottom - ps.rcPaint.top));
		if (curCamtype >> 8 == monoCamera >> 8 || curCamtype >> 8 == textureCamera >> 8) {
			RwBeginCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearCameraViewport(currentCamera);
			RwRenderScene(theScene);
			RwEndCameraUpdate(currentCamera);
			RwShowCameraImage(currentCamera, (void *)(DWORD)hdc);
			EndPaint(hwnd, &ps);
		} else if (curCamtype >> 8 == stereoCamera >> 8) {
			RwBeginStereoCameraUpdate(currentCamera, (void *)(DWORD)hwnd);
			RwClearStereoCameraViewport(currentCamera);
			RwRenderStereoScene(theScene);
			RwEndStereoCameraUpdate(currentCamera);
			RwShowStereoCameraImage(currentCamera, (void *)(DWORD)hdc);
			EndPaint(hwnd, &ps);
		}
	}
}

void 
OnSize(HWND hwnd, WPARAM wparam, LPARAM lparam)
{
    RwInt32 width;
    RwInt32 height;
    RwInt32 oldwidth;
    RwInt32 oldheight;

	double xfactor, yfactor;

	if (currentCamera) {
		width = LOWORD(lparam);
		height = HIWORD(lparam);
		if (graphicsWindowIsIconified && wparam == SIZE_RESTORED ||
				graphicsWindowIsIconified && wparam == SIZE_MAXIMIZED ||
				graphicsWindowIsIconified == 0 && wparam == SIZE_MINIMIZED) {
			graphicsWindowIsIconified = wparam == SIZE_MINIMIZED;

			_MemeGetCameraViewWindow(currentCamera, &xfactor, &yfactor);
		    RwGetCameraViewport(currentCamera, (RwInt32) NULL, (RwInt32) NULL, &oldwidth, &oldheight);

		    RwSetCameraViewport(currentCamera, 0, 0, width, height);

			_MemeSetCameraViewWindow(currentCamera, xfactor * (double)oldheight / (double)height,
						yfactor * (double)oldheight / (double)height);		
		} else {
			_MemeGetCameraViewWindow(currentCamera, &xfactor, &yfactor);				 

		    /* Set the camera viewport to match the window's client area. */
		    RwSetCameraViewport(currentCamera, 0, 0, width, height);

			_MemeSetCameraViewWindow(currentCamera, xfactor, yfactor);
		}

		/* If there is a backdrop specified, set the backdrop rectangle to the
		 * same size as the window's rectangle.
		 */
		if (RwGetCameraBackdrop(currentCamera)) {
			RwSetCameraBackdropViewportRect(currentCamera, (RwInt32)0, (RwInt32)0, width, height);
		}
	}
}

/* This way of setting the zoom factor was once used.
#ifdef REMOVE
	        if (width >= height)
	            RwSetCameraViewwindow(currentCamera,
	                                  CREAL(0.2),
	                 RMul(CREAL(0.2), RDiv(INT2REAL(height), INT2REAL(width))));
	        else
	            RwSetCameraViewwindow(currentCamera,
	                  RMul(CREAL(0.2), RDiv(INT2REAL(width), INT2REAL(height))),
	                                  CREAL(0.2));
*/

void
Render(void)
{
	/* Call the recursive transformation engine.  Transforms
	 * all the objects in the tree.
	 */
	RwIdentityMatrix(RwScratchMatrix());
	TransformTree(&Objects[0], 1.0, 1.0, 1.0);
	// InvalidateRect(theGraphicsWindow, (const RECT *)NULL, (BOOL)0);
	ShowView(theGraphicsWindow);
	NumberFrames++;
}

/*****************************************************************************
 * The following code is called by Meme primitives, and must be defined
 * with rendering engine-specific code.
 */
void
MemeCurrentCamera(obj)
Object *obj;
{
	RwInt32 width, height;

	/* Set global variables so Render() knows what to do */
	currentCamera = obj->ptr.camera;
	curCamtype = obj->objectSubtype;

	/* Get the window dimensions */
	width = (RwInt32)MemeWindowWidth();
	height = (RwInt32)MemeWindowHeight();

	/* Set the camera's viewport to be equal to the size of the window */
	RwSetCameraViewport(currentCamera, (RwInt32)0, (RwInt32)0, width, height);

	/* If a backdrop has been specified, resize the backdrop's rectangle as well */
	if (RwGetCameraBackdrop(currentCamera)) {
		RwSetCameraViewport(currentCamera, (RwInt32)0, (RwInt32)0, width, height);
	}
}

void
MemeSetCameraBackColor(obj, r, g, b)
Object	*obj;
double r, g, b;
{
	RwRGBColor rgbColor;

	rgbColor.r = FL2REAL(r);
	rgbColor.g = FL2REAL(g);
	rgbColor.b = FL2REAL(b);
	RwSetCameraBackColorStruct(obj->ptr.camera, &rgbColor);
}

/* Remove the existing backdrop from the camera, if any,
 * and destroy the backdrop's raster.
 */
static void
_MemeRemoveCameraBackdrop(obj)
Object *obj;
{
	RwRaster *ras = RwGetCameraBackdrop(obj->ptr.camera);

	if (ras) {
		RwSetCameraBackdrop(obj->ptr.camera, NULL);
		RwSetCameraBackdropViewportRect( obj->ptr.camera,
			(RwInt32)0, (RwInt32)0, (RwInt32)0, (RwInt32)0 );
		RwDestroyRaster(ras);
	}
}

/* Set the image that will be used as a backdrop for this camera.
 * If the filename is NULL, remove the existing backdrop.
 * Set Meme's errno user variable to indicate success or failure.
 */
void
MemeSetCameraBackdrop(obj, filename)
Object *obj;
char *filename;
{
	RwRaster *ras;
	RwInt32 width, height;

	if (obj->used == 0 || obj->objectType != cameraObject) {
		XV_ERRNO = ESCBBADOBJECT;
	}

	if (filename) {
		ras = RwReadRaster(filename, rwGAMMARASTER);
		if (ras) {
			_MemeRemoveCameraBackdrop(obj);
			RwSetCameraBackdrop(obj->ptr.camera, ras);
			width = (RwInt32)MemeWindowWidth();
			height = (RwInt32)MemeWindowHeight();
			RwSetCameraBackdropViewportRect( obj->ptr.camera, 
				(RwInt32)0, (RwInt32)0, width, height);
			XV_ERRNO = ENOERROR;
		} else {
			_MemeRemoveCameraBackdrop(obj);
			XV_ERRNO = ESCBINVALIDFILE;
		}
	} else {
			_MemeRemoveCameraBackdrop(obj);
			XV_ERRNO = ENOERROR;
	}
}

void
MemeSetCameraBackdropOffset(obj, x, y)
Object *obj;
cell x;
cell y;
{
	RwSetCameraBackdropOffset(obj->ptr.camera, (RwInt32)x, (RwInt32)y);
}

void
MemeSetCameraViewport(obj, x, y, w, h)
Object  *obj;
cell    x, y, w, h;
{
	RwSetCameraViewport(obj->ptr.camera, x, y, w, h);
}

void
MemeSetCameraViewWindow(obj, w, h)
Object *obj;
double w, h;
{
	_MemeSetCameraViewWindow(obj->ptr.camera, FL2REAL(w), FL2REAL(h));
}

cell
MemeScreenHeight()
{
	return isVoodoo ? (cell) 480 : (cell) GetSystemMetrics(SM_CYSCREEN);
}

cell
MemeScreenWidth()
{
	return isVoodoo ? (cell) 640 : (cell) GetSystemMetrics(SM_CXSCREEN);
}

cell
MemeWindowHeight()
{
	RECT rt;
	GetClientRect(theGraphicsWindow, &rt); 
	return (cell) rt.bottom + 1;
}

cell
MemeWindowWidth()
{
	RECT rt;
	GetClientRect(theGraphicsWindow, &rt); 
	return (cell) rt.right + 1;
}

static RwPolygon3d * RWCALLBACK
_MemeSetPolygonColor(poly, color)
RwPolygon3d *poly;
MemeColorStruct *color;
{
	RwSetPolygonColor(poly, FL2REAL(color->r), FL2REAL(color->g),
							FL2REAL(color->b));
	RwSetPolygonOpacity(poly, FL2REAL(color->opacity));
	return poly;
}

void
MemeSetColor(obj, r, g, b, opacity)
Object *obj;
double r, g, b, opacity;
{
	MemeColorStruct color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.opacity = (opacity > 0.9999) ? 1.0 : opacity;
	RwForAllPolygonsInClumpPointer(obj->ptr.clump, (RwPolygon3dFuncPointer)_MemeSetPolygonColor, &color);
}

void
MemePickObject(mrec, screenx, screeny)
MemePickRecord *mrec;
cell screenx, screeny;
{
	RwPickRecord rec;
	RwV3d vertex;
	RwPickClumpData *cdp;

	RwPickScene(theScene, (int)screenx, (int)screeny, 
											(RwCamera *)currentCamera, &rec);
	switch (rec.type) {
	    case rwPICKCLUMP:
			cdp = &rec.object.clump;
			mrec->vindex = cdp->vertex.vindex;
			mrec->polygon = cdp->polygon;
			mrec->clump = cdp->clump;

			RwGetClumpVertex(mrec->clump, mrec->vindex, &vertex);
			mrec->vx = REAL2FL(vertex.x);	/* The nearest vertex's coords in object space */
			mrec->vy = REAL2FL(vertex.y);
			mrec->vz = REAL2FL(vertex.z);

			mrec->wx = cdp->wcpoint.x;		/* The mouse "hit" coords in world space */
			mrec->wy = cdp->wcpoint.y;
			mrec->wz = cdp->wcpoint.z;

			break;
	    case rwNAPICKOBJECT:
	    default:
			mrec->clump = NULL;
			break;
	}
}

void
MemeSetVertex(obj, vindex, x, y, z)
Object *obj;
cell vindex;
double x;
double y;
double z;
{
	RwV3d vertex;
	vertex.x = FL2REAL(x);
	vertex.y = FL2REAL(y);
	vertex.z = FL2REAL(z);
	RwSetClumpVertex(obj->ptr.clump, vindex, &vertex);
}

cell
MemePolygonCount(obj)
Object *obj;
{
	if (obj->objectType == visibleObject || 
						obj->objectType == spriteObject) {
	    return RwGetClumpNumPolygons(obj->ptr.clump);
	} else {
	    return 0;
	}
}

cell
MemeNewTexture(filename)
char *filename;
{
	RwTexture *tp;
	tp = RwReadTexture(filename);
	if (tp == NULL) {
		XV_ERRNO = ETEXINVALIDTEXFILE;
	}
	return (cell)tp;
}

cell
MemeNewMaskedTexture(texfile, maskfile)
char *texfile;
char *maskfile;
{

	cell tp = MemeNewTexture(texfile);
	RwRaster *raster;

	if (!tp) {
			return tp;
	}

	raster = RwReadMaskRaster(maskfile);

	if (!raster) {
		XV_ERRNO = ETEXINVALIDMASKFILE;
		return 0;
	}

	if (RwMaskTexture((RwTexture *)tp, raster)) {
		return tp;
	} else {
		XV_ERRNO = ETEXINVALIDMASKFILE;
		return 0;
	}

}

void
MemeTextureFrameStep(tp, n)
void *tp;
cell n;
{
	RwSetTextureFrameStep((RwTexture *)tp, (int)n);
}

void
MemeTextureNextFrame(tp)
void *tp;
{
	RwTextureNextFrame( (RwTexture *)tp );
}

void
MemeSetTextureFrame(tp, n)
void *tp;
cell n;
{
	RwSetTextureFrame((RwTexture *)tp, (int)n);
}

/*
 * For PolygonMaterial
 */
static RwPolygon3d * __stdcall _MemePolygonMaterial(RwPolygon3d *pp, void *mp);

static RwPolygon3d * __stdcall
_MemePolygonMaterial(pp, mp)
RwPolygon3d *pp;
void *mp;
{
	mMat *matp = mp;
	RwRGBColor color;
	if (matp->r >= 0.0 || matp->g >= 0.0 || matp->b >= 0.0) {
		(void)RwGetPolygonColor(pp, &color);

		if (matp->r < 0.0) matp->r = REAL2FL(color.r);
		if (matp->g < 0.0) matp->g = REAL2FL(color.g);
		if (matp->b < 0.0) matp->b = REAL2FL(color.b);

		RwSetPolygonColor(pp, FL2REAL(matp->r),
							FL2REAL(matp->g), FL2REAL(matp->b));		
	}
	if (matp->opacity >= 0.0) {
		RwSetPolygonOpacity(pp, FL2REAL(matp->opacity));
	}
	if (matp->Ka < 0.0) matp->Ka = RwGetPolygonAmbient(pp);
	if (matp->Kd < 0.0) matp->Kd = RwGetPolygonDiffuse(pp);
	if (matp->Ks < 0.0) matp->Ks = RwGetPolygonSpecular(pp);
	RwSetPolygonSurface(pp, FL2REAL(matp->Ka),
					FL2REAL(matp->Kd), FL2REAL(matp->Ks));
	if (matp->tex != (void *)(-1)) {
		RwSetPolygonTexture(pp, (RwTexture *)(matp->tex));
		RwSetPolygonTextureModes(pp, 
			isVoodoo ? 
				rwLIT | rwFORESHORTEN | rwFILTER | rwTRILINEAR : 
				rwLIT | rwFORESHORTEN
		);
	}
	if (matp->geometry != (-1)) {
		switch (matp->geometry) {
			case pointCloudVisible:
				RwSetPolygonGeometrySampling(pp, rwPOINTCLOUD);
				break;
			case wireFrameVisible:
				RwSetPolygonGeometrySampling(pp, rwWIREFRAME);
				break;
			case solidVisible:
				RwSetPolygonGeometrySampling(pp, rwSOLID);
				break;
		}
	}
	if (matp->shading != (-1)) {
		switch (matp->shading) {
			case flatShadedVisible:
				RwSetPolygonLightSampling(pp, rwFACET);
				break;
			case smoothShadedVisible:
			case phongShadedVisible:
				RwSetPolygonLightSampling(pp, rwVERTEX);
				break;
		}
	}
	return pp;
}

void
MemePolygonMaterial(pp, mp)
void *pp;
mMat *mp;
{
	(void)_MemePolygonMaterial((RwPolygon3d *)pp, mp);
}
		
	
void
MemeShapeMaterial(objp, mp)
Object *objp, *mp;
{
	RwClump *clump = objp->ptr.clump;

	RwForAllPolygonsInClumpPointer(
		clump,
		_MemePolygonMaterial, 
		mp);

	switch ( ((mMat *)mp)->wrap ) {
		case sphericalWrap:
		case cylindricalWrap:
			RwSphericalTexturizeClump(clump);
			break;
		case cubicalWrap:
		case flatWrap:
			RwCubicTexturizeClump(clump);
			break;
		case noWrap:
		case -1:
		default:
			break;
	}

}

void
MemeGetPolygonMaterial(vp, mp)
void *vp;
void *mp;
{
	RwPolygon3d *pp = vp;
	mMat *matp = mp;
	RwRGBColor color;

	(void)RwGetPolygonColor(pp, &color);
	matp->r = REAL2FL(color.r);
	matp->g = REAL2FL(color.g);
	matp->b = REAL2FL(color.b);

	matp->opacity = RwGetPolygonOpacity(pp);

	matp->Ka = RwGetPolygonAmbient(pp);
	matp->Kd = RwGetPolygonDiffuse(pp);
	matp->Ks = RwGetPolygonSpecular(pp);
	matp->tex = RwGetPolygonTexture(pp);

	switch (RwGetPolygonGeometrySampling(pp)) {
		case rwPOINTCLOUD:
			matp->geometry = pointCloudVisible;
			break;
		case rwWIREFRAME:
			matp->geometry = wireFrameVisible;
			break;
		case rwSOLID:
			matp->geometry = solidVisible;
			break;
		default:
			matp->geometry = -1;
	}

	switch (RwGetPolygonLightSampling(pp)) {
		case rwFACET:
			matp->shading = flatShadedVisible;
			break;
		case rwVERTEX:
			matp->shading = smoothShadedVisible;
			break;
		default:
			matp->shading = -1;
	}

	matp->wrap = -1;
}

void
MemeWriteShape(objp, filename)
void *objp;
char *filename;
{
	if (RwWriteShape(filename, ((Object *)objp)->ptr.clump) == 0) {
		error("Couldn't write shape\n");
	}
}

void
MemeDestroyTexture(texturep)
void  *texturep;
{
	RwDestroyTexture((RwTexture *)texturep);
}

cell
MemeGetVertexCount(obj)
Object *obj;
{
	return (cell) RwGetClumpNumVertices(obj->ptr.clump);
}

void
MemeGetVertex(obj, index, x, y, z)
Object *obj;
cell index;
double *x, *y, *z;
{
	RwV3d vertex;

	RwGetClumpVertex(obj->ptr.clump, index, &vertex);
	*x = (double) REAL2FL(vertex.x);
	*y = (double) REAL2FL(vertex.y);
	*z = (double) REAL2FL(vertex.z);
}

Object *
MemeWhichObject(p)
void *p;
{
	int i;
	Object *op;

	for (i = 0, op = &Objects[0]; i < MAXOBJECTS; i++, op++) {
		if (op->used && op->ptr.module == p) break;
	}
	return i < MAXOBJECTS ? op : NULL;
}

void
MemeLightColor(obj, r, g, b)
Object *obj;
double r;
double g;
double b;
{

	if (obj->objectType == lightObject) {
		RwSetLightColor(obj->ptr.light, FL2REAL(r), FL2REAL(g), FL2REAL(b));
 	}
}

void
MemeLightConeAngle(obj, umbra, penumbra)
Object *obj;
double umbra, penumbra;
{
	penumbra;	/* Quiet the compiler */

	if (obj->objectType == lightObject && obj->objectSubtype == conicalLight) {
		RwSetLightConeAngle(obj->ptr.light, FL2REAL(umbra));
	}
}

/*
 * The following code returns a pointer to a polygon inside a clump.
 * The object must be a visible or sprite object.  Pindex is the polygon
 * number, ranging from 0 to #polygons-1.
 * MemeGetPolygon() returns NULL if the polygon number is out of range.
 */
static long _polygonCounter;		/* This counter counts from 0 to #polygons-1 */
static RwPolygon3d * _thePolygon;	/* Pointer to the desired polygon */
static RwClump * _theClump;

static RwPolygon3d * __stdcall
_MemeGetPolygon(RwPolygon3d *p, RwInt32 pindex)
{
	if (_polygonCounter++ == pindex) {
		_thePolygon = p;
		return 0;
	}
	return p;
}


void *
MemeGetPolygon(Object *obj, cell pindex)
{
	if (obj->objectType != visibleObject && obj->objectType != spriteObject) {
		return NULL;
	}
	_polygonCounter = 0;
	_thePolygon = NULL;
	RwForAllPolygonsInClumpInt(obj->ptr.clump, _MemeGetPolygon, pindex);
	return _thePolygon;
}

void
MemeEmptyShape(Object *obj)
{
	if (obj->objectType == visibleObject) {
		RwDestroyClump(obj->ptr.clump);
		obj->ptr.clump = RwCreateClump(1, 1);
		RwAddClumpToScene(theScene, obj->ptr.clump);
	}
}

cell
MemeAddVertex(Object *obj, double x, double y, double z)
{
	return (cell) RwAddVertexToClump(obj->ptr.clump, 
								FL2REAL(x), FL2REAL(y), FL2REAL(z));
}

void *
MemeAddPolygon(Object *obj, cell vindexlist[], cell n)
{
	return RwAddPolygonToClump(obj->ptr.clump, (RwInt32)n, (RwInt32 *)vindexlist);
}

void
MemeAddShape(Object *srcobj, Object *destobj)
{
	if (srcobj->objectType == visibleObject && 
									destobj->objectType == visibleObject) {
		RwAddPolygonsToClump(destobj->ptr.clump, srcobj->ptr.clump);
	}
}

void
MemeReversePolygonNormal(void *polyp)
{
	RwReversePolygonFace(polyp);
}

static RwPolygon3d * __stdcall _MemeReverseShapeNormals(RwPolygon3d *pp);

static RwPolygon3d * __stdcall
_MemeReverseShapeNormals(RwPolygon3d *pp)
{
	return RwReversePolygonFace(pp);
}

void
MemeReverseShapeNormals(Object *obj)
{
	RwForAllPolygonsInClump(obj->ptr.clump, _MemeReverseShapeNormals);
}

void
MemeTextureCamera(Object *obj, cell texp)
{
	RwCamera * cam = obj->ptr.camera;
	RwRaster * ras;
	RwRaster * dupras;
	RwTexture * rtntex;
	RwInt32 height, width;
	
	ras = RwGetCameraRaster(cam);
	height = RwGetRasterHeight(ras);
	width = RwGetRasterWidth(ras);
	dupras = RwDuplicateRaster(ras);
	height = RwGetRasterHeight(dupras);
	width = RwGetRasterWidth(dupras);
	rtntex = RwSetTextureRaster((RwTexture *) texp, dupras);
}



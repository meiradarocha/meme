/****************************************/
/*     */
/*   RenderWare(TM) Graphics Library    */
/*     */
/****************************************/

/*
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligiation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modifiation.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1991, 1992, 1993, 1994. Canon Inc.
 * All Rights Reserved.
 */

#ifndef _RWTYPE_H
#define _RWTYPE_H

#ifndef RWCALLBACK
#define RWCALLBACK
#endif

/* Common type definitions needed everywhere. */
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef long RwInt32;
typedef unsigned long RwUInt32;
typedef RwInt32 RwBool;

typedef struct
{
    void *rs0;
}
RwDepthCueColor;

/* syntax sugar */
#ifndef _MODELH
typedef struct
{
    void *rs0;
}
RwDisplayDevice;

typedef struct
{
    void *rs0;
}
RwPolyList;

typedef struct
{
    void *rs0;
}
RwMatrix4d;

typedef struct _userdraw
{
    void *rs0;
}
RwUserDraw;

typedef struct _camera
{
    void *rs0;
}
RwCamera;

typedef struct _lite
{
    void *rs0;
}
RwLight;

typedef struct _clump
{
    void *rs0;
}
RwClump;

typedef struct _rwpolygon
{
    void *rs0;
}
RwPolygon3d;

typedef struct
{
    void *rs0;
}
RwFont;

typedef struct
{
    void *rs0;
}
RwText;

typedef struct
{
    void *rs0;
}
RwSpline;

typedef struct
{
    void *rs0;
}
RwScene;

typedef struct
{
    void *rs0;
}
RwRaster;

typedef struct _RwTexture
{
    void *rs0;
}
RwTexture;

typedef struct _rwmaterial
{
    void *rs0;
}
RwMaterial;

typedef struct _rwstream
{
    void *rs0;
}
RwStream;

#endif /* _MODEL_H */

/* Rasters */

typedef enum
{
    rwNARASTER = 0,
    rwRASTERINDEXED,         /* pixels contain indices into palette */
    rwRASTERRGBA,            /* pixels contain RGB */
    rwRASTERZLESS,           /* Z buffer uses near ->0 */
    rwRASTERZGREATER         /* Z buffer uses far -> 0 */
}
RwRasterType;

typedef struct
{
    RwInt32       type;
    RwInt32       size;       /* size in bits of a pixel */
    RwInt32       redmask;    /* position of red bits */
    RwInt32       greenmask;  /* position of green bits */
    RwInt32       bluemask;   /* position of blue bits */
    RwInt32       alphamask;  /* position of alpha bits */
}
RwRasterInfo;

/* Streams */

typedef enum
{
    rwNASTREAM = 0,
    rwSTREAMFILE,
    rwSTREAMFILENAME,
    rwSTREAMMEMORY
}
RwStreamType;

typedef enum
{
    rwNASTREAMACCESS = 0,
    rwSTREAMREAD,
    rwSTREAMWRITE,
    rwSTREAMAPPEND
}
RwStreamAccessType;

/* Lights */

typedef enum
{
    rwNALIGHTTYPE = 0,
    rwDIRECTIONAL,
    rwPOINT,
    rwCONICAL
}
RwLightType;

typedef enum
{
    rwNADEPTHCUETYPE = 0,
    rwLINEAR,
    rwEXPONENTIAL,
    rwEXPONENTIALSQUARED
} 
RwDepthCueType;

typedef enum
{
    rwNASTATE = 0,
    rwOFF,
    rwON
}
RwState;

typedef enum
{
    rwNACAMERAPROJECTION = 0,
    rwPERSPECTIVE = 1,
    rwPARALLEL = 2              /* These are bit operators */
}
RwCameraProjection;

typedef enum
{
    rwNACOMBINEOPERATION = 0,
    rwREPLACE,
    rwPRECONCAT,
    rwPOSTCONCAT

}
RwCombineOperation;

typedef enum
{
    rwNALIGHTSAMPLING = 0,
    rwFACET = 1,
    rwVERTEX = 2                /* These are bit operators, so next comes 4 etc. */
}
RwLightSampling;

typedef enum
{
    rwNAGEOMETRYSAMPLING = 0,
    rwPOINTCLOUD,
    rwWIREFRAME,
    rwPOLYLINE,
    rwSOLID
}
RwGeometrySampling;

typedef enum
{
    rwNASPLINETYPE = 0,
    rwOPENLOOP,
    rwCLOSEDLOOP
}
RwSplineType;

typedef enum
{
    rwNASPLINEPATH = 0,
    rwSMOOTH,
    rwNICEENDS
}
RwSplinePath;

typedef enum
{
    rwNASEARCHMODE = 0,
    rwLOCAL,
    rwGLOBAL
}
RwSearchMode;

/* The RwDebugState type will be removed in the next release of
   RenderWare. The following defines are included for backward
   compatibility. Use RwState for new references to this type */

#define RwDebugState RwState
#define rwNADEBUGMESSAGESTATUS rwNASTATE
#define rwDISABLE rwOFF
#define rwENABLE  rwON

typedef enum
{
    rwNADEBUGMESSAGESEVERITY = 0,
    rwINFORM,
    rwWARNING,
    rwERROR
}
RwDebugSeverity;

typedef enum
{
    rwNATEXTUREDITHER = 0,
    rwDITHERON,
    rwDITHEROFF,
    rwAUTODITHER
}
RwTextureDitherMode;

typedef enum
{
    rwNASTEREOMODE = 0,
    rwSTEREONONE,
    rwSTEREOINTERLACEDRL,
    rwSTEREOINTERLACEDLR,
    rwSTEREOADJACENTRL,
    rwSTEREOADJACENTLR
}
RwStereoMode;

/*
 * Pick object type. Not an enum so that we can
 * force structure packing to long word sized (which
 * is not possible in the general case with an enum).
 */
typedef RwInt32 RwPickObject;

#define rwNAPICKOBJECT     (RwInt32)0
#define rwPICKCLUMP        (RwInt32)1
#define rwPICKVERTEX       (RwInt32)2

/*
 * Clump hints
 */
typedef RwUInt32 RwClumpHints;

#define rwCONTAINER        (RwUInt32)0x0001
#define rwHS               (RwUInt32)0x0002
#define rwEDITABLE         (RwUInt32)0x0004

/*
 * Texture modes.
 */
typedef RwUInt32 RwTextureModes;

#define rwLIT              (RwUInt32)0x0001
#define rwFORESHORTEN      (RwUInt32)0x0002
#define rwFILTER           (RwUInt32)0x0004
#define rwTRILINEAR        (RwUInt32)0x0010

/*
 * This is a read only texture mode. You may not set this, but can query it.
 */
#define rwSMALLTEXTURE     (RwUInt32)0x0008

/*
 * Material modes.
 */
typedef RwUInt32 RwMaterialModes;

#define rwDOUBLE           (RwUInt32)0x0080
#define rwDECAL            (RwUInt32)0x0040

/*
 * Raster processing options. These options determine
 * how a bitmap (either read from a file or created
 * by the system) is processed when it is converted
 * into a raster. Currently the flags control whether
 * the incoming bitmap is dithered, filtered to
 * texture size, matched against the RenderWare palette
 * and/or gamma corrected. If the Raster is to be used
 * as a texture, it is also possible to specify the
 * creation of a set of mipmaps from the initial level
 * 0 texture.
 */
typedef RwUInt32 RwRasterOptions;

#define rwAUTODITHERRASTER (RwUInt32)0x0001
#define rwDITHERRASTER     (RwUInt32)0x0002
#define rwFITRASTER        (RwUInt32)0x0004
#define rwINDEXRASTER      (RwUInt32)0x0008
#define rwGAMMARASTER      (RwUInt32)0x0010
#define rwOFFSETRASTER     (RwUInt32)0x0020
#define rwMIPMAPTEXTURE    (RwUInt32)0x0040

/*
 * Palette processing options. These options determine
 * how a palette is processed when it is being written.
 */
typedef RwUInt32 RwPaletteOptions;

#define rwGAMMAPALETTE     (RwUInt32)0x0001

/*
 * UserDraw alignment types.
 */
typedef RwUInt32 RwUserDrawAlignmentTypes;

#define rwALIGNTOP         (RwUInt32)0x0001
#define rwALIGNBOTTOM      (RwUInt32)0x0002
#define rwALIGNLEFT        (RwUInt32)0x0004
#define rwALIGNRIGHT       (RwUInt32)0x0008
#define rwALIGNTOPLEFT     (rwALIGNTOP | rwALIGNLEFT)
#define rwALIGNBOTTOMRIGHT (rwALIGNBOTTOM | rwALIGNRIGHT)

typedef enum
{
    rwNAUSERDRAWTYPE = 0,
    rwCLUMPALIGN,
    rwVERTEXALIGN,
    rwBBOXALIGN,
    rwVPALIGN
}
RwUserDrawType;

typedef struct
{
    RwInt32 x, y, w, h;
}
RwRect;

typedef struct
{
    RwReal x, y, z;
}
RwV3d;

typedef struct
{
    RwReal u, v;
    /* Possibly add more elements here in future. */
}
RwUV;

/*
 * Memory
 */

typedef struct _rwmemory
{
    char *start;
    RwUInt32 length;
}
RwMemory;

/*
 * Called RwRGBColor as we may introduce other color models at some
 * time later, i.e. RwHSVColor.
 */
typedef struct
{
    RwReal r, g, b;
}
RwRGBColor;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char flags;
}
RwPaletteEntry;

#define rwPALETTELENGTH 256
typedef RwPaletteEntry RwPalette[rwPALETTELENGTH];

typedef struct
{
    RwInt32 vindex;             /* closest vertex to pick point */
    RwInt32 d2;                 /* distance squared in pixels */
}
RwPickVertexData;

typedef struct
{
    struct _clump *clump;
    struct _rwpolygon *polygon;
    RwPickVertexData vertex;
    RwV3d wcpoint;
}
RwPickClumpData;

typedef struct
{
    RwPickObject type;
    union
    {
        RwPickClumpData clump;
        RwPickVertexData vertex;
    }
    object;
}
RwPickRecord;

/*
 * Binary file format public chunk types
 */

#define rwCHUNKCAMERA 		0x43414d45UL  /* CAME */
#define rwCHUNKMATRIX 		0x4d415458UL  /* MATX */
#define rwCHUNKMATERIAL 	0x4d415452UL  /* MATR */
#define rwCHUNKPALETTE		0x50414c4cUL  /* PALL */
#define rwCHUNKRASTER		0x52415354UL  /* RAST */
#define rwCHUNKTEXTURE		0x54455855UL  /* TEXU */
#define rwCHUNKCLUMP		0x434c554dUL  /* CLUM */
#define rwCHUNKLIGHT		0x4c495445UL  /* LITE */
#define rwCHUNKSCENE		0x5343454eUL  /* SCEN */
#define rwCHUNKDATA			0x44415441UL  /* DATA */
#define rwCHUNKRECT			0x52454354UL  /* RECT */
#define rwCHUNKV3D			0x56334420UL  /* V3D  */
#define rwCHUNKSTRING		0x53544e47UL  /* STRG */

#define rwCHUNKUSER			0x5a5a5a5aUL  /* ZZZZ */

/*
 * Binary file format flags
 */

#define rwSAVECHUNKNORMALS		0x1 /* Want normals saved */
#define rwSAVECHUNKUVS		    0x2 /* Want UVs saved */
#define rwSAVECHUNKLUMINANCES	0x4  /* Save chunk luminances */
#define rwSAVECHUNKRASTERS		0x8 /* Save chunk rasters */
#define rwSAVECHUNKTAGS			0x10  /* Save chunk tags */

#define rwSAVECHUNKALL 			0x1f  /* Save everything */

#define rwLOADCHUNKRASTERS		0x8 /* Force rasters to be loaded */

/*
 * Device checking
 */

/* Looking for capabilities */

#define rwCAPABILITYZBUFFERED     (0x1)

#define rwCAPABILITYFLAT          (0x0)
#define rwCAPABILITYTEXTURED      (0x2)
#define rwCAPABILITYPCTEXTURED    (0x4)

#define rwNUMCAPABILITIES (0x6)

/* Feature flags */

#define rwFEATUREHWDOUBLEBUFFER     (0x1)
#define rwFEATUREPALETTEBASED       (0x2)
#define rwFEATUREHARDWARESUPPORT    (0x4)
#define rwFEATUREVARIABLEMAPSIZE    (0x8)
#define rwFEATUREMOUSESUPPORT       (0x10)
#define rwFEATUREZBUFFER            (0x20)
#define rwFEATURERESOLUTIONSWITCH   (0x40)
#define rwFEATUREDEPTHSWITCH        (0x80)
#define rwFEATUREINTERLACEDSTEREO   (0x100)
#define rwFEATUREPASSTHROUGHDEVICE  (0x200)
#define rwFEATUREMIPMAPS            (0x400)
#define rwFEATURETRUEOPACITY        (0x800)
#define rwFEATURECOLORFOG           (0x1000)
#define rwFEATUREGENERICHARDWARE    (0x2000)

/* Capability features */

#define rwCAPABILITYMODEOPACITY     (0x1)
#define rwCAPABILITYMODEGOURAUD     (0x2)
#define rwCAPABILITYMODEFULLGOURAUD (0x4)

#define rwCAPABILITYMODETRANSPARENT (0x8)  /* Only with texturing */
#define rwCAPABILITYMODEFILTERED    (0x10)  /* Only with texturing */
#define rwCAPABILITYMODELITTEXTURE  (0x20)  /* Only with texturing */
#define rwCAPABILITYMODEGOURAUDTEXTURE (0x40)  /* Only with texturing */

typedef struct
{
    char device_name[80];
    RwUInt32 num_names;
    RwUInt32 version_major;
    RwUInt32 version_minor;
    RwUInt32 version_release;
    RwUInt32 display_depth;
    RwUInt32 zbuffer_depth;
    RwUInt32 default_map_size;
    RwUInt32 max_map_tiling;    /* Amount of tiling repititions */
    RwUInt32 small_map_size;
    RwUInt32 max_small_map_tiling;
    RwUInt32 features;

    RwUInt32 capabilities[6];
    RwUInt32 hardware_capabilities[6];
}
RwDeviceCapabilities;

/*
 * Immediate
 */

#define rwIMMEDIATE3D 0
#define rwIMMEDIATE2D 1
#define rwIMMEDIATEVERTICES 2

typedef struct
{
    RwInt32 _d0[2];
    RwReal cameraz;
    RwFixed x;
    RwFixed y;
    RwFixed z;
}
RwImmV2d;

typedef struct
{
    RwV3d p3d;                  /* Object cooridnates in local space */
    RwImmV2d p2d;
    RwReal _d5, _d6, _d7;
    RwReal _d8, _d9, _dA;
    RwReal _dB, _dC, _dD;
    unsigned char flag;
    char _d1[3];
    RwV3d _d2;
    RwFixed lum;
    RwFixed lumg;
    RwFixed lumb;
    RwInt32 texu, texv;         /* Texture coords */
    RwInt32 vertex;
    void **_d4;
}
RwImmVertex3d;

typedef struct
{
    struct _rwmaterial *material;
    RwFixed lum;
    RwFixed lumg;
    RwFixed lumb;
    RwInt32 _d3[11];
    void *_d4[3];
}
RwImmPolygon3d;

typedef struct _rwimmediate
{
    RwInt32 type;
    RwBool zbuffer;             /* TRUE allow Z buffering */

    RwImmVertex3d vertex[3];
    RwImmPolygon3d poly;
}
RwImmediate;

/*
 * Video mode structure
 */

#define rwHWDOUBLEBUFFER (0x1)
#define rwHWWINDOWED     (0x2)
#define rwHWSECONDARY    (0x4)

typedef struct _rwvideomode
{
    RwUInt32 width;
    RwUInt32 height;
    RwUInt32 depth;
    RwBool accessible;
    RwUInt32 flags;
}
RwVideoMode;

/*
 * STD C library call backs
 */
#ifdef WIN32
/*
 * The Windows version needs extra qualifiers.
 */
typedef void *
  (__cdecl *RwStdlibFuncMalloc)  ARGS((size_t size));
typedef void *
  (__cdecl *RwStdlibFuncCalloc)  ARGS((size_t size, size_t count));
typedef void *
  (__cdecl *RwStdlibFuncRealloc) ARGS((void *ptr, size_t size));
typedef void
  (__cdecl *RwStdlibFuncFree)    ARGS((void *ptr));
#else
typedef void *
  (*RwStdlibFuncMalloc)  ARGS((size_t size));
typedef void *
  (*RwStdlibFuncCalloc)  ARGS((size_t size, size_t count));
typedef void *
  (*RwStdlibFuncRealloc) ARGS((void *ptr, size_t size));
typedef void
  (*RwStdlibFuncFree)    ARGS((void *ptr));
#endif

typedef struct
{
    RwStdlibFuncMalloc rwmalloc;
    RwStdlibFuncCalloc rwcalloc;
    RwStdlibFuncRealloc rwrealloc;
    RwStdlibFuncFree rwfree;
} RwStdlib;

/*
 * Clump call backs
 */

typedef RwInt32
  (RWCALLBACK * RwClumpPredicateVoid) ARGS((struct _clump * target));
typedef RwInt32
  (RWCALLBACK * RwClumpPredicateInt) ARGS((struct _clump * target, RwInt32 arg));
typedef RwInt32
  (RWCALLBACK * RwClumpPredicateLong) ARGS((struct _clump * target, RwInt32 arg));
typedef RwInt32
  (RWCALLBACK * RwClumpPredicateReal) ARGS((struct _clump * target, RwReal arg));
typedef RwInt32
  (RWCALLBACK * RwClumpPredicatePointer) ARGS((struct _clump * target, void *arg));

typedef struct _clump *
  (RWCALLBACK * RwClumpFuncVoid) ARGS((struct _clump * target));
typedef struct _clump *
  (RWCALLBACK * RwClumpFuncInt) ARGS((struct _clump * target, RwInt32 arg));
typedef struct _clump *
  (RWCALLBACK * RwClumpFuncLong) ARGS((struct _clump * target, RwInt32 arg));
typedef struct _clump *
  (RWCALLBACK * RwClumpFuncReal) ARGS((struct _clump * target, RwReal arg));
typedef struct _clump *
  (RWCALLBACK * RwClumpFuncPointer) ARGS((struct _clump * target, void *arg));

/*
 * Light call backs
 */

typedef struct _lite *
  (RWCALLBACK * RwLightFuncVoid) ARGS((struct _lite * target));
typedef struct _lite *
  (RWCALLBACK * RwLightFuncInt) ARGS((struct _lite * target, RwInt32 arg));
typedef struct _lite *
  (RWCALLBACK * RwLightFuncLong) ARGS((struct _lite * target, RwInt32 arg));
typedef struct _lite *
  (RWCALLBACK * RwLightFuncReal) ARGS((struct _lite * target, RwReal arg));
typedef struct _lite *
  (RWCALLBACK * RwLightFuncPointer) ARGS((struct _lite * target, void *arg));

/*
 * Polygon call backs
 */

typedef struct _rwpolygon *
  (RWCALLBACK * RwPolygon3dFuncVoid) ARGS((struct _rwpolygon * target));
typedef struct _rwpolygon *
  (RWCALLBACK * RwPolygon3dFuncInt) ARGS((struct _rwpolygon * target, RwInt32 arg));
typedef struct _rwpolygon *
  (RWCALLBACK * RwPolygon3dFuncLong) ARGS((struct _rwpolygon * target, RwInt32 arg));
typedef struct _rwpolygon *
  (RWCALLBACK * RwPolygon3dFuncReal) ARGS((struct _rwpolygon * target, RwReal arg));
typedef struct _rwpolygon *
  (RWCALLBACK * RwPolygon3dFuncPointer) ARGS((struct _rwpolygon * target, void *arg));

/*
 * Texture call backs
 */

typedef struct _RwTexture *
  (RWCALLBACK * RwTextureFuncVoid) ARGS((struct _RwTexture * target));
typedef struct _RwTexture *
  (RWCALLBACK * RwTextureFuncInt) ARGS((struct _RwTexture * target, RwInt32 arg));
typedef struct _RwTexture *
  (RWCALLBACK * RwTextureFuncLong) ARGS((struct _RwTexture * target, RwInt32 arg));
typedef struct _RwTexture *
  (RWCALLBACK * RwTextureFuncReal) ARGS((struct _RwTexture * target, RwReal arg));
typedef struct _RwTexture *
  (RWCALLBACK * RwTextureFuncPointer) ARGS((struct _RwTexture * target, void *arg));

/*
 * UserDraw call backs
 */

typedef struct _userdraw *
  (RWCALLBACK * RwUserDrawFuncVoid) ARGS((struct _userdraw * target));
typedef struct _userdraw *
  (RWCALLBACK * RwUserDrawFuncInt) ARGS((struct _userdraw * target, RwInt32 arg));
typedef struct _userdraw *
  (RWCALLBACK * RwUserDrawFuncLong) ARGS((struct _userdraw * target, RwInt32 arg));
typedef struct _userdraw *
  (RWCALLBACK * RwUserDrawFuncReal) ARGS((struct _userdraw * target, RwReal arg));
typedef struct _userdraw *
  (RWCALLBACK * RwUserDrawFuncPointer) ARGS((struct _userdraw * target, void *arg));

/*
 * Immediate call backs
 */

typedef void (RWCALLBACK * RwClumpCallBack) ARGS((struct _clump * clump));

/*
 * The User Draw Function Prototype
 */

typedef struct _userdraw *(RWCALLBACK * RwUserDrawCallBack) ARGS((struct _userdraw * userdraw,
                                                              void *ddimage,
                                                              RwRect * rect,
                                                               void *data));

/*
 * The User Raster callback used in RwShowCameraImage
 */

typedef void (RWCALLBACK * RwCameraShowCallBack) ARGS((struct _camera * camera, RwRect * rect));

/* clump axis alignment types */

typedef enum
{
    rwNAAXISALIGNMENT = 0,
    rwNOAXISALIGNMENT,
    rwALIGNAXISZORIENTX,
    rwALIGNAXISZORIENTY,
    rwALIGNAXISXYZ
}
RwAxisAlignment;

/* codes used for interogating system information */

typedef enum
{
    rwNASYSTEMINFO = 0,
    rwVERSIONSTRING,
    rwVERSIONMAJOR,
    rwVERSIONMINOR,
    rwVERSIONRELEASE,
    rwFIXEDPOINTLIB,
    rwDEBUGGINGLIB
}
RwSystemInfo;

/*
 * Device info type.
 * Note this is defined as a int rather than an enumerated type
 * to make it simpler for individual devices to specify
 * addional pieces of device info.
 */
typedef RwUInt32 RwDeviceInfo;

/* Generic device info. */
#define rwNADEVICEINFO         (RwUInt32)0
#define rwRENDERDEPTH          (RwUInt32)1
#define rwINDEXEDRENDERING     (RwUInt32)2
#define rwPALETTEBASED         (RwUInt32)3

/* For output devices with a hardware palette. */
#define rwPALETTE              (RwUInt32)4
#define rwPALETTESIZE          (RwUInt32)5
#define rwFIRSTPALETTEENTRY    (RwUInt32)6
#define rwLASTPALETTEENTRY     (RwUInt32)7

/* Immediate mode device dependent value access */
#define rwMAXLUMINANCE         (RwUInt32)8
#define rwMAXZBUFFERZ          (RwUInt32)9
#define rwMINZBUFFERZ          (RwUInt32)10

/* Meta ramp access */

#define rwGETPALETTERAMPLENGTH (RwUInt32)11

/* Information about a device */

#define rwGETDEVICECAPABILITIES (RwUInt32)12

/* Video mode access */

#define rwGETNUMVIDEOMODES        (RwUInt32)13

#define rwGETGAMMACORRECT 		  (RwUInt32)14
#define rwGETDOUBLEBUFFER       (RwUInt32)15

/* Z buffer information */

#define rwGETZBUFFERSTATE	      (RwUInt32)16

/* Lighting */

#define rwGETPSEUDOPHONG		(RwUInt32)17
#define rwGETSATURATION			(RwUInt32)18

/*First code available for device specific info types. */
#define rwDEVICESPECIFICINFO   (RwUInt32)1000

typedef RwUInt32 RwDeviceAction;

/* Generic device control actions. */
#define rwNADEVICEACTION       (RwUInt32)0
#define rwGETPALETTEENTRYNO    (RwUInt32)1
#define rwSETPALETTEENTRY      (RwUInt32)2

#define rwUSEVIDEOMODE         (RwUInt32)3
#define rwGETVIDEOMODEINFORMATION (RwUInt32)4

#define rwSETGAMMACORRECT      (RwUInt32)5
#define rwSETDOUBLEBUFFER    (RwUInt32)6

#define rwSETZBUFFERSTATE      (RwUInt32)7

/* Lighting */

#define rwSETPSEUDOPHONG		(RwUInt32)8
#define rwSETSATURATION			(RwUInt32)9

/* Palette control */

#define rwSETINTERNALPALETTEENTRY (RwUInt32)10
#define rwGETINTERNALPALETTEENTRY (RwUInt32)11

#define rwRESOLVEINTERNALPALETTE (RwUInt32)12

#define rwGETPALETTEMATCHSTATE   (RwUInt32)13
#define rwSETPALETTEMATCHSTATE   (RwUInt32)14

/* Video pass through state */

#define rwSETPASSTHROUGHSTATE   (RwUInt32)15

/* Restore hardware rasters */

#define rwRESTOREHARDWARERASTERS (RwUInt32)16

/* First code available for device specific action types. */
#define rwDEVICESPECIFICACTION (RwUInt32)1000

/*
 * RwOpenExt parameter type. Contains a attribute-value pair for
 * device opening.
 */
typedef RwUInt32 RwOpenOption;
typedef struct _openargument
{
    RwOpenOption option;
    void *value;
}
RwOpenArgument;

/* Generic RwOpenExt paramaters. */
#define rwNAOPENOPTION         (RwUInt32)0
#define rwNOOPENOPTION         (RwUInt32)1

#define rwGAMMACORRECT         (RwUInt32)2

#define rwOPENOPTIONCORRECT	   (RwUInt32)0x80000000UL
#define rwOPENOPTIONMASK	   (RwUInt32)0x7fffffffUL

/* First code available for device specific open options. */
#define rwDEVICESPECIFICOPEN   (RwUInt32)1000

#define RWMAXPATHLEN	   1024
#define RWMAXSHAPEPATHLEN  8192
#define RWMAXDEVICELEN     256

#endif

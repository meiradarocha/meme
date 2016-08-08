/******************************************/
/*                                        */
/*    RenderWare(TM) Graphics Library     */
/*                                        */
/******************************************/

/*
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1991, 1992, 1993, 1994. Canon Inc.
 * All Rights Reserved.
 */

#ifndef   _RWLIBH
#define   _RWLIBH

#ifndef    ARGS
#define    ARGS(ALIST)     ALIST
#endif /* ARGS */

#ifdef WIN32
#define RWCALLBACK   __stdcall
#define RWAPI(TYPE)  TYPE RWCALLBACK
#define RWSPI(TYPE)  TYPE RWCALLBACK
#endif

#ifndef    RWCALLBACK
#define    RWCALLBACK
#endif

#ifndef    RWAPI
#define    RWAPI(TYPE)     TYPE
#endif /* RWAPI */
#ifndef    RWSPI
#define    RWSPI(TYPE)     RWAPI(TYPE)
#endif /* RWAPI */

#ifdef RWSTEREO_OVERLOAD        /* overloading basic RwCamera functions */

#define RwBeginCameraUpdate   RwBeginStereoCameraUpdate
#define RwEndCameraUpdate     RwEndStereoCameraUpdate
#define RwClearCameraViewport RwClearStereoCameraViewport
#define RwRenderClump         RwRenderStereoClump
#define RwRenderScene         RwRenderStereoScene
#define RwCreateCamera        RwCreateStereoCamera
#define RwDestroyCamera       RwDestroyStereoCamera
#define RwShowCameraImage     RwShowStereoCameraImage

#endif                          /* RWSTEREO_OVERLOAD  overloading basic RwCamera functions */

#include <stdio.h>

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

#include "rwmacros.h"
#include "rwtypes.h"

#ifndef _MODELH
#include "rwerrors.h"
#endif                          /* _MODELH */

extern RWAPI (RwClump *)         RwAddChildToClump ARGS((RwClump * clump,
                                                         RwClump * child));
extern RWAPI (RwScene *)         RwAddClumpToScene ARGS((RwScene *,
                                                         RwClump *));
extern RWAPI (RwInt32)           RwAddHint ARGS((RwClumpHints hint));
extern RWAPI (RwClump *)         RwAddHintToClump ARGS((RwClump * clump,
                                                        RwClumpHints hint));
extern RWAPI (RwScene *)         RwAddLightToScene ARGS((RwScene *,
                                                         RwLight *));
extern RWAPI (RwMaterial *)      RwAddMaterialModeToMaterial ARGS((RwMaterial * m,
                                                                   RwMaterialModes mode));
extern RWAPI (RwPolygon3d *)     RwAddMaterialModeToPolygon ARGS((RwPolygon3d * apol,
                                                                  RwMaterialModes mode));
extern RWAPI (RwInt32)           RwAddMaterialModeToSurface ARGS((RwMaterialModes mode));
extern RWAPI (RwPolygon3d *)     RwAddPolygonToClump ARGS((RwClump * clump,
                                                           RwInt32 sides,
                                                           RwInt32 * varray));
extern RWAPI (RwClump *)         RwAddPolygonsToClump ARGS((RwClump * dst,
                                                            RwClump * src));
extern RWAPI (RwMaterial *)      RwAddTextureModeToMaterial ARGS((RwMaterial * m,
                                                                  RwTextureModes mode));
extern RWAPI (RwPolygon3d *)     RwAddTextureModeToPolygon ARGS((RwPolygon3d * apol,
                                                                 RwTextureModes mode));
extern RWAPI (RwInt32)           RwAddTextureModeToSurface ARGS((RwTextureModes mode));
extern RWAPI (RwTexture *)       RwAddTextureToDict ARGS((char *name,
                                                          RwTexture * texture));
extern RWAPI (RwClump *)         RwAddUserDrawToClump ARGS((RwClump * clump,
                                                            RwUserDraw * userdraw));
extern RWAPI (RwV3d *)           RwAddVector ARGS((RwV3d * a,
                                                   RwV3d * b,
                                                   RwV3d * c));
extern RWAPI (RwInt32)           RwAddVertexToClump ARGS((RwClump *,
                                                          RwReal x,
                                                          RwReal y,
                                                          RwReal z));
extern RWAPI (RwCamera *)        RwBeginCameraUpdate ARGS((RwCamera * cam,
                                                           void *param));
extern RWAPI (RwCamera *)        RwBeginStereoCameraUpdate ARGS((RwCamera * c,
                                                                 void *param));
extern RWAPI (RwRaster *)        RwBitmapRaster ARGS((void *bitmap,
                                                      RwRasterOptions options));
extern RWAPI (RwInt32)           RwBlock ARGS((RwReal w,
                                               RwReal h,
                                               RwReal d));
extern RWAPI (RwClump *)         RwCalculateClumpVertexNormal ARGS((RwClump * clump,
                                                                    RwInt32 vindex));
extern RWAPI (void *)            RwCalloc ARGS((RwInt32 count, RwInt32 size));
extern RWAPI (RwBool)            RwCheckStartDisplayDeviceExt ARGS((RwDisplayDevice * dpDev,
                                                                    void *param,
                                                                    RwInt32 numargs,
                                                                    RwOpenArgument * args));
extern RWAPI (RwCamera *)        RwClearCameraViewport ARGS((RwCamera * cam));
extern RWAPI (RwCamera *)        RwClearStereoCameraViewport ARGS((RwCamera * cam));
extern RWAPI (void)              RwClose ARGS((void));
extern RWAPI (void)              RwCloseDebugStream ARGS((void));
extern RWAPI (RwBool)            RwCloseDisplayDevice ARGS((RwDisplayDevice * dpDev));
extern RWAPI (RwBool)            RwCloseStream ARGS((RwStream * stpStream,
                                                     void *pData));
extern RWAPI (RwInt32)           RwClumpBegin ARGS((void));
extern RWAPI (RwReal)            RwClumpDistance ARGS((RwClump * clump,
                                                       RwV3d * pos));
extern RWAPI (RwClump *)         RwClumpEnd ARGS((RwClump ** instance));
extern RWAPI (RwInt32)           RwCone ARGS((RwReal height,
                                              RwReal radius,
                                              RwInt32 sides));
extern RWAPI (RwMaterial *)      RwCopyMaterial ARGS((RwMaterial * src,
                                                      RwMaterial * dst));
extern RWAPI (RwMatrix4d *)      RwCopyMatrix ARGS((RwMatrix4d * src,
                                                    RwMatrix4d * dst));
extern RWAPI (RwCamera *)        RwCreateCamera ARGS((RwInt32 maxw,
                                                      RwInt32 maxh,
                                                      RwRaster * image));
extern RWAPI (RwClump *)         RwCreateClump ARGS((RwInt32 vcount,
                                                     RwInt32 pcount));
extern RWAPI (RwPolyList *)      RwCreateClumpOrder ARGS((RwClump * clump,
                                                          RwInt32 * pindex));
extern RWAPI (RwDepthCueColor *) RwCreateDepthCueColor ARGS((RwReal r, RwReal g, RwReal b));
extern RWAPI (RwDepthCueColor *) RwCreateDepthCueColorStruct ARGS((RwRGBColor *rgb));
extern RWAPI (RwLight *)         RwCreateLight ARGS((RwLightType le,
                                                     RwReal x,
                                                     RwReal y,
                                                     RwReal z,
                                                     RwReal lum));
extern RWAPI (RwMaterial *)      RwCreateMaterial ARGS((void));
extern RWAPI (RwMatrix4d *)      RwCreateMatrix ARGS((void));
extern RWAPI (RwRaster *)        RwCreateRaster ARGS((RwInt32 w,
                                                      RwInt32 h));
extern RWAPI (RwScene *)         RwCreateScene ARGS((void));
extern RWAPI (RwSpline *)        RwCreateSpline ARGS((RwInt32 npoints,
                                                      RwSplineType type,
                                                      RwV3d * pts));
extern RWAPI (RwClump *)         RwCreateSprite ARGS((RwTexture * texture));
extern RWAPI (RwCamera *)        RwCreateStereoCamera ARGS((RwInt32 w,
                                                            RwInt32 h,
                                                            RwRaster * param));
extern RWAPI (RwTexture *)       RwCreateTexture ARGS((RwRaster * raster));
extern RWAPI (RwUserDraw *)      RwCreateUserDraw ARGS((RwUserDrawType type,
                                                        RwUserDrawAlignmentTypes align,
                                                        RwInt32 x,
                                                        RwInt32 y,
                                                        RwInt32 width,
                                                        RwInt32 height,
                                                        RwUserDrawCallBack callback));
extern RWAPI (RwRaster *)        RwCreateUserRaster ARGS((RwInt32 w,
                                                          RwInt32 h,
                                                          RwInt32 stride,
                                                          void *pixels));
extern RWAPI (RwV3d *)           RwCrossProduct ARGS((RwV3d * a,
                                                      RwV3d * b,
                                                      RwV3d * c));
extern RWAPI (RwClump *)         RwCubicTexturizeClump ARGS((RwClump * clump));
extern RWAPI (RwMaterial *)      RwCurrentMaterial ARGS((void));
extern RWAPI (RwInt32)           RwCylinder ARGS((RwReal height,
                                                  RwReal minrad,
                                                  RwReal maxrad,
                                                  RwInt32 sides));
extern RWAPI (RwCamera *)        RwDamageCameraViewport ARGS((RwCamera * cam,
                                                              RwInt32 x,
                                                              RwInt32 y,
                                                              RwInt32 w,
                                                              RwInt32 h));
extern RWAPI (RwScene *)         RwDefaultScene ARGS((void));
extern RWAPI (RwInt32)           RwDestroyCamera ARGS((RwCamera * cam));
extern RWAPI (RwInt32)           RwDestroyClump ARGS((RwClump * clump));
extern RWAPI (void)              RwDestroyClumpOrder ARGS((RwPolyList * order));
extern RWAPI (RwBool)            RwDestroyDepthCueColor ARGS((RwDepthCueColor *col));
extern RWAPI (RwInt32)           RwDestroyLight ARGS((RwLight * lite));
extern RWAPI (RwInt32)           RwDestroyMaterial ARGS((RwMaterial * m));
extern RWAPI (RwInt32)           RwDestroyMatrix ARGS((RwMatrix4d * m));
extern RWAPI (RwInt32)           RwDestroyPolygon ARGS((RwPolygon3d * pol));
extern RWAPI (RwInt32)           RwDestroyRaster ARGS((RwRaster * ras));
extern RWAPI (RwBool)            RwDestroyScene ARGS((RwScene *));
extern RWAPI (RwInt32)           RwDestroySpline ARGS((RwSpline * spline));
extern RWAPI (RwBool)            RwDestroyStereoCamera ARGS((RwCamera * c));
extern RWAPI (RwInt32)           RwDestroyTexture ARGS((RwTexture * t));
extern RWAPI (RwInt32)           RwDestroyUserDraw ARGS((RwUserDraw * userdraw));
extern RWAPI (RwInt32)           RwDeviceControl ARGS((RwDeviceAction action,
                                                       long param1,
                                                       void *param2,
                                                       long size));
extern RWAPI (RwInt32)           RwDisc ARGS((RwReal height,
                                              RwReal radius,
                                              RwInt32 sides));
extern RWAPI (RwReal)            RwDotProduct ARGS((RwV3d * a,
                                                    RwV3d * b));
extern RWAPI (RwCamera *)        RwDuplicateCamera ARGS((RwCamera * cam,
                                                         RwRaster * image));
extern RWAPI (RwClump *)         RwDuplicateClump ARGS((RwClump * clump));
extern RWAPI (RwLight *)         RwDuplicateLight ARGS((RwLight * light));
extern RWAPI (RwMaterial *)      RwDuplicateMaterial ARGS((RwMaterial * src));
extern RWAPI (RwMatrix4d *)      RwDuplicateMatrix ARGS((RwMatrix4d * m));
extern RWAPI (RwRaster *)        RwDuplicateRaster ARGS((RwRaster * ras));
extern RWAPI (RwSpline *)        RwDuplicateSpline ARGS((RwSpline * s));
extern RWAPI (RwUserDraw *)      RwDuplicateUserDraw ARGS((RwUserDraw * userdraw));
extern RWAPI (RwCamera *)        RwEndCameraUpdate ARGS((RwCamera * cam));
extern RWAPI (RwCamera *)        RwEndStereoCameraUpdate ARGS((RwCamera * cam));
extern RWAPI (RwClump *)         RwEnvMapClump ARGS((RwClump * clump));
extern RWAPI (RwBool)            RwExtract ARGS((char *cpIn,
                                                 RwInt32 nNo,
                                                 char *cpOut,
                                                 RwInt32 nMax));
extern RWAPI (RwClump *)         RwFindClump ARGS((RwClump * clump,
                                                   RwClumpPredicateVoid predicate));
extern RWAPI (RwClump *)         RwFindClumpInt ARGS((RwClump * clump,
                                                      RwClumpPredicateInt predicate,
                                                      RwInt32 arg));
extern RWAPI (RwClump *)         RwFindClumpLong ARGS((RwClump * clump,
                                                       RwClumpPredicateLong predicate,
                                                       RwInt32 arg));
extern RWAPI (RwClump *)         RwFindClumpPointer ARGS((RwClump * clump,
                                                          RwClumpPredicatePointer predicate,
                                                          void *arg));
extern RWAPI (RwClump *)         RwFindClumpReal ARGS((RwClump * clump,
                                                       RwClumpPredicateReal predicate,
                                                       RwReal arg));
extern RWAPI (RwUInt32)          RwFindDisplayDevice ARGS((char *cpDevices,
                                                           RwDeviceCapabilities * dcpRequest,
                                                           char *cpInitString,
                                                           void *pParam,
                                                           RwInt32 nNumArgs,
                                                           RwOpenArgument * aaArgs));
extern RWAPI (RwTexture *)       RwFindNamedTexture ARGS((char *name));
extern RWAPI (RwBool)            RwFindStreamChunk ARGS((RwStream * stpStream,
                                                         RwUInt32 nType));
extern RWAPI (RwClump *)         RwFindTaggedClump ARGS((RwClump * clump,
                                                         RwInt32 tag));
extern RWAPI (RwPolygon3d *)     RwFindTaggedPolygon ARGS((RwClump * clump,
                                                           RwInt32 tag));
extern RWAPI (RwClump *)         RwForAllClumpsInHierarchy ARGS((RwClump * clump,
                                                                 RwClumpFuncVoid func));
extern RWAPI (RwClump *)         RwForAllClumpsInHierarchyInt ARGS((RwClump * clump,
                                                                    RwClumpFuncInt func,
                                                                    RwInt32 arg));
extern RWAPI (RwClump *)         RwForAllClumpsInHierarchyLong ARGS((RwClump * clump,
                                                                     RwClumpFuncLong func,
                                                                     long arg));
extern RWAPI (RwClump *)         RwForAllClumpsInHierarchyPointer ARGS((RwClump * clump,
                                                                        RwClumpFuncPointer func,
                                                                        void *arg));
extern RWAPI (RwClump *)         RwForAllClumpsInHierarchyReal ARGS((RwClump * clump,
                                                                     RwClumpFuncReal func,
                                                                     RwReal arg));
extern RWAPI (RwScene *)         RwForAllClumpsInScene ARGS((RwScene *,
                                                             RwClumpFuncVoid));
extern RWAPI (RwScene *)         RwForAllClumpsInSceneInt ARGS((RwScene *,
                                                                RwClumpFuncInt,
                                                                RwInt32));
extern RWAPI (RwScene *)         RwForAllClumpsInSceneLong ARGS((RwScene *,
                                                                 RwClumpFuncLong,
                                                                 RwInt32));
extern RWAPI (RwScene *)         RwForAllClumpsInScenePointer ARGS((RwScene *,
                                                                    RwClumpFuncPointer,
                                                                    void *));
extern RWAPI (RwScene *)         RwForAllClumpsInSceneReal ARGS((RwScene *,
                                                                 RwClumpFuncReal,
                                                                 RwReal));
extern RWAPI (RwScene *)         RwForAllLightsInScene ARGS((RwScene *,
                                                             RwLightFuncVoid));
extern RWAPI (RwScene *)         RwForAllLightsInSceneInt ARGS((RwScene *,
                                                                RwLightFuncInt,
                                                                RwInt32));
extern RWAPI (RwScene *)         RwForAllLightsInSceneLong ARGS((RwScene *,
                                                                 RwLightFuncLong,
                                                                 RwInt32));
extern RWAPI (RwScene *)         RwForAllLightsInScenePointer ARGS((RwScene *,
                                                                    RwLightFuncPointer,
                                                                    void *));
extern RWAPI (RwScene *)         RwForAllLightsInSceneReal ARGS((RwScene *,
                                                                 RwLightFuncReal,
                                                                 RwReal));
extern RWAPI (RwInt32)           RwForAllNamedTextures ARGS((RwTextureFuncVoid func));
extern RWAPI (RwInt32)           RwForAllNamedTexturesInt ARGS((RwTextureFuncInt func,
                                                                RwInt32 arg));
extern RWAPI (RwInt32)           RwForAllNamedTexturesLong ARGS((RwTextureFuncLong func,
                                                                 long arg));
extern RWAPI (RwInt32)           RwForAllNamedTexturesPointer ARGS((RwTextureFuncPointer func,
                                                                    void *arg));
extern RWAPI (RwInt32)           RwForAllNamedTexturesReal ARGS((RwTextureFuncReal func,
                                                                 RwReal arg));
extern RWAPI (RwClump *)         RwForAllPolygonsInClump ARGS((RwClump * clump,
                                                               RwPolygon3dFuncVoid func));
extern RWAPI (RwClump *)         RwForAllPolygonsInClumpInt ARGS((RwClump * clump,
                                                                  RwPolygon3dFuncInt func,
                                                                  RwInt32 arg));
extern RWAPI (RwClump *)         RwForAllPolygonsInClumpLong ARGS((RwClump * clump,
                                                                   RwPolygon3dFuncLong func,
                                                                   long arg));
extern RWAPI (RwClump *)         RwForAllPolygonsInClumpPointer ARGS((RwClump * clump,
                                                                      RwPolygon3dFuncPointer func,
                                                                      void *arg));
extern RWAPI (RwClump *)         RwForAllPolygonsInClumpReal ARGS((RwClump * clump,
                                                                   RwPolygon3dFuncReal func,
                                                                   RwReal arg));
extern RWAPI (RwClump *)         RwForAllUserDrawsInClump ARGS((RwClump * clump,
                                                                RwUserDrawFuncVoid func));
extern RWAPI (RwClump *)         RwForAllUserDrawsInClumpInt ARGS((RwClump * clump,
                                                                   RwUserDrawFuncInt func,
                                                                   RwInt32 arg));
extern RWAPI (RwClump *)         RwForAllUserDrawsInClumpLong ARGS((RwClump * clump,
                                                                    RwUserDrawFuncLong func,
                                                                    long arg));
extern RWAPI (RwClump *)         RwForAllUserDrawsInClumpPointer ARGS((RwClump * clump,
                                                                       RwUserDrawFuncPointer func,
                                                                       void *arg));
extern RWAPI (RwClump *)         RwForAllUserDrawsInClumpReal ARGS((RwClump * clump,
                                                                    RwUserDrawFuncReal func,
                                                                    RwReal arg));
extern RWAPI(RwInt32)            RwFclose ARGS((FILE *stream));
extern RWAPI(FILE *)             RwFopen ARGS((const char *filename, const char *mode ));
extern RWAPI (RwInt32)           RwFread ARGS((void *buffer, RwInt32 size, RwInt32 count, FILE *stream ));
extern RWAPI (void)              RwFree ARGS((void *ptr));
extern RWAPI (RwInt32)           RwFwrite ARGS((void *buffer, RwInt32 size, RwInt32 count, FILE *stream ));
extern RWAPI (RwRGBColor *)      RwGetCameraBackColor ARGS((RwCamera * cam,
                                                            RwRGBColor * color));
extern RWAPI (RwRaster *)        RwGetCameraBackdrop ARGS((RwCamera * cam));
extern RWAPI (RwCamera *)        RwGetCameraBackdropOffset ARGS((RwCamera * cam,
                                                                 RwInt32 * x,
                                                                 RwInt32 * y));
extern RWAPI (RwCamera *)        RwGetCameraBackdropViewportRect ARGS((RwCamera * cam,
                                                                       RwInt32 * x,
                                                                       RwInt32 * y,
                                                                       RwInt32 * w,
                                                                       RwInt32 * h));
extern RWAPI (void *)            RwGetCameraData ARGS((RwCamera * cam));
extern RWAPI (RwReal)            RwGetCameraFarClipping ARGS((RwCamera * cam));
extern RWAPI (RwCamera *)        RwGetCameraRenderOffset(RwCamera * cam, RwInt32 *x, RwInt32 *y);
extern RWAPI(RwReal)             RwGetCameraZFar ARGS((RwCamera * cam));
extern RWAPI(RwReal)             RwGetCameraZNear ARGS((RwCamera * cam));
extern RWAPI (RwRaster *)        RwGetCameraZRaster ARGS((RwCamera * cam));
extern RWAPI (RwState)           RwGetCameraZState ARGS((RwCamera * cam));
extern RWAPI (void *)            RwGetCameraImage ARGS((RwCamera * cam));
extern RWAPI (RwMatrix4d *)      RwGetCameraLTM ARGS((RwCamera * camera,
                                                      RwMatrix4d * m));
extern RWAPI (RwV3d *)           RwGetCameraLookAt ARGS((RwCamera * cam,
                                                         RwV3d * p));
extern RWAPI (RwV3d *)           RwGetCameraLookRight ARGS((RwCamera * cam,
                                                            RwV3d * p));
extern RWAPI (RwV3d *)           RwGetCameraLookUp ARGS((RwCamera * cam,
                                                         RwV3d * p));
extern RWAPI (RwReal)            RwGetCameraNearClipping ARGS((RwCamera * cam));
extern RWAPI (RwV3d *)           RwGetCameraPosition ARGS((RwCamera * cam,
                                                           RwV3d * p));
extern RWAPI (RwCameraProjection) RwGetCameraProjection ARGS((RwCamera *));
extern RWAPI (RwRaster *)        RwGetCameraRaster ARGS((RwCamera * cam));
extern RWAPI (RwV3d *)           RwGetCameraViewOffset ARGS((RwCamera * cam,
                                                             RwV3d * p));
extern RWAPI (RwCamera *)        RwGetCameraViewport ARGS((RwCamera * cam,
                                                           RwInt32 * x,
                                                           RwInt32 * y,
                                                           RwInt32 * w,
                                                           RwInt32 * h));
extern RWAPI (RwCamera *)        RwGetCameraViewwindow ARGS((RwCamera * cam,
                                                             RwReal * w,
                                                             RwReal * h));
extern RWAPI (RwUInt32)          RwGetChunkSize ARGS((RwUInt32 nType,
                                                      void *pData,
                                                      RwUInt32 nFlags));
extern RWAPI (RwAxisAlignment)   RwGetClumpAxisAlignment ARGS((RwClump * clump));
extern RWAPI (RwClump *)         RwGetClumpBBox ARGS((RwClump * clump,
                                                      RwV3d * blleft,
                                                      RwV3d * furight));
extern RWAPI (void *)            RwGetClumpData ARGS((RwClump * clump));
extern RWAPI (RwClumpHints)      RwGetClumpHints ARGS((RwClump * clump));
extern RWAPI (RwImmPolygon3d *)  RwGetClumpImmPolygon ARGS((RwClump * clump,
                                                            RwInt32 i));
extern RWAPI (RwImmVertex3d *)   RwGetClumpImmVertex ARGS((RwClump * clump,
                                                           RwInt32 i));
extern RWAPI (RwClumpCallBack)   RwGetClumpImmediateCallBack ARGS((RwClump * clump));
extern RWAPI (RwMatrix4d *)      RwGetClumpJointMatrix ARGS((RwClump * clump,
                                                             RwMatrix4d * matrix));
extern RWAPI (RwMatrix4d *)      RwGetClumpLTM ARGS((RwClump * clump,
                                                     RwMatrix4d * mat));
extern RWAPI (RwReal)            RwGetClumpLightSampleRate ARGS((RwClump * clump));
extern RWAPI (RwClump *)         RwGetClumpLocalBBox ARGS((RwClump * clump,
                                                           RwV3d * blleft,
                                                           RwV3d * furight));
extern RWAPI (RwMatrix4d *)      RwGetClumpMatrix ARGS((RwClump * clump,
                                                        RwMatrix4d * mat));
extern RWAPI (RwInt32)           RwGetClumpNumChildren ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwGetClumpNumPolygons ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwGetClumpNumUserDraws ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwGetClumpNumVertices ARGS((RwClump * clump));
extern RWAPI (RwV3d *)           RwGetClumpOrigin ARGS((RwClump *,
                                                        RwV3d * o));
extern RWAPI (RwScene *)         RwGetClumpOwner ARGS((RwClump * clump));
extern RWAPI (RwClump *)         RwGetClumpParent ARGS((RwClump * clump));
extern RWAPI (RwClump *)         RwGetClumpRoot ARGS((RwClump * clump));
extern RWAPI (RwState)           RwGetClumpState ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwGetClumpTag ARGS((RwClump * clump));
extern RWAPI (RwV3d *)           RwGetClumpVertex ARGS((RwClump * clump,
                                                        RwInt32 vindex,
                                                        RwV3d * vertex));
extern RWAPI (RwV3d *)           RwGetClumpVertexNormal ARGS((RwClump * clump,
                                                              RwInt32 vindex,
                                                              RwV3d * normal));
extern RWAPI (RwUV *)            RwGetClumpVertexUV ARGS((RwClump * clump,
                                                          RwInt32 vindex,
                                                          RwUV * uv));
extern RWAPI (RwBool)            RwGetClumpVertexViewportPosition ARGS((RwClump * clump,
                                                                        RwInt32 vindex,
                                                                        RwCamera * camera,
                                                                        RwInt32 * px,
                                                                        RwInt32 * py,
                                                                        RwBool * visible));
extern RWAPI (RwClump *)         RwGetClumpViewportRect ARGS((RwClump * clump,
                                                              RwCamera * cam,
                                                              RwInt32 * x,
                                                              RwInt32 * y,
                                                              RwInt32 * w,
                                                              RwInt32 * h));
extern RWAPI (RwDebugState)      RwGetDebugAssertionState ARGS((void));
extern RWAPI (RwDebugState)      RwGetDebugMessageState ARGS((void));
extern RWAPI (RwDebugState)      RwGetDebugScriptState ARGS((void));
extern RWAPI (RwDebugSeverity)   RwGetDebugSeverity ARGS((void));
extern RWAPI (RwDebugState)      RwGetDebugTraceState ARGS((void));
extern RWAPI (RwReal)            RwGetDepthCueDistance ARGS((void));
extern RWAPI (RwDepthCueType)    RwGetDepthCueType ARGS((void));
extern RWAPI (RwInt32)           RwGetDeviceInfo ARGS((RwDeviceInfo info,
                                                       void *value,
                                                       long size));
extern RWAPI (char *)            RwGetDisplayDevices ARGS((void));
extern RWAPI (RwClump *)         RwGetFirstChildClump ARGS((RwClump * clump));
extern RWAPI (RwCamera *)        RwGetLeftStereoCamera ARGS((RwCamera * c));
extern RWAPI (RwReal)            RwGetLightBrightness ARGS((RwLight * l));
extern RWAPI (RwRGBColor *)      RwGetLightColor ARGS((RwLight * l,
                                                       RwRGBColor * color));
extern RWAPI (RwReal)            RwGetLightConeAngle ARGS((RwLight * l));
extern RWAPI (void *)            RwGetLightData ARGS((RwLight * lite));
extern RWAPI (RwMatrix4d *)      RwGetLightLTM ARGS((RwLight * light,
                                                     RwMatrix4d * m));
extern RWAPI (RwScene *)         RwGetLightOwner ARGS((RwLight * l));
extern RWAPI (RwV3d *)           RwGetLightPosition ARGS((RwLight * lite,
                                                          RwV3d * p));
extern RWAPI (RwReal)            RwGetLightRadius ARGS((RwLight * l));
extern RWAPI (RwState)           RwGetLightState ARGS((RwLight * lite));
extern RWAPI (RwLightType)       RwGetLightType ARGS((RwLight * l));
extern RWAPI (RwV3d *)           RwGetLightVector ARGS((RwLight * l,
                                                        RwV3d * v));
extern RWAPI (RwReal)            RwGetMaterialAmbient ARGS((RwMaterial * m));
extern RWAPI (RwRGBColor *)      RwGetMaterialAmbientRGB ARGS((RwMaterial * m,
                                                              RwRGBColor * ka));
extern RWAPI (RwRGBColor *)      RwGetMaterialColor ARGS((RwMaterial * m,
                                                          RwRGBColor * color));
extern RWAPI (void *)            RwGetMaterialData ARGS((RwMaterial * m));
extern RWAPI (RwReal)            RwGetMaterialDiffuse ARGS((RwMaterial * m));
extern RWAPI (RwRGBColor *)      RwGetMaterialDiffuseRGB ARGS((RwMaterial * m,
                                                              RwRGBColor * kd));
extern RWAPI (RwGeometrySampling) RwGetMaterialGeometrySampling ARGS((RwMaterial * m));
extern RWAPI (RwLightSampling)   RwGetMaterialLightSampling ARGS((RwMaterial * m));
extern RWAPI (RwMaterialModes)   RwGetMaterialModes ARGS((RwMaterial * m));
extern RWAPI (RwReal)            RwGetMaterialOpacity ARGS((RwMaterial * m));
extern RWAPI (RwReal)            RwGetMaterialSpecular ARGS((RwMaterial * m));
extern RWAPI (RwRGBColor *)      RwGetMaterialSpecularRGB ARGS((RwMaterial * m,
                                                              RwRGBColor * ks));
extern RWAPI (RwTexture *)       RwGetMaterialTexture ARGS((RwMaterial * m));
extern RWAPI (RwTextureModes)    RwGetMaterialTextureModes ARGS((RwMaterial * m));
extern RWAPI (RwReal)            RwGetMatrixElement ARGS((RwMatrix4d * m,
                                                          RwInt32 i,
                                                          RwInt32 j));
extern RWAPI (RwReal *)          RwGetMatrixElements ARGS((RwMatrix4d * m,
                                                           RwReal farray[4][4]));
extern RWAPI (RwTexture *)       RwGetNamedTexture ARGS((char *name));
extern RWAPI (RwClump *)         RwGetNextClump ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwGetNumNamedTextures ARGS((void));
extern RWAPI (RwPaletteEntry *)  RwGetPaletteEntries ARGS((RwInt32 n,
                                                           RwInt32 length,
                                                           RwPaletteEntry * rgb));
extern RWAPI (RwReal)            RwGetPolygonAmbient ARGS((RwPolygon3d * pol));
extern RWAPI (RwRGBColor *)      RwGetPolygonAmbientRGB ARGS((RwPolygon3d * pol,
                                                              RwRGBColor * Ka));
extern RWAPI (RwV3d *)           RwGetPolygonCenter ARGS((RwPolygon3d * pol,
                                                          RwV3d * p));
extern RWAPI (RwRGBColor *)      RwGetPolygonColor ARGS((RwPolygon3d * pol,
                                                         RwRGBColor * color));
extern RWAPI (void *)            RwGetPolygonData ARGS((RwPolygon3d * pol));
extern RWAPI (RwReal)            RwGetPolygonDiffuse ARGS((RwPolygon3d * pol));
extern RWAPI (RwRGBColor *)      RwGetPolygonDiffuseRGB ARGS((RwPolygon3d * pol,
                                                              RwRGBColor * Kd));
extern RWAPI (RwGeometrySampling) RwGetPolygonGeometrySampling ARGS((RwPolygon3d * p));
extern RWAPI (RwLightSampling)   RwGetPolygonLightSampling ARGS((RwPolygon3d * apol));
extern RWAPI (RwMaterial *)      RwGetPolygonMaterial ARGS((RwPolygon3d * p));
extern RWAPI (RwMaterialModes)   RwGetPolygonMaterialModes ARGS((RwPolygon3d * apol));
extern RWAPI (RwV3d *)           RwGetPolygonNormal ARGS((RwPolygon3d * pol,
                                                          RwV3d * p));
extern RWAPI (RwInt32)           RwGetPolygonNumSides ARGS((RwPolygon3d * p));
extern RWAPI (RwReal)            RwGetPolygonOpacity ARGS((RwPolygon3d * p));
extern RWAPI (RwClump *)         RwGetPolygonOwner ARGS((RwPolygon3d * pol));
extern RWAPI (RwReal)            RwGetPolygonSpecular ARGS((RwPolygon3d * pol));
extern RWAPI (RwRGBColor *)      RwGetPolygonSpecularRGB ARGS((RwPolygon3d *pol,
                                                               RwRGBColor *Ks));
extern RWAPI (RwInt32)           RwGetPolygonTag ARGS((RwPolygon3d * pol));
extern RWAPI (RwTexture *)       RwGetPolygonTexture ARGS((RwPolygon3d * p));
extern RWAPI (RwTextureModes)    RwGetPolygonTextureModes ARGS((RwPolygon3d * apol));
extern RWAPI (RwUV *)            RwGetPolygonUV ARGS((RwPolygon3d * p,
                                                      RwUV * uv));
extern RWAPI (RwInt32)           RwGetPolygonVertices ARGS((RwPolygon3d * p,
                                                            RwInt32 * varray));
extern RWAPI (void *)            RwGetRasterData ARGS((RwRaster * ras));
extern RWAPI (RwInt32)           RwGetRasterDepth ARGS((RwRaster * ras));
extern RWAPI (RwInt32)           RwGetRasterHeight ARGS((RwRaster * ras));
extern RWAPI (RwRaster *)        RwGetRasterInfo ARGS((RwRaster * ras, RwRasterInfo * info));
extern RWAPI (unsigned char *)   RwGetRasterPixels ARGS((RwRaster * ras));
extern RWAPI (RwInt32)           RwGetRasterStride ARGS((RwRaster * ras));
extern RWAPI (RwInt32)           RwGetRasterWidth ARGS((RwRaster * ras));
extern RWAPI (RwCamera *)        RwGetRightStereoCamera ARGS((RwCamera * c));
extern RWAPI (void *)            RwGetSceneData ARGS((RwScene *));
extern RWAPI (RwInt32)           RwGetSceneNumClumps ARGS((RwScene *));
extern RWAPI (RwInt32)           RwGetSceneNumLights ARGS((RwScene *));
extern RWAPI (char *)            RwGetShapePath ARGS((char *path));
extern RWAPI (void *)            RwGetSplineData ARGS((RwSpline * spline));
extern RWAPI (RwInt32)           RwGetSplineNumPoints ARGS((RwSpline * sp));
extern RWAPI (RwV3d *)           RwGetSplinePoint ARGS((RwSpline * sp,
                                                        RwInt32 control,
                                                        RwV3d * point));
extern RWAPI (RwReal)            RwGetStereoCameraEyeSeparation ARGS((RwCamera * c));
extern RWAPI (RwReal)            RwGetStereoCameraFocalDistance ARGS((RwCamera * c));
extern RWAPI (RwStereoMode)      RwGetStereoCameraMode ARGS((RwCamera * c));
extern RWAPI (RwInt32)           RwGetSystemInfo ARGS((RwSystemInfo info,
                                                       void *value,
                                                       long size));
extern RWAPI (void *)            RwGetTextureData ARGS((RwTexture * texture));
extern RWAPI (RwSearchMode)      RwGetTextureDictSearchMode ARGS((void));
extern RWAPI (RwTextureDitherMode) RwGetTextureDithering ARGS((void));
extern RWAPI (RwInt32)           RwGetTextureFrame ARGS((RwTexture * t));
extern RWAPI (RwInt32)           RwGetTextureFrameStep ARGS((RwTexture * t));
extern RWAPI (RwState)           RwGetTextureGammaCorrection ARGS((void));
extern RWAPI (RwRaster *)        RwGetTextureMipmapRaster ARGS((RwTexture * texture));
extern RWAPI (RwState)           RwGetTextureMipmapState ARGS((void));
extern RWAPI (char *)            RwGetTextureName ARGS((RwTexture * texture,
                                                        char *name,
                                                        long size));
extern RWAPI (RwInt32)           RwGetTextureNumFrames ARGS((RwTexture * t));
extern RWAPI (RwRaster *)        RwGetTextureRaster ARGS((RwTexture * texture));
extern RWAPI (RwUserDrawAlignmentTypes) RwGetUserDrawAlignment ARGS((RwUserDraw * userdraw));
extern RWAPI (RwUserDrawCallBack) RwGetUserDrawCallback ARGS((RwUserDraw * userdraw));
extern RWAPI (void *)            RwGetUserDrawData ARGS((RwUserDraw * userdraw));
extern RWAPI (RwUserDraw *)      RwGetUserDrawOffset ARGS((RwUserDraw * userdraw,
                                                           RwInt32 * x,
                                                           RwInt32 * y));
extern RWAPI (RwClump *)         RwGetUserDrawOwner ARGS((RwUserDraw * userdraw));
extern RWAPI (RwUserDrawAlignmentTypes) RwGetUserDrawParentAlignment ARGS((RwUserDraw *
                                                                 userdraw));
extern RWAPI (RwUserDraw *)      RwGetUserDrawSize ARGS((RwUserDraw * userdraw,
                                                         RwInt32 * width,
                                                         RwInt32 * height));
extern RWAPI (RwUserDrawType)    RwGetUserDrawType ARGS((RwUserDraw * userdraw));
extern RWAPI (RwInt32)           RwGetUserDrawVertexIndex ARGS((RwUserDraw * userdraw));
extern RWAPI (RwInt32)           RwHemisphere ARGS((RwReal radius,
                                                    RwInt32 sides));
extern RWAPI (RwInt32)           RwIdentityCTM ARGS((void));
extern RWAPI (RwInt32)           RwIdentityJointTM ARGS((void));
extern RWAPI (RwMatrix4d *)      RwIdentityMatrix ARGS((RwMatrix4d * m));
extern RWAPI (void)              RwImmVertexPixelSpace ARGS((RwImmVertex3d * ivpV));
extern RWAPI (RwFixed)           RwImmZBufferDepth ARGS((RwCamera * cam,
                                                         RwReal depth));
extern RWAPI (void)              RwImmediateBegin ARGS((RwRect * rect));
extern RWAPI (void)              RwImmediateEnd ARGS((void));
extern RWAPI (RwInt32)           RwInclude ARGS((RwClump * clump));
extern RWAPI (RwInt32)           RwIncludeGeometry ARGS((RwClump * clump));
extern RWAPI (RwBool)            RwInitialize ARGS((RwStdlib *stdlib));
extern RWAPI (RwCamera *)        RwInvalidateCameraViewport ARGS((RwCamera * cam));
extern RWAPI (RwMatrix4d *)      RwInvertMatrix ARGS((RwMatrix4d * src,
                                                      RwMatrix4d * dst));
extern RWAPI (RwInt32)           RwJointTransformBegin ARGS((void));
extern RWAPI (RwInt32)           RwJointTransformEnd ARGS((void));
extern RWAPI (RwTexture *)       RwMaskTexture ARGS((RwTexture * t,
                                                     RwRaster * mask));
extern RWAPI (void *)            RwMalloc ARGS((RwInt32 size));
extern RWAPI (RwInt32)           RwMaterialBegin ARGS((void));
extern RWAPI (RwInt32)           RwMaterialEnd ARGS((void));
extern RWAPI (RwInt32)           RwModelBegin ARGS((void));
extern RWAPI (RwInt32)           RwModelEnd ARGS((void));
extern RWAPI (RwMatrix4d *)      RwMultiplyMatrix ARGS((RwMatrix4d * a,
                                                        RwMatrix4d * b,
                                                        RwMatrix4d * dst));
extern RWAPI (RwV3d *)           RwNormalize ARGS((RwV3d * vector));
extern RWAPI (RwClump *)         RwNormalizeClump ARGS((RwClump * clump));
extern RWAPI (RwBool)            RwOpen ARGS((char *name,
                                              void *param));
extern RWAPI (RwInt32)           RwOpenDebugStream ARGS((char *filename));
extern RWAPI (RwDisplayDevice *) RwOpenDisplayDevice ARGS((char *cpDevice,
                                                           char *cpInitString));
extern RWAPI (RwBool)            RwOpenExt ARGS((char *name,
                                                 void *param,
                                                 RwInt32 numargs,
                                                 RwOpenArgument * args));
extern RWAPI (RwStream *)        RwOpenStream ARGS((RwStreamType stType,
                                                    RwStreamAccessType saAccess,
                                                    void *pData));
extern RWAPI (RwMatrix4d *)      RwOrthoNormalizeMatrix ARGS((RwMatrix4d * src,
                                                              RwMatrix4d * dst));
extern RWAPI (RwCamera *)        RwPanCamera ARGS((RwCamera * cam,
                                                   RwReal theta));
extern RWAPI (RwPickRecord *)    RwPickClump ARGS((RwClump * clump,
                                                   RwInt32 vpx,
                                                   RwInt32 vpy,
                                                   RwCamera * cam,
                                                   RwPickRecord * record));
extern RWAPI (RwPickRecord *)    RwPickScene ARGS((RwScene *,
                                                   RwInt32 x,
                                                   RwInt32 y,
                                                   RwCamera *,
                                                   RwPickRecord *));
extern RWAPI (RwCamera *)        RwPointCamera ARGS((RwCamera * cam,
                                                     RwReal x,
                                                     RwReal y,
                                                     RwReal z));
extern RWAPI (RwInt32)           RwPolygon ARGS((RwInt32 sides,
                                                 RwInt32 * varray));
extern RWAPI (RwInt32)           RwPolygonExt ARGS((RwInt32 sides,
                                                    RwInt32 * varray,
                                                    RwInt32 tag));
extern RWAPI (RwMaterial *)      RwPopCurrentMaterial ARGS((void));
extern RWAPI (RwMatrix4d *)      RwPopScratchMatrix ARGS((void));
extern RWAPI (RwInt32)           RwProtoBegin ARGS((char *name));
extern RWAPI (RwInt32)           RwProtoEnd ARGS((void));
extern RWAPI (RwInt32)           RwProtoInstance ARGS((char *name));
extern RWAPI (RwInt32)           RwProtoInstanceGeometry ARGS((char *name));
extern RWAPI (RwMaterial *)      RwPushCurrentMaterial ARGS((void));
extern RWAPI (RwMatrix4d *)      RwPushScratchMatrix ARGS((void));
extern RWAPI (RwInt32)           RwQuad ARGS((RwInt32 v1,
                                              RwInt32 v2,
                                              RwInt32 v3,
                                              RwInt32 v4));
extern RWAPI (RwInt32)           RwQuadExt ARGS((RwInt32 v1,
                                                 RwInt32 v2,
                                                 RwInt32 v3,
                                                 RwInt32 v4,
                                                 RwInt32 tag));
extern RWAPI (RwMatrix4d *)      RwQueryRotateMatrix ARGS((RwMatrix4d * mat,
                                                           RwV3d * axis,
                                                           RwReal * degrees,
                                                           RwV3d * center));
extern RWAPI (long)              RwRandom ARGS((void));
extern RWAPI (RwRaster *)        RwReadMaskRaster ARGS((char *name));
extern RWAPI (RwTexture *)       RwReadNamedTexture ARGS((char *name));
extern RWAPI (RwRaster *)        RwReadRaster ARGS((char *name,
                                                    RwRasterOptions options));
extern RWAPI (RwClump *)         RwReadShape ARGS((char *filename));
extern RWAPI (RwBool)            RwReadStream ARGS((RwStream * stpStream,
                                                    void *pBuffer,
                                                    RwUInt32 nLength));
extern RWAPI (RwBool)            RwReadStreamChunk ARGS((RwStream * stpStream,
                                                         RwUInt32 nType,
                                                         void *pData,
                                                         RwUInt32 nFlags));
extern RWAPI (RwInt32)           RwReadStreamChunkHeader ARGS((RwStream * stpStream));
extern RWAPI (RwBool)            RwReadStreamChunkType ARGS((RwStream * stpStream,
                                                             RwUInt32 * npType));
extern RWAPI (RwBool)            RwReadStreamInt ARGS((RwStream * stpStream,
                                                       RwInt32 * npInt,
                                                       RwInt32 nBytes));
extern RWAPI (RwBool)            RwReadStreamReal ARGS((RwStream * stpStream,
                                                        RwReal * npReal,
                                                        RwInt32 nBytes));
extern RWAPI (RwTexture *)       RwReadTexture ARGS((char *name));
extern RWAPI (void *)            RwRealloc ARGS((void *ptr, RwInt32 size));
extern RWAPI (void)              RwRelease ARGS((void));
extern RWAPI (RwRaster *)        RwReleaseRasterPixels ARGS((RwRaster * ras,
                                                             unsigned char *pixels));
extern RWAPI (RwClump *)         RwRemoveChildFromClump ARGS((RwClump * clump));
extern RWAPI (RwClump *)         RwRemoveClumpFromScene ARGS((RwClump *));
extern RWAPI (RwInt32)           RwRemoveHint ARGS((RwClumpHints hint));
extern RWAPI (RwClump *)         RwRemoveHintFromClump ARGS((RwClump * clump,
                                                             RwClumpHints hint));
extern RWAPI (RwLight *)         RwRemoveLightFromScene ARGS((RwLight *));
extern RWAPI (RwMaterial *)      RwRemoveMaterialModeFromMaterial ARGS((RwMaterial * m,
                                                                        RwMaterialModes mode));
extern RWAPI (RwPolygon3d *)     RwRemoveMaterialModeFromPolygon ARGS((RwPolygon3d * apol,
                                                                       RwMaterialModes mode));
extern RWAPI (RwInt32)           RwRemoveMaterialModeFromSurface ARGS((RwMaterialModes mode));
extern RWAPI (RwMaterial *)      RwRemoveTextureModeFromMaterial ARGS((RwMaterial * m,
                                                                       RwTextureModes mode));
extern RWAPI (RwPolygon3d *)     RwRemoveTextureModeFromPolygon ARGS((RwPolygon3d * apol,
                                                                      RwTextureModes mode));
extern RWAPI (RwInt32)           RwRemoveTextureModeFromSurface ARGS((RwTextureModes mode));
extern RWAPI (RwUserDraw *)      RwRemoveUserDrawFromClump ARGS((RwUserDraw * userdraw));
extern RWAPI (RwClump *)         RwRenderClump ARGS((RwClump * clump));
extern RWAPI (void)              RwRenderImmediateClump ARGS((void));
extern RWAPI (void)              RwRenderImmediateLine ARGS((RwImmediate * imm));
extern RWAPI (void)              RwRenderImmediateTriangle ARGS((RwImmediate * imm));
extern RWAPI (RwScene *)         RwRenderScene ARGS((RwScene *));
extern RWAPI (RwClump *)         RwRenderStereoClump ARGS((RwClump * c));
extern RWAPI (RwScene *)         RwRenderStereoScene ARGS((RwScene * s));
extern RWAPI (RwCamera *)        RwResetCamera ARGS((RwCamera * cam));
extern RWAPI (RwPolygon3d *)     RwReversePolygonFace ARGS((RwPolygon3d * pol));
extern RWAPI (RwCamera *)        RwRevolveCamera ARGS((RwCamera * cam,
                                                       RwReal theta));
extern RWAPI (RwInt32)           RwRotateCTM ARGS((RwReal ax,
                                                   RwReal ay,
                                                   RwReal az,
                                                   RwReal theta));
extern RWAPI (RwInt32)           RwRotateJointTM ARGS((RwReal ax,
                                                       RwReal ay,
                                                       RwReal az,
                                                       RwReal theta));
extern RWAPI (RwMatrix4d *)      RwRotateMatrix ARGS((RwMatrix4d * m,
                                                      RwReal x,
                                                      RwReal y,
                                                      RwReal z,
                                                      RwReal theta,
                                                      RwCombineOperation op));
extern RWAPI (RwMatrix4d *)      RwRotateMatrixCos ARGS((RwMatrix4d * m,
                                                         RwReal x,
                                                         RwReal y,
                                                         RwReal z,
                                                         RwReal ctheta,
                                                         RwReal direction,
                                                         RwCombineOperation op));
extern RWAPI (void)              RwSRandom ARGS((unsigned long x));
extern RWAPI (RwInt32)           RwScaleCTM ARGS((RwReal sx,
                                                  RwReal sy,
                                                  RwReal sz));
extern RWAPI (RwMatrix4d *)      RwScaleMatrix ARGS((RwMatrix4d * m,
                                                     RwReal sx,
                                                     RwReal sy,
                                                     RwReal sz,
                                                     RwCombineOperation op));
extern RWAPI (RwV3d *)           RwScaleVector ARGS((RwV3d * a,
                                                     RwReal scale,
                                                     RwV3d * c));
extern RWAPI (RwMatrix4d *)      RwScratchMatrix ARGS((void));
extern RWAPI (RwBool)            RwSeekStream ARGS((RwStream * stpStream,
                                                    RwUInt32 offset));
extern RWAPI (RwInt32)           RwSetAxisAlignment ARGS((RwAxisAlignment alignment));
extern RWAPI (RwCamera *)        RwSetCameraBackColor ARGS((RwCamera * cam,
                                                            RwReal r,
                                                            RwReal g,
                                                            RwReal b));
extern RWAPI (RwCamera *)        RwSetCameraBackColorStruct ARGS((RwCamera * cam,
                                                                  RwRGBColor * color));
extern RWAPI (RwCamera *)        RwSetCameraBackdrop ARGS((RwCamera * cam,
                                                           RwRaster * raster));
extern RWAPI (RwCamera *)        RwSetCameraBackdropOffset ARGS((RwCamera * cam,
                                                                 RwInt32 x,
                                                                 RwInt32 y));
extern RWAPI (RwCamera *)        RwSetCameraBackdropViewportRect ARGS((RwCamera * cam,
                                                                       RwInt32 x,
                                                                       RwInt32 y,
                                                                       RwInt32 w,
                                                                       RwInt32 h));
extern RWAPI (RwCamera *)        RwSetCameraData ARGS((RwCamera * cam,
                                                       void *data));
extern RWAPI (RwCamera *)        RwSetCameraFarClipping ARGS((RwCamera * cam,
                                                              RwReal fard));
extern RWAPI (RwCamera *)        RwSetCameraZState ARGS((RwCamera * cam, RwState state));
extern RWAPI (RwCamera *)        RwSetCameraLookAt ARGS((RwCamera * cam,
                                                         RwReal x,
                                                         RwReal y,
                                                         RwReal z));
extern RWAPI (RwCamera *)        RwSetCameraLookUp ARGS((RwCamera * cam,
                                                         RwReal x,
                                                         RwReal y,
                                                         RwReal z));
extern RWAPI (RwCamera *)        RwSetCameraNearClipping ARGS((RwCamera * cam,
                                                               RwReal neard));
extern RWAPI (RwCamera *)        RwSetCameraPosition ARGS((RwCamera * cam,
                                                           RwReal x,
                                                           RwReal y,
                                                           RwReal z));
extern RWAPI (RwCamera *)        RwSetCameraProjection ARGS((RwCamera * cam,
                                                             RwCameraProjection kind));
extern RWAPI (RwCamera *)        RwSetCameraRenderOffset(RwCamera * cam, RwInt32 x, RwInt32 y);
extern RWAPI (RwCamera *)        RwSetCameraViewOffset ARGS((RwCamera * cam,
                                                             RwReal x,
                                                             RwReal y));
extern RWAPI (RwCamera *)        RwSetCameraViewport ARGS((RwCamera * cam,
                                                           RwInt32 x,
                                                           RwInt32 y,
                                                           RwInt32 w,
                                                           RwInt32 h));
extern RWAPI (RwCamera *)        RwSetCameraViewwindow ARGS((RwCamera * cam,
                                                             RwReal w,
                                                             RwReal h));
extern RWAPI (RwClump *)         RwSetClumpAxisAlignment ARGS((RwClump * clump,
                                                               RwAxisAlignment alignment));
extern RWAPI (RwClump *)         RwSetClumpData ARGS((RwClump * clump,
                                                      void *data));
extern RWAPI (RwClump *)         RwSetClumpHints ARGS((RwClump * clump,
                                                       RwClumpHints hints));
extern RWAPI (RwClump *)         RwSetClumpImmediateCallBack ARGS((RwClump * clump,
                                                                   RwClumpCallBack callback));
extern RWAPI (RwReal)            RwSetClumpLightSampleRate ARGS((RwClump * clump,
                                                                 RwReal rate));
extern RWAPI (RwClump *)         RwSetClumpLocalBBox ARGS((RwClump * clump,
                                                           RwV3d * bll,
                                                           RwV3d * fur));
extern RWAPI (RwPolyList *)      RwSetClumpOrder ARGS((RwClump * clump,
                                                       RwPolyList * order));
extern RWAPI (RwClump *)         RwSetClumpState ARGS((RwClump * clump,
                                                       RwState onoff));
extern RWAPI (RwClump *)         RwSetClumpTag ARGS((RwClump * clump,
                                                     RwInt32 tag));
extern RWAPI (RwClump *)         RwSetClumpVertex ARGS((RwClump * clump,
                                                        RwInt32 vindex,
                                                        RwV3d * vertex));
extern RWAPI (RwClump *)         RwSetClumpVertexNormal ARGS((RwClump * clump,
                                                              RwInt32 vindex,
                                                              RwV3d * normal));
extern RWAPI (RwClump *)         RwSetClumpVertexUV ARGS((RwClump * clump,
                                                          RwInt32 vindex,
                                                          RwReal u,
                                                          RwReal v));
extern RWAPI (RwClump *)         RwSetClumpVertices ARGS((RwClump * clump,
                                                          RwInt32 * vindex,
                                                          RwV3d * vertex,
                                                          RwInt32 n));
extern RWAPI (void)              RwSetDebugAssertionState ARGS((RwDebugState state));
extern RWAPI (void)              RwSetDebugMessageState ARGS((RwDebugState state));
extern RWAPI (void)              RwSetDebugOutputState ARGS((RwDebugState state));
extern RWAPI (void)              RwSetDebugScriptState ARGS((RwDebugState state));
extern RWAPI (void)              RwSetDebugSeverity ARGS((RwDebugSeverity severity));
extern RWAPI (RwInt32)           RwSetDebugStream ARGS((FILE * stream));
extern RWAPI (void)              RwSetDebugTraceState ARGS((RwDebugState state));
extern RWAPI (RwBool)            RwSetDepthCueColor ARGS((RwDepthCueColor *col));
extern RWAPI (RwBool)            RwSetDepthCueDistance ARGS((RwReal d));
extern RWAPI (RwBool)            RwSetDepthCueType ARGS((RwDepthCueType t));
extern RWAPI (RwInt32)           RwSetHints ARGS((RwClumpHints hints));
extern RWAPI (void)              RwSetImmediateZRange ARGS((RwReal nNear,
                                                            RwReal nFar));
extern RWAPI (RwLight *)         RwSetLightBrightness ARGS((RwLight * lite,
                                                            RwReal lum));
extern RWAPI (RwLight *)         RwSetLightColor ARGS((RwLight * l,
                                                       RwReal r,
                                                       RwReal g,
                                                       RwReal b));
extern RWAPI (RwLight *)         RwSetLightColorStruct ARGS((RwLight * l,
                                                             RwRGBColor * color));
extern RWAPI (RwLight *)         RwSetLightConeAngle ARGS((RwLight * lite,
                                                           RwReal theta));
extern RWAPI (RwLight *)         RwSetLightData ARGS((RwLight * lite,
                                                      void *data));
extern RWAPI (RwLight *)         RwSetLightPosition ARGS((RwLight * lite,
                                                          RwReal x,
                                                          RwReal y,
                                                          RwReal z));
extern RWAPI (RwLight *)         RwSetLightRadius ARGS((RwLight * l,
                                                        RwReal rad));
extern RWAPI (RwLight *)         RwSetLightState ARGS((RwLight * lite,
                                                   RwState state));
extern RWAPI (RwLight *)         RwSetLightVector ARGS((RwLight * lite,
                                                        RwReal x,
                                                        RwReal y,
                                                        RwReal z));
extern RWAPI (RwMaterial *)      RwSetMaterialAmbient ARGS((RwMaterial * m,
                                                            RwReal ka));
extern RWAPI (RwMaterial *)      RwSetMaterialAmbientRGB ARGS((RwMaterial * m,
                                                               RwReal kar,
                                                               RwReal kag,
                                                               RwReal kab));
extern RWAPI (RwMaterial *)      RwSetMaterialAmbientRGBStruct  ARGS((RwMaterial * m, RwRGBColor * ka));
extern RWAPI (RwMaterial *)      RwSetMaterialColor ARGS((RwMaterial * m,
                                                          RwReal r,
                                                          RwReal g,
                                                          RwReal b));
extern RWAPI (RwMaterial *)      RwSetMaterialColorStruct ARGS((RwMaterial * m,
                                                                RwRGBColor * color));
extern RWAPI (RwMaterial *)      RwSetMaterialData ARGS((RwMaterial * m,
                                                         void *data));
extern RWAPI (RwMaterial *)      RwSetMaterialDiffuse ARGS((RwMaterial * m,
                                                            RwReal kd));
extern RWAPI (RwMaterial *)      RwSetMaterialDiffuseRGB ARGS((RwMaterial * m,
                                                               RwReal kdr,
                                                               RwReal kdg,
                                                               RwReal kdb));
extern RWAPI (RwMaterial *)      RwSetMaterialDiffuseRGBStruct ARGS((RwMaterial * m, RwRGBColor * kd));
extern RWAPI (RwMaterial *)      RwSetMaterialGeometrySampling ARGS((RwMaterial * m,
                                                                     RwGeometrySampling t));
extern RWAPI (RwMaterial *)      RwSetMaterialLightSampling ARGS((RwMaterial * m,
                                                                  RwLightSampling t));
extern RWAPI (RwMaterial *)      RwSetMaterialModes ARGS((RwMaterial * m,
                                                          RwMaterialModes modes));
extern RWAPI (RwMaterial *)      RwSetMaterialOpacity ARGS((RwMaterial * m,
                                                            RwReal opacity));
extern RWAPI (RwMaterial *)      RwSetMaterialSpecular ARGS((RwMaterial * m,
                                                             RwReal ks));
extern RWAPI (RwMaterial *)      RwSetMaterialSpecularRGB ARGS((RwMaterial * m,
                                                                RwReal ksr,
                                                                RwReal ksg,
                                                                RwReal ksb));
extern RWAPI (RwMaterial *)      RwSetMaterialSpecularRGBStruct ARGS((RwMaterial * m, RwRGBColor * ks));
extern RWAPI (RwMaterial *)      RwSetMaterialSurface ARGS((RwMaterial * m,
                                                            RwReal ka,
                                                            RwReal kd,
                                                            RwReal ks));
extern RWAPI (RwMaterial *)      RwSetMaterialTexture ARGS((RwMaterial * m,
                                                            RwTexture * what));
extern RWAPI (RwMaterial *)      RwSetMaterialTextureModes ARGS((RwMaterial * m,
                                                                 RwTextureModes modes));
extern RWAPI (RwMatrix4d *)      RwSetMatrixElement ARGS((RwMatrix4d * m,
                                                          RwInt32 i,
                                                          RwInt32 j,
                                                          RwReal val));
extern RWAPI (RwMatrix4d *)      RwSetMatrixElements ARGS((RwMatrix4d * m,
                                                           RwReal farray[4][4]));
extern RWAPI (RwInt32)           RwSetPaletteEntries ARGS((RwInt32 n,
                                                           RwInt32 length,
                                                           RwPaletteEntry * rgb,
                                                           RwPaletteOptions options));
extern RWAPI (RwPolygon3d *)     RwSetPolygonAmbient ARGS((RwPolygon3d * pol,
                                                           RwReal Ka));
extern RWAPI (RwPolygon3d *)     RwSetPolygonAmbientRGB ARGS((RwPolygon3d * pol,
                                                              RwReal Kar,
                                                              RwReal Kag,
                                                              RwReal Kab));
extern RWAPI (RwPolygon3d *)     RwSetPolygonAmbientRGBStruct ARGS((RwPolygon3d * pol, RwRGBColor * Ka));
extern RWAPI (RwPolygon3d *)     RwSetPolygonColor ARGS((RwPolygon3d * pol,
                                                         RwReal r,
                                                         RwReal g,
                                                         RwReal b));
extern RWAPI (RwPolygon3d *)     RwSetPolygonColorStruct ARGS((RwPolygon3d * pol,
                                                               RwRGBColor * color));
extern RWAPI (RwPolygon3d *)     RwSetPolygonData ARGS((RwPolygon3d * pol,
                                                        void *data));
extern RWAPI (RwPolygon3d *)     RwSetPolygonDiffuse ARGS((RwPolygon3d * pol,
                                                           RwReal Kd));
extern RWAPI (RwPolygon3d *)     RwSetPolygonDiffuseRGB ARGS((RwPolygon3d * pol,
                                                              RwReal Kdr,
                                                              RwReal Kdg,
                                                              RwReal Kdb));
extern RWAPI (RwPolygon3d *)     RwSetPolygonDiffuseRGBStruct ARGS((RwPolygon3d * pol, RwRGBColor * Kd));
extern RWAPI (RwPolygon3d *)     RwSetPolygonGeometrySampling ARGS((RwPolygon3d * p,
                                                                    RwGeometrySampling t));
extern RWAPI (RwPolygon3d *)     RwSetPolygonLightSampling ARGS((RwPolygon3d * p,
                                                                 RwLightSampling t));
extern RWAPI (RwPolygon3d *)     RwSetPolygonMaterial ARGS((RwPolygon3d * p,
                                                            RwMaterial * m));
extern RWAPI (RwPolygon3d *)     RwSetPolygonMaterialModes ARGS((RwPolygon3d * apol,
                                                                 RwMaterialModes modes));
extern RWAPI (RwPolygon3d *)     RwSetPolygonOpacity ARGS((RwPolygon3d * p,
                                                           RwReal opacity));
extern RWAPI (RwPolygon3d *)     RwSetPolygonSpecular ARGS((RwPolygon3d * pol,
                                                            RwReal Ks));
extern RWAPI (RwPolygon3d *)     RwSetPolygonSpecularRGB ARGS((RwPolygon3d *pol,
                                                               RwReal Ksr,
                                                               RwReal Ksg,
                                                               RwReal Ksb));
extern RWAPI (RwPolygon3d *)     RwSetPolygonSpecularRGBStruct ARGS((RwPolygon3d * pol, RwRGBColor * Ks));
extern RWAPI (RwPolygon3d *)     RwSetPolygonSurface ARGS((RwPolygon3d * pol,
                                                           RwReal ka,
                                                           RwReal kd,
                                                           RwReal ks));
extern RWAPI (RwPolygon3d *)     RwSetPolygonTag ARGS((RwPolygon3d * pol,
                                                       RwInt32 data));
extern RWAPI (RwPolygon3d *)     RwSetPolygonTexture ARGS((RwPolygon3d * p,
                                                           RwTexture * t));
extern RWAPI (RwPolygon3d *)     RwSetPolygonTextureModes ARGS((RwPolygon3d * apol,
                                                                RwTextureModes modes));
extern RWAPI (RwPolygon3d *)     RwSetPolygonUV ARGS((RwPolygon3d * p,
                                                      RwUV * uv));
extern RWAPI (RwRaster *)        RwSetRasterData ARGS((RwRaster * ras,
                                                       void *data));
extern RWAPI (RwScene *)         RwSetSceneData ARGS((RwScene *,
                                                      void *));
extern RWAPI (RwInt32)           RwSetShapePath ARGS((char *path,
                                                      RwCombineOperation op));
extern RWAPI (RwSpline *)        RwSetSplineData ARGS((RwSpline * spline,
                                                       void *data));
extern RWAPI (RwSpline *)        RwSetSplinePoint ARGS((RwSpline * sp,
                                                        RwInt32 control,
                                                        RwV3d * point));
extern RWAPI (RwCamera *)        RwSetStereoCameraEyeSeparation ARGS((RwCamera * c,
                                                                      RwReal sep));
extern RWAPI (RwCamera *)        RwSetStereoCameraFocalDistance ARGS((RwCamera * c,
                                                                      RwReal dist));

extern RWAPI (RwCamera *)        RwSetStereoCameraMode ARGS((RwCamera * c,
                                                             RwStereoMode mode));

extern RWAPI (RwInt32)           RwSetSurface ARGS((RwReal ka,
                                                    RwReal kd,
                                                    RwReal ks));
extern RWAPI (RwInt32)           RwSetSurfaceAmbient ARGS((RwReal ka));
extern RWAPI (RwInt32)           RwSetSurfaceAmbientRGB ARGS((RwReal kar,
                                                              RwReal kag,
                                                              RwReal kab));
extern RWAPI (RwInt32)           RwSetSurfaceColor ARGS((RwReal r,
                                                         RwReal g,
                                                         RwReal b));
extern RWAPI (RwInt32)           RwSetSurfaceDiffuse ARGS((RwReal kd));
extern RWAPI (RwInt32)           RwSetSurfaceDiffuseRGB ARGS((RwReal kdr,
                                                              RwReal kdg,
                                                              RwReal kdb));
extern RWAPI (RwInt32)           RwSetSurfaceGeometrySampling ARGS((RwGeometrySampling sampling));
extern RWAPI (RwInt32)           RwSetSurfaceLightSampling ARGS((RwLightSampling sampling));
extern RWAPI (RwInt32)           RwSetSurfaceMaterialModes ARGS((RwMaterialModes modes));
extern RWAPI (RwInt32)           RwSetSurfaceOpacity ARGS((RwReal opacity));
extern RWAPI (RwInt32)           RwSetSurfaceSpecular ARGS((RwReal ks));
extern RWAPI (RwInt32)           RwSetSurfaceSpecularRGB ARGS((RwReal ksr,
                                                               RwReal ksg,
                                                               RwReal ksb));
extern RWAPI (RwInt32)           RwSetSurfaceTexture ARGS((char *name));
extern RWAPI (RwInt32)           RwSetSurfaceTextureExt ARGS((char *texturename,
                                                              char *maskname));
extern RWAPI (RwInt32)           RwSetSurfaceTextureModes ARGS((RwTextureModes modes));
extern RWAPI (RwInt32)           RwSetTag ARGS((RwInt32 tag));
extern RWAPI (RwTexture *)       RwSetTextureData ARGS((RwTexture * texture,
                                                        void *data));
extern RWAPI (RwInt32)           RwSetTextureDictSearchMode ARGS((RwSearchMode mode));
extern RWAPI (RwInt32)           RwSetTextureDithering ARGS((RwTextureDitherMode mode));
extern RWAPI (RwTexture *)       RwSetTextureFrame ARGS((RwTexture * t,
                                                         RwInt32 mindex));
extern RWAPI (RwTexture *)       RwSetTextureFrameStep ARGS((RwTexture * t,
                                                             RwInt32 val));
extern RWAPI (RwInt32)           RwSetTextureGammaCorrection ARGS((RwState state));
extern RWAPI (RwTexture *)       RwSetTextureMipmapRaster ARGS((RwTexture * texture,
                                                          RwRaster * raster));
extern RWAPI (RwInt32)           RwSetTextureMipmapState ARGS((RwState state));
extern RWAPI (RwTexture *)       RwSetTextureRaster ARGS((RwTexture * texture,
                                                          RwRaster * raster));
extern RWAPI (RwUserDraw *)      RwSetUserDrawAlignment ARGS((RwUserDraw * userdraw,
                                                              RwUserDrawAlignmentTypes align));
extern RWAPI (RwUserDraw *)      RwSetUserDrawCallback ARGS((RwUserDraw * userdraw,
                                                             RwUserDrawCallBack callback));
extern RWAPI (RwUserDraw *)      RwSetUserDrawData ARGS((RwUserDraw * userdraw,
                                                         void *data));
extern RWAPI (RwUserDraw *)      RwSetUserDrawOffset ARGS((RwUserDraw * userdraw,
                                                           RwInt32 x,
                                                           RwInt32 y));
extern RWAPI (RwUserDraw *)      RwSetUserDrawParentAlignment ARGS((RwUserDraw * userdraw,
                                                                    RwUserDrawAlignmentTypes align));
extern RWAPI (RwUserDraw *)      RwSetUserDrawSize ARGS((RwUserDraw * userdraw,
                                                         RwInt32 width,
                                                         RwInt32 height));
extern RWAPI (RwUserDraw *)      RwSetUserDrawType ARGS((RwUserDraw * userdraw,
                                                         RwUserDrawType type));
extern RWAPI (RwUserDraw *)      RwSetUserDrawVertexIndex ARGS((RwUserDraw * userdraw,
                                                                RwInt32 vindex));
extern RWAPI (RwBool)            RwSetUserRasterParameters ARGS((RwRaster * rpRas,
                                                                 RwInt32 stride,
                                                                 void *pixels));
extern RWAPI (RwImmediate *)     RwSetupImmediateLine ARGS((RwImmediate * imm));
extern RWAPI (RwImmediate *)     RwSetupImmediateTriangle ARGS((RwImmediate * imm));
extern RWAPI (RwCamera *)        RwShowCameraImage ARGS((RwCamera * cam,
                                                         void *param));
extern RWAPI (RwCamera *)        RwShowStereoCameraImage ARGS((RwCamera * c,
                                                               void *param));
extern RWAPI (RwBool)            RwSkipStreamChunk ARGS((RwStream * stpStream));
extern RWAPI (RwInt32)           RwSphere ARGS((RwReal radius,
                                                RwInt32 sides));
extern RWAPI (RwClump *)         RwSphericalTexturizeClump ARGS((RwClump * clump));
extern RWAPI (RwV3d *)           RwSplinePoint ARGS((RwSpline * spline,
                                                     RwSplinePath path,
                                                     RwReal where,
                                                     RwV3d * pt,
                                                     RwV3d * vec));
extern RWAPI (RwReal)            RwSplineTransform ARGS((RwSpline * sp,
                                                         RwSplinePath path,
                                                         RwReal where,
                                                         RwV3d * up,
                                                         RwMatrix4d * mat));
extern RWAPI (RwBool)            RwStartDisplayDevice ARGS((RwDisplayDevice * dpDevice,
                                                            void *pParam));
extern RWAPI (RwBool)            RwStartDisplayDeviceExt ARGS((RwDisplayDevice * dpDevice,
                                                               void *param,
                                                               RwInt32 numargs,
                                                               RwOpenArgument * args));
extern RWAPI (RwBool)            RwStopDisplayDevice ARGS((RwDisplayDevice * dpDevice));
extern RWAPI (RwV3d *)           RwSubtractVector ARGS((RwV3d * a,
                                                        RwV3d * b,
                                                        RwV3d * c));
extern RWAPI (RwInt32)           RwTextureDictBegin ARGS((void));
extern RWAPI (RwInt32)           RwTextureDictEnd ARGS((void));
extern RWAPI (RwTexture *)       RwTextureNextFrame ARGS((RwTexture * t));
extern RWAPI (RwCamera *)        RwTiltCamera ARGS((RwCamera * cam,
                                                    RwReal theta));
extern RWAPI (RwInt32)           RwTransformBegin ARGS((void));
extern RWAPI (RwInt32)           RwTransformCTM ARGS((RwMatrix4d * matrix));
extern RWAPI (RwCamera *)        RwTransformCamera ARGS((RwCamera * c,
                                                         RwMatrix4d * m,
                                                         RwCombineOperation op));
extern RWAPI (RwCamera *)        RwTransformCameraOrientation ARGS((RwCamera * c,
                                                                    RwMatrix4d * m));
extern RWAPI (RwClump *)         RwTransformClump ARGS((RwClump * clump,
                                                        RwMatrix4d * m,
                                                        RwCombineOperation op));
extern RWAPI (RwClump *)         RwTransformClumpJoint ARGS((RwClump * clump,
                                                             RwMatrix4d * matrix,
                                                             RwCombineOperation op));
extern RWAPI (RwInt32)           RwTransformEnd ARGS((void));
extern RWAPI (RwInt32)           RwTransformJointTM ARGS((RwMatrix4d * matrix));
extern RWAPI (RwLight *)         RwTransformLight ARGS((RwLight * light,
                                                        RwMatrix4d * m,
                                                        RwCombineOperation op));
extern RWAPI (RwMatrix4d *)      RwTransformMatrix ARGS((RwMatrix4d * m,
                                                         RwMatrix4d * mat,
                                                         RwCombineOperation op));
extern RWAPI (RwV3d *)           RwTransformPoint ARGS((RwV3d * a,
                                                        RwMatrix4d * m));
extern RWAPI (RwV3d *)           RwTransformVector ARGS((RwV3d * a,
                                                         RwMatrix4d * m));
extern RWAPI (RwInt32)           RwTranslateCTM ARGS((RwReal tx,
                                                      RwReal ty,
                                                      RwReal tz));
extern RWAPI (RwMatrix4d *)      RwTranslateMatrix ARGS((RwMatrix4d * m,
                                                         RwReal tx,
                                                         RwReal ty,
                                                         RwReal tz,
                                                         RwCombineOperation op));
extern RWAPI (RwInt32)           RwTriangle ARGS((RwInt32 v1,
                                                  RwInt32 v2,
                                                  RwInt32 v3));
extern RWAPI (RwInt32)           RwTriangleExt ARGS((RwInt32 v1,
                                                     RwInt32 v2,
                                                     RwInt32 v3,
                                                     RwInt32 tag));
extern RWAPI (RwCamera *)        RwUndamageCameraViewport ARGS((RwCamera * cam,
                                                                RwInt32 x,
                                                                RwInt32 y,
                                                                RwInt32 w,
                                                                RwInt32 h));
extern RWAPI (RwCamera *)        RwVCMoveCamera ARGS((RwCamera * cam,
                                                      RwReal x,
                                                      RwReal y,
                                                      RwReal z));
extern RWAPI (RwInt32)           RwVertex ARGS((RwReal x,
                                                RwReal y,
                                                RwReal z));
extern RWAPI (RwInt32)           RwVertexExt ARGS((RwReal x,
                                                   RwReal y,
                                                   RwReal z,
                                                   RwUV * uv,
                                                   RwV3d * normal));
extern RWAPI (RwCamera *)        RwWCMoveCamera ARGS((RwCamera * cam,
                                                      RwReal x,
                                                      RwReal y,
                                                      RwReal z));
extern RWAPI (RwInt32)           RwWriteShape ARGS((char *filename,
                                                    RwClump * clump));
extern RWAPI (RwBool)            RwWriteStream ARGS((RwStream * stpStream,
                                                     void *pBuffer,
                                                     RwUInt32 nLength));
extern RWAPI (RwBool)            RwWriteStreamChunk ARGS((RwStream * stpStream,
                                                          RwUInt32 nType,
                                                          void *pData,
                                                          RwUInt32 nFlags));
extern RWAPI (RwBool)            RwWriteStreamChunkHeader ARGS((RwStream * stpStream,
                                                                RwInt32 nType,
                                                                RwInt32 nBytes));
extern RWAPI (RwBool)            RwWriteStreamInt ARGS((RwStream * stpStream,
                                                        RwInt32 * npInt,
                                                        RwInt32 nBytes));
extern RWAPI (RwBool)            RwWriteStreamReal ARGS((RwStream * stpStream,
                                                         RwReal * npReals,
                                                         RwInt32 nBytes));
extern RWSPI(RwMatrix4d *)       _rwPlaneProjectDirectionMatrix ARGS((RwMatrix4d * m,
                                                                      RwReal normal_x,
                                                                      RwReal normal_y,
                                                                      RwReal normal_z,
                                                                      RwReal normal_w,
                                                                      RwReal direction_x,
                                                                      RwReal direction_y,
                                                                      RwReal direction_z,
                                                                      RwCombineOperation op));

extern RWSPI(RwMatrix4d *)       _rwPlaneProjectPointMatrix ARGS((RwMatrix4d * m,
                                                                  RwReal normal_x,
                                                                  RwReal normal_y,
                                                                  RwReal normal_z,
                                                                  RwReal normal_w,
                                                                  RwReal point_x,
                                                                  RwReal point_y,
                                                                  RwReal point_z,
                                                                  RwCombineOperation op));
extern RWSPI(RwClump *)          _rwSetClumpVertices ARGS((RwClump * clump,
                                                           RwInt32 * vindex,
                                                           RwV3d * vertex,
                                                           RwInt32 n));
extern RWSPI(RwV3d *)            _rwTransformHomogeneousPoint ARGS((register RwV3d * a,
                                                                    RwMatrix4d * m));




#ifdef CHECK_OVERLOADING

/* 
   These #ifdef CHECK_OVERLOADING predicated macros check macro
   overloading of new stereo camera interface on top of old mono camera
   interface:
 */

#define DEFINE_STRING(ARG)       static char * ARG = #ARG
#define CONCATENATE(PRE,POST)  DEFINE_STRING ( PRE ## POST)
#define CHECK_EXPANSION(X)  CONCATENATE(X ## _expands_to_, X)

/* 
   The CHECK_EXPANSION macro checks how expanded macros are resolved.
   For example, 
   CHECK_EXPANSION(RwCreateCamera);
   expands to
   * with  -D__OLSTEREOH__
   static char * RwCreateCamera_expands_to_RwStereoCreateCamera = 
   "RwCreateCamera_expands_to_RwStereoCreateCamera";
   which generates a warning of the form
   `RwCreateCamera_expands_to_RwStereoCreateCamera' defined but not used
   * with  -U__OLSTEREOH__
   static char * RwCreateCamera_expands_to_RwCreateCamera = 
   "RwCreateCamera_expands_to_RwCreateCamera";
   `RwCreateCamera_expands_to_RwCreateCamera' defined but not used
   The final expansion is therefore indicated in either case

 */

      CHECK_EXPANSION(RwBeginCameraUpdate);
      CHECK_EXPANSION(RwEndCameraUpdate);
      CHECK_EXPANSION(RwClearCameraViewport);
      CHECK_EXPANSION(RwRenderClump);
      CHECK_EXPANSION(RwRenderScene);
      CHECK_EXPANSION(RwCreateCamera);
      CHECK_EXPANSION(RwDestroyCamera);
      CHECK_EXPANSION(RwShowCameraImage);

#endif                          /* CHECK_OVERLOADING */

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _RWLIBH */

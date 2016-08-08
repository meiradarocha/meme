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

#ifndef   _RWWINH
#define   _RWWINH

#if !defined(RWNODDRAW)
#include <ddraw.h>
#endif

/* Windows and WinG specific stretching stuff. */
typedef struct
{
    RwInt32 width;
    RwInt32 height;
    RwCamera *camera;
}
RwWinOutputSize;

/* Structure for passing in to RwBitmapRaster. */
typedef struct
{
    HDC hdc;
    HBITMAP hBitmap;
}
RwWinBitmapRaster;

#if (!defined(RWNODDRAW))
typedef struct
{
    RwRaster *raster;
    LPDIRECTDRAWSURFACE surface;
}
RwD3DSurfaceQuery;
#endif

/* Windows specific device information codes... */
#define rwWINIMAGEISDIB           rwDEVICESPECIFICINFO
#define rwWINUSINGDIBS           (rwDEVICESPECIFICINFO + 1)
#define rwWINUSINGWING           (rwDEVICESPECIFICINFO + 2)
#define rwWINISBACKGROUNDPALETTE (rwDEVICESPECIFICINFO + 3)
#define rwWINUSINGDIBSECTIONS    (rwDEVICESPECIFICINFO + 4)
#define rwWINUSINGDD             (rwDEVICESPECIFICINFO + 100)

/* Windows specific device control codes... */
#define rwWINSETOUTPUTSIZE        rwDEVICESPECIFICACTION
#define rwWINBACKGROUNDPALETTE   (rwDEVICESPECIFICACTION + 1)
#define rwWINSETRENDERDEPTH      (rwDEVICESPECIFICACTION + 2)

#define rwD3DGETBACKSURFACE      (rwDEVICESPECIFICACTION + 100)
#define rwD3DGETPRIMARYSURFACE   (rwDEVICESPECIFICACTION + 101)
#define rwD3DGETRASTERSURFACE    (rwDEVICESPECIFICACTION + 102)
#define rwD3DRESTORESURFACES     (rwDEVICESPECIFICACTION + 103)
#define rwD3DGETDDHANDLE         (rwDEVICESPECIFICACTION + 104)

#define rwDDGETPRIMARYSURFACE    (rwDEVICESPECIFICACTION + 101)
#define rwDDRESTORESURFACES      (rwDEVICESPECIFICACTION + 103)
#define rwDDGETDDHANDLE          (rwDEVICESPECIFICACTION + 104)

/* Windows specific open options... */
#define rwWINUSEDIBS              rwDEVICESPECIFICOPEN
#define rwWINUSEWING             (rwDEVICESPECIFICOPEN + 1)
#define rwWINASSUME16IS16        (rwDEVICESPECIFICOPEN + 2)
#define rwWINSETWINGDIBORIENT    (rwDEVICESPECIFICOPEN + 3)
#define rwWINUSERENDERDEPTH      (rwDEVICESPECIFICOPEN + 4)
#define rwWINUSEDD               (rwDEVICESPECIFICOPEN + 5)
#define rwAPPCANUSEFULLSCREEN    (rwDEVICESPECIFICOPEN + 6)
#define rwWINUSEDIBSECTION       (rwDEVICESPECIFICOPEN + 7)

#endif /* _RWWINH */

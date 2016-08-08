/* rwdos.h - module for code requiring dos support */

#ifndef RWDOS_PROTO
#define RWDOS_PROTO

#include <rwtypes.h>

/****************************************************************************
 Defines
 */

/* Screen attributes */

#define rwSCRWIDTH rwDEVICESPECIFICACTION+1
#define rwSCRHEIGHT rwDEVICESPECIFICACTION+2
#define rwSCRDEPTH rwDEVICESPECIFICACTION+3

#define rwGETMEMORYRENDER rwDEVICESPECIFICACTION+7

/* Colour */

#define rwSCRGETCOLOR rwDEVICESPECIFICACTION+11
#define rwSCRSETCOLOR rwDEVICESPECIFICACTION+12
#define rwSETPHYSICALPALETTEENTRY rwDEVICESPECIFICACTION+13

/* Character set */

#define rwPRINTCHAR  rwDEVICESPECIFICACTION+21
#define rwSETCHARSET rwDEVICESPECIFICACTION+22

/* Bitmap to raster conversion */

#define rwEXPANDBITMAP rwDEVICESPECIFICACTION+31

/* Mouse operations */

#define rwMOUSESETREGION rwDEVICESPECIFICACTION+41
#define rwMOUSESETPOSITION rwDEVICESPECIFICACTION+42
#define rwMOUSESETSPEED    rwDEVICESPECIFICACTION+43

#define rwMOUSEGETPOSITION rwDEVICESPECIFICACTION+44
#define rwMOUSEGETRELATIVE rwDEVICESPECIFICACTION+45

#define rwSETMOUSESTATE   rwDEVICESPECIFICACTION+46
#define rwGETMOUSESTATE   rwDEVICESPECIFICACTION+47

/* Pointer operations */

#define rwPOINTERSETCLIPREGION rwDEVICESPECIFICACTION+51
#define rwPOINTERSETIMAGE rwDEVICESPECIFICACTION+52
#define rwPOINTERREMOVE rwDEVICESPECIFICACTION+53
#define rwPOINTERDISPLAY     rwDEVICESPECIFICACTION+54
#define rwPOINTERDISPLAYAT   rwDEVICESPECIFICACTION+55
#define rwPOINTERDEFAULT     rwDEVICESPECIFICACTION+56

/* VESA mode access */

#define rwVESAGETINFO             rwDEVICESPECIFICACTION+61
#define rwVESAGETVESAMODENUMBER   rwDEVICESPECIFICACTION+62

/* DAC stuff */

#define rwVESAGETDACSIZE		  rwDEVICESPECIFICACTION+70
#define rwVESASETDACSIZE		  rwDEVICESPECIFICACTION+71

/* Compatability with previous DOS releases */

#define rwPOINTERSETREGION rwMOUSESETREGION
#define rwSETPOINTERPOSITION rwMOUSESETPOSITION
#define rwGETPOINTERRELATIVE rwMOUSEGETRELATIVE
#define rwGETPOINTERPOSITION rwMOUSEGETPOSITION

/* Error codes */

#define E_RW_DOS_MODE_UNAVAILABLE 1
#define E_RW_DOS_NO_VESA_BIOS 2
#define E_RW_DOS_INCOMPATIBLE_BIOS 3
#define E_RW_DOS_NO_MOUSE 4

/* Character printing defines */
/* Or together to get desired mode */

#define rwTRANSPARENT 1
#define rwRASTER 2

/****************************************************************************
 Types
 */

/* Character displaying structure. Mode is a or'ed combination of the
   TRANSPARENT/RASTER fields.
   rw RASTER indicates that the char will be blitted to the raster entry */

/* In all of the colour settings 0 indicates transparent pixel when
   transparency is enabled */

typedef struct RwPrintCharTag
{
    RwInt32 x;
    RwInt32 y;
    char c;
    char mode;
    char __pad[2];
    RwRaster *raster;
    RwInt32 foreground;
    RwInt32 background;
}
RwPrintChar;

/* Mouse pointer structure */

typedef struct RwMousePointerTag
{
    int x;
    int y;
    int buttons;
}
RwMousePointer;

/* bitmap to raster image conversion structure. The raster must be pre created
   The size of the bitmap is pulled from the created raster. Each bitmap
   raster line is considered to consist of a number of whole bytes. Thus a
   wide one bit bitmap 2 lines high is made of 2 bytes 0x80,0x80 */

typedef struct RwBitmapExpandTag
{
    unsigned char *bitmap;
    RwInt32 foreground;
    RwInt32 background;
    RwRaster *raster;
}
RwBitmapExpand;

/* Structure for setting an arbirary character set */
/* A null bit map indicates setting the default */

typedef struct RwCharsetTag
{
    RwInt32 first;
    RwInt32 last;
    unsigned char *bitmap;
    RwInt32 width;
    RwInt32 height;
}
RwCharset;

/* VESA Block information */
/* This is the structure of the VESA 2.0 mode block */

typedef struct
{
    short nModeAttributes;      /* Mode attributes                  */
    char cWinAAttributes;       /* Window A attributes              */
    char cWinBAttributes;       /* Window B attributes              */
    short nWinGranularity;      /* Window granularity in k          */
    short nWinSize;             /* Window size in k                 */
    short nWinASegment;         /* Window A segment                 */
    short nWinBSegment;         /* Window B segment                 */
    void *pWinFuncPtr;          /* Pointer to window function       */
    short nBytesPerScanLine;    /* Bytes per scanline               */
    short nXResolution;         /* Horizontal resolution            */
    short nYResolution;         /* Vertical resolution              */
    char cXCharSize;            /* Character cell width             */
    char cYCharSize;            /* Character cell height            */
    char cNumberOfPlanes;       /* Number of memory planes          */
    char cBitsPerPixel;         /* Bits per pixel                   */
    char cNumberOfBanks;        /* Number of CGA style banks        */
    char cMemoryModel;          /* Memory model type                */
    char cBankSize;             /* Size of CGA style banks          */
    char cNumberOfImagePages;   /* Number of images pages           */
    char cRes1;                 /* Reserved                         */
    char cRedMaskSize;          /* Size of direct color red mask    */
    char cRedFieldPosition;     /* Bit posn of lsb of red mask      */
    char cGreenMaskSize;        /* Size of direct color green mask  */
    char cGreenFieldPosition;   /* Bit posn of lsb of green mask    */
    char cBlueMaskSize;         /* Size of direct color blue mask   */
    char cBlueFieldPosition;    /* Bit posn of lsb of blue mask     */
    char cRsvdMaskSize;         /* Size of direct color res mask    */
    char cRsvdFieldPosition;    /* Bit posn of lsb of res mask      */
    char cDirectColorModeInfo;  /* Direct color mode attributes     */
    unsigned long nPhysBasePtr; /* Physical address for linear buf  */
    unsigned long nOffScreenMemOffset;  /* Pointer to start of offscreen mem */
    short nOffScreenMemSize;    /* Amount of offscreen mem in 1K's  */
}
RwVESAInfo;

#endif

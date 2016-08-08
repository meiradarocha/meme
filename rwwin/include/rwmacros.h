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

#ifndef _RW_MACROS_
#define _RW_MACROS_

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   RwFixed - type
 */

typedef long RwFixed;

#define FIX_MIN		(1)
#define FIX_MAX		(0x7fffffff)
#define CFIX(A)		((RwFixed)((RwFixed)((A) * 65536.0)))
#define INT2FIX(A)	(((long)(A))<<16)
#define FIX2INT(A)	((A)>>16)
#define FL2FIX(A)	((RwFixed)((A) * 65536.0))
#define FIX2FL(A)	((float)(((float)(A))/65536.0))

/* Conversions to and from reals */

#ifdef RWFLOAT
#define FIX2REAL	FIX2FL
#define REAL2FIX	FL2FIX
#endif /* RWFLOAT */

#ifdef RWFIXED
#ifdef __PSX__
#define FIX2REAL(a) ((a)>>4)
#define REAL2FIX(a) ((a)<<4)
#else /* __PSX__ */
#define FIX2REAL
#define REAL2FIX
#endif /* __PSX__ */
#endif /* RWFLOAT */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#ifdef RWFIXED
#undef RWFIXED
#define RWFIXED
#endif

#ifdef RWFLOAT
#undef RWFLOAT
#define RWFLOAT
#endif

#if (defined RWFLOAT) && (defined RWFIXED)
#error "Inconsistent definitions of REAL type."
#endif

#ifndef RWFIXED

#define RwReal float

#define CREAL(a) ((RwReal)(a))
#define INT2REAL(a) ((RwReal)(a))
#define REAL2INT(a) ((long)(a))
#define REAL2FL(a) ((float)(a))
#define FL2REAL(a) ((RwReal)(a))

#define RMul(a, b) ((a) * (b))
#define RAdd(a, b) ((a) + (b))
#define RDiv(a, b) ((a) / (b))
#define RSub(a, b) ((a) - (b))

#define RSqrt(a) ((RwReal)sqrt((double)a))

#else /* RWFIXED */

#ifndef __PSX__

#define RwReal long

#define CREAL(a) ((RwReal)((a) * 65536L))
#define INT2REAL(a) ((RwReal)(((long)(a))<<16))
#define REAL2INT(a) ((a)>>16)
#define REAL2FL(a) (((float)(a))/65536.0f)
#define FL2REAL(a) ((RwReal)((a)*65536.0f))

#define RMul(a, b) (_rwFixMul((a),(b)))
#define RAdd(a, b) ((a) + (b))
#define RDiv(a, b) (_rwFixDiv((a),(b)))
#define RSub(a, b) ((a) - (b))

#ifndef BINARY_PLACES
#define BINARY_PLACES 16
#endif /* BINARY_PLACES */

#if (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16))

#define _rwFixMul(arg1, arg2)						\
   ((long)((((long long)(arg1))*(arg2))>>16))
#define _rwFixDiv(arg1, arg2)						\
({									\
   long long __result;							\
   __result = ((((long long)(arg1))<<16)/(arg2));			\
   ((__result>>31) > 0) ? (long)0x7fffffffUL:				\
           (((__result>>31) < -1) ? (long)0x80000000UL :		\
	    (long)(__result));						\
})

#else /* (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16)) */

extern
RWSPI(RwReal)
_rwFixMul ARGS((RwReal a, RwReal b));
extern
RWSPI(RwReal)
_rwFixDiv ARGS((RwReal a, RwReal b));

#endif /* (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16)) */

#define RSqrt(a) (_rwlsqrt(a))
extern RwReal _rwlsqrt ARGS((RwReal a));

#else /* __PSX__ */

#define RwReal long

#define CREAL(a) ((RwReal)((a) * 4096L))
#define INT2REAL(a) ((RwReal)(((long)(a))<<12))
#define REAL2INT(a) ((a)>>12)
#define REAL2FL(a) (((float)(a))/4096.0f)
#define FL2REAL(a) ((RwReal)((a)*4096.0f))

#define RAdd(a, b) ((a) + (b))
#define RSub(a, b) ((a) - (b))

extern RwReal _rwPSXDiv(RwReal nA, RwReal nB);
extern RwReal _rwPSXMul(RwReal nA, RwReal nB);

#define RMul(a, b) ((_rwPSXMul((a),(b))))
#define RDiv(a, b) ((_rwPSXDiv((a),(b))))

extern int _rwPSXStrToReal(char *cpString, RwReal * npNum);
extern void _rwPSXRealToStr(RwReal nReal, char *cpBuf);

#define REAL2STR(a,b) (_rwPSXRealToStr(a,b))
#define STR2REAL(a,b) (_rwPSXStrToReal(a,b))

#if 0
#if (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16))
#endif

#define _rwFixMul(arg1, arg2)						\
   (long)((((long long)(arg1))*(arg2))>>16)
#define _rwFixDiv(arg1, arg2)						\
({									\
   long long __result;							\
   __result = ((((long long)(arg1))<<16)/(arg2));			\
   ((__result>>31) > 0) ? (long)0x7fffffffUL:				\
           (((__result>>31) < -1) ? (long)0x80000000UL :		\
	    (long)(__result));						\
})

#if 0
#else /* (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16)) */

extern
RWSPI(RwFixed)
_rwFixMul ARGS((RwFixed a, RwFixed b));
extern
RWSPI(RwFixed)
_rwFixDiv ARGS((RwFixed a, RwFixed b));

#endif /* (defined(LONG_LONG_AVAILABLE) && (BINARY_PLACES == 16)) */
#endif

/*#define RSqrt(a) (((_rwlsqrt((a)<<4)))>>4)
   extern RwReal _rwlsqrt ARGS((RwReal a)); */

#define RSqrt(a) (SquareRoot12(a))

#endif /* __PSX__ */

#endif /* RWFIXED */

#if (!defined FALSE) && (!defined TRUE)
#define FALSE 0
#define TRUE !FALSE
#endif

#define RAbs(a) (((a)<CREAL(0.0))?-(a):(a))
/* Provided for backwards compatibility */
#define Rabs(a) (((a)<CREAL(0.0))?-(a):(a))

/**************************************************************************/
/* Immediate mode structure access */

#ifndef __PSX__

/***** 2d immediate mode access *****/

#define RWSETIMMVERTEX2DX(ivV,nX) (((ivV).p2d.x)=(nX))
#define RWSETIMMVERTEX2DY(ivV,nY) (((ivV).p2d.y)=(nY))
#define RWSETIMMVERTEX2DZ(ivV,nZ) (((ivV).p2d.z)=(nZ))
#define RWSETIMMVERTEX2DCAMERAZ(ivV,nZ) (((ivV).p2d.cameraz)=(nZ))

#define RWGETIMMVERTEX2DX(ivV) ((ivV).p2d.x)
#define RWGETIMMVERTEX2DY(ivV) ((ivV).p2d.y)
#define RWGETIMMVERTEX2DZ(ivV) ((ivV).p2d.z)
#define RWGETIMMVERTEX2DCAMERAZ(ivV) ((ivV).p2d.cameraz)

/***** Luminance access *****/

/* Vertex */

/* Is the vertex valid ? */

#define RWISVERTEXVALID(ivV) (!((ivV).flag&0x3f))

/* Indexed colour */

#define RWSETIMMVERTEXLUM(ivV,nL) (((ivV).lum)=(nL))
#define RWGETIMMVERTEXLUM(ivV) ((ivV).lum)

/* True colour */

#define RWSETIMMVERTEXRGB(ivV,nR,nG,nB)					\
  (((ivV).lum)=(nR)); (((ivV).lumg)=(nG)); (((ivV).lumb)=(nB))

#define RWGETIMMVERTEXRED(ivV) ((ivV).lum)
#define RWGETIMMVERTEXGREEN(ivV) ((ivV).lumg)
#define RWGETIMMVERTEXBLUE(ivV) ((ivV).lumb)

/***** Polygon *****/

/* Indexed colour */

#define RWSETIMMPOLYLUM(pPoly,nL) (((pPoly).lum)=(nL))
#define RWGETIMMPOLYLUM(pPoly) ((pPoly).lum)

/* True colour */

#define RWSETIMMPOLYRGB(pPoly,nR,nG,nB)					\
  (((pPoly).lum)=(nR)); (((pPoly).lumg)=(nG)); (((pPoly).lumb)=(nB))

#define RWGETIMMPOLYRED(pPoly) ((pPoly).lum)
#define RWGETIMMPOLYGREEN(pPoly) ((pPoly).lumg)
#define RWGETIMMPOLYBLUE(pPoly) ((pPoly).lumb)

/***** Texture coordinates *****/

#define RWSETIMMVERTEXTEXUV(ivV,nU,nV)					\
  (((ivV).texu)=(nU));(((ivV).texv)=(nV))

#define RWGETIMMVERTEXTEXU(ivV) ((ivV).texu)
#define RWGETIMMVERTEXTEXV(ivV) ((ivV).texv)

/***** Immediate mode type *****/

#define RWSETIMMEDIATE3D(iImm) ((iImm).type=rwIMMEDIATE3D)
#define RWSETIMMEDIATE2D(iImm) ((iImm).type=rwIMMEDIATE2D)
#define RWSETIMMEDIATEVERTICES(iImm) ((iImm).type=rwIMMEDIATEVERTICES)

#define RWGETIMMEDIATETYPE(iImm) (iImm.type)

/***** Z buffering ******/

#define RWSETIMMEDIATEZBUFFERON(iImm) ((iImm).zbuffer=TRUE)
#define RWSETIMMEDIATEZBUFFEROFF(iImm) ((iImm).zbuffer=FALSE)

#define RWGETIMMEDIATEZBUFFER(iImm) ((iImm).zbuffer)

/***** Immediate vertices *****/

#define RWSETIMMVERTEXNO(ivV,nN) (((ivV).vertex)=nN)
#define RWGETIMMVERTEXNO(ivV,nN) ((ivV).vertex)

/***** Getting immediate vertex *****/

#define RWIMMVERTEX(iImm,nVert) ((iImm).vertex[nVert])

/***** Getting immediate mode poly */

#define RWIMMPOLY(iImm) ((iImm).poly)

/***** Material access *****/

#define RWSETIMMEDIATEMATERIAL(iImm,mpMat) (((iImm).poly.material)=(mpMat))
#define RWGETIMMEDIATEMATERIAL(iImm) ((iImm).poly.material)

/***** Local 3d space *****/

#define RWSETIMMVERTEX3DX(ivV,nX) (((ivV).p3d.x)=(nX))
#define RWSETIMMVERTEX3DY(ivV,nY) (((ivV).p3d.y)=(nY))
#define RWSETIMMVERTEX3DZ(ivV,nZ) (((ivV).p3d.z)=(nZ))

#define RWGETIMMVERTEX3DX(ivV) ((ivV).p3d.x)
#define RWGETIMMVERTEX3DY(ivV) ((ivV).p3d.y)
#define RWGETIMMVERTEX3DZ(ivV) ((ivV).p3d.z)

#else /* __PSX__ */

#endif /* __PSX__ */

#endif /* _RW_MACROS_ */

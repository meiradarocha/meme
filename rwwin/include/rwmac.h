/* This file is a product of Criterion Software Ltd.
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

#ifndef   _RWMAC_H
#define   _RWMAC_H


/* Mac specific device information codes... */
#define rwMACISBACKGROUNDPALETTE rwDEVICESPECIFICINFO

/* Mac specific device control codes... */
#define rwMACPIXELDOUBLE	rwDEVICESPECIFICACTION 
#define rwMACBACKGROUNDPALETTE  (rwDEVICESPECIFICACTION + 1)
#define rwMACSETRENDERDEPTH     (rwDEVICESPECIFICACTION + 2)

/* Mac specific open options... */
#define rwMACUSEGDEVICE		rwDEVICESPECIFICOPEN
#define rwMACUSERENDERDEPTH     (rwDEVICESPECIFICOPEN + 1)

#endif
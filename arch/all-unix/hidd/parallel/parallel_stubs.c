/*
    (C) 1998 AROS - The Amiga Research OS
    $Id$

    Desc: Stubs for Parallel and ParallelUnit class
    Lang: english
*/

#ifndef AROS_USE_OOP
#   define AROS_USE_OOP
#endif

#include <aros/config.h>
#include <exec/types.h>
#include <exec/libraries.h>

#include <proto/oop.h>

#include <utility/tagitem.h>

#include <oop/oop.h>
#include <hidd/parallel.h>

#include "parallel_intern.h"

#undef  SDEBUG
#undef  DEBUG
#define DEBUG 0
#include <aros/debug.h>

#undef OOPBase
#define OOPBase ((struct Library *)OOP_OCLASS(OOP_OCLASS(OOP_OCLASS(obj)))->UserData)


/* A small utility function for using varargs when setting attrs */

#warning OOP_SetAttrsTags is defined in inline/oop.h

#ifndef OOP_SetAttrsTags
IPTR OOP_SetAttrsTags(OOP_Object *obj, IPTR tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = OOP_SetAttrs(obj, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST

}
#endif

/***************************************************************/

OOP_Object * HIDD_Parallel_NewUnit(OOP_Object *obj, ULONG unitnum)
{
    static OOP_MethodID mid = 0;
    struct pHidd_Parallel_NewUnit p;
    
    if(!mid) mid = OOP_GetMethodID(IID_Hidd_Parallel, moHidd_Parallel_NewUnit);
        
    p.mID      = mid;
    p.unitnum  = unitnum;

    return((OOP_Object *) OOP_DoMethod(obj, (OOP_Msg) &p));
}
/***************************************************************/

VOID HIDD_Parallel_DisposeUnit(OOP_Object *obj, OOP_Object *unit)
{
    static OOP_MethodID mid = 0;
    struct pHidd_Parallel_DisposeUnit p;
    
    if(!mid) mid = OOP_GetMethodID(IID_Hidd_Parallel, moHidd_Parallel_DisposeUnit);
        
    p.mID    = mid;
    p.unit   = unit;

    OOP_DoMethod(obj, (OOP_Msg) &p);
}




/********************** Stubs for parallel unit **********************/

BOOL HIDD_ParallelUnit_Init(OOP_Object *obj, VOID * DataReceived, VOID * DataReceivedUserData, VOID * WriteData, VOID * WriteDataUserData)
{
    static OOP_MethodID mid = 0;
    struct pHidd_ParallelUnit_Init p;
    
    if(!mid) mid = OOP_GetMethodID(IID_Hidd_ParallelUnit, moHidd_ParallelUnit_Init);
        
    p.mID      		   = mid;
    p.DataReceived 	   = DataReceived;
    p.DataReceivedUserData = DataReceivedUserData;
    p.WriteData    	   = WriteData;
    p.WriteDataUserData	   = WriteDataUserData;

    return((BOOL) OOP_DoMethod(obj, (OOP_Msg) &p));
}
/***************************************************************/

ULONG HIDD_ParallelUnit_Write (OOP_Object *obj, UBYTE * data, ULONG length)
{
    static OOP_MethodID mid = 0;
    struct pHidd_ParallelUnit_Write p;
    
    if(!mid) mid = OOP_GetMethodID(IID_Hidd_ParallelUnit, moHidd_ParallelUnit_Write);
        
    p.mID	= mid;
    p.Length	= length;
    p.Outbuffer	= data; 

    return ((ULONG) OOP_DoMethod(obj, (OOP_Msg) &p));
}

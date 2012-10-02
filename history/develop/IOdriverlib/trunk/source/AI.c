
/******************************************************************************
*
*   FILE
*   ----
*   AI.c
*
*   History
*   -------
*   2012-10-02   File created
*
*******************************************************************************
*
*   This file is generated by the 'acplt_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_IOdriverlib
#define OV_COMPILE_LIBRARY_IOdriverlib
#endif


#include "IOdriverlib.h"
#include "libov/ov_macros.h"


OV_DLLFNCEXPORT void IOdriverlib_AI_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_IOdriverlib_AI pinst = Ov_StaticPtrCast(IOdriverlib_AI, pfb);

    return;
}

OV_DLLFNCEXPORT OV_RESULT IOdriverlib_AI_constructor(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_IOdriverlib_AI pinst = Ov_StaticPtrCast(IOdriverlib_AI, pobj);
    OV_RESULT    result;

    /* do what the base class does first */
    result = IOdriverlib_input_constructor(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */


    return OV_ERR_OK;
}

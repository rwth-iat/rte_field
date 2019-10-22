
/******************************************************************************
*
*   FILE
*   ----
*   SpecialIN.c
*
*   History
*   -------
*   2011-05-27   File created
*
*******************************************************************************
*
*   This file is generated by the 'fb_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_kbuslib
#define OV_COMPILE_LIBRARY_kbuslib
#endif


#include "kbuslib.h"
#include "ov_macros.h"


OV_DLLFNCEXPORT void kbuslib_SpecialIN_startup(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_kbuslib_SpecialIN pinst = Ov_StaticPtrCast(kbuslib_SpecialIN, pobj);

    /* do what the base class does first */
    kbuslib_Clamp_startup(pobj);

    /* do what */

	pinst->v_Value.value.valueunion.val_double = 0;
	pinst->v_Value.value.vartype = OV_VT_BYTE_VEC;
	

    return;
}

OV_DLLFNCEXPORT void kbuslib_SpecialIN_shutdown(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
    OV_INSTPTR_kbuslib_SpecialIN pinst = Ov_StaticPtrCast(kbuslib_SpecialIN, pobj);

    /* do what */

	ov_variable_setanyvalue(&pinst->v_Value, NULL);

    /* set the object's state to "shut down" */
    fb_functionblock_shutdown(pobj);

    return;
}

OV_DLLFNCEXPORT void kbuslib_SpecialIN_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    /*    
    *   local variables
    */

    return;
}

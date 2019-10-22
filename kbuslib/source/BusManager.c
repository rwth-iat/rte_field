
/******************************************************************************
*
*   FILE
*   ----
*   BusManager.c
*
*   History
*   -------
*   2011-02-07   File created
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
#include "kbusl.h"
#include "ov_macros.h"
#include "ov_logfile.h"


OV_DLLFNCEXPORT OV_RESULT kbuslib_BusManager_checkinit(
	OV_INSTPTR_ov_object 	pobj
) {
    /*    
    *   local variables
    */
	OV_INSTPTR_ov_class pBusMngr = NULL;
	OV_INSTPTR_kbuslib_BusManager pUsedManager = NULL;
	OV_INSTPTR_kbuslib_BusManager ptmpManager = NULL;
	OV_INSTPTR_kbuslib_BusManager pinst = Ov_StaticPtrCast(kbuslib_BusManager, pobj);
	OV_RESULT    result;

    /* do what the base class does first */
    result = fb_functionblock_checkinit(pobj);
    if(Ov_Fail(result))
         return result;

    /* do what */
	Ov_ForEachChild(ov_inheritance, pclass_kbuslib_BusManager, pBusMngr)
	{
		ptmpManager = Ov_StaticPtrCast(kbuslib_BusManager, Ov_GetFirstChild(ov_instantiation, pBusMngr));	/*get the Manager used in the Database*/
		if(ptmpManager)
		{
			if(Ov_GetNextChild(ov_instantiation, ptmpManager))		/*if there is more than one Manager of the same type: Error*/
			{
				ov_logfile_error("%s: Error: There is already a Bus Manager existing.", pinst->v_identifier);
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_CRMANAGER);
				pinst->v_ErrorCode = KBUS_ERROR_CRMANAGER;
				pinst->v_Error = FALSE;
				return OV_ERR_BADPARAM;
			}
			if(!pUsedManager)
			{
				pUsedManager = ptmpManager;
			}
			else						/*if a Manager is already found and a second Manager of a different type is found: Error*/
			{
				ov_logfile_error("%s: Error: There is already a Bus Manager existing.", pinst->v_identifier);
				ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_CRMANAGER);
				pinst->v_ErrorCode = KBUS_ERROR_CRMANAGER;
				pinst->v_Error = FALSE;
				return OV_ERR_BADPARAM;
			}
		}
	}
	
	
	
	ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NOERROR);
	pinst->v_ErrorCode = KBUS_ERROR_NOERROR;
	pinst->v_Error = TRUE;
	pinst->v_actimode = 0;

    return OV_ERR_OK;
}

OV_DLLFNCEXPORT void kbuslib_BusManager_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
    

    return;
}


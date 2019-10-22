
/******************************************************************************
 *
 *   FILE
 *   ----
 *   Request.c
 *
 *   History
 *   -------
 *   2015-04-29   File created
 *
 *******************************************************************************
 *
 *   This file is generated by the 'acplt_builder' command
 *
 ******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_modbusTcpLib
#define OV_COMPILE_LIBRARY_modbusTcpLib
#endif


#include "modbusTcpLib.h"
#include "ov_macros.h"
#include "ov_logfile.h"
#include <sys/types.h>


OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_Request_handleResponse(
		OV_INSTPTR_modbusTcpLib_Request	thisReq,
		OV_UINT		dataLength,
		OV_BYTE*	dataToHandle
) {

	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_INT modbusTcpLib_Request_readWord(
		OV_BYTE* pData
) {
	int16_t		temp	=	0;
	temp = pData[0] & 0xFF;
	temp <<= 8;
	temp |= pData[1];
	return temp;
}

OV_DLLFNCEXPORT void modbusTcpLib_Request_writeWord(
		OV_INT	data,
		OV_BYTE* dst
) {
	data &= 0xFFFF;	// cut to word length
	dst[1] = data & 0xFF;
	dst[0] = (data >> 8) & 0xFF;
	return;
}

OV_DLLFNCEXPORT OV_BOOL modbusTcpLib_Request_readBit(
		OV_BYTE* pData,
		OV_UINT bitNumber
) {
	if(pData[bitNumber/8] & (1<<bitNumber % 8)){
		return TRUE;
	} else {
		return FALSE;
	}
}

OV_DLLFNCEXPORT void modbusTcpLib_Request_writeBit(
		OV_BOOL data,
		OV_UINT bitNumber,
		OV_BYTE* dst	
) {
	if(data == TRUE){
		dst[bitNumber/8] |= (1<<bitNumber % 8);
	} else {
		dst[bitNumber/8] &= ~(1<<bitNumber % 8);
	}
	return;
}

OV_DLLFNCEXPORT OV_ACCESS modbusTcpLib_Request_getaccess(
		OV_INSTPTR_ov_object	pobj,
		const OV_ELEMENT		*pelem,
		const OV_TICKET			*pticket
) {
	/*
	 *   local variables
	 */
//	OV_INSTPTR_modbusTcpLib_Request pinst = Ov_StaticPtrCast(modbusTcpLib_Request, pobj);
	/*
	 *	switch based on the element's type
	 */
	switch(pelem->elemtype) {
	case OV_ET_VARIABLE:
		if(pelem->elemunion.pvar->v_offset >= offsetof(OV_INST_ov_object,__classinfo)) {
			if(pelem->elemunion.pvar->v_varprops & OV_VP_SETACCESSOR) {
				return OV_AC_READWRITE;
			}
			return OV_AC_READ;
		}
		break;
	default:
		break;
	}
	return ov_object_getaccess(pobj, pelem, pticket);
}

OV_DLLFNCEXPORT void modbusTcpLib_Request_destructor(
		OV_INSTPTR_ov_object 	pobj
) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_modbusTcpLib_Request pinst = Ov_StaticPtrCast(modbusTcpLib_Request, pobj);
	OV_INSTPTR_modbusTcpLib_IOChannel	pIOCHannel = NULL;
	OV_INSTPTR_modbusTcpLib_IOChannel	pNextIOCHannel = NULL;
	/* do what */
	pIOCHannel = Ov_GetChild(modbusTcpLib_requestToChannel, pinst);
	if(pIOCHannel){
		Ov_Unlink(modbusTcpLib_requestToChannel, pinst, pIOCHannel);
		do{
			pNextIOCHannel = Ov_GetChild(modbusTcpLib_toNextChannel, pIOCHannel);
			if(pNextIOCHannel){
				Ov_Unlink(modbusTcpLib_toNextChannel, pIOCHannel, pNextIOCHannel);
			}
			pIOCHannel = pNextIOCHannel;
		}while(pIOCHannel);
	}
	/* destroy object */
	ov_object_destructor(pobj);

	return;
}

OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_Request_addItem(
		OV_INSTPTR_modbusTcpLib_Request	pRequest,
		OV_INT	address
) {
	if(pRequest->v_requestStartAddr == -1){
		pRequest->v_requestStartAddr = address;
		pRequest->v_requestedItems = 1;
	} else {
		if(address < pRequest->v_requestStartAddr){
			pRequest->v_requestedItems += (pRequest->v_requestStartAddr - address) + 1;
			pRequest->v_requestStartAddr = address;
		} else {
			if(address >= pRequest->v_requestStartAddr + pRequest->v_requestedItems){
				pRequest->v_requestedItems = (address - pRequest->v_requestStartAddr) + 1;
			}
		}
	}
	return OV_ERR_OK;
}
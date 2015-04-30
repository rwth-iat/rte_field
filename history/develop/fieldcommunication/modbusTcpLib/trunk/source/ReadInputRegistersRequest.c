
/******************************************************************************
 *
 *   FILE
 *   ----
 *   ReadInputRegistersRequest.c
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
#include "libov/ov_macros.h"
#include "libov/ov_memstack.h"
#include "ksbase_helper.h"


OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_ReadInputRegistersRequest_handleResponse(
		OV_INSTPTR_modbusTcpLib_Request	thisReq,
		OV_UINT		dataLength,
		OV_BYTE*	dataToHandle
) {
	OV_BYTE	operationCode;
	OV_BYTE	errorCode;
	OV_UINT byteCount;
	OV_TIME timestamp;
	OV_INSTPTR_modbusTcpLib_IOChannel	pIOCHannel = NULL;
	OV_INSTPTR_modbusTcpLib_Slave		pSlave	=	NULL;

	ov_time_gettime(&timestamp);

	/*	request	*/
	operationCode = *dataToHandle;
	dataToHandle++;
	if(operationCode == ((thisReq->v_requestType & 0xFF) | 0x80)){
		/*	we have an error	*/
		pIOCHannel = Ov_GetChild(modbusTcpLib_requestToChannel, thisReq);
		if(!pIOCHannel){
			return OV_ERR_GENERIC;
		}
		pSlave = Ov_DynamicPtrCast(modbusTcpLib_Slave, Ov_GetParent(ov_containment, thisReq)->v_pouterobject);
		if(pSlave){
			pSlave->v_error = TRUE;
		}
		errorCode = *dataToHandle;
		dataToHandle++;
		do{
			pIOCHannel->v_error = TRUE;
			pIOCHannel->v_errorCode = errorCode;
			modbusTcpLib_IOChannel_setErrorText(pIOCHannel);
			Ov_LinkPlaced(modbusTcpLib_errorChannels, pSlave, pIOCHannel, OV_PMH_END);
			pIOCHannel = Ov_GetNextChild(modbusTcpLib_toNextChannel, pIOCHannel);
		}while(pIOCHannel);
	} else if(operationCode == (thisReq->v_requestType & 0xFF)){
		/*	allright	*/
		pIOCHannel = Ov_GetChild(modbusTcpLib_requestToChannel, thisReq);
		if(!pIOCHannel){
			return OV_ERR_GENERIC;
		}
		pSlave = Ov_DynamicPtrCast(modbusTcpLib_Slave, Ov_GetParent(ov_containment, thisReq)->v_pouterobject);
		if(pSlave){
			pSlave->v_error = TRUE;
		}
		byteCount = modbusTcpLib_Request_readWord(dataToHandle);
		dataToHandle += 2;
		if(byteCount + 2 > dataLength){
			return OV_ERR_BADPARAM;
		}
		if(byteCount == thisReq->v_requestedItems * 2){
			do{
				if(((pIOCHannel->v_address - thisReq->v_requestStartAddr) * 2) <= byteCount){
					if(Ov_CanCastTo(modbusTcpLib_AoRI, pIOCHannel)){
						Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PVRaw = modbusTcpLib_Request_readWord(dataToHandle + ((pIOCHannel->v_address - thisReq->v_requestStartAddr) * 2));
						Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PV = (Ov_DynamicPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PVRaw * 1.0)
								/ (OV_SINGLE)(Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_RawHi - Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_RawLo);
						Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PVPV.value = Ov_DynamicPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PV;
						Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PVPV.time = timestamp;
						Ov_StaticPtrCast(modbusTcpLib_AoRI, pIOCHannel)->v_PVPV.state = OV_ST_GOOD;
					}
				} else {
					pIOCHannel->v_error = TRUE;
					pIOCHannel->v_errorCode = 0xFF;
					modbusTcpLib_IOChannel_setErrorText(pIOCHannel);
					Ov_LinkPlaced(modbusTcpLib_errorChannels, pSlave, pIOCHannel, OV_PMH_END);
				}
				pIOCHannel = Ov_GetNextChild(modbusTcpLib_toNextChannel, pIOCHannel);
			}while(pIOCHannel);
		}
	} else {
		/*	weird things....	*/
		pIOCHannel = Ov_GetChild(modbusTcpLib_requestToChannel, thisReq);
		if(!pIOCHannel){
			return OV_ERR_GENERIC;
		}
		pSlave = Ov_DynamicPtrCast(modbusTcpLib_Slave, Ov_GetParent(ov_containment, thisReq)->v_pouterobject);
		if(pSlave){
			pSlave->v_error = TRUE;
		}
		errorCode = 0xFF;
		do{
			pIOCHannel->v_error = TRUE;
			pIOCHannel->v_errorCode = errorCode;
			modbusTcpLib_IOChannel_setErrorText(pIOCHannel);
			Ov_LinkPlaced(modbusTcpLib_errorChannels, pSlave, pIOCHannel, OV_PMH_END);
			pIOCHannel = Ov_GetNextChild(modbusTcpLib_toNextChannel, pIOCHannel);
		}while(pIOCHannel);
	}
	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_ReadInputRegistersRequest_constructor(
		OV_INSTPTR_ov_object 	pobj
) {
	/*
	 *   local variables
	 */
	OV_INSTPTR_modbusTcpLib_ReadInputRegistersRequest pinst = Ov_StaticPtrCast(modbusTcpLib_ReadInputRegistersRequest, pobj);
	OV_RESULT    result;

	/* do what the base class does first */
	result = ov_object_constructor(pobj);
	if(Ov_Fail(result))
		return result;

	/* do what */
	pinst->v_requestID = modbusTcpLib_ModbusTcpManager_getReqId();
	pinst->v_requestType = 04;

	return OV_ERR_OK;
}

OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_ReadInputRegistersRequest_sendRequest(
		OV_INSTPTR_modbusTcpLib_Request thisReq
) {
	KS_DATAPACKET request;
	OV_INSTPTR_TCPbind_TCPChannel	pChannel = NULL;
	OV_VTBLPTR_TCPbind_TCPChannel	pVtblChannel	=	NULL;
	OV_INSTPTR_ov_domain			pDomain	=	NULL;
	OV_INSTPTR_modbusTcpLib_Slave	pSlave	=	NULL;
	OV_RESULT result;

	ov_memstack_lock();
	request.length = 7 + 6;
	request.data = ov_memstack_alloc(request.length);
	if(!request.data){
		return OV_ERR_HEAPOUTOFMEMORY;
	}
	request.readPT = request.data;
	request.writePT = request.data;
	/*	MBAP header	*/
	modbusTcpLib_Request_writeWord(thisReq->v_requestID, request.writePT);
	request.writePT += 2;
	modbusTcpLib_Request_writeWord(0, request.writePT);	//	protocol identification; has to be 0
	request.writePT += 2;
	modbusTcpLib_Request_writeWord(1 + 6, request.writePT);	// number of bytes following
	request.writePT += 2;
	*request.writePT = (OV_BYTE)(thisReq->v_unitIdentifier & 0xFF);
	request.writePT += 1;
	/*	request	*/
	*request.writePT = (OV_BYTE)(thisReq->v_requestType & 0xFF);
	request.writePT += 1;
	modbusTcpLib_Request_writeWord(thisReq->v_requestStartAddr, request.writePT);
	request.writePT += 2;
	modbusTcpLib_Request_writeWord(thisReq->v_requestedItems, request.writePT);
	request.writePT += 2;

	/*	get channel	*/
	pDomain = Ov_GetParent(ov_containment, thisReq);
	if(!pDomain){
		ov_memstack_unlock();
		return OV_ERR_BADPLACEMENT;
	}
	pSlave = Ov_DynamicPtrCast(modbusTcpLib_Slave, pDomain->v_pouterobject);
	pChannel = &pSlave->p_channel;
	Ov_GetVTablePtr(TCPbind_TCPChannel, pVtblChannel, pChannel);
	if(!pVtblChannel){
		ov_memstack_unlock();
		return OV_ERR_GENERIC;
	}
	/*	copy data and send	*/
	result = ksbase_KSDATAPACKET_append(&(pChannel->v_outData), request.data, request.length);
	ov_memstack_unlock();
	if(Ov_OK(result)){
		pVtblChannel->m_SendData(Ov_PtrUpCast(ksbase_Channel, pChannel));
	}
	ov_time_gettime(&thisReq->v_validTill);
	thisReq->v_validTill.secs += pSlave->v_timeout;
	return result;
}


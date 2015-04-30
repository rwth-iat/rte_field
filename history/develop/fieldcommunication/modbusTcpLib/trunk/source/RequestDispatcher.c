
/******************************************************************************
 *
 *   FILE
 *   ----
 *   RequestDispatcher.c
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
#include "libov/ov_result.h"
#include "ksbase_helper.h"

OV_DLLFNCEXPORT OV_RESULT modbusTcpLib_RequestDispatcher_HandleData(
		OV_INSTPTR_ksbase_DataHandler this,
		OV_INSTPTR_ksbase_Channel pChannel,
		KS_DATAPACKET* dataReceived,
		KS_DATAPACKET* answer
) {
	OV_INT requestId;
	OV_INT protocolID;
	OV_INT overallLength;
	OV_BYTE unitIdentifier;
	OV_UINT offset = 0;
	OV_UINT residualPacketLength;
	OV_INSTPTR_modbusTcpLib_Request pRequest = NULL;
	OV_VTBLPTR_modbusTcpLib_Request pVtblRequest = NULL;
	OV_RESULT result;

	if(!this->v_pouterobject || !Ov_CanCastTo(modbusTcpLib_Slave, this->v_pouterobject)){
		ov_logfile_error("%s: RequestDispatchers only work as parts of modbusTcpLib/Slaves", this->v_identifier);
		ksbase_free_KSDATAPACKET(dataReceived);
	}
	do{
		residualPacketLength = (dataReceived->data + dataReceived->length) - dataReceived->readPT;
		if(residualPacketLength < 10){
			/*	incomplete	*/
			return OV_ERR_OK;
		}
		/*	MBAP header	*/
		requestId = modbusTcpLib_Request_readWord(dataReceived->readPT);
		offset = 2;
		protocolID = modbusTcpLib_Request_readWord(dataReceived->readPT + offset);
		offset += 2;
		overallLength = modbusTcpLib_Request_readWord(dataReceived->readPT + offset);
		offset += 2;
		if(residualPacketLength < overallLength + offset ){
			/*	incomplete	*/
			return OV_ERR_OK;
		}
		/*	from here on we can assume that at least one complete request was received
		 * 	process it...	*/
		dataReceived->readPT += offset;
		unitIdentifier = *(dataReceived->readPT);
		dataReceived->readPT++;
		offset++;
		Ov_ForEachChildEx(ov_containment, &(Ov_StaticPtrCast(modbusTcpLib_Slave, this->v_pouterobject)->p_requests), pRequest, modbusTcpLib_Request){
			if(pRequest->v_requestID == requestId){
				/*	found the right request	*/
				Ov_GetVTablePtr(modbusTcpLib_Request, pVtblRequest, pRequest);
				if(!pVtblRequest){
					ov_logfile_error("%s: could not get Vtable of Request", this->v_identifier);
					break;
				}
				if((protocolID != 0) || (unitIdentifier != pRequest->v_unitIdentifier)){
					ov_logfile_error("%s: mismatch of protocolID and/or unitIdentifier", this->v_identifier);
					break;
				}
				result = pVtblRequest->m_handleResponse(pRequest, overallLength - offset, dataReceived->readPT);
				if(Ov_Fail(result)){
					ov_memstack_lock();
					ov_logfile_error("%s: handle Response failed with error %s", pRequest->v_identifier, ov_result_getresulttext(result));
					ov_memstack_unlock();
				}
				Ov_DeleteObject(pRequest);
				break;
			}
		}
		if(!pRequest){
			ov_logfile_error("%s: no fitting request object found", this->v_identifier);
		}
		dataReceived->readPT += overallLength - offset;
		if(dataReceived->readPT >= dataReceived->data + dataReceived->length){
			/*	everything done	*/
			ksbase_free_KSDATAPACKET(dataReceived);
			break;
		}
	}while(1);
	return OV_ERR_OK;
}


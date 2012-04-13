
/******************************************************************************
*
*   FILE
*   ----
*   WagoFBKManager.c
*
*   History
*   -------
*   2011-06-21   File created
*
*******************************************************************************
*
*   This file is generated by the 'fb_builder' command
*
******************************************************************************/


#ifndef OV_COMPILE_LIBRARY_wagoFBKlib
#define OV_COMPILE_LIBRARY_wagoFBKlib
#endif

#include "kbusl.h"
#include "wagoFBKlib.h"
#include "libov/ov_macros.h"
#include "libov/ov_logfile.h"
#include <string.h>
#include "kbusapi.h"

OV_DLLFNCEXPORT void wagoFBKlib_WagoFBKManager_startup(
	OV_INSTPTR_ov_object 	pobj
) {
	int res;
    	OV_INSTPTR_wagoFBKlib_WagoFBKManager pinst = Ov_StaticPtrCast(wagoFBKlib_WagoFBKManager, pobj);
    
    ov_logfile_info("wagoFBKManager_startup...");
    	
	res = KbusOpen();
	if(res)
	{
		ov_logfile_error("%s: error while opening channel to kbus.\n", pinst->v_identifier);
		pinst->v_Error = FALSE;
		ov_string_setvalue(&pinst->v_ErrorString, strerror(res));
		return;
		
	}
	
	return;
}


OV_DLLFNCEXPORT void wagoFBKlib_WagoFBKManager_shutdown(
	OV_INSTPTR_ov_object 	pobj
) {
	ov_logfile_info("wagoFBKManager_shutdown...");
	
	KbusClose();
	return;
}


OV_DLLFNCEXPORT void wagoFBKlib_WagoFBKManager_typemethod(
	OV_INSTPTR_fb_functionblock	pfb,
	OV_TIME						*pltc
) {
	OV_INSTPTR_wagoFBKlib_WagoFBKManager pinst = Ov_StaticPtrCast(wagoFBKlib_WagoFBKManager, pfb);
	OV_INSTPTR_ov_object pdatablock;
	OV_INSTPTR_kbuslib_Clamp pnewclamp = NULL;
	OV_INSTPTR_ov_object pcClampobj;
	OV_INSTPTR_kbuslib_Clamp pcClamp;
	
	int tmp_number;
	FILE * pConfigFile;
	unsigned char LineBuf[255];
	
	unsigned int SlotNumber;
	unsigned char ArticleNumber[20];
	unsigned int ModuleGroup; 	        /* DUMMY, DI, DO, DIO, AI, AO, COMPLEX */
    unsigned int ChannelCount;	/* Number of channels */
    unsigned char AlternativFormat;   /* Alternative format ? 'y' : 'n' */
    unsigned int OutByteOffset; 	/* OutputProcessImage: Startaddress[Byte]  */
    unsigned int OutBitOffset; 	/* OutputProcessImage: Startaddress[Bit]  */
    unsigned int OutBitSize; 		/* OutputProcessImage: Length [Bit]  */
    unsigned int InByteOffset; 	/* InputProcessImage: Startaddress [Byte]  */
    unsigned int InBitOffset; 	/* InputProcessImage: Startaddress [Bit]  */
    unsigned int InBitSize; 		/* IntputProcessImage: Length [Bit]  */
	
	int dig_input_offset;
	int dig_output_offset;
	unsigned int i;
	unsigned int j;
	OV_STRING clamp_name = NULL;
	
	int res;
	double dtemp;
	uint8_t btemp;
	uint16_t value;
	
	
	dig_input_offset = KbusGetBinaryInputOffset();
	if(dig_input_offset < 0)
		ov_logfile_warning("%s: Offset of digital INs not recognized. Address consistence check not possible", 
			pinst->v_identifier);
	dig_output_offset = KbusGetBinaryOutputOffset();
    	if(dig_output_offset < 0)
		ov_logfile_warning("%s: Offset of digital OUTs not recognized. Address consistence check not possible", 
			pinst->v_identifier);
	
        
        /***************************************************
        	Reset Errors
        ****************************************************/
        
        pinst->v_Error = TRUE;
        pinst->v_ErrorCode = KBUS_ERROR_NOERROR;
        ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_NOERROR);
        
        ov_string_setvalue(&pinst->v_RangeErrorIdent, "<none>");
        
        
        /****************************************************
        	Auto Detection of Clamps
        *****************************************************/
        
        
	if(pinst->v_autodetect == TRUE)
	{
		
		pinst->v_nrDI = 0;
    	pinst->v_nrDO = 0;
    	pinst->v_nrAI = 0;
    	pinst->v_nrAO = 0;
    	pinst->v_nrSP = 0;
    	
    		
		pdatablock = Ov_GetFirstChild(ov_containment, Ov_PtrUpCast(ov_domain, pfb));
		
		while(pdatablock)
		{
									
			if(Ov_CanCastTo(kbuslib_Clamp, pdatablock))	//if contained object is a Clamp, delete it an get the next one
			{
				if(Ov_Fail(Ov_DeleteObject(pdatablock)))
				{
					ov_logfile_error("%s: deletion of datablock %s failed\n", 
						pdatablock->v_identifier, Ov_StaticPtrCast(kbuslib_Clamp, pdatablock)->v_identifier);
					pinst->v_autodetect = FALSE;
					
					pinst->v_Error = FALSE;
					pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
					ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
					return;
				}
			
				pdatablock = Ov_GetFirstChild(ov_containment, Ov_PtrUpCast(ov_domain, pfb));
			}
			else	//if not, get next contained object
				pdatablock = Ov_GetNextChild(ov_containment, Ov_PtrUpCast(ov_object, pdatablock));
		}
		
		
		/* get information about installed terminals */
		
		/*open kbus configuration file*/
		pConfigFile = fopen("/proc/driver/kbus/config.csv", "r");
        
        if(pConfigFile == NULL)
        {
	    	ov_logfile_error("could not open kbus configuration");
	    	pinst->v_Error = FALSE;
			pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
			ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
	      return ;        
        }
        
        
		
		
		while( 0x00 != fgets(LineBuf, 255, pConfigFile)) /*read out values for each slot and create clamps*/ 
		{
			sscanf(LineBuf, "%d\t%s\t%d\t%d\t%c\t%d\t%d\t%d\t%d\t%d\t%d\n",
				&SlotNumber, ArticleNumber, &ModuleGroup, &ChannelCount,
				&AlternativFormat, &OutByteOffset, &OutBitOffset, &OutBitSize,
				&InByteOffset, &InBitOffset, &InBitSize); 	
			
			
			/*use ArticleNumber to determine clamp type*/
			
			if((strstr(ArticleNumber, "750-6")) 
				|| (strstr(ArticleNumber, "750-404")) 
				|| (strstr(ArticleNumber, "750-511")) 
				|| (strstr(ArticleNumber, "750-???")))
			{
				/*special clamps; not tested yet*/
				j=0;	/*initialize j*/
				do		/*at least one input*/
				{
					
					pinst->v_nrSP++;
					
					if(InBitSize)
					{	/*input found*/
						/*create new object with name "Terminal$terminalnumber:$channelnumber-SI"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-SI", SlotNumber, j);
						if(Ov_Fail(Ov_CreateObject(kbuslib_SpecialIN, pnewclamp, pinst, clamp_name)))
						{
								ov_logfile_error("%s: creation of SpecialIN %s failed\n", 
								pinst->v_identifier, clamp_name);
								pinst->v_autodetect = FALSE;
						
								pinst->v_Error = FALSE;
								pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
								ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
								return;
						}
								
						/*assuming number of input channels is the same as number of output channels. assuming
								each channel has the same size*/
									
						pnewclamp->v_ByteAddress = InByteOffset;
						if(ChannelCount)
							pnewclamp->v_ByteAddress += j * (InBitSize / 8 / ChannelCount);
						
						pnewclamp->v_ByteWidth = InBitSize / 8;
					
						if(ChannelCount)
							pnewclamp->v_ByteWidth /= ChannelCount;
									
						pnewclamp->v_Enabled = TRUE;
					}
							
					if(OutBitSize)
					{	/*output found*/
						/*create new object with name "Terminal$terminalnumber:$channelnumber-SO"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-SO", SlotNumber, j);
						if(Ov_Fail(Ov_CreateObject(kbuslib_SpecialOUT, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of SpecialOUT %s failed\n", 
							pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							return;
						}
							
						pnewclamp->v_ByteAddress = OutByteOffset;
						if(ChannelCount)
							pnewclamp->v_ByteAddress += j * (OutBitSize / 8 / ChannelCount);
					
						pnewclamp->v_ByteWidth = OutBitSize / 8;
						
						if(ChannelCount)
							pnewclamp->v_ByteWidth /= ChannelCount;
						
						pnewclamp->v_Enabled = TRUE;
					}
					
					j++;
				} while(j < ChannelCount);		
			}
			else
			{
				if(AlternativFormat == 'n')	/*Digital IO*/
				{
					
					if(strstr(ArticleNumber, "750-4xx"))
					{
						
						
						/*digital in*/
						for(j = 0; j < InBitSize; j++)
						{
							pinst->v_nrDI++;
							/*create new object with name "Terminal$terminalnumber:$channelnumber-DI"*/
							ov_string_print(&clamp_name, "Terminal%03hhu:%02u-DI", SlotNumber, j + 1);
							if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalIN, pnewclamp, pinst, clamp_name)))
							{
								ov_logfile_error("%s: creation of DigitalIN %s failed\n", 
									pinst->v_identifier, clamp_name);
								pinst->v_autodetect = FALSE;
					
								pinst->v_Error = FALSE;
								pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
								ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
								return;
							}
						
							/*set byte Address*/
							pnewclamp->v_ByteAddress = InByteOffset + (InBitOffset + j) / 8;
							/*set BitOffset*/
							pnewclamp->v_BitOffset = (InBitOffset + j) % 8;
						
							
							/*set byteWidth*/
							pnewclamp->v_ByteWidth = 0;
							pnewclamp->v_Enabled = TRUE;
						
						}
					}
					else
					if(strstr(ArticleNumber, "750-5xx"))
					{		/*digital out */
						
					
						for(j = 0; j < OutBitSize; j++)
						{
							pinst->v_nrDO++;
							/*create new object with name "Terminal$terminalnumber:$channelnumber-DO"*/
							ov_string_print(&clamp_name, "Terminal%03hhu:%02u-DO", SlotNumber, j + 1);
							if(Ov_Fail(Ov_CreateObject(kbuslib_DigitalOUT, pnewclamp, pinst, clamp_name)))
							{
								ov_logfile_error("%s: creation of DigitalOUT %s failed\n", 
									pinst->v_identifier, clamp_name);
								pinst->v_autodetect = FALSE;
					
								pinst->v_Error = FALSE;
								pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
								ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
								return;
							}
						
							/*set byte Address*/
							pnewclamp->v_ByteAddress = OutByteOffset + (OutBitOffset + j) / 8;
							/*set BitOffset*/
							pnewclamp->v_BitOffset = (OutBitOffset + j) % 8;
							
							/*set byteWidth*/
							pnewclamp->v_ByteWidth = 0;
							pnewclamp->v_Enabled = TRUE;
						}	
					
					}
				}				
				else
				if(strstr(ArticleNumber, "750-4"))
				{		/*analog in*/
				
					
					pinst->v_nrAI += ChannelCount;
					for(j = 0; j < ChannelCount; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-AI"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-AI", SlotNumber, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_AnalogIN, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of AnalogIN %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
					
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							return;
						}
						
						/*set byte Address*/
						pnewclamp->v_ByteAddress = InByteOffset + j * sizeof(uint16_t);
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = InBitSize / 8 / ChannelCount;
						pnewclamp->v_Enabled = TRUE;						
						
					}
					
				}
				else
				if(strstr(ArticleNumber, "750-5"))
				{		/*analog out*/
				
					pinst->v_nrAO += ChannelCount;
					
					for(j = 0; j < ChannelCount; j++)
					{
						/*create new object with name "Terminal$terminalnumber:$channelnumber-AO"*/
						ov_string_print(&clamp_name, "Terminal%03hhu:%02u-AO", SlotNumber, j + 1);
						if(Ov_Fail(Ov_CreateObject(kbuslib_AnalogOUT, pnewclamp, pinst, clamp_name)))
						{
							ov_logfile_error("%s: creation of AnalogOUT %s failed\n", 
								pinst->v_identifier, clamp_name);
							pinst->v_autodetect = FALSE;
				
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_AUTODETECT;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_AUTODETECT);
							return;
						}
						
						/*set byte Address*/
						pnewclamp->v_ByteAddress = OutByteOffset + j * sizeof(uint16_t);
						/*set byteWidth*/
						pnewclamp->v_ByteWidth = OutBitSize / 8 / ChannelCount;
						pnewclamp->v_Enabled = TRUE;
											
					}
					
				
				}			
			}
			
		}
	
		
		
	pinst->v_autodetect = FALSE;		/*autodetec only once*/
				
	}     
	
	/********************************************************
		End Auto Detection of Clamps
	********************************************************/
	
	
	
	/*****************************************************
		Communication with clamps
	******************************************************/
	
	
	
	
	/*	first iterate over outputs	*/
	
	Ov_ForEachChild(ov_containment, pinst, pcClampobj)		/*iterate over all contained objects*/
	{
		
		/*	Analog Outs	*/
		
		if(Ov_CanCastTo(kbuslib_AnalogOUT, pcClampobj))	/* if object is an AnalogOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{
				/*if((tmp_number <= dig_output_offset) && (dig_output_offset >= 0)) TODO: wieder einfügen, wenn ofsets stimmen*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_RBValue = 
						(pstPabOUT->us.Pab[tmp_number / sizeof(uint16_t)] / 32767.00  
							* (Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_UpperLimit
							- Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_LowerLimit)
							 + Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_LowerLimit);
						
					if(!Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_ReadBackOnly)
					{
						dtemp = Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_Value;
						
						if(dtemp >= Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_LowerLimit
							&& dtemp <= Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_UpperLimit)
						{
							dtemp -= Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_LowerLimit;
							dtemp /= (Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_UpperLimit 
								- Ov_StaticPtrCast(kbuslib_AnalogOUT, pcClamp)->v_LowerLimit);
										
							pstPabOUT->us.Pab[tmp_number / sizeof(uint16_t)] = (uint16_t) (dtemp * 32767);
					
						}
						else
						{
							ov_logfile_error("%s: value outside Upper / Lower limit", 
								Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_identifier);
							pcClamp->v_Error = FALSE;
							pcClamp->v_ErrorCode = KBUS_ERROR_OUT_OF_RANGE;
							ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_OUT_OF_RANGE);
							pinst->v_Error = FALSE;
							pinst->v_ErrorCode = KBUS_ERROR_OUT_OF_RANGE;
							ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_OUT_OF_RANGE);
							ov_string_setvalue(&pinst->v_RangeErrorIdent, pcClamp->v_identifier);
							
						}
					}
					
				}
				/*else
				{
					ov_logfile_error("%s: specified address lies within range of digital outs", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					
				}*/
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Special Outs	*/
		
		if(Ov_CanCastTo(kbuslib_SpecialOUT, pcClampobj))	/* if object is a SpecialOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{
				/*if((tmp_number <= dig_output_offset) && (dig_output_offset >= 0))*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					Ov_SetDynamicVectorValue(&(Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_RbValue.value.valueunion.val_byte_vec), &(pstPabOUT->uc.Pab[tmp_number]), pcClamp->v_ByteWidth, BYTE);
					
					if(!Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_ReadBackOnly)
					{
						if(Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_Value.value.valueunion.val_byte_vec.value)
						{
							for(i=0; i < pcClamp->v_ByteWidth; i++)
								pstPabOUT->uc.Pab[i + tmp_number] = Ov_StaticPtrCast(kbuslib_SpecialOUT, pcClamp)->v_Value.value.valueunion.val_byte_vec.value[i];
						}
					}
					
				}
				/*else
				{
					ov_logfile_error("%s: specified address lies within range of digital outs", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					
				}*/
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Digital outs	*/
		
		if(Ov_CanCastTo(kbuslib_DigitalOUT, pcClampobj))	/* if object is a DigitalOUT*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{
				/*if((tmp_number >= dig_output_offset) && (dig_output_offset >= 0))*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					/*	get byte with value of interest	*/
					
					value = pstPabOUT->uc.Pab[tmp_number];
					
					btemp = value;
					btemp = (btemp >> (pcClamp->v_BitOffset)) & 1;
					if(btemp)
						Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_RBValue = TRUE;
					else
						Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_RBValue = FALSE;	
					
					if(!Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_ReadBackOnly)
					{
					
						/*	alter bit for digital out	*/
						if(Ov_StaticPtrCast(kbuslib_DigitalOUT, pcClamp)->v_Value)
						{
							value |= 1 << (pcClamp->v_BitOffset);
						}
						else
						{
						
							value &= ~(1 << (pcClamp->v_BitOffset));
						}
					
		
						/*	set altered byte in process image	*/
						pstPabOUT->uc.Pab[tmp_number] = value;
					}
					
				}
				/*else
				{
					ov_logfile_error("%s: specified address lies not within space of digital outs", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
				
				}*/
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}

			continue;		/*	continue to next object	*/
		}
	
	}
	
	
	
	res = KbusUpdate();
	if(res)
	{
		pinst->v_Error = FALSE;
		ov_string_setvalue(&pinst->v_ErrorString, KBUS_ERRORSTR_TIME);
		ov_logfile_error("%s: updating IO-Images failed, is kbus initialized?", pinst->v_identifier);
		//pinst->v_actimode = 0;
		//return;
	}
	
		
	/*then iterate over all inputs*/
	
	Ov_ForEachChild(ov_containment, pinst, pcClampobj)		/*iterate over all contained objects*/
	{
		/*	Analog Ins	*/
		
		if(Ov_CanCastTo(kbuslib_AnalogIN, pcClampobj))	/* if object is an AnalogIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp enabled*/
			{
				/*if((tmp_number <= dig_input_offset) && (dig_input_offset >= 0))*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!pcClamp->v_Error) || pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_Value = 
						(pstPabIN->us.Pab[tmp_number / sizeof(uint16_t)] / 32767.00  
							* (Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_UpperLimit
							- Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_LowerLimit)
							 + Ov_StaticPtrCast(kbuslib_AnalogIN, pcClamp)->v_LowerLimit);
				}
				/*else
				{
					ov_logfile_error("%s: An analog input Byte Address lies within space of digital ins", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					
				}*/
				
			}
			/*else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}*/
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Special Ins	*/
		
		if(Ov_CanCastTo(kbuslib_SpecialIN, pcClampobj))	/* if object is an SpecialIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp enabled*/
			{
				/*if((tmp_number <= dig_input_offset) && (dig_input_offset >= 0))*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!pcClamp->v_Error) || pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
	
					Ov_SetDynamicVectorValue(&(Ov_StaticPtrCast(kbuslib_SpecialIN, pcClamp)->v_Value.value.valueunion.val_byte_vec), &(pstPabIN->uc.Pab[tmp_number]), pcClamp->v_ByteWidth, BYTE);
					
						
				}
				/*else
				{
					ov_logfile_error("%s: special input Byte Address lies within space of digital ins", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					
				}*/
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
		/*	Digital ins	*/
		
		if(Ov_CanCastTo(kbuslib_DigitalIN, pcClampobj))	/* if object is an DigitalIN, feed in value*/
		{
			pcClamp = Ov_StaticPtrCast(kbuslib_Clamp, pcClampobj);
			tmp_number = pcClamp->v_ByteAddress;
			
			if(pcClamp->v_Enabled)				/*check if clamp is enabled*/
			{
				/*if((tmp_number >= dig_input_offset) && (dig_input_offset >= 0))*/
				{
					/*if there was an error, or the clamp was diasbled reset it*/
					if((!(pcClamp->v_Error))
						|| pcClamp->v_ErrorCode == KBUS_CLAMP_DISABLED)	
					{
						pcClamp->v_Error = TRUE;
						pcClamp->v_ErrorCode = KBUS_ERROR_NOERROR;
						ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOERROR); 
					}
					
					btemp = pstPabIN->uc.Pab[tmp_number];
					btemp = (btemp >> pcClamp->v_BitOffset) & 1;
					if(btemp)
						Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_Value = TRUE;
					else
						Ov_StaticPtrCast(kbuslib_DigitalIN, pcClamp)->v_Value = FALSE;	
				}
				/*else
				{
					ov_logfile_error("%s: specified address lies not within space of digital inputs", pcClamp->v_identifier);
					pcClamp->v_Error = FALSE;
					pcClamp->v_ErrorCode = KBUS_ERROR_NOCLAMP;
					ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_ERRORSTR_NOCLAMP); 
					
				}*/
				
			}
			else
			{
				pcClamp->v_Error = TRUE;
				pcClamp->v_ErrorCode = KBUS_CLAMP_DISABLED;
				ov_string_setvalue(&pcClamp->v_ErrorString, KBUS_STR_CLAMP_DISABLED); 
				
			}
			
			continue;		/*	continue to next object	*/
		}
		
	}
			
				
				
		
		
	
	

		
	
	
	/*****************************************************
		End Communication with clamps
	******************************************************/

	return;
}


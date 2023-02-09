//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
 
/** @file PlatformRasIpmiLib.c
    Translate Error Log entry from AMD MCA and NBIO format to Ipmi

**/

#include <AmiDxeLib.h>
#include <Library/PlatformRasIpmiLib.h>
#include <Library/PlatformRasSmbiosLib.h>
#include <Library/PlatformRasCommonLib.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Token.h>

extern  UINT32  SymbolSize;

#if (CHANNEL_PER_SOCKET == 2)  //1die

#if (DIMMS_PER_CHANNEL == 1)
UINT8   DimmLookupTable[1][2]={
        {1,0}
};
#else
UINT8   DimmLookupTable[1][2]={
        {2,0}
};
#endif

#elif (CHANNEL_PER_SOCKET == 4) //2die

#if (DIMMS_PER_CHANNEL == 1)
UINT8   DimmLookupTable[2][2]={
        {1,0},
        {3,2}
};
#else
UINT8   DimmLookupTable[2][2]={
        {2,0},
        {6,4}
};
#endif

#else //4die

#if (DIMMS_PER_CHANNEL == 1)
UINT8   DimmLookupTable[4][2]={
        {3,2},
        {0,1},
        {7,6},
        {4,5}
};
#else
UINT8   DimmLookupTable[4][2]={
        {6,4},
        {0,2},
        {14,12},
        {8,10}
};
#endif
#endif

/**
    Translate/convert, if possible, error entry from MCA to Ipmi format

    @param RAS_MCA_ERROR_INFO   *McaErrorRecord - pointer to MCA error structure
    @param UINT8                *SmbiosErrorBuffer - buffer with Ipmi error entry, only data
    @param DIMM_INFO            *DimmInfo - pointer to DIMM info structure, can be NULL for 
                                            for non DRAM ECC errors

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

EFI_STATUS
McaToIpmi(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  UINTN                 BankIndex,
  IN  OUT UINT8             *ErrorBuffer,
  IN  DIMM_INFO             *DimmInfo
  )
{
    EFI_STATUS                  Status;
    IPMI_SEL_EVENT_RECORD_DATA  *SelRecord;
    UINTN                       EccType = 0;
    UINT16                      Syndrome;
    UINT8                       DramErrorType;
    UINT8                       Symbol;
        
    Status = EFI_NOT_FOUND;
    
    if((McaErrorRecord == NULL)||(ErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
   
    switch (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber){
        case MCA_UMC0_BANK:
        case MCA_UMC1_BANK:
            SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
            
            SelRecord->TimeStamp = 0;
            SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
            SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
            SelRecord->EvMRevision = IPMI_EVM_REVISION;
            SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
            
            SelRecord->SensorType = 0x0C; // Memory
                SelRecord->SensorNumber = 0x08;
            
            DramErrorType = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
            
            if( DramErrorType == DramEccErr ){
                  SelRecord->OEMEvData1 = 0xA0 | 0x00;    //MEMORY_SENSOR_CE_OFFSET;
                //BMC FW - OEMEvData2=DIMM number - for GUI
                SelRecord->OEMEvData2 = DimmLookupTable[McaErrorRecord->CpuInfo.DieId][McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK]; //BMC FW OEMvData2->DIMM#
#if (DIMMS_PER_CHANNEL == 2) // lookup table has only first DIMM in channel number                
                SelRecord->OEMEvData2 += (DimmInfo->ChipSelect > 1)?1:0;
#endif                
            
                if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                } else if( McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                }
            
                SelRecord->OEMEvData3 = McaErrorRecord->CpuInfo.SocketId;  //BMC FW OEMvData3->Socket# - for GUI
                // Check if this is a Corrected Error
                if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorPriority == 0x2){
                    // Check if syndrome valid
                    if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.Length != 0){
                        Symbol = (UINT8)((McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation >> 8)& 0x3F - 1);
                        Syndrome = (UINT16)(McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.Syndrome);
                        
                        // Check Single or Multibit
                        if(SymbolSize == 8){
                            if((Symbol <= 0x07)||(Symbol == 0x11)){
                                if(1 == PopCount16(Syndrome & 0x00FF)){
                                    EccType = 1;    // Single bit
                                }else{
                                    EccType = 2;    // MultiBit
                                }
                            }else if (((Symbol > 0x07)&&(Symbol < 0x0F))||(Symbol == 0x12)){
                                if(1 == PopCount16(Syndrome & 0xFF00)){
                                    EccType = 1;    // Single bit
                                }else{
                                    EccType = 2;    // MultiBit
                                }
                            }
                        } else { // SymbolSize = 4
                            if(1 == PopCount16(Syndrome & 0x000F)){
                                EccType = 1;
                            }else{
                                EccType = 2;
                            }
                        } // End if SymbolSize == 8/4
                    }
                }  // End if Corrected error
                
                // OEM PORTING NOTE - this block of code can be used to add details about Corr DRAM ECC error - 
                // Single or Multibit                    
                if(EccType == 1){
                    // Modify event data to single bit 
                }else if (EccType == 2){
                    // Modify event data to multibit
                }else{
                    // Error is not corrected or not enough information
                }
                Status = EFI_SUCCESS;
                
            } else if (DramErrorType == AddressCommandParityErr){
                // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
                // Add Memory Parity Error to BMC SEL +>
                //SelRecord->OEMEvData1 = 0x00 | 0x0A;    //BMC FW;
                SelRecord->OEMEvData1 = 0xA0 | 0x00;    //BMC FW;
                // Add Memory Parity Error to BMC SEL ->
                //BMC FW - OEMEvData2=DIMM number - for GUI
                SelRecord->OEMEvData2 = DimmLookupTable[McaErrorRecord->CpuInfo.DieId][McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK]; //BMC FW OEMvData2->DIMM#
#if (DIMMS_PER_CHANNEL == 2) // lookup table has only first DIMM in channel number                
                    SelRecord->OEMEvData2 += (((McaErrorRecord->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr >> 32) & 0x03) > 1)?1:0;
#endif                
                if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                } else if( McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                }              
                SelRecord->OEMEvData3 = McaErrorRecord->CpuInfo.SocketId;  //BMC FW OEMvData3->Socket# - for GUI
                Status = EFI_SUCCESS;
                
            } else if (DramErrorType == WriteDataCrcErr){
                // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
                // Add Memory CRC Error to BMC SEL +>
                //SelRecord->OEMEvData1 = 0x00 | 0x0A;    //BMC FW;
                SelRecord->OEMEvData1 = 0xA0 | 0x00;    //BMC FW;
                // Add Memory RCR Error to BMC SEL ->
                // BMC FW - OEMEvData2=DIMM number - for GUI
                SelRecord->OEMEvData2 = DimmLookupTable[McaErrorRecord->CpuInfo.DieId][McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber - MCA_UMC0_BANK]; //BMC FW OEMvData2->DIMM#
#if (DIMMS_PER_CHANNEL == 2) //lookup table has only first DIMM in channel number                
                    SelRecord->OEMEvData2 += (((McaErrorRecord->McaBankErrorInfo[BankIndex].McaAddrMsr.Field.ErrorAddr >> 32) & 0x03) > 1)?1:0;
#endif                
                if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                } else if( McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Deferred == 1){
                    SelRecord->OEMEvData1 |= 0x01; // UnCorrectable
                }              
                SelRecord->OEMEvData3 = McaErrorRecord->CpuInfo.SocketId;  //BMC FW OEMvData3->Socket# - for GUI
                Status = EFI_SUCCESS;
                
            } else {
                Status = EFI_NOT_FOUND;
            }

			if (Status == EFI_SUCCESS){
			  // Modify OemData Follow SEL spec+>
              // SelRecord->OEMEvData2 = (Node Controller(Die) ID<<0x3) | (Memory Controll Number<<0x1) | (Boot Error<<0x0)
              // SelRecord->OEMEvData3 = (CPU Socket Number<<0x5) | (Channel Number<<0x2) | (DIMM Number<<0x0)
              switch (SelRecord->OEMEvData2) {
                case 0x0:  //DIMMA1 --- Die1 UMC0
                  SelRecord->OEMEvData2 = (1<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (0<<0x2) | (1<<0x0);
                  break;
                case 0x1:  //DIMMA0 --- Die1 UMC0
                  SelRecord->OEMEvData2 = (1<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (0<<0x2) | (0<<0x0);
                  break;
                case 0x2:  //DIMMB1 --- Die1 UMC1
                  SelRecord->OEMEvData2 = (1<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (1<<0x2) | (1<<0x0);
                  break;
                case 0x3:  //DIMMB0 --- Die1 UMC1
                  SelRecord->OEMEvData2 = (1<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (1<<0x2) | (0<<0x0);
                  break;
                case 0x4:  //DIMMC1 --- Die0 UMC1
                  SelRecord->OEMEvData2 = (0<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (2<<0x2) | (1<<0x0);
                  break;
                case 0x5:  //DIMMC0 --- Die0 UMC1
                  SelRecord->OEMEvData2 = (0<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (2<<0x2) | (0<<0x0);
                  break;
                case 0x6:  //DIMMD1 --- Die0 UMC0
                  SelRecord->OEMEvData2 = (0<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (3<<0x2) | (1<<0x0);
                  break;
                case 0x7:  //DIMMD0 --- Die0 UMC0
                  SelRecord->OEMEvData2 = (0<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (3<<0x2) | (0<<0x0);
                  break;
                case 0x8:  //DIMME1 --- Die3 UMC0
                  SelRecord->OEMEvData2 = (3<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (4<<0x2) | (1<<0x0);
                  break;
                case 0x9:  //DIMME0 --- Die3 UMC0
                  SelRecord->OEMEvData2 = (3<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (4<<0x2) | (0<<0x0);
                  break;
                case 0xA:  //DIMMF1 --- Die3 UMC1
                  SelRecord->OEMEvData2 = (3<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (5<<0x2) | (1<<0x0);
                  break;
                case 0xB:  //DIMMF0 --- Die3 UMC1
                  SelRecord->OEMEvData2 = (3<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (5<<0x2) | (0<<0x0);
                  break;
                case 0xC:  //DIMMG1 --- Die2 UMC1
                  SelRecord->OEMEvData2 = (2<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (6<<0x2) | (1<<0x0);
                  break;
                case 0xD:  //DIMMG0 --- Die2 UMC1
                  SelRecord->OEMEvData2 = (2<<0x3) | (1<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (6<<0x2) | (0<<0x0);
                  break;
                case 0xE:  //DIMMH1 --- Die2 UMC0
                  SelRecord->OEMEvData2 = (2<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (7<<0x2) | (1<<0x0);
                  break;
                case 0xF:  //DIMMH0 --- Die2 UMC0
                  SelRecord->OEMEvData2 = (2<<0x3) | (0<<0x1) | (0<<0x0);
                  SelRecord->OEMEvData3 = (SelRecord->OEMEvData3<<0x5) | (7<<0x2) | (0<<0x0);
                  break;
                default:
                  break;
            }
            //  Modify OemData Follow SEL spec->
		}
            break;  // End of UMC processing
      // Log Processor Error to BMC SEL +>
      case MCA_LS_BANK:
      case MCA_IF_BANK:
      case MCA_L2_BANK:
      case MCA_DE_BANK:
      case MCA_EX_BANK:
      case MCA_FP_BANK:
      case MCA_L3_C0_S0_BANK:
      case MCA_L3_C0_S1_BANK:
      case MCA_L3_C0_S2_BANK:
      case MCA_L3_C0_S3_BANK:
      case MCA_L3_C1_S0_BANK:
      case MCA_L3_C1_S1_BANK:
      case MCA_L3_C1_S2_BANK:
      case MCA_L3_C1_S3_BANK:
      case MCA_SMU_BANK:
      case MCA_PSP_BANK:
      case MCA_FUSE_BANK:
      case MCA_CS0_BANK:
      case MCA_CS1_BANK:
      case MCA_PIE_BANK:  //Report CPU SEL to BMC>
              
      //Fix Hygon CPU inject tool can not sent SEL to BMC +>
      //if((McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val)&&
      //              (McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt == MCA_SYND_TYPE_INTERNAL_ERR)){
      if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val){
      // Fix Hygon CPU inject tool can not sent SEL to BMC ->
                
		UINT16 McaErrorCode;
		DEBUG((EFI_D_ERROR,"\n=========== This is a Processor Error Start ===========\n"));
            SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;

            SelRecord->TimeStamp = 0;
            SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
            SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
            SelRecord->EvMRevision = IPMI_EVM_REVISION;
            SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
                
            // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
            // ---------------------------- start --------------------------------------------------
        SelRecord->SensorType = 0x07; // OEM Command,  Processor Error Type = 0x07
        SelRecord->SensorNumber = 0x0;

        //Report CPU SEL to BMC follow SEL_format_ver_0.08 +>
        // Correctable / Uncorrectable error
        if((McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.UC == 1) || (McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.PCC == 1)){
             SelRecord->OEMEvData1 =  0xA0 | 0x0b; //Uncorrectable // follow bmc advice, modify EventData1>
         } else {
             SelRecord->OEMEvData1 =  0xA0 | 0x0c; //Correctable // follow bmc advice, modify EventData1>
         }            
            
            
		switch (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber) {
		
          case 0:
          case 1:
          case 2: 
          case 3: 
          case 5: 
          case 6: 
		      //SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | 1;
              SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber);
              break;
				  
		  case 7: 
		  case 8: 
		  case 9: 
		  case 10: 
		  case 11: 			  
		  case 12: 
		  case 13: 
		  case 14: 
		      SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 7;
              break;
				  				
          case 17:  
              SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 8;
              break;
				  
		  case 18:  
		      SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 9;
              break;
				  				
          case 19:  
              SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 10;
              break;
				  
		  case 20: 
		      SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 11;
		       break;
		  case 21:
		      SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 12;
              break;
                  
		  case 22:	
		      SelRecord->OEMEvData2 = (McaErrorRecord->CpuInfo.SocketId << 7) | (McaErrorRecord->CpuInfo.DieId << 4) | 13;
			  break;
				  
		  default:
             break;						
		}
        //Report CPU SEL to BMC ->         
        //LinkId = (McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation & 0x1F)>> 2;
            
        //Check Error code types
        McaErrorCode = (UINT16)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCode;
        if (((McaErrorCode & TLB_ERROR_MASK) >> TLB_ERROR_CHK_SHIFT) == 1) {
          SelRecord->OEMEvData3 |= 1; //TLB
        } else if(((McaErrorCode & MEM_ERROR_MASK) >> MEM_ERROR_CHK_SHIFT) == 1) {
          SelRecord->OEMEvData3 |= 2; // Memory 
        } else if (((McaErrorCode & PP_ERROR_MASK) >> PP_ERROR_CHK_SHIFT) == 1) {
          SelRecord->OEMEvData3 |= 3; // Bus
        } else if (((McaErrorCode & INT_ERROR_MASK) >> INT_ERROR_CHK_SHIFT) == 1) {
          SelRecord->OEMEvData3 |= 4; // Internal Unclassified
        } else {
		//Report CPU SEL to BMC+>
        SelRecord->OEMEvData3 |= 0; //Unknow Error Type
		//Report CPU SEL to BMC ->            }
		//<Update RAS Module for Processor SEL Report ->
		DEBUG((EFI_D_ERROR,"[OemDbg]: OEMEvData1 = 0x%x, OEMEvData2 = 0x%x, OEMEvData3 = 0x%x,\n", SelRecord->OEMEvData1, SelRecord->OEMEvData2,SelRecord->OEMEvData3));
		}
        // Report CPU SEL to BMC +>
          Status = EFI_SUCCESS;
        // Report CPU SEL to BMC ->
		DEBUG((EFI_D_ERROR,"\n=========== This is a Processor Error End ===========\n"));
        // ---------------------------- end ----------------------------------------------------
      } else {
        Status = EFI_NOT_FOUND;
      }
      break;  // End Of Processor Error
      // Log Processor Error to BMC SEL ->
      default:
        Status = EFI_NOT_FOUND;
        break;
    }
    
    return  Status;
}


/**
    Translate/convert, if possible, error entry from NBIO to Ipmi format

    @param RAS_NBIO_ERROR_INFO *NbioErrorRecord - pointer to NBIO error structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

EFI_STATUS
NbioToIpmi(
  IN  RAS_NBIO_ERROR_INFO   *NbioErrorRecord,
  IN OUT UINT8              *ErrorBuffer
  )
{
    EFI_STATUS                  Status;
    IPMI_SEL_EVENT_RECORD_DATA  *SelRecord;
    
    Status = EFI_SUCCESS;
    
    if((NbioErrorRecord == NULL)||(ErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
    
    SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
                    
    SelRecord->TimeStamp = 0;
    SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
    SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
    SelRecord->EvMRevision = IPMI_EVM_REVISION;
    SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

// PORTING PORTING    
    SelRecord->SensorType = 0x00;
    SelRecord->SensorNumber = 0x00; // Correctable

    SelRecord->OEMEvData1 = 0x00;
    SelRecord->OEMEvData2 = 0x00;
    SelRecord->OEMEvData3 = 0x00;
    
    Status = EFI_NOT_FOUND;
// -----------------------                            
    return Status;
}

/**
    Translate/convert, if possible, error entry from PCIe to Ipmi format

    @param GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry - pointer to GENERIC_PCIE_AER_ERR_ENTRY structure
    @param UINT8              *ErrorBuffer - buffer with Ipmi error entry, only data

    @retval EFI_SUCCESS - Ipmi error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

EFI_STATUS
PcieToIpmi(
  IN  GENERIC_PCIE_AER_ERR_ENTRY *GenPcieAerErrEntry,
  IN OUT UINT8          *ErrorBuffer
  )
{
    EFI_STATUS                  Status;
    IPMI_SEL_EVENT_RECORD_DATA  *SelRecord;

    if((GenPcieAerErrEntry == NULL)||(ErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
    
    SelRecord = (IPMI_SEL_EVENT_RECORD_DATA*)ErrorBuffer;
                    
    SelRecord->TimeStamp = 0;
    SelRecord->RecordType = IPMI_SEL_SYSTEM_RECORD;
    SelRecord->GeneratorId = (UINT16)EFI_GENERATOR_ID(SMI_HANDLER_SOFTWARE_ID);
    SelRecord->EvMRevision = IPMI_EVM_REVISION;
    SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

// PORTING PORTING    
    SelRecord->SensorType = 0x13; //SEL_SENS_TYPE_CRIT_ERR    
    SelRecord->SensorNumber = 0x00; // Correctable
    if(GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_RECOVERABLE || GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_FATAL)
    {
        SelRecord->OEMEvData1 = 0xA0 | 0x05 ;//CRITICAL_INTERRUPT_PCI_SERR
    } else if (GenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity == ERROR_SEVERITY_CORRECTED)
    {
        SelRecord->OEMEvData1 = 0xA0 | 0x04 ;//CRITICAL_INTERRUPT_PCI_PERR    	
    } else {
    	SelRecord->OEMEvData1 = 0xA0 | 0x04 ;//CRITICAL_INTERRUPT_PCI_PERR    	
    }
    SelRecord->OEMEvData2 = (UINT8)GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.PrimaryBus;
    SelRecord->OEMEvData3 = (UINT8)( GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Device << 3 | 
                                        GenPcieAerErrEntry->PcieAerErrorSection.DeviceId.Function);

    Status = EFI_SUCCESS;
// -------------------------------
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

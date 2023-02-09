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
 
/** @file PlatformRasSmbiosLib.c
    Translate Error Log entry from AMD MCA and NBIO format to Smbios type 15

**/

#include <AmiDxeLib.h>
#include <Protocol/SmBiosElogSupport.h>
#include <Library/PlatformRasIpmiLib.h>
#include <Library/PlatformRasSmbiosLib.h>
#include <Library/PlatformRasCommonLib.h>
#include <Token.h> //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>


extern  UINT32  SymbolSize;


/**
    Translate/convert, if possible, error entry from MCA to Smbios format

    @param RAS_MCA_ERROR_INFO    *McaErrorRecord - pointer to MCA error structure
    @param UINT8              *SmbiosErrorBuffer - buffer with smbios error entry, only data

    @retval EFI_SUCCESS - Smbios error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
EFI_STATUS
McaToSmbios(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN OUT UINT8              *SmbiosErrorBuffer,
  IN     UINT8              BankIndex
  )
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
EFI_STATUS
McaToSmbios(
  IN  RAS_MCA_ERROR_INFO    *McaErrorRecord,
  IN  UINTN                 BankIndex,
  IN OUT UINT8              *SmbiosErrorBuffer
  )
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
{
    EFI_STATUS  Status;
    //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
    //UINTN       BankIndex;
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
    UINT8       Symbol;
    UINT16      Syndrome;
    UINTN       EccType = 0;
    UINT8       DramErrorType;
    
    Status = EFI_SUCCESS;
    
    if((McaErrorRecord == NULL)||(SmbiosErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
    //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
    for(BankIndex = 0; BankIndex < 23; BankIndex++){
        if(McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Value == 0){
            continue;
        }
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
    switch (McaErrorRecord->McaBankErrorInfo[BankIndex].McaBankNumber){
        case MCA_UMC0_BANK:
        case MCA_UMC1_BANK:
            DramErrorType = (UINT8)McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt;
            
            if( DramErrorType == DramEccErr ){
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
            
                if(EccType == 1){
                    SmbiosErrorBuffer[0] = EFI_EVENT_LOG_TYPE_SINGLE_BIT_ECC_MEMORY_ERROR;
                }else if (EccType == 2){
                    SmbiosErrorBuffer[0] = EFI_EVENT_LOG_TYPE_MULTI_BIT_ECC_MEMORY_ERROR;
                }else{
                    SmbiosErrorBuffer[0] = EFI_EVENT_LOG_TYPE_PARITY_MEMORY_ERROR;
                }
                SmbiosErrorBuffer[1] = 0x05; // TEMP Smbios handle = 0x0005
                SmbiosErrorBuffer[2] = 0x00;
                Status = EFI_SUCCESS;
            } else if (DramErrorType == AddressCommandParityErr){
                // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
                SmbiosErrorBuffer[0] = 0x90; // OEM type
                SmbiosErrorBuffer[1] = 0x10;
                SmbiosErrorBuffer[2] = 0x01;
            } else if (DramErrorType == WriteDataCrcErr){
                // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
                SmbiosErrorBuffer[0] = 0x90; // OEM type
                SmbiosErrorBuffer[1] = 0x11;
                SmbiosErrorBuffer[2] = 0x01;
            } else {
                Status = EFI_NOT_FOUND;
            }
            break; // End of UMC processing
        case  MCA_PIE_BANK:
            if((McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.Val)&&
                    (McaErrorRecord->McaBankErrorInfo[BankIndex].McaStatusMsr.Field.ErrorCodeExt == MCA_SYND_TYPE_INTERNAL_ERR)){
                UINT8   LinkId;
                
                // OEM PORTING NOTE - below values are just dummy, need to map with existing OEM error list
                // ---------------------------- start --------------------------------------------------
                SmbiosErrorBuffer[0] = 0x90; // OEM type
                SmbiosErrorBuffer[1] = (McaErrorRecord->CpuInfo.SocketId << 4) | McaErrorRecord->CpuInfo.DieId;
                LinkId = (McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation & 0x1F)>> 2;
                SmbiosErrorBuffer[2] = LinkId << 4;
                // Correctable / Uncorrectable error
                if((McaErrorRecord->McaBankErrorInfo[BankIndex].McaSyndMsr.Field.ErrorInformation & 0x3) == 0x01){
                    // Correctable
                    SmbiosErrorBuffer[2] |= 0x1;
                } else {
                    // Uncorrectable
                    SmbiosErrorBuffer[2] |= 0x2;
                }
                // ---------------------------- end ----------------------------------------------------
            } else {
                Status = EFI_NOT_FOUND;
            }
            break;
        default:
            Status = EFI_NOT_FOUND;
            break;
    }
        if(Status == EFI_SUCCESS){
            //break;//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
        }
      //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
    }
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
    
    return  Status;
}

/**
    Translate/convert, if possible, error entry from NBIO to Smbios format

    @param RAS_NBIO_ERROR_INFO *NbioErrorRecord - pointer to NBIO error structure
    @param UINT8              *SmbiosErrorBuffer - buffer with smbios error entry, only data

    @retval EFI_SUCCESS - Smbios error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

EFI_STATUS
NbioToSmbios(
  IN  RAS_NBIO_ERROR_INFO   *NbioErrorRecord,
  IN OUT UINT8              *SmbiosErrorBuffer
  )
{
    EFI_STATUS  Status;
    
    Status = EFI_SUCCESS;
    
    if((NbioErrorRecord == NULL)||(SmbiosErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
    
    SmbiosErrorBuffer[0] = EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR;
    SmbiosErrorBuffer[1] = 0x06; // TEMP Smbios handle = 0x0006
    SmbiosErrorBuffer[2] = 0x00;
            
    return Status;
}


/**
    Translate/convert, if possible, error entry from NBIO to Smbios format

    @param PCIE_PORT_PROFILE *PciePortProfile - pointer to PCIE_PORT_PROFILE structure
    @param UINT8             *SmbiosErrorBuffer - buffer with smbios error entry, only data

    @retval EFI_SUCCESS - Smbios error log entry created
    @retval EFI_INVALID_PARAMETER - input param contains null pointer
    @retval EFI_NOT_FOUND - translation cannot be done

**/

EFI_STATUS
PcieToSmbios(
  IN  PCIE_PORT_PROFILE *PciePortProfile,
  IN OUT UINT8          *SmbiosErrorBuffer
  )
{
    EFI_STATUS  Status;
    
    Status = EFI_SUCCESS;
    
    if((PciePortProfile == NULL)||(SmbiosErrorBuffer == NULL)){
        return  EFI_INVALID_PARAMETER;
    }
    
    SmbiosErrorBuffer[0] = EFI_EVENT_LOG_TYPE_PCI_PARITY_ERROR;
    SmbiosErrorBuffer[1] = 0x06; // TEMP Smbios handle = 0x0006
    SmbiosErrorBuffer[2] = 0x00;
            
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

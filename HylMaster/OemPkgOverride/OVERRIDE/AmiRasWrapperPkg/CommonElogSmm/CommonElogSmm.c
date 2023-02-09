//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
 
/** @file CommonElogSmm.c
    ErrorLog helper driver   

**/

#include "Token.h"
#include <AmiDxeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PlatformRasCommonLib.h>
#include <Library/PlatformRasIpmiLib.h>
#include <Library/PlatformRasSmbiosLib.h>
#include "Protocol/AmdRasSmmProtocol.h"
#include "Protocol/AmiHygonElogProtocol.h"
#include "Protocol/GenericElogProtocol.h"
//#include "Protocol/GpnvRedirProtocol.h"
#include <Protocol/SmBiosElogSupport.h>

typedef struct {
    EFI_SM_ELOG_PROTOCOL    *RedirProtocol;
    EFI_SM_ELOG_TYPE        ProtocolType;
    BOOLEAN                 Enabled;
} REDIRECT_ENTRY;

typedef struct {
    REDIRECT_ENTRY  RedirectEntry[EfiSmElogMax];
}COMMON_ELOG_PRIVATE;

//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
EFI_STATUS
SendElogEventMca (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo,
  IN  UINT8     BankIndex
  );
#else//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
EFI_STATUS
SendElogEventMca (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo
  );
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
  
EFI_STATUS
SendElogEventNbio (
  IN  UINT8 *ErrorRecord
  );

EFI_STATUS
SendElogEventPcie (
  IN  UINT8 *ErrorRecord
  );

UINT8                       ErrorLogData[0x80];
COMMON_ELOG_PRIVATE         CommonElogPrivate;
AMD_RAS_SMM_PROTOCOL        *AmdRasSmmProtocol = NULL;
EFI_SM_ELOG_REDIR_PROTOCOL  *EfiRedirElogProtocol;
VOID                        *gRegistration;
VOID                        *gRegistrationAmdRas;
UINT32                      SymbolSize;


STATIC AMI_HYGON_ELOG_PROTOCOL    AmiHygonElogProtocol = {
  SendElogEventMca,
  SendElogEventNbio,
  SendElogEventPcie
};

//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
EFI_STATUS
SendElogEventMca (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo,
  IN  UINT8     BankIndex
)
#else //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
EFI_STATUS
SendElogEventMca (
  IN  UINT8     *ErrorRecord,
  IN  DIMM_INFO *DimmInfo
)
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
{
    EFI_STATUS  Status;
    RAS_MCA_ERROR_INFO          *RasMcaErrorInfo;
    EFI_SM_ELOG_REDIR_PROTOCOL  *CurrRedirProtocol;
    UINT64                      RecordId;
    UINTN                       EventDataSize;
    UINTN                       EntryIndex;
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
	//UINTN                       BankIndex;
#else
    UINTN                       BankIndex;
#endif //<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>    
    //DEBUG((EFI_D_ERROR, "SendElogEventMca: == ENTER ==\n"));
    
    RasMcaErrorInfo = (RAS_MCA_ERROR_INFO*) ErrorRecord;
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
    Status = McaToSmbios(RasMcaErrorInfo, (UINT8*)&ErrorLogData[0],BankIndex);
#else
    for(BankIndex = 0; BankIndex < 23; BankIndex++){
        if(RasMcaErrorInfo->McaBankErrorInfo[BankIndex].McaStatusMsr.Value == 0){
            continue;
        }
        Status = McaToSmbios(RasMcaErrorInfo, BankIndex, (UINT8*)&ErrorLogData[0]);
#endif 
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
        if(Status == EFI_SUCCESS){
            for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
                if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmSMBIOS){
                    CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                    if (CurrRedirProtocol != NULL) {
                        EventDataSize = 0;
                        Status = CurrRedirProtocol->SetEventLogData(
                                                      CurrRedirProtocol,   // This
                                                      &ErrorLogData[0],    // *ElogData
                                                      EfiElogSmSMBIOS,     // DataType
                                                      FALSE,               // AlertEvent
                                                      EventDataSize,       // DataSize
                                                      &RecordId            // *RecordId
                                                      );
                        //DEBUG((EFI_D_ERROR, "SendElogEventMca: SetEventLogData for Smbios-%r\n", Status));
                    }
                    break;
                }
            }
        }
    
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
    Status = McaToIpmi(RasMcaErrorInfo, (UINT8*)&ErrorLogData[0], DimmInfo, BankIndex);
#else
	Status = McaToIpmi(RasMcaErrorInfo, BankIndex, (UINT8*)&ErrorLogData[0], DimmInfo);
#endif
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3>
        if(Status == EFI_SUCCESS){
            for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
                if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmIPMI){
                    CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                    if (CurrRedirProtocol != NULL) {
                        EventDataSize = 0;
                        Status = CurrRedirProtocol->SetEventLogData(
                                                      CurrRedirProtocol,   // This
                                                      &ErrorLogData[0],    // *ElogData
                                                      EfiElogSmIPMI,       // DataType
                                                      FALSE,               // AlertEvent
                                                      EventDataSize,       // DataSize
                                                      &RecordId            // *RecordId
                                                      );
                        //DEBUG((EFI_D_ERROR, "SendElogEventMca: SetEventLogData for Ipmi-%r\n", Status));
                    }
                    break;
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 +>
#if defined(MEMORY_LEAKY_BUCKET_UPDATE) && (MEMORY_LEAKY_BUCKET_UPDATE == 1)
				//}
#else
                }
#endif
//<Lvshh001-20210510 Optimized Memory Leaky Bucket_3 ->
            }
        }
    }
    return  EFI_SUCCESS;
}

EFI_STATUS
SendElogEventNbio (
  IN  UINT8 *ErrorRecord
  )
{
    EFI_STATUS          Status;
    RAS_NBIO_ERROR_INFO *RasNbioErrorInfo;
    EFI_SM_ELOG_REDIR_PROTOCOL  *CurrRedirProtocol;
    UINT64              RecordId;
    UINTN               EventDataSize;
    UINTN               EntryIndex;
    
    RasNbioErrorInfo = (RAS_NBIO_ERROR_INFO*)ErrorRecord;
    Status = NbioToSmbios(RasNbioErrorInfo, (UINT8*)&ErrorLogData[0]);

    if(Status == EFI_SUCCESS){
        for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
            if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmSMBIOS){
                CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                if (CurrRedirProtocol != NULL) {
                    EventDataSize = 0;
                    Status = CurrRedirProtocol->SetEventLogData(
                                                  CurrRedirProtocol,   // This
                                                  &ErrorLogData[0],    // *ElogData
                                                  EfiElogSmSMBIOS,     // DataType
                                                  FALSE,               // AlertEvent
                                                  EventDataSize,       // DataSize
                                                  &RecordId            // *RecordId
                                                  );
                    //DEBUG((EFI_D_ERROR, "SendElogEventNbio: SetEventLogData for Smbios-%r\n", Status));
                }
                break;
            }
        }
    }

    Status = NbioToIpmi(RasNbioErrorInfo, (UINT8*)&ErrorLogData[0]);
    if(Status == EFI_SUCCESS){
        for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
            if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmIPMI){
                CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                if (CurrRedirProtocol != NULL) {
                EventDataSize = 0;
                Status = CurrRedirProtocol->SetEventLogData(
                                                CurrRedirProtocol,   // This
                                                &ErrorLogData[0],    // *ElogData
                                                EfiElogSmIPMI,       // DataType
                                                FALSE,               // AlertEvent
                                                EventDataSize,       // DataSize
                                                &RecordId            // *RecordId
                                            );
                //DEBUG((EFI_D_ERROR, "SendElogEventNbio: SetEventLogData for Ipmi-%r\n", Status));
                }
                break;
            }
        }
    }

    return EFI_SUCCESS;
}

EFI_STATUS
SendElogEventPcie (
  IN  UINT8 *ErrorRecord
  )
{
    EFI_STATUS          Status;
    GENERIC_PCIE_AER_ERR_ENTRY   *GenPcieAerErrEntry;
    EFI_SM_ELOG_REDIR_PROTOCOL  *CurrRedirProtocol;
    UINT64              RecordId;
    UINTN               EventDataSize;
    UINTN               EntryIndex;

    GenPcieAerErrEntry = (GENERIC_PCIE_AER_ERR_ENTRY*)ErrorRecord;
    Status = PcieToSmbios(GenPcieAerErrEntry, (UINT8*)&ErrorLogData[0]);

    if(Status == EFI_SUCCESS){
        for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
            if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmSMBIOS){
                CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                if (CurrRedirProtocol != NULL) {
                    EventDataSize = 0;
                    Status = CurrRedirProtocol->SetEventLogData(
                                                  CurrRedirProtocol,   // This
                                                  &ErrorLogData[0],    // *ElogData
                                                  EfiElogSmSMBIOS,     // DataType
                                                  FALSE,               // AlertEvent
                                                  EventDataSize,       // DataSize
                                                  &RecordId            // *RecordId
                                                  );
                    //DEBUG((EFI_D_ERROR, "SendElogEventPcie: SetEventLogData for Smbios-%r\n", Status));
                }
                        break;
            }
        }
    }

    Status = PcieToIpmi(GenPcieAerErrEntry, (UINT8*)&ErrorLogData[0]);
    if(Status == EFI_SUCCESS){
        for(EntryIndex=0; EntryIndex < EfiSmElogMax; EntryIndex++){
            if(CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType == EfiElogSmIPMI){
                CurrRedirProtocol = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol;
                if (CurrRedirProtocol != NULL) {
                    EventDataSize = 0;
                    Status = CurrRedirProtocol->SetEventLogData(
                                                  CurrRedirProtocol,   // This
                                                  &ErrorLogData[0],    // *ElogData
                                                  EfiElogSmIPMI,       // DataType
                                                  FALSE,               // AlertEvent
                                                  EventDataSize,       // DataSize
                                                  &RecordId            // *RecordId
                                                  );
                    //DEBUG((EFI_D_ERROR, "SendElogEventPcie: SetEventLogData for Ipmi-%r\n", Status));
                }
                break;
            }
        }
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS
AmdRasSmmProtocolCallBack (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle )
{
    EFI_STATUS      Status;
    
    Status = pSmst->SmmLocateProtocol (
                    &gAmdRasSmmProtocolGuid,
                    gRegistrationAmdRas,
                    &AmdRasSmmProtocol );
    return EFI_SUCCESS;
}

EFI_STATUS
RedirElogProtocolCallBack (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle )
{
    EFI_STATUS      Status;
    BOOLEAN         SetElogEnable = TRUE;
    EFI_HANDLE      *HandleBuffer = NULL;
    UINTN           HandleBufferSize = 0;
    UINTN           HandleCount = 0;
    UINTN           HandleIndex;
    UINTN           EntryIndex;
    UINTN           DataType;

    Status = pSmst->SmmLocateProtocol (
                &gEfiRedirElogProtocolGuid,
                gRegistration,
                &EfiRedirElogProtocol );
    
    if ( EFI_ERROR (Status) ) {
        return Status;
    }

    // Get all RedirElogProtocol handles.
    Status = pSmst->SmmLocateHandle (
                    ByProtocol,
                    &gEfiRedirElogProtocolGuid,
                    NULL,
                    &HandleBufferSize,
                    HandleBuffer );
    
    if(Status == EFI_BUFFER_TOO_SMALL){
        // Allocate memory for Handle buffer
        Status = pSmst->SmmAllocatePool(
                                    EfiRuntimeServicesData,
                                    HandleBufferSize,
                                    (VOID**)&HandleBuffer);
        if (Status == EFI_SUCCESS) {
            Status = pSmst->SmmLocateHandle (
                            ByProtocol,
                            &gEfiRedirElogProtocolGuid,
                            NULL,
                            &HandleBufferSize,
                            HandleBuffer );
            if(Status == EFI_SUCCESS){
                HandleCount = (HandleBufferSize/sizeof(EFI_HANDLE));

                // Store collected info
                ZeroMem(&CommonElogPrivate, sizeof(CommonElogPrivate));
                EntryIndex = 0;
                    
                for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++){
                        
                    Status = pSmst->SmmHandleProtocol (
                                            (EFI_SM_ELOG_PROTOCOL*)(HandleBuffer[HandleIndex]),
                                            &gEfiRedirElogProtocolGuid,
                                            &CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol
                                            );
                    if((Status != EFI_SUCCESS)||(CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol == NULL)) continue;
                    for(DataType = 0; DataType < EfiSmElogMax; DataType++){
                        Status = CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol->ActivateEventLog(
                                            CommonElogPrivate.RedirectEntry[EntryIndex].RedirProtocol,    // This
                                            DataType,                                                     // DataType
                                            NULL,                                                         // *EnableElog
                                            &CommonElogPrivate.RedirectEntry[EntryIndex].Enabled          // *ElogStatus
                                            );
                        if(Status == EFI_NOT_FOUND) continue;
                        CommonElogPrivate.RedirectEntry[EntryIndex].ProtocolType = DataType;
                        EntryIndex++;
                        break;
                    }
                }
            }
        }
        if(HandleBuffer != NULL){
            pSmst->SmmFreePool(HandleBuffer);
        }
    }
    
    return EFI_SUCCESS;
}    

/**
    CommonElogSmm driver entry point
               
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Return the EFI  Status
**/

EFI_STATUS
CommonElogSmmInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS  Status;
    EFI_HANDLE  Handle = NULL;
    
    Status = InitAmiSmmLib(ImageHandle, SystemTable);
    if (EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR, "CommonElogSmmInit: InitAmiSmmLib error - %r\n", Status));
    }
    
    SymbolSize = GetSymbolSize();
    
    Status = pSmst->SmmInstallProtocolInterface(
               &Handle,
               &gAmiHygonElogProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &AmiHygonElogProtocol
           );
    
    Status = pSmst->SmmRegisterProtocolNotify (
                &gEfiRedirElogProtocolGuid,
                RedirElogProtocolCallBack,
                &gRegistration );
    //DEBUG((EFI_D_LOAD, "CommonElogSmmInit: SmmRegisterProtocolNotify for EfiRedirElog - %r \n", Status));
    Status = pSmst->SmmRegisterProtocolNotify (
                    &gAmdRasSmmProtocolGuid,
                    AmdRasSmmProtocolCallBack,
                    &gRegistrationAmdRas );
    
    Status = EFI_SUCCESS;
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

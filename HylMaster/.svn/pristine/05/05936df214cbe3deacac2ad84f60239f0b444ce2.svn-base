//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SmmSmbiosElogInitFuncs.c
    SmbiosElog Initialization Functions Implementation

**/
//---------------------------------------------------------------------------

#include "SmbiosElog.h"
#include <Protocol/FlashProtocol.h>
#include <Library/SmmServicesTableLib.h>
#include "DxeSmmRedirFuncs.h"
#include <Protocol/SmmReportStatusCodeHandler.h>
#include <Library/AmiBufferValidationLib.h>

//---------------------------------------------------------------------------

// Global pointer to local information.  For Non-SMM version of component,
// Pointer should be updated when virtual memory mode is entered.

EFI_SMBIOS_ELOG_INSTANCE_DATA     *mRedirSmbiosPrivate;
EFI_GUID                          gEfiSetupVariableGuid = SETUP_GUID;
UINT64                            *mSmbiosRecordId;
FLASH_PROTOCOL                    *Flash=NULL;

// Prototypes for Functions Used in Init

BOOLEAN IsSmbiosElogEnabled(VOID);
EFI_STATUS
EFIAPI
DxeSmmRedirFuncsHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  );

// ENTRY POINT

/**

    Entry point of Smbios Elog Driver

    @param ImageHandle - The Image handle of this driver.
    @param SystemTable - The pointer of EFI_SYSTEM_TABLE.

    @return EFI_STATUS 
    @retval EFI_SUCCESS   - The driver is successfully initialized.
    @retval EFI_NOT_FOUND - Needed protocols were not found.
    @retval EFI_NOT_READY - AmiSmbiosElogProtocol is not found.
    @retval Others        - Error status of locating FlashSmmProtocol.

**/

EFI_STATUS
InitializeSmmSmbiosElog (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable )
{
    EFI_STATUS                           Status = EFI_SUCCESS;
    EFI_HANDLE                           NewHandle;
    AMI_SMBIOS_ELOG_SUPPORT_PROTOCOL     *SmbiosProtocol = NULL;
    UINT8                                *PointerToInstanceData;
    EFI_SMM_RSC_HANDLER_PROTOCOL         *SmmRscHandlerProtocol = NULL;

    InitAmiSmmLib (ImageHandle, SystemTable);

    if (IsSmbiosElogEnabled() == FALSE)
        return EFI_SUCCESS;

    // Locating the SMM flash protocol
    Status = gBS->LocateProtocol (&gFlashSmmProtocolGuid, NULL,(VOID **) &Flash);

    if (EFI_ERROR(Status)) {
        return Status;
    }

    // Allocate Memory for SMM Private Data

    Status = gSmst->SmmAllocatePool (
                            EfiRuntimeServicesData,
                            sizeof (EFI_SMBIOS_ELOG_INSTANCE_DATA),
                            (void **)&mRedirSmbiosPrivate 
                            );

    ASSERT_EFI_ERROR (Status);

    Status = gSmst->SmmAllocatePool (
                            EfiRuntimeServicesData,
                            sizeof(UINT64), 
                            (VOID **)&mSmbiosRecordId
                            );

    ASSERT_EFI_ERROR (Status);

    // While in SMM Save Pointer of where Local Memory is so that we are able to
    // see and read the local memory copy and all the variables describing the 
    // current state. If not available from Regular memory version of component, 
    // this SMM version can not run.

    Status = gBS->LocateProtocol (&gAmiSmbiosElogSupportGuid, NULL, (VOID **)&SmbiosProtocol);
    if (Status != EFI_SUCCESS)
       return EFI_NOT_READY;

    PointerToInstanceData = SmbiosProtocol->Something;

    // Copy the all the local data of non SMM into SMM

    MemCpy(mRedirSmbiosPrivate,PointerToInstanceData,sizeof(EFI_SMBIOS_ELOG_INSTANCE_DATA));

    // Now install the SMM Redirect Protocol

    mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog  = EfiActivateSmbiosElog;
    mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData = EfiEraseSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.GetEventLogData   = EfiGetSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.SetEventLogData   = EfiSetSmbiosElogData;
    NewHandle = NULL;

    Status = gSmst->SmmInstallProtocolInterface (
                      &NewHandle,
                      &gEfiRedirElogProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mRedirSmbiosPrivate->SmbiosElog 
                      );

    ASSERT_EFI_ERROR (Status);

    if (!EFI_ERROR(Status)) {

        // Register SMM DxeSmmRedirFuncsHandler SMI handler

        NewHandle = NULL;
        Status = gSmst->SmiHandlerRegister (DxeSmmRedirFuncsHandler, &gAmiSmmDummyProtocolRedirGuid, &NewHandle);
        ASSERT_EFI_ERROR (Status);

        // Register the dummy protocol to notify that SMM Redir Protocol is installed

        NewHandle = NULL;
        Status = gBS->InstallProtocolInterface (
                           &NewHandle,
                           &gAmiSmmDummyProtocolRedirGuid,
                           EFI_NATIVE_INTERFACE,
                           NULL
                           );
        ASSERT_EFI_ERROR (Status);
    }

    Status = gSmst->SmmLocateProtocol (
                           &gEfiSmmRscHandlerProtocolGuid,
                           NULL,
                           (VOID **) &SmmRscHandlerProtocol
                           );

    if (!EFI_ERROR (Status)) {
        // Register the callback function for ReportStatusCode() notification
        Status = SmmRscHandlerProtocol->Register ((EFI_SMM_RSC_HANDLER_CALLBACK)GpnvStatusCodeReportWorker);
        DEBUG((DEBUG_INFO, "Callback function registration for ReportStatusCode() notification Status = %r\n", Status));
    }

    return EFI_SUCCESS;
}

/**

    This handler get control when SMM communicate is called.This function 
    calls different API's depending on the header present.

    @param DispatchHandle  - The unique handle assigned to this handler by
                             SmiHandlerRegister() 
    @param RegisterContext - Points to an optional handler context which was
                             specified when the handler was registered.
    @param CommBuffer      - A pointer to a collection of data in memory
    @param CommBufferSize  - The size of CommBuffer

    @return EFI_STATUS
    @retval EFI_SUCCESS - API is called successfully.

**/
EFI_STATUS
EFIAPI
DxeSmmRedirFuncsHandler (
  IN     EFI_HANDLE   DispatchHandle,
  IN     CONST VOID   *RegisterContext,
  IN OUT VOID         *CommBuffer,
  IN OUT UINTN        *CommBufferSize
  )
{
    EFI_STATUS                                      Status = EFI_SUCCESS;
    SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA            *RedirFuncsSetCommunicateData;
    SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA            *RedirFuncsGetCommunicateData;
    SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA          *RedirFuncsEraseCommunicateData;
    SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA       *RedirFuncsActivateCommunicateData;

    ASSERT (CommBuffer != NULL);

    RedirFuncsSetCommunicateData = (SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA *)CommBuffer;
    //
    // Validate if CommBuffer is of Non-SMRAM region before using it,
    // in order to avoid SMRAM data corruption in SMI handlers.
    //
    Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsSetCommunicateData, sizeof (SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA));
    if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsSetCommunicateData Status: %r\n", __FUNCTION__, Status));
        return EFI_SUCCESS;
    }

    switch (RedirFuncsSetCommunicateData->FunctionNumber) {
        case SMM_REDIR_FUNCS_SET_COMMUNICATE:
            if ((VOID*)RedirFuncsSetCommunicateData->ElogData != NULL) {
                //
                // Validate if ElogData is of Non-SMRAM region before using it,
                // in order to avoid SMRAM data corruption in SMI handlers.
                //
                Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsSetCommunicateData->ElogData, 0xFF);
                if (EFI_ERROR (Status)) {
                    DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsSetCommunicateData->ElogData Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }
            if ((VOID*)RedirFuncsSetCommunicateData->RecordId != NULL) {
                //
                // Validate if RecordId is of Non-SMRAM region before using it,
                // in order to avoid SMRAM data corruption in SMI handlers.
                //
                Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsSetCommunicateData->RecordId, sizeof (UINT64));
                if (EFI_ERROR (Status)) {
                    DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsSetCommunicateData->RecordId Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }

            Status = mRedirSmbiosPrivate->SmbiosElog.SetEventLogData (
                                                        &mRedirSmbiosPrivate->SmbiosElog,
                                                        RedirFuncsSetCommunicateData->ElogData,
                                                        RedirFuncsSetCommunicateData->DataType,
                                                        RedirFuncsSetCommunicateData->AlertEvent,
                                                        RedirFuncsSetCommunicateData->Size,
                                                        RedirFuncsSetCommunicateData->RecordId
                                                        );
            RedirFuncsSetCommunicateData->ReturnStatus = Status;
            break;

       case SMM_REDIR_FUNCS_GET_COMMUNICATE:
           RedirFuncsGetCommunicateData = (SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA *)CommBuffer;
           if ((VOID*)RedirFuncsGetCommunicateData->ElogData != NULL) {
               //
               // Validate if ElogData is of Non-SMRAM region before using it,
               // in order to avoid SMRAM data corruption in SMI handlers.
               //
               Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsGetCommunicateData->ElogData, 0xFF);
               if (EFI_ERROR (Status)) {
                   DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsGetCommunicateData->ElogData Status: %r\n", __FUNCTION__, Status));
                   return EFI_SUCCESS;
               }
           }
           if ((VOID*)RedirFuncsGetCommunicateData->Size != NULL) {
               //
               // Validate if Size is of Non-SMRAM region before using it,
               // in order to avoid SMRAM data corruption in SMI handlers.
               //
               Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsGetCommunicateData->Size, sizeof (UINTN));
               if (EFI_ERROR (Status)) {
                   DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsGetCommunicateData->Size Status: %r\n", __FUNCTION__, Status));
                   return EFI_SUCCESS;
               }
           }
           if ((VOID*)RedirFuncsGetCommunicateData->RecordId != NULL) {
               //
               // Validate if RecordId is of Non-SMRAM region before using it,
               // in order to avoid SMRAM data corruption in SMI handlers.
               //
               Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsGetCommunicateData->RecordId, sizeof (UINT64));
               if (EFI_ERROR (Status)) {
                   DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsGetCommunicateData->RecordId Status: %r\n", __FUNCTION__, Status));
                   return EFI_SUCCESS;
               }
           }

           Status = mRedirSmbiosPrivate->SmbiosElog.GetEventLogData (
                                                        &mRedirSmbiosPrivate->SmbiosElog,
                                                        RedirFuncsGetCommunicateData->ElogData,
                                                        RedirFuncsGetCommunicateData->DataType,
                                                        RedirFuncsGetCommunicateData->Size,
                                                        RedirFuncsGetCommunicateData->RecordId
                                                        );
           RedirFuncsGetCommunicateData->ReturnStatus = Status;
           break;

        case SMM_REDIR_FUNCS_ERASE_COMMUNICATE:
            RedirFuncsEraseCommunicateData = (SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA *)CommBuffer;
            if ((VOID*)RedirFuncsEraseCommunicateData->RecordId != NULL) {
                //
                // Validate if RecordId is of Non-SMRAM region before using it,
                // in order to avoid SMRAM data corruption in SMI handlers.
                //
                Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsEraseCommunicateData->RecordId, sizeof (UINT64));
                if (EFI_ERROR (Status)) {
                    DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsEraseCommunicateData->RecordId Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }

            Status = mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData (
                                                        &mRedirSmbiosPrivate->SmbiosElog,
                                                        RedirFuncsEraseCommunicateData->DataType,
                                                        RedirFuncsEraseCommunicateData->RecordId
                                                        );
            RedirFuncsEraseCommunicateData->ReturnStatus = Status;
            break;

        case SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE:
            RedirFuncsActivateCommunicateData = (SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA *)CommBuffer;
            if ((VOID*)RedirFuncsActivateCommunicateData->EnableElog != NULL) {
                //
                // Validate if EnableElog is of Non-SMRAM region before using it,
                // in order to avoid SMRAM data corruption in SMI handlers.
                //
                Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsActivateCommunicateData->EnableElog, sizeof (BOOLEAN));
                if (EFI_ERROR (Status)) {
                    DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsActivateCommunicateData->EnableElog Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }
            if ((VOID*)RedirFuncsActivateCommunicateData->ElogStatus != NULL) {
                //
                // Validate if ElogStatus is of Non-SMRAM region before using it,
                // in order to avoid SMRAM data corruption in SMI handlers.
                //
                Status = AmiValidateMemoryBuffer ((VOID*)RedirFuncsActivateCommunicateData->ElogStatus, sizeof (BOOLEAN));
                if (EFI_ERROR (Status)) {
                    DEBUG ((DEBUG_ERROR, "%a AmiValidateMemoryBuffer for RedirFuncsActivateCommunicateData->ElogStatus Status: %r\n", __FUNCTION__, Status));
                    return EFI_SUCCESS;
                }
            }

            Status = mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog (
                                                        &mRedirSmbiosPrivate->SmbiosElog,
                                                        RedirFuncsActivateCommunicateData->DataType,
                                                        RedirFuncsActivateCommunicateData->EnableElog,
                                                        RedirFuncsActivateCommunicateData->ElogStatus
                                                        );
            RedirFuncsActivateCommunicateData->ReturnStatus = Status;
            break;
        default:
            break;
    }
    return EFI_SUCCESS;
}

/**

    This will be called when ReportStatusCode() is called and here we log 
    the events.

    @param CodeType  - Type of Status Code.
    @param Value     - Value to output for Status Code.
    @param Instance  - Instance Number of this status code.
    @param CallerId  - ID of the caller of this status code.
    @param Data      - Optional data associated with this status code.

    @return EFI_STATUS
    @retval EFI_SUCCESS  Status returned from the function.
    @retval Others       Status returned from SetEventLogData.

**/
EFI_STATUS
EFIAPI
GpnvStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
    EFI_STATUS                        Status = EFI_SUCCESS;
    EFI_STATUS_CODE_ELOG_INPUT        ToWrite;

    //
    // GpnvErrorLogging module reports only SMBIOS Event log types and 
    // Error codes that are defined in Platform Initialization(PI) Specification
    //

    if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {

        ToWrite.StatusCodeType = CodeType;
        ToWrite.StatusCodeValue = Value;

        ToWrite.RecordType = EFI_EVENT_LOG_TYPE_OEM_EFI_STATUS_CODE;

        *mSmbiosRecordId = 0;
        Status =mRedirSmbiosPrivate->SmbiosElog.SetEventLogData (
                                                 &mRedirSmbiosPrivate->SmbiosElog,
                                                 (UINT8*)&ToWrite,
                                                 EfiElogRedirSmSMBIOS,
                                                 FALSE,
                                                 sizeof(EFI_STATUS_CODE_ELOG_INPUT),
                                                 mSmbiosRecordId );

    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

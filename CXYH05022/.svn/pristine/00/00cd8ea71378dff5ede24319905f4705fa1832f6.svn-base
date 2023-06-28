//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file DxeSmmRedirFucns.c
    DxeSmmRedirFuncs Implementation

**/

//---------------------------------------------------------------------------

#include <Protocol/SmmCommunication.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include "SmbiosElog.h"

//---------------------------------------------------------------------------

// Global variables

EFI_SMM_COMMUNICATION_PROTOCOL  *gSmmCommunication = NULL;
UINT8                           *gCommunicateBuffer = NULL;
UINTN                            gCommunicateBufferSize;
UINT8                           *gElogData;
UINT64                          *gRecordId;
UINTN                           *gElogDataSize;
BOOLEAN                         *gEnableElog;
BOOLEAN                         *gElogStatus;
/**

    Once SMM version of Redirect protocol is available this function will be 
    called whenever SetSmbiosElogData is called.
    This function calls the SetSmbiosElogData in SMM through SMM communicate.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param ElogData - Pointer to the Event-Log data to be recorded.
    @param DataType - Type of Elog Data that is being recorded.
    @param AlertEvent - This is an indication that the input data type is an
                        Alert.
    @param Size - Size of the data to be logged.
    @param RecordId - Record ID sent by the target.

    @return EFI_STATUS
    @retval EFI_SUCCESS - SmmEfiSetSmbiosElogData is called successfully.

**/
EFI_STATUS
EFIAPI
SmmEfiSetSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL  *This,
    IN  UINT8                       *ElogData,
    IN  EFI_SM_ELOG_REDIR_TYPE      DataType,
    IN  BOOLEAN                     AlertEvent,
    IN  UINTN                       Size,
    OUT UINT64                      *RecordId )
{
     EFI_STATUS                            Status;
     EFI_SMM_COMMUNICATE_HEADER            *SmmCommunicateHeader;
     SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA  *SmmRedirFuncsSetCommunicateData;

// Copy SMM Communicate Header Here

     SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
     CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gAmiSmmDummyProtocolRedirGuid);
     SmmCommunicateHeader->MessageLength   = SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA_SIZE;

//Update the SmmRedirFuncsSetCommunicateData here

     SmmRedirFuncsSetCommunicateData = (SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
     SmmRedirFuncsSetCommunicateData->FunctionNumber = SMM_REDIR_FUNCS_SET_COMMUNICATE;
     SmmRedirFuncsSetCommunicateData->ElogData = gElogData;
     CopyMem (SmmRedirFuncsSetCommunicateData->ElogData, ElogData, Size);
     SmmRedirFuncsSetCommunicateData->DataType = DataType;
     SmmRedirFuncsSetCommunicateData->AlertEvent = AlertEvent;
     SmmRedirFuncsSetCommunicateData->Size = Size;
     SmmRedirFuncsSetCommunicateData->RecordId = gRecordId;
     CopyMem (SmmRedirFuncsSetCommunicateData->RecordId, RecordId, sizeof(UINT64));

// Send data to SMM.

     gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA_SIZE;
     Status = gSmmCommunication->Communicate ( gSmmCommunication, 
                                               gCommunicateBuffer, 
                                               &gCommunicateBufferSize 
                                              );

     ASSERT_EFI_ERROR (Status);

// Sending the status from the data present in header


    SmmCommunicateHeader      = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmRedirFuncsSetCommunicateData = (SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    CopyMem (ElogData, SmmRedirFuncsSetCommunicateData->ElogData, Size);
    CopyMem (RecordId, SmmRedirFuncsSetCommunicateData->RecordId, sizeof(UINT64));

    return  SmmRedirFuncsSetCommunicateData->ReturnStatus;

}

/**

    Once SMM version of Redirect protocol is available this function will be 
    called whenever GetSmbiosElogData is called.
    This function calls the GetSmbiosElogData in SMM through SMM communicate.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param ElogData - Pointer to the Event-Log data to be recorded.
    @param DataType - Type of Elog Data that is being recorded.
    @param Size - Size of the data to be logged.
    @param RecordId - Record ID sent by the target.

    @return EFI_STATUS
    @retval EFI_SUCCESS - SmmEfiGetSmbiosElogData is called successfully.

**/

EFI_STATUS
EFIAPI
SmmEfiGetSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL  *This,
    IN  UINT8                       *ElogData,
    IN  EFI_SM_ELOG_REDIR_TYPE      DataType,
    OUT UINTN                       *Size,
    OUT UINT64                      *RecordId )
{
    EFI_STATUS                             Status;
    EFI_SMM_COMMUNICATE_HEADER            *SmmCommunicateHeader;
    SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA  *SmmRedirFuncsGetCommunicateData;

// Copy SMM Communicate Header Here

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gAmiSmmDummyProtocolRedirGuid);
    SmmCommunicateHeader->MessageLength   = SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA_SIZE;

//Update the SmmRedirFuncsGetCommunicateData here

    SmmRedirFuncsGetCommunicateData = (SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmRedirFuncsGetCommunicateData->FunctionNumber = SMM_REDIR_FUNCS_GET_COMMUNICATE;
    SmmRedirFuncsGetCommunicateData->ElogData      = gElogData;
    CopyMem (SmmRedirFuncsGetCommunicateData->ElogData, ElogData, *Size);
    
    SmmRedirFuncsGetCommunicateData->DataType = DataType;
    SmmRedirFuncsGetCommunicateData->Size = gElogDataSize;
    CopyMem (SmmRedirFuncsGetCommunicateData->Size, Size, sizeof(UINTN));
    SmmRedirFuncsGetCommunicateData->RecordId = gRecordId;
    CopyMem (SmmRedirFuncsGetCommunicateData->RecordId, RecordId, sizeof(UINT64));

// Send data to SMM.

    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate ( gSmmCommunication, 
                                              gCommunicateBuffer, 
                                              &gCommunicateBufferSize
                                            );
    ASSERT_EFI_ERROR (Status);

// Getting the Status of the protocol from the communicate data

    SmmCommunicateHeader      = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmRedirFuncsGetCommunicateData = (SMM_REDIR_FUNCS_GET_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    CopyMem (Size, SmmRedirFuncsGetCommunicateData->Size, sizeof(UINTN));
    CopyMem (ElogData, SmmRedirFuncsGetCommunicateData->ElogData, *(SmmRedirFuncsGetCommunicateData->Size));
    CopyMem (RecordId, SmmRedirFuncsGetCommunicateData->RecordId, sizeof(UINT64));

    return  SmmRedirFuncsGetCommunicateData->ReturnStatus;

}

/**
    Once SMM version of Redirect protocol is available this function will be 
    called whenever EraseSmbiosElogData is called.
    This function calls the EraseSmbiosElogData in SMM through SMM communicate.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param DataType - Type of Elog Data that is being recorded.
    @param RecordId - Record ID sent by the target.

    @return EFI_STATUS
    @retval EFI_SUCCESS - SmmEfiEraseSmbiosElogData is called successfully.

**/

EFI_STATUS
EFIAPI
SmmEfiEraseSmbiosElogData (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL    *This,
    IN  EFI_SM_ELOG_REDIR_TYPE        DataType,
    OUT UINT64                        *RecordId )
{
    EFI_STATUS                               Status;
    EFI_SMM_COMMUNICATE_HEADER              *SmmCommunicateHeader;
    SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA  *SmmRedirFuncsEraseCommunicateData;

// Copy SMM Communicate Header Here

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gAmiSmmDummyProtocolRedirGuid);
    SmmCommunicateHeader->MessageLength   = SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA_SIZE;

// Update the SmmRedirFuncsEraseCommunicateData here

    SmmRedirFuncsEraseCommunicateData = (SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmRedirFuncsEraseCommunicateData->FunctionNumber = SMM_REDIR_FUNCS_ERASE_COMMUNICATE;
    SmmRedirFuncsEraseCommunicateData->DataType = DataType;
    SmmRedirFuncsEraseCommunicateData->RecordId = gRecordId;
    CopyMem (SmmRedirFuncsEraseCommunicateData->RecordId, RecordId, sizeof(UINT64));

// Send data to SMM.

    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate ( gSmmCommunication, 
                                              gCommunicateBuffer, 
                                              &gCommunicateBufferSize
                                            );
    ASSERT_EFI_ERROR (Status);

// Getting the Status of the protocol from the communicate data

    SmmCommunicateHeader      = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmRedirFuncsEraseCommunicateData = (SMM_REDIR_FUNCS_ERASE_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;

    CopyMem (RecordId, SmmRedirFuncsEraseCommunicateData->RecordId, sizeof(UINT64));
    return  SmmRedirFuncsEraseCommunicateData->ReturnStatus;

}

/**

    Once SMM version of Redirect protocol is available this function will be 
    called whenever ActivateSmbiosElogData is called.
    This function calls the ActivateSmbiosElogData in SMM through SMM communicate.

    @param This - Instance of EFI_SM_ELOG_REDIR_PROTOCOL.
    @param DataType - Type of Elog Data that is being recorded.
    @param EnableElog - Enables (TRUE)/Disables(FALSE) Event Log. If NULL just
                        returns the Current ElogStatus.
    @param ElogStatus - Current (New) Status of Event Log. Enabled (TRUE),
                        Disabled (FALSE).

    @return EFI_STATUS
    @retval EFI_SUCCESS - SmmEfiActivateSmbiosElogData is called successfully.

**/
EFI_STATUS
EFIAPI
SmmEfiActivateSmbiosElog (
    IN  EFI_SM_ELOG_REDIR_PROTOCOL    *This,
    IN  EFI_SM_ELOG_REDIR_TYPE        DataType,
    IN  BOOLEAN                       *EnableElog,
    OUT BOOLEAN                       *ElogStatus)
{
    EFI_STATUS                          Status;
    EFI_SMM_COMMUNICATE_HEADER          *SmmCommunicateHeader;
    SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA     *SmmRedirFuncsActivateCommunicateData;

// Copy SMM Communicate Header Here

    SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)gCommunicateBuffer;
    CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gAmiSmmDummyProtocolRedirGuid);
    SmmCommunicateHeader->MessageLength   = SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA_SIZE;

//Update the SmmRedirFuncsActivateCommunicateData here

    SmmRedirFuncsActivateCommunicateData = (SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    SmmRedirFuncsActivateCommunicateData->FunctionNumber = SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE;
    SmmRedirFuncsActivateCommunicateData->DataType = DataType;
    SmmRedirFuncsActivateCommunicateData->EnableElog = gEnableElog;
    CopyMem (SmmRedirFuncsActivateCommunicateData->EnableElog, EnableElog, sizeof(BOOLEAN));
    SmmRedirFuncsActivateCommunicateData->ElogStatus = gElogStatus;
    CopyMem (SmmRedirFuncsActivateCommunicateData->ElogStatus, ElogStatus, sizeof(BOOLEAN));

// Send data to SMM.

    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA_SIZE;
    Status = gSmmCommunication->Communicate ( gSmmCommunication, 
                                              gCommunicateBuffer, 
                                              &gCommunicateBufferSize 
                                            );
    ASSERT_EFI_ERROR (Status);

// Getting the Status of the protocol from the communicate data

    SmmCommunicateHeader      = (EFI_SMM_COMMUNICATE_HEADER *) gCommunicateBuffer;
    SmmRedirFuncsActivateCommunicateData = (SMM_REDIR_FUNCS_ACTIVATE_COMMUNICATE_DATA *)SmmCommunicateHeader->Data;
    CopyMem (EnableElog, SmmRedirFuncsActivateCommunicateData->EnableElog, sizeof(BOOLEAN));
    CopyMem (ElogStatus, SmmRedirFuncsActivateCommunicateData->ElogStatus, sizeof(BOOLEAN));
    
    return  SmmRedirFuncsActivateCommunicateData->ReturnStatus;

}


/**

    This function locates the SMM communicate and Overrides all the Redirect 
    Functions of the SmbiosElog.

    @param Event - EFI_EVENT_NOTIFY
    @param Context - The Event relative context

    @return VOID

**/
VOID
EFIAPI
DxeSmmRedirFuncsProtocolReady (
  IN  EFI_EVENT                             Event,
  IN  VOID                                  *Context
  )
{
    EFI_STATUS                      Status;

// Locate the SMM communicate protocol

    Status = gBS->LocateProtocol ( &gEfiSmmCommunicationProtocolGuid, 
                                   NULL, 
                                  (VOID **) &gSmmCommunication 
                                 );

    if (EFI_ERROR (Status)) {
        return;
    }


// Allocate memory for variable store.

    gCommunicateBufferSize  = SMM_COMMUNICATE_HEADER_SIZE + SMM_REDIR_FUNCS_SET_COMMUNICATE_DATA_SIZE;
    gCommunicateBuffer      = AllocateRuntimeZeroPool (gCommunicateBufferSize);
    gElogData               = AllocateRuntimeZeroPool (0xFF);
    gRecordId               = AllocateRuntimeZeroPool (sizeof (UINT64));
    gElogDataSize           = AllocateRuntimeZeroPool (sizeof (UINTN));
    gEnableElog             = AllocateRuntimeZeroPool (sizeof (BOOLEAN));
    gElogStatus             = AllocateRuntimeZeroPool (sizeof (BOOLEAN));
    ASSERT (gCommunicateBuffer != NULL);

// Override all the Redirect Functions of the SmbiosElog

    mRedirSmbiosPrivate->SmbiosElog.ActivateEventLog  = SmmEfiActivateSmbiosElog;
    mRedirSmbiosPrivate->SmbiosElog.EraseEventLogData = SmmEfiEraseSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.GetEventLogData   = SmmEfiGetSmbiosElogData;
    mRedirSmbiosPrivate->SmbiosElog.SetEventLogData   = SmmEfiSetSmbiosElogData;

    return;
}

/**

    This function registers callback for the gAmiSmmDummyProtocolRedirGuid.
    This function gets control when SMM Redirect Protocol is available

    @param VOID

    @return EFI_STATUS
    @retval EFI_SUCCESS - Protocol Callback is registered successfully.

**/
EFI_STATUS
DxeSmmRedirFuncsHook ( VOID )
{
    VOID        *DxeSmmRedirFuncsReg;


// Create Notification event for SmmSmbiosRedirProtocol GUID

    EfiCreateProtocolNotifyEvent (
        &gAmiSmmDummyProtocolRedirGuid,
        TPL_CALLBACK,
        DxeSmmRedirFuncsProtocolReady,
        NULL,
        &DxeSmmRedirFuncsReg
        );

    return EFI_SUCCESS;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

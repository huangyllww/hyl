//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PlatformPeiSystemInformation.c

    This code retrieves the system information and prints it to the video/serial output
*/

#include "PlatformPeiSystemInformation.h"
#include <AmiTextOutHob.h>
#include <Library/HobLib.h>

InitializeCpuData (
    IN  CONST EFI_PEI_SERVICES  **PeiServices,
    IN  AMI_PEI_TEXT_OUT        *This
);
InitializeDimmData (
   IN  CONST EFI_PEI_SERVICES   **PeiServices,
   IN  AMI_PEI_TEXT_OUT         *This
);

EFI_STATUS OutputBiosVersion(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);
EFI_STATUS OutputCpuInformation(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);
EFI_STATUS OutputDimmInformation(IN CONST EFI_PEI_SERVICES**, IN EFI_PEI_NOTIFY_DESCRIPTOR*, IN VOID*);

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiBiosVersionNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    OutputBiosVersion
  }
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiCpuNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    OutputCpuInformation
  }
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mPeiDimmNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    OutputDimmInformation
  }
};

EFI_GUID    gVideoTextOutHobGuid = AMI_TEXT_OUT_HOB_GUID;

/**
    Outputs system information to Pei Early Video

    @param  **PeiServices - pointer to the PEI services.
    @param  NotifyDescriptor - pointer to descriptor
    @param  Ppi - void pointer 
    
    @retval EFI_STATUS
*/

EFI_STATUS
OutputBiosVersion (
        IN CONST EFI_PEI_SERVICES    **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
        IN VOID                      *Ppi)
{   
    EFI_STATUS          Status = EFI_SUCCESS;
    AMI_PEI_TEXT_OUT    *EarlyVideoPpi;
    CHAR8               TextString[160];
    CHAR8               PlatformString[20];

    DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));

    Status = (*PeiServices)->LocatePpi(
                        PeiServices,
#if ConsoleOutConsplitter_SUPPORT 
                        &gAmiPeiConsplitterTextOutPpiGuid,
#else
                        &gAmiPeiTextOutPpiGuid,
#endif
                        0,
                        NULL,
                        &EarlyVideoPpi);
    if (EFI_ERROR(Status)) return Status;

    AsciiSPrint (PlatformString, 20, "DhyanaCrb");

    AsciiSPrint (TextString, 160, " System Information \n");
    Status = EarlyVideoPpi->WriteString(EarlyVideoPpi, 0, TextString);

    AsciiSPrint (TextString, 160, " %a System BIOS Version: %a Date: %a \n", PlatformString, CONVERT_TO_STRING(BIOS_TAG), CONVERT_TO_STRING(TODAY));
    Status = EarlyVideoPpi->WriteString(EarlyVideoPpi, 0, TextString);
    Status = EarlyVideoPpi->SetAttribute(EarlyVideoPpi, 0x7, 0, 0); 

    return Status;
}

/**
    Outputs system information (CPU) to Pei Early Video

    @param  **PeiServices - pointer to the PEI services.
    @param  NotifyDescriptor - pointer to descriptor
    @param  Ppi - void pointer 
    
    @retval EFI_STATUS
*/

EFI_STATUS
OutputCpuInformation (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
    EFI_STATUS          Status=EFI_SUCCESS;
    AMI_PEI_TEXT_OUT    *EarlyVideoPpi;

    DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));

    Status = (*PeiServices)->LocatePpi(
            PeiServices,
#if ConsoleOutConsplitter_SUPPORT
            &gAmiPeiConsplitterTextOutPpiGuid,
#else
            &gAmiPeiTextOutPpiGuid,
#endif
            0,
            NULL,
            &EarlyVideoPpi);
    if (EFI_ERROR(Status)) return Status;

    InitializeCpuData (PeiServices, EarlyVideoPpi);
    return Status;
}

/**
    Outputs system information (Memory and DIMMs) to Pei Early Video

    @param  **PeiServices - pointer to the PEI services.
    @param  NotifyDescriptor - pointer to descriptor
    @param  Ppi - void pointer 
    
    @retval EFI_STATUS
*/

EFI_STATUS
OutputDimmInformation (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                      *Ppi)
{
    EFI_STATUS          Status=EFI_SUCCESS;
    AMI_PEI_TEXT_OUT    *EarlyVideoPpi;

    DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));

    Status = (*PeiServices)->LocatePpi(
            PeiServices,
#if ConsoleOutConsplitter_SUPPORT 
            &gAmiPeiConsplitterTextOutPpiGuid,
#else
            &gAmiPeiTextOutPpiGuid,
#endif
            0,
            NULL,
            &EarlyVideoPpi);
    if (EFI_ERROR(Status)) return Status;

    InitializeDimmData (PeiServices, EarlyVideoPpi);

    return Status;
}

/**
    Registers notification for system information components
    
    @param  FileHandle - Pointer to the FFS file header of the image.
    @param  **PeiServices - pointer to the PEI services.
 
    @retval EFI_STATUS
*/

EFI_STATUS
PeiSystemInformationEntryPoint (
    IN        EFI_PEI_FILE_HANDLE   FileHandle,
    IN  CONST EFI_PEI_SERVICES      **PeiServices 
    )
{
        EFI_STATUS          Status = EFI_SUCCESS;
        EFI_PEI_NOTIFY_DESCRIPTOR   *pNotifyList;
        
        DEBUG((EFI_D_INFO,"[PEI System Information] \n"__FUNCTION__));
        
        pNotifyList = &mPeiBiosVersionNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);

        pNotifyList = &mPeiCpuNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
        
        pNotifyList = &mPeiDimmNotifyList[0];
        Status = (*PeiServices)->NotifyPpi(PeiServices, pNotifyList);
        
        return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

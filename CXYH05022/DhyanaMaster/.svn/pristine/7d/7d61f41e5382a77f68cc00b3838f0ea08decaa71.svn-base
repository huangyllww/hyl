/** @file
  Driver source for PEI module of AliIpmiSetupPei.

**/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <IpmiNetFnAppDefinitions.h>
#include <Ppi/IpmiTransportPpi.h>
#include <Library/BaseMemoryLib.h>
#include "AliIpmiSyncSetup.h"


EFI_STATUS
EFIAPI
AliPeiIpmiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnPeiIpmiList = {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiIpmiTransportPpiGuid,
    AliPeiIpmiCallback
};


/**
  ClearValidFlag :

  Function to clear valid flag of 'Set BIOS Configuration Value' command,
  avoid user to get BIOS Configuration with last boot Setup values

  @param[in]    PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi

  @return EFI_STATUS Status
**/
EFI_STATUS
ClearValidFlag(
    IN PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi
)
{
    EFI_STATUS                  Status;
    UINT8                       RequSize;
    UINT8                       RespSize;
    IPMI_SETUP_WRITE_REQUEST    WriteRequest = {0};
    UINT8                       WriteResponse[20];
    UINT8                       Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;

    while (Retry > 0)
    {
        ZeroMem(&WriteRequest, sizeof(IPMI_SETUP_WRITE_REQUEST));
        WriteRequest.Length = 1;
        RequSize = 3; //Only send 'Configuration' byte, BMC will clear all data

        Status = IpmiTransportPpi->SendIpmiCommand(
            IpmiTransportPpi,
            ALI_IPMI_NET_FUN,
            BMC_LUN,
            ALI_IPMI_BIOS_SETCONFIG_TO_BMC,
            (UINT8 *)&WriteRequest,
            RequSize,
            WriteResponse,
            &RespSize);
        if (EFI_SUCCESS == Status)
        {
            break;
        }
        Retry--;
    }
    return Status;
}

/**
  AliPeiIpmiCallback :

  Function to clear valid flag of 'Set BIOS Configuration Value' command,
  avoid user to get BIOS Configuration with last boot Setup values

  @param[in]    EFI_FFS_FILE_HEADER    *FfsHeader
  @param[in]    EFI_PEI_SERVICES       **PeiServices

  @return EFI_STATUS Status
**/
EFI_STATUS
EFIAPI
AliPeiIpmiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
    EFI_STATUS              Status;
    PEI_IPMI_TRANSPORT_PPI  *IpmiTransportPpi;

    DEBUG((EFI_D_INFO, "%a Start\n", __FUNCTION__));

    //
    //Locate IPMI Transport protocol to send commands to BMC.
    //
    Status = (*PeiServices)->LocatePpi (
            (CONST EFI_PEI_SERVICES**)PeiServices,
            &gEfiPeiIpmiTransportPpiGuid,
            0,
            NULL,
            (VOID **)&IpmiTransportPpi );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = ClearValidFlag(IpmiTransportPpi);

    DEBUG((EFI_D_INFO, "%a end with status %r\n", __FUNCTION__, Status));
    return Status;
}


/**
  AliIpmiPeiInit :

  Entry point of AliIpmiPeiInit Module

  @param[in]    EFI_FFS_FILE_HEADER    *FfsHeader
  @param[in]    EFI_PEI_SERVICES       **PeiServices

  @return EFI_STATUS Status
**/
EFI_STATUS EFIAPI AliIpmiPeiInit (
    IN       EFI_PEI_FILE_HANDLE    FileHandle,
    IN CONST EFI_PEI_SERVICES       **PeiServices
)
{
    EFI_STATUS  Status;
    
    Status = PeiServicesNotifyPpi(&mNotifyOnPeiIpmiList);
    
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

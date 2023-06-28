//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

/** @file

  Whea driver entry point and Initialization functions. 
  
  It contains functions to register   software ERST SMI handler and 
  allocate Runtime memory for WHEA private data and WHEA protocol interface. 
  Register event to create ERST acpi table just before Boot 
  (Exit boot services & legacy boot event).
  
*/

#include "WheaInit.h"

WHEA_MODULE_PRIVATE_DATA    *mWheaData;
WHEA_INTERFACE              *mWheaInterface;

EFI_EVENT                   gReadyToBootEvent;

/**
  This function will create ERST Acpi table 
  when ACPI Support Protocol is available 

  @param  Event    The instance of EFI_EVENT.
  @param  Context  Pointer to context.
*/

VOID
SetWheaAcpiTables (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{

    EFI_ACPI_TABLE_PROTOCOL   *AcpiTable = NULL;
    EFI_STATUS                Status=EFI_SUCCESS;
    UINTN                     ErstTblHandle = 0;

    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
    if(EFI_ERROR(Status)) {
        return;
    }
 
    //Add ERST table 
    Status=AcpiTable->InstallAcpiTable(AcpiTable, mWheaData->Erst,sizeof (WHEA_ERST_ACPI_TABLE),&ErstTblHandle);
    DEBUG((EFI_D_INFO, "ERST: AcpiTable->InstallAcpiTable(ERST) = %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    // Close all the events
    gBS->CloseEvent(gReadyToBootEvent);
}

/**
  This function will initialize pointers

  @param ImageHandle The firmware allocated handle for the EFI image.
  @param SystemTable A pointer to the EFI System Table.

  @retval EFI_SUCCESS - returned always
*/

EFI_STATUS
InitializeWheaLib( 
IN EFI_HANDLE           ImageHandle,
IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    if ((SystemTable != NULL) && (SystemTable->BootServices != NULL)) {
        gST = SystemTable;
        gBS=pBS = SystemTable->BootServices;
        gRT = SystemTable->RuntimeServices;
    }  
    return EFI_SUCCESS; 
}

/**
  Entry point for Whea driver. Installs Wheaprotocol and register
  ReadyToBoot callback.

  @param  ImageHandle  - The firmware allocated handle for the EFI image.
  @param  SystemTable - A pointer to the EFI System Table.

  @retval EFI_SUCCESS - returned always
*/

EFI_STATUS
InitializeWhea (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{

    EFI_STATUS  Status;
    EFI_HANDLE  NewHandle = NULL;

    // Initialize the EFI Runtime Library
    InitializeWheaLib(ImageHandle, SystemTable);

    Status = gBS->AllocatePool(EfiRuntimeServicesData,sizeof(WHEA_MODULE_PRIVATE_DATA),&mWheaData);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->AllocatePool(EfiRuntimeServicesData,sizeof(WHEA_INTERFACE),&mWheaInterface);
    if (EFI_ERROR(Status)) {
        return Status;
    }

    mWheaInterface->WheaData= mWheaData;  
    mWheaInterface->Signature = EFI_WHEA_INTERFACE_SIGNATURE;

    Status = gBS->InstallProtocolInterface (
                   &NewHandle,
                   &gEfiWheaProtocolGuid,
                   EFI_NATIVE_INTERFACE,
                   &(mWheaInterface->EfiWheaProtocol)
                   );

    // Register events to create acpi tables just before Boot.
    EfiCreateEventReadyToBootEx (
        TPL_CALLBACK,
        SetWheaAcpiTables,
        NULL,
        &gReadyToBootEvent
        );

    return EFI_SUCCESS;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**        5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093      **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

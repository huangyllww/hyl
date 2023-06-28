
//**********************************************************************
// $Header: DmiVendorChange.c
//
// $Function: Dmi Vendor SMBIOS table is updated here
//
// $Revision:01
//
//**********************************************************************

#include <Token.h>
#include <AmiLib.h>
#include <IndustryStandard/Smbios.h>
#include <Protocol/Smbios.h>
#include <Setup.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

/**
  Callback function executed when the EndOfDxe event group is signaled.

  @param[in] Event      Event whose notification function is being invoked.
  @param[in] Context    The pointer to the notification function's context, which
                        is implementation-dependent.
**/
VOID
EFIAPI
UpdateDmiVendorOnEndOfDxe (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
    EFI_STATUS                      Status;
    SETUP_DATA                      SetupData;
    UINTN                           VariableSize;
    UINT32                          VariableAttribute;
    EFI_GUID                        SetupGuid = SETUP_GUID;
    EFI_SMBIOS_HANDLE               SmbiosHandle;
    EFI_SMBIOS_PROTOCOL             *SmbiosProtocol;
    EFI_SMBIOS_TYPE                 Type;
    EFI_SMBIOS_TABLE_HEADER         *Record;
    UINTN                           StringNumber;
    CHAR8                           *VendorStr = CONVERT_TO_STRING(ALI_DMI_VENDOR_NAME);
    CHAR8                           *ProductStr = CONVERT_TO_STRING(ALI_DMI_SYSTEM_PRODUCT_NAME);
    
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
    if (EFI_ERROR (Status))
    {
        DEBUG ((DEBUG_ERROR , "%a LocateProtocol(gEfiSmbiosProtocolGuid) with Status %r\n", __FUNCTION__, Status));
        return;
    }
    VariableSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable(
             L"Setup",
             &SetupGuid,
             &VariableAttribute,
             &VariableSize,
             &SetupData
             );
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR , "%a GetVariable with Status %r\n", __FUNCTION__, Status));
        return;
    }
    if (SetupData.DmiVendorChange)
    {
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Type = 0x01;
        Status = SmbiosProtocol->GetNext (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &Type,
                    &Record,
                    NULL);
        DEBUG ((DEBUG_INFO, "%a SmbiosProtocol->GetNext Status:%r \n", __FUNCTION__, Status));
        if (!EFI_ERROR(Status)) {
            // Type 1 Manufacturer String
            SmbiosHandle = Record->Handle;
            StringNumber = 0x01;
            Status = SmbiosProtocol->UpdateString (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &StringNumber,
                    VendorStr);
            StringNumber = 0x02;
            Status = SmbiosProtocol->UpdateString (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &StringNumber,
                    ProductStr);    
        }
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Type = 0x02;
        Status = SmbiosProtocol->GetNext (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &Type,
                    &Record,
                    NULL);
        DEBUG ((DEBUG_INFO, "%a SmbiosProtocol->GetNext Status:%r \n", __FUNCTION__, Status));
        if (!EFI_ERROR(Status)) {
            // Type 2 Manufacturer String
            SmbiosHandle = Record->Handle;
            StringNumber = 0x01;
            Status = SmbiosProtocol->UpdateString (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &StringNumber,
                    VendorStr); 
        }
        SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
        Type = 0x03;
        Status = SmbiosProtocol->GetNext (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &Type,
                    &Record,
                    NULL);
        DEBUG ((DEBUG_INFO, "SmbiosProtocol->GetNext Status:%r \n", Status));
        if (!EFI_ERROR(Status)) {
            // Type 3 Manufacturer String
            SmbiosHandle = Record->Handle;
            StringNumber = 0x01;
            Status = SmbiosProtocol->UpdateString (
                    SmbiosProtocol,
                    &SmbiosHandle,
                    &StringNumber,
                    VendorStr); 
        }
    }
    
    Status = gBS->CloseEvent (Event);
    ASSERT_EFI_ERROR (Status);
    return;
}

/**
   The entry function of the DmiVendorChange driver.

   This function updates SMBIOS for different board design.

   @param[in] ImageHandle  The firmware allocated handle for the EFI image.
   @param[in] SystemTable  A pointer to the EFI System Table.

   @retval EFI_SUCCESS     The entry point is executed successfully.
   @retval EFI_UNSUPPORTED Do not support ACPI S3.
   @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS DmiVendorChangeEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{

    EFI_STATUS    Status;
    EFI_EVENT     EndOfDxeEvent;
    
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateDmiVendorOnEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
    return Status;
}

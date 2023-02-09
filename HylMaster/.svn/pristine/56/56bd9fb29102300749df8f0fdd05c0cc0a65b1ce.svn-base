
#include <Token.h>
#include <AmiDxeLib.h>
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Smbios.h>
#include <Protocol/Smbios.h>
#include <Library\UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <OemIpmiNetFnDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootManagerLib.h>
#include <IpmiNetFnAppDefinitions.h>

EFI_SMBIOS_PROTOCOL *mEfiSmbiosProtocol;

/**
  Add the SMBIOS type 130 for recode BIOS minor version(PROJECT_SUB_VERSION)
**/
VOID AddSmbiosType130(VOID)
{
  EFI_STATUS Status;
  SMBIOS_TABLE_TYPE130 *Type130Record;
  EFI_SMBIOS_HANDLE SmbiosHandle;
  CHAR8 BiosMinorVersion[50] = {0};
  CHAR8 BMCMinorVersion[50] = {0};
  UINTN TotalSize;
  UINTN StringOffset;
  UINT8 DataSize;
  EFI_IPMI_TRANSPORT *gIpmiTransport;
  IPMI_GET_DEVICE_ID_RESPONSE GetDeviceIdResponse;

  DEBUG((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));

  /**
  Note:BIOS MINI VERSION
  **/
  Sprintf(BiosMinorVersion, "F00: %a", CONVERT_TO_STRING(PROJECT_SUB_VERSION));

  /**
  Note:Start Get BMC MiniVersion
  **/
  // Locate IpmiTransport protocol
  Status = pBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]location Ipmi Protocol :%r\r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR(Status))
    return;

  DataSize = sizeof(GetDeviceIdResponse);
  Status = gIpmiTransport->SendIpmiCommand(gIpmiTransport, IPMI_NETFN_APP, BMC_LUN, IPMI_APP_GET_DEVICE_ID, NULL, 0, (UINT8 *)&GetDeviceIdResponse, &DataSize);
  if (EFI_ERROR(Status))
    return;

  Sprintf(BMCMinorVersion, "F01: N/A");
  if (GetDeviceIdResponse.AuxFirmwareRevInfo != 0)
    Sprintf(BMCMinorVersion, "F01: V%08x", GetDeviceIdResponse.AuxFirmwareRevInfo);

  /**
  Note:Try SMBios Type 130
  **/
  //SMBIOS type 130 structure process start
  TotalSize = sizeof(SMBIOS_TABLE_TYPE130) + sizeof(SMBIOS_TABLE_STRING) + AsciiStrLen(BiosMinorVersion) + 1 + AsciiStrLen(BMCMinorVersion) + 1 + 1;

  Type130Record = AllocateZeroPool(TotalSize);
  if (Type130Record == NULL)
  {
    DEBUG((EFI_D_ERROR, "Allocating Space for SmBios Type130Record fails...\n"));
    return;
  }

  Type130Record->Hdr.Type = 130;
  Type130Record->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE130);
  Type130Record->Hdr.Handle = 0xFFFF;  //To be updated by SMBIOS driver.
  Type130Record->BiosMinorVersion = 1; /*PROJECT_SUB_VERSION*/
  ;
  Type130Record->BMCMinorVersion = 2; /*PROJECT_SUB_VERSION*/
  ;

  // Append all strings
  StringOffset = Type130Record->Hdr.Length;
  CopyMem((UINT8 *)Type130Record + StringOffset, BiosMinorVersion, AsciiStrLen(BiosMinorVersion));

  CopyMem((UINT8 *)Type130Record + StringOffset + AsciiStrLen(BiosMinorVersion) + 1, BMCMinorVersion, AsciiStrLen(BMCMinorVersion));
  //
  // Creating the SMBIOS Type 130 Structure.
  //
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mEfiSmbiosProtocol->Add(mEfiSmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *)Type130Record);
  DEBUG((EFI_D_INFO, "SmbiosProtocol->Add Type130Record....  Status: %r \n", Status));
  if (EFI_ERROR(Status))
  {
    return;
  }

  DEBUG((EFI_D_INFO, "%a End...\n", __FUNCTION__));
  return;
}

/**
GC_TODO: Update SMBIOS Table For Mini Version

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
VOID
    EFIAPI
    UpdateMiniVersion_SMBios(
        IN EFI_EVENT Event,
        IN VOID *Context)
{
  EFI_STATUS Status;

  DEBUG((EFI_D_ERROR, __FUNCTION__ " Start\n"));
  //Close the event if it is present
  if (Event != NULL)
    pBS->CloseEvent(Event);

  Status = pBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, &mEfiSmbiosProtocol);
  if (EFI_ERROR(Status))
    return;

  AddSmbiosType130();

  return;
}

/**
  This function is the entry point for this driver.  And initializes the notify event of update SMBIOS table

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS OemMiniVersion_Smbios_Entry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;
  VOID *UpdateSmbiosTableReg;
  EFI_EVENT UpdateSmbiosTableEvent;

  DEBUG((EFI_D_ERROR, __FUNCTION__ " Start\n"));

  InitAmiLib(ImageHandle, SystemTable);

  UpdateSmbiosTableEvent = EfiCreateProtocolNotifyEvent(
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      UpdateMiniVersion_SMBios,
      NULL,
      &UpdateSmbiosTableReg);

  DEBUG((EFI_D_ERROR, __FUNCTION__ " End\n"));
  return Status;
}

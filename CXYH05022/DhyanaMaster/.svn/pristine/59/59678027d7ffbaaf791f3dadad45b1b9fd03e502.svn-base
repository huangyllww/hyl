
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
#include "OemMiniVersion.h"
#include "OemLib.h"

EFI_SMBIOS_PROTOCOL *mEfiSmbiosProtocol;

/**
  Add the SMBIOS type 130 for recode BIOS minor version(PROJECT_SUB_VERSION)
**/
VOID AddSmbiosType130(VOID)
{
  EFI_STATUS Status;
  SMBIOS_TABLE_TYPE130 *Type130Record;
  EFI_SMBIOS_HANDLE SmbiosHandle;
  CHAR8 BiosMinorVersion[16] = {0}; //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
  CHAR8 BMCMinorVersion[16] = {0}; //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
  UINT8 BmcValue;
  UINT8 DataSize;
  UINTN TotalSize;
  UINTN StringOffset;
  UINT8 SetBiosMinorStatus;
  UINT8 GetBmcMinorParam = 0x01;
  CHAR8 *pBIOSVersion = NULL;
  EFI_IPMI_TRANSPORT *gIpmiTransport;
  IPMI_SET_BIOS_MINOR_VERSION_REQUEST SetBiosMinorVersion;
  IPMI_GET_MINOR_VERSION_RESPONSE GetBmcFirmwareMinorVersion;

  MODULE_START

  // INIT STRUCT
  MemSet(&SetBiosMinorVersion.ParamSelect, sizeof(IPMI_SET_BIOS_MINOR_VERSION_REQUEST), 0);
  MemSet(&GetBmcFirmwareMinorVersion.BmcMiniversion[0], sizeof(IPMI_GET_MINOR_VERSION_RESPONSE), 0);
  MemSet(&BiosMinorVersion[0],sizeof(BiosMinorVersion),0);  //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
  /**
  Note:BIOS MINI VERSION
  **/
  // INIT DEFAULT
  Sprintf(BMCMinorVersion, "F01: N/A");
  Sprintf(&SetBiosMinorVersion.BiosMiniversion[0], "N/A");  //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
  Sprintf(BiosMinorVersion, "F00: %a", CONVERT_TO_STRING(PROJECT_SUB_VERSION));

  /**
  Note:Start Get BMC MiniVersion BY IPMI
  **/
  // Locate IpmiTransport protocol
  Status = pBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
  
  if (!EFI_ERROR(Status) && (gIpmiTransport!=NULL))
  {
    // Get BMC Minor Version From BMC
    DataSize = sizeof(IPMI_GET_MINOR_VERSION_RESPONSE);
    Status = gIpmiTransport->SendIpmiCommand(
                                gIpmiTransport,
                                NETFN_OEM,
                                BMC_LUN,
                                CMD_OEM_GET_FIRMWARE_VERSION,
                                (UINT8 *)&GetBmcMinorParam,
                                sizeof(GetBmcMinorParam),
                                (UINT8 *)&GetBmcFirmwareMinorVersion,
                                &DataSize);

    if (!EFI_ERROR(Status) && ((GetBmcFirmwareMinorVersion.BmcMiniversion[9]|GetBmcFirmwareMinorVersion.BmcMiniversion[8]) != 0))
    {
      BmcValue = (GetBmcFirmwareMinorVersion.BmcMiniversion[8] - 0x30) * 0x10 + GetBmcFirmwareMinorVersion.BmcMiniversion[9] - 0x30;
      Sprintf(BMCMinorVersion, "F01: V%02x", BmcValue);
    }

    pBIOSVersion = Strstr(BiosMinorVersion, "V");
          
    if(((UINT8)(pBIOSVersion - BiosMinorVersion)) < ((UINT8)Strlen("F00: Vxx"))) //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
    {
      DEBUG((EFI_D_ERROR, "Found CHAR \"V\"!!!\n")); //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>

      //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync+>
      MemSet(&SetBiosMinorVersion.BiosMiniversion[0], sizeof(CHAR8)*16, 0);

      SetBiosMinorVersion.BiosMiniversion[0] = (UINT8)pBIOSVersion[0];
      SetBiosMinorVersion.BiosMiniversion[1] = (UINT8)pBIOSVersion[1];
      SetBiosMinorVersion.BiosMiniversion[2] = (UINT8)pBIOSVersion[2];
      //SGMTB#162769:Sync BIOS Mini Version With BMC - N/A String Sync->
    }

    DataSize = sizeof(SetBiosMinorStatus);
    Status = gIpmiTransport->SendIpmiCommand(
                                gIpmiTransport,
                                NETFN_OEM,
                                BMC_LUN,
                                CMD_OEM_SET_FIRMWARE_VERSION,
                                (UINT8 *)&(SetBiosMinorVersion),
                                sizeof(IPMI_SET_BIOS_MINOR_VERSION_REQUEST),
                                (UINT8 *)&SetBiosMinorStatus,
                                &DataSize);
    if (EFI_ERROR(Status))
      DEBUG((EFI_D_INFO, "Send BIOS Minor Version failed!\n"));
  }

  /**
  Note:Try Add SMBios Type 130
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
  
  Type130Record->BMCMinorVersion = 2; /*PROJECT_SUB_VERSION*/
 

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

  MODULE_START
  //Close the event if it is present
  if (Event != NULL)
    pBS->CloseEvent(Event);

  Status = pBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, &mEfiSmbiosProtocol);
  if (EFI_ERROR(Status))
    MODULE_VOID_END

  AddSmbiosType130();

  MODULE_VOID_END
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

  MODULE_START

  InitAmiLib(ImageHandle, SystemTable);

  UpdateSmbiosTableEvent = EfiCreateProtocolNotifyEvent(
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      UpdateMiniVersion_SMBios,
      NULL,
      &UpdateSmbiosTableReg);

  MODULE_END(Status);
}

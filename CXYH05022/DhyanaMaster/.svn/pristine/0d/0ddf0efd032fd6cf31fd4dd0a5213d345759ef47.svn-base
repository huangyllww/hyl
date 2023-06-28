#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootManagerLib.h>
#include <IpmiNetFnAppDefinitions.h>
#include "OemMiniVersion.h"
#include <OemIpmiNetFnDefinitions.h>
#include "OemLib.h"

VOID InitFWMiniRevision(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class)
{
  EFI_STATUS Status;
  UINT8 DataSize;
  UINT8 BmcValue;
  UINT8 GetBmcMinorParam = 0x01;
  EFI_IPMI_TRANSPORT *gIpmiTransport;
  IPMI_GET_MINOR_VERSION_RESPONSE  GetBmcFirmwareMinorVersion;

  MODULE_START
  if (Class != ADVANCED_FORM_SET_CLASS)
    MODULE_VOID_END

  /**
  Note:Init BIOS And BMC FW Mini Version
  **/
  InitString(HiiHandle, STRING_TOKEN(STR_BIOS_MINI_VERSION_VALUE), L"%s", CONVERT_TO_WSTRING(PROJECT_SUB_VERSION));

  // Locate IpmiTransport protocol
  Status = pBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
  
  if (EFI_ERROR(Status))
    MODULE_VOID_END

  //Get BMC Minor Version From BMC
  DataSize = sizeof(GetBmcFirmwareMinorVersion);
  Status = gIpmiTransport->SendIpmiCommand(
                              gIpmiTransport,
                              NETFN_OEM,
                              BMC_LUN,
                              CMD_OEM_GET_FIRMWARE_VERSION,
                              (UINT8 *)&GetBmcMinorParam,
                              sizeof(UINT8),
                              (UINT8 *)&GetBmcFirmwareMinorVersion,
                              &DataSize);
    if (!EFI_ERROR(Status) && ((GetBmcFirmwareMinorVersion.BmcMiniversion[9]|GetBmcFirmwareMinorVersion.BmcMiniversion[8]) != 0))
  {
    BmcValue = ((GetBmcFirmwareMinorVersion.BmcMiniversion[8] - 0x30)<<4) |(GetBmcFirmwareMinorVersion.BmcMiniversion[9] - 0x30);
    InitString(HiiHandle, STRING_TOKEN(STR_BMC_MINI_VERSION_VALUE), L"V%02x",BmcValue);
  }
  else
  {
    InitString(HiiHandle, STRING_TOKEN(STR_BMC_MINI_VERSION_VALUE), L"N/A");
  }
  

  MODULE_VOID_END
}
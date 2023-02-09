#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootManagerLib.h>
#include <IpmiNetFnAppDefinitions.h>

VOID InitFWMiniRevision(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class)
{
  EFI_STATUS Status;
  UINT8 DataSize;
  EFI_IPMI_TRANSPORT *gIpmiTransport;
  IPMI_GET_DEVICE_ID_RESPONSE GetDeviceIdResponse;

  if (Class != ADVANCED_FORM_SET_CLASS)
    return;

  /**
  Note:Init BIOS FW Mini Version
  **/
  InitString(HiiHandle, STRING_TOKEN(STR_BIOS_MINI_VERSION_VALUE), L"%s", CONVERT_TO_WSTRING(PROJECT_SUB_VERSION));

  /**
  Note:Init BMC FW Mini Version
  **/

  // Locate IpmiTransport protocol
  Status = pBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport);
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]location Ipmi Protocol :%r\r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR(Status))
    return;

  DataSize = sizeof(GetDeviceIdResponse);
  Status = gIpmiTransport->SendIpmiCommand(gIpmiTransport,IPMI_NETFN_APP,BMC_LUN,IPMI_APP_GET_DEVICE_ID,NULL,0,(UINT8 *)&GetDeviceIdResponse,&DataSize);
  if(EFI_ERROR (Status))
    return;
  
  /**
  Note:BMC Mini Version Must Not Be Zero
  **/
  if(GetDeviceIdResponse.AuxFirmwareRevInfo !=0)
    InitString(HiiHandle, STRING_TOKEN(STR_BMC_MINI_VERSION_VALUE), L"V%08x",GetDeviceIdResponse.AuxFirmwareRevInfo); //Mizl-20220317-SGEZT#22638 Improve BMC Mini Version to 8 Bytes+>
}
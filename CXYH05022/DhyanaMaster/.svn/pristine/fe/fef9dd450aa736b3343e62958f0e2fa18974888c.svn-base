
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#include "UsbPortControl.h"

#include "TseSetupTransfer.h"

#include "OemLib.h"

#include "Protocol/HiiString.h"

#define USBPortLoop(Socket, Die, Port)                  \
  for (Socket = 0; Socket < TOTAL_USB_SOCKET; Socket++) \
    for (Die = 0; Die < TOTAL_USB_DIE_PERSOCKET; Die++) \
      for (Port = 0; Port < TOTAL_USB_PORT_PERDIE; Port++)

#define USBStringID(Socket, Die, Port,String) \
  STRING_TOKEN(STR_CPU##Socket##_DIE##Die##_PORT##Port##String)

USB_PORT_CONTROL UsbPortControl[] = {
    {CPU0_DIE0_Port0, {TRUE, L"J53(Rear USB) & J54(FP USB)", L"If This Port Is Disabled,J53&J54&J55 Ports Will Be Disabled!!!",L"J53(RearUSB)&J54(FPUSB)",USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE0_Port1, {TRUE, L"J55(Type A USB)", L"If This Port Is Disabled,J55 Ports Will Be Disabled!!!",L"J55(TypeAUSB)",USBStringID(0, 0, 1,_PROMPT),USBStringID(0, 0, 1,_PROMPT_HELP)}},
    {CPU0_DIE1_Port0, {TRUE, L"Virtual USB Port(Connected With BMC)", L"If This Port Is Disabled,All Vitual Function Provided By BMC And J38 Can Not Be Enabled!!!",L"VirtualUSBPort(ConnectedWithBMC)", USBStringID(0, 1, 0,_PROMPT),USBStringID(0, 1, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port1, {TRUE, L"J38(Gen-Z_4C+_STRADDLE)",L"If This Port Is Disabled,J38 Ports Will Be Disabled!!!",L"J38(Gen-Z_4C+_STRADDLE)",USBStringID(0, 1, 1,_PROMPT),USBStringID(0, 1, 1,_PROMPT_HELP)}},//dengyh-20220324-hide unused usb port
    {PortTerminate, {TRUE, L"Terminate",L"Terminate",L"Terminate", USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}}};

VOID InitUSBString(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class)
{
  EFI_STATUS  Status=EFI_SUCCESS;
  EFI_HII_STRING_PROTOCOL *pHiiString ;

  UINT8 iUSBSocket = 0, iUSBDie = 0, iUSBPort = 0, iUSB;
  MODULE_START

  Status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID **) &pHiiString);
  if (EFI_ERROR(Status))
    MODULE_VOID_END;

  USBPortLoop(iUSBSocket, iUSBDie, iUSBPort)
  {
    DEBUG((EFI_D_INFO, "Socket:%x Die:%x Port:%x >>>>>\r\n", iUSBSocket, iUSBDie, iUSBPort));
    for (iUSB = 0; UsbPortControl[iUSB].CPUUsbPortLocation != PortTerminate; iUSB++)
    {
      DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]StringID:%x Port Description:%s\r\n", __FUNCTION__, __LINE__, UsbPortControl[iUSB].UsbPort[0].StringID, UsbPortControl[iUSB].UsbPort[0].PortDescription));
      InitString(HiiHandle, UsbPortControl[iUSB].UsbPort[0].StringID, L"%s", UsbPortControl[iUSB].UsbPort[0].PortDescription);
      
      DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]StringID:%x Port Help Description:%s\r\n", __FUNCTION__, __LINE__, UsbPortControl[iUSB].UsbPort[0].HelpStringID, UsbPortControl[iUSB].UsbPort[0].PortHelpDescription));
      InitString(HiiHandle, UsbPortControl[iUSB].UsbPort[0].HelpStringID, L"%s", UsbPortControl[iUSB].UsbPort[0].PortHelpDescription);

      Status = pHiiString->SetString(pHiiString, HiiHandle, UsbPortControl[iUSB].UsbPort[0].StringID,"x-AMI",  UsbPortControl[iUSB].UsbPort[0].PortXAMIDescription, NULL);
    	//
    	//  AMI tool converts the language from x-AMI to x-UEFI-AMI.
    	//
      if (EFI_ERROR(Status))
        Status = pHiiString->SetString(pHiiString, HiiHandle, UsbPortControl[iUSB].UsbPort[0].StringID,"x-UEFI-AMI", UsbPortControl[iUSB].UsbPort[0].PortXAMIDescription, NULL);

    }
  }
  MODULE_VOID_END;
}

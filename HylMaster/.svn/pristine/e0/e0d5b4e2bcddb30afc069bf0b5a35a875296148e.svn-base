
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

#include "UsbPortControl.h"

#include "TseSetupTransfer.h"

#include "OemLib.h"

#define USBPortLoop(Socket, Die, Port)                  \
  for (Socket = 0; Socket < TOTAL_USB_SOCKET; Socket++) \
    for (Die = 0; Die < TOTAL_USB_DIE_PERSOCKET; Die++) \
      for (Port = 0; Port < TOTAL_USB_PORT_PERDIE; Port++)

#define USBStringID(Socket, Die, Port,String) \
  STRING_TOKEN(STR_CPU##Socket##_DIE##Die##_PORT##Port##String)

USB_PORT_CONTROL UsbPortControl[] = {
    {CPU0_DIE0_Port0, {TRUE, L"J53(Rear USB) & J54(FP USB)", L"If This Port Is Disabled,J53&J54&J55 Ports Will Be Disabled!!!", USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}},
    {CPU0_DIE0_Port1, {TRUE, L"J55(Type A USB)", L"If This Port Is Disabled,J55 Ports Will Be Disabled!!!", USBStringID(0, 0, 1,_PROMPT),USBStringID(0, 0, 1,_PROMPT_HELP)}},
    {CPU0_DIE1_Port0, {TRUE, L"Virtual USB Port(Connected With BMC)", L"If This Port Is Disabled,All Vitual Function Provided By BMC And J38 Can Not Be Enabled!!!", USBStringID(0, 1, 0,_PROMPT),USBStringID(0, 1, 0,_PROMPT_HELP)}},
    {CPU0_DIE1_Port1, {TRUE, L"J38(Gen-Z_4C+_STRADDLE)",L"If This Port Is Disabled,J38 Ports Will Be Disabled!!!", USBStringID(0, 1, 1,_PROMPT),USBStringID(0, 1, 1,_PROMPT_HELP)}},//dengyh-20220324-hide unused usb port
    {PortTerminate, {TRUE, L"Terminate",L"Terminate", USBStringID(0, 0, 0,_PROMPT),USBStringID(0, 0, 0,_PROMPT_HELP)}}};

VOID InitUSBString(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class)
{
  UINT8 iUSBSocket = 0, iUSBDie = 0, iUSBPort = 0, iUSB;
  MODULE_START

  USBPortLoop(iUSBSocket, iUSBDie, iUSBPort)
  {
    DEBUG((EFI_D_INFO, "Socket:%x Die:%x Port:%x >>>>>\r\n", iUSBSocket, iUSBDie, iUSBPort));
    for (iUSB = 0; UsbPortControl[iUSB].CPUUsbPortLocation != PortTerminate; iUSB++)
    {
      DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]StringID:%x Port Description:%s\r\n", __FUNCTION__, __LINE__, UsbPortControl[iUSB].UsbPort[0].StringID, UsbPortControl[iUSB].UsbPort[0].PortDescription));
      InitString(HiiHandle, UsbPortControl[iUSB].UsbPort[0].StringID, L"%s", UsbPortControl[iUSB].UsbPort[0].PortDescription);
      
      DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]StringID:%x Port Help Description:%s\r\n", __FUNCTION__, __LINE__, UsbPortControl[iUSB].UsbPort[0].HelpStringID, UsbPortControl[iUSB].UsbPort[0].PortHelpDescription));
      InitString(HiiHandle, UsbPortControl[iUSB].UsbPort[0].HelpStringID, L"%s", UsbPortControl[iUSB].UsbPort[0].PortHelpDescription);
    }
  }
  MODULE_VOID_END;
}

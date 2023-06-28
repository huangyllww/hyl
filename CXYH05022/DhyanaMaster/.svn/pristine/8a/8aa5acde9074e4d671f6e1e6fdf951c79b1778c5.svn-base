#include <Setup.h>
#include <AmiDxeLib.h>
#include "OemPlatLib.h"
#include "Library/HiiLib.h"
/*
  #//SGMTB#0144069-ImproveHintMesssageForOOB +>
  #BIT0 ==>USB IO Status & USB Device
  #BIT1 ==>Slef Test
  #BIT2 ==>IPMI Transfer Protocol
  gOemPlatPkgTokenSpaceGuid.OOBInterfaceSelfTestStatus|0x00|UINT8|0x10000006 
  #//SGMTB#0144069-ImproveHintMesssageForOOB ->
*/

EFI_STRING_ID         OOBSelfTest[OOBStatus_Terminate]={
                            STRING_TOKEN(STR_REDFISH_INTERFACE_NO_DEV_FOUND),
                            STRING_TOKEN(STR_REDFISH_INTERFACE_SELF_TEST_FAIL),
                            STRING_TOKEN(STR_REDFISH_INTERFACE_IPMI_USB_PROTOCOL_NOT_INSTALLED)};

VOID
OemRedfishStatusCheck(
  IN    EFI_HII_HANDLE  HiiHandle,
  IN    UINT16          Class
)
{
  UINT8   OOBInterfaceStatus=PcdGet8 (OOBInterfaceSelfTestStatus);
  UINT8   iOOBStatus;
  EFI_STRING OOBSelfTestString = NULL;

  if(OOBInterfaceStatus == 7)
    return;
  
  for (iOOBStatus = 0; iOOBStatus < OOBStatus_Terminate; iOOBStatus++)
  {
    /* code */
    if(((~OOBInterfaceStatus)&(1<<iOOBStatus)))
    {
      OOBSelfTestString = HiiGetString(HiiHandle, STRING_TOKEN(OOBSelfTest[iOOBStatus]), NULL);

      InitString (HiiHandle, STRING_TOKEN(STR_REDFISH_INTERFACE_SELF_TEST_STATUS),L"%s",OOBSelfTestString);
      break;
    }
  }
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]OOBInterfaceStatus:%x\r\n",__FUNCTION__,__LINE__,OOBInterfaceStatus));
}
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

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description: <jiangzhy001-20180928 - XHCI port can be enabled or disabled singly +>
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>
#include "token.h" 
#include <Include\Setup.h>
#include <Library\DebugLib.h>

//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal +>
#define USB_PORT_DISABLE 0
//USB3 CONTAINER Registers
#define FCH_SMN_USB_CONTAINER                0x16D80000ul
#define FCH_TS_USB_PORT_DISABLE0             FCH_SMN_USB_CONTAINER + 0x128        //0x128

typedef struct
{
     UINT8     PortIndex;
     CHAR16    PortDescription[20];
     UINT8     SocketNumber;
     UINT8     DieNumber;
     UINT8     PortNumber;
}USB_PORT_CONTROL;

//Note, All Port Should Be Defined consistent，Port 0 --> Port 1 .to simplify our code Logic
USB_PORT_CONTROL  UsbPortControl[]={
     {RUSBPort0,L"Rear USB Port",0,0,0}, //Mizl - Mantis 0040303: HW Update Porting Guid
     {USBTypeA,L"USB TypeA Port",0,0,1},
     {BMCUsbPort,L"BMC USB Port",0,1,0},
     {FUSBPort0,L"Front USB Port",0,1,1},
     {0xFF,L"NULL",0xFF,0xFF,0xFF}
};
//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal ->
EFI_STATUS EnableOrDisableEachUSBPort() {
    
    EFI_STATUS   Status;
    SETUP_DATA                  SetupData;
    UINT64                      SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                      SetupVariableAttributes=0;
    EFI_GUID                    gSetupGuid = SETUP_GUID;
    AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr;
    UINT32                      EnableUsbPort = 0;
    EFI_GUID                    gAmdCpmTableProtocolGuid       = AMD_CPM_TABLE_PROTOCOL_GUID;
	//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal +>
    UINT8                       iPortNumber,iPortVariable;
    UINT32                      USBPortDisableRead=0;
	//Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal ->
    
    Status = pRS->GetVariable (L"Setup",
                                &gSetupGuid,
                                &SetupVariableAttributes,
                                &SetupVariableSize,
                                &SetupData);
    if (Status != EFI_SUCCESS) {
        return EFI_SUCCESS;
    }
    
    Status = pBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&AmdCpmTableProtocolPtr
                                    );
    if (Status != EFI_SUCCESS) {
        return EFI_SUCCESS;
    }
    //Mizl-20200616-Mantis 0040303: USB 2.0 Control Item Works Abnormal +>
     //Filter USB Port 0 Disable,Then Set USB Port 1 Disable
     for (iPortNumber = 0; UsbPortControl[iPortNumber].PortIndex != 0xff; iPortNumber++)
     {
          /* code */
          //This Logic Is Based On Our Table's Setting
          if(UsbPortControl[iPortNumber].PortNumber != 0)
               continue;
          
          iPortVariable = UsbPortControl[iPortNumber].PortIndex;
          if(SetupData.USBPortControl[iPortVariable] == USB_PORT_DISABLE)
          {
               iPortVariable = UsbPortControl[iPortNumber+1].PortIndex;
               SetupData.USBPortControl[iPortVariable]=USB_PORT_DISABLE;
          }
     }
     
     for (iPortNumber = 0; UsbPortControl[iPortNumber].PortIndex != 0xff; iPortNumber++)
     {
         /* code */
         iPortVariable = UsbPortControl[iPortNumber].PortIndex;
         if(SetupData.USBPortControl[iPortVariable] == USB_PORT_DISABLE)
         {     
               USBPortDisableRead = AmdCpmTableProtocolPtr->CommonFunction.SmnRead32 (AmdCpmTableProtocolPtr, UsbPortControl[iPortNumber].SocketNumber, \
                                                                 UsbPortControl[iPortNumber].DieNumber, FCH_TS_USB_PORT_DISABLE0, 0);

               DEBUG((EFI_D_ERROR, "[%a][%d]Port %s Current Setting:%x\n\n",__FUNCTION__,__LINE__,UsbPortControl[iPortNumber].PortDescription,USBPortDisableRead));
               USBPortDisableRead = USBPortDisableRead|(1<<UsbPortControl[iPortNumber].PortNumber)|(1<<(UsbPortControl[iPortNumber].PortNumber+16) ) ;
               DEBUG((EFI_D_ERROR, "[%a][%d]Port %s Atfer Setting:%x\n\n",__FUNCTION__,__LINE__,UsbPortControl[iPortNumber].PortDescription,USBPortDisableRead));

               AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, UsbPortControl[iPortNumber].SocketNumber, \
                                                                 UsbPortControl[iPortNumber].DieNumber, FCH_TS_USB_PORT_DISABLE0, 0, USBPortDisableRead);
         }
     }
     return EFI_SUCCESS;
    }

EFI_STATUS UsbPortControlEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS   Status;
    
    InitAmiLib(ImageHandle,SystemTable);

    Status=EnableOrDisableEachUSBPort();
    ASSERT_EFI_ERROR(Status);
    
    return EFI_SUCCESS;
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

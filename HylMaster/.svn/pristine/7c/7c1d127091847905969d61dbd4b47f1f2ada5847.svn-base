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

EFI_STATUS EnableOrDisableEachUSBPort() {
    
    EFI_STATUS   Status;
    SETUP_DATA                  SetupData;
    UINT64                      SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                      SetupVariableAttributes=0;
    EFI_GUID                    gSetupGuid = SETUP_GUID;
    AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr;
    UINT32                      EnableUsbPort = 0;
    UINT32                      DisableUsbPort0= 0x00030003;// when disable PORT 0, PORT 1 also is disabled.
    UINT32                      DisableUsbPort1= 0x00020002;
    UINT32                      DisableUsbPort3= 0x00080008; //<duanjun001_20191028_usb j56 cannot be disabled>
    UINT32                      DisableUSB2 = 0x00000002;//Disable Port1 USB2.0
    UINT32                      DisableUSB3 = 0x00020000;//Disable Port1 USB3.0
    EFI_GUID                    gAmdCpmTableProtocolGuid       = AMD_CPM_TABLE_PROTOCOL_GUID;
    
    
    
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
    
    if(SetupData.J37USBPort0) 
       {
           AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI0_SOCKET, XHCI0_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);  
           if (SetupData.J37USBPort1) {
                       AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI0_SOCKET, XHCI0_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);
                   } else {
                       AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI0_SOCKET, XHCI0_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort1);
                   }   
       }
       else {
               AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI0_SOCKET, XHCI0_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort0);
           }
       
       
       if(SetupData.J53USBPort0) 
       {
           AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI2_SOCKET, XHCI2_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);  
           if (SetupData.J53USBPort1) {
                       AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI2_SOCKET, XHCI2_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);
                   } else {
                       AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI2_SOCKET, XHCI2_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort1);
                   }   
       }
       else {
               AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI2_SOCKET, XHCI2_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort0);
           }
       
       
       if(SetupData.J104USBPort1)
       {
           AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI3_SOCKET, XHCI3_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);   
       }
       else
       {
           AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI3_SOCKET, XHCI3_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort0);   
       }
       
       
       if(SetupData.J104USBPort0)
       {
           if(SetupData.BMCUSBPort)
           {
               AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI1_SOCKET, XHCI1_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);
           }
           else
           {
               AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI1_SOCKET, XHCI1_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUSB2);  
           }
           
           if(SetupData.J56USBPort)
                 {
                     AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI1_SOCKET, XHCI1_DIE, OEM_USB_PORT_DISABLE0, 0, EnableUsbPort);
                 }
                 else
                 {
                     AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI1_SOCKET, XHCI1_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort3);   //<duanjun001_20191028_usb j56 cannot be disabled>
                 }
           
           
       }
       else
       {
           AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, XHCI1_SOCKET, XHCI1_DIE, OEM_USB_PORT_DISABLE0, 0, DisableUsbPort0);  
       }
       
       
       
        return EFI_SUCCESS;
    }

EFI_STATUS OemUsbPortEntryPoint(
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

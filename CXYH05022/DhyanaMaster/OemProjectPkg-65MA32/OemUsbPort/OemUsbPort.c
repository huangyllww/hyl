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
    
    if(SetupData.RearFrontHub30) 
          AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB0_PORT_DISABLE, 0, EnableUsbPort); 
     else
          AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB0_PORT_DISABLE, 0, PORT0_DISABLED); 

     
     if(SetupData.Usb3TypeAPort) 
          AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB0_PORT_DISABLE, 0, EnableUsbPort); 
     else
          AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB0_PORT_DISABLE, 0, PORT1_DISABLED);

     
    if(SetupData.BmcUsbPort) 
         AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB1_PORT_DISABLE, 0, EnableUsbPort); 
    else
         AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB1_PORT_DISABLE, 0, PORT0_DISABLED); 

    
    if(SetupData.OCPUsb20) 
         AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB1_PORT_DISABLE, 0, EnableUsbPort); 
    else
         AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_0, DIE_NO, OEM_USB1_PORT_DISABLE, 0, PORT1_DISABLED);        
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

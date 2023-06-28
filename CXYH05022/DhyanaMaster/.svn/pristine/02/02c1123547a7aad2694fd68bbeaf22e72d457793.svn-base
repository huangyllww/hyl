//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//#*********************************************************************
//---------------------------------------------------------------------------
//
// Name:        OcpWolControl.C
//
// Description: Provide functions to control GPIO to enable or disable
//              OCP WOL function
//
//---------------------------------------------------------------------------
//
#include <Token.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>

EFI_GUID  gSetupGuid                     = SETUP_GUID;
UINT32                    OemSmnGpio    = 0x02D02500;
UINT32                    OemSmnIomux    = 0x02D01D00;


//----------------------------------------------------------------------------
// Procedure:   WolGpioSetting
//
// Description: This function control GPIO for OCP WOL function
//
// Input:       UINT8       OcpWolControl
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
EFI_STATUS WolGpioSetting (
    IN UINT8       OcpWolControl)
{
    EFI_STATUS   Status;
    AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr;
    EFI_GUID                    gAmdCpmTableProtocolGuid       = AMD_CPM_TABLE_PROTOCOL_GUID;
    UINT32                      Value = 0;
    UINT32                      ReadValue = 0;
	
    Status = pBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&AmdCpmTableProtocolPtr
                                    );
    if (Status != EFI_SUCCESS) {
        return EFI_SUCCESS;
    }

    if(OcpWolControl) {
        ReadValue = AmdCpmTableProtocolPtr->CommonFunction.SmnRead32 (AmdCpmTableProtocolPtr, SOCKET_OCP, DIE_OCP, (UINT32) OemSmnGpio, (UINT32) (WOL_DISABLE_N << 2));
		Value = ReadValue | BIT23;
		Value = ReadValue | BIT22;
		AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_OCP, DIE_OCP, (UINT32) OemSmnGpio, (UINT32) (WOL_DISABLE_N << 2), Value);
    } else {
        ReadValue = AmdCpmTableProtocolPtr->CommonFunction.SmnRead32 (AmdCpmTableProtocolPtr, SOCKET_OCP, DIE_OCP, (UINT32) OemSmnGpio, (UINT32) (WOL_DISABLE_N << 2));
		Value = ReadValue | BIT23;
		Value = Value & (~BIT22);
		AmdCpmTableProtocolPtr->CommonFunction.SmnWrite32 (AmdCpmTableProtocolPtr, SOCKET_OCP, DIE_OCP, (UINT32) OemSmnGpio, (UINT32) (WOL_DISABLE_N << 2), Value);
    }
    
    return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   OcpWolInit
//
// Description: This is the entrypoint of the OcpWolInit driver.
//
// Input:       EFI_HANDLE       ImageHandle
//              EFI_SYSTEM_TABLE *SystemTable
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
OcpWolInit(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable )
{
    EFI_STATUS  Status ;
	UINT8       OcpWolControl;
    UINTN       Size = sizeof(SETUP_DATA) ;
    SETUP_DATA  gSetupData;

    InitAmiLib( ImageHandle, SystemTable );

    Status = pRS->GetVariable( L"Setup", 
                               &gSetupGuid, 
                               NULL, 
                               &Size, 
                               &gSetupData );

    if(!EFI_ERROR(Status)) {
        OcpWolControl = gSetupData.OcpWolControl;
        WolGpioSetting(OcpWolControl);
    } else {
        return EFI_NOT_FOUND;
    }

    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

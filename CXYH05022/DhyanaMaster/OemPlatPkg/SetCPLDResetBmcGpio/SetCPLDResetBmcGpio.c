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
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>
#include <Include/ServerMgmtSetupVariable.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>



VOID OemCPLDResetBmcPinEnable (VOID)
{
    EFI_STATUS                  Status;
    AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr;
    UINT8                       DATA=0;
    Status = pBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                    NULL,
                                    (VOID**)&AmdCpmTableProtocolPtr
                                    );
    if(EFI_ERROR(Status))
    {
        DEBUG ((-1,"LocateProtocol  Status=%r\n",Status));
        return ;
    }
    DEBUG ((-1,"AmdCpmTableProtocolPtr  SmnRead8\n"));
    DATA= AmdCpmTableProtocolPtr->CommonFunction.SmnRead8 (AmdCpmTableProtocolPtr, SOCKET_GPIO, DIE_GPIO,SMN_GPIO_Addr , ((GPIO_Number << 2)+2) );
    
    DEBUG ((DEBUG_INFO, "DATA = %x\n",DATA));
                            
    //the highest two bits are reserved
    DATA = DATA & 0xC7; 
                            
    //pull up
    DATA |= 0x10; 
                            
    //output enable
    DATA |= 0x80;
                            
    //output High state
    DATA |= 0x40;
    AmdCpmTableProtocolPtr->CommonFunction.SmnWrite8 (AmdCpmTableProtocolPtr, SOCKET_GPIO, DIE_GPIO,SMN_GPIO_Addr , ((GPIO_Number << 2)+2),DATA );
    
    AmdCpmTableProtocolPtr->CommonFunction.SmnWrite8 (AmdCpmTableProtocolPtr, SOCKET_GPIO, DIE_GPIO,SMN_IOMUX_Addr , GPIO_Number,IOMUX_FUNCTION );
    
    DATA=AmdCpmTableProtocolPtr->CommonFunction.SmnRead8 (AmdCpmTableProtocolPtr, SOCKET_GPIO, DIE_GPIO,SMN_GPIO_Addr , ((GPIO_Number << 2)+2) );
    DEBUG ((DEBUG_INFO, "GPIO DATA = %x\n",DATA));
    
    DATA=AmdCpmTableProtocolPtr->CommonFunction.SmnRead8 (AmdCpmTableProtocolPtr, SOCKET_GPIO, DIE_GPIO,SMN_IOMUX_Addr , GPIO_Number );
    DEBUG ((DEBUG_INFO, "GPIO IOMUX DATA = %x\n",DATA));
    
}

EFI_STATUS
EFIAPI   SetCPLDResetBmcGpioEntryPoint(
                     IN EFI_HANDLE ImageHandle,
                     IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                          Status;
    UINTN                               VariableSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
    SERVER_MGMT_CONFIGURATION_DATA      gServerMgmtConfiguration;
    
    InitAmiLib(ImageHandle,SystemTable);
    DEBUG((-1,"SetCPLDResetBmcGpioEntryPoint Start\n"));
    Status = pRS->GetVariable(
                    L"ServerSetup", &gEfiServerMgmtSetupVariableGuid,
                    NULL,
                    &VariableSize, &gServerMgmtConfiguration
                );
    
    if(EFI_ERROR(Status))
    {
        DEBUG ((-1," Get  ServerSetup Variable Status=%r",Status));
        return Status;
    }
    DEBUG((-1,"SetCPLDResetBmcGpioEntryPoint CPLDResetBmc Value =%d\n",gServerMgmtConfiguration.CPLDResetBmc));
    if(gServerMgmtConfiguration.CPLDResetBmc)
    {
        DEBUG((-1,"OemCPLDResetBmcPinEnable Start\n"));
        OemCPLDResetBmcPinEnable();
        DEBUG((-1,"OemCPLDResetBmcPinEnable End\n"));
    }
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

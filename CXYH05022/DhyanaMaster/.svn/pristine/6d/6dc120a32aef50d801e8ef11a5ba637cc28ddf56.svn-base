//**********************************************************************
//<AMI_FHDR_START>
//
// Name: OemSetup.c
//
// Description: Initialization version for force boot to BIOS Setup support.
//
//
//<AMI_FHDR_END>
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>

#include <Protocol/Variable.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Library/UefiLib.h>
//#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "BootOptions.h"
#include "ServerMgmtSetupVariable.h" //Machong 2015/02/13

#define  EFI_TPL_CALLBACK 8

EFI_IPMI_TRANSPORT                  *pEfiIpmiTransport;
extern EFI_GUID                     gOemNvramUpdateGuid;

#if 0 //<Wangyia002-20170614 Sync Thermal Options>
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   SyncIpmiSetupConfig
//
// Description: Check for the Ipmi OEM Setup Configuration and Set 
// certain variable bmc specified
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
SyncFanCtrlSetupConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
    EFI_STATUS                          Status;
    
    SERVER_MGMT_CONFIGURATION_DATA      mServerMgmtConfiguration;
    UINT8                               *CommandData;
    UINT8                               CommandDataSize;
    UINT8                               *ResponseData;
    UINT8                               ResponseDataSize;
    UINT32                              Attributes;
    UINTN                               VariableSize = 0;
     
    Status = EFI_SUCCESS;
    
    if(pEfiIpmiTransport == NULL) 
    {
        Status = pBS->LocateProtocol (
                        &gEfiDxeIpmiTransportProtocolGuid,
                        NULL,
                        &pEfiIpmiTransport
                        );
    }
    if(!EFI_ERROR(Status))
    {
        Status = pBS->AllocatePool (
                        EfiBootServicesData,
                        0x100,
                        &CommandData );
        pBS->SetMem(CommandData, 0x100, 0);        
        Status = pBS->AllocatePool (
                        EfiBootServicesData,
                        0x100,
                        &ResponseData );
        pBS->SetMem(ResponseData, 0x100, 0);  

        if(!EFI_ERROR(Status))
        {
            VariableSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
            Status = pRS->GetVariable (
                    L"ServerSetup", 
                    &gEfiServerMgmtSetupVariableGuid,
                    &Attributes,
                    &VariableSize,
                    &mServerMgmtConfiguration);

            *(UINT16 *)CommandData = mServerMgmtConfiguration.FanCtl_Mode;            
            CommandDataSize = 9;
            ResponseDataSize = 0x10;                
        
            Status = pEfiIpmiTransport->SendIpmiCommand(
                pEfiIpmiTransport,
                0x3A,//OEM_IPMI_NET_FUN
                0,
                0xd, //CMD_OEM_SET_FAN_MODE_SPEED
                CommandData,
                CommandDataSize,
                ResponseData,
                &ResponseDataSize );
        }
    }
    
    return  Status;
}
#endif //<Wangyia002-20170614 Sync Thermal Options>

EFI_STATUS
OemSetupEntryPoint(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
    )
{
    EFI_GUID            gSetupGuid = SETUP_GUID;
    SETUP_DATA          *SetupData = NULL;
    UINTN               VariableSize = 0;
    UINT32              BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
    EFI_GUID            guidBootFlow = BOOT_FLOW_VARIABLE_GUID;
    UINT32              Attributes;
    EFI_STATUS          Status; //<Wangyia002-20170614 Sync Thermal Options>
#if 0  //<Wangyia002-20170614 Sync Thermal Options>  
    //Machong 2014/02/13 Merge ODM Code Here To Support Fan Control 
    EFI_STATUS      Status, Status_Cmd, Status_Res, Status_Ipmi;
    SERVER_MGMT_CONFIGURATION_DATA      mServerMgmtConfiguration;
    UINT8                               *CommandData;
    UINT8                               CommandDataSize;
    UINT8                               *ResponseData;
    UINT8                               ResponseDataSize;
#endif //<Wangyia002-20170614 Sync Thermal Options>

    InitAmiLib(ImageHandle, SystemTable);
#if 0 //<Wangyia002-20170614 Sync Thermal Options>
    //Machong 2014/02/13 Merge ODM Code Here To Support Fan Control 
    Status_Ipmi = pBS->LocateProtocol(&gEfiDxeIpmiTransportProtocolGuid, NULL, &pEfiIpmiTransport);
    

    if(!EFI_ERROR(Status_Ipmi))
    {
        Status_Cmd = pBS->AllocatePool (
                        EfiBootServicesData,
                        0x100,
                        &CommandData );
     
        Status_Res = pBS->AllocatePool (
                        EfiBootServicesData,
                        0x100,
                        &ResponseData );
     
        if(!EFI_ERROR(Status_Cmd) && !EFI_ERROR(Status_Res))
        {
            VariableSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
            Status = pRS->GetVariable (
                    L"ServerSetup", 
                    &gEfiServerMgmtSetupVariableGuid,
                    &Attributes,
                    &VariableSize,
                    &mServerMgmtConfiguration);
             
            if(!EFI_ERROR(Status))
            {
                CommandDataSize = 0;
                ResponseDataSize = 0x10;                
                
                Status = pEfiIpmiTransport->SendIpmiCommand(
                        pEfiIpmiTransport,
                        0x3A,
                        0,
                        0xE, //CMD_OEM_GET_FAN_MODE_SPEED
                        CommandData,
                        CommandDataSize,
                        ResponseData,
                        &ResponseDataSize );
               
                if(pEfiIpmiTransport->CommandCompletionCode == 0)
                {               
                    if(mServerMgmtConfiguration.FanCtl_Mode != *ResponseData)
                    {
                        mServerMgmtConfiguration.FanCtl_Mode = *ResponseData;
                                        
                        Status = pRS->SetVariable (
                                L"ServerSetup", 
                                &gEfiServerMgmtSetupVariableGuid,
                                Attributes,
                                VariableSize,
                                &mServerMgmtConfiguration);
                        TRACE((-1, "\n SetVariable Status = %x", Status));  
                    }
                }
            }
        }
    }
    
    Status =  EfiNamedEventListen (
                &gOemNvramUpdateGuid,
                EFI_TPL_CALLBACK,
                SyncFanCtrlSetupConfig,
                NULL,
                NULL
                );
#endif //<Wangyia002-20170614 Sync Thermal Options>

    //Machong Please Do not Add your code below this line -------- 2015/02/14
    Status = GetEfiVariable(L"Setup", &gSetupGuid, &Attributes, &VariableSize, &SetupData);
    if(EFI_ERROR(Status)){
	  ((SETUP_DATA*)SetupData)->BootToSetup = 0;
	}
     
    Status = pRS->SetVariable(
                   L"Setup", 
                   &gSetupGuid,
                   Attributes,
                   VariableSize,
                   SetupData
                   );
    
    if(!(((SETUP_DATA*)SetupData)->BootToSetup)){
    return EFI_SUCCESS;
    }
    Status = gRT->SetVariable (
                            L"BootFlow",
                            &guidBootFlow,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS,
                            sizeof(BootFlow),
                            &BootFlow );

    return EFI_SUCCESS;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
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
// $Header: $
//
// $Revision: $
//
// $Date:  $
//**********************************************************************

//**********************************************************************
/** @file PldmSetupEntry.c
    Defines the callback function for PLDM BMC communication

**/
//**********************************************************************

#include "Efi.h"
#include <AmiLib.h>
#include <Protocol/AMIPostMgr.h>
#include <Setup.h>
#include <Library/HiiLib.h>
#include "Library/MemoryAllocationLib.h"
#include "AmiPldmInterfaces.h"
#include <Protocol/IPMITransportProtocol.h> //<KuaiBai-20180313-PLDM FV SOLUTION+>
#include <OemIpmiNetFnDefinitions.h>      //<KuaiBai-20180313-PLDM FV SOLUTION+>

AMI_POST_MANAGER_PROTOCOL       *mPostMgr;
extern EFI_BOOT_SERVICES        *pBS;
extern EFI_SYSTEM_TABLE         *pST;
extern EFI_BOOT_SERVICES        *pBS;
extern EFI_RUNTIME_SERVICES     *pRT;

//<KuaiBai-20180313-PLDM FV SOLUTION+>
EFI_STATUS
SetPldmIpmiCmd (UINT8 CmdA, UINT8 CmdB, UINT8 CmdC, UINT8 ResponseNum, UINT8 ResponseValue)
{
    EFI_STATUS          Status = EFI_UNSUPPORTED;
    EFI_IPMI_TRANSPORT  *pEfiIpmiTransport;
    UINT8               CommandData[10] = {0};
    UINT8               CommandDataSize;
    UINT8               ResponseData[10]= {0};
    UINT8               ResponseDataSize;
    UINT8               BMC_Error_Count = 0;
    
    Status = pBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, &pEfiIpmiTransport);
    if (!EFI_ERROR(Status))
    {
        _SetIPMIretry:       
        CommandData[0] = CmdA; //0x01:Save config 1;    0x02:Save config 2;      0x00: do nothing;
        CommandData[1] = CmdB; //0x01:Restore config 1; 0x02:Restore config 2;   0x00: do nothing;
        CommandData[2] = CmdC; //0x01:BIOS updated;     0x00:Clear;
        CommandDataSize = 3;
        ResponseDataSize = 10;
        
        //Set save config type 1
        Status = pEfiIpmiTransport->SendIpmiCommand (pEfiIpmiTransport,
                                    NETFN_OEM,
                                    BMC_LUN,
                                    CMD_OEM_SET_PRESERVE_BIOS, 
                                    CommandData,
                                    CommandDataSize,
                                    ResponseData,
                                    &ResponseDataSize
                                    );
        DEBUG((DEBUG_ERROR, "\n [SetupCallback] config 1 = %r. \n",Status));
                 
        //Step 2, get command to check if the flag have been set. 
        ResponseDataSize = 10;
        Status = pEfiIpmiTransport->SendIpmiCommand (pEfiIpmiTransport,
                                    NETFN_OEM,
                                    BMC_LUN,
                                    CMD_OEM_GET_PRESERVE_BIOS,
                                    NULL,
                                    0,
                                    ResponseData,
                                    &ResponseDataSize
                                    );
         
        //Step 3: Check the if the flag status, if set fail, try it again.
        if (ResponseData[ResponseNum]!= ResponseValue)
        { 
            if (BMC_Error_Count < 3)
            {
                BMC_Error_Count++;   
                goto _SetIPMIretry;
            }
        }//end if (ResponseData[0])
        return Status;
    }//end of EFI_ERROR Status
    return Status;
}
//<KuaiBai-20180313-PLDM FV SOLUTION->

/**
        Procedure    : SaveInBmc

        Description  : Creates the Pldm tables and saves the tables in BMC for the pldm item.

        Parameter    : EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key

        Return Value : EFI_STATUS

**/
EFI_STATUS SaveInBmc(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    CALLBACK_PARAMETERS *Callback = (CALLBACK_PARAMETERS *)NULL;
    static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    PLDMTables GetPldmData;
    EFI_GUID  gAmiPldmBmcProtocolGuid = AMIPLDM_BMC_DRIVER_PROTOCOL_GUID;
    AMIPLDM_INTERFACE_PROTOCOL *AmiPldmInterfaceProtocol = NULL; 
    
    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;
    
    if(mPostMgr == NULL)
    {
        Status = pBS->LocateProtocol(&AmiPostManagerProtocolGuid, NULL, &mPostMgr);
        if(EFI_ERROR(Status)) {
            return EFI_SUCCESS;
        }
    }
    //<KuaiBai-20180313-PLDM FV SOLUTION+>   
    Status = SetPldmIpmiCmd (0x01,0,0,0,0x01); //Set Ipmi cmd: save config 1.
    if(EFI_ERROR(Status)) return Status;
    //<KuaiBai-20180313-PLDM FV SOLUTION->
    
    Status = pBS->LocateProtocol (&gAmiPldmBmcProtocolGuid, NULL, &AmiPldmInterfaceProtocol);
    if (Status == EFI_SUCCESS)
    {
        //Set the Cache mode to TRUE
        AmiPldmInterfaceProtocol->SetPldmMode(FALSE);
        
        //Create the Pldm tables
        Status = AmiPldmInterfaceProtocol->CreatePldmTables(&GetPldmData,TRUE);
        
        //Set the Cache mode to FALSE
        AmiPldmInterfaceProtocol->SetPldmMode(FALSE);
         
        if (Status == EFI_SUCCESS)
        {
            mPostMgr->DisplayMsgBox( L"Save",L"Create PLDM Success" , MSGBOX_TYPE_OK, NULL );
            
            //Save the Tables in BMC
            Status = AmiPldmInterfaceProtocol->SaveConfigurationInBmc(&GetPldmData);
            if(Status)
                mPostMgr->DisplayMsgBox( L"Save",L"Save in BMC failed" , MSGBOX_TYPE_OK, NULL );
            else
            //<KuaiBai-20180313-PLDM FV SOLUTION+>
                //-mPostMgr->DisplayMsgBox( L"Save",L"Save in BMC Success" , MSGBOX_TYPE_OK, NULL );
            {
                mPostMgr->DisplayMsgBox( L"Save",L"Save in BMC Success" , MSGBOX_TYPE_OK, NULL );
                SetPldmIpmiCmd (0x00,0,0,0,0x00); //Clear Ipmi cmd: Clear save config 1.
            }
            //<KuaiBai-20180313-PLDM FV SOLUTION->
        }
        else
        {
            mPostMgr->DisplayMsgBox( L"Save",L"Create PLDM Failed" , MSGBOX_TYPE_OK, NULL );
            return EFI_ABORTED;
        }
    }
    else
    {
        mPostMgr->DisplayMsgBox( L"Save",L"AmiPldmBmc Protocol Locate Failed" , MSGBOX_TYPE_OK, NULL );
    }
    
    return Status;
}

/**
        Procedure    : RestoreFromBmc

        Description  : Gets the Tables from BMC and Imports the tables

        Parameter    : EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key

        Return Value : EFI_STATUS

**/
EFI_STATUS RestoreFromBmc(EFI_HII_HANDLE HiiHandle, UINT16 Class, UINT16 SubClass, UINT16 Key)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    CALLBACK_PARAMETERS *Callback = (CALLBACK_PARAMETERS *)NULL;
    static EFI_GUID AmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
    PLDMTables PldmData;
    EFI_GUID  gAmiPldmBmcProtocolGuid = AMIPLDM_BMC_DRIVER_PROTOCOL_GUID;
    AMIPLDM_INTERFACE_PROTOCOL *AmiPldmInterfaceProtocol = NULL; 
   
    Callback = GetCallbackParameters();
    if(!Callback || Callback->Action != EFI_BROWSER_ACTION_CHANGING)
        return EFI_UNSUPPORTED;
    
    if(mPostMgr == NULL)
    {
        Status = pBS->LocateProtocol(&AmiPostManagerProtocolGuid, NULL, &mPostMgr);
        if(EFI_ERROR(Status)) {
            return EFI_SUCCESS;
        }
    }
    
    //<KuaiBai-20180313-PLDM FV SOLUTION+>    
    SetPldmIpmiCmd (0,0x01,0,1,0x01); //Set Ipmi cmd: restore config 1.
    //<KuaiBai-20180313-PLDM FV SOLUTION->
        
    Status =  pBS->LocateProtocol(&gAmiPldmBmcProtocolGuid, NULL, &AmiPldmInterfaceProtocol);
    if (Status == EFI_SUCCESS)
    {
        //Get the Pldm tables data
        Status = AmiPldmInterfaceProtocol->RestoreConfigurationFromBmc(&PldmData);
         
        if (Status == EFI_SUCCESS)
        {
            mPostMgr->DisplayMsgBox( L"Restore",L"Restore from BMC Success" , MSGBOX_TYPE_OK, NULL );
            
            //Set the Cache mode to TRUE
            //AmiPldmInterfaceProtocol->SetPldmMode(TRUE);//<lubl_20180110 ->
            AmiPldmInterfaceProtocol->SetPldmMode(FALSE);//<lubl_20180110 +>
            //<lubl_20180110 set to false because the PCH configuration can't be updated immediately without save.>
                   
            //Import the Pldm Tables
            Status = AmiPldmInterfaceProtocol->ImportPldmTables(&PldmData);   
            if(Status)
                mPostMgr->DisplayMsgBox( L"Restore",L"Restore PLDM failed" , MSGBOX_TYPE_OK, NULL );
            else
            //<KuaiBai-20180313-PLDM FV SOLUTION+>
                //- mPostMgr->DisplayMsgBox( L"Restore",L"Restore PLDM Success" , MSGBOX_TYPE_OK, NULL );
            {
                mPostMgr->DisplayMsgBox( L"Restore",L"Restore PLDM Success" , MSGBOX_TYPE_OK, NULL );
                SetPldmIpmiCmd (0,0x00,0,1,0x00); //Clear Ipmi cmd: Clear restore config 2.
            }
            //<KuaiBai-20180313-PLDM FV SOLUTION->
            
            //Set the Cache mode to FALSE
            AmiPldmInterfaceProtocol->SetPldmMode(FALSE);
        }
        else
        {
            mPostMgr->DisplayMsgBox( L"Restore",L"Restore from BMC failed" , MSGBOX_TYPE_OK, NULL );
            return EFI_ABORTED;
        }
    }
    else
    {
        mPostMgr->DisplayMsgBox( L"Restore",L"AmiPldmBmc Protocol Locate Failed" , MSGBOX_TYPE_OK, NULL );
    }
    
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

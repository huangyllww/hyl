//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2016, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************

//************************************************************************
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateBmc.c
//
// Description:	Contains AmiFwUpdate Entry point
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
#include "AmiFwUpdateBmc.h"
#include  <Protocol/AmiAfuProtocol.h>
#include "AmiDxeLib.h"
#include  <Library/MemoryAllocationLib.h>
#include "AmiFwUpdateBmcUIIntf.h"
#include "Protocol/AMIPostMgr.h"


BOOLEAN gBcpPreserveFlag = FALSE;

BOOLEAN gAFU_Protocol_Found = FALSE;
AMI_AFU_PROTOCOL *gAFUFlash = (AMI_AFU_PROTOCOL *)NULL;
FLASH_POLICY_INFO_BLOCK     gFlashingPolicy;

EFI_RUNTIME_SERVICES  *gRT;
EFI_BOOT_SERVICES     *gBS;
EFI_SYSTEM_TABLE *gST;

VOID* gHiiHandle;
EFI_HII_HANDLE HiiHandle = 0;
EFI_HII_HANDLE gAMIFWUpdateHiiHandle = 0;
BOOLEAN       gAbortWaitForKey = FALSE;

extern BOOLEAN		gUsePostMgrService;
AMI_POST_MANAGER_PROTOCOL 	*gPostMgr; 
void UpdateMultilanguageVar();
EFI_STATUS  RemoveStringPack();
extern CHAR16 *FW_HiiGetString( VOID* handle, UINT16 token );

void UefiInitLib(EFI_SYSTEM_TABLE * UefiSysTable)
{
    gRT = UefiSysTable->RuntimeServices;
    gBS = UefiSysTable->BootServices;
    gST = UefiSysTable;
}

/**
    Procedure    :   CheckForAbortKey

    Description  :  wait for user key (y/n)

    Parameter    :   None

    Return Value  :   FW_UPDATE_WAIT_STATUS

**/


FW_UPDATE_BMC_WAIT_STATUS  CheckForAbortKey ()
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_INPUT_KEY  Key;
	FW_UPDATE_BMC_WAIT_STATUS  AbortStatus = WAIT;
	Key.UnicodeChar = 0;
	while(FALSE == gAbortWaitForKey)
	{
		Status = pST->ConIn->ReadKeyStroke( pST->ConIn, &Key );
		if (!EFI_ERROR(Status)) 
		{
			if( (Key.UnicodeChar == 'n' ) || (Key.UnicodeChar == 'y' ) || (Key.UnicodeChar == 'N' ) || (Key.UnicodeChar == 'Y' ))
			{
				if( (Key.UnicodeChar == 'n')||(Key.UnicodeChar == 'N'))
					AbortStatus = ABORT;
				else
					AbortStatus = START;
				break;
			}
		}
	}
	gAbortWaitForKey = FALSE;
	return AbortStatus;
}

/**
    Procedure    :   AbortOberation

    Description  :   abort wait for user key after some second

    Parameter    :   None

    Return Value  :   FW_UPDATE_WAIT_STATUS

**/

VOID AbortOberation (IN EFI_EVENT  Wait4AbortEvent,IN VOID       *Context)
{
	*(BOOLEAN*)Context = TRUE;
}

/**
    Procedure    :   WaitingForAbortFwUpdate

    Description  :   show  FW update start message and wait for User selection to start or abort

    Parameter    :   None

    Return Value  :   EFI_STATUS

**/


EFI_STATUS WaitingForAbortFwUpdate()
{
	
	EFI_STATUS Status = EFI_SUCCESS;
	INTN Index = 0;
	CHAR16 Str[128];
	CHAR16 *FlashStrFrmt = (CHAR16 *)NULL;
	EFI_EVENT 		AbortEvent = NULL; 

	FW_UPDATE_BMC_WAIT_STATUS  AbortStatus = START;
	Status = pBS->CreateEvent ( EVT_TIMER | EVT_NOTIFY_SIGNAL,TPL_NOTIFY,AbortOberation,(VOID*)&gAbortWaitForKey,&AbortEvent);
	if (!EFI_ERROR(Status)) 
	{
		Status = pBS->SetTimer (AbortEvent, TimerPeriodic, WAIT_TIME_INTERVAL);
		if (!EFI_ERROR(Status)) 
		{
			FlashStrFrmt = FW_HiiGetString(gHiiHandle,STR_WAIT_REMOTE_FW_BMC_UPDATE);

			for(Index = WAITTING_SEC_FOR_ABORT_BMC;Index>=0;Index--)
			{				
				Swprintf(Str, (CHAR16*)FlashStrFrmt, Index);
				DEBUG((EFI_D_ERROR, "[SK] Prompt string.\n")); //Spiral
				ShowMessageInTextMode((CHAR16*)Str, FALSE, FALSE);				
				AbortStatus = CheckForAbortKey();
				DEBUG((EFI_D_ERROR, "[SK] AbortStatus = %x.\n",AbortStatus)); //Spiral
				if(AbortStatus != WAIT)
					break;		
			}
			pBS->SetTimer (AbortEvent, TimerCancel, 0);
		}
		pBS->CloseEvent(AbortEvent );
	}
	switch(AbortStatus)
	{
	case ABORT:
		Status = EFI_ABORTED;
		break;
	case WAIT:
	case START:
	    DEBUG((EFI_D_ERROR, "[SK] START.\n")); //Spiral
		Status = EFI_SUCCESS;
		break;
	}
		
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   AmiFwUpdateBmcEntryPoint
//
// Description  :   AmiFwUpdate BMC Entry point
//
// Parameter    :   EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE * UefiSysTable
//
// Return Value  :  EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiFwUpdateBmcEntryPoint(EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE * UefiSysTable)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
    UefiInitLib(UefiSysTable);
    FWUP_UI_InterfaceInit(ImageHandle, UefiSysTable);
    
    pBS->SetWatchdogTimer (0, 0, 0, (CHAR16*)NULL); 
    
    LoadStrings(ImageHandle, &HiiHandle);
    gAMIFWUpdateHiiHandle = HiiHandle;
    gHiiHandle = HiiHandle;
    
	if(NULL == gPostMgr)
	{
		Status = gBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL,(void**) &gPostMgr);
		 if (EFI_ERROR(Status))
			 gUsePostMgrService = FALSE;
	}
	
    //Spiral - AMIFWUpdateHiiInitializeProtocol();
	Status = AMIFWUpdateHiiInitializeProtocol();
	DEBUG((EFI_D_ERROR, "[SK] AMIFWUpdateHiiInitializeProtocol = %r.\n", Status)); //Spiral
    UpdateMultilanguageVar();
    
    //Spiral - Initialise_AFU_Protocol();
    Status = Initialise_AFU_Protocol();
    DEBUG((EFI_D_ERROR, "[SK] Initialise_AFU_Protocol = %r.\n", Status)); //Spiral
    Provide_BCPSupport() ;
    InitializeBackground();
    PerformTextModeOperation(ImageHandle);
    DEBUG((EFI_D_ERROR, "[SK] PerformTextModeOperation may be not back.\n")); //Spiral
    //Spiral - RemoveStringPack();
    RemoveStringPack();
    DEBUG((EFI_D_ERROR, "[SK] RemoveStringPack = %r.\n", Status)); //Spiral
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   Initialise_AFU_Protocol
//
// Description  :   Check AFU protocol and initialize global structure FLASH_POLICY_INFO_BLOCK 
//
// Parameter    :   EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE * UefiSysTable
//
// Return Value  :  EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS Initialise_AFU_Protocol()
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_GUID AFUProtocolGuid = AMI_AFU_PROTOCOL_GUID;
	
	Status = gBS->LocateProtocol (&AFUProtocolGuid,NULL, (VOID **)&gAFUFlash);
	if(EFI_SUCCESS ==Status)
	{
		gAFU_Protocol_Found = TRUE;
		/* Ask BIOS which policy is supported for secure flash. */
		Status = gAFUFlash->GetFlashPolicy( &gFlashingPolicy );
	}
	else
		gAFU_Protocol_Found = FALSE;

	return Status;
}


//************************************************************************
//************************************************************************
//**                                                    		    	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                    		    	**
//**                       All Rights Reserved.            				**
//**                                                    				**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                    				**
//**                       Phone: (770)-246-8600           				**
//**                                                    				**
//************************************************************************
//************************************************************************

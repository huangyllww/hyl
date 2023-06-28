//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file AMISetupNVLock.c
    This file contains the algorithm to protect the BIOS Configuration
    update during runtime. The variables Setup & AMITSESetup write is
    blocked until the system password provided to Unlock.

**/

#include <AmiDxeLib.h>
#include <Token.h>
#include "Setup.h"
#include "AMISetupNVLock.h"
#include <NVLockElink.h>

extern EFI_BOOT_SERVICES *pBS;
extern EFI_RUNTIME_SERVICES	*pRS;
extern EFI_SYSTEM_TABLE    *pST;

static BOOLEAN g_NVWriteProtect = FALSE;
//static BOOLEAN g_ControlInSetup = TRUE;
static BOOLEAN gEventsRegistered = FALSE;
static BOOLEAN gRuntimeLockEnabled = FALSE;
static BOOLEAN gInSmmPhase = FALSE;
static BOOLEAN gSmiFlashUpdate = FALSE;
static UINT16 gPasswordRetryCount = 0;
static EFI_EVENT    gTSE_EnterSetupEvent = NULL;
static BOOLEAN g_LockedinRuntime = FALSE;
static BOOLEAN g_InstallSetupNVProtocol = FALSE;

//NVLockMailbox Variable used for communication b/w DXE and SMM
typedef struct
{
    UINTN NVLockState;
} SETUPNVLOCK_MAILBOX;

#define SETUPNVLOCK_MAILBOX_ADDRESS_VARIABLE_GUID \
        { 0x504af431, 0x3025, 0x4d32,0x9f, 0xbf, 0xe9, 0xf3, 0x18, 0x55, 0x55, 0x1d }
SETUPNVLOCK_MAILBOX NvLockMailbox = {0};

//Dummy Variable GUID
#define DUMMY_NVLOCK_VARIABLE_GUID \
		{ 0x48662780, 0xd354, 0x4a41, 0xa7, 0xf4, 0x3d, 0x1c, 0xaf, 0xc3, 0x32, 0x28 }
EFI_GUID DummyNVLockVariableGuid = DUMMY_NVLOCK_VARIABLE_GUID;

EFI_GUID gTSE_EnterSetupGuid = AMITSE_EVENT_AFTER_BOOT_GUID;
EFI_GUID SetupEnterProtocolGuid = AMITSE_SETUP_ENTER_GUID;
VOID *gSetupEnterNotifyReg = NULL;
VOID *gTSERegistration = NULL;

typedef struct
{
    CHAR8 VariableName[50];
    EFI_GUID VariableGuid;
} SETUP_VARIABLE_DATA;

typedef EFI_STATUS (HOOK_LOCK_UNLOCK_NV)(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);

extern HOOK_LOCK_UNLOCK_NV LOCK_UNLOCK_NV_LIST EndOfLockUnlockHook;

EFI_GUID NVLockMailboxVariableGuid = SETUPNVLOCK_MAILBOX_ADDRESS_VARIABLE_GUID;
HOOK_LOCK_UNLOCK_NV* ValidateAndLockNVHookList[] = {LOCK_UNLOCK_NV_LIST NULL};

//Support to allow Variable update through AFU
typedef EFI_STATUS (*TOGGLE_SMI_FLASH_FLAG)(BOOLEAN Start);
typedef struct
{
    TOGGLE_SMI_FLASH_FLAG ToggleSmiFlashUpdateFlag;
} AMI_SETUPNV_CONTROL_PROTOCOL;
static EFI_GUID gAmiSetupNvControlProtocolGuid = { 0x1bd378d6, 0xd2d6, 0x417f, { 0x87, 0xf6, 0x9e, 0x52, 0xf2, 0x8d, 0xcb, 0x2f } };

EFI_STATUS ToggleSmiFlashUpdateFlag (BOOLEAN Start);
AMI_SETUPNV_CONTROL_PROTOCOL SetupNvControl = {ToggleSmiFlashUpdateFlag};
EFI_STATUS InstallSetupNVControlProtocol();

/**
    This Function Registers the event callback

    @param EFI_GUID *pProtocol, EFI_EVENT_NOTIFY NotifyFunction, VOID *pNotifyContext,
           EFI_EVENT *pEvent, VOID **ppRegistration

    @retval EFI_STATUS

**/

EFI_STATUS RegisterEventCallback(
	EFI_GUID *pProtocol, EFI_EVENT_NOTIFY	NotifyFunction,
	VOID *pNotifyContext, EFI_EVENT *pEvent,
	VOID **ppRegistration
)
{
	EFI_STATUS Status;

	Status = pBS->CreateEvent(
				 EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
				 NotifyFunction, pNotifyContext, pEvent
			 );

	if (EFI_ERROR(Status))
	{
		return Status;
	}

	return pBS->RegisterProtocolNotify(pProtocol, *pEvent, ppRegistration);
}

/**
    This Function notifies the callback for EndOfDxe and ReadyToBoot event 

    @param EFI_EVENT Event, VOID *Context

    @retval VOID
**/

VOID AmiSetupNVEndOfDxeAndReadyToBootNotify(EFI_EVENT Event, VOID *Context)
{
    EFI_STATUS RetStatus = EFI_UNSUPPORTED;
    SETUP_DATA NvLockSetupVariable = {0};
    UINTN VarSize = sizeof(NvLockSetupVariable);
    //Get variable and check
    RetStatus =  DxeGetVariable(L"Setup", &gSetupVariableGuid, NULL, &VarSize, &NvLockSetupVariable);
    if(!EFI_ERROR(RetStatus))
    {
        if(NvLockSetupVariable.NVLockSetupControl == 0)
            return;
    }

    //g_ControlInSetup = FALSE;
    g_NVWriteProtect = TRUE;
    gEventsRegistered = TRUE;

    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: EndOfDxe or Entering OS"));
    //Set NvLockMailbox variable value with 1
    NvLockMailbox.NVLockState = 1; // after readytoboot and EndOfDxe
    RetStatus = pRS->SetVariable( L"NvLockMailbox", &NVLockMailboxVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS , sizeof(NvLockMailbox), &NvLockMailbox);
    DEBUG((DEBUG_ERROR, "\nSetupNVLockNotify: Set Status: %x", RetStatus));
    
    return;
}

/**
    This Function notifies the callback for EnterSetup event 

    @param EFI_EVENT Event, VOID *Context

    @retval VOID
**/

VOID AmiSetupNVEnterSetupNotify(EFI_EVENT Event, VOID *Context)
{
    EFI_STATUS RetStatus = EFI_UNSUPPORTED;

    //g_ControlInSetup = TRUE;
    g_NVWriteProtect = FALSE;
    gEventsRegistered = TRUE;
    
    g_LockedinRuntime = FALSE;

    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Entering Setup"));
    //Set NvLockMailbox variable value with 0
    NvLockMailbox.NVLockState = 0; //Entering Setup
    RetStatus = pRS->SetVariable( L"NvLockMailbox", &NVLockMailboxVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS , sizeof(NvLockMailbox), &NvLockMailbox);
    DEBUG((DEBUG_ERROR, "\nSetupNVLockNotify: Set Status: %x", RetStatus));

    return;
}

/**
    This Function Registers the event 

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS
RegisterEvents()
{
    EFI_EVENT   Event;
    EFI_STATUS  Status = EFI_UNSUPPORTED;
   

    if(!gInSmmPhase && (gEventsRegistered == FALSE))
	{
	    Status = CreateReadyToBootEvent(TPL_CALLBACK, AmiSetupNVEndOfDxeAndReadyToBootNotify, NULL, &Event);
		DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: RegisterEvents ReadytoBoot: %x", Status));
#if (LOCK_SETVAR_AT_ENDOFDXE == 1)
		{
		VOID *Registration = (VOID*) NULL;
        // Create an End of DXE event.
        Status = pBS->CreateEventEx(
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                AmiSetupNVEndOfDxeAndReadyToBootNotify,
                NULL,
                &gEfiDxeSmmReadyToLockProtocolGuid,
                &Event
        );     
        Status = pBS->RegisterProtocolNotify(&gEfiDxeSmmReadyToLockProtocolGuid, Event, &Registration);
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: EndOfDxe CreateEventEx Status: %x", Status));
		}
#endif
	    Status = RegisterEventCallback( &gTSE_EnterSetupGuid, AmiSetupNVEnterSetupNotify, NULL, &gTSE_EnterSetupEvent, &gTSERegistration );
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: EnterSetup: %x", Status));
		//
		//Install callback on entering into Setup
		//
		Status = RegisterProtocolCallback( &SetupEnterProtocolGuid,
		                                    AmiSetupNVEnterSetupNotify,
		                                    NULL,
		                                    &Event,
		                                    &gSetupEnterNotifyReg);
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify:SetupEnter: %x", Status));

	    gEventsRegistered = TRUE;
	}

    return Status;
}

/**
    This Function Interface to lock NVRAM

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS LockNV()
{
    EFI_STATUS RetStatus = EFI_UNSUPPORTED;
    SETUP_DATA NvLockSetupVariable = {0};
    UINTN VarSize = sizeof(NvLockSetupVariable);
    //Get variable and check
    RetStatus =  DxeGetVariable(L"Setup", &gSetupVariableGuid, NULL, &VarSize, &NvLockSetupVariable);
    if(!EFI_ERROR(RetStatus))
    {
        if(NvLockSetupVariable.NVLockSetupControl == 0)
        {
            // Return without locking
            return EFI_SUCCESS;
        }
    }

	g_NVWriteProtect = TRUE;

    NvLockMailbox.NVLockState = 1; //Lock NV
    RetStatus = DxeSetVariable( L"NvLockMailbox", &NVLockMailboxVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS , sizeof(NvLockMailbox), &NvLockMailbox);
    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Set Status: %x", RetStatus));

    return EFI_SUCCESS;
}

/**
    This Function Interface to unlock NVRAM

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS UnlockNV()
{
    EFI_STATUS RetStatus = EFI_UNSUPPORTED;

	g_NVWriteProtect = FALSE;

	NvLockMailbox.NVLockState = 0; //NV unlocked
    RetStatus = DxeSetVariable( L"NvLockMailbox", &NVLockMailboxVariableGuid,
                                  EFI_VARIABLE_BOOTSERVICE_ACCESS , sizeof(NvLockMailbox), &NvLockMailbox);
    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Set Status: %x", RetStatus));

    return EFI_SUCCESS;
}

/**
    This function compares the variable is SETUPPASSWD

    @param CHAR16   *VariableName - Pointer to Variable name
           EFI_GUID *VendorGuid - Pointer to VendorGuid

    @retval BOOLEAN - TRUE if Variable is SETUPPASSWD, Otherwise FALSE

**/
BOOLEAN IsSetupPasswordVariable(
    IN CHAR16   *VariableName,
    IN EFI_GUID *VendorGuid
)
{
	BOOLEAN RetFlag = FALSE;
	EFI_GUID SetupPwdGuid = SETUPPASSWD_GUID;

    if ((guidcmp(VendorGuid, &SetupPwdGuid) == FALSE) && (Wcscmp((CHAR16 *)VariableName, (CHAR16 *)SETUPPASS_VAR_NAME) == FALSE))
    {
        RetFlag = TRUE;
    }

    DEBUG((DEBUG_INFO, "\n IsSetupPass: return: %x", RetFlag));
    return RetFlag;
}

/**
    This function to say whether the NV Write to the Setup Variable is Protected or Not

    @param VOID

    @retval BOOLEAN - TRUE  : If in Locked State
                      FALSE : If in UnLocked State
**/
BOOLEAN IsNVWriteProtected()
{
	return g_NVWriteProtect;
}

/**
    This Function to call all the hook functions in ValidateAndLockNVHookList

    @param  CHAR16 *VariableName  - NV Variable Name
            EFI_GUID *VendorGUID  - Variable GUID
            UINT32 Attributes     - BS / RT / NV
            UINTN DataSize        - Size of the Data
            VOID *Data            - Variable Data

    @retval EFI_STATUS
**/
EFI_STATUS ValidateAndLockNVHook(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
    UINTN i;
    EFI_STATUS Result = EFI_UNSUPPORTED;

    for (i = 0; ValidateAndLockNVHookList[i] && (Result == EFI_UNSUPPORTED); i++)
    {
        Result = ValidateAndLockNVHookList[i](VariableName, VendorGuid, Attributes, DataSize, Data);
    }

    return Result;
}

/**
    This Function to check whether the Variable is $SETUPPASSWD or not.
    If $SETUPPASSWD, Validates the password in Variable data and Locks/Unlocks NV

    @param  CHAR16 *VariableName  - NV Variable Name
            EFI_GUID *VendorGUID  - Variable GUID
            UINT32 Attributes     - BS / RT / NV
            UINTN DataSize        - Size of the Data
            VOID *Data            - Variable Data

    @retval EFI_UNSUPPORTED  : If Variable is not $SETUPPASSWD
            EFI_SUCCESS : If Variable is $SETUPPASSWD
**/
EFI_STATUS ValidateAndLockNV(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 Attributes, UINTN DataSize, VOID *Data)
{
    UINT32 PasswordState = AMI_PASSWORD_NONE;
    CHAR16 *PassWord = (VOID*)Data;
    EFI_STATUS Status = EFI_SUCCESS;
    SETUP_DATA NvLockSetupVariable = {0};
    UINTN VarSize = sizeof(NvLockSetupVariable);

    if (IsSetupPasswordVariable(VariableName, VendorGuid) != TRUE)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: Not SETUPPASS_VAR_NAME"));
        return EFI_UNSUPPORTED;    // Not a SETUPPASS_VAR_NAME and SETUPPASSWD_GUID
    }
    
    //Check for the AmiSetupNvLock Support, if the support is disabled then $SETUPPASSWD, $SETADMINPASSWD and $SETUSERPASSWD should not be handled.
    Status =  DxeGetVariable(L"Setup", &gSetupVariableGuid, NULL, &VarSize, &NvLockSetupVariable);
    if(!EFI_ERROR(Status))
    {
        if(NvLockSetupVariable.NVLockSetupControl == 0)
        {
            // return unsupported so that Set variable will be handled. 
            return EFI_UNSUPPORTED;
        }
    }
    
    if (gPasswordRetryCount == 3)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: PasswordRetryCount 3:Access Denied"));
        return EFI_ACCESS_DENIED;
    }

    //Invoke the to authentication method PasswordAuthenticate
    PasswordState = AuthenticatePassword(PassWord, DataSize);

    //If the status is AMI_PASSWORD_NONE, the password is not valid ignore the write
    if (PasswordState == AMI_PASSWORD_NONE)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: No PASSWORD Set"));
        LockNV();
        Status = EFI_NOT_READY;	//When Admin Password is not set
    }
    else if (PasswordState == AMI_INVALID_PASSWORD)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: INVALID PASSWORD"));

        if(IsNVWriteProtected() == TRUE)
        {
            gPasswordRetryCount++;
        }
        else
        {
            gPasswordRetryCount = 0;
        }

        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: PasswordRetryCount: %x", gPasswordRetryCount));

        LockNV();
        Status = EFI_SECURITY_VIOLATION;	//When Invalid password is provided for validation
    }
    else
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateAndLockNV: Unlocks NV"));
        //If the status is ADMIN, then update the g_NVWriteProtect with FALSE
        UnlockNV();
        Status = EFI_SUCCESS;
        gPasswordRetryCount = 0;
    }

    return Status;
}

/**
    Hook for the Variable Write Method SetVariableHook().
    This method should not allow the Write Calls for Setup / AMITSESetup until the password provided in SETUPPASSWD variable.

    @param  CHAR16 *VariableName  - NV Variable Name
            EFI_GUID *VendorGUID  - Variable GUID
            UINT32 Attributes     - BS / RT / NV
            UINTN DataSize        - Size of the Data
            VOID *Data            - Variable Data

    @retval EFI_SECURITY_VIOLATION : NV Locked, The variable could not be updated
            EFI_UNSUPPORTED : Variable could be updated
**/
EFI_STATUS 	AMISetupNVWriteHook(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 Attributes, UINTN DataSize, VOID *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_STATUS RetStatus = EFI_UNSUPPORTED;
	EFI_GUID SetupPwdGuid = SETUPPASSWD_GUID;
	UINTN VarSize = sizeof(NvLockMailbox);

    if (!Wcscmp(VariableName, L"NvLockMailbox") && (guidcmp(VendorGuid, &NVLockMailboxVariableGuid) == FALSE))
    {
        //If Variable has EFI_VARIABLE_RUNTIME_ACCESS,Block the access
        if ((Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == EFI_VARIABLE_RUNTIME_ACCESS)
        {
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: Set NvLockMailbox Var Access denied "));
            return EFI_ACCESS_DENIED;
        }
        else
        {
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: Set NvLockMailbox Success"));
            return EFI_UNSUPPORTED ;
        }
    }
    
    //Always allow access for the DummyNvLockVar Variable
    if (!Wcscmp(VariableName, L"DummyNvLockVar") && (guidcmp(VendorGuid, &DummyNVLockVariableGuid) == FALSE))
	{
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: Set DummyNvLockVar Success"));
        return EFI_UNSUPPORTED ;
	}

    if (pSmst == NULL) // DXE
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: In DXE"));
        gInSmmPhase = FALSE;
    }
    else // SMM
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: In SMM"));
        gInSmmPhase = TRUE;
    }

	// only for DXE
	if(!gInSmmPhase && !gEventsRegistered)
	{
		RegisterEvents();
		gEventsRegistered = TRUE;
	}

    if(gInSmmPhase)
    {
        //When entering Runtime, SetVariable for BOOTSERVICE_ACCESS variable will fail. 
        //So use this to identify Runtime entry and enable Lock for Runtime.
        if(FALSE == g_LockedinRuntime)
        {
            UINT8 DummyData = 0;
            
            RetStatus = DxeSetVariable( L"DummyNvLockVar", &DummyNVLockVariableGuid,
                                    EFI_VARIABLE_BOOTSERVICE_ACCESS , sizeof(DummyData), &DummyData);
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: DummyNvLockVar Set Status: %x", RetStatus));
            if(RetStatus == EFI_SUCCESS)
            {
                RetStatus = DxeSetVariable( L"DummyNvLockVar", &DummyNVLockVariableGuid,
                                                        EFI_VARIABLE_BOOTSERVICE_ACCESS , 0, &DummyData);
                DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: DummyNvLockVar Delete Status: %x", RetStatus));
            }
            else
            {
                LockNV();
                //g_ControlInSetup = FALSE;
                g_LockedinRuntime = TRUE;
                DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: RunTime Lock Applied"));
            }
        }
        
        if(FALSE == g_InstallSetupNVProtocol)
        {
            RetStatus = InstallSetupNVControlProtocol();
            if(RetStatus == EFI_SUCCESS)//Install SetupNvControl Protocol Only once
                g_InstallSetupNVProtocol = TRUE;
        }
    }

    //During Flash skip the check for Lock and Unlock
    //and After entering OS don't use NvLockMailBox
    if((gSmiFlashUpdate == FALSE) && (FALSE == g_LockedinRuntime))
    {
        // If Variable is present, Control comes in SMM phase first time after ReadyToBoot.
        RetStatus =  DxeGetVariable(L"NvLockMailbox", &NVLockMailboxVariableGuid, NULL, &VarSize, &NvLockMailbox);

        if(RetStatus == EFI_SUCCESS )
        {
            //Based on Lock state, Lock/Unlock NV
            if(NvLockMailbox.NVLockState == 1)
            {
                DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: DxeGetVariable LockNV"));
                //LockNV();
                g_NVWriteProtect = TRUE;
                //g_ControlInSetup = FALSE;
            }
            else
            {
                DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: DxeGetVariable UnLockNV"));
                //UnlockNV();
                g_NVWriteProtect = FALSE;
                //g_ControlInSetup = TRUE;
            }
        }
        else
        {
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: DxeGetVariable VarFail: %x", RetStatus));
        }
    }

	//If we are not on Setup,Then only need to go for Lock and Unlocking
	//if(FALSE == g_ControlInSetup)
	{
		//Check the VariableName is $SETUPPASSWD, with the said GUID
		Status = ValidateAndLockNVHook(VariableName, VendorGuid, Attributes, DataSize, Data);

        if (EFI_UNSUPPORTED == Status)
		{
			int i = 0;
			SETUP_VARIABLE_DATA VarsToValidate[] = {VARS_TO_VALIDATE_LIST {"", NULL_GUID}};//Array of defined Formset Guids
			int varcount = sizeof(VarsToValidate)/sizeof(SETUP_VARIABLE_DATA);
			CHAR8 VarNameinChar8[256] = {0};	//[EIP:181511]:Used Static Memory to avoid memory crash in Runtime(OS).
			
			//Set the Setup Password Admin or User Using $SETADMINPASSWD and $SETUSERPASSWD Variables
			if( ((guidcmp(VendorGuid, &SetupPwdGuid) == FALSE) && (Wcscmp((CHAR16 *)VariableName, (CHAR16 *)SET_ADMINPASS_VAR_NAME) == FALSE) )||
						((guidcmp(VendorGuid, &SetupPwdGuid) == FALSE) && (Wcscmp((CHAR16 *)VariableName, (CHAR16 *)SET_USERPASS_VAR_NAME) == FALSE)) )
			{
                Status = ValidateandSetSetupPwd(VariableName, VendorGuid, Attributes, DataSize, Data);
                DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: ValidateandSetSetupPwd Status: %x\n", Status));
				return Status;
			}

			ConvertChar16toChar8(VariableName,VarNameinChar8);

			//If VariableName is not $SETUPPASSWD, $SETADMINPASSWD and $SETUSERPASSWD, then Check for Lock
			for(i = 0; i < varcount; i++)
			{
				if((Strcmp(VarNameinChar8, (VarsToValidate[i].VariableName)) == 0) &&
					(guidcmp(VendorGuid, &(VarsToValidate[i].VariableGuid)) == 0))
				{
                    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: Security Violation Check for: Var: %s, %x, %x", VarNameinChar8, IsNVWriteProtected(), gSmiFlashUpdate));

                    //If the Write is Protected report EFI_SECURITY_VIOLATION
                    if ((IsNVWriteProtected() == TRUE) && (gSmiFlashUpdate == FALSE))
                    {
                        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: AMISetupNVWriteHook: Security Violation: Applied: %x, Flash: %x", IsNVWriteProtected(), gSmiFlashUpdate));
						Status = EFI_SECURITY_VIOLATION;
					}

					break;
				}
			}
		}
	}

	DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Exit\n"));
	return Status;
}

/**
    Function to update the global flag to check for flash update

    @param  BOOLEAN Start - TRUE/FALSE

    @retval EFI_STATUS
**/

EFI_STATUS ToggleSmiFlashUpdateFlag (BOOLEAN Start)
{
    gSmiFlashUpdate = !Start;
    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Setting gSmiFlashUpdate to %d", gSmiFlashUpdate));
    return EFI_SUCCESS;
}

/**
    Function to install the NVControl protocol

    @param  VOID

    @retval EFI_STATUS
**/
EFI_STATUS InstallSetupNVControlProtocol()
{
    EFI_STATUS status = EFI_NOT_READY;

    if (pSmst != NULL)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Install NV Protocol"));
        status = pSmst->SmmInstallConfigurationTable(
                     pSmst, &gAmiSetupNvControlProtocolGuid, &SetupNvControl, sizeof(SetupNvControl)
                 );
    }

    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: Install NV Protocol Status: %x", status));
    return status;
}

/**
    Function to set the Setup Password (User or Admin)

    @param  CHAR16 *VariableName  - NV Variable Name
            EFI_GUID *VendorGUID  - Variable GUID
            UINT32 Attributes     - BS / RT / NV
            UINTN DataSize        - Size of the Data
            VOID *Data            - Variable Data


    @retval EFI_STATUS
**/
EFI_STATUS ValidateandSetSetupPwd(CHAR16 *VariableName, EFI_GUID *VendorGuid, UINT32 Attributes, UINTN DataSize, VOID *Data)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	CHAR16 *PassWord = (VOID*)Data;
	
    if (FALSE == IsNVWriteProtected())
    {
        
        DEBUG((DEBUG_INFO, "\n SetupNVLockNotify: ValidateandSetSetupPwd Received Password Length: %d", DataSize));
#if (SETUP_STORE_KEYCODE_PASSWORD == 0)
        
        //For Unicode Password,exclude the size of NULL for PasswordSize
        if ( ( DataSize >= 2 ) && ( L'\0' == PassWord[(DataSize/2) -1] ) )
        {
            DataSize = DataSize - 2;
            DEBUG((DEBUG_INFO, "\n SetupNVLockNotify: ValidateandSetSetupPwd Modified Password Length: %d", DataSize));
        }
#endif
        //Password length should be minimum PASSWORD_MIN_SIZE chars and maximum is SETUP_PASSWORD_LENGTH
        if ((DataSize < (PASSWORD_MIN_SIZE * sizeof(CHAR16)) ) || (DataSize > (SETUP_PASSWORD_LENGTH * sizeof(CHAR16)) ))
        {
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateandSetSetupPwd Invalid Length: %d", DataSize));
            Status = EFI_INVALID_PARAMETER;
        }
        else
        {
            DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateandSetSetupPwd: SetPassword"));
            Status = SetPassword(VariableName, Data, DataSize);
        }
    }
    else
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ValidateandSetSetupPwd SetPWD: NV write protected"));
        Status = EFI_SECURITY_VIOLATION;
    }

    return Status;
}

/**
    Function to Converts CHAR16 string to CHAR8

    @param  CHAR16 *InputString     - Input CHAR16 String
            CHAR8 *OutString       - Output CHAR8 string


    @retval void
**/
void ConvertChar16toChar8(CHAR16 *InputString,CHAR8 *OutString)
{
	UINTN Index = 0;

    if (*InputString == L'\0')
    {
        return;
    }

	while(InputString[Index] != L'\0')
	{
		OutString[Index] = (CHAR8)InputString[Index];
		Index++;
	}

	OutString[Index] = '\0';
}

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

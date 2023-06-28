//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/BootOnly/protocol.c $
//
// $Author: Premkumara $
//
// $Revision: 35 $
//
// $Date: 9/24/12 9:14a $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//

/** @file protocol.c
    This file contains code for TSE exported protocol
    functions.

**/

#include "minisetup.h"
#include "boot.h"
#include "Include/Protocol/AMIPostMgr.h"
#include "Include/Protocol/AMIScreenMgmt.h"
//<Ouyyx-20190831 To support UEFI PXE Retry boot +>
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1)
#include <NetworkStackSetup.h>
//<Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function +>
#include <AmiIpmiPkg/Include/Protocol/FrbProtocol.h>
//<Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function ->
#include <AMIVfr.h> //<Lvshh001-20181103 Add UEFI Shell disable token >
#include <Setup.h>
#endif
//<Ouyyx-20190831 To support UEFI PXE Retry boot ->

UINT32 gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
BOOLEAN gBootNextFlag = FALSE;
UINT16 *pOSRecoveryOrder = (UINT16 *)NULL;
BOOLEAN SecondBootFlag = FALSE;
BOOLEAN gOsRecoverySupported=FALSE;
BOOLEAN gPlatformRecoverySupported=FALSE;
BOOLEAN gDoNotBoot = FALSE;
BOOLEAN gLaunchOtherSetup = FALSE;
BOOLEAN gDrawQuietBootMessage = FALSE;
extern UINT16 gGifSrcX;
extern UINT16 gGifSrcY;
extern BOOLEAN GifImageFlag;
extern BOOLEAN ActivateInputDone;
BOOT_DATA *RecoveryGetRecoveryData( UINT16 Option, BOOLEAN OsRecovery);
EFI_STATUS ProcessRecovery(UINT16 *pRecoveryOrder,UINTN size, BOOLEAN OsRecovery);
VOID StopClickEvent(VOID);
void ClearGrphxScreen (void);

static EFI_HANDLE gProtocolHandle = NULL;
//EFI_GUID gAmiPostManagerProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
BOOLEAN TseIgnoreKeyForFastBoot();
BOOLEAN GetBBSOptionStatus(BOOT_DATA *pBootData); 
extern VOID *SavePostScreen( UINTN *SizeOfX, UINTN *SizeOfY );
extern VOID RestorePostScreen( VOID *UgaBlt, UINTN SizeOfX, UINTN SizeOfY );
extern BOOLEAN IsTSEGopNotificationSupport();
static AMI_POST_MANAGER_PROTOCOL	gPostManagerProtocol =
{
	PostManagerHandshake,
	PostManagerDisplayPostMessage,
    PostManagerDisplayPostMessageEx,
    PostManagerDisplayQuietBootMessage,
    PostManagerDisplayMsgBox,
    PostManagerSwitchToPostScreen,
	PostManagerSetCursorPosition,
	PostManagerGetCursorPosition,
	PostManagerInitProgressBar,
	PostManagerSetProgressBarPosition,
	PostManagerGetPostStatus,
	PostManagerDisplayInfoBox,
	PostManagerSetAttribute,
	PostManagerDisplayMenu,
	PostManagerDisplayMsgBoxEx,
	PostManagerDisplayProgress,
	PostManagerGetAttribute, 	
	PostManagerDisplayTextBox 	
};

static AMI_TSE_SCREEN_MGMT_PROTOCOL	gScreenMgmtProtocol =
{
		ScreenMgmtSaveTseScreen, 	
		ScreenMgmtRestoreTseScreen,
		ScreenMgmtClearScreen
};

static UINTN	gScreenWidth;
static UINTN 	gScreenHeight;
static EFI_UGA_PIXEL *gScreenBuffer;
static SCREEN_BUFFER *gScreenBuf = NULL;


//<Ouyyx-20190831 To support UEFI PXE Retry boot +>
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
extern BOOT_FLOW    *gBootFlowTable; //<lubl-20170919 uefi pxe boot>
EFI_GUID        gEfiNetworkStackSetupGuid = NETWORK_STACK_GUID;
NETWORK_STACK   gNetworkStackSetupData;
//<Lvshh001-20181103 Add UEFI Shell disable token +>
EFI_GUID       AmiTseSetupguid = AMITSESETUP_GUID;
AMITSESETUP    TSEVar;  
static EFI_GUID            gEfiSetupGuid           = SETUP_GUID;
SETUP_DATA          SetupData;
//<Lvshh001-20181103 Add UEFI Shell disable token ->
//<Ouyyx-20190831 don't launch Built-In uefi shell +> 
#define AMI_MEDIA_DEVICE_PATH_GUID \
    { 0x5023b95c, 0xdb26, 0x429b, { 0xa6, 0x48, 0xbd, 0x47, 0x66, 0x4c, 0x80, 0x12 } }

struct {
    VENDOR_DEVICE_PATH Media;
    EFI_DEVICE_PATH_PROTOCOL End;
} FwLoadFileDp = {
    {
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        AMI_MEDIA_DEVICE_PATH_GUID
    },
    {
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        sizeof(EFI_DEVICE_PATH_PROTOCOL)
    }
};
//<Ouyyx-20190831 don't launch Built-In uefi shell -> 
#endif
//<Ouyyx-20190831 To support UEFI PXE Retry boot ->


/**
 
    <Ouyyx-20190831 This function Check SubStr if exist in BufStr.>
    @param VOID
    @return

**/
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
BOOLEAN Strstr_oyyx( CHAR16 *BufStr, CHAR16 *SubStr )
{
    CHAR16 *bp, *sp;
    if(!*SubStr)
        return FALSE;
    while (*BufStr)
    {
        bp = BufStr;
        sp = SubStr;
        do{
            if(!*sp)
                return TRUE;
        } while (*bp++ == *sp++);
        BufStr++;
    }
    return FALSE;
}
#endif


/**
    <Ouyyx-20190831 This function CheckNetworkStackName When Define UEFI_PxeRetry.>

    @param VOID

    @retval if Check Network Return True Otherwise Return FLASE.

**/
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
BOOLEAN CheckNetworkStackName (CHAR16 *Name)
{
    CHAR16      CheckName1[]=L"Network Card";
    CHAR16      CheckName2[]=L"UEFI: IP4";
    CHAR16      CheckName3[]=L"UEFI: IP6";
    //<Xuran-20180108-Add UEFI PXE string+>
    CHAR16      CheckName4[]=L"UEFI: PXE IP4";
    CHAR16      CheckName5[]=L"UEFI: PXE IP6";
    UINT8       CheckLength1 = 12, CheckLength2 = 9, CheckLength3 = 13, i = 0;
    //<Xuran-20180108-Add UEFI PXE string->
    
    //<Ouyyx-20190918-Improve CheckNetwork Function When UEFI_PXE_Retry Enabled +>
    CHAR16      CheckName6[]=L"UEFI: ";
    CHAR16      CheckName7[]=L"IP6";
    CHAR16      CheckName8[]=L"IP4";
    
    DEBUG((DEBUG_ERROR,"[xuran-20180108] BootData name: %s\n",Name));
    if(Strstr_oyyx(Name,CheckName1))
        return TRUE;
    else if(Strstr_oyyx(Name,CheckName6))
    {
        if(Strstr_oyyx(Name,CheckName7) || Strstr_oyyx(Name,CheckName8))
            return TRUE;  
        else
            return FALSE;
    }
    else
        return FALSE;
    //<Ouyyx-20190918-Improve CheckNetwork Function When UEFI_PXE_Retry Enabled ->   
         
    //<Xuran-20180108-Add UEFI PXE string->
   /*     
    DEBUG((DEBUG_ERROR,"[xuran-20180108] BootData name: %s\n",Name));
    
    for(i = 0; (i < CheckLength1)&&(((CHAR16)*(Name+i)) == CheckName1[i]); i++);
    if(i == CheckLength1) return TRUE;
    
    for(i = 0; (i < CheckLength2)&&(((CHAR16)*(Name+i)) == CheckName2[i]); i++);
    if (i==CheckLength2) return TRUE;
    
    for(i = 0; (i < CheckLength2)&&(((CHAR16)*(Name+i)) == CheckName3[i]); i++);
    if(i == CheckLength2) return TRUE;
    
    //<Xuran-20180108-Add UEFI PXE string+>
    for(i = 0; (i < CheckLength3)&&(((CHAR16)*(Name+i)) == CheckName4[i]); i++);
    if(i == CheckLength3) return TRUE;
    
    for(i = 0; (i < CheckLength3)&&(((CHAR16)*(Name+i)) == CheckName5[i]); i++);
    if(i == CheckLength3) return TRUE;
    */
    //<Xuran-20180108-Add UEFI PXE string->
    //return FALSE;
}
#endif

EFI_STATUS InvalidateBgrtStatusByProtocol (VOID);
TSE_INVALIDATE_BGRT_STATUS_PROTOCOL gInvalidateBgrtStatus = {InvalidateBgrtStatusByProtocol};
EFI_STATUS InstallInvalBGRTStatusProtocol (EFI_HANDLE Handle);

EFI_STATUS ShowPostMsgBox(IN CHAR16  *MsgBoxTitle,IN CHAR16  *Message,IN UINT8  MsgBoxType, UINT8 *pSelection);


EFI_STATUS ShowPostMsgBoxEx(
	IN CHAR16			*Title,
 	IN CHAR16			*Message,
 	IN CHAR16			*Legend,
 	IN MSGBOX_EX_CATAGORY	 	MsgBoxExCatagory,
 	IN UINT8	 		MsgBoxType,
    	IN UINT16			*OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
    	IN UINT16	 		OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
    	IN AMI_POST_MGR_KEY		*HotKeyList, 		// NULL - AnyKeyPress closes
    	IN UINT16 			HotKeyListCount, 
    	OUT UINT8			*MsgBoxSel,
    	OUT AMI_POST_MGR_KEY		*OutKey
	);
EFI_STATUS ShowInfoBox(IN CHAR16  *InfoBoxTitle, IN CHAR16  *Message, IN UINTN   TimeLimit, EFI_EVENT  *RetEvent);

EFI_STATUS HiiString2BltBuffer(	CHAR16 *Message,
								EFI_UGA_PIXEL Foreground, 
								EFI_UGA_PIXEL Background, 
								OUT	UINTN *Width,
								OUT EFI_UGA_PIXEL **BltBuffer,
								OUT UINTN *BltGlyphWidth);
EFI_STATUS DrawHiiStringBltBuffer(CHAR16 *Message, INTN CoOrdX, INTN CoOrdY, CO_ORD_ATTRIBUTE Attribute, EFI_UGA_PIXEL Foreground, EFI_UGA_PIXEL Background);
EFI_STATUS ShowPostMenu (
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,	  
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16  *pSelection
);

EFI_STATUS ShowPostTextBox(
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE DisplayTextKeyValidate
);

EFI_STATUS ShowPostProgress(
    IN UINT8			ProgressBoxState, 
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,	
    IN UINTN 			Percent,	
    IN OUT VOID			**Handle,	
    OUT AMI_POST_MGR_KEY	*OutKey
);
BOOLEAN   	IsTseBestTextGOPModeSupported (VOID);
EFI_STATUS	SaveCurrentTextGOP ( UINTN *currenttextModeCols, UINTN *currenttextModeRows, UINT32 *currentGOPMode );
EFI_STATUS	RestoreTextGOPMode ( UINTN prevTextModeCols, UINTN prevTextModeRows, UINT32 prevGOPMode ); 
VOID SaveGraphicsScreen(VOID);

/**
    This function installs different protocols exported.

    @param VOID

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS InstallProtocol( VOID )
{
	EFI_STATUS Status;

	Status = gBS->InstallMultipleProtocolInterfaces(
			&gProtocolHandle,
			&gAmiPostManagerProtocolGuid, &gPostManagerProtocol,
#ifdef USE_COMPONENT_NAME
			&gEfiComponentNameProtocolGuid, &gComponentName,
#endif
			NULL
			);
	if ( !EFI_ERROR( Status ) )
	{
		Status = InstallFormBrowserProtocol(gProtocolHandle);
		Status = InstallInvalBGRTStatusProtocol (gProtocolHandle);
		Status = InstallScreenMgmtProtocol (gProtocolHandle);
	}

	return Status;
}

/**
    This function uninstalls different protocols exported.

    @param VOID

    @retval VOID

**/
VOID UninstallProtocol( VOID )
{
	gBS->UninstallMultipleProtocolInterfaces(
			&gProtocolHandle,
			&gAmiPostManagerProtocolGuid, &gPostManagerProtocol,
#ifdef USE_COMPONENT_NAME
			&gEfiComponentNameProtocolGuid, &gComponentName,
#endif
			NULL
			);

	UnInstallFormBrowserProtocol(gProtocolHandle);
	UninstallScreenMgmtProtocol(gProtocolHandle);

}

/**
    This function installs Screen management protocol.

    @param EFI_HANDLE Handle

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS InstallScreenMgmtProtocol( EFI_HANDLE Handle)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status = gBS->InstallMultipleProtocolInterfaces (
				  &Handle,
				  &gTSEScreenMgmtProtocolGuid,
				  &gScreenMgmtProtocol,
				  NULL
				  );
	
	return Status;
}

/**
    This function uninstalls screen management protocol.

    @param EFI_HANDLE Handle

    @retval VOID

**/
VOID UninstallScreenMgmtProtocol( EFI_HANDLE Handle)
{
	gBS->UninstallMultipleProtocolInterfaces(
			&Handle,
			&gTSEScreenMgmtProtocolGuid, 
			&gScreenMgmtProtocol,
			NULL
			);
}

VOID SetBootTimeout( EFI_EVENT Event, BOOLEAN *timeout )
{
	if ( timeout != NULL )
		*timeout = TRUE;
}

VOID AboartFastBootPath(VOID)
{
	gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
	gPostManagerHandshakeCallIndex--;
    gRT->SetVariable(
    			L"BootFlow",
    			&_gBootFlowGuid,
    			EFI_VARIABLE_BOOTSERVICE_ACCESS,
    			sizeof(gBootFlow),
    			&gBootFlow
    			);
}
BOOLEAN IsBootTimeOutValueZero(VOID);
/**
    This function is the handshake function to which BDS
    hands-off.

    @param VOID

    @retval This function never returns. It only boots different
        options.

**/
EFI_STATUS PostManagerHandshake( VOID )
{
	EFI_EVENT timer = NULL;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	volatile BOOLEAN bootTimeout = FALSE;
	UINT16  Value = 1;
    UINT64  TimeoutValue;
	UINTN   size = 0;
	EFI_GUID    AmitseAfterFirstBootOptionGuid = AMITSE_AFTER_FIRST_BOOT_OPTION_GUID;
	EFI_GUID AmiTseBeforeTimeOutGuid = AMITSE_BEFORE_TIMEOUT_GUID;
	EFI_GUID AmiTseAfterTimeOutGuid = AMITSE_AFTER_TIMEOUT_GUID;
	BOOLEAN AfterFirstBootSignalled = FALSE;
	UINT16  *pBootOrder=NULL;
	UINT16  u16BootCount = 0,i;				//Dont change i data type, in infinite loop it is reinitialized to -1.
	UINT16  BootNext, *pBootNext = NULL;
	UINTN   RecoverySize = 0;
	UINT16 *pRecoveryOrder=NULL;
	BOOLEAN BootSuccess=FALSE;
	UINT64 		OsIndications = 0;
	UINT32 		Attributes = 0;
	UINTN 		DataSize = sizeof (UINT64);  

	INT16      Time            = 0; //<Ouyyx-20190831 Count the Times of Launch LAN Option>
	//<Ouyyx-20190831 Add UEFI Shell disable token +>
	UINTN           AMITSESETUPSize             = sizeof(AMITSESETUP);
	UINTN            BufferSize                 = sizeof(SETUP_DATA);
	BOOLEAN         BuildUefiShellFlag          = FALSE;	
	//<Ouyyx-20190831 Add UEFI Shell disable token ->
//<Ouyyx-20190831 To support UEFI PXE Retry boot +> 
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
	    //BOOT_FLOW       *bootFlowPtr = NULL;  //<lubl-20170919 uefi pxe boot>
	    INT16           UefiPxeRetryCount               = 0;  
	    //<Chendl004-20180124 Fixed when press F12 hotkey UEFI PXE not retry +>
	    BOOLEAN         F12_Boot                    = FALSE;
	    //<Chendl004-20180124 Fixed when press F12 hotkey UEFI PXE not retry ->
	    BOOLEAN         CheckPxeFlag                = FALSE;
	    BOOLEAN         FirstDetectFlag             = TRUE;
	    BOOLEAN         EndlessPxeRetryFlag         = FALSE;
	    UINTN           NetworkStackSize            = sizeof(NETWORK_STACK);
	    //<Ouyyx-20190831 Add UEFI Shell disable token +>
	    //<Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function +>
	    EFI_SM_FRB_PROTOCOL    *FrbProtocol;
	    //<Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function ->
#endif
//<Ouyyx-20190831 To support UEFI PXE Retry boot ->
	// Performance measurement starta
	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);

	RUNTIME_DEBUG( L"mre" );

	gPostManagerHandshakeCallIndex++;

	if(gPostManagerHandshakeCallIndex!=1)
	{
		gEnterSetup = TRUE;
		goto _ShowMainMenu;
	}

    PostManagerHandShakeHookHook();

#ifndef STANDALONE_APPLICATION
	if ( ! gConsoleControl )
	{
		//All necessary protocols are not available yet.
		//We can still proceed if Uga Draw is the only protocol not available yet.
		ActivateApplication();
		if (!IsDelayLogoTillInputSupported())
		{
			ActivateInputDone = TRUE;
			ActivateInput();
		}
	}
#endif

    if(!gVariableList)
        VarLoadVariables( (VOID **)&gVariableList, NULL );

	TimerStopTimer( &gKeyTimer );

	// this *MUST* be run a EFI_TPL_APPLICATION
	gBS->RaiseTPL( EFI_TPL_HIGH_LEVEL );	// guarantees that RestoreTPL won't ASSERT
	gBS->RestoreTPL( EFI_TPL_APPLICATION );

	// Performance measurement Pause
//	PERF_END (0,L"Boot", NULL, 0);
	// Exclude IDEPasswordCheck IDEPasswordCheck that it may get the use input.
	// IDE password Module takes care of the Password check.
	//TSEIDEPasswordCheck(); 

	// Performance measurement continue
//	PERF_START (0, L"Boot", NULL, 0);    

    // get the current boot options and languages
	BootGetBootOptions();
	BootGetLanguages();

	StyleUpdateVersionString();

	gPostStatus = TSE_POST_STATUS_IN_BOOT_TIME_OUT;

	//Providing boot time out value depending on TSE_BOOT_TIME_OUT_AS_ZERO token
	if(!IsBootTimeOutValueZero())
		Value = GetBootTimeOut(Value);
	else
		Value = 0;

#ifdef STANDALONE_APPLICATION
    Value = 0;
    gEnterSetup = TRUE;
#endif

	if ( Value == 0 )
		bootTimeout = TRUE;

	if ( !TseIgnoreKeyForFastBoot() ) 
	{
		CheckForKeyHook( (EFI_EVENT)NULL, NULL );
	}

    if((BOOT_FLOW_CONDITION_NORMAL != gBootFlow) || (gEnterSetup==TRUE))
    {
        UINT32 condition = BOOT_FLOW_CONDITION_NORMAL;
        UINT32 *conditionPtr;

        size = 0;
        conditionPtr = VarGetNvramName( L"BootFlow", &_gBootFlowGuid, NULL, &size );
        if ( conditionPtr != NULL )
        	condition = *conditionPtr;
        
        MemFreePointer( (VOID **)&conditionPtr );

		if((condition == BOOT_FLOW_CONDITION_FAST_BOOT) && ((gBootFlow != condition)||(gEnterSetup==TRUE)))
		{
			// Take the Normal boot path as Fast boot path is altered by CheckForKeyHook().
			AboartFastBootPath();
			return EFI_UNSUPPORTED;
		}

        if(BOOT_FLOW_CONDITION_NORMAL == condition)
        {
            gRT->SetVariable(
    					L"BootFlow",
    					&_gBootFlowGuid,
    					EFI_VARIABLE_BOOTSERVICE_ACCESS,//BootFlow wil use RS attribute to avoid inconsistent attributes
    					sizeof(UINT32),
    					&gBootFlow
    					);
        }
        else
            gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
    }

	Status = BootFlowManageEntry();
	if (Status == EFI_UNSUPPORTED)
    {
        Value = 0xFFFF;
        gDoNotBoot = TRUE;
    }
	if(Status == EFI_NOT_STARTED)
		bootTimeout = TRUE;

	if(gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT)
	{
		UINT32 PasswordInstalled = PasswordCheckInstalled();
		
		if((gPasswordType == AMI_PASSWORD_NONE) && (PasswordInstalled != AMI_PASSWORD_NONE))
		{
			if(CheckSystemPasswordPolicy(PasswordInstalled))
			{
				// Take the Normal boot path as Password is installed and not validated by the user yet
				AboartFastBootPath();
				return EFI_UNSUPPORTED;
			}
		}
	}
    else
    {
	// Reinit Before Boottimeout So any action can set the boot flow again.
	gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
    }

	if ( Value != 0xFFFF )
    {
        TimeoutValue = (UINT64) TIMER_TENTH_SECOND;
        TimeoutValue = MultU64x32( TimeoutValue, (UINT32) Value );

        TimerCreateTimer( &timer, (EFI_EVENT_NOTIFY) SetBootTimeout, (VOID *)&bootTimeout, TimerRelative, TimeoutValue, EFI_TPL_CALLBACK );
    }

	// Performance measurement Pause
	PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);

	// While the boot timeout has not expired
	EfiLibNamedEventSignal (&AmiTseBeforeTimeOutGuid);
	while ( ! bootTimeout )
	{
		if ( gEnterSetup || gBootFlow )
		{
			bootTimeout = TRUE;
            continue;
		}
		if ( !TseIgnoreKeyForFastBoot() ) 
		{
			// check for 'hotkey' actions that would invoke something
			CheckForKeyHook( (EFI_EVENT)NULL, NULL );
		}

        TimeOutLoopHookHook();
	}
	EfiLibNamedEventSignal (&AmiTseAfterTimeOutGuid);

	// Performance measurement continue
	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);

	TimerStopTimer( &timer );

    if ((!gDoNotBoot) && (gBootFlow))
    {
        gRT->SetVariable(
					L"BootFlow",
					&_gBootFlowGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS,//BootFlow wil use RS attribute to avoid inconsistent attributes
					sizeof(UINT32),
					&gBootFlow
					);
        BootFlowManageEntry();
    }

_ShowMainMenu:

	if ( gEnterSetup )
	{
    	// Performance measurement Pause
    	PERF_END (0, AMITSE_TEXT("Boot"), NULL, 0);
    	// Disable access to the post manager protocol display post message functions and to 
        //  the switch to post screen functions of post manager protocol
    	gPostMsgProtocolActive = FALSE;

		gPostStatus = TSE_POST_STATUS_ENTERING_TSE;
		gSetupContextActive = TRUE;

		Status = gST->ConIn->Reset( gST->ConIn, FALSE );
		Status = MainSetupLoopHook();
		gSetupContextActive = FALSE;
		

    	// Reenable access to the post manager protocol display post message functions and to 
        //  the switch to post screen functions of post manager protocol
    	gPostMsgProtocolActive = TRUE;
    	// Performance measurement continue
    	PERF_START (0, AMITSE_TEXT("Boot"), NULL, 0);
	}


	if(gPostManagerHandshakeCallIndex!=1)
	{
		// In case of not a first call
		// Don't do the Boot Manager Work just exit the to caller.
		gPostManagerHandshakeCallIndex--;
		return Status;
	}

	// Pass control to the boot process to handle the selected boot option
	// if the boot is allowed
    if (gDoNotBoot)
   {
		if(!ItkSupport())
		{
			gEnterSetup = TRUE;
			goto _ShowMainMenu;
		}
		else
			gEnterSetup = FALSE;
    }

	if(!NoVarStoreSupport())
	{
		size = 0;
		//VarGetNvram( VARIABLE_ID_AMITSESETUP, &size );
		HelperGetVariable( VARIABLE_ID_AMITSESETUP, (CHAR16 *)NULL, (EFI_GUID *)NULL, NULL, &size );
	
		if(size < sizeof(AMITSESETUP))
		{
			Status = MiniSetupEntry();
			MiniSetupExit( Status );
		}
	}

	gPostStatus = TSE_POST_STATUS_PROCEED_TO_BOOT;

	// LoadDriverOrder();	//Remove Load Driver Option from TSE 2.x as it is handling from Core.

    ProcessProceedToBootHook();

    TSEUnlockHDD();

	if(ItkSupport())
	    BbsItkBoot();
	else
	{
	    //<Ouyyx-20190831 Check if UEFI PXE Retry Funtion +>         
	    #if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
	            
	            // <Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function +>
	            // Locate EFI_SM_FRB_PROTOCOL to send IPMI command to disable watchdog timer.
	            Status = gBS->LocateProtocol(
	                            &gEfiSmFrbProtocolGuid,
	                            NULL,
	                            (VOID **)&FrbProtocol 
	                            );
	            if(Status == EFI_SUCCESS)
	            {
	                // <Yaosr001-20190809 In the case of disable BMC, fixed the hang92 when enter to boot devices directly. +>
	                FrbProtocol->DisableFrb ( FrbProtocol, 1 );
	                // <Yaosr001-20190809 In the case of disable BMC, fixed the hang92 when enter to boot devices directly. ->
	            }
	            // <Malj-20190704 When Open uefi pxe endless retry funtion, We need disable frb2 function ->
	            //<Lvshh001-20181103 Add UEFI Shell disable token +>      
	            /*AMITSESETUPSize = sizeof (TSEVar);        
	            Status = pRS->GetVariable(
	                                L"AMITSESetup", \
	                                &AmiTseSetupguid, \
	                                NULL, \
	                                &AMITSESETUPSize, \
	                                &TSEVar );	*/	
				Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, NULL, &BufferSize, &SetupData);
	            if(!EFI_ERROR(Status) && (SetupData.UEFIShellDisableSelect == 0))
	            {
	                DEBUG((DEBUG_ERROR, "[OemDbg] Disable UEFI Shell!\n"));  
	                BuildUefiShellFlag = FALSE;
	            }
	            else
	            {
	                DEBUG((DEBUG_ERROR, "[OemDbg] Enable UEFI Shell!\n")); 
	                BuildUefiShellFlag = TRUE;
	            }
	            //<Lvshh001-20181103 Add UEFI Shell disable token ->
	            
	            NetworkStackSize = sizeof(gNetworkStackSetupData); 
	            Status= gRT->GetVariable(
	                                L"NetworkStackVar",
	                                &gEfiNetworkStackSetupGuid,
	                                NULL,
	                                &NetworkStackSize,
	                                &gNetworkStackSetupData
	                                );
	             
	            if(Status == EFI_SUCCESS)
	            {
	                //<Chendl001-20180329 Fixed pxe retry times not match Setup Setting +>
									//SGPZT#23576:PXE Can Not Loop All Network Device When Set As 1 +>
	                //UefiPxeRetryCount = gNetworkStackSetupData.UefiPxeRetryCount - 1; 
									UefiPxeRetryCount = gNetworkStackSetupData.UefiPxeRetryCount; 
									//SGPZT#23576:PXE Can Not Loop All Network Device When Set As 1 ->

	                DEBUG((DEBUG_ERROR,"[lubl-20170915] UefiPxeRetryCount=%d\n",UefiPxeRetryCount));
	                DEBUG((DEBUG_ERROR,"[lubl-20170915] gNetworkStackSetupData.UefiPxeRetryCount=%d\n",gNetworkStackSetupData.UefiPxeRetryCount));
	                //<Chendl001-20180329 Fixed pxe retry times not match Setup Setting ->
	                
	                //<Ouyyx-20190831 EndlessPxeRetry When UefiPxeRetryCount is 255 +>
	                    /*<Ouyyx-20190821>
	                     * BOOT_FLOW_CONDITION_NORMAL:bootFlowPtr->LaunchShell is 1 
	                     * BOOT_FLOW_HOT_KEY:bootFlowPtr->LaunchShell is 0
	                     */
	                    //<lubl-20170919 uefi pxe boot +>
	                    //bootFlowPtr = gBootFlowTable;
	                    //bootFlowPtr->LaunchShell = 0; //<Xuran-20180121-Allow to run shell->
	                    //DEBUG((DEBUG_ERROR,"[lubl-20170915] bootFlowPtr->LaunchShell1=%d \n", bootFlowPtr->LaunchShell));
	                    //<lubl-20170919 uefi pxe boot ->   

	                    //<Ouyyx-20190831 Check F12 Key Status +>
	                    if(gNetworkStackSetupData.F12_Hot_Key == 1)
	                    {
	                        F12_Boot = TRUE;
	                        gNetworkStackSetupData.F12_Hot_Key = 0; 
	                        DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] F12 HotKey is Pressed!!!\n"));
	                        NetworkStackSize = sizeof(gNetworkStackSetupData);
	                        Status = gRT->SetVariable(
	                                    L"NetworkStackVar",
	                                    &gEfiNetworkStackSetupGuid, 
	                                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE, 
	                                    NetworkStackSize,
	                                    &gNetworkStackSetupData
	                                    );
	                        if (EFI_ERROR(Status))
	                        {
	                            TRACE((TRACE_ALWAYS,"Set NetworkStackVar Variable is failed ... \n"));
	                        }  
	                    }
	                    else
						{
	                    	F12_Boot = FALSE;
	                        DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] F12 HotKey is UnPressed!!!\n")); 
							} 
	                    //<Ouyyx-20190831 Check F12 Key Status ->
	                if(gNetworkStackSetupData.UefiPxeRetryCount == 255)
	                {
	                    EndlessPxeRetryFlag = TRUE;
	                    DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] Set UefiPxeRetry is Endless!!!\n"));
	                }
	                //<Ouyyx-20190831 EndlessPxeRetry When UefiPxeRetryCount is 255 ->
	                
	                //<Ouyyx-20190831 Set UefiPxeRetry Times is UefiPxeRetryCount+>
	                else
	                {
	                    EndlessPxeRetryFlag = FALSE;
  
	                }
	                //<Ouyyx-20190831 Set UefiPxeRetry Times is UefiPxeRetryCount->
	            } 
	            //<Ouyyx-20190831 Add UEFI PXE Retry Funtion ->  
	    #endif
	    //Try boot next first
	    size = 0;
	    pBootNext = (UINT16 *)VarGetNvramName(L"BootNext", &gEfiGlobalVariableGuid, NULL, &size);
	    if(pBootNext && (size == sizeof(UINT16)))
	    {
	        BootNext = *pBootNext;
	        //Clear Boot next
	        VarSetNvramName(L"BootNext",
	                        &gEfiGlobalVariableGuid,
	                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
	                        pBootNext,
	                        0);
	        if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT) { 
				// FastBoot module normally checks for Bootnext before taking the FastBoot path
				// If the Boot next is set in FastBoot case then do the full boot.
				    AboartFastBootPath();
					return Status;
				}
			else {
				BBSSetBootNowPriorityForBootNext(BootNext, NULL, 0);
			}
		}
	
	
	    if(pBootNext)
	        MemFreePointer((void **) &pBootNext);
	
	    size =0 ;
	    pBootOrder = (UINT16 *)VarGetNvramName( L"BootOrder", &gEfiGlobalVariableGuid, NULL, &size );
	
	#ifdef EFI_NT_EMULATOR
	    if(!pBootOrder || !gBootData)
	    {
	        VarSetNvramName(L"BootOrder",
	        &gEfiGlobalVariableGuid,
	        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
	        NULL,
	        0);
	
	        return Status;
	    }
	#endif
	
	    //Get enabled options count
	    for(u16BootCount=0; u16BootCount<(UINT16)(size/sizeof(UINT16)); u16BootCount++ )
	    {
            if(!IsPreservedDisabledBootOptionOrder())
            {
    	        BOOT_DATA *pBootData;

	            pBootData = BootGetBootData(pBootOrder[u16BootCount]);
	            if(!(pBootData->Active & LOAD_OPTION_ACTIVE))
	                break;
            }
	    }
	
		if(!u16BootCount)
	    {
			gPostStatus = TSE_POST_STATUS_NO_BOOT_OPTION_FOUND;
	        //No valid/enabled boot option
			Status = BootFlowManageExit();
	            //infinite loop
	            while(EFI_NOT_STARTED == Status)
	            {
                    CHAR16 *text = NULL;
                    
                    text = HiiGetString( gHiiHandle, STRING_TOKEN(STR_NO_BOOT_OPTIONS));
                    if ( ( text != NULL ) && (gST->ConOut != NULL))
                    {
                    	gST->ConOut->OutputString(gST->ConOut, text);
                    	MemFreePointer( (VOID **)&text );
                    }
                    
                    //Wait for key
                    gBS->WaitForEvent( 1, &gST->ConIn->WaitForKey, &size );
                    CheckForKeyHook( (EFI_EVENT)NULL, NULL );
                    if (TRUE == gEnterSetup)                          //If setup key detects then launch the setup
                    {
                        goto _ShowMainMenu;
                    }
                    Status = BootFlowManageExit ();                 //If user plugs the any thumb drive after boot then TSE will try to launch the image from that file system using the path efi\boot\bootxxxx.efi
					if( gBootFlow )// Checking for gBootFlow, making gBootFlow to normal such that the key will be consumed in chcekforkey
						gBootFlow = BOOT_FLOW_CONDITION_NORMAL;
	            }
	    }
	
		if ( !gEnterSetup )// If no key is pressed to go to setup then stopping MousePointingDevice and stopping ClickEvents
		{
			StopClickEvent();
			MouseDestroy();
		}
//If Recovery support is disabled, normal boot options only should be processed
        if(!IsRecoverySupported())
            gOsRecoverySupported = FALSE;

//Check for OS Recovery flag and process normal boot if unavailable
_ProcessBootOptions:	
//<Ouyyx-20190831 To support UEFI PXE Retry boot +>
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
        do{
            DEBUG((DEBUG_ERROR,"[lubl-20170915] start here \n"));
            if((gOsRecoverySupported) || (u16BootCount==0 )) 
                break;//<Songts043_20190507 Fixed hang92 when no boot option with the UEFI pxe endless>

            Time = 0; //<Ouyyx-20190831 Count the Times of Launch LAN Option>
            for (i = 0; (!gOsRecoverySupported) && (i < u16BootCount); i++)
            {
                BOOT_DATA *pBootData;  
                pBootData = BootGetBootData(pBootOrder[i]); 
                if(pBootData == NULL || !(pBootData->Active & LOAD_OPTION_ACTIVE))
                    continue;
                
                //<Ouyyx-20190831 don't launch Built-In uefi shell +>
                if (BuildUefiShellFlag == FALSE)
                {
                    if (NODE_LENGTH(pBootData->DevicePath)==NODE_LENGTH(&FwLoadFileDp.Media.Header)
                            && MemCmp(pBootData->DevicePath,&FwLoadFileDp.Media.Header,NODE_LENGTH(pBootData->DevicePath))==0
                            )
                    {
                        DEBUG((DEBUG_ERROR,"[Ouyyx-20190831]  don't launch Built-In uefi shell\n"));
                        continue; 
                    }          
                }
                //<Ouyyx-20190831 don't launch Built-In uefi shell ->
   
                if ( BBSValidDevicePath(pBootData->DevicePath) ) 
                    if ( GetBBSOptionStatus (pBootData) )
                        continue;
                
                DEBUG((DEBUG_ERROR,"[xuran] UefiPxeRetryCount: %d \n",UefiPxeRetryCount));
                    
                if(CheckNetworkStackName(pBootData->Name)==TRUE) //if LAN option
                {   
                    DEBUG((DEBUG_ERROR,"[lubl-20170915] Detect LAN option \n"));
                    //<Ouyyx-20190831 Add Launch LAN Option Message +>
                    DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] Launch LAN %d option\n",Time));
                    Time++;
                    //<Ouyyx-20190831 Add Launch LAN Option Message ->
                    
                    FirstDetectFlag = FALSE;
                    CheckPxeFlag = TRUE;
                    Status = BootLaunchBootOption(pBootOrder[i], pBootOrder+i, u16BootCount - i);
                }
                else // Non-LAN device
                {
                    DEBUG((DEBUG_ERROR,"[lubl-20170915] Non-LAN device \n"));
                  
                    //<Chendl004-20180124 Fixed when press F12 hotkey UEFI PXE not retry +>
                    if((FirstDetectFlag == TRUE) && (F12_Boot == FALSE))
                    {//<Chendl004-20180124 Fixed when press F12 hotkey UEFI PXE not retry ->  
                       
                        DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] First Option is Not LAN device \n"));
                        Status = BootLaunchBootOption(pBootOrder[i], pBootOrder+i, u16BootCount - i);
                    }
                    else
                    {
                        Status = EFI_UNSUPPORTED;
                        if((EndlessPxeRetryFlag == FALSE) && (UefiPxeRetryCount == 0))
                        {
                            DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] Launch No LAN Device When UefiPxeRetryCount is Zero \n"));
                            SETUP_DEBUG_TSE("\n[lubl-test-2] pBootData->Name = %s,pBootData->Option = %d,pBootData->Active = %x,BBSValidDevicePath = %d\n",pBootData->Name,pBootData->Option,pBootData->Active,BBSValidDevicePath(pBootData->DevicePath));
                            Status = BootLaunchBootOption(pBootOrder[i], pBootOrder+i, u16BootCount - i);     
                        }  
                    }   
                }   
            }
            
            if(!EFI_ERROR(Status))
            {
                BootSuccess=TRUE;
            }
                
            if ( !AfterFirstBootSignalled )
            {
                EfiLibNamedEventSignal ( &AmitseAfterFirstBootOptionGuid );
                if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT) 
                {
                    AboartFastBootPath();
                    return Status;
                }   
                AfterFirstBootSignalled = TRUE;
            }
            
            //<Ouyyx-20190831 goto _ShowMainMenu +>
            if(CheckPxeFlag == FALSE || (CheckPxeFlag == TRUE && UefiPxeRetryCount == 0 && EndlessPxeRetryFlag == FALSE))
            {
                if((i+1) >= u16BootCount)
                {
                    gPostStatus = TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED;
                    Status = BootFlowManageExit();
                    if ( EFI_NOT_STARTED == Status )
                    {
                        CheckForKeyHook( (EFI_EVENT)NULL, NULL );
                        if (TRUE == gEnterSetup)  //If setup key detects then launch the setup
                        { 
                            DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] If setup key detects then launch the setup \n"));
                            goto _ShowMainMenu;
                        }              
                    } 
                                    
                    //<Xuran-20180121-Allow to enter Setup+>
                    gEnterSetup = TRUE;
                    DEBUG((DEBUG_ERROR,"[Ouyyx-20190831] Set gEnterSetup is TRUE then launch the setup \n"));
                    goto _ShowMainMenu;  
                    //<Xuran-20180121-Allow to enter Setup->
                }   
            }
            //<Ouyyx-20190831 goto _ShowMainMenu When No Lan Device And No Endless PxeRetry ->
            
            //<Xuran-20180121-Allow to enter Setup+>
            if(EndlessPxeRetryFlag == FALSE)
            {
                if(CheckPxeFlag == TRUE)
                {
                    if( (UefiPxeRetryCount--) <= 0)
                        UefiPxeRetryCount =0;
                }
            }
        }while(1);            
#else
        
	    for ( i = 0; (!gOsRecoverySupported) && (i < u16BootCount); i++)
	    {
	        BOOT_DATA *pBootData;

	        pBootData = BootGetBootData(pBootOrder[i]);

  	        if(pBootData == NULL || !(pBootData->Active & LOAD_OPTION_ACTIVE))
                continue;

			if ( BBSValidDevicePath(pBootData->DevicePath) ) 
				if ( GetBBSOptionStatus (pBootData) )
					continue;

	        Status = BootLaunchBootOption(pBootOrder[i], pBootOrder+i, u16BootCount - i);
	
	        if(!EFI_ERROR(Status))
	        	BootSuccess=TRUE;
	        if ( !AfterFirstBootSignalled )
			{
				EfiLibNamedEventSignal ( &AmitseAfterFirstBootOptionGuid );

				if( gBootFlow == BOOT_FLOW_CONDITION_FAST_BOOT) {
				    AboartFastBootPath();
					return Status;
				}	

				AfterFirstBootSignalled = TRUE;
			}
	
	        if((i+1) >= u16BootCount)
	        {
	        	gPostStatus = TSE_POST_STATUS_ALL_BOOT_OPTIONS_FAILED;
	        	Status = BootFlowManageExit();
	            if ( EFI_NOT_STARTED == Status )
				{
                    CheckForKeyHook( (EFI_EVENT)NULL, NULL );
                    if (TRUE == gEnterSetup)                          //If setup key detects then launch the setup
                    {
                        goto _ShowMainMenu;
                    }
					// Infinite loop is true so we start again
					i = -1;											//Changed 0 to -1. Then only in next iteration it will be zero otherwise it will be 1.
				}													//If it is i = 0 then it will miss first boot option
	        }
	    }
#endif
//<Ouyyx-20190831 To support UEFI PXE Retry boot ->
	    if(IsRecoverySupported())
	    {
            //OSRecovery is performed if first time boot failed or gOsRecoverySupported flag is true
            if( (!SecondBootFlag) && ((!BootSuccess) || gOsRecoverySupported) )
            {
                gOsRecoverySupported = FALSE;
                Status = pRS->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Attributes, &DataSize, (VOID *)&OsIndications);
                if (!EFI_ERROR (Status))
                {
                   OsIndications = OsIndications & (~EFI_OS_INDICATIONS_START_OS_RECOVERY);
                   Status = pRS->SetVariable (L"OsIndications", &gEfiGlobalVariableGuid, Attributes, DataSize, (VOID *)&OsIndications);
                   
                   pOSRecoveryOrder  = (UINT16 *)VarGetNvramName( L"OsRecoveryOrder", &gEfiGlobalVariableGuid, NULL, &RecoverySize );
                   if(pOSRecoveryOrder != NULL && RecoverySize){
                    Status= ProcessRecovery(pOSRecoveryOrder ,RecoverySize,TRUE);
                        if (!EFI_ERROR (Status))
                        BootSuccess=TRUE;
                   }
                   
                   //<Lvshh001-20221108 For MemoryLeak Fix,otherwise when the EfiBootServicesData increases too much, the hang60 will occur +>
                   if(pOSRecoveryOrder) {
                      MemFreePointer((void **) &pOSRecoveryOrder);//For MemoryLeak Fix
                   } 
                   //<Lvshh001-20221108 For MemoryLeak Fix,otherwise when the EfiBootServicesData increases too much, the hang60 will occur ->
                     
                }	
            }
            //It will again try normal boot for second time when OS Recovery failed.
            if( (!SecondBootFlag) && (!BootSuccess) ){
                SecondBootFlag = TRUE;
                goto _ProcessBootOptions;
            }
            //Platform Recovery is performed when boot order failed for second time also or OSRecovery failed
            if((!BootSuccess) || gPlatformRecoverySupported)
            {
                gPlatformRecoverySupported = FALSE;
                SecondBootFlag = FALSE;
                Status = pRS->GetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Attributes, &DataSize, (VOID *)&OsIndications);
                if (!EFI_ERROR (Status))
                {
                   OsIndications = OsIndications & (~EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY);
                   Status = pRS->SetVariable (L"OsIndications", &gEfiGlobalVariableGuid, Attributes, DataSize, (VOID *)&OsIndications);
                   
                   pRecoveryOrder  = (UINT16 *)VarGetNvramName( L"PlatformRecoveryOrder", &gEfiGlobalVariableGuid, NULL, &RecoverySize );
                   if(pRecoveryOrder != NULL && RecoverySize)
                   {
                     Status= ProcessRecovery(pRecoveryOrder ,RecoverySize,FALSE);
                        if (!EFI_ERROR (Status))
                            BootSuccess=TRUE;    
                   }
                   
                   //<Lvshh001-20221108 For MemoryLeak Fix,otherwise when the EfiBootServicesData increases too much, the hang60 will occur +>
                   if(pRecoveryOrder) {
                      MemFreePointer((void **) &pRecoveryOrder);//For MemoryLeak Fix
                   }  
                   //<Lvshh001-20221108 For MemoryLeak Fix,otherwise when the EfiBootServicesData increases too much, the hang60 will occur ->
                   
                }	
            }
	    }
	}
#ifndef STANDALONE_APPLICATION
    gEnterSetup = TRUE;
    goto _ShowMainMenu;
#else
	return Status;
#endif

}

/**
    This function is the protocol to display messages in
    the post screen.

    @param message : Unicode string to be displayed.

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message )
{
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	return PrintPostMessage(message, TRUE);
}

/**
    This function is the protocol to display messages with
    attributes in the post screen.

    @param message : Unicode string to be displayed.
    @param Attribute : Attribute for the message

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerDisplayPostMessageEx( CHAR16 *message, UINTN Attribute )
{
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	switch(Attribute)
    {
        case PM_EX_DONT_ADVANCE_TO_NEXT_LINE:
            return PrintPostMessage(message, FALSE);
    }

    return EFI_UNSUPPORTED;
}

/**
    This function is the protocol to switch to post screen

    @param VOID

    @retval Return Status based on errors that occurred in library
        functions.

**/
EFI_STATUS PostManagerSwitchToPostScreen( VOID )
{
	EFI_GUID tSwitchToPoscreenGuid = AMITSE_SWITCHING_TO_POST_SCREEN_GUID;

#ifdef STANDALONE_APPLICATION
	return EFI_UNSUPPORTED;
#else
	if ( ! gPostMsgProtocolActive )
		return EFI_UNSUPPORTED;

	//Signalling event while using SwitchingToPostScreen interface
	EfiLibNamedEventSignal (&tSwitchToPoscreenGuid);
	
    if ( gQuietBoot )
	{
		UpdateGoPUgaDraw();
#if SETUP_USE_GRAPHICS_OUTPUT_PROTOCOL
      if (!gGOP)
#else
      if (!gUgaDraw)
#endif
		 return EFI_UNSUPPORTED;
	
		gQuietBoot = FALSE;
		CleanUpLogo();
		//If Post message printed in the Quiteboot by using Set cur position
		// It is better to flash all the screen.
		//FlushLines( 0, gMaxRows - 1 );
		//DoRealFlushLines(); 
		InitPostScreen();
      return EFI_SUCCESS;
	}

    return EFI_UNSUPPORTED;
#endif
}
/**
    Initializes the PostManagerProgressBar

    @param x Column or Left screen position of ProgressBar in pixels
    @param y Row or Top screen position of ProgressBar in pixels
    @param w Width of ProgressBar in pixels
    @param h Height of ProgressBar in pixels
    @param iterate Total number of iterations or Calls
        to advanced the progressbar to 100% of the given width.
        The delta or changed value will be computed.								 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerInitProgressBar(
	UINTN x,
	UINTN y,
	UINTN w,
	UINTN h,
	UINTN iterate)
{
	EFI_STATUS Status = 0;
	EFI_UGA_PIXEL	*BGColor;
	EFI_UGA_PIXEL	*BDRColor;
	EFI_UGA_PIXEL	*FillColor;

	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->InitProgressBar (
					x,
					y,
					w,
					h,
					iterate);
	}
	else
	{
		BGColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		BDRColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		FillColor = EfiLibAllocateZeroPool(sizeof (EFI_UGA_PIXEL));
		
		GetProgressColor(BGColor,BDRColor,FillColor);
		
		gProgress->delta = w/iterate;
		gProgress->w = gProgress->delta*iterate;
		gProgress->h = h;
		gProgress->x = x;
		gProgress->y = y;
		gProgress->backgroundColor = BGColor;
		gProgress->borderColor = BDRColor;
		gProgress->fillColor = FillColor;
		gProgress->quiteBootActive = FALSE ;
		gProgress->active = TRUE ;         // progressbar has been initialized
	}
	return Status;
}

/**
    Increments the PostManagerProgressBar

    @param VOID
				 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerSetProgressBarPosition()
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->SetProgressBarPosition ();
	}
	else
	{
		if(gProgress->active)
			DrawBltProgressBar();
		else
			Status = EFI_UNSUPPORTED;
	}
	return Status;
}

/**
    Wrapper function for SetCurPos

    @param UINTN X - Value of the column 
        UINTN Y - Number of row below the last written line

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/

EFI_STATUS PostManagerSetCursorPosition(UINTN X, UINTN Y)
{
	return SetCurPos(X,Y);
}

/**
    Wrapper function for GetCurPos
    Writes cursor position into given X and Y locations.

    @param pX Pointer to storage for current column value 
    @param pY Pointer to storage for current row value

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/

EFI_STATUS PostManagerGetCursorPosition(UINTN *pX, UINTN *pY)
{
	return GetCurPos(pX, pY);
}

/**
 

    @param MsgBoxTitle 
    @param Message 
    @param MsgBoxType 

    @retval EFI_STATUS status and UINT8 MsgBoxSel

**/
VOID SaveGraphicsScreen(VOID);
VOID RestoreGraphicsScreen(VOID);
EFI_STATUS
PostManagerDisplayMsgBox (
    IN CHAR16  *MsgBoxTitle,
    IN CHAR16  *Message,
    IN UINT8   MsgBoxType,
    OUT UINT8  *MsgBoxSel
	)
{

	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	EFI_TPL OldTpl;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	Status =	InitEsaTseInterfaces ();
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (TPL_APPLICATION);
	}
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->DisplayMsgBox (
			    MsgBoxTitle,
			    Message,
			    MsgBoxType,
			    MsgBoxSel
			    );
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		
		if( gPostStatus <= TSE_POST_STATUS_ENTERING_TSE )
			SaveGraphicsScreen();

		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);
		if (EFI_ERROR(Status))
			return Status;

		Status = ShowPostMsgBox(MsgBoxTitle, Message, MsgBoxType,MsgBoxSel);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);

		if( gPostStatus <= TSE_POST_STATUS_ENTERING_TSE )
			RestoreGraphicsScreen();
	}
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (OldTpl);
	}
	return Status;
}

/**
    Function for the quit booting display.

    @param Message INTN CoOrdX, INTN CoOrdY,
    @param Attribute EFI_UGA_PIXEL Foreground,
    @param Background 

    @retval Status

**/
EFI_STATUS
PostManagerDisplayQuietBootMessage(
    CHAR16 *Message,
    INTN CoOrdX,
    INTN CoOrdY,
    CO_ORD_ATTRIBUTE Attribute,
    EFI_UGA_PIXEL Foreground,
    EFI_UGA_PIXEL Background
    )
{
#ifdef STANDALONE_APPLICATION
	return EFI_UNSUPPORTED;
#elif defined TSE_FOR_APTIO_4_50
    UINTN BltGlyphWidth=0;
    EFI_UGA_PIXEL *BltBuffer=NULL;
    UINTN BltIndex;
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN SizeOfX, SizeOfY;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

    UpdateGoPUgaDraw();
    if(EFI_SUCCESS != HiiInitializeProtocol())
        return EFI_UNSUPPORTED;

    if(
        (EFI_SUCCESS != GetScreenResolution(&SizeOfX, &SizeOfY))||
        (!gQuietBoot)
        )
        return EFI_UNSUPPORTED;

    if ((Message != NULL) && (EfiStrLen(Message) == 0)) 
		return EFI_SUCCESS;
    BltIndex = 0;

	Status = HiiString2BltBuffer(Message,Foreground,Background,&BltIndex,&BltBuffer,&BltGlyphWidth);
	gDrawQuietBootMessage = TRUE;
    if(GifImageFlag) {
    	gGifSrcX = 0; 
    	gGifSrcY = 0;
    }
	if(Status == EFI_SUCCESS)
	    DrawBltBuffer(BltBuffer, Attribute, BltIndex, HiiGetGlyphHeight(), CoOrdX, CoOrdY, BltGlyphWidth);
	gDrawQuietBootMessage = FALSE;
    //Free BltBuffer
    MemFreePointer((VOID **)&BltBuffer);
    if ( IsTSEGopNotificationSupport() )
    	SaveGraphicsScreen();
    
    return Status;
#else
// For EDK Nt32 support.
//    Status = DrawHiiStringBltBuffer(Message, CoOrdX, CoOrdY, Attribute, Foreground, Background);
	return EFI_UNSUPPORTED;
#endif
}

/**
    Function returns current Post status.

    @param VOID

        One of the TSE_POST_STATUS
    @retval TSE_POST_STATUS_BEFORE_POST_SCREEN TSE Loaded and yet goto post screen
    @retval TSE_POST_STATUS_IN_POST_SCREEN In the post screen
    @retval TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN In the quite boot screen
    @retval TSE_POST_STATUS_IN_BOOT_TIME_OUT Witing for Boot timeout
    @retval TSE_POST_STATUS_ENTERING_TSE Entering TSE
    @retval TSE_POST_STATUS_IN_TSE Inside TSE
    @retval TSE_POST_STATUS_IN_BBS_POPUP Inside BBS Poupup
    @retval TSE_POST_STATUS_PROCEED_TO_BOOT Outside TSE and Booting or in Shell

**/
TSE_POST_STATUS	PostManagerGetPostStatus(VOID)
{
//	Status =	InitEsaTseInterfaces ();
	if ((TRUE == gLaunchOtherSetup) && (NULL != gEsaInterfaceForTSE))
	{
		return gEsaInterfaceForTSE->GetPostStatus ();
	}
	else
	{
		return gPostStatus;
	}
}


/**
    Function to display the Information box

    @param VOID

        One of the TSE_POST_STATUS
    @retval TSE_POST_STATUS_BEFORE_POST_SCREEN TSE Loaded and yet goto post screen
    @retval TSE_POST_STATUS_IN_POST_SCREEN In the post screen
    @retval TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN In the quite boot screen
    @retval TSE_POST_STATUS_IN_BOOT_TIME_OUT Witing for Boot timeout
    @retval TSE_POST_STATUS_ENTERING_TSE Entering TSE
    @retval TSE_POST_STATUS_IN_TSE Inside TSE
    @retval TSE_POST_STATUS_IN_BBS_POPUP Inside BBS Poupup
    @retval TSE_POST_STATUS_PROCEED_TO_BOOT Outside TSE and Booting or in Shell

**/
EFI_STATUS
PostManagerDisplayInfoBox(	IN CHAR16		*InfoBoxTitle,
							IN CHAR16		*InfoString,
							IN UINTN		Timeout,
							OUT EFI_EVENT	*Event
						 )
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		return gEsaInterfaceForTSE->DisplayInfoBox (
				InfoBoxTitle,
				InfoString,
				Timeout,
				Event
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;

		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			return Status;

		Status = ShowInfoBox(InfoBoxTitle, InfoString, Timeout, Event);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);

		return Status;
	}
}


/**
    Function to set color

    @param Attrib Text color 
			 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/
EFI_STATUS PostManagerSetAttribute(UINT8 Attrib)
{
	gPostMgrAttribute = Attrib ;
	return EFI_SUCCESS ;
}

/**
    Function to Get color

    @param Attrib Pointer to get Text color 
			 

    @retval EFI_STATUS status - If the function runs correctly, returns
        EFI_SUCCESS, else other EFI defined error values.

**/
EFI_STATUS PostManagerGetAttribute(UINT8 *Attrib)
{
	*Attrib = gPostMgrAttribute ;
	return EFI_SUCCESS ;
}

/**
    Wrapper function for Display OEM Message box. 
               

    @param MsgBoxTitle Caption of the Message Box
    @param Message String to be displayed by the Message Box
    @param Legend Legend String to be displayed by the Message Box
    @param MsgBoxType Message Box type

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayMsgBoxEx (
    IN CHAR16			*Title,
    IN CHAR16			*Message,
    IN CHAR16			*Legend,
    IN MSGBOX_EX_CATAGORY	MsgBoxExCatagory,
    IN UINT8	 		MsgBoxType,
    IN UINT16			*OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
    IN UINT16	 		OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
    IN AMI_POST_MGR_KEY		*HotKeyList, 		// NULL - AnyKeyPress closes
    IN UINT16 			HotKeyListCount, 
    OUT UINT8			*MsgBoxSel,
    OUT AMI_POST_MGR_KEY	*OutKey
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		return gEsaInterfaceForTSE->DisplayMsgBoxEx (
			    Title,
			    Message,
			    Legend,
			    MsgBoxExCatagory,
			    MsgBoxType,
			    OptionPtrTokens,	// Valid only with MSGBOX_TYPE_CUSTOM 
			    OptionCount,		// Valid only with MSGBOX_TYPE_CUSTOM
			    HotKeyList, 		// NULL - AnyKeyPress closes
			    HotKeyListCount, 
			    MsgBoxSel,
			    OutKey
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			return Status;

		Status = ShowPostMsgBoxEx (Title, Message, Legend, MsgBoxExCatagory,
											MsgBoxType, OptionPtrTokens, OptionCount,
											HotKeyList, HotKeyListCount, MsgBoxSel, OutKey
											);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);

		return Status;
	}

}
/**
    Wrapper function for Display OEM Message box. 
               

    @param Percent Percent Complete

    @retval EFI_STATUS status - EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayProgress (
    IN UINT8	ProgressBoxState, 
    IN CHAR16	*Title,
    IN CHAR16	*Message,
    IN CHAR16	*Legend,
    IN UINTN 	Percent,	// 0 - 100
    IN OUT VOID	**Handle,	//Out HANDLE. Valid Handle for update and close
    OUT AMI_POST_MGR_KEY	*OutKey	//Out Key 
)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		Status = gEsaInterfaceForTSE->DisplayProgress (
			    ProgressBoxState, 
			    Title,
			    Message,
			    Legend,
			    Percent,
			    Handle,
			    OutKey
				);
	}
	else
	{
		Status =  ShowPostProgress( ProgressBoxState, Title, Message, Legend, Percent, Handle, OutKey) ;		
	}
	return Status ;
}
/**
    Wrapper function for Display Menu. 
               

    @param HiiHandle - Handle that contains String
    @param TitleToken - Menu Title Token
    @param LegendToken - Menu Legend Token  
    @param MenuData - Menu Items to be displayed 
    @param MenuCount - Number of Menu items
    @param pSelection Menu selection (DEFAULT)

    @retval EFI_STATUS status 		- EFI_SUCCESS if OK,
        EFI_INVALID_PARAMETER if NULL pointer

**/
EFI_STATUS PostManagerDisplayMenu (
    IN VOID	*HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN UINT16 	LegendToken,	  
    IN POSTMENU_TEMPLATE *MenuData,
    IN UINT16 	MenuCount,
    OUT UINT16  *pSelection
)
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINTN SizeOfX;
	UINTN SizeOfY; 
	EFI_UGA_PIXEL *UgaBlt = NULL;

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	if(gPostStatus < TSE_POST_STATUS_IN_BOOT_TIME_OUT)
		return EFI_NOT_READY;
	
	Status =	InitEsaTseInterfaces ();
	
	if (!EFI_ERROR (Status))
	{
		//Set the tse data to esa
		/*Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (TSEHANDLE, 1, &gHiiHandle, 0);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (BOOTDATA,1, &gBootData, &gBootOptionCount);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (LANGDATA,1, &gLanguages, &gLangCount);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (GOP, 1, &gGOP, 0);
		Status = gEsaInterfaceForTSE->EsaTseSetGetVariables (PostStatus, 1, 0, &gPostStatus);*/
//		gEsaInterfaceForTSE->InitGraphicsLibEntry (gImageHandle, gST);
		
		return gEsaInterfaceForTSE->DisplayPostMenu (
					HiiHandle, 	
					TitleToken, 
					LegendToken,	  
					MenuData,
					MenuCount,
					pSelection
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		 if (TSE_POST_STATUS_IN_POST_SCREEN == gPostStatus||TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
		 {
			 UgaBlt = SavePostScreen(&SizeOfX, &SizeOfY);
		 }
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			return Status;
		
		Status = ShowPostMenu(HiiHandle, TitleToken, LegendToken, MenuData, MenuCount, pSelection);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);
		 if (TSE_POST_STATUS_IN_POST_SCREEN == gPostStatus||TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN == gPostStatus)
		 {
			 RestorePostScreen(UgaBlt,SizeOfX,SizeOfY);
		 }

		return Status;

	}
}

/**
    Function to display text entry interface
 

    @param HiiHandle 
    @param TitleToken 
    @param InputData 
    @param ItemCount 
    @param ValidateKeyFunc 

    @retval EFI_STATUS

**/
EFI_STATUS
PostManagerDisplayTextBox (
    IN VOID	    *HiiHandle, 	
    IN UINT16 	TitleToken, 
    IN TEXT_INPUT_TEMPLATE *InputData,
    IN UINT16 	ItemCount,
    IN DISPLAY_TEXT_KEY_VALIDATE ValidateKeyFunc
	)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	EFI_TPL OldTpl;
	
	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	Status =	InitEsaTseInterfaces ();
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (TPL_APPLICATION);
	}
	
	if (!EFI_ERROR (Status))
	{
		return gEsaInterfaceForTSE->DisplayTextBox (
			    HiiHandle, 	
			    TitleToken, 
			    InputData,
			    ItemCount,
			    ValidateKeyFunc
				);
	}
	else
	{
		UINTN currenttextModeCols = 0, currenttextModeRows = 0;
		UINT32 currentGOPMode = 0;
		
		Status = SaveCurrentTextGOP (&currenttextModeCols, &currenttextModeRows, &currentGOPMode);

		if ( EFI_ERROR(Status) )
			return Status;

		Status = ShowPostTextBox(HiiHandle, TitleToken, InputData, ItemCount, ValidateKeyFunc);

		RestoreTextGOPMode (currenttextModeCols, currenttextModeRows, currentGOPMode);

		return Status;

	}
	
	if (gPostStatus == TSE_POST_STATUS_IN_POST_SCREEN || gPostStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN) {
		gBS->RaiseTPL (TPL_HIGH_LEVEL);
		gBS->RestoreTPL (OldTpl);
	}
}
/**
    Function to Stop the Click Event
               
    @param VOID

    @retval VOID

**/
VOID StopClickEvent(VOID)
{
	if(gClickTimer)
	{
		gBS->SetTimer ( gClickTimer,TimerCancel,0);
		TimerStopTimer( &gClickTimer );
	}
}


/**
    Protocol interface to invalidate BGRT status
 

    @param VOID
 

    @retval EFI_STATUS

**/
VOID InvalidateStatusInBgrtWrapper (VOID);
EFI_STATUS InvalidateBgrtStatusByProtocol (VOID)
{
	InvalidateStatusInBgrtWrapper();
	return EFI_SUCCESS;
}

/**
    Install Invalidate BGRT status protocol

    @param Handle 

    @retval EFI_STATUS
**/
EFI_STATUS InstallInvalBGRTStatusProtocol (EFI_HANDLE Handle)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gTSEInvalidateBgrtStatusProtocolGuid,
                  &gInvalidateBgrtStatus,
                  NULL
                  );

  return Status;
}

/**
        Name				:	SaveCurrentTextGOP

        Description		:	Function to Save current TextMode and GOP
               
        Input				:  UINTN currentTextModCols, UINTN currentTextModRows, UINT32 currentGOPMode

        Output			:	EFI_STATUS

**/
EFI_STATUS SaveCurrentTextGOP ( UINTN *currenttextModeCols, UINTN *currenttextModeRows, UINT32 *currentGOPMode )
{

	EFI_STATUS 	Status = EFI_SUCCESS;
	
	if ( IsTseBestTextGOPModeSupported() && TSE_POST_STATUS_IN_TSE != gPostStatus )
	{
		if (gGOP)
		{
			*currentGOPMode = gGOP->Mode->Mode;
		}

		if (TSE_BEST_HORIZONTAL_RESOLUTION != gGOP->Mode->Info->HorizontalResolution || TSE_BEST_VERTICAL_RESOLUTION != gGOP->Mode->Info->VerticalResolution)
		{
			SetScreenResolution(TSE_BEST_HORIZONTAL_RESOLUTION,TSE_BEST_VERTICAL_RESOLUTION); //If any postinterface is invoked during QuietBoot
		}

		Status = gST->ConOut->QueryMode( gST->ConOut, gST->ConOut->Mode->Mode, currenttextModeCols, currenttextModeRows);
		if (EFI_ERROR(Status))
			return EFI_NOT_FOUND;

		if ((STYLE_FULL_MAX_COLS != *currenttextModeCols)  || (STYLE_FULL_MAX_ROWS != *currenttextModeRows))
		{
			gMaxRows = STYLE_FULL_MAX_ROWS;
			gMaxCols = STYLE_FULL_MAX_COLS;
			SetDesiredTextMode ();
		}
	}
	return Status;
}

/**
        Name				:	RestoreTextGOPMode

        Description		:	Function to Save current TextMode and GOP
               
        Input				:  UINTN currentTextModCols, UINTN currentTextModRows, UINT32 currentGOPMode

        Output			:	EFI_STATUS

**/
EFI_STATUS RestoreTextGOPMode ( UINTN prevTextModeCols, UINTN prevTextModeRows, UINT32 prevGOPMode )
{
	EFI_STATUS 	Status = EFI_SUCCESS;

	if ( IsTseBestTextGOPModeSupported() && TSE_POST_STATUS_IN_TSE != gPostStatus )
	{
		if ( (gMaxRows != prevTextModeRows) || (gMaxCols != prevTextModeCols) )
		{
			gMaxRows = prevTextModeRows; 
			gMaxCols = prevTextModeCols;
			SetDesiredTextMode (); //Restoring to previous textmode if any changed
		}

		if (gGOP && prevGOPMode != gGOP->Mode->Mode)
		{
			Status = gGOP->SetMode (gGOP, prevGOPMode);	//In some case changing text mode will change the graphcis mode, so reverting here.
		}
	}
	return Status;
}
/**
        Name				:	ProcessRecovery

        Description		:	Function ProcessRecovery attempt
               
        Input				:  UINT16 *pRecoveryOrder, UINTN size, 
        Input				:  BOOLEAN - TRUE for OsRecovery, FALSE for PlatformRecovery

        Output			:	EFI_STATUS

**/
EFI_STATUS ProcessRecovery(UINT16 *pRecoveryOrder,UINTN size, BOOLEAN OsRecovery)
{

	UINT16  u16RecoveryCount = 0,i;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	u16RecoveryCount = (UINT16)(size/sizeof(UINT16));
	for ( i = 0; i < u16RecoveryCount; i++)
	{
		 BOOT_DATA *pRecoveryData;	    
		 pRecoveryData = RecoveryGetRecoveryData(pRecoveryOrder[i],OsRecovery);
		 if(pRecoveryData == NULL || !(pRecoveryData->Active & LOAD_OPTION_ACTIVE))
	                continue;
		 Status = BootLaunchBootOption(pRecoveryOrder[i], pRecoveryOrder+i, u16RecoveryCount - i);
		 if (!EFI_ERROR (Status))
			 return Status;
	}
	 
	return Status;
}

/**
        Name			:	ScreenMgmtSaveTseScreen

        Description		:	Save the current post screen
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtSaveTseScreen ()
{
	if (EFI_ERROR(InitEsaTseInterfaces()))
	{
		gScreenBuffer = (EFI_UGA_PIXEL *)SavePostScreen(&gScreenWidth, &gScreenHeight);	//Saving buffer of Quiteboot logo
		
		gScreenBuf = EfiLibAllocatePool( sizeof(SCREEN_BUFFER) );

		if (gScreenBuf && gActiveBuffer)
		{
			MemCopy( gScreenBuf, gActiveBuffer, sizeof(SCREEN_BUFFER) ); //Saving buffer of PostMessage, which will used to show message normal boot\quite boot
		}
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

/**
        Name			:	ScreenMgmtRestoreTseScreen

        Description		:	Restore the saved post screen.
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtRestoreTseScreen ()
{

	if (EFI_ERROR(InitEsaTseInterfaces())) 
	{
		if (gActiveBuffer && gScreenBuf)
		{
			MemSet(gActiveBuffer, sizeof(SCREEN_BUFFER), 0);
			MemCopy( gActiveBuffer, gScreenBuf, sizeof(SCREEN_BUFFER) );	//Restoring Post message in active buffer. This will be drawn when DoRealFlushLines happens
		}
		RestorePostScreen(gScreenBuffer,gScreenWidth,gScreenHeight); //Restoring Quiteboot logo

		if (gScreenBuf)
			MemFreePointer((VOID **)&gScreenBuf);
			
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

/**
        Name			:	ScreenMgmtClearScreen

        Description		:	Cleans the current screen
               
        Output			:	EFI_STATUS

**/
EFI_STATUS ScreenMgmtClearScreen (
		IN UINT8 Attribute
		)
{
	if (EFI_ERROR(InitEsaTseInterfaces())) 
	{
		ClearGrphxScreen ();//To clear screen while drawing logo at (0,0) co-ordinates
		ClearScreen( Attribute );
		DoRealFlushLines();
		return EFI_SUCCESS;
	}
	else
	{
		return EFI_UNSUPPORTED;
	}
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**         (C)Copyright 2015, American Megatrends, Inc.             **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateCoreIntf.c
//
// Description:	Contains AmiFwUpdate Core interfaces
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************

#include "Token.h"
#include "AmiDxeLib.h"
#include "AmiFwUpdateBmcStrDefs.h"
#include "AmiFwUpdateBmcUIIntf.h"
#include "AmiFwUpdateBmcCoreIntf.h"
#include "AmiFwUpdateBmc.h"
#include "Library/MemoryAllocationLib.h"
#include "Library/BaseMemoryLib.h"
#include "FileSystemInfo.h"
#include "string.h"
#include "Protocol/LoadedImage.h"
#include "Protocol/AMIPostMgr.h"
//<kuaibai-20190320-Auto control BMC virtual usb status +>
#include <Protocol/IpmiTransportProtocol.h>
#define VIRTUAL_USB_DEVICES_ENABLED  0x00
#define VIRTUAL_USB_DEVICES_DISABLED 0x01
#define COMMAND_SUCCESS              0x01
//<kuaibai-20190320-Auto control BMC virtual usb status ->
extern BOOLEAN gBcpPreserveFlag;
extern BOOLEAN gAFU_Protocol_Found;

extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
extern EFI_SYSTEM_TABLE *gST;

extern VOID* gHiiHandle;

TARGET_FSINFO *gUITargetDIskInfo;
UINT32 gUIDiskCount;
BOOLEAN		gUsePostMgrService = TRUE;
extern AMI_POST_MANAGER_PROTOCOL 	*gPostMgr;
EFI_STATUS WaitingForAbortFwUpdate();
EFI_STATUS SendPatchReq ();

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_InterfaceInit
//
// Description: Entry Point of AMI firmware update application
//
// Input:       EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE *SystemTable
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FWUP_UI_InterfaceInit(EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    InitAmiLib(ImageHandle, SystemTable);
    AMI_FS_GetFileSystemsInformation(&gUITargetDIskInfo, (UINT32*)&gUIDiskCount) ;

    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:           DisplayMessage
//
// Description:         Displays a message
//
// Parameter:          IN CHAR16 * ErrorMsgStr
//
// Return Value:       VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID DisplayMessage(CHAR16 *ErrorMsgStr , UINT32 ErrorMsgID , BOOLEAN ValueType)
{
    void *Message = (void*)NULL;
    if(TRUE == ValueType)
        Message = (void*)&ErrorMsgID;
    else
        Message = (CHAR16*)ErrorMsgStr;
    
    ShowMessageInTextMode(Message, ValueType, TRUE);
    
    return ;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_ShowError
//
// Description: UI Wrapper to show error message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWUP_UI_ShowError(UINT32 StrID, CHAR16 *ErrMsg)
{
    if(StrID)
        DisplayMessage((CHAR16 *)NULL , StrID , TRUE);
    else
        DisplayMessage(ErrMsg , 0 , FALSE);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_ShowStatus
//
// Description: UI Wrapper to show status message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWUP_UI_ShowStatus(UINT32 StrID, CHAR16 *StatusMsg)
{
    // Call UI message progress handler
	if(StrID)
	    ShowMessageInTextMode((CHAR16 *)&StrID, TRUE, FALSE);
	else
	    ShowMessageInTextMode(StatusMsg, FALSE, FALSE);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_ShowEndStatus
//
// Description: UI Wrapper to show end status message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWUP_UI_ShowEndStatus(UINT32 StrID, CHAR16 *StatusMsg)
{
    //ShowMessageInTextMode((CHAR16 *)L"", FALSE, TRUE);
    // Call UI message progress handler
    if(StrID)
    {
        ShowMessageInTextMode((CHAR16 *)&StrID, TRUE, TRUE);
    }
    else
    {
        ShowMessageInTextMode(StatusMsg, FALSE, TRUE);
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_ShowConfirmation
//
// Description: UI Wrapper to Show Confirmation and return TRUE if user accepts
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      TRUE - User accepts
//              FALSE - User rejects
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN FWUP_UI_ShowConfirmation(UINT32 StrID, CHAR16 *QuestionStr)
{
    if(StrID)
        DisplayMessage((CHAR16 *)NULL, StrID , TRUE);
    else
        DisplayMessage(QuestionStr, 0, FALSE);

    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWCore_ReportError
//
// Description: UI Wrapper to show error message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWCore_ReportError(UINT32 StrID, CHAR16 *ErrMsg)
{	
	FWUP_UI_ShowError(StrID, ErrMsg);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWCore_ReportStatus
//
// Description: UI Wrapper to show status message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWCore_ReportStatus(UINT32 StrID, CHAR16 *StatusMsg)
{
	FWUP_UI_ShowStatus(StrID, StatusMsg);  
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWCore_ReportEndStatus
//
// Description: UI Wrapper to show End status message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FWCore_ReportEndStatus(UINT32 StrID, CHAR16 *StatusMsg)
{
    FWUP_UI_ShowEndStatus(StrID, StatusMsg);  
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWUP_UI_ShowConfirmation
//
// Description: UI Wrapper to show status message
//
// Input:       UINT32 StrID, CHAR16 *ErrMsg
//
// Output:      BOOLEAN ( Accepted or rejected )
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN FWCore_ReportConfirmation(UINT32 StrID, CHAR16 *StatusMsg)
{
	return FWUP_UI_ShowConfirmation(StrID, StatusMsg); 	
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   IsSecureUpdateEnabled
//
// Description  :   To check weather secured mode is enabled or not
//
// Parameter    :   void
//
// Return Value  :  BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN IsSecureUpdateEnabled()
{
	#if SECURE_UPDATE_SUPPORTED
		return TRUE;
	#endif
		return FALSE;
}

extern CHAR16 *FwUHiiGetStringLanguage( VOID* Handle, UINT16 StringId , CHAR16* Lang);
CHAR16 *FW_HiiGetString( VOID* handle, UINT16 token )
{
	if(INVALID_TOKEN != token)
		return FwUHiiGetStringLanguage( handle, token, (CHAR16*)NULL );
	else
		return (CHAR16*)NULL;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   UI_GetBCPCheckBoxValues
//
// Description  :   Decide BCP flash option
//
// Parameter    :   void
//
// Return Value  :  UINTN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN UI_GetBCPCheckBoxValues(void)
{
    return (UINTN)gBcpPreserveFlag;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   Provide_BCPSupport
//
// Description  :   Decide BCP supported or not
//
// Parameter    :   void
//
// Return Value  :  UINTN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN Provide_BCPSupport(void)
{
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
    {
        gBcpPreserveFlag = TRUE;
        return 1 ;
    }
#else
    {
        gBcpPreserveFlag = FALSE;
        return 0 ;
    }
#endif
}

//<kuaibai-20190320-Auto control BMC virtual usb status +>
#if VIRTUAL_BMC_USB_INTERFACE
VOID
EFIAPI
DisableUsbInterface ()
{
  EFI_STATUS    Status;
  UINTN         VarSize = 0;
  UINT16        VarData = 0;
  UINT8         ResponseData = 0;
  UINT8         ResponseDataSize = 0;
  EFI_IPMI_TRANSPORT *tIpmiTransport = NULL;
  UINT8         UsbEnableCommandData = 0;    // 0- Enabling BMC USB Interface
  UINT8         UsbDisableCommandData = 1;   // 1- Disabling BMC USB Interface

  Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, &tIpmiTransport);
  DEBUG((DEBUG_INFO, "[SK] In app: LocateProtocol tIpmiTransport Status: %r\n", Status));
  
  if (!EFI_ERROR(Status))
  {
    DEBUG((DEBUG_INFO, "[SK] In app: go to disable virtual USB device!\n"));
    ResponseDataSize = sizeof(ResponseData);
    Status = tIpmiTransport->SendIpmiCommand (
                               tIpmiTransport,
                               0x3a,
                               0,
                               0xAA,
                               &UsbDisableCommandData,
                               1,
                               &ResponseData,
                               &ResponseDataSize
                               );
    DEBUG((DEBUG_INFO, "[SK] In app: Disable Status: %r and ResponseData: %x \n", Status, ResponseData));
    if (!EFI_ERROR(Status)) {
      if (ResponseData == COMMAND_SUCCESS)
        DEBUG((DEBUG_INFO, "[SK] In app: Virtual USB devices are Disabled Using Send Ipmi Command\n"));
      else
        DEBUG((DEBUG_ERROR, "[SK] In app: Virtual USB devices Disabling FAILED Using Send Ipmi Command\n"));
    }
  }
}
#endif
//<kuaibai-20190320-Auto control BMC virtual usb status ->

EFI_STATUS
_AMIFWUpdateFlash_TextMode()
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT16 LoadingImagMsgID = STR_LOADING_IMG_MSG;
    UINT16 RestartMsgID = STR_RESTART_SYSTEM_MSG;
    DEBUG((EFI_D_ERROR, "[SK] gAFU_Protocol_Found = %x.\n", gAFU_Protocol_Found)); //Spiral
    if(gAFU_Protocol_Found)
    {
    	ShowMessageInTextMode(&LoadingImagMsgID, TRUE, TRUE);
    	DEBUG((EFI_D_ERROR, "[SK] ShowMessageInTextMode.\n")); //Spiral
        //Start BIOS flshing
        Status = FWCore_StartBIOSUpdate();
        DEBUG((EFI_D_ERROR, "[SK] ShowMessageInTextMode = %r.\n",Status)); //Spiral
        if(EFI_SUCCESS == Status)
        {
        	ShowMessageInTextMode(&RestartMsgID, TRUE, TRUE);
        	//Spiral - SendPatchReq();
        	Status = SendPatchReq();
        	DEBUG((EFI_D_ERROR, "[SK] SendPatchReq = %r.\n",Status)); 
#if VIRTUAL_BMC_USB_INTERFACE
//<kuaibai-20190320-Auto control BMC virtual usb status> DisableUsbInterface();
#endif
            gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
    }
    else
        FWCore_ReportError(STR_AFUPROTO_NOT_FOUND, (CHAR16*)NULL);
  
    return Status;
}


UINTN GetHandleIndexFromVol(CHAR16 *VolName)
{
    UINTN Index = 0;
    UINTN CurIndex = 0;
    
    if(NULL != gUITargetDIskInfo)
    {
        if(VolName)
        {
            for(Index =  0;Index <gUIDiskCount;Index++)
            {
                if( (!StrCmp(gUITargetDIskInfo[Index].VolumeName,VolName)) || (!StrCmp(gUITargetDIskInfo[Index].FriendlyName,VolName)) )
                {
                    CurIndex = Index;
                    break;
                }       
            }
        }
    }
    return CurIndex;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   IsTextMode
//
// Description  :   Check Text/GUI mode 
//
// Parameter    :   
//
// Return Value  :  BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
/*BOOLEAN IsTextMode()
{
    return gTextmode;
}*/

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   PerformTextModeOperation
//
// Description  :   Check and perform CommandLine(Text) mode 
//
// Parameter    :   EFI_HANDLE ImageHandle
//
// Return Value  :  BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN PerformTextModeOperation(EFI_HANDLE ImageHandle)
{
    BOOLEAN CommandLineStatus = FALSE;

    if(TRUE == OperateCommmandLineArg(ImageHandle))
    {
        DEBUG((EFI_D_ERROR, "[SK] TRUE == OperateCommmandLineArg(ImageHandle).\n")); //Spiral
        _AMIFWUpdateFlash_TextMode();
        CommandLineStatus = TRUE;
    }   
    DEBUG((EFI_D_ERROR, "[SK] CommandLineStatus = %x.\n", CommandLineStatus)); //Spiral
    return CommandLineStatus;
}


/**
    Duplicates a String from CHAR8 to CHAR16

    @param string 

    @retval CHAR16 *
              

**/
CHAR16 *FwStrDup8to16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;
    text = (CHAR16 *)AllocateZeroPool ( (1 + AsciiStrLen ( string )) * sizeof(CHAR16));
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
    }
    return text;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   OperateCommmandLineArg
//
// Description  :   Parse Command line Arguments 
//
// Parameter    :   EFI_HANDLE ImageHandle
//
// Return Value  :  BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN OperateCommmandLineArg(EFI_HANDLE ImageHandle)
{
    CHAR8 		*Options8 = (CHAR8 *)NULL;
    CHAR16 		*Options16 = (CHAR16 *)NULL;
    UINT32 		OptionSize=0;
    EFI_STATUS 	Status;
    BOOLEAN 	IsCommandLine = FALSE;
    EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;
	BOOLEAN IsAbort = FALSE;

    Status = gBS->HandleProtocol( ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage );

    if (!EFI_ERROR (Status))
    {
        //Options = FwStrDup8to16 ((CHAR8 *)LoadedImage->LoadOptions);
    	Options8 = AllocateZeroPool (LoadedImage->LoadOptionsSize+2);
        if (NULL != Options8)
        {
			CopyMem (Options8, LoadedImage->LoadOptions, LoadedImage->LoadOptionsSize);
			DEBUG((EFI_D_ERROR, "[SK] Options8 = %s.\n",*Options8)); //Spiral
			Options16 = FwStrDup8to16 (Options8);
			if (NULL != Options16)
			{
			    
				if(NULL != pST->ConIn)
				{
					Status = WaitingForAbortFwUpdate();
			    	if (EFI_ERROR (Status))
			    	{
			    		IsAbort = TRUE;
			    	}
				}
				if(FALSE == IsAbort)
				{
				    DEBUG((EFI_D_ERROR, "[SK] FALSE == IsAbort. Options16 = %s.\n",*Options16)); //Spiral
					if(TRUE == SetGlobalFlagForFlash (Options16, (UINT32)StrLen (Options16)))
					{
						IsCommandLine = TRUE;
						DEBUG((EFI_D_ERROR, "[SK] FIsCommandLine = TRUE;\n")); //Spiral
					}
				}
				FreePool (Options16);
			}
			FreePool (Options8);
        }
    }
    return IsCommandLine;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   ShowMessageInTextMode
//
// Description  :   Show message in Text mode 
//
// Parameter    :   VOID *Message,BOOLEAN IsMsgIntegerType,BOOLEAN IsNewLine
//
// Return Value  :  VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID ShowMessageInTextMode(VOID *Message,BOOLEAN IsMsgIntegerType,BOOLEAN IsNewLine)
{
    CHAR16 *Str = (CHAR16*)NULL;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForeGround = {0xFF,0xFF,0xFF,0};
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackGround = {0,0,0,0};
	static UINTN StartX = 0,StartY = 0;
	TSE_POST_STATUS TseStatus = 0 ;
	CHAR16 *EmptyStr = (CHAR16 *)NULL;
	
    if(Message)
    {
        if(TRUE == IsMsgIntegerType)
            Str = FW_HiiGetString( gHiiHandle,*((UINT16*)Message));
        else
            Str = (CHAR16*)Message;
        if(Str)
        {
			if( (gUsePostMgrService == TRUE) && (gPostMgr != NULL))
			{				
				TseStatus = gPostMgr->GetPostStatus() ;
				if(TseStatus == TSE_POST_STATUS_IN_QUITE_BOOT_SCREEN)
				{
					EmptyStr = FW_HiiGetString(gHiiHandle,STR_EMPTY_STRING_BMC);
					if(EmptyStr)
						gPostMgr->DisplayQuietBootMessage((CHAR16 *)EmptyStr, 0, 0, CA_AttributeLeftBottom, ForeGround, BackGround) ;
					gPostMgr->DisplayQuietBootMessage((CHAR16 *)Str, 0, 0, CA_AttributeLeftBottom, ForeGround, BackGround) ;
										
				}
				else
				{
					if(FALSE ==  IsNewLine)	
					{

						if( (StartX != 0) || (StartY != 0) )
							gPostMgr->SetCurPos(StartX,StartY);			
						gPostMgr->GetCurPos(&StartX,&StartY);
					}
					else
					{
						StartX = 0;
						StartY = 0;
					}

					gPostMgr->DisplayPostMessage(Str);
				}
			}
			else				
			{				
				gST->ConOut->OutputString (gST->ConOut, (CHAR16*)Str);
				if(TRUE ==  IsNewLine)
					gST->ConOut->OutputString (gST->ConOut, (CHAR16*)L"\r\n");
				else
					gST->ConOut->OutputString (gST->ConOut, (CHAR16*)L"\r");
			}
        }
    }
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    :   InitializeBackground
//
// Description  :   Initializes Text mode Background 
//
// Parameter    :   None
//
// Return Value  :  VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID InitializeBackground()
{
    EFI_STATUS  Status = EFI_SUCCESS;
    gUsePostMgrService = FALSE;
    
    TRACE ((TRACE_ALWAYS, "\nInitializeBackground"));
    Status = gST->ConOut->ClearScreen(gST->ConOut);
    if(EFI_ERROR(Status))
    {
        TRACE ((TRACE_ALWAYS, "\nClearScreen failed"));
        return;
    }

    Status = gST->ConOut->SetCursorPosition(gST->ConOut, 0, 1);
    if(EFI_ERROR(Status))
    {
        TRACE ((TRACE_ALWAYS, "\nSetCursorPosition failed"));
        return;
    }
    
    ShowMessageInTextMode((CHAR16*)L"#########################", FALSE, TRUE);
    ShowMessageInTextMode((CHAR16*)L"Remote Firmware Update", FALSE, TRUE);
    ShowMessageInTextMode((CHAR16*)L"#########################", FALSE, TRUE);
    TRACE ((TRACE_ALWAYS, "\nBackground initialized"));
}

//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**       (C)Copyright 1985-2015, American Megatrends, Inc.     **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateUIIntf.h
//
// Description:	Defines Rescue UI interfaces
//
//<AMI_FHDR_END>
//*****************************************************************//

#ifndef __AMIFWUPDATEAPPUIINTF_H_
#define __AMIFWUPDATEAPPUIINTF_H_

#ifndef INVALID_TOKEN
#define	INVALID_TOKEN	((UINT16)-1)
#endif

#define MAX_ARG_COUNT 20

// UI interface related functions - start - to be called by UIAppModule
EFI_STATUS FWUP_UI_InterfaceInit(EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE *SystemTable);
VOID FWUP_UI_ShowError(UINT32 StrID, CHAR16 *ErrMsg);
VOID FWUP_UI_ShowStatus(UINT32 StrID, CHAR16 *StatusMsg);
BOOLEAN FWUP_UI_ShowConfirmation(UINT32 StrID, CHAR16 *ErrMsg);
VOID FWCore_ReportError(UINT32 StrID, CHAR16 *ErrMsg);
VOID FWCore_ReportStatus(UINT32 StrID, CHAR16 *StatusMsg);
VOID FWCore_ReportEndStatus(UINT32 StrID, CHAR16 *StatusMsg);
BOOLEAN FWCore_ReportConfirmation(UINT32 StrID, CHAR16 *StatusMsg);
// UI interface related functions - ends

CHAR16 *FW_HiiGetString( VOID* handle, UINT16 token );
UINTN UI_GetBCPCheckBoxValues(void) ;
BOOLEAN Provide_BCPSupport(void) ;

VOID InitializeBackground();
VOID ShowMessageInTextMode(VOID *Message,BOOLEAN IsMsgIntegerType,BOOLEAN IsNewLine);
BOOLEAN OperateCommmandLineArg(EFI_HANDLE ImageHandle);
BOOLEAN PerformTextModeOperation(EFI_HANDLE ImageHandle);
UINTN GetHandleIndexFromVol(CHAR16 *VolName);

BOOLEAN 	IsTextMode();
EFI_STATUS 	_AMIFWUpdateFlash_TextMode();
#endif

//************************************************************************
//************************************************************************
//**                                                                    **
//**        (C)Copyright 2015, American Megatrends, Inc.                **
//**                                                                    **
//**                       All Rights Reserved.                         **
//**                                                                    **
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                    **
//**                       Phone: (770)-246-8600                        **
//**                                                                    **
//************************************************************************
//************************************************************************

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**           6145-F Northbelt Pkwy, Norcross, GA 30071         **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/FormBrowser2.h $
//
// $Author: Rajashakerg $
//
// $Revision: 8 $
//
// $Date: 9/17/12 6:22a $
//
//*****************************************************************//
/** @file FormBrowser2.h

**/
//*************************************************************************

//----------------------------------------------------------------------------
#ifndef _FORM_BROWSER2_H
#define _FORM_BROWSER2_H
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
#include "minisetup.h"
#include EFI_PROTOCOL_DEFINITION (FormBrowser2)
//----------------------------------------------------------------------------
//Added for UEFI spec. 2.3 compatibility
#ifndef EFI_BROWSER_ACTION_REQUEST_FORM_SUBMIT_EXIT
#define EFI_BROWSER_ACTION_REQUEST_FORM_SUBMIT_EXIT 	4
#endif

#ifndef EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT
#define EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT	5
#endif

#ifndef EFI_BROWSER_ACTION_REQUEST_FORM_APPLY
#define EFI_BROWSER_ACTION_REQUEST_FORM_APPLY	6
#endif

#ifndef EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD
#define EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD	7
#endif
//Added for UEFI spec. 2.3 compatibility
VOID SetCallBackControlInfo(EFI_HANDLE VarHandle, UINT32 VarIndex);

EFI_STATUS  InstallFormBrowserProtocol(EFI_HANDLE Handle);
VOID UnInstallFormBrowserProtocol(EFI_HANDLE Handle);

EFI_STATUS
EFIAPI
SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  );

EFI_STATUS
EFIAPI
BrowserCallback (
  IN CONST EFI_FORM_BROWSER2_PROTOCOL  *This,
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN CONST EFI_GUID                    *VariableGuid, OPTIONAL
  IN CONST CHAR16                      *VariableName  OPTIONAL
  );

EFI_STATUS FormBrowserLocateSetupHandles(VOID*	**handleBuffer,UINT16 *count);
BOOLEAN FormBrowserHandleValid(VOID);

#endif /* #define _FORM_BROWSER2_H */
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

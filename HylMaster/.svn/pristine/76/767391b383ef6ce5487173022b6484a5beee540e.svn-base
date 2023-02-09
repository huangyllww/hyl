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

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include <Setup.h>
#include <AmiDxeLib.h>
//#include <Library/OemDebugAlwaysLib.h>
#include "OemDebugAlways.h"
/**
    This function sets the Memory Clock speed
**/
// External data definitions
EFI_GUID OemSetupVariableGuid = OEM_COMMON_VAR_GUID;
UINT32   VariableAttribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

VOID
EFIAPI
SetPOSTEXCallBack (
  IN EFI_HII_HANDLE HiiHandle,
  IN UINT16         Class, 
  IN UINT16         SubClass, 
  IN UINT16         Key )
{
    OEM_COMMON_SETUP_DATA   OEMSetupData;
    UINTN                   VariableSize;
    EFI_STATUS              Status;
    

    VariableSize = sizeof(OEM_COMMON_SETUP_DATA);
    Status = pRS->GetVariable(
                    L"OEM_VAR", 
                    &OemSetupVariableGuid,
                    &VariableAttribute,
                    &VariableSize,
                    &OEMSetupData);
    if(EFI_ERROR(Status))
        return;

    //Override for Memory Configuration
    if (OEMSetupData.PostExEn != 0)
      PcdSet8(PcdPostCodeDebug,1);
    else
      PcdSet8(PcdPostCodeDebug,0);

    return;
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

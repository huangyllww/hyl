//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
//#include <Library/DebugLib.h>
#include "setupdata.h"
#include <Variable.h>
#include <TseSetupTransfer.h>


//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern EFI_BOOT_SERVICES *gBS;
extern EFI_HANDLE   gImageHandle;
extern VARIABLE_LIST    *gVariables;
extern NVRAM_VARIABLE   *gVariableList;
extern INTN EfiStrCmp (IN CHAR16 *String, IN CHAR16 *String2);


//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------
EFI_STATUS  VarGetBuffer(CHAR16 Name[40],EFI_GUID Guid, UINT8** buf,UINTN *size); 

Tse_Setup_transfer gTse_Setup_transfer_List = {
    VarGetBuffer,
};

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------

//<AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Output:    VOID
//
// Modified:  Nothing
//
// Referrals: None
//
// Note:
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS 	VarGetBuffer(
	CHAR16	Name[40],EFI_GUID Guid,	UINT8** buf,UINTN *size) 
{
	UINTN	i;
	VARIABLE_INFO *varInfo = NULL;

	for(i=0;i<gVariables->VariableCount;i++) {
		varInfo = (VARIABLE_INFO *)((UINT8 *)gVariables + gVariables->VariableList[i]);
		if(EfiStrCmp(Name,varInfo->VariableName) ==0 && guidcmp(&Guid,&varInfo->VariableGuid)==0) {
			*buf = gVariableList[i].Buffer;
			*size = gVariableList[i].Size;
			return EFI_SUCCESS;
		}
	}	
	return EFI_NOT_FOUND;	
}

//ASUS+
//<AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Output:    VOID
//
// Modified:  Nothing
//
// Referrals: None
//
// Note:
//-------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID InstallTseSetupProtocol(VOID)
{
	EFI_STATUS Status;
	EFI_GUID gTseTransferGuid = TSE_TRANSFER_GUID;

	Status = gBS->InstallProtocolInterface (
			&gImageHandle, &gTseTransferGuid, 
			EFI_NATIVE_INTERFACE, &gTse_Setup_transfer_List
		);
    return;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

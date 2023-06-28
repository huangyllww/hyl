//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file ElinkPeiLib.c
    This file contains the functions used for AMI Elink

**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <Library/BaseLib.h>
#include <Uefi/UefiBaseType.h>
#include <Library/ElinkLib.h>
#include <Library/MemoryAllocationLib.h>

/**
    This function compares ParentName with the name published in
    gAmiElinkTable, and add the Function to the corresponding
    elink while matches.

    @param TokenNumber The name of the parent elink

    @retval UINTN elink pointer

**/
UINTN 
ElinkGet (
  IN UINTN            TokenNumber
  )
{
  return (UINTN) LibPcdGet32 (TokenNumber);
}


/**
    This function compares ParentName with the name published in
    gAmiElinkTable, and add the Function to the corresponding
    elink while matches.

    @param TokenNumber The name of the parent elink
    @param Function The function which is going to be added to the
        parent elink.

    @retval EFI_INVALID_PARAMETER mAmiElinkTableNum is zero
    @retval EFI_OUT_OF_RESOURCES Failed to add elink
    @retval EFI_NOT_FOUND Can not find matched name in
        gAmiElinkTable
    @retval EFI_SUCCESS Find matched name in gAmiElinkTable
        and is successful to add elink

**/

EFI_STATUS
ElinkAdd (
  IN UINTN                TokenNumber,
  IN AMI_HOOK             Function
  )
{
  UINT32                   ElinkPtr;
  AMI_HOOK_LINK           *AmiHookLink;
  UINT32                   Index;

  ElinkPtr = LibPcdGet32 (TokenNumber);

  if (ElinkPtr == 0)  {
    AmiHookLink = AllocateZeroPool (sizeof(AMI_HOOK_LINK));
    if (AmiHookLink == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    LibPcdSet32 (TokenNumber, (UINT32) AmiHookLink);
  } else {
    AmiHookLink = (AMI_HOOK_LINK *) ElinkPtr;
  }

  for (Index = 0; Index < ELINK_ARRAY_NUM; Index++) {
    if (AmiHookLink->ElinkArray[Index] == NULL) {
      AmiHookLink->ElinkArray[Index] = Function;
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

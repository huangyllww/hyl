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
/** @file ElinkLib.h
    This file contains the definitions used by ElinkLib and the
    interfaces provided by ElinkLib.

**/
//*************************************************************************

#ifndef __ELINK_LIB_H__
#define __ELINK_LIB_H__

#include <token.h>
#include <Library/PcdLib.h>

//#if !defined (_PCD_VALUE_PcdElinkArrayNum)
//#error "Please define gAmiChipsetPkgTokenSpaceGuid.PcdElinkArrayNum in the INF file while including ElinkLib.h ! "
//#endif

#define ElinkRegister(TokenName, Function) \
                                            ElinkAdd (_PCD_TOKEN_##TokenName, Function)

typedef VOID*         AMI_HOOK;

typedef struct  _AMI_HOOK_LINK          AMI_HOOK_LINK;


struct _AMI_HOOK_LINK {
//  AMI_HOOK            ElinkArray[FixedPcdGet32 (PcdElinkArrayNum)];
  AMI_HOOK            ElinkArray[ELINK_ARRAY_NUM];
};


#pragma pack(1)

typedef struct {
  UINTN               TokenNumber;
} AMI_ELINK_TABLE;

typedef struct {
  UINTN               ElinkPtr;
} AMI_HOOK_TABLE;

#pragma pack()

EFI_STATUS
ElinkAdd (
  IN UINTN            TokenNumber,
  IN AMI_HOOK         Function
  );

UINTN 
ElinkGet (
  IN UINTN            TokenNumber
  );

#endif

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

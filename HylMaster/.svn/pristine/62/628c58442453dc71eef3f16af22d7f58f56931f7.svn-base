//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
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
// $Header: $
//
// $Revision:  $
//
// $Date: $
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	CpuSmmCommon.h
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __CPUSMMCOMMON__H__
#define __CPUSMMCOMMON__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Protocol/SmmConfiguration.h>
#include "token.h"

typedef struct {
    UINT32  ApicId;
    UINTN   SmmBase;
    UINTN   StackTop;
}CPU_SMM_I;

typedef struct {
    UINT32              NumCpus;
    volatile UINT32     fSmmDone;
    volatile UINT32     fNumInSmm;
    volatile UINT32     BspIndex;
    volatile BOOLEAN    InSmm[NCPU];
	volatile BOOLEAN    InsideSmm;
    EFI_SMM_ENTRY_POINT CoreSmmEntryPoint; // SMM Foundation Entry Point
    EFI_HANDLE          CpuHandle;
    VOID                *SmmPageTableStart;
}CPU_SMM_PRIVATE_DATA;

typedef VOID (*SMM_AP_PROCEDURE) (
    IN VOID *ProcArguments
);

typedef struct {
    volatile SMM_AP_PROCEDURE   Procedure;
    volatile VOID               *ProcArguments;
} SMM_AP_CPU_CONTROL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:    CpuSmmCommon.c
//
// Description: Cpu Smm Driver common data
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

#include    "CpuSmmCommon.h"


    CPU_SMM_PRIVATE_DATA    gCpuSmmInt;
    CPU_SMM_I               gCoreDataI[FixedPcdGet32(PcdCpuMaxCores)];
    CPU_SMM_I               *pCoreDataI = &gCoreDataI[0];
    SMM_AP_CPU_CONTROL      gApCpuControl[FixedPcdGet32(PcdCpuMaxCores)];
    UINT32                  MaxCores = FixedPcdGet32(PcdCpuMaxCores);



//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
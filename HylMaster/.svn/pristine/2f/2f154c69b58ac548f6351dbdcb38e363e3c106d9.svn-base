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

//*************************************************************************
/** @file HygonSbWrapper.h
    This header file contains the HygonSbWrapper.h definition.

**/
//*************************************************************************

#ifndef HYGONSBWRAPPERPPI_H_
#define HYGONSBWRAPPERPPI_H_

// Refer to HYGON reference code.
#include <Ppi/SbPpi.h>
#include "AGESA.h"
#include "AgesaModulePkg\Fch\Taishan\FchTaishanCore\Common\FchCommonCfg.h"

typedef struct _HYGON_FCH_AMI_POLICY_PPI  HYGON_FCH_AMI_POLICY_PPI;

typedef VOID (EFIAPI *AMI_FCH_AGESA_HOOK_PEI)(
  IN EFI_PEI_SERVICES **PeiServices, IN HYGON_FCH_AMI_POLICY_PPI *This, IN VOID *InitAgesaHook
);

#pragma pack(1)

//============================================
// AMI AGESA-FCH call out PPI. To notify user.
//============================================

#define HYGON_FCH_AMI_CALLOUT_PPI_GUID \
    {0xA68DE042, 0x3264, 0x4CFD, 0xB7, 0x8B, 0x5E, 0xC2, 0xAD, 0x42, 0x92, 0xE3}

typedef struct _HYGON_FCH_AMI_CALLOUT_PPI       HYGON_FCH_AMI_CALLOUT_PPI;

// User can use the notification PPI below.
typedef enum {
    AfacInitReset = 0,
    AfacInitEarly,
    AfacInitPost,
    AfacS3LateRestore,
    AfacAfterInitReset,
    AfacAfterInitEarly,
    AfacAfterInitPost,
    AfacAfterS3LateRestore,
    AfacAgesaFchOemCalloutPei,
    AfacMax,
} HYGON_FCH_AMI_CALLOUT_TYPE;

typedef struct _HYGON_FCH_AMI_CALLOUT_PPI {
    HYGON_FCH_AMI_CALLOUT_TYPE    CallOutType;
    // refer to FchCommonCfg.h and AGESA.h for CallOutData.
    VOID                        *CallOutData;
} HYGON_FCH_AMI_CALLOUT_PPI;


//===========================================================================
// Wrapper PPI communicate with AGESA-FCH and pass parameter into AGESA-FCH.
//===========================================================================
#define HYGON_FCH_AMI_POLICY_PPI_GUID \
    {0x5752A401, 0xB49F, 0x4309, 0xB9, 0x87, 0x29, 0x90, 0x5, 0xFE, 0xB3, 0x29}

typedef struct _HYGON_FCH_AMI_POLICY_PPI {
    AMI_GPIO_INIT_TABLE_STRUCT  *DefaultGpioTbl;
    UINT32                      DefaultGpioTblEntries;
    AMI_PEI_SB_CUSTOM_PPI       *AmiPeiSbCustomPpi;
//###    VOID                        *HygonFchOemResetProgrammingTablePtr;
    AMI_FCH_AGESA_HOOK_PEI      HookInitReset;
    AMI_FCH_AGESA_HOOK_PEI      HookInitEarly;
    AMI_FCH_AGESA_HOOK_PEI      HookInitPost;
    AMI_FCH_AGESA_HOOK_PEI      HookS3LateRestore;
    AMI_FCH_AGESA_HOOK_PEI      HookAfterInitReset;
    AMI_FCH_AGESA_HOOK_PEI      HookAfterInitEarly;
    AMI_FCH_AGESA_HOOK_PEI      HookAfterInitPost;
    AMI_FCH_AGESA_HOOK_PEI      HookAfterS3LateRestore;
    AMI_FCH_AGESA_HOOK_PEI      HookAgesaFchOemCalloutPei;
    AMI_FCH_AGESA_HOOK_PEI      HookApplyGpioTable;
    FCH_RESET_DATA_BLOCK        HygonParameters; // This is HYGON-FCH parameter
    BOOLEAN                     HygonSafeDefault;
} HYGON_FCH_AMI_POLICY_PPI;


//===========================================================================
// Others.
//===========================================================================

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HYGON FCH doesn't provide gHygonPeiSbInitDonePolicyGuid anymore.
//                      This is for backward compatible old PXP module.
#define HYGON_PEI_SB_INIT_DONE_POLICY_PPI_GUID \
  { 0x9b584afb, 0x600d, 0x42b5, 0x82, 0xdb, 0x2d, 0xbc, 0x9d, 0xc0, 0xa9, 0x80 }

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#pragma pack()

extern EFI_GUID gHygonFchAmiCallOutPpiGuid;
extern EFI_GUID gHygonFchAmiPolicyPpiGuid;
extern EFI_GUID gHygonPeiSbInitDonePolicyGuid;

#endif /* HYGONSBWRAPPERPPI_H_ */

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

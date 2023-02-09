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
/** @file HygonSbPolicy.h
    the policy for pass parameter to SouthBridge Driver.

**/
//**********************************************************************

#ifndef _HYGONSB_POLICY_
#define _HYGONSB_POLICY_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HYGON_FCH_AMI_POLICY_PROTOCOL HYGON_FCH_AMI_POLICY_PROTOCOL;

typedef VOID (EFIAPI *AMI_FCH_AGESA_HOOK)(
    IN HYGON_FCH_AMI_POLICY_PROTOCOL *This, IN VOID *InitAgesaHook
);

#pragma pack(1)

//===============================================================================
// Wrapper PROTOCOL communicate with AGESA-FCH and pass parameter into AGESA-FCH.
//===============================================================================
#define HYGON_FCH_AMI_POLICY_PROTOCOL_GUID \
        {0xfdbf9134, 0xf2dd, 0x49e1, 0xbf, 0xc4, 0xf5, 0x78, 0x88, 0xf5, 0xb0, 0x91}
#define HYGONSB_POLICY_PROTOCOL_GUID HYGON_FCH_AMI_POLICY_PROTOCOL_GUID

extern EFI_GUID gHygonSbPolicyProtocolGuid;
extern EFI_GUID gHygonSbAmiPolicyProtocolGuid;
extern EFI_GUID gHygonFchAmiPolicyProtocolGuid;

// Refer to HYGON reference code.
#include "AgesaModulePkg\Include\AGESA.h"
#include "AgesaModulePkg\Fch\Taishan\FchTaishanCore\Common\FchCommonCfg.h"

typedef struct _HYGON_FCH_AMI_POLICY_PROTOCOL {
    UINT8   RevisionID;
    UINT32  SataClass:3;
    UINT32  SataIdeCombinedMode:1;
    UINT32  SataIdeCombMdPriSecOpt:1;
    // Don't use the member above. that is for backward compatible.
    AMI_FCH_AGESA_HOOK      HookInitEnv;
    AMI_FCH_AGESA_HOOK      HookInitMid;
    AMI_FCH_AGESA_HOOK      HookInitLate;
    AMI_FCH_AGESA_HOOK      HookCustomizeS3Save;
    AMI_FCH_AGESA_HOOK      HookAfterInitEnv;
    AMI_FCH_AGESA_HOOK      HookAfterInitMid;
    AMI_FCH_AGESA_HOOK      HookAfterInitLate;
    AMI_FCH_AGESA_HOOK      HookAgesaFchOemCalloutDxe;
    FCH_DATA_BLOCK          HygonParameters; // This is HYGON-FCH parameter
    BOOLEAN                 HygonSafeDefault;
}HYGON_FCH_AMI_POLICY_PROTOCOL, /* Don't use the right member */HYGONSB_POLICY_PROTOCOL/* It's backward compatible */;


//===============================================================================
// Others.
//===============================================================================

#define SB_ROMHOLE_SIG_GUID  \
   {0x05ca0209,0x0fc1,0x11dc,0x90,0x11,0x00,0x17,0x31,0x53,0xeb,0xa8}
#define SB_ROMHOLE_IMC_GUID  \
   {0x05ca020a,0x0fc1,0x11dc,0x90,0x11,0x00,0x17,0x31,0x53,0xeb,0xa8}
#define SB_ROMHOLE_GEC_GUID  \
   {0x05ca020b,0x0fc1,0x11dc,0x90,0x11,0x00,0x17,0x31,0x53,0xeb,0xa8}
#define SB_ROMHOLE_XHCI_GUID \
   {0x05ca0208,0x0fc1,0x11dc,0x90,0x11,0x00,0x17,0x31,0x53,0xeb,0xa8}

#pragma pack()

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

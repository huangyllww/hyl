
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols
#include <Protocol\PciIo.h>
#include <Uefi\UefiBaseType.h>
#include <Token.h>
#include "Setup.h"
#include "Library/PcdLib.h"
#include "Library/AmdCbsVariable.h"

//#include "WorkLoadingSetting.h"

#include "OemLib.h"

/// P-state MSR
typedef union {
  struct {
    UINT64 CpuFid_7_0:8;               ///< CpuFid[7:0]
    UINT64 CpuDid:6;                   ///< CpuDid
    UINT64 CpuVid:8;                   ///< CpuVid
    UINT64 IddValue:8;                 ///< IddValue
    UINT64 IddDiv:2;                   ///< IddDiv
    UINT64 :31;                        ///< Reserved
    UINT64 PstateEn:1;                 ///< Pstate Enable
  } Field;
  UINT64  Value;
} PSTATE_MSR;

#define MSR_PSTATE_0 0xC0010064ul
#define MSR_PSTATE_1 0xC0010065ul
#define MSR_PSTATE_2 0xC0010066ul
#define MSR_PSTATE_3 0xC0010067ul
#define MSR_PSTATE_4 0xC0010068ul
#define MSR_PSTATE_5 0xC0010069ul
#define MSR_PSTATE_6 0xC001006Aul
#define MSR_PSTATE_7 0xC001006Bul

#define PSTATE_0 0
#define PSTATE_1 1
#define PSTATE_2 2
#define PSTATE_3 3
#define PSTATE_4 4
#define PSTATE_5 5
#define PSTATE_6 6
#define PSTATE_7 7

#define DEFAULT_FID 0x10
#define DEFAULT_DID 0x8
#define DEFAULT_VID 0xFF

#define DISABLE_PSTATE 0
#define CUSTOM_PSTATE  1
#define AUTO_PSTATE    2

UINT32
GetPstateInfoFeq (
  IN  UINT8 Fid,
  IN  UINT8 Did
  )
{
  return (UINT32) (DivU64x32 (MultU64x64 (200, Fid), Did));
}

UINT32
GetPstateInfoVoltage (
  IN  UINT8 Vid
  )
{
  return (UINT32) (1550000L - MultU64x64 (6250, Vid));
}

VOID
SetFidDidVidToDefault1 (
  OUT CBS_CONFIG *pSetup_Config
  )
{
    PSTATE_MSR PstateMsr;
    UINT8 Index;
    if ((pSetup_Config->CbsCpuPstFid[0] == 0) || (pSetup_Config->CbsCpuPstFid[1] == 0) || (pSetup_Config->CbsCpuPstFid[2] == 0))
    {
        for (Index = 0; Index < 3; Index++)
        {
            PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + Index);
            if (PstateMsr.Field.PstateEn == 1) {
                pSetup_Config->CbsCpuPstFid[Index]  = (UINT8) PstateMsr.Field.CpuFid_7_0;
                pSetup_Config->CbsCpuPstDid[Index]  = (UINT8) PstateMsr.Field.CpuDid;
                pSetup_Config->CbsCpuPstVid[Index]  = (UINT8) PstateMsr.Field.CpuVid;
            }
            else
            {
                pSetup_Config->CbsCpuPstFid[Index]  = DEFAULT_FID;
                pSetup_Config->CbsCpuPstDid[Index]  = DEFAULT_DID;
                pSetup_Config->CbsCpuPstVid[Index]  = DEFAULT_VID;
            }
        }

    }

    pSetup_Config->CbsCpuPst0Fid = pSetup_Config->CbsCpuPstFid[0];
    pSetup_Config->CbsCpuPst0Did = pSetup_Config->CbsCpuPstDid[0];
    pSetup_Config->CbsCpuPst0Vid = pSetup_Config->CbsCpuPstVid[0];

    pSetup_Config->CbsCpuPst1Fid = pSetup_Config->CbsCpuPstFid[1];
    pSetup_Config->CbsCpuPst1Did = pSetup_Config->CbsCpuPstDid[1];
    pSetup_Config->CbsCpuPst1Vid = pSetup_Config->CbsCpuPstVid[1];

    pSetup_Config->CbsCpuPst2Fid = pSetup_Config->CbsCpuPstFid[2];
    pSetup_Config->CbsCpuPst2Did = pSetup_Config->CbsCpuPstDid[2];
    pSetup_Config->CbsCpuPst2Vid = pSetup_Config->CbsCpuPstVid[2];


    pSetup_Config->CbsCpuPst3Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst3Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst3Vid = DEFAULT_VID;

    pSetup_Config->CbsCpuPst4Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst4Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst4Vid = DEFAULT_VID;

    pSetup_Config->CbsCpuPst5Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst5Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst5Vid = DEFAULT_VID;

    pSetup_Config->CbsCpuPst6Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst6Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst6Vid = DEFAULT_VID;

    pSetup_Config->CbsCpuPst7Fid = DEFAULT_FID;
    pSetup_Config->CbsCpuPst7Did = DEFAULT_DID;
    pSetup_Config->CbsCpuPst7Vid = DEFAULT_VID;

}

VOID
SetFollowingCustomPstateOptions (
  OUT CBS_CONFIG *pSetup_Config,
  IN  UINT8 Pstate,
  IN  UINT8 Choice
  )
{
  if (Pstate == PSTATE_0) {
    pSetup_Config->CbsCpuPstCustomP0 = Choice;
  }
  if (Pstate <= PSTATE_1) {
    pSetup_Config->CbsCpuPstCustomP1 = Choice;
  }
  if (Pstate <= PSTATE_2) {
    pSetup_Config->CbsCpuPstCustomP2 = Choice;
  }
  if (Pstate <= PSTATE_3) {
    pSetup_Config->CbsCpuPstCustomP3 = Choice;
  }
  if (Pstate <= PSTATE_4) {
    pSetup_Config->CbsCpuPstCustomP4 = Choice;
  }
  if (Pstate <= PSTATE_5) {
    pSetup_Config->CbsCpuPstCustomP5 = Choice;
  }
  if (Pstate <= PSTATE_6) {
    pSetup_Config->CbsCpuPstCustomP6 = Choice;
  }
  if (Pstate <= PSTATE_7) {
    pSetup_Config->CbsCpuPstCustomP7 = Choice;
  }
}
//<hujf1001-20200512 Add Fix P state feature +>

VOID
GetPstateSetting (
  OUT CBS_CONFIG *pSetup_Config,
  IN  UINT8       Pstate,
  OUT UINT32     *CpuFreq,
  OUT UINT8      *CpuFid,
  OUT UINT8      *CpuDid,
  OUT UINT8      *CpuVid
  )
{
  UINT8 CustomPstateX;
  UINT8 CustomPstateXFid;
  UINT8 CustomPstateXDid;
  UINT8 CustomPstateXVid;
  PSTATE_MSR PstateMsr;

  switch (Pstate) {
  case PSTATE_0:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP0;
    CustomPstateXFid = pSetup_Config->CbsCpuPst0Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst0Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst0Vid;
    break;
  case PSTATE_1:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP1;
    CustomPstateXFid = pSetup_Config->CbsCpuPst1Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst1Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst1Vid;
    break;
  case PSTATE_2:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP2;
    CustomPstateXFid = pSetup_Config->CbsCpuPst2Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst2Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst2Vid;
    break;
  case PSTATE_3:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP3;
    CustomPstateXFid = pSetup_Config->CbsCpuPst3Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst3Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst3Vid;
    break;
  case PSTATE_4:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP4;
    CustomPstateXFid = pSetup_Config->CbsCpuPst4Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst4Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst4Vid;
    break;
  case PSTATE_5:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP5;
    CustomPstateXFid = pSetup_Config->CbsCpuPst5Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst5Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst5Vid;
    break;
  case PSTATE_6:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP6;
    CustomPstateXFid = pSetup_Config->CbsCpuPst6Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst6Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst6Vid;
    break;
  case PSTATE_7:
    CustomPstateX = pSetup_Config->CbsCpuPstCustomP7;
    CustomPstateXFid = pSetup_Config->CbsCpuPst7Fid;
    CustomPstateXDid = pSetup_Config->CbsCpuPst7Did;
    CustomPstateXVid = pSetup_Config->CbsCpuPst7Vid;
    break;
  default:
    *CpuFreq = 0;
    *CpuVid = 0xFF;
    return;
    break;
  }

  if (CustomPstateX == DISABLE_PSTATE) {
    *CpuFreq = 0;
    *CpuFid  = DEFAULT_FID;
    *CpuDid  = DEFAULT_DID;
    *CpuVid  = DEFAULT_VID;
  } else if (CustomPstateX == CUSTOM_PSTATE) {
    *CpuFreq = GetPstateInfoFeq (CustomPstateXFid, CustomPstateXDid);
    *CpuFid  = CustomPstateXFid;
    *CpuDid  = CustomPstateXDid;
    *CpuVid  = CustomPstateXVid;
  } else {
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + Pstate);
    if (PstateMsr.Field.PstateEn == 1) {
      *CpuFreq = GetPstateInfoFeq ((UINT8) PstateMsr.Field.CpuFid_7_0, (UINT8) PstateMsr.Field.CpuDid);
      *CpuFid  = (UINT8) PstateMsr.Field.CpuFid_7_0;
      *CpuDid  = (UINT8) PstateMsr.Field.CpuDid;
      *CpuVid  = (UINT8) PstateMsr.Field.CpuVid;
    } else {
      *CpuFreq = 0;
      *CpuFid  = DEFAULT_FID;
      *CpuDid  = DEFAULT_DID;
      *CpuVid  = DEFAULT_VID;
    }
  }
}

VOID
UpdatePstInfo0 (
  OUT CBS_CONFIG                             *pSetup_Config
  )
{
  UINT8  CurrentFid;
  UINT8  AdjacentFid;
  UINT8  CurrentDid;
  UINT8  AdjacentDid;
  UINT8  CurrentVid;
  UINT8  AdjacentVid;
  UINT32 CurrentFreq;
  UINT32 AdjacentFreq;

  if (pSetup_Config->CbsCpuPst0Did > 0x1A) {
    pSetup_Config->CbsCpuPst0Did &= 0xFE;
  }

  // Check settings
  GetPstateSetting (pSetup_Config, PSTATE_0, &CurrentFreq, &CurrentFid, &CurrentDid, &CurrentVid);
  GetPstateSetting (pSetup_Config, PSTATE_1, &AdjacentFreq, &AdjacentFid, &AdjacentDid, &AdjacentVid);
  if (AdjacentFreq != 0) {
    if (CurrentFreq < AdjacentFreq) {
      pSetup_Config->CbsCpuPst0Fid = AdjacentFid;
      pSetup_Config->CbsCpuPst0Did = AdjacentDid;
    }
  }
  if (AdjacentVid != DEFAULT_VID) {
    if (CurrentVid > AdjacentVid) {
      pSetup_Config->CbsCpuPst0Vid = AdjacentVid;
    }
  }

  // Display frequency
  if (pSetup_Config->CbsCpuPst0Did == 0) {
    pSetup_Config->CbsCpuCofP0 = 0;
  } else {
    pSetup_Config->CbsCpuCofP0 = GetPstateInfoFeq (pSetup_Config->CbsCpuPst0Fid, pSetup_Config->CbsCpuPst0Did);
  }

  // Display voltage
  if ((pSetup_Config->CbsCpuPst0Vid >= 0xF8) && (pSetup_Config->CbsCpuPst0Vid <= 0xFF)) {
    pSetup_Config->CbsCpuVoltageP0 = 0;
  } else {
    pSetup_Config->CbsCpuVoltageP0 = GetPstateInfoVoltage (pSetup_Config->CbsCpuPst0Vid);
  }
}

EFI_STATUS
FixPstate (
  OUT CBS_CONFIG                             *pSetup_Config
  )
{
    // Auto
    SetFidDidVidToDefault1 (pSetup_Config);
    SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_0, CUSTOM_PSTATE);

    if (pSetup_Config->CbsCpuPstateFix != 0) {
        pSetup_Config->CbsCmnCpuCpb = 0;
        SetFollowingCustomPstateOptions (pSetup_Config, PSTATE_1, DISABLE_PSTATE);
    }
    // P0
    if (pSetup_Config->CbsCpuPstateFix == 1) {
        UpdatePstInfo0 (pSetup_Config);
    }
    // P1
    if (pSetup_Config->CbsCpuPstateFix == 2) {
        pSetup_Config->CbsCpuPst0Fid = pSetup_Config->CbsCpuPst1Fid;
        pSetup_Config->CbsCpuPst0Did = pSetup_Config->CbsCpuPst1Did;
        pSetup_Config->CbsCpuPst0Vid = pSetup_Config->CbsCpuPst1Vid;
        UpdatePstInfo0 (pSetup_Config);
    }
    // P2
    if (pSetup_Config->CbsCpuPstateFix == 3) {
        pSetup_Config->CbsCpuPst0Fid = pSetup_Config->CbsCpuPst2Fid;
        pSetup_Config->CbsCpuPst0Did = pSetup_Config->CbsCpuPst2Did;
        pSetup_Config->CbsCpuPst0Vid = pSetup_Config->CbsCpuPst2Vid;
        UpdatePstInfo0 ( pSetup_Config);
    }
    return EFI_SUCCESS;
}
//<hujf1001-20200512 Add Fix P state feature ->

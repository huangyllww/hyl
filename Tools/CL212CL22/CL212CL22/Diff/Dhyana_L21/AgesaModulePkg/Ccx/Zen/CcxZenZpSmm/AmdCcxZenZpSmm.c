/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   AMD CCX Zen SMM  Driver
 * @e \$Revision: 315303 $   @e \$Date: 2015-03-24 12:41:35 +0800 (Tue, 24 Mar 2015) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include "AGESA.h"
#include "PiSmm.h"
#include <Library/BaseLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Library/IdsLib.h"
#include "Library/SmmServicesTableLib.h"
#include <Library/CcxRolesLib.h>
#include <Library/CcxSetMcaLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/AmdHeapLib.h>
#include <Protocol/AmdPspResumeServiceProtocol.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include "cpuRegisters.h"
#include "Filecode.h"

#define FILECODE CCX_ZEN_CCXZENZPSMM_AMDCCXZENZPSMM_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
CcxZenZpPfeh (
  );

VOID
CcxZenZpPfehAp (
  IN       VOID *Buffer
  );

VOID
CcxZenZpS3ResumeLateAp (
  IN       VOID *Buffer
  );

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// for MSR S3 save/restore
typedef struct {
  IN  UINT32 MsrAddr;     ///< MSR address
  IN  UINT64 MsrData;     ///< MSR data
  IN  UINT64 MsrMask;     ///< Mask to be used before data write. Set every bit of all save/restore fields.
} S3_SAVE_MSR;


/*----------------------------------------------------------------------------------------
 *               G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
S3_SAVE_MSR     mMsrSaveRestoreTable[] =
{
  { 0xC0011093, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  },
  { 0xC0011097, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  },
  { 0xC0010296, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  }, // Cstate_Config
  { 0xC0010015, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF  }, // HWCR, bit[0][SmmLock] should not be restored. IBV may clear this bit at very early of S3 resume path
  { 0xC0011028, 0x0000000000000000, 0x0000080000000000  }, //DNPS-86 FP_CFG[43][RET_DISACCF] should be restored
  { CPU_LIST_TERMINAL               }
};

UINT64 UcodePatch = 0;
BOOLEAN mPfehMode = FALSE;
FCH_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch = NULL;

/*----------------------------------------------------------------------------------------
 *               L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
CcxZenZpPfeh (
  )
{
  UINT16          i;
  LOCATE_HEAP_PTR LocateHeapParams;

  LocateHeapParams.BufferHandle = AMD_PFEH_HANDLE;
  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    mPfehMode = *((BOOLEAN *) LocateHeapParams.BufferPtr);
  }

  CcxZenZpPfehAp (NULL);

  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            CcxZenZpPfehAp,
            i,
            NULL
            );
  }
}

VOID
CcxZenZpPfehAp (
  IN       VOID *Buffer
  )
{
  // Platform First Error Handling
  if (mPfehMode) {
    AsmWriteMsr64 (0xC0010121, 0xFFFFFFFFFFFFFFFF);
    AsmWriteMsr64 (0xC0010122, 0);
    AsmMsrOr64 (0xC0010120, BIT0);
  } else {
    AsmWriteMsr64 (0xC0010121, 0);
    AsmWriteMsr64 (0xC0010122, 0);
    AsmMsrAnd64 (0xC0010120, ~(UINT64)BIT0);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Save MSRs to mMsrSaveRestoreTable
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
CcxZenZpS3SaveSmi (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  UINTN  i;

  // Platform First Error Handling
  CcxZenZpPfeh ();

  if(SwDispatch != NULL){
      SwDispatch->UnRegister(SwDispatch, DispatchHandle);
  }
  //return EFI_SUCCESS; //DNPS-86 -- Do not return

  for (i = 0; mMsrSaveRestoreTable[i].MsrAddr != CPU_LIST_TERMINAL; i++) {
    mMsrSaveRestoreTable[i].MsrData = AsmReadMsr64 (mMsrSaveRestoreTable[i].MsrAddr);
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback function for restore MSRs from mMsrSaveRestoreTable
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
CcxZenZpS3RestoreSmi (
  IN  RESUME_TYPE   ResumeType,
  IN  VOID          *Context
  )
{
  UINTN  i;
  SOC_LOGICAL_ID LogicalId;

  if (CcxIsBsp (NULL)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenZpS3RestoreSmi\n");

    // Clear MSRC001_1023[49, TwCfgCombineCr0Cd]
    AsmMsrAnd64 (0xC0011023, ~((UINT64) BIT49));

    // Workaround PLAT-11398
    // BSP gets ucode patch address from scratch MSR
    UcodePatch = AsmReadMsr64 (0xC001100C);
  }

  // Set ApicEn
  AsmMsrOr32 (0x1B, BIT11);

  // Workaround PLAT-11398, reload ucode patch during S3 resume. UcodePatch should only be non-zero for revs < B1
  if (UcodePatch != 0) {
    BaseGetLogicalIdOnExecutingCore (&LogicalId);
    if (CcxIsComputeUnitPrimary (NULL)) {
      if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
        AsmMsrOr64 (MSR_LS_CFG2, BIT7);
      }

      AsmWriteMsr64 (0xC0010020, UcodePatch);

      if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
        AsmWbinvd ();
        AsmMsrAnd64 (MSR_LS_CFG2, ~((UINT64) BIT7));
      }
    }
  }

  for (i = 0; mMsrSaveRestoreTable[i].MsrAddr != CPU_LIST_TERMINAL; i++) {
    IDS_HDT_CONSOLE (CPU_TRACE, "Restoring MSR %X with %lX\n", mMsrSaveRestoreTable[i].MsrAddr, mMsrSaveRestoreTable[i].MsrData);
    AsmMsrAndThenOr64 (mMsrSaveRestoreTable[i].MsrAddr, ~(mMsrSaveRestoreTable[i].MsrMask), (mMsrSaveRestoreTable[i].MsrData & mMsrSaveRestoreTable[i].MsrMask));
  }

  // Mca initialization
  CcxSetMca ();

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * All threads need to perform the following at late S3 resume:
 * - Force recalculation of TSC
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
CcxZenZpS3ResumeLateSmi (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  UINT16          i;

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenZpS3ResumeLateSmi Entry\n");

  CcxZenZpS3ResumeLateAp (NULL);

  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            CcxZenZpS3ResumeLateAp,
            i,
            NULL
            );
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenZpS3ResumeLateSmi Exit\n");
  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * This routine runs after S3 boot script has been executed and does the following:
 * - Force recalculation of TSC after custom Pstates registers have been restored
 */
/*----------------------------------------------------------------------------------------*/
VOID
CcxZenZpS3ResumeLateAp (
  IN       VOID *Buffer
  )
{
  UINT64 LocalMsrRegister;

  // Force recalc of TSC
  LocalMsrRegister = AsmReadMsr64 (0xC0010064);
  AsmWriteMsr64 (0xC0010064, LocalMsrRegister);

  // Clear and set again CpbDis
  LocalMsrRegister = AsmReadMsr64 (0xC0010015);
  if ((LocalMsrRegister & BIT25) == BIT25) {
    AsmMsrAnd64 (0xC0010015, ~((UINT64) BIT25));
    AsmMsrOr64 (0xC0010015, BIT25);
  }

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Perform WBINVD just before entering S3
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
CcxZenZpS3EntryAp (
  IN       VOID *Buffer
  )
{
  AsmWbinvd ();
}

/*----------------------------------------------------------------------------------------*/
/**
 * All threads need to perform WBINVD just before entering S3
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
CcxS3Callback (
  IN       EFI_HANDLE                          DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT   *DispatchContext,
  IN OUT   VOID                                *CommBuffer OPTIONAL,
  IN OUT   UINTN                               *CommBufferSize  OPTIONAL
  )
{
  UINT16      i;
  UINT64      MsrValue;

  IDS_HDT_CONSOLE (CPU_TRACE, "CCX S3 Callback\n");
  // Read value of MSRC001_0015[21] and update the value in mMsrSaveRestoreTable
  MsrValue = (AsmReadMsr64 (0xC0010015) & 0x0000000000200000);
  IDS_HDT_CONSOLE (CPU_TRACE, "C0010015 %X\n", MsrValue);
  for (i = 0; mMsrSaveRestoreTable[i].MsrAddr != CPU_LIST_TERMINAL; i++) {
    if (mMsrSaveRestoreTable[i].MsrAddr == 0xC0010015) {
      mMsrSaveRestoreTable[i].MsrData |= MsrValue;
      break;
    }
  }


  CcxZenZpS3EntryAp (NULL);
  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            CcxZenZpS3EntryAp,
            i,
            NULL
            );
  }

  return (EFI_SUCCESS);
}


/*----------------------------------------------------------------------------------------*/
/**
 * AMD ZEN driver main entry point
 *
 *
 * @param[in]       ImageHandle           Image Handle
 * @param[in]       SystemTable           Pointer to system globals
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
AmdCcxZenZpSmmDriverEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  PSP_RESUME_SERVICE_PROTOCOL    *PspResume;
  FCH_SMM_SW_REGISTER_CONTEXT     SwContext;
  EFI_HANDLE                      SwHandle;
  FCH_SMM_SX_DISPATCH2_PROTOCOL  *SxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT     SxContext;
  EFI_HANDLE                      SxHandle;

  AGESA_TESTPOINT (TpCcxSmmEntry, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenZpSmm Entry\n");

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Locate gFchSmmSwDispatch2ProtocolGuid failed\n");
    return EFI_SUCCESS;
  }

  // Install Save MSRs SMI handler
  SwContext.AmdSwValue  = PcdGet8 (PcdAmdCcxS3SaveSmi);
  SwContext.Order       = 0x80;
  Status = SwDispatch->Register (
                          SwDispatch,
                          CcxZenZpS3SaveSmi,
                          &SwContext,
                          &SwHandle
                          );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenZpS3SaveSmi is not registered\n");
    return EFI_SUCCESS;
  }

  // Install S3 resume late handler
  SwContext.AmdSwValue  = PcdGet8 (PcdFchOemBeforePciRestoreSwSmi);
  SwContext.Order       = 0x80;
  Status = SwDispatch->Register (
                          SwDispatch,
                          CcxZenZpS3ResumeLateSmi,
                          &SwContext,
                          &SwHandle
                          );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenZpS3ResumeLateSmi is not registered\n");
    return EFI_SUCCESS;
  }

  // Install Restore MSRs callback
  Status = gSmst->SmmLocateProtocol (
                  &gAmdPspResumeServiceProtocolGuid,
                  NULL,
                  &PspResume
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Locate gAmdPspResumeServiceProtocolGuid failed\n");
    return EFI_SUCCESS;
  }
  PspResume->Register (PspResume, CcxZenZpS3RestoreSmi, mMsrSaveRestoreTable, PSP_RESUME_CALLBACK_HIGHEST_LEVEL, PspResumeCallBackFlagAllCores, NULL);

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &SxDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Register a callback on S3 entry as one of the last hooks to run
  SxContext.Type  = SxS3;
  SxContext.Phase = SxEntry;
  SxContext.Order = 0xFE;

  IDS_HDT_CONSOLE (CPU_TRACE, "\tRegister Sleep SMI\n");
  Status = SxDispatch->Register (
                          SxDispatch,
                          CcxS3Callback,
                          &SxContext,
                          &SxHandle
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenZpSmm End\n");

  AGESA_TESTPOINT (TpCcxSmmExit, NULL);

  return EFI_SUCCESS;
}


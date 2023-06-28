/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS XV Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include "AGESA.h"
#include "cpuRegisters.h"
#include "CcxRegistersXv.h"
#include "UnbRegisters.h"
#include "Filecode.h"
#include "CcxSmbiosXvCommonLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/CcxSmbiosLib.h"
#include <AmdSoc.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXSMBIOSXVLIB_CCXSMBIOSXVLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8 ROMDATA str_A12[] = "AMD A12-";
CONST CHAR8 ROMDATA str_A10[] = "AMD A10-";
CONST CHAR8 ROMDATA str_A8[]  = "AMD A8-";
CONST CHAR8 ROMDATA str_A6[]  = "AMD A6-";
CONST CHAR8 ROMDATA str_A4[]  = "AMD A4-";
CONST CHAR8 ROMDATA str_FX[]  = "AMD FX-";
CONST CHAR8 ROMDATA str_AthlonX4[] = "AMD Athlon(TM) X4";
CONST CHAR8 ROMDATA str_R[]  = "AMD Embedded R-Series RX-";
CONST CHAR8 ROMDATA str_G[]  = "AMD Embedded G-Series GX-";
CONST CHAR8 ROMDATA str_PRO_A12[] = "AMD PRO A12-";
CONST CHAR8 ROMDATA str_PRO_A10[] = "AMD PRO A10-";
CONST CHAR8 ROMDATA str_PRO_A8[]  = "AMD PRO A8-";
CONST CHAR8 ROMDATA str_PRO_A6[]  = "AMD PRO A6-";
CONST CHAR8 ROMDATA str_PRO_A4[]  = "AMD PRO A4-";
CONST CHAR8 ROMDATA str_PRO_FX[]  = "AMD PRO FX-";
CONST CHAR8 ROMDATA str_OpteronX3[] = "AMD Opteron(TM) X3";
/*---------------------------------------------------------------------------------------
 * Processor Family Table
 *       04Bh = "R-Series"
 *       049h = "G-Series"
 *       048h = "A-Series"
 *       066h = "AMD Athlon(TM) X4 Series Processor"
 *       03Fh = "FX (TM) Series"
 *       06Ah = "AMD Opteron(TM) X3000 Series APU"
 *-------------------------------------------------------------------------------------*/

CONST CPU_T4_PROC_FAMILY ROMDATA XvT4ProcFamily[] =
{
  {str_FX,  0x3F},
  {str_A12, 0x48},
  {str_A10, 0x48},
  {str_A8,  0x48},
  {str_A6,  0x48},
  {str_A4,  0x48},
  {str_R,   0x4B},
  {str_G,   0x49},
  {str_AthlonX4,  0x66},
  {str_PRO_FX, 0x3F},
  {str_PRO_A12, 0x48},
  {str_PRO_A10, 0x48},
  {str_PRO_A8,  0x48},
  {str_PRO_A6,  0x48},
  {str_PRO_A4,  0x48},
  {str_OpteronX3, 0x6A}
};


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorFamilyForSmbios
 *
 *    Get processor family information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT16
CcxGetProcessorFamilyForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  CHAR8 NameString[49];
  CONST CHAR8 *DmiString;
  CONST VOID  *DmiStringTable;
  UINT8 NumberOfDmiString;
  UINT8 i;
  UINT16 ProcessofFamily;

  ProcessofFamily = P_FAMILY_UNKNOWN;

  // Get name string from MSR_C001_00[30:35]
  GetNameString (NameString, StdHeader);

  // Get DMI String
  DmiStringTable = (CONST VOID *) &XvT4ProcFamily[0];
  NumberOfDmiString = sizeof (XvT4ProcFamily) / sizeof (CPU_T4_PROC_FAMILY);

  // Find out which DMI string matches current processor's name string
  if (NumberOfDmiString != 0) {
    for (i = 0; i < NumberOfDmiString; i++) {
      DmiString = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).Stringstart;
      if (IsSourceStrContainTargetStr (NameString, DmiString, StdHeader)) {
        ProcessofFamily = (((CPU_T4_PROC_FAMILY *) DmiStringTable)[i]).T4ProcFamilySetting;
        break;
      }
    }
  }
  return ProcessofFamily;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorUpgradeForSmbios
 *
 *    Get processor upgrade information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT8
CcxGetProcessorUpgradeForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8      PackageType;
  UINT8      ProcUpgrade;
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  PackageType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  switch (PackageType) {
  case BR_PKG_AM4:
    ProcUpgrade = P_UPGRADE_AM4;
    break;
  default:
    ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }
  return ProcUpgrade;
}

VOID
CcxGetCacheInfo (
  IN       UINTN               Socket,
  IN       GET_CACHE_INFO     *CacheInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8                 NumOfCoresPerCU;
  UINT8                 NumOfCU;
  UINT32                CacheSize;
  UINTN                 PciAddress;
  CPUID_DATA            CpuId;
  CU_STATUS_1_REGISTER  CuStatus;

  PciAddress = CU_STATUS_1_PCI_ADDR;
  *((UINT32 *) &CuStatus) = PciRead32 (PciAddress);
  if (CuStatus.DualCore == 0) {
    NumOfCoresPerCU = 1;
  } else {
    NumOfCoresPerCU = 2;
  }

  switch (CuStatus.Enabled) {
  case 1:
  case 2:
    NumOfCU = 1;
    break;
  case 3:
    NumOfCU = 2;
    break;
  default:
    NumOfCU = 1;
    ASSERT (FALSE);
  }

  // L1 Size & Associativity
  AsmCpuid (
      AMD_CPUID_TLB_L1Cache,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize  = ((CpuId.ECX_Reg >> 24) * NumOfCoresPerCU + (CpuId.EDX_Reg >> 24)) * NumOfCU;
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].Associativity = DMI_ASSOCIATIVE_2_WAY; // Per the BKDG, this is hard-coded to 2-Way.

  // L2 Size & Associativity
  AsmCpuid (
      AMD_CPUID_L2L3Cache_L2TLB,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.ECX_Reg >> 16) * NumOfCU;
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].Associativity = DMI_ASSOCIATIVE_16_WAY; // Per the BKDG, this is hard-coded to 16-Way.

  // L3 Size & Associativity
  CacheInfo->CacheEachLevelInfo[CpuL3Cache].CacheSize = 0;
  CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_UNKNOWN;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH IO access common routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*;********************************************************************************
;
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
;*********************************************************************************/

#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHPELIB_FILECODE

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramPciByteTable - Program PCI register by table (8 bits data)
 *
 *
 *
 * @param[in] pPciByteTable    - Table data pointer
 * @param[in] dwTableSize      - Table length
 * @param[in] StdHeader
 *
 */
VOID
ProgramPciByteTable (
  IN       REG8_MASK           *pPciByteTable,
  IN       UINT16              dwTableSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8     i;
  UINT8     dbBusNo;
  UINT8     dbDevFnNo;
  UINTN     PciAddress;

  dbBusNo = pPciByteTable->RegIndex;
  dbDevFnNo = pPciByteTable->AndMask;
  pPciByteTable++;

  for ( i = 1; i < dwTableSize; i++ ) {
    if ( (pPciByteTable->RegIndex == 0xFF) && (pPciByteTable->AndMask == 0xFF) && (pPciByteTable->OrMask == 0xFF) ) {
      pPciByteTable++;
      dbBusNo = pPciByteTable->RegIndex;
      dbDevFnNo = pPciByteTable->AndMask;
      pPciByteTable++;
      i++;
    } else {
      PciAddress = (dbBusNo << 20) + (dbDevFnNo << 12) + pPciByteTable->RegIndex;
      PciAndThenOr8 (PciAddress, pPciByteTable->AndMask, pPciByteTable->OrMask);
      pPciByteTable++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchAcpiMmioTbl - Program FCH ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 * @param[in] StdHeader
 *
 */
VOID
ProgramFchAcpiMmioTbl (
  IN       ACPI_REG_WRITE      *pAcpiTbl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   i;
  UINT8   Or8;
  UINT8   Mask8;
  UINT32  ddtempVar;

  if (pAcpiTbl != NULL) {
    if ((pAcpiTbl->MmioReg == 0) && (pAcpiTbl->MmioBase == 0) && (pAcpiTbl->DataAndMask == 0xB0) && (pAcpiTbl->DataOrMask == 0xAC)) {
      // Signature Checking
      pAcpiTbl++;
      for ( i = 1; pAcpiTbl->MmioBase < 0x1D; i++ ) {
        ddtempVar = ACPI_MMIO_BASE | (pAcpiTbl->MmioBase) << 8 | pAcpiTbl->MmioReg;
        Or8 = pAcpiTbl->DataOrMask;
        Mask8 = ~pAcpiTbl->DataAndMask;
        LibAmdMemRMW (AccessWidth8, (UINT64) ddtempVar, &Or8, &Mask8, StdHeader);
        pAcpiTbl++;
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchSciMapTbl - Program FCH SCI Map table (8 bits data)
 *
 *
 *
 * @param[in] pSciMapTbl   - Table data pointer
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchSciMapTbl (
  IN       SCI_MAP_CONTROL  *pSciMapTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  AMD_CONFIG_PARAMS   *StdHeader;

  UINT32  ddtempVar;
  StdHeader = FchResetDataBlock->StdHeader;

  if (pSciMapTbl != NULL) {
    while (pSciMapTbl->InputPin != 0xFF) {
      if ((pSciMapTbl->InputPin >= 0x40) && (pSciMapTbl->InputPin < 0x80) && (pSciMapTbl->GpeMap < 0x20)) {
        ddtempVar = ACPI_MMIO_BASE | SMI_BASE | pSciMapTbl->InputPin;
        if (((pSciMapTbl->InputPin == FCH_SMI_xHC0Pme) && (FchResetDataBlock->FchReset.Xhci0Enable == 0)) || \
           ((pSciMapTbl->InputPin == FCH_SMI_xHC1Pme) && (FchResetDataBlock->FchReset.Xhci1Enable == 0))) {
        } else {
          LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pSciMapTbl->GpeMap, StdHeader);
        }
      } else {
        //Assert Warning "SCI map is invalid"
      }
      pSciMapTbl++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramFchGpioTbl - Program FCH Gpio table (8 bits data)
 *
 *
 *
 * @param[in] pGpioTbl   - Table data pointer
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchGpioTbl (
  IN       GPIO_CONTROL  *pGpioTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  AMD_CONFIG_PARAMS   *StdHeader;

  UINT32  ddtempVar;
  StdHeader = FchResetDataBlock->StdHeader;

  if (pGpioTbl != NULL) {
    while (pGpioTbl->GpioPin != 0xFF) {
      ddtempVar = ACPI_MMIO_BASE | IOMUX_BASE | pGpioTbl->GpioPin;
      LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pGpioTbl->PinFunction, StdHeader);
      ddtempVar = ACPI_MMIO_BASE | GPIO_BASE | pGpioTbl->GpioPin;
      LibAmdMemWrite (AccessWidth8, (UINT64) ddtempVar, &pGpioTbl->CfgByte, StdHeader);
      pGpioTbl++;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * ProgramSataPhyTbl - Program FCH Sata Phy table (8 bits data)
 *
 *
 *
 * @param[in] pSataPhyTbl   - Table data pointer
 * @param[in] FchResetDataBlock
 *
 */
VOID
ProgramFchSataPhyTbl (
  IN       SATA_PHY_CONTROL  *pSataPhyTbl,
  IN       FCH_RESET_DATA_BLOCK *FchResetDataBlock
  )
{
  if (pSataPhyTbl != NULL) {
    while (pSataPhyTbl->PhyData != 0xFFFFFFFF) {
      //to be implemented
      pSataPhyTbl++;
    }
  }
}

/**
 * GetChipSysMode - Get Chip status
 *
 *
 * @param[in] Value - Return Chip strap status
 *   StrapStatus [15.0] - Hudson-2 chip Strap Status
 *    @li <b>0001</b> - Not USED FWH
 *    @li <b>0002</b> - Not USED LPC ROM
 *    @li <b>0004</b> - EC enabled
 *    @li <b>0008</b> - Reserved
 *    @li <b>0010</b> - Internal Clock mode
 * @param[in] StdHeader
 *
 */
VOID
GetChipSysMode (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80), Value, StdHeader);
}

/**
 * IsImcEnabled - Is IMC Enabled
 * @retval  TRUE for IMC Enabled; FALSE for IMC Disabled
 */
BOOLEAN
IsImcEnabled (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   dbSysConfig;
  GetChipSysMode (&dbSysConfig, StdHeader);
  if (dbSysConfig & ChipSysEcEnable) {
    return TRUE;
  } else {
    return FALSE;
  }
}


/**
 * GetEfuseStatue - Get Efuse status
 *
 *
 * @param[in] Value - Return Chip strap status
 * @param[in] StdHeader
 *
 */
VOID
GetEfuseStatus (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    Or8;
  UINT8    Mask8;

  Or8 = BIT5;
  Mask8 = BIT5;
  LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC8), &Or8, &Mask8, StdHeader);
  LibAmdMemWrite (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD8), Value, StdHeader);
  LibAmdMemRead (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD8 + 1), Value, StdHeader);
  Or8 = 0;
  Mask8 = BIT5;
  LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGC8), &Or8, &Mask8, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * SbSleepTrapControl - SB Sleep Trap Control
 *
 *
 *
 * @param[in] SleepTrap    - Whether sleep trap is enabled
 *
 */
VOID
SbSleepTrapControl (
  IN        BOOLEAN          SleepTrap
  )
{
  if (SleepTrap) {
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT2;

    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) &= ~ (BIT5);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT1;
  } else {
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) |= BIT5;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT0;

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
  }
}
BOOLEAN
FchCheckZPB2 (
  )
{
  CPUID_DATA CpuId;
  UINT8 Family;
  UINT8 ExtFamily;
  UINT8 Model;
  UINT8 ExtModel;
  UINT8 Stepping;
  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  Family    = (UINT8) (CpuId.EAX_Reg >> 8) & 0xF;   // bit 11:8
  ExtFamily = (UINT8) (CpuId.EAX_Reg >> 20) & 0xFF; // bit 27:20
  Model     = (UINT8) (CpuId.EAX_Reg >> 4) & 0xF;   // bit 7:4
  ExtModel  = (UINT8) (CpuId.EAX_Reg >> 16) & 0xF;  // bit 19:16
  Stepping  = (UINT8) (CpuId.EAX_Reg >> 0) & 0xF;   // bit 3:0
  if ((Family == 0xF) && (ExtFamily == 0x8) && (ExtModel == 0x0) && (Model == 0x1) && (Stepping == 0x2)) {
    return TRUE;
  } else {
  	if ((Family == 0xF) && (ExtFamily == 0x9))	//hygon SOC
		return TRUE; 
	else	
    	return FALSE;
  }
}

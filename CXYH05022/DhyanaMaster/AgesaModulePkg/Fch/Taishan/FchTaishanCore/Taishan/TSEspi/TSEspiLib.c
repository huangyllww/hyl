/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch ESPI controller
 *
 * Init ESPI features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
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
#include "Filecode.h"
#define FILECODE FCH_TAISHAN_FCHTAISHANCORE_TAISHAN_TSESPI_TSESPILIB_FILECODE

#define MAX_ESPI_RETRY     100000          // 100ms

#define SET_CONFIGURATION  0
#define GET_CONFIGURATION  1
#define IN_BAND_RESET      2
#define PC_MSG_DOWN_STREAM 4
#define VM_DOWN_STREAM     5
#define OOB_DOWN_STREAM    6

#define SLAVE_REG_ID           0x04
#define SLAVE_GENERAL_CAPCFG   0x08
#define SLAVE_PC_CAPCFG        0x10
#define SLAVE_VW_CAPCFG        0x20
#define SLAVE_OOB_CAPCFG       0x30
#define SLAVE_FA_CAPCFG        0x40
#define SLAVE_MEC1701_SCRATCH_REG 0x80      // This is a scratch register of MEC1701 eSPI and does nothing.

//ESPIx00
#define DNCMD_STATUS       BIT3

//ESPIx2C Master Capability
#define MASTER_FA_SUPPORT                   BIT0
#define MASTER_OOB_SUPPORT                  BIT1
#define MASTER_VW_SUPPORT                   BIT2
#define MASTER_PERIPHERAL_SUPPORT           BIT3

//ESPIx68  Slave0 Configuration
#define SLAVE_FA_ENABLE                     BIT0
#define SLAVE_OOB_ENABLE                    BIT1
#define SLAVE_VW_ENABLE                     BIT2
#define SLAVE_PC_ENABLE             BIT3

//SLAVE offset 0x08   SLAVE_GENERAL_CAPCFG
#define SLAVE_FA_SUPPORT                    BIT3
#define SLAVE_OOB_SUPPORT                   BIT2
#define SLAVE_VW_SUPPORT                    BIT1
#define SLAVE_PERIPHERAL_SUPPORT            BIT0

/// eSPIx00 eSPI Software Specific Register 0
typedef union {
  struct {
    UINT32      SWCommandType:3;
    UINT32      CommandStatus:1;
    UINT32      SlaveNSelect:2;
    UINT32      Reserved_7_6:2;
    UINT32      AddrByte0PCycTypeVWCntOOBCycType:8;
    UINT32      AddrByte0VWIdxOOBPktByte1:8;
    UINT32      VWDataOOBPktByte2:8;
  } Field;
  UINT32 Value;
} ESPIx00_DN_DXHDR0;

/// eSPIx04 eSPI Software Specific Register 1
typedef union {
  struct {
    UINT32      VWIdxOOBPktbyte3:8;
    UINT32      VWDataOOBByteCnt:8;
    UINT32      VWIdxOOBPktByte5:8;
    UINT32      VWDataOOBPktByte6:8;
  } Field;
  UINT32 Value;
} ESPIx04_DN_DXHDR1;

/// eSPIx2C eSPI Master Capability
typedef union {
  struct {
    UINT32      FlashAccessChannelSupport:1;
    UINT32      OOBMessageChannelSupport:1;
    UINT32      VWChannelSupport:1;
    UINT32      PChannelSupport:1;
    UINT32      MasterVersion:3;
    UINT32      FlashAccessChannelMaxPayload:3;
    UINT32      OOBMessageChannelMaxPayload:3;
    UINT32      OperatingMaxVWCount:6;
    UINT32      PChannelMaxPayloadSize:3;
    UINT32      NumberOfSlave:3;
    UINT32      OperatingSupportFreq:3;
    UINT32      IOMode:2;
    UINT32      AlertMode:1;
    UINT32      CRCCheck:1;
  } Field;
  UINT32 Value;
} ESPIx2C_MASTER_CAP;

/// eSPIx68 eSPI Slave N Configuration
typedef union {
  struct {
    UINT32      FlashAccessChannelEnable:1;
    UINT32      OOBMessageChannelEnable:1;
    UINT32      VWChannelEnable:1;
    UINT32      PChannelEnable:1;
    UINT32      OOBValidBitEnable:1;
    UINT32      Reserved_24_5:20;
    UINT32      OperatingFreq:3;
    UINT32      IOModeSelect:2;
    UINT32      AlertMode:1;
    UINT32      CRCCheckingEnable:1;
  } Field;
  UINT32 Value;
} ESPIx68_SLAVE0_CONFIG;

///
/// Slave Registers
///
/// Offset 04h: Device Identification
typedef union {
  struct {
    UINT32      RO_VersionID:8;
    UINT32      Reserved_31_8:24;
  } Field;
  UINT32 Value;
} ESPI_SL04_DEVICE_ID;

/// Offset 08h: General Capabilities and Configurations
typedef union {
  struct {
    UINT32      RO_PCSupported:1;
    UINT32      RO_VWSupported:1;
    UINT32      RO_OOBMsgSupported:1;
    UINT32      RO_FASupported:1;
    UINT32      Reserved_7_3:4;
    UINT32      Reserved_11_8:4;
    UINT32      RO_MaxWaitStateAllowed:4;
    UINT32      RO_MaxFreqSupported:3;
    UINT32      RO_OpenDrainAlertSupported:1;
    UINT32      OperatingFreq:3;
    UINT32      OpenDrainAlertSelect:1;
    UINT32      RO_IOModeSupported:2;
    UINT32      IOModeSelect:2;
    UINT32      AlertMode:1;
    UINT32      Reserved_29:1;
    UINT32      ResponseModifierEn:1;
    UINT32      CRCCheckingEn:1;
  } Field;
  UINT32 Value;
} ESPI_SL08_SLAVE_GENERAL_CAPCFG;

/// Offset 10h: Channel 0 Capabilities and Configurations
typedef union {
  struct {
    UINT32      PCEn:1;
    UINT32      RO_PCReady:1;
    UINT32      BusMasterEn:1;
    UINT32      Reserved_3:1;
    UINT32      RO_PCMaxPayloadSizeSupported:3;
    UINT32      Reserved_7:1;
    UINT32      PCMaxPayloadSizeSelected:3;
    UINT32      Reserved_11:1;
    UINT32      PCMaxReadRequestSize:3;
    UINT32      Reserved_31_15:17;
  } Field;
  UINT32 Value;
} ESPI_SL10_SLAVE_PC_CAPCFG;

/// Offset 20h: Channel 1 Capabilities and Configurations
typedef union {
  struct {
    UINT32      VWEn:1;
    UINT32      RO_VWReady:1;
    UINT32      Reserved_7_2:6;
    UINT32      RO_MaxVWCntSupported:6;
    UINT32      Reserved_15_14:2;
    UINT32      OpMaxVWCnt:6;
    UINT32      Reserved_31_22:10;
  } Field;
  UINT32 Value;
} ESPI_SL20_SLAVE_VW_CAPCFG;

/// Offset 30h: Channel 2 Capabilities and Configurations
typedef union {
  struct {
    UINT32      OOBEn:1;
    UINT32      RO_OOBReady:1;
    UINT32      Reserved_3_2:2;
    UINT32      RO_MsgChMaxPayloadSizeSupported:3;
    UINT32      Reserved_7:1;
    UINT32      MsgChMaxPayloadSizeSelected:3;
    UINT32      Reserved_31_11:21;
  } Field;
  UINT32 Value;
} ESPI_SL30_SLAVE_OOB_CAPCFG;

/// Offset 40h: Channel 2 Capabilities and Configurations
typedef union {
  struct {
    UINT32      FAEn:1;
    UINT32      RO_FAReady:1;
    UINT32      FlashBlockEraseSize:3;
    UINT32      RO_ChMaxPayloadSizeSupported:3;
    UINT32      ChMaxPayloadSizeSelected:3;
    UINT32      RO_FlashSharingMode:1;
    UINT32      ChMaxReadReqSize:3;
    UINT32      Reserved_31_15:17;
  } Field;
  UINT32 Value;
} ESPI_SL40_SLAVE_FA_CAPCFG;


UINT32 getESPIBase ()
{
  UINT32 base;
  ReadPci ((LPC_BUS_DEV_FUN << 16) + 0xA0, AccessWidth32, &base, NULL);
  base &= 0xFFFFFFC0;
  base += 0x00010000;

  // eSPI Master Capability
  if ( ACPIMMIO32 (base + 0x2C) == 0 || ACPIMMIO32 (base + 0x2C) == 0xFFFFFFFF) {
    ASSERT (FALSE);
  }

  return base;
}
/*
void wait4stClear ()
{
    while(eSPIx00.Field.CommandStatus) {
        IoWrite16(0x80, 0xE291); // delay
        IoRead8(0x404);          // delay
    }
}
*/
VOID
wait4stClear (
  IN  UINT32     EspiBase
  )
{
  UINT32 Retry;

  for ( Retry = 0; Retry < MAX_ESPI_RETRY; Retry++ ) {
    if ( (ACPIMMIO32 (EspiBase + 0x00) & DNCMD_STATUS) == 0 ) {
      break;
	}
    //FchStall (2, NULL);
    IoWrite16(0x80, 0xE291); // delay
  }
}
/*
void eSPI_InBandRst (UINT32 slave)
{
    eSPIx00.Field.CommandStatus = 0;
    wait4stClear();
    eSPIx00.Field.SlaveNSelect = slave;
    eSPIx68.Field.OperatingFreq = 0;
    eSPIx00.Field.SWCommandType = IN_BAND_RESET;
    eSPIx00.Field.CommandStatus = 1;
    wait4stClear();
}
*/
VOID
TSeSPI_InBandRst  (
  IN  UINT32     EspiBase
  )
{
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)BIT3, 0x00);
  wait4stClear (EspiBase);
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)(BIT4 + BIT5), 0x00);
  RwMem (EspiBase + 0x68, AccessWidth32, 0xF1FFFFFF, 0x00);              // [27:25] CLK_FREQ_SEL
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFFFFFFF8, IN_BAND_RESET);     // [2:0] DNCMD_TYPE
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)BIT3, BIT3);
  wait4stClear (EspiBase);
}

/*
UINT32 eSPI_GetConfiguration(UINT32 slave, UINT16 offset)
{
    wait4stClear();
    eSPIx00.Field.SlaveNSelect = slave;
    eSPIx00.Field.SWCommandType = GET_CONFIGURATION;
    eSPIx00.Field.AddrByte0PCycTypeVWCntOOBCycType = ((offset >> 8) & 0xFF);
    eSPIx00.Field.AddrByte0VWIdxOOBPktByte1 = (offset & 0xFF);
    eSPIx04.Value = 0;
    eSPIx00.Field.CommandStatus = 1;
    wait4stClear();
    return eSPIx04.Value;
}
*/
UINT32
TSeSPI_GetConfiguration  (
  IN  UINT32     EspiBase,
  IN  UINT32     RegAddr
  )
{
  wait4stClear (EspiBase);
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32) (BIT4 + BIT5), 0x00);
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFFFFFFF8, GET_CONFIGURATION);     // [2:0] DNCMD_TYPE
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFF00FFFF, (RegAddr & 0xFF) << 16);
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFFFF00FF, RegAddr & 0xFF00);
  RwMem (EspiBase + 0x04, AccessWidth32, 0x00, 0x00);
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)BIT3, BIT3);
  wait4stClear (EspiBase);
  return ACPIMMIO32 (EspiBase + 0x04);
}

/*
void eSPI_SetConfiguration (UINT32 slave, UINT32 offset, UINT32 value)
{
    wait4stClear();
    eSPIx00.Field.SlaveNSelect = slave;
    eSPIx00.Field.SWCommandType = SET_CONFIGURATION;
    eSPIx00.Field.AddrByte0PCycTypeVWCntOOBCycType = ((offset >> 8) & 0xFF);
    eSPIx00.Field.AddrByte0VWIdxOOBPktByte1 = (offset & 0xFF);
    eSPIx04.Value = value;
    eSPIx00.Field.CommandStatus = 1;
    wait4stClear();
}
*/
VOID
TSeSPI_SetConfiguration  (
  IN  UINT32     EspiBase,
  IN  UINT32     RegAddr,
  IN  UINT32     Value
  )
{
  wait4stClear (EspiBase);
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32) (BIT4 + BIT5), 0x00);
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFFFFFFF8, SET_CONFIGURATION);     // [2:0] DNCMD_TYPE
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFF00FFFF, (RegAddr & 0xFF) << 16);
  RwMem (EspiBase + 0x00, AccessWidth32, 0xFFFF00FF, RegAddr & 0xFF00);
  RwMem (EspiBase + 0x04, AccessWidth32, 0x00, Value);
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)BIT3, BIT3);
  wait4stClear (EspiBase);
}

/**
 * FchinitTSEspiSlave0Device - Config ESPI controller and device
 *
 *  // It recommends software to do device initialization sequence as below:
 *  // 1.Set Master SlaveN Configuration register (MMIO_reg0x68)
 *  Clock Frequency to 16.7Mhz, then send In-Band Reset.
 *  // 2. Get_Configuration to read Slave Device Capability
 *  Register.
 *  // 3. Set_Configuration to write Slave Device General
 *  Configuration Register, such as CRC, IO mode, Alert mode,
 *  Clock Frequency.
 *  // 4. Change Master SlaveN Configuration registers
 *  (MMIO_reg0x68), make sure Master SlaveN's configuration is
 *  same as device.
 *  // 5. Get_Configuration to read Slave Device's each channel
 *  Capablity Register.
 *  // 6. Set_Configuration to write Slave Device's each channel
 *  Configuration Register.
 *  // 7. Get_Configuration to read Slave Device's each channel
 *  ready bit, wait for each channel ready.
 *  // 8. Enable Master SlaveN Configuration registers
 *  (MMIO_reg0x68) to enable each channel's enable bit.
 *
 * @param[in] EspiBase ESPI MMIO base.
 *
 */
VOID
FchinitTSEspiSlave0Device (
  IN  UINT32     EspiBase
  )
{
  ESPIx2C_MASTER_CAP              MasterCapibility;
  ESPIx68_SLAVE0_CONFIG           Slave0Config;
  ESPI_SL08_SLAVE_GENERAL_CAPCFG  DeviceCapCfg;
  ESPI_SL10_SLAVE_PC_CAPCFG       SlavePCCapCfg;
  ESPI_SL20_SLAVE_VW_CAPCFG       SlaveVWCapCfg;
  ESPI_SL30_SLAVE_OOB_CAPCFG      SlaveOOBCapCfg;
  ESPI_SL40_SLAVE_FA_CAPCFG       SlaveFACapCfg;

  //
  // 1, In-Band Reset
  //
  TSeSPI_InBandRst (EspiBase);

  //
  // 2 - 4, check and update Slave Device Capability and Slave0 configuration
  //
  MasterCapibility.Value = ACPIMMIO32 (EspiBase + 0x2C);
  DeviceCapCfg.Value     = TSeSPI_GetConfiguration (EspiBase, SLAVE_GENERAL_CAPCFG);
  Slave0Config.Value     = 0;
  //
  // Check support for each channel
  //
  if ( MasterCapibility.Field.FlashAccessChannelSupport && DeviceCapCfg.Field.RO_FASupported ) {
    Slave0Config.Field.FlashAccessChannelEnable = 1;
  }

  if ( MasterCapibility.Field.OOBMessageChannelSupport && DeviceCapCfg.Field.RO_OOBMsgSupported ) {
    Slave0Config.Field.OOBMessageChannelEnable = 1;
  }

  if ( MasterCapibility.Field.VWChannelSupport && DeviceCapCfg.Field.RO_VWSupported ) {
    Slave0Config.Field.VWChannelEnable = 1;
  }

  if ( MasterCapibility.Field.PChannelSupport && DeviceCapCfg.Field.RO_PCSupported ) {
    Slave0Config.Field.PChannelEnable = 1;
  }

  //
  // Operating Frequency - fix to 16.7MHz (000) for now
  //
  Slave0Config.Field.OperatingFreq = 0;
  DeviceCapCfg.Field.OperatingFreq = 0;

  //
  // I/O Mode Select:
  //
  if ( (MasterCapibility.Field.IOMode == 2) && (DeviceCapCfg.Field.RO_IOModeSupported & BIT1) ) {
    // Quad Mode
    Slave0Config.Field.IOModeSelect = 2;
    DeviceCapCfg.Field.IOModeSelect = 2;
  } else if ( (MasterCapibility.Field.IOMode == 1) && (DeviceCapCfg.Field.RO_IOModeSupported & BIT0) ) {
    // Dual Mode
    Slave0Config.Field.IOModeSelect = 1;
    DeviceCapCfg.Field.IOModeSelect = 1;
  } else {
    // single Mode
    Slave0Config.Field.IOModeSelect = 0;
    DeviceCapCfg.Field.IOModeSelect = 0;
  }

  //
  // Alert Mode
  //
  Slave0Config.Field.AlertMode = MasterCapibility.Field.AlertMode;
  DeviceCapCfg.Field.AlertMode = MasterCapibility.Field.AlertMode;

  //
  // CRC Checking
  //
  Slave0Config.Field.CRCCheckingEnable = MasterCapibility.Field.CRCCheck;
  DeviceCapCfg.Field.CRCCheckingEn     = MasterCapibility.Field.CRCCheck;

  //
  // update Device configuration and Slave0 configuration
  //
  TSeSPI_SetConfiguration (EspiBase, SLAVE_GENERAL_CAPCFG, DeviceCapCfg.Value);
  ACPIMMIO32 (EspiBase + 0x68) = (Slave0Config.Value & 0xFFFFFFF0);

  //
  // 5 - 8 Configure and enable channels
  //
  // FA Channel
  //
  if ( Slave0Config.Field.FlashAccessChannelEnable ) {
    SlaveFACapCfg.Value      = TSeSPI_GetConfiguration (EspiBase, SLAVE_FA_CAPCFG);
    SlaveFACapCfg.Field.FAEn = 1;
    TSeSPI_SetConfiguration (EspiBase, SLAVE_FA_CAPCFG, SlaveFACapCfg.Value);
    do {
      SlaveFACapCfg.Value = TSeSPI_GetConfiguration (EspiBase, SLAVE_FA_CAPCFG);
      IoWrite16(0x80, 0xE292); // delay
    } while (!SlaveFACapCfg.Field.RO_FAReady);
    ACPIMMIO32(EspiBase + 0x68) |= SLAVE_FA_ENABLE;
  }

  //
  // OOB Channel
  //
  if ( Slave0Config.Field.OOBMessageChannelEnable ) {
    SlaveOOBCapCfg.Value       = TSeSPI_GetConfiguration (EspiBase, SLAVE_OOB_CAPCFG);
    SlaveOOBCapCfg.Field.OOBEn = 1;
    TSeSPI_SetConfiguration (EspiBase, SLAVE_OOB_CAPCFG, SlaveOOBCapCfg.Value);
    do {
      SlaveOOBCapCfg.Value = TSeSPI_GetConfiguration (EspiBase, SLAVE_OOB_CAPCFG);
      IoWrite16 (0x80, 0xE292); // delay
    } while (!SlaveOOBCapCfg.Field.RO_OOBReady);
    ACPIMMIO32 (EspiBase + 0x68) |= SLAVE_OOB_ENABLE;
  }

  //
  // VW Channel
  //
  if ( Slave0Config.Field.VWChannelEnable ) {
    SlaveVWCapCfg.Value      = TSeSPI_GetConfiguration (EspiBase, SLAVE_VW_CAPCFG);
    SlaveVWCapCfg.Field.VWEn = 1;
    TSeSPI_SetConfiguration (EspiBase, SLAVE_VW_CAPCFG, SlaveVWCapCfg.Value);
    do {
      SlaveVWCapCfg.Value = TSeSPI_GetConfiguration (EspiBase, SLAVE_VW_CAPCFG);
      IoWrite16 (0x80, 0xE292); // delay
    } while (!SlaveVWCapCfg.Field.RO_VWReady);
    ACPIMMIO32 (EspiBase + 0x68) |= SLAVE_VW_ENABLE;
  }

  //
  // PC Channel
  //
  if ( Slave0Config.Field.PChannelEnable ) {
    SlavePCCapCfg.Value      = TSeSPI_GetConfiguration (EspiBase, SLAVE_PC_CAPCFG);
    SlavePCCapCfg.Field.PCEn = 1;
    TSeSPI_SetConfiguration (EspiBase, SLAVE_PC_CAPCFG, SlavePCCapCfg.Value);
    do {
      SlavePCCapCfg.Value = TSeSPI_GetConfiguration (EspiBase, SLAVE_PC_CAPCFG);
      IoWrite16 (0x80, 0xE292); // delay
    } while (!SlavePCCapCfg.Field.RO_PCReady);
    ACPIMMIO32 (EspiBase + 0x68) |= SLAVE_PC_ENABLE;
  }
}

/**
 * FchinitEspiIoMux - Config ESPI controller
 *
 *
 *
 */
VOID
FchinitTSEspiIoMux ()
{
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x75, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x76, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x77, AccessWidth8, 0x00, 0x01);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x78, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x79, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x7A, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x7B, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x81, AccessWidth8, 0x00, 0x00);
  RwMem (ACPI_MMIO_BASE + IOMUX_BASE + 0x85, AccessWidth8, 0x00, 0x00);

  // To enable dedicated Alert pin. (ESPI_ALERT)
  // D14F3x78[3,2] = 00b;
  // D14F3x78[9,  LDRQ0_PU_EN] = 1b; Reset 0;
  // D14F3x78[10, LDRQ0_PD_EN] = 0b; Reset 1;
  RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG78), AccessWidth32, 0xFFFFF9F3, 0x00000200, NULL);
}

/**
 * EspiEc0DisableLpcDecoding - Config ESPI controller
 *
 *
 */
VOID
EspiEc0DisableLpcDecoding ()
{
  RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG48), AccessWidth32, ~(UINT32)(BIT29 + BIT23 + BIT3), 0x00, NULL);          // 60h/64h, 6xxh
  RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG48), AccessWidth8, 0xFC, 0x00, NULL);                                      // 2E/2F, 4E/$F
}

/**
 * FchinitTSEspiEnableKbc6064 - Config ESPI controller
 *
 * @param[in] Enable Enable/disable ESPI 60h/64h decoding.
 */
VOID
FchinitTSEspiEnableKbc6064 (
  IN  BOOLEAN     Enable
  )
{
  UINT32     EspiBase;

  EspiBase = getESPIBase ();

  if ( Enable ) {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG48), AccessWidth32, ~(UINT32)BIT29, 0x00, NULL);          // disable LPC 60h/64h
    RwMem (EspiBase + 0x40, AccessWidth32, ~(UINT32)BIT1, BIT1);                                                // 60h/64h
  } else {
    RwMem (EspiBase + 0x40, AccessWidth32, ~(UINT32)BIT1, 0x00);                                                // 60h/64h
  }

}
/**
 * FchinitTSEspiTimer - Config ESPI controller
 *
 *
 *
 * @param[in] EspiBase ESPI MMIO base.
 *
 */
VOID
FchinitTSEspiTimer (
  IN  UINT32     EspiBase
  )
{
  // eSPIx30 [29:24, WaitStateCounter]     = 00h,
  //         [23:8,  WatchDogCounter]      = 0FFFh,
  //         [6:4,   Glb_Alink_Idle_Timer] = 111b - 2048 clocks
  RwMem (EspiBase + 0x30, AccessWidth32, 0xFF00008F, 0x000FFF70);
  RwMem (EspiBase + 0x30, AccessWidth32, ~(UINT32)BIT0, BIT0);                            // WDG_EN
  RwMem (EspiBase + 0x30, AccessWidth32, ~(UINT32)BIT1, BIT1);                            // WAIT_CHKEN.
}

/**
 * FchinitTSEspiEc0 - Config ESPI controller
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchinitTSEspiEc0 (
  IN  VOID     *FchDataPtr
  )
{
  UINT32     EspiBase;

  EspiBase = getESPIBase ();

  EspiEc0DisableLpcDecoding ();

  //6.1 eSPI Control Register Initialization
  //  1  Enable PCI Bus Watch Dog Timer
  //  2  Enable eSPI Bus Wait State Control Timer
  //  3  Enable eSPI clock gating function if neeed.
  //  4  Set each SlaveN Error Interrupt Enable.
  //  5  Set eSPI Controller ERROR interrupt Mapping, default is SMI.
  //  6  Set eSPI error protection bit if needed.
  //  7  Set each SlaveN Register command interrupt Enable.
  //  8  Set eSPI Register Command interrupt Mapping, default is SMI.
  //  9  Set each SlaveN IO Range, MMIO Range and decoding enable.
  //  10 Enable eSPI subtractive decoding if needed
  //  11 Assign IRQ0~IRQ23 to each SlaveN by setting each SlaveN IRQ Mask bit, IRQ plority.
  //  12 Enable Configurable Virtual Wire Index/Data Received Register.
  //  13 Set eSPI Bus Master Enable.

  //  eSPIx00[5:4, SlaveNSelect] = 00b - Slave 0 selected.
  //  Make sure the following operation is for slave 0
  RwMem (EspiBase + 0x00, AccessWidth32, ~(UINT32)(BIT4 + BIT5), 0x00);                   // Select Slave0

  FchinitTSEspiTimer (EspiBase);                                                            // 1 - 2

  //FchinitEspiClockGating (0, EspiBase);                                                 // 3
  //FchinitEspiErrCmdInterrupt (0, EspiBase);                                             // 4 - 8
  //FchinitEspiDecoding (0, EspiBase);                                                    // 9 - 10
  RwMem (EspiBase + 0x44, AccessWidth32, 0x00, 0x004E004F);                               // IO range
  RwMem (EspiBase + 0x48, AccessWidth32, 0x00, 0x004C0600);                               // IO range
  RwMem (EspiBase + 0x4C, AccessWidth32, 0x00, 0x00FF0000);                               // IO range size
  RwMem (EspiBase + 0x40, AccessWidth32, 0xFFFFF0FF, 0x00000F00);                         // Enable IO range
  RwMem (EspiBase + 0x40, AccessWidth32, ~(UINT32)BIT1, BIT1);                            // 60h/64h
  // CRCCheckingEnable/AlertMode; OOBValidBitEnable/PChannelEnable/VWChannelEnable/OOBMessageChannelEnable
  //RwMem (EspiBase + 0x68, AccessWidth32, 0x00, 0xC000001E);
  //FchinitEspiVWIrq (0, EspiBase);                                                       // 11 - 12
  RwMem (EspiBase + 0xA4, AccessWidth32, 0x00, 0x00040506);
  RwMem (EspiBase + 0xA8, AccessWidth32, 0x00, 0x00000007);

  RwMem (EspiBase + 0x34, AccessWidth32, ~(UINT32)BIT1, BIT1);                            // BUS_MASTER_EN

  RwMem (EspiBase + 0x6C, AccessWidth32, 0x00, 0xFFFFFFFF);                               // Enable interrupt at last - CZ

  FchinitTSEspiSlave0Device (EspiBase);
}


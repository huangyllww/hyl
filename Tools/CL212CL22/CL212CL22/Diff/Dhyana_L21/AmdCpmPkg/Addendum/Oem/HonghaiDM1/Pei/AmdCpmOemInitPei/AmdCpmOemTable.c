/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM OEM tables, and callback function.
 *
 * Contains code that defines OEM tables and callback function to override
 * OEM table on run time.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  OEM
 * @e \$Revision: 270847 $   @e \$Date: 2013-08-16 10:52:00 -0400 (Fri, 16 Aug 2013) $
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

#include <AmdCpmPei.h>
#include <AmdCpmBaseIo.h>
#include "AmdCpmOemInitPeim.h"

//
// OEM CPM Table Definition
//

//
// Platform Id Table: Get Board Id from SMBUS
//

// Read SMBUS to detect board ID
//BOARD_ID_BIT2  BOARD_ID_BIT1  BOARD_ID_BIT0    PCBA
//    0                      0                          0             Normal  REVA
//    0                      1                          0             SLT REVA
//    1                      0                          0             DAP REVA
AMD_CPM_PLATFORM_ID_TABLE2          gCpmPlatformIdTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GET_PLATFORM_ID2, sizeof (gCpmPlatformIdTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
//   UINT8  SmbusSelect;      ///< SMBUS Number
//   UINT8  SmbusAddress;     ///< SMBUS Address
//   UINT8  SmbusOffset;      ///< SMBUS Offset
//   UINT8  SmbusBit;         ///< SMBUS Bit
    0xFF,
  }
};

//
// Convert Table from Board Id to Platform Id
//

AMD_CPM_PLATFORM_ID_CONVERT_TABLE   gCpmPlatformIdConvertTable = {
  {CPM_SIGNATURE_GET_PLATFORM_ID_CONVERT, sizeof (gCpmPlatformIdConvertTable) / sizeof (UINT8), 0, 0, 0, 1},
  {
    0xFFFF,
  }
};


//
// Pre-Init Table
//
AMD_CPM_PRE_INIT_TABLE              gCpmPreInitTable = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PRE_INIT, sizeof (gCpmPreInitTable) / sizeof (UINT8), 0, 0, 0, 0x01},
  {
//  {UINT8  Type;     // Register type. 0: FCH MMIO. 1: PCI
//   UINT8  Select;   // Register sub-type
//   UINT8  Offset;   // Register offset
//   UINT8  AndMask;  // AND mask
//   UINT8  OrMask;   // OR mask
//   UINT8  Stage;    // Stage number},
    0xFF,
  }
};

//
// GPIO Init Table
//
AMD_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {  
   // (socket, die, gpio, function, output, pullup)
    GPIO_DEF_V2 (0, 0, 3,     GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // BIOS Recovery Pin
    GPIO_DEF_V2 (0, 0, 4,     GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // BMC_SMI_N_R
    GPIO_DEF_V2 (0, 0, 5,     GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // BMC_PRESENT_N_R
    GPIO_DEF_V2 (0, 0, 6,     GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // MEZZ Type Info BMC  //GR:Need confirm
    GPIO_DEF_V2 (0, 0, 8,     GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // CPLD INPUT
    GPIO_DEF_V2 (0, 0, 9,     GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // CPLD
    GPIO_DEF_V2 (0, 0, 10,    GPIO_FUNCTION_2,  GPIO_NA,          GPIO_PD_EN    ),   // CPLD
    GPIO_DEF_V2 (0, 0, 19,    GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // IIC3
    GPIO_DEF_V2 (0, 0, 20,    GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // IIC3
    GPIO_DEF_V2 (0, 0, 23,    GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // CPLD
    GPIO_DEF_V2 (0, 0, 40,    GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // CPLD
    GPIO_DEF_V2 (0, 0, 42,    GPIO_FUNCTION_1,  GPIO_INPUT,       GPIO_PU_PD_DIS),   // BMC Ready Pin
    GPIO_DEF_V2 (0, 0, 70,    GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // Clean BIOS Password
    GPIO_DEF_V2 (0, 0, 76,    GPIO_FUNCTION_0,  GPIO_OUTPUT_LOW,  GPIO_PU_EN    ),   // GPIO88 0:0, 1:1
    GPIO_DEF_V2 (0, 0, 85,    GPIO_FUNCTION_0,  GPIO_NA,          GPIO_PD_EN    ),   // FAN_OUT
    //GPIO_DEF_V2 (0, 0, 86,    GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // SYNC_FLOOD_N_R
    GPIO_DEF_V2 (0, 0, 88,    GPIO_FUNCTION_2,  GPIO_INPUT,       GPIO_PD_EN    ),   // Feild card 1: field card enable, 0: disable
    GPIO_DEF_V2 (0, 0, 90,    GPIO_FUNCTION_2,  GPIO_INPUT,       GPIO_PU_EN    ),   // 1:PHYA lane0-3 to SlotA 12-15, 0: PHYA lane0-3 to onboard device
    GPIO_DEF_V2 (0, 0, 91,    GPIO_FUNCTION_1,  GPIO_INPUT,       GPIO_PU_EN    ),   // 1:PHYA lane4-7 to SlotA 12-15, 0: PHYA lane4-7 to onboard device
    GPIO_DEF_V2 (0, 0, 92,    GPIO_FUNCTION_3,  GPIO_INPUT,       GPIO_PU_EN    ),   // SLOTA_PRSNT#
	GPIO_DEF_V2 (0, 0, 108,   GPIO_FUNCTION_2,  GPIO_OUTPUT_HIGH, GPIO_PD_EN    ),   // BIOS Complete
    //GPIO_DEF_V2 (0, 0, 113,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PU_PD_DIS),   // IIC2
    //GPIO_DEF_V2 (0, 0, 114,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PU_PD_DIS),   // IIC2
	GPIO_DEF_V2 (0, 0, 115,   GPIO_FUNCTION_1,  GPIO_INPUT,       GPIO_PD_EN    ),   // SLOTB_PRSNT#
	GPIO_DEF_V2 (0, 0, 116,   GPIO_FUNCTION_1,  GPIO_INPUT,       GPIO_PD_EN    ),   // SLOTC_PRSNT#
	GPIO_DEF_V2 (0, 0, 131,   GPIO_FUNCTION_3,  GPIO_INPUT,       GPIO_PU_EN    ),   // SD 0: exist SD, 1: no SD
	GPIO_DEF_V2 (0, 0, 132,   GPIO_FUNCTION_2,  GPIO_INPUT,       GPIO_PD_EN    ),   // CPLD
	GPIO_DEF_V2 (0, 0, 135,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // UART2 RXD //need confirm
	GPIO_DEF_V2 (0, 0, 137,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // UART2 TXD
	GPIO_DEF_V2 (0, 0, 138,   GPIO_FUNCTION_0,  GPIO_NA,          GPIO_PD_EN    ),   // UART0 TXD
	GPIO_DEF_V2 (0, 0, 140,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // UART3 TXD
	GPIO_DEF_V2 (0, 0, 142,   GPIO_FUNCTION_1,  GPIO_NA,          GPIO_PD_EN    ),   // UART3 RXD
	GPIO_DEF_V2 (0, 0, 143,   GPIO_FUNCTION_0,  GPIO_NA,          GPIO_PD_EN    ),   // UART1 TXD
    GPIO_DEF_V2 (0, 0, 147,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //PS-67 for xgbe SPF_SCL
    GPIO_DEF_V2 (0, 0, 148,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //PS-67 for xgbe SPF_SDA
	GPIO_DEF_V2 (0, 0, 95,    GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // CPLD
	GPIO_DEF_V2 (0, 0, 96,    GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // CPLD
	GPIO_DEF_V2 (0, 0, 97,    GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // Fild Card
	GPIO_DEF_V2 (0, 0, 98,    GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // Fild Card
	GPIO_DEF_V2 (0, 0, 99,    GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // Fild Card
	GPIO_DEF_V2 (0, 0, 100,   GPIO_FUNCTION_1,  GPIO_OUTPUT_LOW,     GPIO_PD_EN    ),   // Fild Card
	GPIO_DEF_V2 (0, 0, 131,   GPIO_FUNCTION_0,  GPIO_INPUT,          GPIO_PD_EN    ),   // CPLD
    0xFF,
  }
};

//
// GEVENT Init Table
//
AMD_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x00000001},
  {//                  GEvent EventEnable   SciTrigE      SciLevl         SmiSciEn        SciS0En         SciMap      SmiTrig       SmiControl
//    GEVENT_DEFINITION (0x03,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_03,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT03: PM_INT_IN
//    GEVENT_DEFINITION (0x05,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_05,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT05: LAN_MEDIA_SENSE
//    GEVENT_DEFINITION (0x08,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_08,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT08: PCIE_WAKE_UP#
//    GEVENT_DEFINITION (0x0C,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_12,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT12: USB_OC#
//    GEVENT_DEFINITION (0x0D,  EVENT_ENABLE, SCITRIG_HI,   SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_13,  SMITRIG_HI,   SMICONTROL_DISABLE), // GEVENT13: LAN_LOW_POWER
//    GEVENT_DEFINITION (0x0E,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_14,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT14: LAN_SMART#
//    GEVENT_DEFINITION (0x0F,  EVENT_ENABLE, SCITRIG_LOW,  SCILEVEL_EDGE,  SMISCI_DISABLE, SCIS0_DISABLE,  SCIMAP_15,  SMITRIG_LOW,  SMICONTROL_DISABLE), // GEVENT15: EVALCARD_ALERT#
    0xFF,
  }
};

//
// CPM GPIO Module
//

AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//                       DeviceId          Enable            Assert  Deassert  Hotplugs
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_GBE,    CPM_DEVICE_ON,    0,      0,        0), // GBE
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_BT,     CPM_DEVICE_ON,    0,      0,        0), // BT
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_WLAN,   CPM_DEVICE_ON,    0,      0,        0), // WLAN
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_PCIE_X16_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // PCIe 1x16/2x8 Switch
//    GPIO_DEVICE_DEFINITION (DEVICE_ID_SATAE_M2_SWITCH,   CPM_DEVICE_ON,    0,      0,        0), // SataExpress/M.2 Switch
    0xFF,
  }
};

//
// CPM Device Detection Table for Speedway
//
AMD_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId; // Device Id
//   UINT8  Type;     // Detection type. 0: One GPIO pin. 1: Two GPIO pins. 2: Special Pin
//   UINT16 PinNum1;  // Pin number of GPIO 1
//   UINT8  Value1;   // Value of GPIO 1
//   UINT16 PinNum2;  // Pin number of GPIO 2
//   UINT8  Value2;   // Value of GPIO 2
//   UINT16 PinNum3;  // Pin number of GPIO 3
//   UINT8  Value3;   // Value of GPIO 3},
    0xFF,
  }
};

//
// CPM Device Reset Table
//
AMD_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Reset mode     // 0: Reset Assert. // 1: Reset De-assert             // 2: Delay between Assert and Deassert
//   UINT8  Type;           // Register type  // 0: GPIO.         // 1: Special pin.if Mode = 0 or 1
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value
//   UINT8  InitFlag;       // Init flag in post},
    0xFF,
  }
};

//
// CPM GPIO Device Init Table (Power On/Off)
//
AMD_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
//  {UINT8  DeviceId;       // Device Id
//   UINT8  Mode;           // Device Power Mode. 1: Power On. 0: Power Off
//   UINT8  Type;           // Device Power Item. 0: Set GPIO. 1: Wait GPIO. 2: Add Delay
//   UINT32 ((UINT16)Pin + ((UINT8)Value << 16));                 // GPIO pin value or delay timer
//   UINT8  InitFlag;       // Init flag in post},
//    {DEVICE_ID_GBE,    CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 1),  0},
//    {DEVICE_ID_BT,     CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 0),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 0), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 0), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_OFF, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 0), 0},

//    {DEVICE_ID_GBE,    CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (GBE_POWER_CONTROL_REVA, 0),  0},
//    {DEVICE_ID_BT,     CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (BT_POWER_CONTROL_REVA, 1),   0},
//    {DEVICE_ID_WLAN,   CPM_POWER_ON,  CPM_POWER_SET,   CPM_GPIO_PIN (WLAN_POWER_CONTROL_REVA, 1), 0},
//    {DEVICE_ID_PCIE_X16_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (PCIE_X16_SWITCH, 1), 0},
//    {DEVICE_ID_SATAE_M2_SWITCH,   CPM_POWER_ON, CPM_POWER_SET,   CPM_GPIO_PIN (SATAE_M2_SWITCH, 1), 0},
    0xFF,
  }
};

//
// PCIE Clock Table
//
AMD_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableHonghaiDM1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableHonghaiDM1) / sizeof (UINT8), 0, 0, 0, 0x0F},
  {
//  {UINT8  ClkId;              // FCH PCIe Clock
//   UINT8  ClkReq;             // FCH PCIe ClkReq
//   UINT8  ClkIdExt;           // External Clock Source
//   UINT8  ClkReqExt;          // External ClkReq
//   UINT8  DeviceId;           // Device Id. No Device Id if 0xFF
//   UINT8  Device;             // Device Number of PCIe bridge
//   UINT8  Function;           // Function Number of PCIe bridge
//   UINT8  SlotCheck;          // Slot Check Flag: // BIT0: Check PCI Space // BIT1: Check GPIO pin // BIT2: Check Clock Power Management Enable // BIT3~6: Reserved // BIT7: Change PCIe Clock in ACPI method
//   UINT32 SpecialFunctionId;  // Id of Special Function}
    // GPP_CLK0 clock for PCIE X8 slot is controlled by CLK_REQ0_L. For Bristol, this slot is not used
//    {GPP_CLK0,            CLK_ENABLE,  SRC_CLK4,  CLK_ENABLE,  DEVICE_ID_PCIE_X8_SLOT,  0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK1 clock for SSD is controlled by CLK_REQ1_L
//    {GPP_CLK1,            CLK_REQ1,    SRC_CLK8,  CLK_REQ8,    DEVICE_ID_M2_CONNECT,    0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK2 clock for Promontory chip is always on
//    {GPP_CLK2,            CLK_ENABLE,  SRC_CLK6,  CLK_ENABLE,  DEVICE_ID_PROMONTORY,    0, 0, NON_SLOT_CHECK,            0},
    // GFX_CLK clock for PCIE X16 slot is controlled by CLK_REQG_L. For Bristol, this slot is used as x8
//    {GPP_CLK9,            CLK_REQGFX,  SRC_CLK2,  CLK_REQ2,    DEVICE_ID_PCIE_X16_SLOT, 0, 0, NON_SLOT_CHECK,            0},
    // GPP_CLK3 clock for PCIE X4 slot is always on
//    {GPP_CLK3,            CLK_ENABLE,  SRC_CLK10, CLK_ENABLE,  DEVICE_ID_PCIE_X4_SLOT,  0, 0, NON_SLOT_CHECK,            0},
//    {CPM_CLKID_PCIE_RCLK, CLK_DISABLE, SRC_SKIP,  0,           0,                       0, 0, NON_SLOT_CHECK,            0},
    0xFF,
  }
};



//
// CPM DXIO Topology Table Socket 0
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHonghaiDM1S0 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHonghaiDM1S0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//  { UINT32  Flags;                    /**< Descriptor flags
//    #define DXIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId) {mType, mHotplug, 0, mStartLane, mEndLane, mGpioGroupId, 0, 0, 0}
//    #define DXIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mAspmL1_1, mAspmL1_2,  mClkPmSupport) \
//            {mPortPresent, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mAspmL1_1, mAspmL1_2, 0, mHotplug, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, DxioEndpointDetect}

  { // P_GPP0 - x2 PCIe Device, 0-1: x2 I350 2:BMC 3: x1 SLOTC
    // I350
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 1, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    1,                                      // Requested Device
    3,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  },
  
  { // P_GPP0 - x1 PCIe Device, 0-1: x2 I350 2:BMC 3: x1 SLOTC
    // BMC
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 2, 2, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    1,                                      // Requested Device
    5,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  },
  
  { // P_GPP0 - x1 PCIe Device, 0-1: x2 I350 2:BMC 3: x1 SLOTC
    // SlotC
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 3, 3, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    1,                                      // Requested Device
    6,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  },
  
  // 1x4 PCIE
  { // GPIO90 is hight, x4 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 3, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    0,                                      // Requested Device
    0,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  },
  
  // 4x1 SATA
  { // GPIO88 && GPIO92 is high SATA x4 ports
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_SATA (
    DxioPortEnabled                       // Port Present
    )
  },
  
  // 4x1 SATA
  { // S0D0P0 - x1 SATA x4 ports
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 4, 7, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_SATA (
    DxioPortEnabled                       // Port Present
    )
  },
 // 4x1 XGBE
  // GPIO88 && GPIO92 is high, 4 x1 XGBE 
  { // P0 - EtherNet Port 0
      0,
      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
      DXIO_PORT_DATA_INITIALIZER_ENET (
      DxioPortEnabled,                      // Port Present
      0, /// mPortNum
      8, /// mPlatConf   8
                                                                                   /*< Platform Config
                                                                                   *  0 = Reserved
                                                                                   *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                   *  2 = 2.5G BackPlane
                                                                                   *  3= Soldered down 1000Base-T
                                                                                   *  4 = Soldered down 1000Base-X
                                                                                   *  5 = Soldered down NBase-T
                                                                                   *  6 = Soldered down 10GBase-T
                                                                                   *  7 = Soldered down 10GBase-r
                                                                                   *  8 = SFP+ Connector
                                                                                   */
       0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
       0xB, /// mSuppSpeed   0xB
                                                                                    /*< Supported Speeds by Platform             !!!!!!!
                                                                                    *  1 = 100M Supported
                                                                                    *  2 = 1G Supported
                                                                                    *  4 = 2.5G Supported
                                                                                    *  8 = 10G Supported
            */
       1, /// mConnType   1
                                                                                    /*< Supported Speeds by Platform
                                                                                    *  0 = Port not Used
                                                                                    *  1 = SFP+ Connection I2C interface
                                                                                    *  2 = MDIO PHY
                                                                                    *  4 = Backplane Connection     !!!!!
                                                                                    */
       0, /// mMdioReset   0
                                                                                    /*< MDIO Reset Type   //PS-67
                                                                                    *  0 = None
                                                                                    *  1 = I2C GPIO
                                                                                    *  2 = Integrated GPIO
                                                                                    *  3 = Reserved
                                                                                    *  If an external MDIO PHY is used, this field 
                                                                                    *  indicates the method used by the software 
                                                                                    *  driver to control for the reset to that PHY
                                                                                    *  11b = Reserved
                                                                                    *  10b = Integrated GPIO
                                                                                    *  01b = I2C GPIO
                                                                                    *  00b = None
                                                                                    */
       0, /// mMdioGpioResetNum
                                                                                    /*
                                                                                     * If MDIO Reset Type is set to 10b, this field 
                                                                                     * indicates the Ethernet integrated GPIO number 
                                                                                     * used to control the reset.
                                                                                     */
       3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                     /*
                                                                                      * If an external MDIO PHY is used in 
                                                                                      combination with an I2C GPIO reset, this field 
                                                                                      indicates the lower I2C address of the 
                                                                                      PCA9535 GPIO expander used to drive the reset.
                                                                                      If this port is used as part of an SFP+ 
                                                                                      connection, this field indicates the lower I2C 
                                                                                      address of the PCA9535 GPIO expander used 
                                                                                      to drive the SFP+ sideband signals
                                                                                      */
       0x2, /// mTxFault,   0xE
                                                                                    /*
                                                                                      * Set this field to the PCA9535 GPIO number 
                                                                                      used to control either the SFP+ TX_FAULT 
                                                                                      signal or the MDIO reset associated with this 
                                                                                      port. If neither of those features are in use, set 
                                                                                      this to 0.
                                                                                      */
       0, /// mRs,
                                                                                    /*
                                                                                     * Set this field to the PCA9535 GPIO number 
                                                                                      used to control the SFP+ RS signal. If this port 
                                                                                      is not used with an SFP+ connector, set this 
                                                                                      field to 0.
                                                                                      */
       0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                    //  set this field to 0.
                                                                                   
       0x1, /// mRxLoss,    0xD      
                                                                                    /*
                                                                                     * Set this field to the PCA9535 GPIO number 
                                                                                      used to control the SFP+ Rx_LOS signal. If 
                                                                                      this port is not used with an SFP+ connector, 
                                                                                      set this field to 0.
                                                                                      */
       2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
       0x0, /// mSfpMux,                                        //PS-67  On field card, PCA9545  A1A0 = 2b10
                                                                                /*
                                                                                 * Set this field to indicate the lower address of 
                                                                                  the PCA9545 I2C multiplexer used to drive the
                                                                                  SFP+ connector. If no multiplexing is used, set 
                                                                                  this field to 111b. If this port is not used with 
                                                                                  an SFP+ connector, set this field to 0.
                                                                                  000b-011b = PCA9545 lower address
                                                                                  100b-110b = Reserved
                                                                                  111b = SFP+ directly connected to I2C
                                                                                 * */
       0, /// mSfpBusSeg,
                                                                                 /*
                                                                                 * Set this field to indicate the downstream 
                                                                                  channel of the PCA9545 I2C multiplexer used 
                                                                                  to drive the SFP+ connector. If no multiplexing
                                                                                  is used, set this field to 111b. If this port is not 
                                                                                  used with an SFP+ connector, set this field to 
                                                                                  0.
                                                                                  000b-011b = PCA9545 lower address
                                                                                  100b-110b = Reserved
                                                                                  111b = SFP+ directly connected to processor 
                                                                                  I2C*/
       0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                /*
                                                                                 * Set this field to indicate the upper address of 
                                                                                  the PCA9545 I2C multiplexer used to drive the
                                                                                  SFP+ connector. PCA9545 comes in 3 variants 
                                                                                  each with a different upper address. 
                                                                                  11100b = PCA9545A
                                                                                  11010b = PCA9545B
                                                                                  10110b = PCA9545C
                                                                                  11111b = SFP+ directly connected to processor
                                                                                  I2C
                                                                                  If no multiplexing is used, set this field to 
                                                                                  11111b. If this port is not used with an SFP+ 
                                                                                  connector, set this field to 0.
                                                                                  */
       0x40, /// mRedriverAddress, 0x40
                                                                                    /*
                                                                                     * This field encodes the I2C or MDIO address 
                                                                                      used to control a platform level redriver 
                                                                                      connected to this port. If no redriver is used, 
                                                                                      set this field to 0. If the redriver is connected 
                                                                                      using MDIO, the upper 2 address bits must be 
                                                                                      set to 0.
                                                                                    */
       1, /// mRedriverInterface,  1
                                                                                    /*
                                                                                     * This field encodes the I2C or MDIO address 
                                                                                      used to control a platform level redriver 
                                                                                      connected to this port. If no redriver is used, 
                                                                                      set this field to 0. If the redriver is connected 
                                                                                      using MDIO, the upper 2 address bits must be 
                                                                                      set to 0.
                                                                                      */
       0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
       0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
       0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
       0x7, /// mSlotMux,        //
                                                                                           /*
                                                                                            * Set this field to indicate the lower address of 
                                                                                              the PCA9545 I2C multiplexer used to drive the
                                                                                              slot which FieldCard inserted . If no multiplexing 
                                                                                              is used, set this field to 111b.
                                                                                              000b-011b = PCA9545 lower address
                                                                                              100b-110b = Reserved
                                                                                              111b = No Slot Mux used
                                                                                             * */
        3, /// mSfpBusSeg,
                                                                                             /*
                                                                                              * Set this field to indicate the downstream 
                                                                                                channel of the PCA9545 I2C multiplexer used 
                                                                                                to drive the slot which FieldCard inserted. 
                                                                                                If no multiplexing is used, set this field 
                                                                                                to 111b. 
                                                                                                000b-011b = PCA9545 lower address
                                                                                                100b-110b = Reserved
                                                                                                111b = SFP+ directly connected to processor 
                                                                                                I2C*/
         0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                               /*
                                                                                                * Set this field to indicate the upper address of 
                                                                                                  the PCA9545 I2C multiplexer used to drive the
                                                                                                  slot which FieldCard inserted. PCA9545 comes
                                                                                                  in 3 variants ,each with a different upper address. 
                                                                                                  11100b = PCA9545A
                                                                                                  11010b = PCA9545B
                                                                                                  10110b = PCA9545C
                                                                                                  11111b = SFP+ directly connected to processor I2C

                                                                                                  If no multiplexing is used, set this field to 
                                                                                                   11111b. If this port is not used with an SFP+ 
                                                                                                  connector, set this field to 0.
                                                                                                   */
       0x4,///mInfoRomAdd,At24c08d low i2c address
       0x14,///mInfoRomAddUP,At24c08d up i2c address
       1, ///InfoRomLocation,0=on board,1=field card
       4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
       0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
       0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
       0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
       0, /// mTxEqPre,  TBD
       0, /// mTxEqMain, TBD    0x28
       0  /// mTxEqPost  TBD
       )
   },
   { // P0 - EtherNet Port 1
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_ENET (
         DxioPortEnabled,                      // Port Present
         1, /// mPortNum
         8, /// mPlatConf   8
         0, /// mMdioId
         0xB, /// mSuppSpeed   0xB
         1, /// mConnType   1
         0, /// mMdioReset   0
         0, /// mMdioGpioResetNum
         3, /// mSfpGpioAdd   6
         6, /// mTxFault,   
         0, /// mRs,
         4, /// mModAbs,
         5, /// mRxLoss,
         2, /// mSfpGpioMask,  2
         0, /// mSfpMux,
         1, /// mSfpBusSeg,
         0x1C, /// mSfpMuxUpAdd,  0x1C
         0x40, /// mRedriverAddress, 0x40
         1, /// mRedriverInterface,  1
         1, /// mRedriverLane,
         0, /// mRedriverModel,
         0, /// mRedriverPresent,  1
         7, /// mSlotMux
         3, /// mSlotBusSeg
         0x1c, ///mSlotMuxUpAdd
         0x4,///mInfoRomAdd,At24c08d low i2c address
         0x14,///mInfoRomAddUP,At24c08d up i2c address
         1, ///InfoRomLocation,0=on board,1=field card
         4, /// mPadMux0,  4
         0, /// mPadMux1,
         0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
         0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
         0, /// mTxEqPre,  TBD
         0, /// mTxEqMain, TBD    0x28
         0  /// mTxEqPost  TBD
       )
   },
   { // P0 - EtherNet Port 2
         0,
         DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
         DXIO_PORT_DATA_INITIALIZER_ENET (
           DxioPortEnabled,                      // Port Present
           2, /// mPortNum
           8, /// mPlatConf   8
           0, /// mMdioId
           0xB, /// mSuppSpeed   0xB
           1, /// mConnType   1
           0, /// mMdioReset   0
           0, /// mMdioGpioResetNum
           3, /// mSfpGpioAdd   6
           0xA, /// mTxFault,   0xE
           0, /// mRs,
           8, /// mModAbs,   0xC
           9, /// mRxLoss,    0xD
           2, /// mSfpGpioMask,  2
           0, /// mSfpMux,
           2, /// mSfpBusSeg,
           0x1C, /// mSfpMuxUpAdd,  0x1C
           0x40, /// mRedriverAddress, 0x40
           1, /// mRedriverInterface,  1
           2, /// mRedriverLane,
           0, /// mRedriverModel,
           0, /// mRedriverPresent,  1
           7, /// mSlotMux
           3, /// mSlotBusSeg
           0x1c, ///mSlotMuxUpAdd
           0x4,///mInfoRomAdd,At24c08d low i2c address
           0x14,///mInfoRomAddUP,At24c08d up i2c address
           1, ///InfoRomLocation,0=on board,1=field card
           4, /// mPadMux0,  4
           0, /// mPadMux1,
           0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
           0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
           0, /// mTxEqPre,  TBD
           0, /// mTxEqMain, TBD    0x28
           0  /// mTxEqPost  TBD
         )
   },
   { // P0 - EtherNet Port 3
         0,
         DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
         DXIO_PORT_DATA_INITIALIZER_ENET (
           DxioPortEnabled,                      // Port Present
           3, /// mPortNum
           8, /// mPlatConf   8
           0, /// mMdioId
           0xB, /// mSuppSpeed   0xB
           1, /// mConnType   1
           0, /// mMdioReset   0
           0, /// mMdioGpioResetNum
           3, /// mSfpGpioAdd   6
           0xE, /// mTxFault,  
           0, /// mRs,
           0xC, /// mModAbs,   
           0xD, /// mRxLoss,    
           0, /// mSfpGpioMask,  2
           0, /// mSfpMux,
           3, /// mSfpBusSeg,
           0x1C, /// mSfpMuxUpAdd,  0x1C
           0x40, /// mRedriverAddress, 0x40
           1, /// mRedriverInterface,  1
           3, /// mRedriverLane,
           0, /// mRedriverModel,
           0, /// mRedriverPresent,  1
           7, /// mSlotMux
           3, /// mSlotBusSeg
           0x1c, ///mSlotMuxUpAdd
           0x4,///mInfoRomAdd,At24c08d low i2c address
           0x14,///mInfoRomAddUP,At24c08d up i2c address
           1, ///InfoRomLocation,0=on board,1=field card
           4, /// mPadMux0,  4
           0, /// mPadMux1,
           0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
           0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
           0, /// mTxEqPre,  TBD
           0, /// mTxEqMain, TBD    0x28
           0  /// mTxEqPost  TBD
       )
  },      
 
  // 4x1 PCIE
  { // GPIO91 is hight, x4 slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    1,                                      // Requested Device
    2,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  },
  
  // 1x8 PCIE
  { // GPIO88 is low && GPIO92 is low, P_GPP0 - x8 SLOTC
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 7, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    0,                                      // Requested Device
    0,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  }, 
  
  // 1x8 PCIE
  { // P_GPP0 - x8 SLOTC
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    0,                                      // Requested Device
    0,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  }, 
  
  // 1x16 PCIE
  { // GPIO88 is low && GPIO92 is high, P_GPP0 - x16 SLOTC
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 15, DxioHotplugDisabled, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
    DxioPortEnabled,                        // Port Present
    0,                                      // Requested Device
    0,                                      // Requested Function
    DxioHotplugDisabled,                    // Hotplug
    DxioGenMaxSupported,                    // Max Link Speed
    DxioGenMaxSupported,                    // Max Link Capability
    DxioAspmDisabled,                          // ASPM
    DxioAspmDisabled,                       // ASPM L1.1 disabled
    DxioAspmDisabled,                       // ASPM L1.2 disabled
    DxioClkPmSupportDisabled                // Clock PM
    )
  }, 

  // 2x8 PCIE
  { // P_GFX[24:27] - x8 PCIe Slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 16, 23, DxioHotplugServerExpress, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugServerExpress,             // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed
      DxioGenMaxSupported,                  // Max Link Capability
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  
  // 2x8 PCIE
  { // P_GFX[24:27] - x8 PCIe Slot
    0,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 24, 31, DxioHotplugServerExpress, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      0,                                    // Requested Device
      0,                                    // Requested Function
      DxioHotplugServerExpress,             // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed
      DxioGenMaxSupported,                  // Max Link Capability
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  },
  
  // 1x16 PCIE
  { // P_GFX[24:27] - x16 PCIe Slot
    DESCRIPTOR_TERMINATE_LIST,
    DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 16, 31, DxioHotplugServerExpress, 1),
    DXIO_PORT_DATA_INITIALIZER_PCIE (
      DxioPortEnabled,                      // Port Present
      3,                                    // Requested Device
      1,                                    // Requested Function
      DxioHotplugServerExpress,             // Hotplug
      DxioGenMaxSupported,                  // Max Link Speed
      DxioGenMaxSupported,                  // Max Link Capability
      DxioAspmDisabled,                        // ASPM
      DxioAspmDisabled,                     // ASPM L1.1 disabled
      DxioAspmDisabled,                     // ASPM L1.2 disabled
      DxioClkPmSupportDisabled              // Clock PM
    )
  }
  } // End of DXIO_PORT_DESCRIPTOR  
};

/*----------------------------------------------------------------------------------------*/
/**
 * Detect PCIe Devices Automatically
 *
 * This function patches PCIe Topology Override Table by detecting PCIe devices
 * automatically.
 *
 * @param[in]     AmdCpmTablePpi                Pointer to AMD CPM Table PPI
 * @param[in]     PcieTopologyTablePtr          Pointer to PCIe Topology Table
 * @param[in]     BoardId                       Current Board ID
 *
 */
VOID
DetectPcieDevices (
  IN       AMD_CPM_TABLE_PPI            *AmdCpmTablePpi,
  IN       AMD_CPM_PCIE_TOPOLOGY_TABLE  *PcieTopologyTablePtr,
  IN       UINT16                       BoardId
  )
{

}

/*----------------------------------------------------------------------------------------*/
/**
 * CPM Override Function After AMD CPM Table PPI
 *
 * This function updates CPM OEM Tables according to setup options or the value to be detected
 * on run time after AMD CPM Table PPI is installed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Function initialized successfully
 * @retval        EFI_ERROR      Function failed (see error for more details)
 *
 */
EFI_STATUS
EFIAPI
AmdCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;

  return Status;
}

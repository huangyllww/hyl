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
#include "HygonCpmOemInitPeim.h"

#define LANE40 40
#define LANE80 80
#define LANE112 112
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
// GPIO Init Table for Hygon62DB32SL1r2
//
AMD_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//        (socket,die,gpio,function,         output,           pullup)
  //
  //Socket 0:
  //
      GPIO_DEF_V2 (0, 0, 89,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //  PSP_INTR_0
      GPIO_DEF_V2(0, 0, 3  , GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // BMC_READY_R_N
      GPIO_DEF_V2(0, 0, 4  , GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // FM_SMI_ACTIVE_R_N
      GPIO_DEF_V2(0, 0, 5  , GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // PCIE_SLOT5_PRSNT_N
      GPIO_DEF_V2(0, 0, 70 , GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PD_EN    ),  // CLR_PASSWORD_N
      GPIO_DEF_V2(0, 0, 135, GPIO_FUNCTION_2, GPIO_INPUT      , GPIO_PD_EN    ),  // BIOS_CFG_ID1
      GPIO_DEF_V2(0, 0, 136, GPIO_FUNCTION_1, GPIO_INPUT      , GPIO_PD_EN    ),  // UART0_P0_R_RXD
      GPIO_DEF_V2(0, 0, 139, GPIO_FUNCTION_1, GPIO_INPUT      , GPIO_PU_EN    ),  // BIOS_CFG_ID0
	//Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA
    //CPU0 AGPIO88 PEDET_M2_1_N        
    GPIO_DEF_V2(0, 0, 88, GPIO_FUNCTION_2, GPIO_INPUT      , GPIO_PU_EN    ),  // PEDET_M2_1_N
       //for Riser
      GPIO_DEF_V2(0, 1, 6, GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // PCIE_SLOT4_RISER_TYPE_N
      GPIO_DEF_V2(0, 1, 3, GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // NVME_CARD_DET_N
      GPIO_DEF_V2(0, 0, 137, GPIO_FUNCTION_2, GPIO_OUTPUT_HIGH, GPIO_PU_EN    ),  // BIOS_POST_COMPLT_N
      GPIO_DEF_V2(0, 0, 151  , GPIO_FUNCTION_0, GPIO_INPUT      , GPIO_PU_EN    ),  // BIOS_RECOVERY#
    0xFF,
  }
};

//
// GEVENT Init Table
//
AMD_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x00000001},
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

AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
// CPM Device Detection Table for Hygon62DB32SL1r2
//
AMD_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableHygon62DB32SL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0F},
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
// CPM DXIO Topology Table Socket 0 with field card in slot1
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon62DB32SL1r2S0 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHygon62DB32SL1r2S0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
      { //S0D0P0 - x1 J54
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 0, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          1,                                    // Requested Device
          1,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                              // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                     // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },

     { //S0D0P0 - x1 BMC
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 1, 1, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         2,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen1,                               // Max Link Speed
         DxioGen1,                               // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
     //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA>>>     
          { //S0D0P0 - x2 M2 CARD J29-01
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 2, 2, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              1,                                    // Requested Device
              3,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGen3,                               // Max Link Speed
              DxioGen3,                              // Max Link Capability
              DxioAspmDisabled,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },     
   //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA <<<  
     { //S0D0P0 SATA ODD J116
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 2, 2, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_SATA (
             DxioPortEnabled                       // Port Present
           )
     },
//Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA >>>     
     { //S0D0P0 - x2 M2 CARD J29-02
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 3, 3, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         7,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                            // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },     
//Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA <<<
     
     { //S0D0P0 SATA M2 J29
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 3, 3, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_SATA (
             DxioPortEnabled                       // Port Present
           )
     },
//<duanjun 20200808 j27 support hotplug +>
     { //S0D0P0 - x4 NVME
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugServerExpress, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         6,                                    // Requested Function
         DxioHotplugServerExpress,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                               // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
//<duanjun 20200808 j27 support hotplug ->     
     { //S0D0P0 SATA HDDMINISAS
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 4, 7, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_SATA (
             DxioPortEnabled                       // Port Present
           )
     },
     
     { //S0D0P0 - x2 M2 CARD J29
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioUnusedEngine, 8, 9, DxioHotplugDisabled, 1),  //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         3,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                          // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
     
     { //S0D0P0 - x2 NIC
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 10, 11, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         4,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen2,                               // Max Link Speed
         DxioGen2,                              // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
     
     { //S0D0P0 - x4 J31
       0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)	   
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 15, 12, DxioHotplugDisabled, 1),
#else
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugDisabled, 1),
#endif	   
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         5,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                           // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
     
     { //S0D1P1 - x8 OCP J32
       0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)	   
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 87, 80, DxioHotplugDisabled, 1),
#else
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48, 55, DxioHotplugDisabled, 1),
#endif	   
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         0,                                    // Requested Device
         0,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                            // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
    
     {
       DESCRIPTOR_TERMINATE_LIST,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)	   
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 88, 95, DxioHotplugDisabled, 1),
#else
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 56, 63, DxioHotplugDisabled, 1),
#endif	   
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         0,                                    // Requested Device
         0,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                            // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     }                    
  }   // End of DXIO_PORT_DESCRIPTOR
};

//
// CPM DXIO Topology Table
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon62DB32SL1r2S1 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY_S1, sizeof (gCpmDxioTopologyTableHygon62DB32SL1r2S1) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  1,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//  { UINT32  Flags;                    /**< Descriptor flags
//    #define DXIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId) {mType, mHotplug, 0, mStartLane, mEndLane, mGpioGroupId, 0, 0, 0}
//    #define DXIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mAspmL1_1, mAspmL1_2,  mClkPmSupport) \
//            {mPortPresent, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mAspmL1_1, mAspmL1_2, 0, mHotplug, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, DxioEndpointDetect}

      { //S1D0P0 X8 J35
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 7, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S1D0P0 X8 J36
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      //Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME >>>
      
      { //S1D1P1 X16 J34
        0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)		
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 95, DxioHotplugDisabled, 1),
#else
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48, 63, DxioHotplugDisabled, 1),
#endif		
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                                  // Max Link Speed
          DxioGen3,                       // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S1D1P1 X16 J34.1
          0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)		  
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 83, DxioHotplugServerExpress, 1),
#else
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48, 51, DxioHotplugServerExpress, 1),
#endif		  
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                             // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.2
          0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)          
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 84, 87, DxioHotplugServerExpress, 1),
#else
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 52, 55, DxioHotplugServerExpress, 1),
#endif        
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                                // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.3
          0,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)          
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 88, 91, DxioHotplugServerExpress, 1),
#else
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 56, 59, DxioHotplugServerExpress, 1),
#endif        
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                            // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.4
            DESCRIPTOR_TERMINATE_LIST,
#if defined(Hygon52D16_4DIECPU_SUPPORT)&&(Hygon52D16_4DIECPU_SUPPORT==1)			
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 92, 95, DxioHotplugServerExpress, 1),
#else
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 60, 63, DxioHotplugServerExpress, 1),
#endif			
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGen3,                               // Max Link Speed
              DxioGen3,                           // Max Link Capability
              DxioAspmDisabled,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
      }
  }   // End of DXIO_PORT_DESCRIPTOR
};

//Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME <<<

//<duanjun-20200427-support 5xx cpu && 7xx cpu +>
//
// CPM DXIO Topology Table Socket 0 with field card in slot1
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon62DB32SL1r2S0_4DIE = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHygon62DB32SL1r2S0_4DIE) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
       { //S0D0P0 - x1 J54
         0,
         DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 0, DxioHotplugDisabled, 1),
         DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port Present
           1,                                    // Requested Device
           1,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGen3,                               // Max Link Speed
           DxioGen3,                             // Max Link Capability
           DxioAspmDisabled,                     // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
         )
       },

      { //S0D0P0 - x1 BMC
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 1, 1, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          1,                                    // Requested Device
          2,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen1,                               // Max Link Speed
          DxioGen1,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },

     //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA >>>     
          { //S0D0P0 - x2 M2 CARD J29-01
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 2, 2, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              1,                                    // Requested Device
              3,                                    // Requested Function
              DxioHotplugDisabled,                  // Hotplug
              DxioGen3,                               // Max Link Speed
              DxioGen3,                             // Max Link Capability
              DxioAspmDisabled,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
          },     
     
     { //S0D0P0 SATA ODD J116
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 2, 2, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_SATA (
             DxioPortEnabled                       // Port Present
           )
     },
    
     { //S0D0P0 - x2 M2 CARD J29-02
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 3, 3, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         7,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                        // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },     
//Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA <<<
      
      { //S0D0P0 SATA M2 J29
            0,
            DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 3, 3, DxioHotplugDisabled, 1),
            DXIO_PORT_DATA_INITIALIZER_SATA (
              DxioPortEnabled                       // Port Present
            )
      }, 
     { //S0D0P0 - x4 NVME
       0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugDisabled, 1),
       DXIO_PORT_DATA_INITIALIZER_PCIE (
         DxioPortEnabled,                      // Port Present
         1,                                    // Requested Device
         6,                                    // Requested Function
         DxioHotplugDisabled,                  // Hotplug
         DxioGen3,                               // Max Link Speed
         DxioGen3,                           // Max Link Capability
         DxioAspmDisabled,                        // ASPM
         DxioAspmDisabled,                     // ASPM L1.1 disabled
         DxioAspmDisabled,                     // ASPM L1.2 disabled
         DxioClkPmSupportDisabled              // Clock PM
       )
     },
     
     { //S0D0P0 SATA HDDMINISAS
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 4, 7, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_SATA (
             DxioPortEnabled                       // Port Present
           )
     },
      
      { //S0D0P0 - x2 M2 CARD J29
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 9, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          1,                                    // Requested Device
          3,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                                 // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S0D0P0 - x2 NIC
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 10, 11, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          1,                                    // Requested Device
          4,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen2,                               // Max Link Speed
          DxioGen2,                             // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S0D0P0 - x4 J31
        0,
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugDisabled, 1),
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          1,                                    // Requested Device
          5,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S0D1P1 - x8 OCP J32
        0,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 87, DxioHotplugDisabled, 1),     
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                                // Max Link Speed
          DxioGen3,                               // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
     
      {
        DESCRIPTOR_TERMINATE_LIST,
       DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 88, 95, DxioHotplugDisabled, 1),  
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      }                    
   }   // End of DXIO_PORT_DESCRIPTOR
};

//
// CPM DXIO Topology Table Socket 1
// CPU PORT    PCIE lane    PCIE Configuration      Condition
// CPU1 P0     Lane0-7      1：SATA X8              SATA_CPU1_DETECT0_N =0
//                          2: 2*PCIEX4             SATA_CPU1_DETECT0_N =1
//             Lane8-15     1：PCIE X8              OCP_PCIEX8_DET_N=0
//                          2: 2*PCIE X4            OCP_PCIEX8_DET_N=1
// CPU1 P1     Lane0-15     1: PCIEX8+PCIEX8        RISERB_TYPE0_N=0, RISERB_TYPE1_N=0
//                          2: PCIE X16             RISERB_TYPE0_N=1, RISERB_TYPE1_N=0
//                          3: 4*PCIEX4             RISERB_TYPE1_N=1, 
// CPU1 P2     Lane0-15     1: PCIE X16             RISERB_TYPE1_N=0
//                          2: 4*PCIEX4             RISERB_TYPE1_N=1, 
// CPU1 P3     Lane0-15     1: PCIEX16              RISERC_TYPE0_N=1, RISERC_NVME_DETECT_N=1
//                          2: 2*PCIEX8             RISERC_TYPE0_N=0, RISERC_NVME_DETECT_N=1
//                          3: 4*PCIEX4             RISERC_NVME_DETECT_N =0
//
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHygon62DB32SL1r2S1_4DIE = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY_S1, sizeof (gCpmDxioTopologyTableHygon62DB32SL1r2S1_4DIE) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  1,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//  { UINT32  Flags;                    /**< Descriptor flags
//    #define DXIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId) {mType, mHotplug, 0, mStartLane, mEndLane, mGpioGroupId, 0, 0, 0}
//    #define DXIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mAspmL1_1, mAspmL1_2,  mClkPmSupport) \
//            {mPortPresent, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mAspmL1_1, mAspmL1_2, 0, mHotplug, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, DxioEndpointDetect}

      { //S1D0P0 X8 J35
         0,
         DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 7, DxioHotplugDisabled, 1),
         DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port Present
           0,                                    // Requested Device
           0,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGen3,                               // Max Link Speed
           DxioGen3,                             // Max Link Capability
           DxioAspmDisabled,                        // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
         )
       },
       
       { //S1D0P0 X8 J36
         0,
         DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
         DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port Present
           0,                                    // Requested Device
           0,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGen3,                               // Max Link Speed
           DxioGen3,                             // Max Link Capability
           DxioAspmDisabled,                        // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
         )
       },
   //Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME >>>
      { //S1D1P1 X16 J34
        0,       
        DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 95, DxioHotplugDisabled, 1),     
        DXIO_PORT_DATA_INITIALIZER_PCIE (
          DxioPortEnabled,                      // Port Present
          0,                                    // Requested Device
          0,                                    // Requested Function
          DxioHotplugDisabled,                  // Hotplug
          DxioGen3,                               // Max Link Speed
          DxioGen3,                              // Max Link Capability
          DxioAspmDisabled,                        // ASPM
          DxioAspmDisabled,                     // ASPM L1.1 disabled
          DxioAspmDisabled,                     // ASPM L1.2 disabled
          DxioClkPmSupportDisabled              // Clock PM
        )
      },
      
      { //S1D1P1 X16 J34.1
          0,          
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 80, 83, DxioHotplugServerExpress, 1),
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                               // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.2
          0,        
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 84, 87, DxioHotplugServerExpress, 1), 
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                               // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.3
          0,         
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 88, 91, DxioHotplugServerExpress, 1),
          DXIO_PORT_DATA_INITIALIZER_PCIE (
            DxioPortEnabled,                      // Port Present
            0,                                    // Requested Device
            0,                                    // Requested Function
            DxioHotplugServerExpress,                  // Hotplug
            DxioGen3,                               // Max Link Speed
            DxioGen3,                             // Max Link Capability
            DxioAspmDisabled,                        // ASPM
            DxioAspmDisabled,                     // ASPM L1.1 disabled
            DxioAspmDisabled,                     // ASPM L1.2 disabled
            DxioClkPmSupportDisabled              // Clock PM
          )
      },
      { //S1D1P1 X16 J34.4
            DESCRIPTOR_TERMINATE_LIST,          
            DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 92, 95, DxioHotplugServerExpress, 1),        
            DXIO_PORT_DATA_INITIALIZER_PCIE (
              DxioPortEnabled,                      // Port Present
              0,                                    // Requested Device
              0,                                    // Requested Function
              DxioHotplugServerExpress,                  // Hotplug
              DxioGen3,                               // Max Link Speed
              DxioGen3          ,                  // Max Link Capability
              DxioAspmDisabled,                        // ASPM
              DxioAspmDisabled,                     // ASPM L1.1 disabled
              DxioAspmDisabled,                     // ASPM L1.2 disabled
              DxioClkPmSupportDisabled              // Clock PM
            )
      }
  }   // End of DXIO_PORT_DESCRIPTOR
};

//Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME <<<
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->
HYGON_CPM_EQ_CONFIG_TABLE  gCpmEqConfigurationTableHygon62DB32SL1r2 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof (gCpmEqConfigurationTableHygon62DB32SL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
  
  	{FLAG_SPECIFIED, pcie_gen3_ttx_force_otherside_preset, CPU_HG2, 2, 0x100000E6, 0xFFFFFFFF, 8, 15, TRUE, {3,3,3,3,3,3,3,3}},
    {FLAG_SPECIFIED, pcie_gen3_ttx_force_otherside_preset, CPU_HG2, 2, 0x100010E2, 0xFFFFFFFF, 8, 15, TRUE, {3,3,3,3,3,3,3,3}},
    {FLAG_SPECIFIED, pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 2, 0x100000E6, 0xFFFFFFFF, 8, 15, TRUE, {3,3,3,3,3,3,3,3}},
    {FLAG_SPECIFIED, pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 2, 0x100010E2, 0xFFFFFFFF, 8, 15, TRUE, {3,3,3,3,3,3,3,3}},
    {FLAG_SPECIFIED, pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 1, 0xa824144d, 0xFFFFFFFF, 16,  23, TRUE, {1}},
    {FLAG_FORCE,     pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 1, 0xFFFFFFFF, 0xFFFFFFFF, 24,  31, TRUE, {3}},
    {EQ_TABLE_END, eq_type_end, 0xFF, 0xFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, TRUE, {0} }
  }       
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


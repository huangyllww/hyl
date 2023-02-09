#include <AmdCpmPei.h>
#include <AmdCpmBaseIo.h>
#include <HygonCpmOem.h>
#include <Ppi/Pca9545aPpi.h>
#include <Ppi/Ds125Br401aPpi.h>
#include "HygonCpmOemInitPeim.h"

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
// GPIO Init Table for HonghaiSL1SLTSL1r2
//
AMD_CPM_GPIO_INIT_TABLE             gCpmGpioInitTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_INIT, sizeof (gCpmGpioInitTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {//        (socket,die,gpio,function,         output,           pullup)
    GPIO_DEF_V2 (0, 0, 89,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //  PSP_INTR_0
//    GPIO_DEF_V2 (0, 0, 90,  GPIO_FUNCTION_0,  GPIO_INPUT,       GPIO_PU_EN    ),   // TPM_INT#
//    GPIO_DEF_V2 (0, 0, 130, GPIO_FUNCTION_0,  GPIO_NA,          GPIO_PU_PD_DIS),   // SATA_LED
//    GPIO_DEF_V2 (0, 0, 131, GPIO_FUNCTION_3,  GPIO_OUTPUT_HIGH, GPIO_PU_EN    ),   // 0: SATA, 1: PCIE
    GPIO_DEF_V2 (0, 0, 147,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //Die 0 xgbe SPF_SCL
    GPIO_DEF_V2 (0, 0, 148,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //Die 0 for xgbe SPF_SDA
    GPIO_DEF_V2 (0, 1, 147,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //Die 1 xgbe SPF_SCL
    GPIO_DEF_V2 (0, 1, 148,  GPIO_FUNCTION_1,  GPIO_NA,       GPIO_PU_PD_DIS),   //Die 1 for xgbe SPF_SDA
    0xFF,
  }
};

//
// GEVENT Init Table
//
AMD_CPM_GEVENT_INIT_TABLE           gCpmGeventInitTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GEVENT_INIT, sizeof (gCpmGeventInitTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x00000001},
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

AMD_CPM_GPIO_DEVICE_CONFIG_TABLE    gCpmGpioDeviceConfigTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_CONFIG, sizeof (gCpmGpioDeviceConfigTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
// CPM Device Detection Table for HonghaiSL1SLTSL1r2
//
AMD_CPM_GPIO_DEVICE_DETECTION_TABLE gCpmGpioDeviceDetectionTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_DETECTION, sizeof (gCpmGpioDeviceDetectionTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_GPIO_DEVICE_RESET_TABLE gCpmGpioDeviceResetTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_RESET, sizeof (gCpmGpioDeviceResetTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_GPIO_DEVICE_POWER_TABLE gCpmGpioDevicePowerTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_GPIO_DEVICE_POWER, sizeof (gCpmGpioDevicePowerTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
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
AMD_CPM_PCIE_CLOCK_TABLE    gCpmPcieClockTableHonghaiSL1SLTSL1r2 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table content
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_PCIE_CLOCK, sizeof (gCpmPcieClockTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0F},
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
AMD_CPM_DXIO_TOPOLOGY_TABLE gCpmDxioTopologyTableHonghaiSL1SLTSL1r2S0 = {
//{  UINT32 TableSignature;   // Signature of CPM table
//   UINT16 TableSize;        // Table size
//   UINT8  FormatRevision;   // Revision of table format
//   UINT8  ContentRevision;  // Revision of table contect
//   UINT32 PlatformMask;     // The mask of platform table supports
//   UINT32 Attribute;        // Table attribute},
  {CPM_SIGNATURE_DXIO_TOPOLOGY, sizeof (gCpmDxioTopologyTableHonghaiSL1SLTSL1r2S0) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  0,  // SocketId
  {   // DXIO_PORT_DESCRIPTOR
//  { UINT32  Flags;                    /**< Descriptor flags
//    #define DXIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId) {mType, mHotplug, 0, mStartLane, mEndLane, mGpioGroupId, 0, 0, 0}
//    #define DXIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, mAspm, mAspmL1_1, mAspmL1_2,  mClkPmSupport) \
//            {mPortPresent, mDevAddress, mDevFunction, mMaxLinkSpeed, mAspm, mAspmL1_1, mAspmL1_2, 0, mHotplug, {0, mMaxLinkCap, 0, mClkPmSupport}, {0, 0, 0}, DxioEndpointDetect}

       { // P0 - SATA port
          0,
          DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 7, DxioHotplugDisabled, 1),
          DXIO_PORT_DATA_INITIALIZER_SATA (
            DxioPortEnabled                       // Port Present
          )
        },
       
       { // P0 - x4 I350
          0,
          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
          DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port Present
                 1,                                    // Requested Device
                 1,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                  // Max Link Speed
                 DxioGenMaxSupported,                  // Max Link Capability
          		 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
            )
    },

	   { // P0 - x1 BMC
		  0,
		  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
		  DXIO_PORT_DATA_INITIALIZER_PCIE (
			DxioPortEnabled,                      // Port Present
			1,                                    // Requested Device
			2,                                    // Requested Function
			DxioHotplugDisabled,                  // Hotplug
			DxioGen1,                             // Max Link Speed
			DxioGen1,                             // Max Link Capability
			DxioAspmDisabled,                        // ASPM
			DxioAspmDisabled,                     // ASPM L1.1 disabled
			DxioAspmDisabled,                     // ASPM L1.2 disabled
			DxioClkPmSupportDisabled              // Clock PM
		  )
	   }, 
	    { // P0 - x1 slot
	          0,
	          DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
	          DXIO_PORT_DATA_INITIALIZER_PCIE (
	            DxioPortDisabled,                      // Port Present
	            1,                                    // Requested Device
	            3,                                    // Requested Function
	            DxioHotplugDisabled,                  // Hotplug
	            DxioGenMaxSupported,                  // Max Link Speed
	            DxioGenMaxSupported,                  // Max Link Capability
	            DxioAspmDisabled,                        // ASPM
	            DxioAspmDisabled,                     // ASPM L1.1 disabled
	            DxioAspmDisabled,                     // ASPM L1.2 disabled
	            DxioClkPmSupportDisabled              // Clock PM
	          )
	       },
       { // P0 - x1
                0,
                DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                  DxioPortEnabled,                      // Port Present
                  1,                                    // Requested Device
                  4,                                    // Requested Function
                  DxioHotplugDisabled,                  // Hotplug
                  DxioGenMaxSupported,                  // Max Link Speed
                  DxioGenMaxSupported,                  // Max Link Capability
                  DxioAspmDisabled,                        // ASPM
                  DxioAspmDisabled,                     // ASPM L1.1 disabled
                  DxioAspmDisabled,                     // ASPM L1.2 disabled
                  DxioClkPmSupportDisabled              // Clock PM
                )
             },
			 
       { // G0 - x16 slot
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 16, 31, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port not Present
           3,                                    // Requested Device
           1,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGenMaxSupported,                            // Max Link Speed
           DxioGenMaxSupported,                            // Max Link Capability
           DxioAspmDisabled,                        // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
          )
       },

       { // P1 - x16 slot
           0,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 32, 47, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port Present
           1,                                    // Requested Device
           1,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGenMaxSupported,                             // Max Link Speed
           DxioGenMaxSupported,                             // Max Link Capability
           DxioAspmDisabled,                        // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
          )
       },
       
       { // G1 - x16 slot
           DESCRIPTOR_TERMINATE_LIST,
           DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 48, 63, DxioHotplugDisabled, 1),
           DXIO_PORT_DATA_INITIALIZER_PCIE (
           DxioPortEnabled,                      // Port Present
           3,                                    // Requested Device
           1,                                    // Requested Function
           DxioHotplugDisabled,                  // Hotplug
           DxioGenMaxSupported,                  // Max Link Speed
           DxioGenMaxSupported,                  // Max Link Capability
           DxioAspmDisabled,                        // ASPM
           DxioAspmDisabled,                     // ASPM L1.1 disabled
           DxioAspmDisabled,                     // ASPM L1.2 disabled
           DxioClkPmSupportDisabled              // Clock PM
          )
       },
  }   // End of DXIO_PORT_DESCRIPTOR
};

HYGON_CPM_EQ_CONFIG_TABLE  gCpmEqConfigurationTableHonghaiSL1SLTSL1r2 = {
  {CPM_SIGNATURE_EQ_CONFIG, sizeof (gCpmEqConfigurationTableHonghaiSL1SLTSL1r2) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  {
    {FLAG_FORCE, pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 1, 0xFFFFFFFF, 0xFFFFFFFF, 0,  15, TRUE, {4}},
    {FLAG_FORCE, pcie_gen4_ttx_force_otherside_preset, CPU_HG3, 1, 0xFFFFFFFF, 0xFFFFFFFF, 16, 31, TRUE, {4}},
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

UINT8 EQOffsetList[] = {
  //Channel B 0-3 EQ Offeset
  0x0F,
  0x16,
  0x1D,
  0x24,
  //Channel A 0-3 EQ Offset
  0x2C,
  0x33,
  0x3A,
  0x41
};
#define EQ_LIST_COUNT (sizeof (EQOffsetList) / sizeof (UINT8))


UINT8 VODOffsetList[] = {
  //Channel B 0-3 VOD Offset
  0x10,
  0x17,
  0x1E,
  0x25,
  //Channel A 0-3 VOD Offset
  0x2D,
  0x34,
  0x3B,
  0x42
};
#define VOD_LIST_COUNT (sizeof (VODOffsetList) / sizeof (UINT8))

UINT8 DEMOffsetList[] = {
  //Channel B 0-3 DEM
  0x11,
  0x18,
  0x1F,
  0x26,
  //Channel A 0-3 DEM
  0x2E,
  0x35,
  0x3C,
  0x43
};
#define DEM_LIST_COUNT (sizeof (VODOffsetList) / sizeof (UINT8))

#define FIELD_REDRIVER_COUNT (3)
REDRIVER_PROFILE FieldRedriverList[FIELD_REDRIVER_COUNT] = {
//   Type, Channel, 7-bit SlaveAddr, EQ, VOD, DEM
    {BR800, 1, 0x58, 0,    0, 0x0},
    {BR800, 1, 0x59, 1, 0xAB, 0x0},
    {BR800, 1, 0x5A, 0, 0XAB, 0x0}
};

#define I2CMUX_BASE (0x70)

/*----------------------------------------------------------------------------------------*/
/**
 * Setup re-driver on the card
 *
 * This function program redriver on the card based on different card design
 * automatically.
 *
 * @param[in]     SlotInfo                Slot information
 *
 */
EFI_STATUS
SetRedriver (
  IN       EFI_PEI_SERVICES             **PeiServices  
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Counter;
  UINT8 TotalCount;
  UINT8 i;
  UINT8 I2cMuxAddr;
  EFI_PEI_PCA9545A_PPI   *Pca9545aPpi;
  EFI_PEI_DS125BR401A_PPI *Ds125Br401aPpi;
  REDRIVER_PROFILE *RedriverProfile;
  
  RedriverProfile = &FieldRedriverList[0];
  TotalCount = FIELD_REDRIVER_COUNT;
  

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gPca9545aPpiGuid,
     0,
     NULL,
     &Pca9545aPpi
     );

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gDs125Br401aPpiGuid,
     0,
     NULL,
     &Ds125Br401aPpi
     );

  DEBUG ((EFI_D_ERROR, "I2cRe-Driver Total Count: 0x%x\n", TotalCount));
  I2cMuxAddr = I2CMUX_BASE | 0;
  for (Counter = 0; Counter < TotalCount; Counter++) {
    Pca9545aPpi->Set (PeiServices, 0, I2cMuxAddr , RedriverProfile[Counter].Channel);

    //Set re-driver Register Enable
    Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x06, 0x18);
    for (i = 0; i < EQ_LIST_COUNT; i++) {
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, EQOffsetList[i], RedriverProfile[Counter].EQ);
    }
    for (i = 0; i < VOD_LIST_COUNT; i++) {
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, VODOffsetList[i], RedriverProfile[Counter].VOD);
    }
    for (i = 0; i < DEM_LIST_COUNT; i++) {
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, DEMOffsetList[i], RedriverProfile[Counter].DEM);
    }
    if (RedriverProfile[Counter].Type == BR800) {
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x0D, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x14, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x1B, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x22, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x2A, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x31, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x38, 0x02);
      Ds125Br401aPpi->Set (PeiServices, 0, RedriverProfile[Counter].SlaveAddress, 0x3F, 0x02);
    }
    Pca9545aPpi->Set (PeiServices, 0, I2cMuxAddr, 0);
  }

  return Status;
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

//#if FieldCard_SUPPORT
//  SetRedriver (PeiServices);
//#endif
  
  return Status;
}

/* $NoKeywords:$ */
/**
 * @file
 *
 * Memory DMI structures and definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
 *
 ***************************************************************************/


#ifndef _MEM_DMI_H_
#define _MEM_DMI_H_

#include <AGESA.h>

#define MAX_DIMMS_PER_SOCKET          16
#define MEMORY_CAPACITY_SIZE_KB_4GB   0x00400000
#define MEMORY_CAPACITY_SIZE_KB_8GB   0x00800000
#define MEMORY_CAPACITY_SIZE_KB_16GB  0x04000000
#define MEMORY_CAPACITY_SIZE_KB_32GB  0x02000000
#define MEMORY_CAPACITY_SIZE_KB_2TB   0x80000000

#define MAX_T19_REGION_SUPPORTED      3                 ///< Max SMBIOS T19 Memory Region count

/// DMI Type 16 offset 04h - Location
typedef enum {
  OtherLocation = 0x01,                                 ///< Assign 01 to Other
  UnknownLocation,                                      ///< Assign 02 to Unknown
  SystemboardOrMotherboard,                             ///< Assign 03 to systemboard or motherboard
  IsaAddonCard,                                         ///< Assign 04 to ISA add-on card
  EisaAddonCard,                                        ///< Assign 05 to EISA add-on card
  PciAddonCard,                                         ///< Assign 06 to PCI add-on card
  McaAddonCard,                                         ///< Assign 07 to MCA add-on card
  PcmciaAddonCard,                                      ///< Assign 08 to PCMCIA add-on card
  ProprietaryAddonCard,                                 ///< Assign 09 to proprietary add-on card
  NuBus,                                                ///< Assign 0A to NuBus
  Pc98C20AddonCard,                                     ///< Assign 0A0 to PC-98/C20 add-on card
  Pc98C24AddonCard,                                     ///< Assign 0A1 to PC-98/C24 add-on card
  Pc98EAddoncard,                                       ///< Assign 0A2 to PC-98/E add-on card
  Pc98LocalBusAddonCard                                 ///< Assign 0A3 to PC-98/Local bus add-on card
} DMI_T16_LOCATION;

/// DMI Type 16 offset 05h - Memory Error Correction
typedef enum {
  OtherUse = 0x01,                                      ///< Assign 01 to Other
  UnknownUse,                                           ///< Assign 02 to Unknown
  SystemMemory,                                         ///< Assign 03 to system memory
  VideoMemory,                                          ///< Assign 04 to video memory
  FlashMemory,                                          ///< Assign 05 to flash memory
  NonvolatileRam,                                       ///< Assign 06 to non-volatile RAM
  CacheMemory                                           ///< Assign 07 to cache memory
} DMI_T16_USE;

/// DMI Type 16 offset 07h - Maximum Capacity
typedef enum {
  Dmi16OtherErrCorrection = 0x01,                       ///< Assign 01 to Other
  Dmi16UnknownErrCorrection,                            ///< Assign 02 to Unknown
  Dmi16NoneErrCorrection,                               ///< Assign 03 to None
  Dmi16Parity,                                          ///< Assign 04 to parity
  Dmi16SingleBitEcc,                                    ///< Assign 05 to Single-bit ECC
  Dmi16MultiBitEcc,                                     ///< Assign 06 to Multi-bit ECC
  Dmi16Crc                                              ///< Assign 07 to CRC
} DMI_T16_ERROR_CORRECTION;

/// DMI Type 16 - Physical Memory Array
typedef struct {
  OUT DMI_T16_LOCATION          Location;               ///< The physical location of the Memory Array,
                                                        ///< whether on the system board or an add-in board.
  OUT DMI_T16_USE               Use;                    ///< Identifies the function for which the array
                                                        ///< is used.
  OUT DMI_T16_ERROR_CORRECTION  MemoryErrorCorrection;  ///< The primary hardware error correction or
                                                        ///< detection method supported by this memory array.
  OUT UINT16                    NumberOfMemoryDevices;  ///< The number of slots or sockets available
                                                        ///< for memory devices in this array.
} TYPE16_DMI_INFO;

/// DMI Type 17 offset 0Eh - Form Factor
typedef enum {
  OtherFormFactor = 0x01,                               ///< Assign 01 to Other
  UnknowFormFactor,                                     ///< Assign 02 to Unknown
  SimmFormFactor,                                       ///< Assign 03 to SIMM
  SipFormFactor,                                        ///< Assign 04 to SIP
  ChipFormFactor,                                       ///< Assign 05 to Chip
  DipFormFactor,                                        ///< Assign 06 to DIP
  ZipFormFactor,                                        ///< Assign 07 to ZIP
  ProprietaryCardFormFactor,                            ///< Assign 08 to Proprietary Card
  DimmFormFactorFormFactor,                             ///< Assign 09 to DIMM
  TsopFormFactor,                                       ///< Assign 10 to TSOP
  RowOfChipsFormFactor,                                 ///< Assign 11 to Row of chips
  RimmFormFactor,                                       ///< Assign 12 to RIMM
  SodimmFormFactor,                                     ///< Assign 13 to SODIMM
  SrimmFormFactor,                                      ///< Assign 14 to SRIMM
  FbDimmFormFactor                                      ///< Assign 15 to FB-DIMM
} DMI_T17_FORM_FACTOR;

/// DMI Type 17 offset 12h - Memory Type
typedef enum {
  OtherMemType = 0x01,                                  ///< Assign 01 to Other
  UnknownMemType,                                       ///< Assign 02 to Unknown
  DramMemType,                                          ///< Assign 03 to DRAM
  EdramMemType,                                         ///< Assign 04 to EDRAM
  VramMemType,                                          ///< Assign 05 to VRAM
  SramMemType,                                          ///< Assign 06 to SRAM
  RamMemType,                                           ///< Assign 07 to RAM
  RomMemType,                                           ///< Assign 08 to ROM
  FlashMemType,                                         ///< Assign 09 to Flash
  EepromMemType,                                        ///< Assign 10 to EEPROM
  FepromMemType,                                        ///< Assign 11 to FEPROM
  EpromMemType,                                         ///< Assign 12 to EPROM
  CdramMemType,                                         ///< Assign 13 to CDRAM
  ThreeDramMemType,                                     ///< Assign 14 to 3DRAM
  SdramMemType,                                         ///< Assign 15 to SDRAM
  SgramMemType,                                         ///< Assign 16 to SGRAM
  RdramMemType,                                         ///< Assign 17 to RDRAM
  DdrMemType,                                           ///< Assign 18 to DDR
  Ddr2MemType,                                          ///< Assign 19 to DDR2
  Ddr2FbdimmMemType,                                    ///< Assign 20 to DDR2 FB-DIMM
  Ddr3MemType = 0x18,                                   ///< Assign 24 to DDR3
  Fbd2MemType,                                          ///< Assign 25 to FBD2
  Ddr4MemType,                                          ///< Assign 26 to DDR4
  LpDdrMemType,                                         ///< Assign 27 to LPDDR
  LpDdr2MemType,                                        ///< Assign 28 to LPDDR2
  LpDdr3MemType,                                        ///< Assign 29 to LPDDR3
  LpDdr4MemType,                                        ///< Assign 30 to LPDDR4
} DMI_T17_MEMORY_TYPE;

/// DMI Type 17 offset 13h - Type Detail
typedef struct {
  OUT UINT16                    Reserved1:1;            ///< Reserved
  OUT UINT16                    Other:1;                ///< Other
  OUT UINT16                    Unknown:1;              ///< Unknown
  OUT UINT16                    FastPaged:1;            ///< Fast-Paged
  OUT UINT16                    StaticColumn:1;         ///< Static column
  OUT UINT16                    PseudoStatic:1;         ///< Pseudo-static
  OUT UINT16                    Rambus:1;               ///< RAMBUS
  OUT UINT16                    Synchronous:1;          ///< Synchronous
  OUT UINT16                    Cmos:1;                 ///< CMOS
  OUT UINT16                    Edo:1;                  ///< EDO
  OUT UINT16                    WindowDram:1;           ///< Window DRAM
  OUT UINT16                    CacheDram:1;            ///< Cache Dram
  OUT UINT16                    NonVolatile:1;          ///< Non-volatile
  OUT UINT16                    Registered:1;           ///< Registered (Buffered)
  OUT UINT16                    Unbuffered:1;           ///< Unbuffered (Unregistered)
  OUT UINT16                    LRDIMM:1;               ///< LRDIMM
} DMI_T17_TYPE_DETAIL;

/// DMI Type 17 offset 28h - Memory Technology
typedef enum {
  OtherType = 0x01,                                     ///< Assign 01 to Other
  UnknownType = 0x02,                                   ///< Assign 02 to Unknown
  DramType = 0x03,                                      ///< Assign 03 to DRAM
  NvDimmNType = 0x04,                                   ///< Assign 04 to NVDIMM-N
  NvDimmFType = 0x05,                                   ///< Assign 05 to NVDIMM-F
  NvDimmPType = 0x06,                                   ///< Assign 06 to NVDIMM-P
  IntelPersistentMemoryType = 0x07,                     ///< Assign 07 to Intel persistent memory
} DMI_T17_MEMORY_TECHNOLOGY;
/// DMI Type 17 offset 29h - Memory Operating Mode Capability
typedef union {
  UINT16                          MemOperatingModeCap;
  struct {
    OUT UINT16                    Reserved1:1;            ///< Reserved, set to 0
    OUT UINT16                    Other:1;                ///< Other
    OUT UINT16                    Unknown:1;              ///< Unknown
    OUT UINT16                    VolatileMemory:1;       ///< Volatile memory
    OUT UINT16                    ByteAccessiblePersistentMemory:1;    ///< Byte-accessible persistent memory
    OUT UINT16                    BlockAccessiblePersistentMemory:1;   ///< Block-accessible persistent memory
    OUT UINT16                    Reserved2:10;           ///< Reserved, set to 0
  } RegField;
} DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY;
/// DMI Type 17 - Memory Device
typedef struct {
  OUT UINT16                    Handle;                 ///< The temporary handle, or instance number, associated with the structure
  OUT UINT16                    TotalWidth;             ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  OUT UINT16                    DataWidth;              ///< Data Width, in bits, of this memory device.
  OUT UINT16                    MemorySize;             ///< The size of the memory device.
  OUT DMI_T17_FORM_FACTOR       FormFactor;             ///< The implementation form factor for this memory device.
  OUT UINT8                     DeviceSet;              ///< Identifies when the Memory Device is one of a set of
                                                        ///< Memory Devices that must be populated with all devices of
                                                        ///< the same type and size, and the set to which this device belongs.
//<Kangmm-20220128 Modify the length of DIMM device locator +>
  OUT CHAR8                     DeviceLocator[13];       ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
//<Kangmm-20220128 Modify the length of DIMM device locator ->
  OUT CHAR8                     BankLocator[13];        ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  OUT DMI_T17_MEMORY_TYPE       MemoryType;             ///< The type of memory used in this device.
  OUT DMI_T17_TYPE_DETAIL       TypeDetail;             ///< Additional detail on the memory device type
  OUT UINT16                    Speed;                  ///< Identifies the speed of the device, in megahertz (MHz).
  OUT UINT64                    ManufacturerIdCode;     ///< Manufacturer ID code.
  OUT CHAR8                     SerialNumber[20];        ///< Serial Number. //<Lvshh001-20200911 Modify Memory SN follow Tencent spec V1.5>
  OUT CHAR8                     PartNumber[21];         ///< Part Number.
  OUT UINT8                     Attributes;             ///< Bits 7-4: Reserved, Bits 3-0: rank.
  OUT UINT32                    ExtSize;                ///< Extended Size.
  OUT UINT16                    ConfigSpeed;            ///< Configured memory clock speed
  OUT UINT16                    MinimumVoltage;         ///< Minimum operating voltage for this device, in millivolts
  OUT UINT16                    MaximumVoltage;         ///< Maximum operating voltage for this device, in millivolts
  OUT UINT16                    ConfiguredVoltage;      ///< Configured voltage for this device, in millivolts
  OUT UINT8                     MemoryTechnology;       ///< Memory technology type for this memory device
  OUT DMI_T17_MEMORY_OPERATING_MODE_CAPABILITY   MemoryOperatingModeCapability; ///< The operating modes supported by this memory device
  OUT CHAR8                     FirmwareVersion[10];    ///< String number for the firmware version of this memory device
  OUT UINT16                    ModuleManufacturerId;   ///< The two-byte module manufacturer ID found in the SPD of this memory device; LSB first.
  OUT UINT16                    ModuleProductId;        ///< The two-byte module product ID found in the SPD of this memory device; LSB first
  OUT UINT16                    MemorySubsystemControllerManufacturerId; //< The two-byte memory subsystem controller manufacturer ID found in the SPD of this memory device; LSB first
  OUT UINT16                    MemorySubsystemControllerProductId; //< The two-byte memory subsystem controller product ID found in the SPD of this memory device; LSB first
  OUT UINT64                    NonvolatileSize;        ///< Size of the Non-volatile portion of the memory device in Bytes, if any.
  OUT UINT64                    VolatileSize;           ///< Size of the Volatile portion of the memory device in Bytes, if any.
  OUT UINT64                    CacheSize;              ///< Size of the Cache portion of the memory device in Bytes, if any. 
  OUT UINT64                    LogicalSize;            ///< Size of the Logical memory device in Bytes.
} TYPE17_DMI_INFO;

/// Memory DMI Type 17 - for memory use
typedef struct {
  OUT UINT8                     Socket:3;               ///< Socket ID
  OUT UINT8                     Channel:2;              ///< Channel ID
  OUT UINT8                     Dimm:2;                 ///< DIMM ID
  OUT UINT8                     DimmPresent:1;          ///< Dimm Present
  OUT UINT16                    Handle;                 ///< The temporary handle, or instance number, associated with the structure
  OUT UINT16                    TotalWidth;             ///< Total Width, in bits, of this memory device, including any check or error-correction bits.
  OUT UINT16                    DataWidth;              ///< Data Width, in bits, of this memory device.
  OUT UINT16                    MemorySize;             ///< The size of the memory device.
  OUT DMI_T17_FORM_FACTOR       FormFactor;             ///< The implementation form factor for this memory device.
  OUT UINT8                     DeviceLocator;          ///< The string number of the string that identifies the physically labeled socket or board position where the memory device is located.
  OUT UINT8                     BankLocator;            ///< The string number of the string that identifies the physically labeled bank where the memory device is located.
  OUT UINT16                    Speed;                  ///< Identifies the speed of the device, in megahertz (MHz).
  OUT UINT64                    ManufacturerIdCode;     ///< Manufacturer ID code.
  OUT UINT8                     SerialNumber[9];        ///< Serial Number.
  OUT UINT8                     PartNumber[21];         ///< Part Number.
  OUT UINT8                     Attributes;             ///< Bits 7-4: Reserved, Bits 3-0: rank.
  OUT UINT32                    ExtSize;                ///< Extended Size.
  OUT UINT16                    ConfigSpeed;            ///< Configured memory clock speed
  OUT UINT16                    MinimumVoltage;         ///< Minimum operating voltage for this device, in millivolts
  OUT UINT16                    MaximumVoltage;         ///< Maximum operating voltage for this device, in millivolts
  OUT UINT16                    ConfiguredVoltage;      ///< Configured voltage for this device, in millivolts
} MEM_DMI_PHYSICAL_DIMM_INFO;

/// Memory DMI Type 20 - for memory use
typedef struct {
  OUT UINT8                     Socket:3;               ///< Socket ID
  OUT UINT8                     Channel:2;              ///< Channel ID
  OUT UINT8                     Dimm:2;                 ///< DIMM ID
  OUT UINT8                     DimmPresent:1;          ///< Dimm Present
  OUT BOOLEAN                   Interleaved;            ///< Interleaved;
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes, of a range
                                                        ///< of memory mapped to the referenced Memory Device.
  OUT UINT32                    EndingAddr;             ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    MemoryDeviceHandle;     ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the referenced Memory Device.
  OUT UINT64                    ExtEndingAddr;          ///< The physical ending address, in bytes, of the last of
                                                        ///< a range of addresses mapped to the referenced Memory Device.
} MEM_DMI_LOGICAL_DIMM_INFO;

/// DMI Type 19 - Memory Array Mapped Address
typedef struct {
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes,
                                                        ///< of a range of memory mapped to the
                                                        ///< specified physical memory array.
  OUT UINT32                    EndingAddr;             ///< The physical ending address of the
                                                        ///< last kilobyte of a range of addresses
                                                        ///< mapped to the specified physical memory array.
  OUT UINT16                    MemoryArrayHandle;      ///< The handle, or instance number, associated
                                                        ///< with the physical memory array to which this
                                                        ///< address range is mapped.
  OUT UINT8                     PartitionWidth;         ///< Identifies the number of memory devices that
                                                        ///< form a single row of memory for the address
                                                        ///< partition defined by this structure.
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the specified Physical Memory Array.
  OUT UINT64                    ExtEndingAddr;          ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the specified Physical Memory Array.
} TYPE19_DMI_INFO;

///DMI Type 20 - Memory Device Mapped Address
typedef struct {
  OUT UINT32                    StartingAddr;           ///< The physical address, in kilobytes, of a range
                                                        ///< of memory mapped to the referenced Memory Device.
  OUT UINT32                    EndingAddr;             ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    MemoryDeviceHandle;     ///< The handle, or instance number, associated with
                                                        ///< the Memory Device structure to which this address
                                                        ///< range is mapped.
  OUT UINT16                    MemoryArrayMappedAddressHandle; ///< The handle, or instance number, associated
                                                        ///< with the Memory Array Mapped Address structure to
                                                        ///< which this device address range is mapped.
  OUT UINT8                     PartitionRowPosition;   ///< Identifies the position of the referenced Memory
                                                        ///< Device in a row of the address partition.
  OUT UINT8                     InterleavePosition;     ///< The position of the referenced Memory Device in
                                                        ///< an interleave.
  OUT UINT8                     InterleavedDataDepth;   ///< The maximum number of consecutive rows from the
                                                        ///< referenced Memory Device that are accessed in a
                                                        ///< single interleaved transfer.
  OUT UINT64                    ExtStartingAddr;        ///< The physical address, in bytes, of a range of
                                                        ///< memory mapped to the referenced Memory Device.
  OUT UINT64                    ExtEndingAddr;          ///< The physical ending address, in bytes, of the last of
                                                        ///< a range of addresses mapped to the referenced Memory Device.
} TYPE20_DMI_INFO;

/// Collection of pointers to the DMI records
typedef struct {
  OUT TYPE16_DMI_INFO           T16;                          ///< Type 16 struc
  OUT TYPE17_DMI_INFO           T17[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL]; ///< Type 17 struc
  OUT TYPE19_DMI_INFO           T19[MAX_T19_REGION_SUPPORTED];                                              ///< Type 19 struc
  OUT TYPE20_DMI_INFO           T20[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL]; ///< Type 20 struc
} DMI_INFO;

AGESA_STATUS
MemConstructDmiInfo (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   DMI_INFO              **DmiTable
  );

#endif

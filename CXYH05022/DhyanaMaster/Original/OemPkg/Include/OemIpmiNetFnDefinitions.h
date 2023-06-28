//**********************************************************************
// $Header:Oem OEM IPMI SPEC struct definition
//
// $Revision: 0.01
//
// $Date: <Lizhq-20161228 Update struct definition base on OemOEMIPMICommandListV1.0_Scorpio.doc>
//

// $Date:  <Lizhq-20161217>
//         Base On OEM IPMI SPEC 20161217, create this file.
//**********************************************************************
#ifndef _OEM_IPMI_NETFN_DEFINITIONS_H_
#define _OEM_IPMI_NETFN_DEFINITIONS_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>
#include <Efi.h>

#define NETFN_OEM                       0x3A

//----------BMC IPMI Command------------//
#define CMD_OEM_SET_LED_INFO            0x00
#define CMD_OEM_SET_MEM_ECC_THRESHOLD   0x05
#define CMD_OEM_GET_MEM_ECC_THRESHOLD   0x06
#define CMD_OEM_SET_MEM_ECC_INFO        0x07
#define CMD_OEM_GET_MEM_ECC_INFO        0x08
#define CMD_OEM_SET_FAN_POLICY          0x0b
#define CMD_OEM_GET_FAN_POLICY          0x0c
#define CMD_OEM_SET_FAN_SPEED           0x0d
#define CMD_OEM_GET_FAN_SPEED           0x0e
#define CMD_OEM_SET_MEM_MRC_INFO        0x0f
#define CMD_OEM_GET_MEM_MRC_INFO        0x10
#define CMD_OEM_GET_MEM_MRC_ALL_INFO    0x11
#define CMD_OEM_GET_POWER               0x19
#define CMD_OEM_GET_AGGREGATETHERMAL_INFO 0x1d
#define CMD_OEM_SET_HOTPLUG_FLAG        0x1e
#define CMD_OEM_GET_HOTPLUG_FLAG        0x1f
#define CMD_OEM_SET_BIOS_VERSION_INFO   0x20
#define CMD_OEM_GET_BIOS_VERSION_INFO   0x21
#define CMD_OEM_SET_POWER_STANDBY_MODE  0x22
#define CMD_OEM_GET_POWER_STANDBY_MODE  0x23
#define CMD_OEM_GET_SPEC_VERSION        0x24
#define CMD_OEM_SET_SETUP_BOOT_OPTIONS  0xc0
#define CMD_OEM_GET_SETUP_BOOT_OPTIONS  0xc1
#define CMD_OEM_GET_FW_BUILD_TIME       0x25
#define CMD_OEM_SET_SERVER_REPOSITORY_INFO 0x26
#define CMD_OEM_GET_SERVER_REPOSITORY_INFO 0x27
#define CMD_OEM_SET_MODULE_STATE        0x28
#define CMD_OEM_GET_MODULE_STATE        0x29
#define CMD_OEM_SET_GPIO_PARAM          0x30
#define CMD_OEM_GET_GPIO_PARAM          0x31

#define CMD_OEM_GET_FIRMWARE_VERSION    0x32
#define CMD_OEM_SET_FIRMWARE_VERSION    0x33


#define CMD_OEM_GET_IOM_Type            0x35
#define CMD_OEM_GET_PRESERVE_BIOS       0x3b  //kuaibai ++
#define CMD_OEM_SET_PRESERVE_BIOS       0x3c  //kuaibai ++
#define CMD_OEM_GET_REDFISH_SERVICE     0x3d  //<kuaibai-20180802-Auto disable if bmc redfish is off>

#if defined(Preserve_BIOS_Setting_OOB) && (Preserve_BIOS_Setting_OOB == 1)
//<xuyj1-20211207 Add BIOS Save Configuration after update BIOS +>
#define CMD_OEM_GET_SAVE_BIOS_CONFIG_FLAG     0xA3
#define CMD_OEM_SET_SAVE_BIOS_CONFIG_FLAG     0xA4
//<xuyj1-20211207 Add BIOS Save Configuration after update BIOS +>
#endif

#define OEM_IPMI_MAX_MANUFACTURER_STRLEN 16
//<hety-20220725 follow tencent spec v1.5 +>
#if defined(TENCENT_ERRATA_VERSION_SUPPORT) && (TENCENT_ERRATA_VERSION_SUPPORT == 1)
  #define OEM_IPMI_MAX_SERIALNUMBER_STRLEN 19
#else //TENCENT_ERRATA_VERSION_SUPPORT == 0
  #define OEM_IPMI_MAX_SERIALNUMBER_STRLEN 8
#endif //TENCENT_ERRATA_VERSION_SUPPORT
//<hety-20220725 follow tencent spec v1.5 ->
#define OEM_IPMI_MAX_MODELNUMBER_STRLEN  32

#define OEM_IPMI_HDD_SN_LENGTH  20
#define OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH  8
#define OEM_IPMI_HDD_MODEL_NAME_LENGTH  40

typedef enum {
  SreverRespositorySubTypeChassis = 0x00,//Chassis Status: 0x0
  SreverRespositorySubTypeMB      = 0x01,//MB Status:0x1 
  SreverRespositorySubTypeCpu     = 0x02,//CPU Status: 0x2
  SreverRespositorySubTypeMemory  = 0x03,//Memory Status: 0x3
  SreverRespositorySubTypeHdd     = 0x04,//HDD Status:0x4
  SreverRespositorySubTypeFan     = 0x05,//FAN Status: 0x5
  SreverRespositorySubTypePsu     = 0x06,//PSU Status: 0x6
  SreverRespositorySubTypeRaid    = 0x07,//RAID Status: 0x7
  SreverRespositorySubTypeNic     = 0x08,//NIC Status: 0x8
  SreverRespositorySubTypePcie    = 0x09,//PCIe  Status: 0x9
  SreverRespositorySubTypePower   = 0x0a,//Power  Status: 0xa
  SreverRespositorySubTypeResource= 0x0b,//Resource Status: 0xb
  SreverRespositorySubTypeCpuEx   = 0x0c,//CPU Extend Info: 0xc
  SreverRespositorySubTypeMax
}OEM_IPMI_SERVER_RESPOSITORY_SUBTYPR;

typedef enum {
  SET_MRC_DIMM_ERROR = 0x0F,
  GET_MRC_DIMM_ERROR = 0x10
}OEM_IPMI_CMD_TYPE;

typedef enum {
  SourceOther = 0,
  SourceBIOS = 1
}SET_COMMAND_SOURCE;

typedef enum {
  NotBIOSPackage = 0,
  BIOSPackageValid = 1
}PACKAGE_DATA_VALID;
#pragma pack(1)

typedef enum {
  DramUnknown = 0,
  DramDdr     = 1,
  DramDdr2    = 2,
  DramDdr3    = 3,
  DramDdr4    = 4,
  DramAEP     = 5
}ENUM_DRAM_TYPE;

typedef enum {
  DimmVolatile  = 1,
  DimmAEP       = 2,
  DimmNvdimm    = 3
}ENUM_MEMORY_TYPE;

typedef enum {
  MemFreq800MT    = 1,
  MemFreq1000MT   = 2,
  MemFreq1067MT   = 3,
  MemFreq1200MT   = 4,
  MemFreq1333MT   = 5,
  MemFreq1400MT   = 6,
  MemFreq1600MT   = 7,
  MemFreq1800MT   = 8,
  MemFreq1867MT   = 9,
  MemFreq2000MT   =10,
  MemFreq2133MT   =11,
  MemFreq2200MT   =12,
  MemFreq2400MT   =13,
  MemFreq2600MT   =14,
  MemFreq2667MT   =15,
  MemFreq2800MT   =16,
  MemFreq2933MT   =17,
  MemFreq3000MT   =18,
  MemFreq3200MT   =19
};

typedef enum {
  Unit_MB = 1,
  Unit_GB = 2,
  Unit_TB = 3
}STORAGE_UNIT;

typedef enum {
  TypeScsi    = 1,
  TypeIde     = 2,
  TypeFdd     = 3,
  TypeIpiBusController = 4,
  TypeRaid    = 5,
  TypeAtaAdma = 6,
  TypeAhci    = 7,
  TypeSas     = 8,
  TypeNvme    = 9,
  TypeOthers  = 10
}ENUM_HDD_TYPE;
//
//OEM IPMI struct definition
//
typedef struct {
  UINT8 SetCommandSource; 
  /* 00: All, 
   * 01: From BIOS
   */
  UINT8 ParameterSelector;//Refer to OEM_IPMI_SERVER_RESPOSITORY_SUBTYPR
}SET_SERVER_RESPOSITORY_HEADER;

//CPU Info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  //<lubl_add_oemipmi spec s>
  UINT8 DATA1; //DATA1, cpu number
  UINT8 DATA2; //DATA2
  UINT8 DATA3; //DATA3
  //<lubl_add_oemipmi spec e>
  UINT8 DataValid;
  /* 00: Have no BIOS operation, 
   * 01: BIOS Send data to BMC
   */
  UINT8 CpuPresent;
  UINT8 CpuLocation[8];
  UINT8 Die0BusNum;
  UINT8 Die1BusNum;
  UINT8 Die2BusNum;
  UINT8 Die3BusNum;
  UINT8 Reserved1[2];
  UINT8 CpuDie0TypeAConfig;
  UINT8 CpuDie0TypeBConfig; 
  UINT8 CpuDie1TypeAConfig; 
  UINT8 CpuDie1TypeBConfig; 
  UINT8 CpuDie2TypeAConfig; 
  UINT8 CpuDie2TypeBConfig; 
  UINT8 CpuDie3TypeAConfig; 
  UINT8 CpuDie3TypeBConfig; 
  /* 00: 16;
   * 01: 8:8;
   * 02: 8:4:4;
   * 03: 8:4:2:2;
   * 04: 8:4:2:1:1;
   * 05: 8:4:1:1:1:1;
   * 06: 8:2:2:2:2;
   * 07: 8:2:2:2:1:1;
   * 08: 8:2:2:1:1:1:1;
   * 09: 8:2:1:1:1:1:1:1;
   * 0A: 4:4:4:4;
   * 0B: 4:4:4:2:2;
   * 0C: 4:4:4:2:1:1;
   * 0D: 4:4:4:1:1:1:1;
   * 0E: 4:4:2:2:2:2;
   * 0F: 4:4:2:2:2:1:1;
   * 10: 4:4:2:2:1:1:1:1;
   * 11: 4:2:2:2:2:2:2;
   * 12: 4:2:2:2:2:2:1:1;
   * 13: 2:2:2:2:2:2:2:2;
   */
  CHAR8 CpuBrandName[64];
  UINT8 xGMIFreq[4];
  /* 01: 6.4GB/s, 
   * 02: 7.2GB/s, 
   * 03: 8.0GB/s, 
   * 04: 9.6GB/s, 
   * 05: 10.4GB/s, 
   * other: Reserved
   */
  UINT8 Reserved2[2];
} PACKED_CPU_INFO;
//CPU Extend  Info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  UINT8 DATA1; //DATA1, cpu number
  UINT8 DATA2; //DATA2
  UINT8 DATA3; //DATA3
  UINT8 DataValid;
  /* 00: Have no BIOS operation,
   * 01: BIOS Send data to BMC
   */
  UINT8 CpuId[10];      //Get from SMBIOS type 4
  UINT16 CpuFre;        //CPU frequency, in MHz, get from SMBIOS type 4
  UINT16 CoreCount;     //CPU core count, get from SMBIOS type 4
  UINT16 ThreadCount;   //CPU thread count, get from SMBIOS type 4
  UINT16 L1CacheSize;   //L1 cache size of socket, in KB, get from SMBIOS type 7
  UINT16 L2CacheSize;   //L2 cache size of socket, in KB, get from SMBIOS type 7
  UINT32 L3CacheSize;   //L3 cache size of socket, in KB, get from SMBIOS type 7
  //<Yaosr-20190313 Add CPU Info Send to BMC +>
  UINT16 CpuSysClock;    //CPU external clock, from SMBIOS type 4
  UINT8  MaxCoreCount;   //CPU max core count, get from SMBIOS type 4 
  UINT8  MaxThreadCount; //CPU max thread count, get from SMBIOS type 4
  UINT8  CpuIdFamily;    //CPU family, get from SMBIOS type 4
  UINT8  CpuIdModel;     //CPU model, get from SMBIOS type 4
  UINT8  CpuIdStepping;  //CPU stepping, get from SMBIOS type 4
  UINT16 CpuMaxPcieLinkRate;
  UINT16 CpuTDP; //<YinQiang001-20190910-Send CPU TDP to BMC> 
  //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC +>
//<Kangmm-20201125-Send CPU SN to BMC +>
#if defined(CPU_MAXSPEED_TO_BMC) && (CPU_MAXSPEED_TO_BMC == 1)
  UINT16 MaxSpeed; 
  UINT64 CpuSN; 
  UINT8  Reserved[23];
#else
  UINT64 CpuSN;
  UINT8  Reserved[25];
#endif
//<Kangmm-20201125-Send CPU SN to BMC ->
  //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC ->
  //<Yaosr-20190313 Add CPU Info Send to BMC ->
} PACKED_CPU_EX_INFO;

//DIMM Info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  UINT8 DimmNode; //DATA1   //<lubl_add_oemipmi spec +>
  UINT8 DimmChannel; //DATA2   //<lubl_add_oemipmi spec +>
  UINT8 DimmNum;//DATA3  //<lubl_add_oemipmi spec +>
  UINT8 DataValid;  //--- //<lubl_add_oemipmi spec +>
  /* 00: Have no BIOS operation, 
   * 01: BIOS Send data to BMC
   */
  UINT8 DimmPresent;
  UINT8 MemoryLocation[24];
  UINT8 DramType;
  /* 00h: unkown 
   * 01h: DDR 
   * 02h: DDR2 
   * 03h: DDR3 
   * 04h: DDR4
   * 05h: DDR-T
   */
  UINT8 MemoryType;
  /* 01h: VOLATILE
   * 02h: AEP
   * 03h: NVDIMM
   */
  UINT8 DimmFreq;
  /* Memory Frequency DDR3:
   * 01h: 800 MHz
   * 02h: 1067MHz
   * 03h: 1333MHz
   * 04h: 1600MHz
   * 05h: 1867MHz
   * 06h: 2133MHz
   * 07h: 2400MHz
   * 
   * Memory FrequencyDDR4: 
   * 01h(1): 800 MT/s
   * 02h(2): 1000MT/s
   * 03h(3): 1067MT/s
   * 04h(4): 1200MT/s
   * 05h(5): 1333MT/s
   * 06h(6): 1400MT/s
   * 07h(7): 1600MT/s
   * 08h(8): 1800MT/s
   * 09h(9): 1867MT/s
   * 0ah(10): 2000MT/s
   * 0bh(11): 2133MT/s
   * 0ch(12): 2200MT/s
   * 0dh(13): 2400MT/s
   * 0eh(14): 2600MT/s
   * 0fh(15): 2667MT/s
   * 10h(16): 2800MT/s
   * 11h(17): 2933MT/s
   * 12h(18): 3000MT/s
   * 13h(19): 3200MT/s
   */ 
  UINT8 DimmWorkFreq;
  UINT8 DimmSizeUnit;
  /* 1: MB
   * 2: GB
   * 3: TB
   */
  UINT16 DimmSize;
  /* !!!!!!Warning!!!!!!
   * If DimmSizeUnit=1, DimmSize MAX support < 64GB due to UINT16 limitation 
   */
  CHAR8 DimmManufacture[OEM_IPMI_MAX_MANUFACTURER_STRLEN];
  CHAR8 DimmPartNum[OEM_IPMI_MAX_MODELNUMBER_STRLEN];
  UINT8 DimmSN[OEM_IPMI_MAX_SERIALNUMBER_STRLEN];
  //<Malj-20181105 Add Mem Info Send to BMC +> 
  UINT8 EccCapbility;
  /* 1: Non-ECC
   * 2: ECC
   */
  UINT8 ModuleType; //<Malj-20181106 Fixed Mem Info Send to BMC> 
  /*
   * 01h(1): RDIMM
   * 02h(2): UDIMM
   * 03h(3): SO-DIMM
   * 04h(4): LRDIMM
   * 05h(5): Mini-RDIMM
   * 06h(6): Mini-UDIMM
   * 07h(7): Reserved
   * 08h(8): 72b-SO-RDIMM
   * 09h(9): 72b-SO-UDIMM
   * 0ah(10): Reserved
   * 0bh(11): Reserved
   * 0ch(12): 16b-SO-DIMM
   * 0dh(13): 32b-SO-DIMM
   * 0eh(14): Reserved
   * 0fh(15): Reserved
   */
  UINT8 RankNum;
  UINT8 DramWidth; //<Malj-20181106 Fixed Mem Info Send to BMC> 
  /* 1: x4
   * 2: x8
   * 3: x16
   * 4: x32
   */
   //<Malj-20181105 Add Mem Info Send to BMC ->
  //Mizl-20210927-Sync Parameter Of PACKED_DIMM_INFO With OEM IPMI Command List V1.82+> 
  UINT8 MinCasLantecyTime;  //SPD byte 24
  UINT8 tREFI[2];
  UINT8 ProfileType;
  /* 1: LP: Low Profile
   * 2: VLP: Very Low Profile
   * 3: ULP: Ultra Low Profile
   */
  //Mizl-20210927-Sync Parameter Of PACKED_DIMM_INFO With OEM IPMI Command List V1.82-> 
  //<hujf1001-20201108 Send DDR4 Voltage to BMC, 1.2V will send 120 +>
  UINT8 DdrVoltage;
  UINT8 Reserved[31];
} PACKED_DIMM_INFO;

//MRC error DIMM Info struct
typedef struct {
  UINT8 MrcDimmNode;  
  UINT8 MrcDimmChannel;
  UINT8 MrcDimmNo;
  UINT8 MrcMajorCode;  
  UINT8 MrcMinorCode;
} PACKED_MRC_DIMM_ERROR;

//DIMM Info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  //<lubl_add_oemipmi spec s>
  UINT8 DATA1; //DATA1, PCIE number
  UINT8 DATA2; //DATA2
  UINT8 DATA3; //DATA3
  //<lubl_add_oemipmi spec e>
  UINT8 DataValid;  
  /* 00: Have no BIOS operation, 
   * 01: BIOS Send data to BMC
   */
  UINT8 PcieDevicePresent;
  UINT8 CpuNum;
  UINT8 PcieLocation[16];
  
  UINT8 PcieSegmentNum;
  UINT8 ParentPcieBusNum;
  UINT8 ParentPcieDevNum;
  UINT8 ParentPcieFunNum;
  UINT8 PcieBusNum;
  UINT8 PcieDevNum;
  UINT8 PcieFunNum;
  
  UINT16 PcieVenderId;
  UINT16 PcieDeviceId;
  
  UINT8 PcieBaseClass;
  UINT8 PcieSubClass;
  UINT8 PcieProgrammingInterface;
  
  UINT16 SubVenderId;
  UINT16 SubSystemId;
  
  
  UINT8 LinkSpeed;
  /* 0h: unknown.
   * 1h: 2.5 Gbps.
   * 2h: 5.0 Gbps.
   * 3h: 8.0 Gbps
   * others: reserved.
   */
  UINT8 LinkWidth;
  /* 0h: unknown.
   * 1h: x1
   * 2h: x2
   * 4h: x4
   * 8h: x8
   * 10h: x16
   * others: reserved
   */
  
  CHAR8 PcieBrandName[64];
} PACKED_PCIE_INFO;


///  The possible software prefetch mode settings.
//SGPZT23830#SyncDiskType(HDD/SSD)WithBMC+>
typedef enum  {
  DISK_HDD,     //Mechanical hard disk
  DISK_SSD,          //Solid-state drives
  DISK_UNKNOWN      //Default Value
} DISK_TYPE;
//SGPZT23830#SyncDiskType(HDD/SSD)WithBMC->
//Hard Disk info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  //<lubl_add_oemipmi spec s>
  UINT8 DATA1; //DATA1, HDD index
  UINT8 DATA2; //DATA2, reserved
  UINT8 DATA3; //DATA3, reserved
  //<lubl_add_oemipmi spec e>
  UINT8 DataValid;
  /* 00: Have no BIOS operation, 
   * 01: BIOS Send data to BMC
   */
  UINT8 HddPreset;
  UINT8 HddLocation[16];
  UINT8 HddMode;
  /* 01h: SCSI
   * 02h: IDE
   * 03h: FDD
   * 04h: IPI BUS controller
   * 05h: RAID
   * 06h: ATA ADMA
   * 07h: AHCI
   * 08h: SAS
   * 09h: NVMe
   * Others:  Others 
   */
  CHAR8 HddSN[20];
  CHAR8 HddFirmwareVersion[8];
  CHAR8 HddModelName[40];
  UINT8 HddSizeUnit;
  /* 01h: MB
   * 02h: GB
   * 03h: TB 
   */
  UINT16  HddSize;
  #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
  //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
  //UINT8 Reserved[10];
  UINT8 HDDPosition;
  UINT8 HDDControllerIndex;
  UINT8 HDDParentBus;
  UINT8 HDDParentDevice;
  UINT8 HDDParentFunction;
  //SGPZT23830#SyncDiskType(HDD/SSD)WithBMC+>
  UINT8 HDDEid;
  UINT8 HDDMediaType;
  UINT8 Reserved[2];
  //SGPZT23830#SyncDiskType(HDD/SSD)WithBMC->
  //<hujf1001-20210107 Add more Hard Disk asset info for OemMIPMICmdV1.8  ->
  #else
  UINT8 Reserved[10];
  #endif
  UINT8 HddGroupType;
  /* 
   * 
   */
  UINT8 HddStatus;
  /*
   * 
   */
  UINT8 HddRunStatus;
  /*
   * 
   */
} PACKED_OBHDD_INFO;

//NIC info struct
typedef struct {
  SET_SERVER_RESPOSITORY_HEADER SetServerRespositoryHeader;
  //<Wangyia001-20170527 fix pcie device info in BMC WebUI +>
  UINT8 NicNum;
  UINT8 Data2;
  UINT8 Data3;
  //<Wangyia001-20170527 fix pcie device info in BMC WebUI ->
  UINT8 DataValid;  
  /* 00: Have no BIOS operation, 
   * 01: BIOS Send data to BMC
   */
  UINT8 NicPresent;
  UINT8 CpuNum;
  UINT8 NicLocation[16];
  UINT8 NicPort;
  UINT8 NicCategory;
  /* 01h: LOM
   * 02h: Pcie Card
   */
  UINT8 NicSegmentNum;
  UINT8 NicParentBusNum;
  UINT8 NicParentDevNum;
  UINT8 NicParentFunNum;
  
  UINT8 NicBusNum;
  UINT8 NicDevNum;
  UINT8 NicFunNum;
  
  UINT16 NicVenderid;
  UINT16 NicDeviceId;
  
  UINT8 NicBaseClass;
  UINT8 NicSubClass;
  UINT8 NicProgrammingInterface;
  
  UINT16 NicSubVenderId;
  UINT16 NicSubSystemId;
  
  UINT8 NicLinkSpeed;
  
  UINT8 NicMacGuidSelect;
  /* 01h: MAC
   * 02h: GUID
   */
  CHAR8 NicMac[6];
  CHAR8 NicGuid[16];
  CHAR8 NicBrandName[64];

  //<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+>  
  #if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)  
    UINT16 FwVersion;             
    UINT32    FwEtid;                     
  #endif 
  //<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.->  

} PACKED_NIC_INFO;

//BIOS Version struct
typedef struct {
  UINT8 BiosMeSelect;
  /* 01: BIOS Version
   * 02: ME Version
   */
  CHAR8 StrVersion[32];
} PACKED_BIOS_VERSION;

//Memory ECC threshold struct
typedef struct {     
  UINT16 MemEccErrorThreshold;
} PACKED_MEM_ECC_THRESHOLD;

//<Xuran001-20180609-Add IPMI data structure+>
//PSU structure
typedef struct {
    UINT8   PsuPresent;             //00h: Not Present, 01h:Present
    UINT8   PsuLocation[16];        //Ascii string, PSU1...N
    UINT8   PsuVendor[16];
    UINT8   PsuVendorLocation[16];
    UINT8   PsuVendorDate[16];
    UINT8   PsuModel[16];
    UINT8   PsuRevision[16];
    UINT8   PsuPmbusRevision[16];
    UINT8   PsuSn[16];
    UINT16  PsuRatedPower;
} PACKED_OEM_IPMI_PSU_INFO;

//PSU Module State
typedef struct {
    UINT8   Present;             //00h: Not Present, 01h:Present
    UINT8   CriticalLevel;       //0-->Normal   1-->Not Critical (Sensor LNC/UNC)    2--> Critical (Sensor LC/UC)  3-->Urgency(Sensor LNR/UNR)
    UINT8   Location[16];
    UINT8   isPsuOn;
    UINT8   VinFlag;
    UINT32  Vin;        //Ex:  Real Value=220.1V    Vin= 220.1 x 1000 =  220100;
    UINT8   IinFlag;
    UINT32  Iin;
    UINT8   PinFlag;
    UINT8   Pin[2];       //Pin=Pin[0]*256+Pin[1]
    UINT8   IoutFlag;
    UINT32  Iout;
    UINT8   VoutFlag;
    UINT32  Vout;
    UINT8   PoutFlag;
    UINT8   Pout[2];      // same as Pin
    UINT8   Temp1Flag;
    UINT8   Temp1;
    UINT8   Temp2Flag;
    UINT8   Temp2;
    UINT8   Fan1SpeedFlag;
    UINT8   Fan1Speed[2];
    UINT8   Fan2SpeedFlag;
    UINT8   Fan2Speed[2];
    UINT8   ACLossFlag;
    UINT8   HotPlugFlag;
    UINT16  StatusWord;
    UINT16  PsuEventStatus;
    UINT8   OemData[16];
} PACKED_OEM_IPMI_PSU_MODULE_STATE;
//<Xuran001-20180609-Add IPMI data structure->

typedef enum {
  BiosSetup_CpuOption=0,
  BiosSetup_MemoryOption,
  BiosSetup_PowerOption,
  BiosSetup_IoOption,
  BiosSetup_ThermalConfig,
  BiosSetup_ManagememyConfig,
  BiosSetup_BootOption,
  BiosSetup_MiscOption=13,
  BiosSetup_DebugOption,
}PACKED_OEM_BIOS_SETUP_OPTIONS;

typedef  enum{
  BiosSetup_ModifiedData=0,
  BiosSetup_CurrentData,
}PACKED_OEM_BIOS_SETUP_OPTIONS_DATA_TYPE;

//SGEZT#24420 Add OEM SEL Entry For PCIe Error Record+>
//Error Type & Error Log Phase

#define   OEM_IPMI_SEL_E0_EPSR_RECORD   0xE0
typedef union 
{
  struct
  {
    /* data */
    UINT8   ErrorType:1;//AER Corrected=0;Uncorrected=1
    UINT8   Reserved_1_3:3;
    UINT8   ERPhase:1;//Error Recorded Phase BERT=0/Runtime=1
    UINT8   Reserved_5_7:3;
  }Field;
  UINT8 Value;
}ERROR_RECOED_TYPE_PHASE;

typedef union 
{
  struct
  {
    /* data */
    UINT16   Function:3;
    UINT16   Device:5;
    UINT16   Bus:8;
  }Field;
  UINT16 Value;
}ERROR_RECORD_PCIE_DEVICE;

typedef struct
{
  UINT16  RecordId;
  UINT8   RecordType;
  ERROR_RECOED_TYPE_PHASE   ERTP;
  ERROR_RECORD_PCIE_DEVICE  ERPD;
  UINT32  ErrorStatus;
  UINT8   Reserved[6];
}OEM_SEL_EPSR_E0_REQUEST;//Oem Sel Entry(E0) For Error PCIe Status Record

//SGEZT#24420 Add OEM SEL Entry For PCIe Error Record+>
#pragma pack()

EFI_STATUS
OemIpmiCmdSend(
        IN UINT8    IpmiCmd,
        IN UINT8    *CommandData,
        IN UINT8    CommandDataSize,
        OUT UINT8   *ResponseData,
        OUT UINT8   *ResponseDataSize
    );
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

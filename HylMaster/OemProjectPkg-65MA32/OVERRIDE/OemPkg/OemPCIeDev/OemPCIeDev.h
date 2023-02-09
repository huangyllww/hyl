#ifndef __PCIE_DEV_H__
#define __PCIE_DEV_H__
#include "Token.h" //Token

typedef enum {
  Speed_Undefined = 0,
  Speed_100Mbps   = 1,
  Speed_1Gbps     = 2,
  Speed_2Gbps     = 3,
  Speed_2_5Gbps   = 4,
  Speed_4Gbps     = 5,
  Speed_8Gbps     = 6,
  Speed_10Gbps    = 7,
  Speed_16Gbps    = 8,
  Speed_20Gbps    = 9,
  Speed_25Gbps    = 10,
  Speed_40Gbps    = 11,
  Speed_56Gbps    = 12,
  Speed_100Gbps   = 13,
  Speed_200Gbps   = 14,
  Speed_400Gbps   = 15,
  Speed_32Gbps    = 16,//<zhaoyf-20190327 add enum synic intel>
  Speed_50Gbps    = 17 //<zhaoyf-20190327 add enum synic intel>
}NIC_LINKSPEED;

typedef enum{
  POLICY_DISABLE = 0,
  POLICY_UEFI,
  POLICY_LEGACY,
  POLICY_BOTH,
  POLICY_OTHER
}OPROM_POLICY_TYPE;

//<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --BEGIN>
typedef enum{
    PCIDEVEN = 0,
    OPROMEN  = 1,
    SRIOVEN  = 2,
    PCILNKSPEED = 3 //<Yaosr-20201201 Set PCIe link speed by device.>
}PCIEDATA_TYPE;
//<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --END>

typedef struct _PCI_DEV_SETUP_DATA{
  VOID                    *PciSetupOpCodeHandle;
  VOID                    *PciOpRomOpCodeHandle;
  VOID                    *PciRBInfo;
  VOID                    *PciDevInfo;
  CHAR16                  Prefix[22];
  CHAR16                  BrandName[68];
  UINT16                  VarStoreId;
  UINT16                  DevEnableQid;
  UINT16                  DevStringId;
  UINT16                  DevHelpStringId;
  UINTN                   PciDevIdx;
  UINT8                   PortIdx;        //port index for current device
  UINT8                   *pPortCount;    //port count for current device 
  UINT8                   SlotNum;
  UINT8                   CpuNum;
  UINT8                   BoardType;      //1:Onboard,0:offboard
  UINT8                   DevDefValue;
  UINT8                   PortDefValue;
  UINT8                   Speed;
  UINT8                   PciDevEN;       //1:Enable,0:Disable
  UINT8                   OpROMEN;        //1:Enable,0:Disable
  UINT8                   OpExtend;
  UINT8                   PcieLocation[16];
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
  VOID                    *PcieSriovOpCodeHandle;
  UINT8                   SriovDefValue;
  UINT8                   SriovCap;
#endif
//<bieyb-20180727 add function enable SRIOV by port->
  BOOLEAN                 ShowItem;
  //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
  VOID                    *PcieLnkSpeedOpCodeHandle;
  UINT8                   PciLnkSpeed;    //Auto: 0, 2.5GT/s: 1, 5.0GT/s: 2, 8.0GT/s: 3.
  UINT8                   LnkSpeedDefValue;
#endif
  //<Yaosr-20201201 Set PCIe link speed by device. ->
} PCI_DEV_SETUP_DATA;

//<Chendl001-20190903 Pcie device list option can show device link Status +>
typedef struct _PCI_ATTRIBUTE_PARAM{
  CHAR16                  MaxSpeed[16];
  CHAR16                  MaxWidth[16];
  CHAR16                  CurrentSpeed[16];
  CHAR16                  CurrentWidth[16];
} PCI_ATTRIBUTE_PARAM;
//<Chendl001-20190903 Pcie device list option can show device link Status ->
typedef struct _PCI_DEV_IDX_DATA{
  UINTN         PciDevIdx;
  UINT8         PortCount;
  UINT8         PcieLocation[16];
  PCI_DEV_SETUP_DATA       PortData[MAX_EXTERN_PORT_COUNTS];
} PCI_DEV_IDX_DATA;

typedef struct _PCI_DEV_SETUP_FORM_DATA {
  //--Device Form--------------------
  VOID                   *StartOpCodeHandle;
  VOID                   *EndOpCodeHandle;
  VOID                   *StartLabel;
  VOID                   *EndLabel;
  UINTN                  InitStatus;
  //Like T_ITEM_LIST
  UINTN                  PciDevInitCnt;
  UINTN                  PciDevCount;    //total devices port count
  PCI_DEV_IDX_DATA	     **PciDevIdxData;
} PCI_DEV_SETUP_FORM_DATA;

typedef struct _PCI_DEV_NINFO {
  UINT16  VendorId;
  UINT16  DeviceId;
  UINT16  SubsystemID;
  UINT8   Speed;
  CHAR16  *BrandName;
} PCI_DEV_NINFO;

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
typedef struct _PCIE_NIC_INFO {
    UINT8 Speed;
    UINT8 NicMacGuidSelect;    
    CHAR8 NicMac[6];
    CHAR8 NicGuid[8];
} PCIE_NIC_INFO;

typedef struct _PCIE_DEV_INFO {
    UINT8 PcieDevicePresent;
    UINT8 BoardType;
    UINT8 CpuNum;
    UINT8 PcieLocation[16];
    
    UINT8 SegNum;
    UINT8 RbBus;
    UINT8 RbDev;
    UINT8 RbFunc;
    UINT8 BusNum;
    UINT8 DevNum;
    UINT8 FuncNum;
    
    UINT16 VenderId;
    UINT16 DeviceId;
    
    UINT8 BaseClassCode;
    UINT8 SubClassCode;    
    UINT16 SubVenderId;
    UINT16 SubSystemId;   
    
    UINT8 LinkSpeed;
    UINT8 LinkWidth;
    UINT8 PortIndex;
    CHAR8 BrandName[64];
    UINT8 *DevInfo;     
} PCIE_DEV_INFO;
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

typedef struct _OEM_PCIE_DEV_DATA_PROTOCOL  OEM_PCIE_DEV_DATA_PROTOCOL;

typedef EFI_STATUS (*EFI_GET_PCIE_DEV_DATA)(
        IN  PCI_DEV_SETUP_FORM_DATA  **PcieData
);

typedef EFI_STATUS (*EFI_GET_BOARD_SKU_DATA)(
    UINT8 *pBoardId,
    UINT8 *pSkuID
);

typedef UINT8 (*EFI_GET_DEV_DEFAULT_POLICY)(
  UINT8       BoardId,
  UINT8       SkId,
  UINTN       VarIndex
  );

typedef EFI_STATUS (*EFI_UPDATE_DEVICE_PORT_CFG)(
  PCI_DEV_SETUP_DATA *PortData
  );

typedef EFI_STATUS (*EFI_GET_DEV_INDEX)(
  VOID *Dev,
  UINTN *DevIndex
  );

typedef EFI_STATUS (*EFI_GET_DEV_PORT_INDEX)(
  VOID *Dev,
  UINTN DevIdx,
  UINT8 *PortIndex
  );

typedef EFI_STATUS (*EFI_GET_DEV_PORT_VARINDEX)(
  VOID *Dev,
  UINTN DevIdx,
  UINT8 *PortIndex,
  UINTN *PortVarIndex
  );

typedef VOID (*EFI_DEV_LIST_INIT_DONE)(
  );

//<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --BEGIN>
typedef EFI_STATUS(*EFI_GET_SET_PCIE_DEV_POLICY)(
        IN OUT VOID* PolicyData,
        IN PCIEDATA_TYPE  Type,
        IN     BOOLEAN Get
);
//<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --END>

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
typedef EFI_STATUS (*EFI_GET_PCIE_DEV_INFO)(
        IN  PCI_DEV_SETUP_FORM_DATA  *PcieData,
        UINTN DevIndex,
        UINT8 PortIndex,
        PCIE_DEV_INFO *PcieDevInfo
);

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

//<licm-20210513 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
typedef EFI_STATUS (*EFI_UPDATE_ROOT_BRIGE_TABLE)(
        VOID *Dev
);

typedef EFI_STATUS (*EFI_GET_ROOT_BRIGE_TABLE_IN_VAR)(
        VOID
);
#endif
//<licm-20210513 Optimization PCIE Device List module ->

struct _OEM_PCIE_DEV_DATA_PROTOCOL
{
  EFI_GET_BOARD_SKU_DATA      GetBoardSkuId;
  EFI_GET_PCIE_DEV_DATA       GetPcieDevList;
  EFI_GET_DEV_DEFAULT_POLICY  GetDeviceDefaultPolicy;
  EFI_GET_DEV_DEFAULT_POLICY  GetDevicePortDefaultPolicy;
  EFI_GET_DEV_INDEX           GetDeviceIndex;
  EFI_GET_DEV_PORT_INDEX      GetDevicePortIndex;
  EFI_GET_DEV_PORT_VARINDEX   GetDevicePortVarIndex;
  EFI_UPDATE_DEVICE_PORT_CFG  UpdateDevicePortCfg;
  EFI_DEV_LIST_INIT_DONE      DeviceListInitDone;
  EFI_GET_PCIE_DEV_INFO       GetPcieDevInfo;//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message >
  //<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --BEGIN>
  EFI_GET_SET_PCIE_DEV_POLICY GetSetPcieDevPolicy;
  //<Yuanshai-20190710 For :Get/Set "OEMPCIE_DEV" Variable --END>
  //<licm-20210513 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  EFI_UPDATE_ROOT_BRIGE_TABLE       DynamicGenerationRbtable;
  EFI_GET_ROOT_BRIGE_TABLE_IN_VAR   GetTableListInVariable;
#endif
  //<licm-20210513 Optimization PCIE Device List module ->
};

#define OPTION_AUTO_POLICY       4      //Option value for auto

#endif

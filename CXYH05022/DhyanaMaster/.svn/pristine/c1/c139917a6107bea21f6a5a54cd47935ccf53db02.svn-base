/** @file
 * OemPCIeDevPlatLib.h
  This file contains the definition of Oem PCIe Device platform Library Function Definition
  
  Author:lvych
  Date:  20170803
  
  changes for AMD Naples Platform by wangyia
  Date: 20171221
  
Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR> 
 
**/

#ifndef _EFI_OEM_PCIE_DEV_PLAT_LIB_H_
#define _EFI_OEM_PCIE_DEV_PLAT_LIB_H_

#include "OemPCIeDev.h"
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
#include <Protocol/PciIo.h>
#include "PCIEXPRESS.h"
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

//
// Data Type
//
typedef struct {
  UINT8   RbType; //0:SB,1:NB,other:other
  UINT8   RbSeg;
  UINT8   RbBus;
  UINT8   RbDev;
  UINT8   RbFunc;
  UINT8   Data1;
  UINT8   Data2;
  UINT8   Data3;
} ROOTB_DATA;

typedef struct {
  UINT8   RbType;     //0:SB,1:NB,other:other
  UINT8   RbSeg;
  UINT8   RbBus;
  UINT8   RbDev;
  UINT8   RbFunc;
  UINT8   Socket;
  UINT8   DieNum;
  UINT8   Data3;  
} ROOTB_NAPLES_DATA;

//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
#define AUTO_SET_VALUE    1
#define FORCE_SET_VALUE   10

typedef struct _PCIeListNode{
  UINT8                 Index;
  UINT8                 DevEn;     
  ROOTB_DATA            PciPortInfo;
  struct _PCIeListNode *Next;
} PCIeListLinkNode;

typedef struct {
  UINT8       Sigindex;
  ROOTB_DATA  SigPciRootPortTable;
  UINT16      VendorId;
  UINT16      DeviceId;
  UINT8       DevEn;
} ROOTB_INDEX_TABLE;

typedef struct {
  UINT8              isUpdate;
  ROOTB_INDEX_TABLE  PciRootPortTable[MAX_DEV_COUNT];
} ROOTB_VAR_TABLE;
#endif
//<licm-20210531 Optimization PCIE Device List module ->
//
// Slot Attributes
//
typedef union {
  struct{
    UINT8  BoardType:1;   //BIT0: 1:Onboard,0:offboard
    UINT8  HideItem:1;    //BIT1
    UINT8  Reserved:6;    //BIT2-7
  } Bits;
  UINT8    Data8;
} SLOT_ATR;

typedef struct _PCI_SLOT_MAP
{
  UINT8   SkID;       //oem define for identify,0xFF ignore
  UINT8   SltAtr;     //oem define for identify
  UINT8   Id1;
  UINT8   Id2;
  UINT8   Id3;
  UINT8   Id4;
  UINT8   PortIndex;
  UINT8   DevDefValue;  //0xFF ignore
  UINT8   PortDefValue; //0xFF ignore
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
  UINT8   LnkSpeedDefValue;//0xFF ignore //<Yaosr-20201201 Set PCIe link speed by device.>
#endif
}PCI_SLOT_MAP;

typedef struct _PCI_NAPLES_SLOT_MAP{
  UINT8     SkID;       //oem define for identify,0xFF ignore
  SLOT_ATR  SltAtr;     //BIT0:0->offboard,1->Onboard;BIT1:0->ShowItem,1->HideItem
  UINT8     Socket;
  UINT8     DieNumber;
  UINT8     RbDev;
  UINT8     RbFunc;
  UINT8     PortIndex;
  UINT8     DevDefValue;  //0xFF ignore
  UINT8     PortDefValue; //0xFF ignore
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
  UINT8     LnkSpeedDefValue;//0xFF ignore //<Yaosr-20201201 Set PCIe link speed by device.>
#endif
}PCI_NAPLES_SLOT_MAP;

//
// Library functions
//
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
UINT8 PcieFindCapId(
  EFI_PCI_IO_PROTOCOL *pPciIo,
  UINT8 CapId
);
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

EFI_STATUS GetBoardSkuId(
    UINT8 *pBoardId,
    UINT8 *pSkuID
);

VOID PrintRbTable();

UINT8 GetDeviceDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    );

UINT8 GetDevicePortDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    );
//<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
UINT8 GetDeviceLnkSpeedDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    );
#endif
//<Yaosr-20201201 Set PCIe link speed by device. ->

EFI_STATUS UpdateDevicePortCfg(PCI_DEV_SETUP_DATA *PortData);

EFI_STATUS GetDeviceIndexLib(
		VOID *Dev,
		UINTN *DevIndex
		);
EFI_STATUS GetDevicePortIndexLib(
		PCI_DEV_SETUP_FORM_DATA *PciDevListData,
		VOID *Dev,
		UINTN DevIdx,
		UINT8 *PortIndex);

EFI_STATUS GetDevicePortVarIndexLib(
    PCI_DEV_SETUP_FORM_DATA *PciDevListData,
    VOID *Dev,
    UINTN DevIdx,
    UINT8 *PortIndex,
    UINTN *PortVarIndex
    );

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
EFI_STATUS GetPcieDevInfo(
    PCI_DEV_SETUP_FORM_DATA *PciDevListData,
    UINTN DevIndex,
    UINT8 PortIndex,
    PCIE_DEV_INFO *PcieDevInfo
);
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

//<licm-20210513 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
EFI_STATUS DynamicGenerationRbtable(VOID *Dev);
EFI_STATUS GetTableListInVariable(VOID);
#endif

EFI_STATUS UpdateTableListInVariable(VOID);

//<licm-20210513 Optimization PCIE Device List module ->
#endif

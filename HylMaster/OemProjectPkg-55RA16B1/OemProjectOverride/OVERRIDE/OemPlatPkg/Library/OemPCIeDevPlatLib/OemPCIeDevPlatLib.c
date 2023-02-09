/** @file
  This file contains OemPCIeDevPlatLib Use to Support OemPCIeDev Module
  
  Author:lvych
  Date:  20170803
  
  changes for AMD Naples Platform by wangyia
  Date: 20171221
  
Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR> 
 
**/
#include <Token.h> //Token
#include <Efi.h>
#include <UefiHii.h>
#include <AmiDxeLib.h>
#include <PciBus.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/OemPCIeDevPlatLib.h>
#include <Library/PciLib.h>
#include "PciSlotMapList.h"
#include "OemPCIeDev.h"
#include <Protocol/FabricTopologyServices.h>
#include <Library/UefiBootServicesTableLib.h>
#include <OemSmbios.h>//dengyh 20211231 smbios type 9 +

EFI_SMBIOS_PROTOCOL           *Smbios = NULL;//dengyh 20211231 smbios type 9 +
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)

extern EFI_RUNTIME_SERVICES   *gRT;

#define AMD_VENDOR_ID                  0x1022
#define HIGON_VENDOR_ID                0x1D94
#define DYNAMIC_ROOTB_TABLE_GUID \
    {0x8ee0bbad, 0xcc61, 0x4fab, 0x8b, 0xe2, 0x92, 0x44, 0xbc, 0x78, 0x57, 0x3c}
PCIeListLinkNode gPCIeLinkListEntry;
ROOTB_VAR_TABLE gDynamicVarPciRootPortTable;
UINT16 CPU_INTERNAL_DEVICES_IDTABLE[]={0x1450,0x1451,0x1452,0x1453,0x1454,0x145A,0x1456,0x145F,0x1455,0x1468,0x7901,0x790B,0x790E};
#endif
//<licm-20210531 Optimization PCIE Device List module ->

PCI_SLOT_MAP gPciSlotMapTable[] = { PCI_SLOT_MAP_LIST };

#if NSOCKETS == 1
ROOTB_DATA PciRootPortTable[MAX_DEV_COUNT] = {

  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //Die   //Data3
  //PCH
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x00,     0xFF}, //(S0D0P0,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x00,     0xFF},

  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x00,     0xFF}, //(S0D0G0,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x00,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x01,     0xFF}, //(S0D1P1,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x01,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x01,     0xFF}, //(S0D1G1,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x01,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x02,     0xFF}, //(S0D2P2,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x02,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x02,     0xFF}, //(S0D2G2,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x02,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x03,     0xFF}, //(S0D3P3,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x03,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x03,     0xFF}, //(S0D3G3,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x03,     0xFF},

};
#else
ROOTB_DATA PciRootPortTable[MAX_DEV_COUNT] = {
#if !defined(H520_AK_SUPPORT)
  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //Die   //Data3
  //PCH
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x00,     0xFF}, //(S0D0P0,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x00,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x01,     0xFF}, //(S0D1P1,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x01,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x02,     0xFF}, //(S0D2G2,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x02,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x03,     0xFF}, //(S0D3G3,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x03,     0xFF},

  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x04,     0xFF}, //(S1D0P0,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x04,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x05,     0xFF}, //(S1D1P1,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x05,     0xFF},
 
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x06,     0xFF}, //(S1D2G2,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x06,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x07,     0xFF}, //(S1D3G3,typeB)
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x07,     0xFF},
#else
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x00,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x00,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x00,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x00,     0xFF},  
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x01,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x01,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x01,     0xFF},   
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x02,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x02,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x02,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x02,     0xFF},   
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x03,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x03,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x03,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x03,     0xFF},    
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x04,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x04,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x04,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x04,     0xFF},  
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x05,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x01,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x05,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x05,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x05,     0xFF},   
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x06,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x06,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x06,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x06,     0xFF},   
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x07,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x03,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x04,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x05,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x06,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x01,     0x07,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x02,     0x01,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x01,     0xFF,     0x07,     0xFF}, 
  {0x00,     0x00,     0xFF,     0x03,     0x02,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x03,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x04,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x05,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x06,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x03,     0x07,     0xFF,     0x07,     0xFF},
  {0x00,     0x00,     0xFF,     0x04,     0x01,     0xFF,     0x07,     0xFF},     
#endif  
};
#endif

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
//-----------------------------------------------------------------------
// PCIE LAN: Ethernet MAC address     From Intel LAN Spec
// Receive Address Low - RAL (0x5400 + 8*n [n=0...15]; 0x54E0 + 8*n [n=0...15]; R/W)
// Receive Address Low - RAL[n] (0x0A200 + 8*n,n=0...127; RW)
// While "n" is the exact unicast/multicast address entry and it is equals to 0,1,...127.
//-----------------------------------------------------------------------
//1 Gbps
#define  RAL_82574 	          0x5400    
//Intel 82574 GbE Controller Family Datasheet
//DeviceID 0x10D3
#define  RAL_82576 	          0x5400    
//Intel 82576 Gigabit Ethernet Controller Datasheet
//DeviceID 0x10C9 0x10CA 0x10E6 0x10E7
#define  RAL_82580	          0x5400    
//Intel 82580 Quad/Dual Gigabit Ethernet LAN Controller Datasheet 
//DeviceID 0x1509 0x150E 0x150F 0x1510 0x1511 0x1516
#define  RAL_I210             0x5400    
//Intel Ethernet Controller I210 Datasheet 
//DeviceID 0x1531 0x1533 0x1534 0x1536 0x1537 0x1538
#define  RAL_I350 	          0x5400    
//Intel Ethernet Controller I350 Datasheet 
//DeviceID 0x151F 0x1521 0x1522 0x1523 0x1524 
//10 Gbps
#define  RAL_82599 	          0xA200    //Intel 82599 10 GbE Controller Datasheet  
//DeviceID 0x10D8  
#define  RAL_X540 	          0x5400    //Intel Ethernet Controller X540 Datasheet  
//DeviceID 0x1512

//<ChengXW001-20170821 BMC Web show Intel X710 NIC info +>
#define  RAL_X710beLow        0x1E4440  //Intel Ethernet Controller X710 Datasheet
#define  RAL_X710beHigh       0x1E44C0  //Intel Ethernet Controller X710 Datasheet
//<ChengXW001-20170821 BMC Web show Intel X710 NIC info ->

//<Sunhp001-20180103-fix issue#17562 sometimes after flash bios,bmc show x722 mac 000000000317,but os show normal+>
#define  RAL_PCH10GbeLow      0x1E4440
#define  RAL_PCH10GbeHigh     0x1E44C0
//<Sunhp001-20180103-fix issue#17562 sometimes after flash bios,bmc show x722 mac 000000000317,but os show normal->

#define EFI_PCIE_EX_CAPABILITY_ID_SN  0x0003 //<Lizhq-20190515 Get MAC address for Broadcom network device>

//<Xuran001-20181031-Get MLX NIC MAC by VSC.+>
#define EFI_PCI_CAPABILITY_ID_VENDOR  0x09

#define MLX_VSC_SPACE_OFFSET        0x04
#define MLX_VSC_COUNTER_OFFSET      0x08
#define MLX_VSC_SEMAPHORE_OFFSET    0x0C
#define MLX_VSC_MAC_H_OFFSET        0x10
#define MLX_VSC_MAC_L_OFFSET        0x14

#define MLX_VSC_SPACE_MAC_ID        0x0F
#define PCI_CLASS_NETWORK           0x02
#define PCI_CLASS_NETWORK_ETHERNET  0x00

//<xuran-20171130 +> BMC Web can't show Intel OPA Card GUID
#define  CCE                        0x000000000000
#define  CCE_DC_CTRL                (CCE + 0x0000000000B8)
#define  DC_TOP_CSRS                0x000000600000
#define  DC_8051_CSRS               (DC_TOP_CSRS + 0x000000002000)
#define  DC_DC8051_CFG_LOCAL_GUID   (DC_8051_CSRS + 0x000000000038)
//<xuran-20171130 -> BMC Web can't show Intel OPA Card GUID

//<Xuran001-20181031-Get MLX NIC MAC by VSC.->
#define   PCI_CLASS_NETWORK_IB      0x07 //<Songts032_20190319-Fill all 0xFF to MLX IB MAC under legacy mode.+>
//
// Extern Variables
//
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

EFI_STATUS GetDeviceRootPortData(VOID *Dev,ROOTB_DATA *Rb,UINTN *DevIndex);
EFI_STATUS GetDevRootPortIndexLib(ROOTB_DATA *Rb,UINTN *RPIndex);
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
EFI_STATUS GetDevRootPortIndexAndDevEnLib(ROOTB_DATA *Rb,UINTN *RPIndex,UINT8 *DevEn);
EFI_STATUS GetDeviceRootPortData2(VOID *Dev,ROOTB_DATA *Rb,UINTN *DevIndex,UINT8 *DevEn);
VOID isUpdateRbInVar(PCIeListLinkNode *listEntry,ROOTB_VAR_TABLE *VarPciRootPortTable);
EFI_STATUS isPCIeLinkListData(PCIeListLinkNode *listEntry,VOID *Data,UINTN *Index,UINT8 *DevEN);
EFI_STATUS GetDeviceRootBrgEX(VOID *Dev,VOID **rBrg);
EFI_STATUS GetDeviceRootBrg(VOID *Dev,VOID **rBrg);
#endif
//<licm-20210531 Optimization PCIE Device List module ->
VOID
AsciiToUnicodeSize (
  OUT CHAR16            *UStr,
  IN UINTN              Size,
  IN CHAR8              *AStr
  )
{
  UINTN Idx;

  if(UStr == NULL || AStr == NULL)
    return;

  Idx = 0;
  while (AStr[Idx] != 0) {
    UStr[Idx] = (CHAR16) AStr[Idx];
    if (Idx == Size) {
      break;
    }

    Idx++;
  }
  UStr[Idx] = 0;
}

EFI_STATUS GetBoardSkuId(
    UINT8 *pBoardId,
    UINT8 *pSkuID
    )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  //TODO
  if(pBoardId) *pBoardId = 0;
  
  if(pSkuID) *pSkuID = 0;
  //TODO
  
  return Status;
}

CHAR8 *GetDevicePrefix(
  UINT8 BoardId,
  UINT8 SkId,
  ROOTB_DATA  RbData,
  PCI_DEV_SETUP_DATA *PortData
  )
{
  CHAR8             *PciePrefix = "SlotXX";

  if ( NULL == PortData)
    return  NULL;
  
  if (PortData->BoardType) {//onboard device
    return "Embedded";
  }

  PciePrefix[4] = PortData->SlotNum/10 + '0';
  PciePrefix[5] = PortData->SlotNum%10 + '0';  
   
  return PciePrefix;
    
}
//<huangrj-20220516 display pcie device slot num. +>
UINT16 GetDeviceSmBiosT9SlotID(
        UINT8 BoardId,
        UINT8 SkId,
        ROOTB_DATA  RbData,
        PCI_DEV_SETUP_DATA *PortData
        )
      {
        UINT8                         Bus, Dev, Func;
        EFI_SMBIOS_HANDLE             SmbiosHandle;
        EFI_SMBIOS_TYPE               SmbiosType;
        SMBIOS_TABLE_TYPE_9           *SmbiosType9Record;
        EFI_SMBIOS_TABLE_HEADER       *SmbiosRecord;
        CHAR8                         *SlotDesignationAscii = NULL;
        UINT8                         i;
        EFI_STATUS                    Status = EFI_SUCCESS;
        UINT16                         SmBiosT9SlotID = 0;

        if ( NULL == PortData)
          return  0;
        
        if (Smbios == NULL)
        {
            Status = gBS->LocateProtocol (
                            &gEfiSmbiosProtocolGuid,
                            NULL,
                            (VOID **) &Smbios
                            );
            DEBUG((EFI_D_ERROR, "%a gEfiSmbiosProtocolGuid Status: %r\n", __FUNCTION__, Status));
        }

        if(!EFI_ERROR(Status))
        {
            SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
            SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
            Bus = ((PCI_DEV_INFO *)(PortData->PciDevInfo))->Address.Addr.Bus;
            Dev = ((PCI_DEV_INFO *)(PortData->PciDevInfo))->Address.Addr.Device;
            Func = ((PCI_DEV_INFO *)(PortData->PciDevInfo))->Address.Addr.Function;
            
            for (i = 0; ; ++i) {
              Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
              if (EFI_ERROR(Status)) {
                break;
              }
              SmbiosType9Record = (SMBIOS_TABLE_TYPE_9 *) SmbiosRecord;
              if ((SmbiosType9Record->BusNum == Bus)
                      && ((UINT8)(SmbiosType9Record->DevFuncNum >> 3) == Dev)
                      && ((SmbiosType9Record->DevFuncNum & 0x7) == Func))
              {
                  SmBiosT9SlotID=SmbiosType9Record->SlotID;
              }
            }
        }
        
        return SmBiosT9SlotID;
          
      }
//<huangrj-20220516 display pcie device slot num. ->
UINT8 GetDeviceSlotAtrByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    )
{
  EFI_STATUS              Status;
  PCI_NAPLES_SLOT_MAP     *PCISlotEntry;
  ROOTB_DATA              RbData;
  ROOTB_NAPLES_DATA       *pRb;
  UINTN                   i;
  UINTN                   dVarIndex;
  UINT8                   SlotAtr = 0;
  
  
  pRb = (ROOTB_NAPLES_DATA *)&RbData;
  PCISlotEntry = (PCI_NAPLES_SLOT_MAP *)&gPciSlotMapTable[0];
  for(i=0;i<sizeof(gPciSlotMapTable) / sizeof(PCI_SLOT_MAP);PCISlotEntry++,i++){

    pRb->RbSeg = 0;
    pRb->RbBus = 0;
    pRb->RbDev = PCISlotEntry->RbDev;
    pRb->RbFunc = PCISlotEntry->RbFunc;
    pRb->Socket = 0xFF;
    pRb->DieNum = PCISlotEntry->DieNumber;
    pRb->Data3 = 0xFF;
    
    Status = GetDevRootPortIndexLib(&RbData,&dVarIndex);
    if(EFI_ERROR(Status)) {
      continue;
    }

    if((PCISlotEntry->SkID == SkId || PCISlotEntry->SkID == 0xFF) && (dVarIndex == VarIndex)){
      SlotAtr = PCISlotEntry->SltAtr.Data8;
      
      break;
    }

  };

  return SlotAtr;
  
}

UINT8 GetDeviceDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    )
{
  EFI_STATUS              Status;
  PCI_NAPLES_SLOT_MAP     *PCISlotEntry;
  ROOTB_DATA              RbData;
  ROOTB_NAPLES_DATA       *pRb;
  UINTN                   i;
  UINTN                   dVarIndex;
  UINT8                   DevDefaultVal = PCI_SETUP_DEFAULT_VALUE;
  
  
  pRb = (ROOTB_NAPLES_DATA *)&RbData;
  PCISlotEntry = (PCI_NAPLES_SLOT_MAP *)&gPciSlotMapTable[0];
 
  for(i=0;i<sizeof(gPciSlotMapTable) / sizeof(PCI_SLOT_MAP);PCISlotEntry++,i++){

    pRb->RbSeg = 0;
    pRb->RbBus = 0;
    pRb->RbDev = PCISlotEntry->RbDev;
    pRb->RbFunc = PCISlotEntry->RbFunc;
    pRb->Socket = 0xFF;
    pRb->DieNum = PCISlotEntry->DieNumber;
    pRb->Data3 = 0xFF;
    
    Status = GetDevRootPortIndexLib(&RbData,&dVarIndex);
    if(EFI_ERROR(Status)) {
      continue;
    }
 

    if((PCISlotEntry->SkID == SkId || PCISlotEntry->SkID == 0xFF) && (dVarIndex == VarIndex)){
      DevDefaultVal = PCISlotEntry->DevDefValue;
      
      break;
    }
  };
  

  if(DevDefaultVal == 0xFF)
    DevDefaultVal = PCI_SETUP_DEFAULT_VALUE;

  return DevDefaultVal;
  
}

UINT8 GetDevicePortDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    )
{
  EFI_STATUS            Status;
  PCI_NAPLES_SLOT_MAP   *PCISlotEntry;
  ROOTB_DATA            RbData;
  ROOTB_NAPLES_DATA     *pRb;
  UINTN                 i;
  UINTN                 dVarIndex;
  UINTN                 tVarIndex;
  UINT8                 PortDefaultVal = PCI_OPROM_DEFAULT_VALUE;

  
  PCISlotEntry = (PCI_NAPLES_SLOT_MAP *)&gPciSlotMapTable[0];
  pRb = (ROOTB_NAPLES_DATA *)&RbData;

  for(i=0;i<sizeof(gPciSlotMapTable) / sizeof(PCI_SLOT_MAP);PCISlotEntry++,i++){

    pRb->RbSeg = 0;
    pRb->RbBus = 0;
    pRb->RbDev = PCISlotEntry->RbDev;
    pRb->RbFunc = PCISlotEntry->RbFunc;
    pRb->Socket = 0xFF;
    pRb->DieNum = PCISlotEntry->DieNumber;
    pRb->Data3 = 0xFF;
    
    Status = GetDevRootPortIndexLib(&RbData,&dVarIndex);
    if(EFI_ERROR(Status)) {
      continue;
    }
    
    tVarIndex = dVarIndex*MAX_DEV_PORT + PCISlotEntry->PortIndex;

    
    if((PCISlotEntry->SkID == SkId || PCISlotEntry->SkID == 0xFF) && (tVarIndex == VarIndex) && (PCISlotEntry->PortIndex != 0xFF)){
      PortDefaultVal = PCISlotEntry->PortDefValue;
      break;
    }

  };

  if(PortDefaultVal == 0xFF)
    PortDefaultVal = PCI_OPROM_DEFAULT_VALUE;
  
  return PortDefaultVal;  
}

//<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
UINT8 GetDeviceLnkSpeedDefValueByVarIdx(
    UINT8       BoardId,
    UINT8       SkId,
    UINTN       VarIndex
    )
{
  EFI_STATUS              Status;
  PCI_NAPLES_SLOT_MAP     *PCISlotEntry;
  ROOTB_DATA              RbData;
  ROOTB_NAPLES_DATA       *pRb;
  UINTN                   i;
  UINTN                   dVarIndex;
  //UINT8                   DevDefaultVal = PCI_SETUP_DEFAULT_VALUE;
  UINT8                   DevDefaultVal = 0;

  pRb = (ROOTB_NAPLES_DATA *)&RbData;
  PCISlotEntry = (PCI_NAPLES_SLOT_MAP *)&gPciSlotMapTable[0];

  for(i=0;i<sizeof(gPciSlotMapTable) / sizeof(PCI_SLOT_MAP);PCISlotEntry++,i++){

    pRb->RbSeg = 0;
    pRb->RbBus = 0;
    pRb->RbDev = PCISlotEntry->RbDev;
    pRb->RbFunc = PCISlotEntry->RbFunc;
    pRb->Socket = 0xFF;
    pRb->DieNum = PCISlotEntry->DieNumber;
    pRb->Data3 = 0xFF;

    Status = GetDevRootPortIndexLib(&RbData,&dVarIndex);
    if(EFI_ERROR(Status)) {
      continue;
    }

    if((PCISlotEntry->SkID == SkId || PCISlotEntry->SkID == 0xFF) && (dVarIndex == VarIndex)){
      DevDefaultVal = PCISlotEntry->LnkSpeedDefValue;
      break;
    }
  };

  if(DevDefaultVal == 0xFF)
    DevDefaultVal = 0;

  return DevDefaultVal;
}
#endif
//<Yaosr-20201201 Set PCIe link speed by device. ->
EFI_STATUS UpdateDevicePortCfg(PCI_DEV_SETUP_DATA *PortData)
{
  EFI_STATUS        Status;
  ROOTB_DATA        RbData;
  UINTN             DevIdx;
  UINTN             PortVarIdx;
  CHAR8             *pPrefix;
  UINT8             BoardId=0;
  UINT8             SkID = 0;
  SLOT_ATR          SlotAtr;
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  UINT8             DevEn;
#endif
//<licm-20210531 Optimization PCIE Device List module ->

  if ( NULL == PortData)
     return  EFI_INVALID_PARAMETER;
  
  Status = GetBoardSkuId(&BoardId,&SkID);
  
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  Status = GetDeviceRootPortData2(PortData->PciDevInfo,&RbData,&DevIdx,&DevEn);
#else
  Status = GetDeviceRootPortData(PortData->PciDevInfo,&RbData,&DevIdx);
#endif
//<licm-20210531 Optimization PCIE Device List module ->
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  PortVarIdx = PortData->PciDevIdx * MAX_DEV_PORT + PortData->PortIdx;
  SlotAtr.Data8 = GetDeviceSlotAtrByVarIdx(BoardId,SkID,PortData->PciDevIdx);
  PortData->BoardType = SlotAtr.Bits.BoardType;
  PortData->ShowItem = (SlotAtr.Bits.HideItem ? FALSE : TRUE);
  PortData->DevDefValue = GetDeviceDefValueByVarIdx(BoardId,SkID,PortData->PciDevIdx);
  PortData->PortDefValue = GetDevicePortDefValueByVarIdx(BoardId,SkID,PortVarIdx);
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  if(DevEn == FORCE_SET_VALUE){
     PortData->PciDevEN = 1;
  }
#endif
//<licm-20210531 Optimization PCIE Device List module ->
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
  PortData->LnkSpeedDefValue = GetDeviceLnkSpeedDefValueByVarIdx(BoardId,SkID,PortData->PciDevIdx);//<Yaosr-20201201 Set PCIe link speed by device.>
#endif
  PortData->CpuNum = ((ROOTB_NAPLES_DATA *)&RbData)->Socket;
  
  //GetDevicePrefix
  pPrefix = GetDevicePrefix(BoardId,SkID,RbData,PortData);
  
  PortData->SmBiosT9SlotID=GetDeviceSmBiosT9SlotID(BoardId,SkID,RbData,PortData);
  
  if(pPrefix) AsciiToUnicodeSize(PortData->Prefix,20,pPrefix);

  return Status;
}

//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
EFI_STATUS GetDevRootPortIndexLib(ROOTB_DATA *Rb,UINTN *RPIndex)
{
  UINT8      dDevEn;
  UINTN      i;
  EFI_STATUS Status;
  if (NULL == Rb || NULL == RPIndex)
     return  EFI_INVALID_PARAMETER;

  Status = isPCIeLinkListData(&gPCIeLinkListEntry,Rb,&i,&dDevEn);
  if(Status == EFI_SUCCESS) {
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
    DEBUG((EFI_D_ERROR,"[OemDbg]isPCIeLinkListData index=%ld\n",i));
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->
    *RPIndex = i;
  }
  return  Status;

}
#else
EFI_STATUS GetDevRootPortIndexLib(ROOTB_DATA *Rb,UINTN *RPIndex)
{
  ROOTB_DATA      *PCIRBEntry;
  ROOTB_DATA      *pRootPortlist;
  UINTN           RpCount;
  UINTN           i;

//<licm-20210513 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  pRootPortlist = gDynamicPciRootPortTable.PciRootPortTable;
  RpCount = sizeof(gDynamicPciRootPortTable.PciRootPortTable) / sizeof(ROOTB_DATA);
#else
  pRootPortlist = PciRootPortTable;
  RpCount = sizeof(PciRootPortTable) / sizeof(ROOTB_DATA);
#endif
//<licm-20210513 Optimization PCIE Device List module ->
  if ( NULL == pRootPortlist || NULL == Rb || NULL == RPIndex)
     return  EFI_INVALID_PARAMETER;
  
  PCIRBEntry = pRootPortlist;
  for( i = 0;i < RpCount;i++ ){

    if(((PCIRBEntry->RbSeg == Rb->RbSeg)||(PCIRBEntry->RbSeg == 0xFF))
        &&((PCIRBEntry->RbBus == Rb->RbBus)||(PCIRBEntry->RbBus == 0xFF))
        &&((PCIRBEntry->RbDev == Rb->RbDev)||(PCIRBEntry->RbDev == 0xFF))
        &&((PCIRBEntry->RbFunc == Rb->RbFunc)||(PCIRBEntry->RbFunc == 0xFF))
        &&((PCIRBEntry->Data1 == Rb->Data1)||(PCIRBEntry->Data1 == 0xFF))
        &&((PCIRBEntry->Data2 == Rb->Data2)||(PCIRBEntry->Data2 == 0xFF))
        &&((PCIRBEntry->Data3 == Rb->Data3)||(PCIRBEntry->Data3 == 0xFF)))
    {
      Rb->RbType = PCIRBEntry->RbType;
      *RPIndex = i;
       break;
    }
    
    PCIRBEntry++;
  };

  if(i == RpCount)
    return EFI_NOT_FOUND;

  return  EFI_SUCCESS;

}
#endif
//<licm-20210531 Optimization PCIE Device List module ->

//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
EFI_STATUS PCIeLinkedListInit(PCIeListLinkNode *listEntry)
{
  if(listEntry == NULL) {
    return  EFI_INVALID_PARAMETER;
  }
  listEntry->Next=NULL;
  ZeroMem(&listEntry->PciPortInfo, sizeof(ROOTB_DATA));
  return EFI_SUCCESS;
}

EFI_STATUS PCIeLinkedListAddHead(PCIeListLinkNode *listEntry,VOID *Data)
{
  ROOTB_DATA *Prb;
  PCIeListLinkNode *PNode = NULL;
  if(listEntry == NULL || Data == NULL) {
    return  EFI_INVALID_PARAMETER;
  }
  Prb = (ROOTB_DATA *)Data;
  PNode = MallocZ(sizeof(PCIeListLinkNode));
  if(PNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  MemCpy(&PNode->PciPortInfo,Prb,sizeof(ROOTB_DATA));
  PNode->Next = listEntry->Next;
  listEntry->Next=PNode;
  return EFI_SUCCESS;
}

EFI_STATUS PCIeLinkedListAddTail(PCIeListLinkNode *listEntry,VOID *Data,UINT8 DevEn,UINTN PortIndex)
{
  ROOTB_DATA *Prb;
  PCIeListLinkNode *PNode = NULL;
  PCIeListLinkNode *PHead = NULL;
  if(listEntry == NULL || Data == NULL) {
    return  EFI_INVALID_PARAMETER;
  }
  PHead = listEntry;
  Prb = (ROOTB_DATA *)Data;
  PNode = MallocZ(sizeof(PCIeListLinkNode));
  if(PNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  MemCpy(&PNode->PciPortInfo,Prb,sizeof(ROOTB_DATA));
  PNode->Index = (UINT8)PortIndex;
  PNode->DevEn = (UINT8)DevEn;
  while(PHead->Next!=NULL) {
    PHead = PHead->Next;
  }

  PHead->Next = PNode;
  PNode->Next = NULL;
  return EFI_SUCCESS;
}

EFI_STATUS isPCIeLinkListData(PCIeListLinkNode *listEntry,VOID *Data,UINTN *Index,UINT8 *DevEN)
{
  EFI_STATUS         Status = EFI_NOT_FOUND;
  ROOTB_DATA         *Prb;
  PCIeListLinkNode   *PNode = NULL;
  if(listEntry == NULL || Data == NULL) {
    return  EFI_INVALID_PARAMETER;
  }
  PNode = listEntry->Next;
  Prb = (ROOTB_DATA *)Data;
  while(PNode!=NULL) {
    if(MemCmp(&PNode->PciPortInfo,Prb,sizeof(ROOTB_DATA))==0) {
      *Index = PNode->Index;
      *DevEN = PNode->DevEn;
      Status = EFI_SUCCESS;
      break;
    }
    PNode=PNode->Next;
  }
  return Status;
}

EFI_STATUS DynamicRbIndexInVariable(VOID)
{
    UINTN         i;
    UINTN         Size;
    UINT32        Attribute;
    EFI_GUID      DynamicRbTableGuid = DYNAMIC_ROOTB_TABLE_GUID;
    EFI_STATUS    Status;
    Size = sizeof(ROOTB_VAR_TABLE);
    Status=gRT->GetVariable (L"DynamicPciRootPortTable", &DynamicRbTableGuid, &Attribute, &Size, &gDynamicVarPciRootPortTable);
    if (EFI_ERROR(Status)) {
            //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
    DEBUG((EFI_D_ERROR,"[OemDbg]DynamicPciRootPortTable_GetVariable %r\n",Status));
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

        gDynamicVarPciRootPortTable.isUpdate=1;
        for(i=0;i<MAX_DEV_COUNT;i++) {
          gDynamicVarPciRootPortTable.PciRootPortTable[i].Sigindex=0xFF;
        }
    }
    return Status;
}

EFI_STATUS UpdateTableListInVariable(VOID)
{
  EFI_STATUS    Status = EFI_NOT_FOUND;
  UINTN         Size;
  UINT32        Attribute;
  EFI_GUID      DynamicRbTableGuid = DYNAMIC_ROOTB_TABLE_GUID;
  Attribute = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  Size = sizeof(ROOTB_VAR_TABLE);
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
    DEBUG((EFI_D_ERROR,"[OemDbg]UpdateTableListInVariable...\n"));
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

  isUpdateRbInVar(&gPCIeLinkListEntry,&gDynamicVarPciRootPortTable);
  if(gDynamicVarPciRootPortTable.isUpdate) {
      gDynamicVarPciRootPortTable.isUpdate=0;
      Status=gRT->SetVariable (L"DynamicPciRootPortTable", &DynamicRbTableGuid, Attribute, Size, &gDynamicVarPciRootPortTable);
      //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
      DEBUG((EFI_D_ERROR,"[OemDbg]SetVariable %r\n",Status));
      //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

    }
  return Status;
}

EFI_STATUS GetRootPortIndexInVar(VOID *Data,UINTN *index)
{
  UINTN                     i;
  EFI_STATUS                Status = EFI_NOT_FOUND;
  ROOTB_INDEX_TABLE         *Prb;
  if(Data == NULL) {
    return  EFI_INVALID_PARAMETER;
  }
  Prb = (ROOTB_INDEX_TABLE *)Data;
  Prb->DevEn = AUTO_SET_VALUE;
  for(i=0;i<MAX_DEV_COUNT;i++) {
    if(gDynamicVarPciRootPortTable.PciRootPortTable[i].Sigindex!=0xFF) {
      if(MemCmp(&gDynamicVarPciRootPortTable.PciRootPortTable[i].SigPciRootPortTable,&Prb->SigPciRootPortTable,sizeof(ROOTB_DATA))==0) {
        *index = gDynamicVarPciRootPortTable.PciRootPortTable[i].Sigindex;
        if(gDynamicVarPciRootPortTable.PciRootPortTable[i].VendorId!=Prb->VendorId
        ||gDynamicVarPciRootPortTable.PciRootPortTable[i].DeviceId!=Prb->DeviceId) {  //todo...
          gDynamicVarPciRootPortTable.PciRootPortTable[i].DeviceId=Prb->DeviceId;
          gDynamicVarPciRootPortTable.PciRootPortTable[i].VendorId=Prb->VendorId;
          Prb->DevEn=FORCE_SET_VALUE;
          gDynamicVarPciRootPortTable.isUpdate=1;
          
          //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
          DEBUG((EFI_D_ERROR,"[OemDbg]gDynamicVarPciRootPortTable0index=%d\n",i));
          //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

        }
        //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
        DEBUG((EFI_D_ERROR,"[OemDbg]gDynamicVarPciRootPortTable1index=%d\n",i));
        //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

        Status = EFI_SUCCESS;
        return Status;
      }
    }
  }
  //not find data, add data and set index
  for(i=0;i<MAX_DEV_COUNT;i++) {
    if(gDynamicVarPciRootPortTable.PciRootPortTable[i].Sigindex == 0xFF) {
      MemCpy(&gDynamicVarPciRootPortTable.PciRootPortTable[i].SigPciRootPortTable,&Prb->SigPciRootPortTable,sizeof(ROOTB_DATA));
      gDynamicVarPciRootPortTable.PciRootPortTable[i].DeviceId=Prb->DeviceId;
      gDynamicVarPciRootPortTable.PciRootPortTable[i].VendorId=Prb->VendorId;
      gDynamicVarPciRootPortTable.PciRootPortTable[i].Sigindex = (UINT8)i;
      Prb->DevEn=FORCE_SET_VALUE;
      *index = i;
      gDynamicVarPciRootPortTable.isUpdate=1;
      Status = EFI_SUCCESS;
      //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
      DEBUG((EFI_D_ERROR,"[OemDbg]gDynamicVarPciRootPortTable2index=%d\n",i));
      //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

      return Status;
    }
  }
  return Status;
}


VOID isUpdateRbInVar(PCIeListLinkNode *listEntry,ROOTB_VAR_TABLE *VarPciRootPortTable)
{
  UINT8 DevEn;
  UINTN             i;
  UINTN index;
  EFI_STATUS    Status;
  for(i=0;i<MAX_DEV_COUNT;i++) {
    if(VarPciRootPortTable->PciRootPortTable[i].Sigindex!=0xFF) {
      Status = isPCIeLinkListData(listEntry,&VarPciRootPortTable->PciRootPortTable[i].SigPciRootPortTable,&index,&DevEn);
      if(Status!=EFI_SUCCESS) {
      
        //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
        DEBUG((EFI_D_ERROR,"[OemDbg]isPCIeLinkListData = %ld\n",i));
        //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

        VarPciRootPortTable->PciRootPortTable[i].Sigindex=0xFF;
        VarPciRootPortTable->isUpdate=1;
      }
    }
  }
}

EFI_STATUS GetTableListInVariable(VOID)
{
  DynamicRbIndexInVariable();
  return PCIeLinkedListInit(&gPCIeLinkListEntry);
}

EFI_STATUS AddSingeToRbTableList(ROOTB_INDEX_TABLE *pRb)
{
  UINT8 DevEn;
  UINTN i;
  EFI_STATUS Status;
  if (NULL == pRb) {
      return  EFI_INVALID_PARAMETER;
  }
  Status = isPCIeLinkListData(&gPCIeLinkListEntry,&pRb->SigPciRootPortTable,&i,&DevEn);
  if(Status!=EFI_SUCCESS) {
    
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
    DEBUG((EFI_D_ERROR,"[OemDbg]PCIeLinkedListAddTail...\n"));
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

    Status = GetRootPortIndexInVar(pRb,&i);
    if(Status == EFI_SUCCESS) {
      Status = PCIeLinkedListAddTail(&gPCIeLinkListEntry,&pRb->SigPciRootPortTable,pRb->DevEn,i);
    }
  }
  return Status;
}

EFI_STATUS AddDevInfoInRbtable(VOID *RbDev)
{
  EFI_STATUS            Status;
  PCI_DEV_INFO          *Prt;
  ROOTB_INDEX_TABLE      RbData;
  ROOTB_INDEX_TABLE      *pRb;
   if ( NULL == RbDev ) {
       return  EFI_INVALID_PARAMETER;
    }

  Prt = (PCI_DEV_INFO *)RbDev;
  pRb =  &RbData;
  pRb->VendorId = Prt->DevVenId.VenId;
  pRb->DeviceId = Prt->DevVenId.DevId;
  pRb->SigPciRootPortTable.RbType = 0xFF;
  pRb->SigPciRootPortTable.RbSeg  = Prt->ParentBrg->RbIo->SegmentNumber;
  pRb->SigPciRootPortTable.RbBus  = Prt->ParentBrg->Address.Addr.Bus;
  pRb->SigPciRootPortTable.RbDev  = Prt->ParentBrg->Address.Addr.Device;
  pRb->SigPciRootPortTable.RbFunc = Prt->ParentBrg->Address.Addr.Function; 
  pRb->SigPciRootPortTable.Data2 = ((ACPI_HID_DEVICE_PATH *)Prt->ParentBrg->DevicePath)->UID; 
  pRb->SigPciRootPortTable.Data1 = pRb->SigPciRootPortTable.Data2/4; 
  pRb->SigPciRootPortTable.Data3  = 0xFF;

  Status = AddSingeToRbTableList(pRb);
  return Status;
}
VOID OemDebugHiGonDevices(VOID *Dev)
{
   PCI_DEV_INFO   *Prt;
   Prt = (PCI_DEV_INFO *)Dev;
   DEBUG((EFI_D_ERROR,"[OemDbg]%02d,%02d,%02d,%02d,%02X,%02X\n",Prt->Type,Prt->Address.Addr.Bus,Prt->Address.Addr.Device,Prt->Address.Addr.Function,Prt->DevVenId.VenId,Prt->DevVenId.DevId));
   Prt = Prt->ParentBrg;
   DEBUG((EFI_D_ERROR,"[OemDbg]%02d,%02d,%02d,%02d,%02X,%02X\n",Prt->Type,Prt->Address.Addr.Bus,Prt->Address.Addr.Device,Prt->Address.Addr.Function,Prt->DevVenId.VenId,Prt->DevVenId.DevId));
}

EFI_STATUS IsNotCPUInternalDevices(VOID *Dev)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINTN           i;
  UINTN           Devcount;
  PCI_DEV_INFO   *Prt;
  if ( NULL == Dev ) {
      return  EFI_INVALID_PARAMETER;
  }
  Prt = (PCI_DEV_INFO *)Dev;
  if(Prt->DevVenId.VenId==AMD_VENDOR_ID||Prt->DevVenId.VenId==HIGON_VENDOR_ID) {
      Devcount = sizeof(CPU_INTERNAL_DEVICES_IDTABLE)/sizeof(UINT16);
      for(i=0;i<Devcount;i++) {
        if(Prt->DevVenId.DevId == CPU_INTERNAL_DEVICES_IDTABLE[i]) { //The device ID is the CPU internal device
            if(Prt->ParentBrg->DevVenId.DevId == 0x1454) {           //The bridge ID is the CPU internal bridge
              Status = EFI_UNSUPPORTED;
              break;
            }
          }
      }
  }
  return Status;
}

EFI_STATUS DynamicGenerationRbtable(VOID *Dev)
{
  EFI_STATUS     Status = EFI_SUCCESS;
  PCI_DEV_INFO   *Prt;
  if ( NULL == Dev ) {
      return  EFI_INVALID_PARAMETER;
  }
  Prt = (PCI_DEV_INFO *)Dev;
  while(Prt->ParentBrg != NULL && Prt->Type!=tPciRootBrg) {
      if(Prt->Type==tPciDevice) {
         Status = IsNotCPUInternalDevices(Prt);
         if(Status == EFI_SUCCESS) {
             Status = AddDevInfoInRbtable(Prt);
         }
      }
      Prt=Prt->ParentBrg;
  }
  return Status;
}

EFI_STATUS GetDevRootPortIndexAndDevEnLib(ROOTB_DATA *Rb,UINTN *RPIndex,UINT8 *DevEn)
{
  UINT8      dDevEn;
  UINTN      i;
  EFI_STATUS Status;
  if (NULL == Rb || NULL == RPIndex)
     return  EFI_INVALID_PARAMETER;

  Status = isPCIeLinkListData(&gPCIeLinkListEntry,Rb,&i,&dDevEn);
  if(Status == EFI_SUCCESS) {
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
    DEBUG((EFI_D_ERROR,"[OemDbg]isPCIeLinkListData index=%ld\n",i));
    //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->

    *RPIndex = i;
    *DevEn = dDevEn;
  }
  return  Status;

}

EFI_STATUS GetDeviceRootPortData2(VOID *Dev,ROOTB_DATA *Rb,UINTN *DevIndex,UINT8 *DevEn)
{
  EFI_STATUS          Status;
  ROOTB_NAPLES_DATA   *pRb;
  PCI_DEV_INFO        *Prt,*pDev;
  UINTN               RpIndex;
  UINT8               RpDevEn;

  if ( NULL == Dev || NULL == Rb || NULL == DevIndex)
    return  EFI_INVALID_PARAMETER;
  
  pDev = (PCI_DEV_INFO *)Dev;
  
  if(pDev->Signature != AMI_PCI_SIG)
    return EFI_UNSUPPORTED;
  
  if(pDev->Type <= tPciHostDev && pDev->Type >= tPciRootBrg) 
    return EFI_UNSUPPORTED;
  

  Status = GetDeviceRootBrg(Dev,&Prt);
  if(EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  
  pRb = (ROOTB_NAPLES_DATA *) Rb;
  pRb->RbType = 0xFF;
  pRb->RbSeg  = Prt->RbIo->SegmentNumber;
  pRb->RbBus  = Prt->Address.Addr.Bus;
  pRb->RbDev  = Prt->Address.Addr.Device;
  pRb->RbFunc = Prt->Address.Addr.Function; 
  pRb->DieNum = ((ACPI_HID_DEVICE_PATH *)Prt->DevicePath)->UID; 
  pRb->Socket = pRb->DieNum/4; 
  pRb->Data3  = 0xFF;

  Status = GetDevRootPortIndexAndDevEnLib(Rb,&RpIndex,&RpDevEn);
  
  if(!EFI_ERROR(Status)) {
    *DevIndex = RpIndex;
    *DevEn = RpDevEn;
  }
  return  Status;
  
}

#endif
//<licm-20210531 Optimization PCIE Device List module ->

EFI_STATUS GetDeviceRootBrg(VOID *Dev,VOID **rBrg)
{
  PCI_DEV_INFO          *Prt,*PreDev;
  UINT8                 DieNum;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;

  
  
  if ( NULL == Dev || NULL == rBrg)
    return  EFI_INVALID_PARAMETER;
  
  Prt = (PCI_DEV_INFO *)Dev;
  PreDev = Prt->ParentBrg;
  while(PreDev->ParentBrg != NULL && PreDev->Type!=tPciRootBrg) {
    Prt = PreDev;
    PreDev= Prt->ParentBrg;
  }
  
  //get which die the device on
  DevicePathNode = Prt->DevicePath;  
  if ((DevicePathType (DevicePathNode) == ACPI_DEVICE_PATH) && (DevicePathSubType (DevicePathNode) == ACPI_DP) ) {
    DieNum = ((ACPI_HID_DEVICE_PATH *)DevicePathNode)->UID; 
  }

  *rBrg = Prt;
  
  DEBUG((-1,"RootPort is DieNum:%02x, DevNum:%02x, FuncNum:%02x\n", DieNum, Prt->Address.Addr.Device, Prt->Address.Addr.Function));

  return EFI_SUCCESS;
  
}

EFI_STATUS GetDeviceRootPortData(VOID *Dev,ROOTB_DATA *Rb,UINTN *DevIndex)
{
  EFI_STATUS          Status;
  ROOTB_NAPLES_DATA   *pRb;
  PCI_DEV_INFO        *Prt,*pDev;
  UINTN               RpIndex;

  if ( NULL == Dev || NULL == Rb || NULL == DevIndex)
    return  EFI_INVALID_PARAMETER;
  
  pDev = (PCI_DEV_INFO *)Dev;
  
  if(pDev->Signature != AMI_PCI_SIG)
    return EFI_UNSUPPORTED;
  
  if(pDev->Type <= tPciHostDev && pDev->Type >= tPciRootBrg) 
    return EFI_UNSUPPORTED;
  
  Status = GetDeviceRootBrg(Dev,&Prt);
  if(EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  
  pRb = (ROOTB_NAPLES_DATA *) Rb;
  pRb->RbType = 0xFF;
  pRb->RbSeg  = Prt->RbIo->SegmentNumber;
  pRb->RbBus  = Prt->Address.Addr.Bus;
  pRb->RbDev  = Prt->Address.Addr.Device;
  pRb->RbFunc = Prt->Address.Addr.Function; 
  pRb->DieNum = ((ACPI_HID_DEVICE_PATH *)Prt->DevicePath)->UID; 
  pRb->Socket = pRb->DieNum/4; 
  pRb->Data3  = 0xFF;

  Status = GetDevRootPortIndexLib(Rb,&RpIndex);
  
  if(!EFI_ERROR(Status)) {
    *DevIndex = RpIndex;
  }
  return  Status;
  
}

EFI_STATUS GetDeviceIndexLib(VOID *Dev,UINTN *DevIndex)
{
  EFI_STATUS       Status;
  ROOTB_DATA       RbData;

  if ( NULL == Dev || NULL == DevIndex)
    return  EFI_INVALID_PARAMETER;

  Status = GetDeviceRootPortData(Dev,&RbData,DevIndex);

  return Status;
  
}

EFI_STATUS GetDevicePortIndexLib(
    PCI_DEV_SETUP_FORM_DATA *PciDevListData,
    VOID *Dev,
    UINTN DevIdx,
    UINT8 *PortIndex
    )
{
  EFI_STATUS              Status;
  PCI_DEV_IDX_DATA        *Devitem;
  PCI_DEV_INFO            *Prt;
  UINTN                   i,j;
  
  
  for(i=0;i<PciDevListData->PciDevCount;i++){
    Devitem = PciDevListData->PciDevIdxData[i];
    if(Devitem->PciDevIdx == DevIdx){
      for(j=0;j<Devitem->PortCount;j++){
        if(Devitem->PortData[j].PciDevInfo == Dev){
          *PortIndex = Devitem->PortData[j].PortIdx;
          return EFI_SUCCESS;
        }
      }
      
      //Add device port to exist device list
      if(Devitem->PortCount >= MAX_EXTERN_PORT_COUNTS) 
        return EFI_UNSUPPORTED;
      
      *PortIndex = Devitem->PortData[Devitem->PortCount-1].PortIdx + 1;
      Devitem->PortData[Devitem->PortCount].PortIdx = *PortIndex;
      Devitem->PortData[Devitem->PortCount].PciDevInfo = Dev;
      Devitem->PortData[Devitem->PortCount].PciDevIdx = DevIdx;
      Devitem->PortData[Devitem->PortCount].pPortCount = (VOID *)&Devitem->PortCount;
      Devitem->PortData[Devitem->PortCount].PciRBInfo = Devitem->PortData[0].PciRBInfo;
      Devitem->PortCount++;
      
      return EFI_SUCCESS;
    }
  }

  Devitem = MallocZ(sizeof(PCI_DEV_IDX_DATA));
  ASSERT_EFI_ERROR (Devitem != NULL);
  
  Status = GetDeviceRootBrg(Dev,&Prt);
  if(EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  *PortIndex = 0;
  Devitem->PortCount = 1;
  Devitem->PciDevIdx = DevIdx;
  Devitem->PortData[0].PciDevIdx = DevIdx;
  Devitem->PortData[0].PortIdx = 0;
  Devitem->PortData[0].PciDevInfo = Dev;
  Devitem->PortData[0].pPortCount = (VOID *)&Devitem->PortCount;
  Devitem->PortData[0].PciRBInfo = Prt;
  Status = AppendItemLst((T_ITEM_LIST*)&PciDevListData->PciDevInitCnt, Devitem);
  
  return Status;
}

EFI_STATUS GetDevicePortVarIndexLib(
		PCI_DEV_SETUP_FORM_DATA *PciDevListData,
		VOID *Dev,
		UINTN DevIdx,
		UINT8 *PortIndex,
		UINTN *PortVarIndex
		)
{
  EFI_STATUS          Status;
  UINTN               SetupDataIndex = 0,tIndex=0;
  UINT8               PortIdx;
  
  
  if ( NULL == PciDevListData || NULL == Dev || NULL == PortIndex || NULL == PortVarIndex)
    return  EFI_INVALID_PARAMETER;
  
  Status = GetDevicePortIndexLib(PciDevListData,Dev,DevIdx,&PortIdx);
  if(EFI_ERROR(Status)) {
    return Status;
  }
  
  SetupDataIndex = DevIdx * MAX_DEV_PORT + PortIdx;
  *PortIndex = PortIdx;
  *PortVarIndex = SetupDataIndex;
  
  if(SetupDataIndex+1>DYNAMIC_PCIDEV_ENTRY_NUMBER)
    return EFI_INVALID_PARAMETER;

  return Status;
}

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
/**
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param[in] pPciIo               EFI_PCI_IO_PROTOCOL
  @param[in] CapId                CAPID to search for

  @retval 0                       CAPID not found
  @retval Other                   CAPID found, Offset of desired CAPID
**/
UINT8
PcieFindCapId (
  IN EFI_PCI_IO_PROTOCOL        *pPciIo,
  IN UINT8                      CapId
  )
{
  UINT8 CapHeaderOffset;
  UINT8 CapHeaderId;
  UINT8 Data8;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  

  PciIo = pPciIo;
  //DEBUG((DEBUG_INFO,"PcieFindCapId () BDF %0x: %0x :%0x, CapId = %0x \n", Bus, Device, Function, CapId));
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x06,1,&Data8);
  if ((Data8 & 0x10) == 0x00) {
    ///
    /// Function has no capability pointer
    ///
    return 0;
  } else {
    ///
    /// Check the header layout to determine the Offset of Capabilities Pointer Register
    ///
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x0E,1,&Data8);
    if ((Data8 & 0x7F) == 0x02) {
      ///
      /// If CardBus bridge, start at Offset 0x14
      ///
      CapHeaderOffset = 0x14;
    } else {
      ///
      /// Otherwise, start at Offset 0x34
      ///
      CapHeaderOffset = 0x34;
    }
    ///
    /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
    ///
    CapHeaderId     = 0;
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset,1,&Data8);
    CapHeaderOffset = Data8 & ((UINT8) ~(BIT0 | BIT1));
    while (CapHeaderOffset != 0 && CapHeaderId != 0xFF) {
      PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset,1,&CapHeaderId);
      if (CapHeaderId == CapId) {
        if (CapHeaderOffset > 0x3F) {
          ///
          /// Return valid capability offset
          ///
          return CapHeaderOffset;
        } else {
          ASSERT((FALSE));
          return 0;
        }
      }
      ///
      /// Each capability must be DWORD aligned.
      /// The bottom two bits of all pointers (including the initial pointer at 34h) are reserved
      /// and must be implemented as 00b although software must mask them to allow for future uses of these bits.
      ///
      PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset+1,1,&Data8);
      CapHeaderOffset = Data8 & ((UINT8) ~(BIT0 | BIT1));
    }
    return 0;
  }
}

//<Lizhq-20190515 Get MAC address for Broadcom network device +>
/**
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Reporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability

  @param[in] PciIo                EFI_PCI_IO_PROTOCOL
  @param[in] CapId                Extended CAPID to search for

  @retval 0                       CAPID not found
  @retval Other                   CAPID found, Offset of desired CAPID
**/
UINT16
PcieFindExtendedCapId (
  IN EFI_PCI_IO_PROTOCOL        *PciIo,
  IN UINT16                      CapId
  )
{
  UINT16  CapHeaderOffset;
  UINT16  CapHeaderId;

  ///
  /// Start to search at Offset 0x100
  /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
  ///
  CapHeaderId     = 0;
  CapHeaderOffset = 0x100;
  while (CapHeaderOffset != 0 && CapHeaderId != 0xFFFF) {
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,CapHeaderOffset,1,&CapHeaderId);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }
    ///
    /// Each capability must be DWORD aligned.
    /// The bottom two bits of all pointers are reserved and must be implemented as 00b
    /// although software must mask them to allow for future uses of these bits.
    ///
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,CapHeaderOffset+2,1,&CapHeaderOffset);
    CapHeaderOffset = (CapHeaderOffset >> 4) & ((UINT16) ~(BIT0 | BIT1));
  }

  return 0;
}
//<Lizhq-20190515 Get MAC address for Broadcom network device->
//<xuran003-20180724-Use EFI device path to get NIC MAC+>
EFI_STATUS
GetNicMacByUefi(
  IN  EFI_HANDLE          PciDevHandle,
  IN OUT  UINT8          *NicMac
)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINTN                           HandleCount, Index, Index1;
  EFI_HANDLE                      *HandleBuffer = NULL, NewHandle;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL, *TmpDp = NULL, *Dp = NULL, *DpPciNode = NULL;
  MAC_ADDR_DEVICE_PATH            *MacDp = NULL;

  //
  // Find all MAC device path handle
  //
  Status = gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiDevicePathProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {

    //Open the DevicePath
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    if (EFI_ERROR(Status)) continue;

    //MAC_ADDR_DEVICE_PATH filter
    for(Dp=DevicePath; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp)){
      if (Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_MAC_ADDR_DP){
        break;
      }
    }
    if(isEndNode(Dp)) continue;
    MacDp = (MAC_ADDR_DEVICE_PATH *)Dp;

    //Create the parent PCI DevicePath
    for(Dp=DevicePath; !(isEndNode(Dp)); Dp=NEXT_NODE(Dp)){
      if (Dp->Type == HARDWARE_DEVICE_PATH && Dp->SubType == HW_PCI_DP){
        DpPciNode = Dp;
      }
    }
    if(DpPciNode==NULL) continue;
    TmpDp = DuplicateDevicePath(DevicePath);
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINTN)TmpDp + ((UINTN)NEXT_NODE(DpPciNode) - (UINTN)DevicePath));
    Dp->Type = END_DEVICE_PATH;
    Dp->SubType = END_ENTIRE_SUBTYPE;
    SET_NODE_LENGTH(Dp,sizeof(*Dp));
    Dp = TmpDp;

    Status = gBS->LocateDevicePath(
              &gEfiPciIoProtocolGuid,
              &Dp,
              &NewHandle);
    if(TmpDp){
      FreePool(TmpDp);
      TmpDp = NULL;
    }
    if (EFI_ERROR(Status)) continue;

    //Found the parent PCI device handle
    if(NewHandle == PciDevHandle){

      for(Index1 = 0; Index1 < 6; Index1++){
        NicMac[Index1] = MacDp->MacAddress.Addr[Index1];
      }
      DEBUG((EFI_D_ERROR, " ***UEFI MAC is %x %x %x %x %x %x *** \n", NicMac[0], NicMac[1],NicMac[2], NicMac[3], NicMac[4], NicMac[5]));
      break;
    }
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if(Index == HandleCount){
    return EFI_NOT_FOUND;
  }
  return Status;
}
//<xuran003-20180724-Use EFI device path to get NIC MAC->

//<xuran-20171130 +> BMC Web can't show Intel OPA Card GUID
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   GetNicGuid
//
// Description: Get NIC Guid
//
// Input:
//
// Output:
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetNicGuid(
    EFI_PCI_IO_PROTOCOL             *PciIo,
    UINT8                           *NicGuid
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINT64                          Data64, Bak;
    UINT16                          LVendorID, LDeviceID;
    UINT8                           Data8, j;
    UINT8                           BaseClassCode;
    UINT8                           GuidValid =0;

    DEBUG((EFI_D_ERROR, "[%a]()...\n", __FUNCTION__));
    if(PciIo == NULL || NicGuid == NULL)
      return EFI_INVALID_PARAMETER;

    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x0B,1,&BaseClassCode);
    if(BaseClassCode != 2)
      return EFI_UNSUPPORTED;

    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x00,1,&LVendorID);
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x02,1,&LDeviceID);
    DEBUG((EFI_D_ERROR, "  LVendorID/LDeviceID    is  0x%x / 0x%x\n", LVendorID, LDeviceID));

    //<xuran-20171214 +> Get NIC GUID by VendorId and DeviceId
    ZeroMem(NicGuid, sizeof(NicGuid));
    if (LVendorID  == 0x8086){ //Intel network
      switch (LDeviceID) {
        case 0x24F0: //Intel Omni-Path HFI Silicon 100 Series
        case 0x24F1:
          //Enable Bus Master
          PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x04,1,&Data8);
          Data8 |= 0x02;
          PciIo->Pci.Write(PciIo,EfiPciIoWidthUint8,0x04,1,&Data8);

          PciIo->Mem.Read(PciIo,EfiPciIoWidthUint64,0,CCE_DC_CTRL,1,&Bak);
          DEBUG((EFI_D_ERROR, "  CCE_DC_CTRL is  0x%llx .\n", Bak));

          //Take the DC out of reset to get a valid GUID value
          Data64 = 0;
          PciIo->Mem.Write(PciIo,EfiPciIoWidthUint64,0,CCE_DC_CTRL,1,&Data64);
          PciIo->Mem.Read(PciIo,EfiPciIoWidthUint64,0,CCE_DC_CTRL,1,&Data64);
          DEBUG((EFI_D_ERROR, "  CCE_DC_CTRL is  0x%llx .\n", Data64));

          //Read GUID
          PciIo->Mem.Read(PciIo,EfiPciIoWidthUint64,0,DC_DC8051_CFG_LOCAL_GUID,1,NicGuid);
          DEBUG((EFI_D_ERROR, "  The NicGuid is  %llx .\n", *((UINT64 *)NicGuid)));

          //restore the DC status
          PciIo->Mem.Write(PciIo,EfiPciIoWidthUint64,0,CCE_DC_CTRL,1,&Bak);
          PciIo->Mem.Read(PciIo,EfiPciIoWidthUint64,0,CCE_DC_CTRL,1,&Bak);
          DEBUG((EFI_D_ERROR, "  CCE_DC_CTRL is  0x%llx .\n", Bak));
          break;
        default:
          Status = EFI_UNSUPPORTED;
      }
    }else{
      return EFI_UNSUPPORTED;
    }
    //<xuran-20171214 ->

    //if device no guid report 0xff
    GuidValid = 0;
    for (j = 0; j < 8; j++) {
        if(NicGuid[j] != 0){
          GuidValid = 1;
            break;
        }
    }

    if(GuidValid == 0){
      SetMem(NicGuid, 8, 0xFF);
    }

    return Status;
}
//<xuran-20171130 -> BMC Web can't show Intel OPA Card GUID




//<Xuran001-20181031-Get MLX NIC MAC by VSC.+>
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   GetMlxNicMac
//
// Description: Get Mellanox Technology network  MAC by VSC (Vendor Specific Capability)
//
// Input: 
//
// Output: 
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
GetMlxNicMac(
  IN   EFI_PCI_IO_PROTOCOL             *PciIo,
  OUT  UINT8                           *NicMac
)
{
  UINT8                           CapIdOffset, RetryTimes1, RetryTimes2,LClassCode, LSubClassCode;
  UINT16                          LVendorID;
  UINT32                          Counter, Semaphore;
  BOOLEAN                         OwnershipFlag = FALSE;
  UINT32                          Data32;
  UINT64                          MacAddr, MacAddrH, MacAddrL;
  UINT8                           *Ptr;
    
  if(PciIo == NULL || NicMac == NULL)
    return EFI_INVALID_PARAMETER;
  
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x00,1,&LVendorID);
  if(LVendorID != 0x15B3)
    return EFI_UNSUPPORTED;

  //
  //Only support MLX Ethernet, not support IB.
  //
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,PCI_BCC,1,&LClassCode);
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,PCI_SCC,1,&LSubClassCode);
  if(!((LClassCode == PCI_CLASS_NETWORK) && (LSubClassCode == PCI_CLASS_NETWORK_ETHERNET)))
    return EFI_UNSUPPORTED;
        
  CapIdOffset = PcieFindCapId(PciIo, EFI_PCI_CAPABILITY_ID_VENDOR);
  DEBUG((EFI_D_ERROR, "    Mellanox VSC CapIdOffset   is  0x%x\n", CapIdOffset));
  if(CapIdOffset == 0)
    return EFI_UNSUPPORTED;
  
  //
  //Get Gateway Ownership
  //
  RetryTimes1 = 10;
  do{
    RetryTimes2 = 10;
    do{
      PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SEMAPHORE_OFFSET, 1 , &Semaphore);
      if(Semaphore == 0)
        break;
      gBS->Stall(10000);//10ms
      RetryTimes2--;
      
    }while(RetryTimes2);
    if(RetryTimes2 == 0)
      return EFI_TIMEOUT;
    
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_COUNTER_OFFSET, 1 , &Counter);
    PciIo->Pci.Write(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SEMAPHORE_OFFSET, 1 , &Counter);
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SEMAPHORE_OFFSET, 1 , &Semaphore);
    if(Semaphore == Counter){
      OwnershipFlag = TRUE;
    }
    if(RetryTimes1-- == 0)
      return EFI_TIMEOUT;
  }while(OwnershipFlag == FALSE);
  
  //
  //Get the MAC
  //
  Data32 = MLX_VSC_SPACE_MAC_ID;
  PciIo->Pci.Write(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SPACE_OFFSET, 1 , &Data32);
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SPACE_OFFSET, 1 , &Data32);
  if((Data32 & BIT29) != BIT29){
    DEBUG((EFI_D_ERROR, "    The Mellanox NIC doesn't support VSC MAC space, please update the NIC FW.\n"));
    return EFI_DEVICE_ERROR;
  }
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_MAC_H_OFFSET, 1 , &MacAddrH);
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_MAC_L_OFFSET, 1 , &MacAddrL);
  MacAddr = (MacAddrH << 32) | MacAddrL;
  DEBUG((EFI_D_ERROR, "     Mellanox MAC is %llX \n", MacAddr));
  
  //
  //Release the Gateway ownership
  //
  Data32 = 0;
  PciIo->Pci.Write(PciIo,EfiPciIoWidthUint32, CapIdOffset + MLX_VSC_SEMAPHORE_OFFSET, 1 , &Data32);
  OwnershipFlag = FALSE;
  
  MacAddr = SwapBytes64(MacAddr);
  Ptr = (UINT8 *)&MacAddr;
  CopyMem(NicMac, Ptr + 2, 6);
  
  return EFI_SUCCESS;
}
//<Xuran001-20181031-Get MLX NIC MAC by VSC.->

//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   GetNicMac
//
// Description: Get network Mac Info
//
// Input: 
//
// Output: 
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetNicMac(
    EFI_PCI_IO_PROTOCOL   					*PciIo,
    UINT8														*NicMac
)
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  UINT64                          MMIOAddress;
  UINT32                          Data32;
  UINT16                          Data16;
  UINT16                          LVendorID, LDeviceID;
  UINT16                          IOAddress;
  UINT64                          RAL_Address;    //<tanzhk-20170307-001 change the type to UINT64.>
  UINT16                          i,j,k;//<Yinnn-Get Beijing Wangxun Technology NIC MAC.>
  UINT8                           MacValid =0;
  UINT8                           Data8;
  UINT8                           BaseClassCode;
  UINT8                           SubClassCode; //<Songts032_20190319-Fill all 0xFF to MLX IB MAC under legacy mode.+>


  if(PciIo == NULL || NicMac == NULL)
    return EFI_INVALID_PARAMETER;

  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x0B,1,&BaseClassCode);
  if(BaseClassCode != 2)
    return EFI_UNSUPPORTED;

  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x00,1,&LVendorID);
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x02,1,&LDeviceID);
  DEBUG((EFI_D_ERROR, " *** LVendorID/LDeviceID    is  0x%x / 0x%x   ***\n", LVendorID, LDeviceID));

  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,0x10,1,&Data32);
  MMIOAddress = Data32;
  if((MMIOAddress & 0x7) == 4)  {//64-bit Bar //<lvych00320160329>
  	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,0x14,1,&Data32);
      MMIOAddress |= ((UINT64)Data32<<32);
  }
  
  if ((MMIOAddress & 01) == 0) {
      MMIOAddress = MMIOAddress & (UINT64)-16;
      
      if (LVendorID  == 0x8086){ //Intel network 
          switch (LDeviceID) {
              //X540
              case 0x1528: //1/2 Ports, RJ45, 100/1000/10GbE,PCIe, X540; Intel Converged Network Adapter X540-T1 /T2
                  RAL_Address = RAL_X540;
                  break;
              //82599
              case 0x10F8:
              case 0x10FB: //1/2 Ports, SFP+ SR Optics, LC, 1000/10GbE, PCIe, 82599; Intel Ethernet Server Adapter X520-LR1/DA2/SR1/SR2 
              case 0x151C: //2 Ports, RJ45, 1000/10GbE,PCIe, 82599; Intel Ethernet Server Adapter X520-T2 
              //92598GB
              case 0x10F1: //SFP+, 10GbE, PCIe, 82598GB; Intel 10 Gigabit AF DA Dual Port Server Adapter 
              case 0x10C8: //RJ45, 1000/10GbE, PCIe, 82598GB; Intel 10 Gigabit AT Server Adapter
              case 0x150B: //RJ45, 1000/10GbE, PCIe, 82598GB; Intel 10 Gigabit AT2 Server Adapter
              //82598EB
              case 0x10EC: //CX4, 10GbE, PCIe, 82598EB; Intel 10 Gigabit CX4 Dual Port Server Adapter
              case 0x10F4: //LC, 10GbE, PCIe, 82598EB; Intel 10 Gigabit XF LR Server Adapter 
              case 0x10C6: //LC, 10GbE, PCIe, 82598EB; Intel 10 Gigabit XF SR Dual Port Server Adapter
              case 0x10C7: //LC, 10GbE, PCIe, 82598EB; Intel 10 Gigabit XF SR Server Adapter
              //82597EX
              case 0x1048: //SC, 10GbE, PCI-X, 82597EX; Intel PRO/10GbE LR Server Adapter
              case 0x109E: //CX4, 10GbE, PCI-X, 82597EX; Intel PRO/10GbE CX4 Server Adapter
              case 0x1A48: //LC, 10GbE, PCI-X, 82597EX ; Intel PRO/10GbE SR Server Adapter
              case 0x1B48: //SC, 10GbE, PCI-X, 82597EX; Intel PRO/10GbE LR Server Adapter
                  RAL_Address = RAL_82599;
                  break;
              //<ChengXW001-20170821 BMC Web show Intel X710 NIC info +>
              //X710
              case 0x1572:
//<huangjin001-20200227 Mantis 0037102 Fix Intel XL710 40G MAC is incorrect in legacy mode +>                  
              case 0x1583://2 Port
              case 0x1584://1 Port    
//<huangjin001-20200227 Mantis 0037102 Fix Intel XL710 40G MAC is incorrect in legacy mode ->                  
                  RAL_Address = RAL_X710beLow;
                  break;
              //<ChengXW001-20170821 BMC Web show Intel X710 NIC info ->
               //<tanzhk-20170307-001 BMC Web can't show NIC info.+>
              case 0x37cc: //<Sunhp001-20170906 add no phy ID pch gbe>
              case 0x37d0:
              case 0x37d1:
              case 0x37d2:
              case 0x37d3:
              	  RAL_Address = RAL_PCH10GbeLow;
  
                  break;
				//<tanzhk-20170307-001 BMC Web can't show NIC info.->	  
              //Gigabit Server Adapters
              default:
                  RAL_Address = RAL_I350;     //Other 1Gbps Intel LAN :RAL=0x5400
                  break;
          } //switch (LDeviceID)  
          
          //Enable Bus Master
          PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x04,1,&Data8);
          Data8 |= 0x02;
          PciIo->Pci.Write(PciIo,EfiPciIoWidthUint8,0x04,1,&Data8);

          DEBUG((EFI_D_ERROR, " *** MMIO Address is  0x%x ***\n", MMIOAddress));
          DEBUG((EFI_D_ERROR, " *** RAL Address is   0x%x ***\n", RAL_Address)); 
//<tanzhk-20170307-001 BMC Web can't show NIC info.+>
          if (RAL_Address == RAL_PCH10GbeLow) {

            for (j = 0; j < 6; j++) {
              if(j<4){
                NicMac[j] = MmioRead8(MMIOAddress + RAL_PCH10GbeLow + j );
              } else {

                NicMac[j] = MmioRead8(MMIOAddress + RAL_PCH10GbeHigh + j - 4 );
              }
            }
          } 
          //<tanzhk-20170307-001 BMC Web can't show NIC info.->
          //<ChengXW001-20170821 BMC Web show Intel X710 NIC info +>
          else if (RAL_Address == RAL_X710beLow) {
            for (j = 0; j < 6; j++) {
              if(j<4){
                NicMac[j] = MmioRead8(MMIOAddress + RAL_X710beLow + j );
              } else {
                NicMac[j] = MmioRead8(MMIOAddress + RAL_X710beHigh + j - 4 );
              }
            }            
          } else {      
            for (j = 0; j < 6; j++) {
              NicMac[j] = MmioRead8(MMIOAddress + RAL_Address + j );
            }            
          }
          //<ChengXW001-20170821 BMC Web show Intel X710 NIC info -> 
      } //if (LVendorID  == 0x8086)
      
      if (LVendorID  == 0x15B3){ //Mellanox Technology network 
        //<Xuran001-20181031-Get MLX NIC MAC by VSC.+>
        Status = GetMlxNicMac(PciIo, NicMac);
        if(EFI_ERROR(Status)){//<Songts032_20190319-Fill all 0xFF to MLX IB MAC under legacy mode.+>
          PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,PCI_SCC,1,&SubClassCode);
          if(SubClassCode == PCI_CLASS_NETWORK_IB){
            SetMem(NicMac, 6, 0xFF);
          }else {               
          //<Songts032_20190319-Fill all 0xFF to MLX IB MAC under legacy mode.->    
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x153,1,&NicMac[0]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x152,1,&NicMac[1]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x151,1,&NicMac[2]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x14E,1,&NicMac[3]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x14D,1,&NicMac[4]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x14C,1,&NicMac[5]);
          }//<Songts032_20190319-Fill all 0xFF to MLX IB MAC under legacy mode.>   
        }
        //<Xuran001-20181031-Get MLX NIC MAC by VSC.->
      }
      
      //<Yinnn-Get Beijing Wangxun Technology NIC MAC.+>
            if (LVendorID  == 0x8088){ //Beijing Wangxun Technology Co., Ltd.
                
                MmioWrite8((MMIOAddress + 0x16210), 0 );         
                for (i = 0; i < 6; i++) {
                    k = 5 - i;
                    if(i < 4){
                        NicMac[k] = MmioRead8(MMIOAddress + 0x16200 + i );
                    } else {

                        NicMac[k] = MmioRead8(MMIOAddress + 0x16204 + i - 4 );
                    } //if()                                   
               }//for() 
             }
       //<Yinnn-Get Beijing Wangxun Technology NIC MAC.->
            
      if ((LDeviceID == 0x0710)&&(LVendorID  == 0x19A2)){ //Emulex BladeEngine3 network 
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x173,1,&NicMac[0]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x172,1,&NicMac[1]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x171,1,&NicMac[2]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x16E,1,&NicMac[3]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x16D,1,&NicMac[4]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x16C,1,&NicMac[5]);
      	PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x174,1,&Data8);
          NicMac[5] += Data8;
      }

      //<Lizhq-20190515 Get MAC address for Broadcom network device+>
      if (LVendorID  == 0x14E4){ //Broadcom 
        /*  
         *  Dump from Linux PCIe configuration:
         *  Capabilities: [13c v1] Device Serial Number 00-0a-f7-ff-fe-a6-9a-50
         *                Port#0 MAC: 00:0a:f7:a6:9a:50 
                                       S/N: 00-0a-f7-ff-fe-a6-9a-50.
         *                MAC +1 for Port#1/2/3 
         */
              
         {
             UINT16    CapIdOffset;
             UINT32    Data32_1, Data32_2;
             UINTN     Segment=0, Bus=0, Device=0,Function=0;
                    
             CapIdOffset = PcieFindExtendedCapId(PciIo, EFI_PCIE_EX_CAPABILITY_ID_SN);
             DEBUG((EFI_D_ERROR, " *** ExCapIdOffset    is  0x%x\n", CapIdOffset));
             if(CapIdOffset == 0)
                 return EFI_UNSUPPORTED;
                    
             PciIo->GetLocation(PciIo, &Segment, &Bus, &Device, &Function);
             PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + 4, 1 , &Data32_1);
             PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32, CapIdOffset + 8, 1 , &Data32_2);
                    
             DEBUG((EFI_D_ERROR, " *** Seg:0x%x B:0x%x D:0x%x F:0x%x SN data is  0x%04x - 0x%04x   ***\n",\
                                       Segment, Bus, Device, Function, Data32_1, Data32_2));
                    
             NicMac[0] = (UINT8)((Data32_2&0xFF000000) >> 24);
             NicMac[1] = (UINT8)((Data32_2&0x00FF0000) >> 16);
             NicMac[2] = (UINT8)((Data32_2&0x0000FF00) >> 8);
             NicMac[3] = (UINT8)((Data32_1&0x00FF0000) >> 16);
             NicMac[4] = (UINT8)((Data32_1&0x0000FF00) >> 8);
             NicMac[5] = (UINT8)(Data32_1&0x000000FF);
                          
           //Songts045_20190519  Add Broadcom FC card Info  +>
           if((LDeviceID == 0x16A1) || (LDeviceID == 0x16A4)){
               NicMac[5] += 2*(UINT8)Function;              
           } else {
           //Songts045_20190519  Add Broadcom FC card Info  ->    
           NicMac[5] += (UINT8)Function; 
           } //Songts045_20190519  Modify Broadcom FC card MAC info on BMC web  +>           
                    
         }
         
         //<Lvshh001-20190522 Get MAC address method for the device(VendorID:0x14E4,DeviceID:0x16D8) is replaced +>
         //<Chendl001-20180711 Support Broadcom Network Card +>
          /*if ((LDeviceID == 0x16D8)&&(LVendorID  == 0x14E4)){ 
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x147,1,&NicMac[0]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x146,1,&NicMac[1]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x145,1,&NicMac[2]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x142,1,&NicMac[3]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x141,1,&NicMac[4]);
            PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x140,1,&NicMac[5]);
            device=(PCI_DEV_INFO*)PciIo;
            NicMac[5] += device->Address.Addr.Function;
          }*/
          //<Chendl001-20180711 Support Broadcom Network Card ->
         //<Lvshh001-20190522 Get MAC address method for the device(VendorID:0x14E4,DeviceID:0x16D8) is replaced ->
         
      }
      //<Lizhq-20190515 Get MAC address for Broadcom network device->
      
  } else { //if ((MMIOAddress & 01) == 0)
  		PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x10,1,&Data16);
      IOAddress =  Data16 & 0xFFFE; 
      for (j = 0; j < 6; j++) {
        
          NicMac[j] = IoRead8(IOAddress + j);
      }
  } //if ((MMIOAddress & 01) == 0)  
  DEBUG((EFI_D_ERROR, " *** MAC is %x %x %x %x %x %x *** \n", NicMac[0], NicMac[1],NicMac[2], NicMac[3], NicMac[4], NicMac[5]));

  //if device no mac report 0xff
  MacValid = 0;
  for (j = 0; j < 6; j++) {
      if(NicMac[j] != 0){
          MacValid = 1;
          break;
      }
  }
  
  if(MacValid == 0){
    SetMem(NicMac, 6, 0xFF); //<Lizhq036-20170105 Compile error if DEBUG_MODE is disabled>
  }
	
  return Status;

}


//**********************************************************************
// Procedure:   GetPcieDevInfo
//
// Description: Get PCIE device Info
//
// Input: 
//
// Output: 
//
//**********************************************************************
EFI_STATUS GetPcieDevInfo(
        PCI_DEV_SETUP_FORM_DATA *PciDevListData,
        UINTN DevIndex,
        UINT8 PortIndex,
        PCIE_DEV_INFO *PcieDevInfo
)
{
    PCI_DEV_IDX_DATA                *IdxDev;
    PCI_DEV_SETUP_DATA              *Devitem = NULL;
    PCI_DEV_INFO                    *DevPtr,*RbPtr;
    PCIE_LNK_STS                    LinkStatus;
    EFI_PCI_IO_PROTOCOL             *PciIo;
    UINT16                          SubSystemId,SubVenderId;
    UINT8                           CapIdOffset;
    EFI_STATUS                      Status;
    PCIE_NIC_INFO                   *NicInfo;
    UINTN                           Index;
    
    DEBUG((EFI_D_ERROR, " ***GetPcieDevInfo Entry***\n"));
    
    if ( NULL == PciDevListData || NULL == PcieDevInfo )
        return  EFI_INVALID_PARAMETER;
    
    for(Index=0;Index<PciDevListData->PciDevCount;Index++){
        IdxDev = PciDevListData->PciDevIdxData[Index];
        if(IdxDev->PciDevIdx == DevIndex){
            Devitem = &IdxDev->PortData[PortIndex];
            break;
        } 
    }    
    
    if (Devitem == NULL)
        return  EFI_INVALID_PARAMETER;
    
    PcieDevInfo->CpuNum = Devitem->CpuNum;
    PcieDevInfo->PortIndex = Devitem->PortIdx;
    //rootport
    RbPtr = (PCI_DEV_INFO *)(Devitem->PciRBInfo);
    PcieDevInfo->RbBus = RbPtr->Address.Addr.Bus;
    PcieDevInfo->RbDev = RbPtr->Address.Addr.Device;
    PcieDevInfo->RbFunc = RbPtr->Address.Addr.Function;
    
//Get PCIE root port Link Status
    LinkStatus.Word = 0; 
    CapIdOffset = PcieFindCapId(&RbPtr->PciIo,0x10);
    if(CapIdOffset !=0){
        RbPtr->RbIo->Pci.Read (RbPtr->RbIo,
                    EfiPciIoWidthUint16,
                    (RbPtr->Address.ADDR + CapIdOffset + PCIE_LINK_STATUS_OFFSET),
                    1,
                    &LinkStatus.Word
                    );
    }
          
    PcieDevInfo->LinkWidth = (UINT8)LinkStatus.Bits.NegotiatedLinkWidth;
    PcieDevInfo->LinkSpeed = (UINT8)LinkStatus.Bits.LinkSpeed;
          
//Get PCIE device infomation
    DevPtr = (PCI_DEV_INFO *)Devitem->PciDevInfo;
    PciIo = &DevPtr->PciIo;
    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          0x2c,
                          2,
                          &SubVenderId
                          );
    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          0x2e,
                          2,
                          &SubSystemId
                          );
   ASSERT_EFI_ERROR (Status); 
   PcieDevInfo->PcieDevicePresent = 1;
   PcieDevInfo->BusNum = DevPtr->Address.Addr.Bus;
   PcieDevInfo->DevNum = DevPtr->Address.Addr.Device;
   PcieDevInfo->FuncNum = DevPtr->Address.Addr.Function;
   PcieDevInfo->VenderId = DevPtr->DevVenId.VenId;
   PcieDevInfo->DeviceId = DevPtr->DevVenId.DevId;
   PcieDevInfo->BaseClassCode = DevPtr->Class.BaseClassCode;
   PcieDevInfo->SubClassCode  = DevPtr->Class.SubClassCode;
   PcieDevInfo->SubVenderId = SubVenderId;
   PcieDevInfo->SubSystemId = SubSystemId;
   PcieDevInfo->BoardType = Devitem->BoardType;
   
   //<Nieht-20181119- Additional port's BrandName is obtained form port 0 for X710+>
   if((DevPtr->DevVenId.VenId == 0x8086) && (DevPtr->DevVenId.DevId == 0x1572))
       UnicodeStrToAsciiStr(IdxDev->PortData[0].BrandName,PcieDevInfo->BrandName);
   else
       UnicodeStrToAsciiStr(Devitem->BrandName,PcieDevInfo->BrandName);
   //<Nieht-20181119- Additional port's BrandName is obtained form port 0 for X710->
   DEBUG((EFI_D_ERROR, " *** Num %2d Slot is 0x%02x          ***\n",DevIndex ,Devitem->SlotNum));
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie Rb  Bus/Dev/Fun is 0x%02x 0x%02x 0x%02x ***\n", DevIndex, PcieDevInfo->RbBus,PcieDevInfo->RbDev,PcieDevInfo->RbFunc));
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie Dev Bus/Dev/Fun is 0x%02x 0x%02x 0x%02x ***\n", DevIndex, PcieDevInfo->BusNum,PcieDevInfo->DevNum,PcieDevInfo->FuncNum));          
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie VendorID  is  0x%04x              ***\n", DevIndex, PcieDevInfo->VenderId));
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie DeviceID  is  0x%04x              ***\n", DevIndex, PcieDevInfo->DeviceId));
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie BaseClass is  0x%02x                ***\n", DevIndex, PcieDevInfo->BaseClassCode));
   DEBUG((EFI_D_ERROR, " *** Num %2d Pcie SubClass  is  0x%02x                ***\n", DevIndex, PcieDevInfo->SubClassCode));
   
   
   if (PcieDevInfo->BaseClassCode == PCI_CL_NETWORK){
       //<xuran-20171214 +> Get NIC GUID by VendorId and DeviceId
       Status = pBS->AllocatePool (
                     EfiBootServicesData,
                     sizeof(PCIE_NIC_INFO),
                     &NicInfo );       
       ZeroMem(NicInfo, sizeof(PCIE_NIC_INFO));
       
       NicInfo->Speed = Devitem->Speed;
       Status = GetNicGuid(PciIo,(UINT8*)(NicInfo->NicGuid));
       DEBUG((EFI_D_ERROR, " *** Get Nic GUID %r  ***\n",Status));
       if(!EFI_ERROR(Status)){
         NicInfo->NicMacGuidSelect = 2;
       //<xuran-20171214 ->
       }else{
         NicInfo->NicMacGuidSelect = 1;
         //<xuran003-20180724-Use EFI device path to get NIC MAC>
         Status = GetNicMacByUefi(DevPtr->Handle, (UINT8*)(NicInfo->NicMac));
         DEBUG((EFI_D_ERROR, " *** Uefi Get MAC Status %r  ***\n",Status));
         if(EFI_ERROR(Status)){
           Status = GetNicMac(PciIo,(UINT8*)(NicInfo->NicMac));
           DEBUG((EFI_D_ERROR, " *** Get MAC Status %r  ***\n",Status));
         }
       }
       PcieDevInfo->DevInfo = (UINT8 *)NicInfo;
   }
   return EFI_SUCCESS;
}
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

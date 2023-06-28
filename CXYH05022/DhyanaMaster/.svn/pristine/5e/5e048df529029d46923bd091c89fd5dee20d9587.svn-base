/** @file
  This file contains OemPCIeDevPlatLib Use to Support OemPCIeDev Module
  
  Author:lvych
  Date:  20170803
  

  
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
#include <Library/UefiBootServicesTableLib.h>
#include <SataControllerInfoLib.h>
#include <Library/DevicePathLib.h>


//<Kangmm-20190307 Update setup pci device list table on Rome +>
#if NSOCKETS == 1
ROOTB_DATA PciRootPortTableSsp[MAX_DEV_COUNT] = {

  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //RbNum   //Data3
  //PCH 
  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x02,     0xFF}, //(S0 NBIO1 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x02,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x01,     0xFF}, //(S0 NBIO2 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x01,     0xFF},
  
};
#else
ROOTB_DATA PciRootPortTableSsp[MAX_DEV_COUNT] = {
	
  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //RbNum   //Data3
  //PCH
  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x02,     0xFF}, //(S0 NBIO1 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x02,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x01,     0xFF}, //(S0 NBIO2 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x01,     0xFF},

  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x06,     0xFF}, //(S1 NBIO1 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x06,     0xFF},
  
  {0x00,     0x00,     0xFF,     0x08,     0x02,     0xFF,     0x05,     0xFF}, //(S1 NBIO2 Sata Controller Root Bridge)
  {0x00,     0x00,     0xFF,     0x08,     0x03,     0xFF,     0x05,     0xFF}, 


};
#endif
#if NSOCKETS == 1
ROOTB_DATA PciRootPortTableZp[MAX_DEV_COUNT] = {

  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //Die   //Data3
  //PCH
  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x00,     0xFF}, //(S0D0P0,typeA)
  
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x01,     0xFF}, //ASM1061R //<hujf1001-20210316 Fix SATA controller index conflict +> 

  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x01,     0xFF}, //(S0D1P1,typeA)
  
  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x02,     0xFF}, //(S0D2G2,typeB)
 
  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x03,     0xFF}, //(S0D3G3,typeB)

};
#else
ROOTB_DATA PciRootPortTableZp[MAX_DEV_COUNT] = {
	
  //RbType   //RbSeg   //RbBus   //RbDev   //RbFunc  //Socket  //Die   //Data3
  //PCH
  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x00,     0xFF}, //(S0D0P0,typeA)
  {0x00,     0x00,     0xFF,     0x01,     0x02,     0xFF,     0x01,     0xFF}, //ASM1061R //<hujf1001-20210316 Fix SATA controller index conflict +> 

  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x01,     0xFF}, //(S0D1P1,typeA)

  {0x00,     0x00,     0xFF,     0x08,     0x01,     0xFF,     0x04,     0xFF}, //(S1D0P0,typeA)
  
  {0x00,     0x00,     0xFF,     0x01,     0x01,     0xFF,     0x05,     0xFF}, //(S1D1P1,typeA)

};
#endif
//<Kangmm-20190307 Update setup pci device list table on Rome->


EFI_STATUS GetSataControllerIndex(VOID *Dev, VOID **rBrg, UINTN *RPIndex)
{
  PCI_DEV_INFO          *Prt,*PreDev;
  UINT8                 RbNum;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode;
  ROOTB_DATA      *PCIRBEntry;
  ROOTB_DATA      *pRootPortlist;
  UINTN           RpCount;
  UINTN           i;
  UINT32          EAXReg;
  UINT32          EBXReg;
  BOOLEAN         IsSsp = TRUE;
  UINT32          SocFamilyID;    ///< SOC family ID
  UINT8           PackageType;    ///< SOC package type
  // Check SoC hardware, default is SSP
  SocFamilyID = 0x00830F00;//F17_SSP_RAW_ID
  PackageType = 4;//SSP_SP3
  
#define AMD_CPUID_FMF      0x80000001ul  // Family Model Features information
#define RAW_FAMILY_ID_MASK 0x0FFF0F00u
  
  AsmCpuid (
        AMD_CPUID_FMF,&(EAXReg),&(EBXReg),NULL,NULL);

    // Check Hardware Identification

    if (SocFamilyID != (EAXReg & RAW_FAMILY_ID_MASK)) {
      IsSsp = FALSE;
    }

    if (PackageType != ((EBXReg >> 28) & 0xF)) {
      IsSsp = FALSE;
    }
  if(IsSsp == TRUE)
  {
      pRootPortlist = PciRootPortTableSsp;
      RpCount = sizeof(PciRootPortTableSsp) / sizeof(ROOTB_DATA);
  }
  else{
      pRootPortlist = PciRootPortTableZp;
      RpCount = sizeof(PciRootPortTableZp) / sizeof(ROOTB_DATA);
  }
  
  
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
    RbNum = ((ACPI_HID_DEVICE_PATH *)DevicePathNode)->UID; 
  }
  *rBrg = Prt;
  PCIRBEntry = pRootPortlist;
  for( i = 0;i < RpCount;i++ ){

    if(((PCIRBEntry->RbBus == Prt->Address.Addr.Bus)||(PCIRBEntry->RbBus == 0xFF))
        &&((PCIRBEntry->RbDev == Prt->Address.Addr.Device)||(PCIRBEntry->RbDev == 0xFF))
        &&((PCIRBEntry->RbFunc == Prt->Address.Addr.Function)||(PCIRBEntry->RbFunc == 0xFF))
        &&((PCIRBEntry->Data2 == RbNum)||(PCIRBEntry->Data2 == 0xFF)))
    {
      *RPIndex = i;
       break;
    }
    
    PCIRBEntry++;
  }

  if(i == RpCount)
    return EFI_NOT_FOUND;
  
  DEBUG((-1,"RootPort is RbNum:%02x, Bus: %02x, DevNum:%02x, FuncNum:%02x\n", RbNum, Prt->Address.Addr.Bus, Prt->Address.Addr.Device, Prt->Address.Addr.Function));

  return EFI_SUCCESS;
  
}

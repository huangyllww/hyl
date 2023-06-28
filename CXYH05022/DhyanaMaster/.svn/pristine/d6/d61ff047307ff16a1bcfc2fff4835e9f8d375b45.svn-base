/** @file
  This file contains hooks for PeiPciEnumeration Module
  Patch some registers that device drivers concerns
  
  $Revision:0A
  $Date:  Machong 2017/07/22
  
Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.<BR>
 
**/

#include "PeiPciEnumeration.h"

//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
UINT64                                  PadBuffer = 0;
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->
 
VOID AmdBridgeInitFunc(EFI_PHYSICAL_ADDRESS PciDevBaseAddr)
{
  //Set the ast device VGA Support. {
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) &= 0xFFFF;
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) |= 0x1800ff;
  //Set the ast device VGA Support. } 
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_COMMAND_OFFSET) = 0x00100000; //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
  //Set the ast device Cacheline {
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_CACHELINE_SIZE_OFFSET) &= 0xFFFF0000; 
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_CACHELINE_SIZE_OFFSET) |= 0x10; 
  //Set the ast device Cacheline }
}

VOID ASTInitFunc(EFI_PHYSICAL_ADDRESS PciDevBaseAddr)
{  
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_COMMAND_OFFSET) = 0x02100000; //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) &= 0xFFFF;
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) |= 0x0105;
}

VOID ASTInitBridgeFunc(EFI_PHYSICAL_ADDRESS PciDevBaseAddr)
{ 
  //Set the ast device VGA Support. {
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) &= 0xFFFF;
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_INT_LINE_OFFSET) |= 0x180105;
  //Set the ast device VGA Support. } 
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_COMMAND_OFFSET) = 0x00100000; //Let IO & MMIO can Read/Write(Bit0:IO, Bit1:MMIO)
  //Set the ast device Cacheline {
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_CACHELINE_SIZE_OFFSET) &= 0xFFFF0000; 
  *(UINT32 *)((UINT8 *)PciDevBaseAddr + PCI_CACHELINE_SIZE_OFFSET) |= 0x10; 
  //Set the ast device Cacheline }
}
 
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>

//
// AMD PCIE Root Port Memory Resource Align On 16MB
//
VOID AmdRootPortMemoryAlign(UINT64 *PciDevBaseAddr)
{
    
  UINT64  TempBaseAddress;
  
  //Mizl-20200714-Need 32MB Align for Switch 16MB/32MB Function +>
  //TempBaseAddress = (*PciDevBaseAddr) & 0xff000000; //16MB Align
  TempBaseAddress = (*PciDevBaseAddr) & 0xFE000000; //32MB Align
  //Mizl-20200714-Need 32MB Align for Switch 16MB/32MB Function ->

  PadBuffer = (*PciDevBaseAddr) - TempBaseAddress;
  (*PciDevBaseAddr) = TempBaseAddress;
  
}

//
// ASPEED PCIE Root Port Memory Resource Align On 16MB
//
VOID AstRootMemoryAlign(UINT64 *PciDevBaseAddr)
{
  
  UINT64  TempBaseAddress;
  //Mizl-20200714-Need 32MB Align for Switch 16MB/32MB Function +>
  //TempBaseAddress = (*PciDevBaseAddr) & 0xff000000; //16MB Align
  TempBaseAddress = (*PciDevBaseAddr) & 0xFE000000; //32MB Align
  //Mizl-20200714-Need 32MB Align for Switch 16MB/32MB Function ->
  
  PadBuffer = (*PciDevBaseAddr) - TempBaseAddress;
  (*PciDevBaseAddr) = TempBaseAddress;

}

//
// Adjust ASPEED VGA Controller Memory Base Address
//
VOID AstVgaMemoryAlign(UINT64 *PciDevBaseAddr)
{
  (*PciDevBaseAddr) -= PadBuffer;
}
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->

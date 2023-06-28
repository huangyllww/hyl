/** @file
  This file contains OemPCIeDevPlatLib Use to Support OemPCIeDev Module
  
  Author:Kangmm
  Date:  20210120
  

  
Copyright (C) 2020 - 2021 Cancon Industry Co.,LTD.All rights reserved.<BR> 

**/
#include <Token.h> //Token
#include <Base.h>
#include <AmiLib.h>

#define AMD_CPUID_FMF      0x80000001ul  // Family Model Features information
#define RAW_FAMILY_ID_MASK 0x0FFF0F00ul
#define F17_SSP_RAW_ID     0x00830F00ul
#define F19_GN_RAW_ID      0x00A00F00ul
#define SSP_SP3  4
#define MAX_RB 8


#define FABRIC_BUS_NUM 0
#define FABRIC_DEV_NUM 0x18
#define FABRIC_FUNC_NUM 0

#define PCI_LIB_ADDRESS(Bus,Device,Function,Register)   \
  (((Register) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

UINT8 NaplesFabricId [] = { 04, 0x24, 0x44, 0x64, 0x84, 0xA4, 0xC4, 0xE4 };
UINT8 RomeFabricId [] = {0x18, 0x19, 0x1A, 0x1B, 0x38, 0x39, 0x3A, 0x3B};
/**
    This function returns TRUE if the platform is SSP.
   
    @param     NONE
    @retval    TRUE      Current platform is SSP
    @retval    FALSE     Current platform is non-SSP
*/
 BOOLEAN
 IsSOCSSP (
    )
 {
      UINT32 EAXReg;
      UINT32 EBXReg;

      AsmCpuid (AMD_CPUID_FMF,&EAXReg,&EBXReg,NULL,NULL);

      // Check Hardware Identification
      if (F17_SSP_RAW_ID != (EAXReg & RAW_FAMILY_ID_MASK)) {
          return FALSE;
      }

      if (SSP_SP3 != ((EBXReg >> 28) & 0xF)) {
          return FALSE;
      }
      return TRUE;
}
 /**
     This function returns TRUE if the platform is GN.
    
     @param     NONE
     @retval    TRUE      Current platform is GN
     @retval    FALSE     Current platform is non-GN
 */
 BOOLEAN
 IsSOCGn (
   )
 {
     UINT32 EAXReg;
     UINT32 EBXReg;

   AsmCpuid (AMD_CPUID_FMF,&EAXReg,&EBXReg,NULL,NULL);

   // Check Hardware Identification
   if (F19_GN_RAW_ID != (EAXReg & RAW_FAMILY_ID_MASK)) {
     return FALSE;
   }

   if (SSP_SP3 != ((EBXReg >> 28) & 0xF)) {
     return FALSE;
   }

   return TRUE;
 }
 /**
     This function returns the bus number for related NBIO.
    
     @param[in]  SocketNum         Socket number for the NBIO RootBridge
     @param[in]  DieNum            Die number for the NBIO RootBridge
     @param[out] RbBusNum          Return the bus number for the NBIO RootBridge
     @param[out] FabricId          Return the FabricId for the NBIO RootBridge
 */
UINT8 
GetCPURbBusNumAndFabricId(
    IN  UINT8            SocketNum,
    IN  UINT8            DieNum,
    OUT UINT8            *FabricId
  )
{
  UINTN                             PciAddress;
  UINT8                             DstFabricIDTemp = 0;
  UINT8                             DFCfgAddrMap = 0xA0;
  UINT32                            DFCfgAddrData = 0;
  UINT8                             Index = 0;
  UINT8                             RbBusNum;
  
  DEBUG((DEBUG_INFO, "GetCPURbBusNumAndFabricId Entry\n"));
 //Scan DF configuration address maps to get bus number
  for(Index=0 ; Index < MAX_RB ; Index++){ 
    PciAddress = PCI_LIB_ADDRESS(FABRIC_BUS_NUM,FABRIC_DEV_NUM,FABRIC_FUNC_NUM,DFCfgAddrMap + Index*4);
    DFCfgAddrData = MmioRead32((UINTN) PcdGet64(PcdPciExpressBaseAddress) + PciAddress);
    DstFabricIDTemp = (DFCfgAddrData>>4)&(0x3FF);    //DstFabricID data from bit 4-13
    DEBUG((DEBUG_INFO, "DstFabricIDTemp %x\n",DstFabricIDTemp));
    if(IsSOCSSP()||IsSOCGn())
    {
      if(DstFabricIDTemp == RomeFabricId[SocketNum *4 + DieNum])
      {
        *FabricId = (UINT8)DstFabricIDTemp;
        RbBusNum = (DFCfgAddrData>>16)&(0xFF);
        DEBUG((DEBUG_INFO, "RbBusNum %x\n",RbBusNum));
        break;
      }

    }
    else{
      if(DstFabricIDTemp == NaplesFabricId[SocketNum *4 + DieNum])
      {
        *FabricId = (UINT8)DstFabricIDTemp;
        RbBusNum = (DFCfgAddrData>>16)&(0xFF);
        break;
      }
    }
  }
  return RbBusNum;
}
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>
UINT8 
GetCPURbBusNumLimitAndFabricId(
    IN  UINT8            SocketNum,
    IN  UINT8            DieNum,
    OUT UINT8            *FabricId
  )
{
  UINTN                             PciAddress;
  UINT8                             DstFabricIDTemp = 0;
  UINT8                             DFCfgAddrMap = 0xA0;
  UINT32                            DFCfgAddrData = 0;
  UINT8                             Index = 0;
  UINT8                             RbBusNumLimit;
  
  DEBUG((DEBUG_INFO, "GetCPURbBusNumLimitAndFabricId Entry\n"));
 //Scan DF configuration address maps to get bus number
  for(Index=0 ; Index < MAX_RB ; Index++){ 
    PciAddress = PCI_LIB_ADDRESS(FABRIC_BUS_NUM,FABRIC_DEV_NUM,FABRIC_FUNC_NUM,DFCfgAddrMap + Index*4);
    DFCfgAddrData = MmioRead32((UINTN) PcdGet64(PcdPciExpressBaseAddress) + PciAddress);
    DstFabricIDTemp = (DFCfgAddrData>>4)&(0x3FF);    //DstFabricID data from bit 4-13
    DEBUG((DEBUG_INFO, "DstFabricIDTemp %x\n",DstFabricIDTemp));
    if(IsSOCSSP()||IsSOCGn())
    {
      if(DstFabricIDTemp == RomeFabricId[SocketNum *4 + DieNum])
      {
        *FabricId = (UINT8)DstFabricIDTemp;
        RbBusNumLimit = (DFCfgAddrData>>24)&(0xFF);
        DEBUG((DEBUG_INFO, "RbBusLimit %x\n",RbBusNumLimit));
        break;
      }

    }
    else{
      if(DstFabricIDTemp == NaplesFabricId[SocketNum *4 + DieNum])
      {
        *FabricId = (UINT8)DstFabricIDTemp;
        RbBusNumLimit = (DFCfgAddrData>>24)&(0xFF);
        break;
      }
    }
  }
  return RbBusNumLimit;
}
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->

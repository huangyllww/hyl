#include "Uefi\UefiBaseType.h"
#include "Pi\PiPeiCis.h"
#include "AmdCpmPreDefine.h"
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>
#include "Library\PciLib.h"
#include "Pci.h"
#include "Library\BaseMemoryLib.h"
#include "Ppi\ReadOnlyVariable2.h"
#include "Setup.h"

#include "FabricRegistersZP.h"
#include "AmdSoc.h"

#include "Library/SmnAccessLib.h"
#include "Library/BaseFabricTopologyLib.h"

#include "OemLib.h"
typedef struct _BUS_ARRARY
{
  UINT8 BusBase;
  UINT8 BusLimit;
} BUS_ARRAY;

UINT8
CpuPkgIndentify()
{
  UINT8 PkgType;
  UINT32 EAX_Reg;
  UINT32 EBX_Reg;
  UINT32 ECX_Reg;
  UINT32 EDX_Reg;

  AsmCpuid(
      0x80000001,
      &EAX_Reg,
      &EBX_Reg,
      &ECX_Reg,
      &EDX_Reg);
  PkgType = (EBX_Reg >> 28) & 0x0F;

  return PkgType;
}

EFI_STATUS
EFIAPI
DataFabricBusTopUpdate_Entry(
    IN EFI_PEI_FILE_HANDLE FileHandle,
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status;
  BUS_ARRAY aBus[] = {DataFaricBusScope, {0xFF, 0xFF}};
  UINT8 idx, icam;
  UINT32 DataFabricAddress;
  CFG_ADDRESS_MAP_REGISTER sConfigBusMap;
  CFG_ADDRESS_CTRL_REGISTER sConfigAddCtl;
  UINT32 PIECount;

  MODULE_START
  /**
  Note:Just For SP3 & 2 CPU Installed Config
  **/
  if (CpuPkgIndentify() != ZP_SP3)
    MODULE_END(Status = EFI_UNSUPPORTED)

  SmnRegisterRead((UINT32)(FabricTopologyGetHostBridgeBusBase(0, 0, 0)), (0x1c400 + 0x204), &PIECount);
  PIECount=PIECount&0x0F;
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]PIECount:%x\r\n", __FUNCTION__, __LINE__, PIECount));
  if (PIECount < (MAX_DIES_PER_SOCKET + 1))
    MODULE_END(Status = EFI_UNSUPPORTED)

  if (aBus[0].BusBase == 0xFF)
    MODULE_END(Status = EFI_NO_MAPPING)

  //Program Data Fabric Reg
  for (idx = 0; idx < MAX_DIES_PER_SOCKET * 2; idx++)
  {
    /* code */
    DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, PCI_VID);
    if (PciRead16(DataFabricAddress) == 0xFFFF) //If No Device Present,Continue
      continue;

    //Update Secondary Bus Number
    DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, 0x84);
    sConfigAddCtl.Value = PciRead32(DataFabricAddress);
    sConfigAddCtl.Field.SecBusNum = aBus[idx].BusBase;
    // DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Offset:%x  sConfigAddCtl Write:%x\r\n",__FUNCTION__,__LINE__,DataFabricAddress,sConfigAddCtl.DWord));
    PciWrite32(DataFabricAddress, sConfigAddCtl.Value);
    // DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Offset:%x  sConfigAddCtl Read:%x\r\n",__FUNCTION__,__LINE__,DataFabricAddress,PciRead32(DataFabricAddress)));
    //Program Configuration Address Maps]
    for (icam = 0; icam < 8; icam++)
    {
      /* code */
      DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, 0xA0 + icam * 4);
      sConfigBusMap.Value = PciRead32(DataFabricAddress);
      //Update Config BusBase/Bus Limit
      if ((sConfigBusMap.Field.DstFabricID & 0x04) == 0x04)
      {
        sConfigBusMap.Field.BusNumBase = aBus[(sConfigBusMap.Field.DstFabricID >> 5)].BusBase;
        sConfigBusMap.Field.BusNumLimit = aBus[(sConfigBusMap.Field.DstFabricID >> 5)].BusLimit;
        PciWrite32(DataFabricAddress, sConfigBusMap.Value);
      }
    }
  }
  //Dump DataFabric Reg
  for (idx = 0; idx < 8; idx++)
  {
    UINT8 SecondaryBusNumber;
    UINT8 DieNumber;

    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]idx:%x ======>\r\n", __FUNCTION__, __LINE__, idx));
    DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, PCI_VID);
    if (PciRead16(DataFabricAddress) == 0xFFFF) //If No Device Present,Continue
      continue;

    DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, 0x84);
    SecondaryBusNumber = PciRead32(DataFabricAddress);
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Offset:%x SecondaryBus:%x\r\n", __FUNCTION__, __LINE__, DataFabricAddress, SecondaryBusNumber));

    for (icam = 0; icam < 8; icam++)
    {
      /* code */
      DataFabricAddress = PCI_LIB_ADDRESS(0, (0x18 + idx), 0, 0xA0 + icam * 4);
      sConfigBusMap.Value = PciRead32(DataFabricAddress);
      DieNumber = sConfigBusMap.Field.DstFabricID >> 5;
      if (DieNumber > 7)
        continue;
      DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Offset:%x Die:%x BusBase:%x BusLimit:%x - %x\r\n",
             __FUNCTION__, __LINE__, (0xA0 + icam * 4), DieNumber, sConfigBusMap.Field.BusNumBase, sConfigBusMap.Field.BusNumLimit, sConfigBusMap.Value));
    }
  }

  MODULE_END(Status = EFI_SUCCESS)
}
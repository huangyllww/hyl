//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file NbDxeMemFeat.c
    This file contains generic NB code that is common between
    various components such as NB DXE etc

**/

#include <DramMap.h>
#include <MemSmmLib.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmCpuIo.h>
#include <Library/NbCspLib.h>

UINT64  mMmioBase = 0;
UINT64  ReadMsr(UINT32 Msr);

EFI_SMM_SYSTEM_TABLE    *mSmst;



/*----------------------------------------------------------------------------------------*/
/**
 *  LibAgesaSmmMemRead
 *  Description
 *      SMM Memory Read Access
 *
 * @param[in]       SmmIo           Pointer to SMM CPU IO interface
 * @param[in]       AccessWidth     Access width
 * @param[in]       Address         Memory address
 * @param[out]      Value           Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibAgesaSmmMemRead (
  IN       EFI_SMM_CPU_IO_INTERFACE    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT64                      Address,
     OUT   VOID                        *Value
  )
{
  SmmIo->Mem.Read (
                   SmmIo,
                   AccessWidth,
                   Address,
                   1,
                   Value
                   );
}

/*----------------------------------------------------------------------------------------*/
/**
 *  LibAgesaSmmMemWrite
 *  Description
 *      SMM Memory Write
 *
 * @param[in]       SmmIo           Pointer to SMM CPU IO interface
 * @param[in]       AccessWidth     Access width
 * @param[in]       Address         Memory address
 * @param[out]      Value           Pointer to data buffer
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibAgesaSmmMemWrite (
  IN       EFI_SMM_CPU_IO_INTERFACE    *SmmIo,
  IN       EFI_SMM_IO_WIDTH            AccessWidth,
  IN       UINT64                      Address,
  IN       VOID                        *Value
  )
{
  SmmIo->Mem.Write (
                   SmmIo,
                   AccessWidth,
                   Address,
                   1,
                   Value
                   );
}


/*----------------------------------------------------------------------------------------*/
/**
 *  Set_PCI
 *  Description
 *      Sets the value at given Bus, Device, Function and Offset
 *
 * @param[in]       Bus           Bus
 * @param[in]       Device        Device (mostly 24)
 * @param[in]       Function      Function
 * @param[in]       Foffset       Offset
 * @param[out]      Value         Value
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
Set_PCI (
  IN      UINT32   Bus,
  IN      UINT32   Device,
  IN      UINT32   Function,
  IN      UINT32   Foffset,
  IN      UINT64   Value
  )
{
  UINT32        AddressValue;

//  AddressValue = MAKE_SBDFO (0, Bus, Device, Function, Foffset);
  AddressValue = ((UINT32)1 << 31) +
        (Bus       << 16) +
        (Device    << 11) +
        (Function  << 8) +
        (Foffset  &  0xFC) +
        ((Foffset & 0xF00) << (24 - 8));
  AddressValue =  (UINT32) mMmioBase | AddressValue;
  LibAgesaSmmMemWrite (&mSmst->SmmIo, 2, AddressValue, &Value);
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Get_PCI
 *  Description
 *      Reads the value at given Bus, Device, Function and Offset
 *
 * @param[in]       Bus           Bus
 * @param[in]       Device        Device (mostly 24)
 * @param[in]       Function      Function
 * @param[in]       Foffset       Offset
 *
 */
/*----------------------------------------------------------------------------------------*/
UINT64
Get_PCI (
  IN      UINT32   Bus,
  IN      UINT32   Device,
  IN      UINT32   Function,
  IN      UINT32   Foffset
  )
{
  UINT32        AddressValue;
  UINT64        Value;

//  AddressValue = MAKE_SBDFO (0, Bus, Device, Function, Foffset);
  AddressValue = ((UINT32)1 << 31) +
        (Bus       << 16) +
        (Device    << 11) +
        (Function  << 8) +
        (Foffset  &  0xFC) +
        ((Foffset & 0xF00) << (24 - 8));
  AddressValue =  (UINT32) mMmioBase | AddressValue;
  LibAgesaSmmMemRead (&mSmst->SmmIo, 2, AddressValue, &Value);
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  CSInterleavingBitSwap
 *  Description
 *      Function returns a normalized address after adjusting for possible bit swapping
 *      by CS Interleaving.
 *
 * @param[in]       addr                addr
 * @param[in]       CSMask              CSMask)
 * @param[in]       hiBit               hiBit
 * @param[in]       lowBit              lowBit
 *
 */
/*----------------------------------------------------------------------------------------*/

UINT64
CSInterleavingBitSwap
(
  IN       UINT64 addr,
  IN       UINT64 CSMask,
  IN       UINT8 hiBit,
  IN       UINT8 lowBit
  )
{
  UINT64    mask;
  // Bits are swapped if the lower CSMask bits are zeros.
  // hiBit, lowBit indicate the LSB at the start of the range of bits that
  // take part in the swap an depend on the DimmAddrMap.
  // Sanity check the CSMask
  mask = ( CSMask >> lowBit & 0x7) ^ 0x7;
  return ( mask ? (addr & ~(mask << hiBit) & ~(mask << lowBit)) |
    (((addr >> hiBit) & mask) << lowBit) |
    (((addr >> lowBit) & mask) << hiBit) : addr);
}

/*----------------------------------------------------------------------------------------*/
/**
 *  AddrToBankSwizzle
 *  Description
 *      Function returns a DRAM device bank address after adjusting for possible
 *      remapping by bank swizzling. See D18F2x94[BankSwizzleMode]
 *
 * @param[in]       swizzle     swizzle
 * @param[in]       addr        addr)
 * @param[in]       bankbit     bankbit
 * @param[in]       offset      offset
 *
 */
/*----------------------------------------------------------------------------------------*/
UINT8
AddrToBankSwizzle (
  IN      BOOLEAN swizzle,
  IN      UINT64  addr,
  IN      UINT8   bankbit,
  IN      UINT8   offset
  )
{
  // bankbit indicates the LSB at the start of the range of bits that
  // are mapped to the DRAM device bank address and depends on the DimmAddrMap.
  if (!swizzle) {
    return (UINT8) (((addr >> bankbit) & 0x7));
    } else {
      return (
        (UINT8) ((((addr >> bankbit) & 1) ^ ((addr >> (bankbit + 3 + offset)) & 1) ^
        ((addr >> (bankbit + 6 + offset)) & 1)) |
        ((((addr >> (bankbit + 1)) & 1) ^ ((addr >> (bankbit + 4 + offset)) & 1) ^
        ((addr >> (bankbit + 7 + offset)) & 1)) << 1) |
        ((((addr >> (bankbit + 2)) & 1) ^ ((addr >> (bankbit + 5 + offset)) & 1) ^
        ((addr >> (bankbit + 8 + offset)) & 1)) << 2)
        ));
    }
}

/*----------------------------------------------------------------------------------------*/
/**
 *  BankSwapAddr
 *  Description
 *      Function returns a normalized address after adjusting for possible bit swapping
 *      by CS Interleaving.
 *
 * @param[in]       BankSwap            BankSwap
 * @param[in]       addr                addr
 * @param[in]       DctSelBankSwap      DctSelBankSwap
 *
 */
/*----------------------------------------------------------------------------------------*/

UINT64
BankSwapAddr
(
  IN  BOOLEAN   BankSwap,
  IN  UINT64    addr,
  IN  BOOLEAN   DctSelBankSwap
  )
{
  if (BankSwap) {
    if (DctSelBankSwap) {
      // [10:8] swapped with [15:13]
      addr = ((((addr >> 8) & 0x7) << 13) | (((addr >> 13) & 0x7) << 8) | (addr & ~(0xE700)));
      } else {
        // [11:9] swapped with [15:13]
        addr = ((((addr >> 9) & 0x7) << 13) | (((addr >> 13) & 0x7) << 9) | (addr & ~(0xEE00)));
      }
    }
  return addr;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  BankSwizzleToAddr
 *  Description
 *      Function returns a normalized DRAM addresss after adjusting for possible
 *      remapping by bank swizzlling. See D18F2x[1,0]94[BankSwizzleMode]
 *
 * @param[in]       swizzle     swizzle
 * @param[in]       addr        addr
 * @param[in]       bank        bank
 * @param[in]       bankbit     bankbit
 * @param[in]       offset      offset
 *
 */
/*----------------------------------------------------------------------------------------*/
UINT64
BankSwizzleToAddr
(
  IN       BOOLEAN    swizzle,
  IN       UINT64     addr,
  IN       UINT8      bank,
  IN       UINT8      bankbit,
  IN       UINT8      offset
  )
{
  // Recreate the normalized address; Note: A == A ^ B ^ B
  if (!swizzle) {
    addr |= bank << bankbit;
    } else {
      addr |= (
        ((bank & 1) ^ ((addr >> (bankbit + 3 + offset)) & 1) ^ ((addr >> (bankbit + 6 + offset)) & 1)) |
        ((((bank >> 1) & 1) ^ ((addr >> (bankbit + 4 + offset)) & 1) ^ ((addr >> (bankbit + 7 + offset)) & 1)) << 1) |
        ((((bank >> 2) & 1) ^ ((addr >> (bankbit + 5 + offset)) & 1) ^ ((addr >> (bankbit + 8 + offset)) & 1)) << 2)
        ) << bankbit;
    }
  return addr;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  NormalizedToBankAddrMap
 *  Description
 *      This function converst the given physical address into DIMM specific info
 *
 * @param[in]       ChannelAddr         ChannelAddr
 * @param[in]       CSMask              CSMask)
 * @param[in]       DimmAddrMap         DimmAddrMap
 * @param[in]       BankSwap            BankSwap
 * @param[in]       DctSelBankSwap      DctSelBankSwap
 * @param[in]       BankSwizzleMode     BankSwizzleMode
 * @param[in, out]  *Bank               Bank
   @param[in, out]  *Row                Row
   @param[in, out]  *Col                Col
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
NormalizedToBankAddrMap (
  IN       UINT64  ChannelAddr,
  IN       UINT64  CSMask,
  IN       UINT8   DimmAddrMap,
  IN       BOOLEAN BankSwap,
  IN       BOOLEAN DctSelBankSwap,
  IN       BOOLEAN BankSwizzleMode,
  IN OUT   UINT8   *Bank,
  IN OUT   UINT32  *Row,
  IN OUT   UINT16  *Col
  )
{
  UINT64  Addr;

  Addr = 0;

  ChannelAddr = BankSwapAddr (BankSwap, ChannelAddr, DctSelBankSwap);

  // BankSwizzleMode is calculated on the normalized addr
  // Select Row/Col based on the CS interleave adjustment tables
  switch (DimmAddrMap) {
  case 0x1:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 13, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 28, !BankSwap ? 16 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32)(((Addr >> 18) & 0x3FF) | (((Addr >> 16) & 0x3) << 10));
    *Col = (UINT16)((Addr >> 3) & 0x3FF);
    break;
  case 0x2:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 13, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 29, !BankSwap ? 16 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32) (((Addr >> 18) & 0x7FF) | (((Addr >> 16) & 0x3) << 11));
    *Col = (UINT8)((Addr >> 3) & 0x3FF);
    break;
  case 0x5:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 13, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 30, !BankSwap ? 16 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32) (((Addr >> 18) & 0xFFF) | (((Addr >> 16) & 0x3) << 12));
    *Col = (UINT8) ((Addr >> 3) & 0x3FF);
    break;
  case 0x7:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 13, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 31, !BankSwap ? 16 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32) (((Addr >> 18) & 0x1FFF) | (((Addr >> 16) & 0x3) << 13));
    *Col = (UINT8) ((Addr >> 3) & 0x3FF);
    break;
  case 0xA:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 13, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 32, !BankSwap ? 16 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32) (((Addr >> 18) & 0x3FFF) | (((Addr >> 16) & 0x3) << 14));
    *Col = (UINT8)((Addr >> 3) & 0x3FF);
    break;
  case 0xB:
    *Bank = AddrToBankSwizzle (BankSwizzleMode, ChannelAddr, 14, 0);
    Addr = CSInterleavingBitSwap (ChannelAddr, CSMask, 33, !BankSwap ? 17 : (DctSelBankSwap ? 11 : 12));
    *Row = (UINT32) (((Addr >> 18) & 0x7FFF) | (((Addr >> 17) & 0x1) << 15));
    *Col = (UINT8) (((Addr >> 3) & 0x3FF) | (((Addr >> 13) & 0x1) << 11));
    break;
  default:
    break;
    }
}

/*----------------------------------------------------------------------------------------*/
/**
 *  PlatformTranslateSysAddrToCS
 *  Description
 *      This function converst the given physical address into DIMM specific info
 *
 * @param[in]       SysAddr         SysAddr
 * @param[out]      CSFound         CSFound)
 * @param[out]      NodeID          NodeID
 * @param[out]      ChannelSelect   ChannelSelect
 * @param[out]      ChipSelect      ChipSelect
 * @param[out]      Bank            Bank
 * @param[out]      Row             Row
 * @param[out]      Col             Col
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
PlatformTranslateSysAddrToCS (
  IN       UINT64      SysAddr,
     OUT   BOOLEAN     *CSFound,
     OUT   UINT8       *NodeID,
     OUT   UINT8       *ChannelSelect,
     OUT   UINT8       *ChipSelect,
     OUT   UINT8       *Bank,
     OUT   UINT32      *Row,
     OUT   UINT16      *Col
  )
{
  UINT32    node;
  UINT32    range;
  UINT32    cs;
  UINT32    dctrange;
  UINT32    foffset;
  UINT64    temp;
  BOOLEAN   DramEn;
  UINT64    DramBase;
  UINT64    DramLimit;
  UINT64    DramBaseSysAddr;
  UINT64    DramLimitSysAddr;
  UINT64    DramHoleBase;
  UINT64    DramHoleOffset;
  UINT64    DctBaseAddr;
  UINT64    DctLimitAddr;
  BOOLEAN   DramMemHoistValid;
  BOOLEAN   DctSelBankSwap;
  BOOLEAN   DctAddrVal;
  BOOLEAN   LgcyMmioHoleEn;
  UINT32    DctSel;
  UINT64    ChannelOffset;
  UINT64    ChannelAddr;
  UINT64    CSBase;
  UINT64    CSMask;
  BOOLEAN   CSEn;
  UINT64    InputAddr;
  BOOLEAN   BankSwizzleMode;
  BOOLEAN   BankSwap;
  UINT8 DimmAddrMap;

  *CSFound = FALSE;
  // Loop to determine the dram range
  for (node = 0; node < NUM_NODES; ++node) {
    for (range = 0; range < NUM_NODES; ++range) {
      // Extract variables from Dram Base
      foffset = 0x40 + (range << 3);
      temp = Get_PCI (BUS_0, (DEVICE_24 + node), FUNCTION_1, foffset);
      DramEn = (temp & 0x3) > 0 ? TRUE : FALSE;
      if (!DramEn) {
        continue;
      }
      DramBase = (((UINT64)Get_PCI (BUS_0, (DEVICE_24 + node), FUNCTION_1, foffset + 0x100) & 0xFF) << 40) | ((temp & 0xFFFF0000) << 8);
      // Extract variables from Dram Limit
      temp = Get_PCI (BUS_0, (DEVICE_24 + node), FUNCTION_1, foffset + 4);
      *NodeID = (UINT8)(temp & 0x7);
      DramLimit = (((UINT64)Get_PCI (BUS_0, (DEVICE_24 + node), FUNCTION_1, foffset + 0x104) & 0xFF) << 40) | ((temp << 8) | 0xFFFFFF);

      // For compatiblity with DRAM C6 Storage, extract variables from the per-node DRAM Base/Limit System Address
      temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0x120);
      DramBaseSysAddr = (temp & 0x1FFFFF) << 27;
      // Extract variables from Dram Limit
      temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0x124);
      DramLimitSysAddr = ((temp & 0x1FFFFF) << 27) | 0x7FFFFFF;

      if ((DramEn && DramBase <= SysAddr && SysAddr <= DramLimitSysAddr)) {
        // Extract variables from D18F1xF0
        temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0xF0);
        DramHoleOffset = (temp & 0xFF80) << 16;
        DramMemHoistValid = (temp & 0x2) > 0 ? TRUE : FALSE;
        DramHoleBase = ((temp >> 24) & 0xFF) << 24;
        // Address belongs to this node based on DramBase/Limit,
        // but is in the memory hole so it doesn't map to DRAM
        if (DramMemHoistValid && DramHoleBase <= SysAddr && SysAddr < 0x100000000) {
          return;
          }
        // Extract variables from D18F2x114
        temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_2, 0x114);
        DctSelBankSwap = ((temp >> 9) & 0x1) > 0 ? TRUE : FALSE;

        // Single Channel
        DctAddrVal = FALSE;
        for (dctrange = 0; dctrange < 4; ++dctrange) {
          // Extract variables from F2x2[1,0][8,0]
          temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0x200 + 8 * dctrange);
          DctAddrVal = (temp & 0x1) > 0 ? TRUE : FALSE;
          if (!DctAddrVal) {
            continue;
          }
          DctBaseAddr = (((temp >> 11) & 0x1FFF) << 27);
          DctSel = (UINT32) ((temp >> 4) & 0x7);
          LgcyMmioHoleEn = ((temp >> 1) & 0x1) > 0 ? TRUE : FALSE;
          // Extract variables from F2x2[1,0][C,4]
          temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0x204 + 8 * dctrange);
          DctLimitAddr = (((temp >> 11) & 0x1FFF) << 27);

          if (DctAddrVal) {
            break;
          }
        }

        *ChannelSelect = 0;
        ChannelOffset = 0;
        if (DramMemHoistValid && (SysAddr >= 0x100000000)) {
          ChannelOffset = DramHoleOffset;
          } else {
            ChannelOffset = DramBase;
          }

        // Remove hoisting offset and normalize to DRAM bus addresses
        ChannelAddr = SysAddr - ChannelOffset;

        // Set DctCfgSel = ChannelSelect
        Set_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_1, 0x10C, *ChannelSelect);
        // Determine the Chip Select
        for (cs = 0; cs < NUM_CS_PER_CHANNEL; ++cs) {
          // Obtain the CS Base from D18F2x[1,0][4C:40]
          foffset = 0x40 + (cs << 2);
          temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_2, foffset);
          CSEn = (temp & 0x1) > 0 ? TRUE : FALSE;
          CSBase = (((temp >> 19) & 0xFFF) << 27) | (((temp >> 5) & 0x3FF) << 11);
          // Obtain the CS Mask from D18F2x[64:60]
          foffset = 0x60 + ((cs >> 1) << 2);
          temp = Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_2, foffset);
          CSMask = (((temp >> 19) & 0xFFF) << 27) | (((temp >> 5) & 0x3FF) << 11);
          // Adjust the Channel Addr for easy comparison
          InputAddr = (((ChannelAddr >> 27) & 0xFFF) << 27) | (((ChannelAddr >> 11) & 0x3FF) << 11);

          if (CSEn && (InputAddr & ~CSMask) == (CSBase & ~CSMask)) {
            *CSFound = TRUE;
            *ChipSelect = (UINT8)cs;
            // Extract variables from D18F2x80
            DimmAddrMap = (UINT8) ((((UINT64)Get_PCI (BUS_0, (DEVICE_24 + *NodeID), FUNCTION_2, 0x80)) >> ((*ChipSelect >> 1) * 4)) & 0xF);
            // Extract variables from the CS Mask
            foffset = 0x60 + ((*ChipSelect >> 1) << 2);

            // Extract variables from D18F2x94
            BankSwizzleMode = ((Get_PCI (BUS_0, DEVICE_24 + *NodeID, FUNCTION_2, 0x94) >> 22) & 0x1) > 0 ? TRUE : FALSE;
            // Extract variables from D18F2xA8
            BankSwap = ((Get_PCI (BUS_0, DEVICE_24 + *NodeID, FUNCTION_2, 0xA8) >> 20) & 0x1) > 0 ? TRUE : FALSE;
            // Determine the Bank, Row, and Col
            NormalizedToBankAddrMap (ChannelAddr, CSMask, DimmAddrMap, BankSwap, DctSelBankSwap, BankSwizzleMode, Bank, Row, Col);
            // Sanity check outputs
            return;
          }
         }
       }
     }
   }

  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  TranslateSysAddrToCS
 *  Description
 *      This function converst the given physical address into DIMM specific info
 *
 * @param[in]       HygonDimmInfo       HygonDimmInfo
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
TranslateSysAddrToCS (
  IN  HYGON_IDENTIFY_DIMM   *HygonDimmInfo
  )
{
  BOOLEAN   ChipSelectFound;

  ChipSelectFound = FALSE;

  //
  // Get the MmioBase address to access extended config info
  //
  mMmioBase = ReadMsr (0xC0010058);
  mMmioBase &= 0x000000FFFFF00000;

  PlatformTranslateSysAddrToCS (
      HygonDimmInfo->MemoryAddress,
      &ChipSelectFound,
      &HygonDimmInfo->SocketId,
      &HygonDimmInfo->MemChannelId,
      &HygonDimmInfo->ChipSelect,
      &HygonDimmInfo->Bank,
      &HygonDimmInfo->Row,
      &HygonDimmInfo->Column
      );

  HygonDimmInfo->DimmId = HygonDimmInfo->ChipSelect / 2;
  HygonDimmInfo->ChipSelect %= 2;

  if (!ChipSelectFound) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}

/**
    Get the information of the DIMM location indicated by MemoryAddr

    @param MemoryAddr         The system address to convert
    @param NbAddressDecode    Pointer to the buffer used to store NB_ADDRESS_DECODE

    @retval EFI_UNSUPPORTED   This function is not supported
    @retval EFI_SUCCESS       Success to get the information of DIMM location

**/
EFI_STATUS NbGetDimmLocInfo (
    IN CONST UINTN        MemoryAddr,
    OUT NB_ADDRESS_DECODE *NbAddressDecode
)
{
    HYGON_IDENTIFY_DIMM   DimmInfo;
    EFI_STATUS            Status;
/**** PORTING REQUIRED ****
    Please check if MRC supports memory address decode.
    If it is not supported by MRC, please return EFI_UNSUPPORTED.
**** PORTING REQUIRED ****/

    DimmInfo.MemoryAddress = (UINT64)MemoryAddr;
    Status = TranslateSysAddrToCS(&DimmInfo);

    NbAddressDecode->ChannelNumber = (UINT8) DimmInfo.MemChannelId;
    NbAddressDecode->DimmNumber    = (UINT8) DimmInfo.DimmId;
    NbAddressDecode->Rank          = (UINT8) DimmInfo.ChipSelect;
    NbAddressDecode->Bank          = (UINT8) DimmInfo.Bank;
    NbAddressDecode->Ras           = (UINT16) DimmInfo.Row;
    NbAddressDecode->Cas           = (UINT16) DimmInfo.Column;

    return Status;
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

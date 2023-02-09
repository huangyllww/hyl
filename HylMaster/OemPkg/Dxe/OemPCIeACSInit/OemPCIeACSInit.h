#ifndef __OEM_PCIE_ACS_INIT_H__
#define __OEM_PCIE_ACS_INIT_H__

#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include "Library\UefiLib.h"

typedef enum {
  AddrTypePCIE    = 0,
  AddrTypePCIEBLK,
  AddrTypeCSR,
  AddrTypeMMIO,
  AddrTypeIO,
  AddrTypeMaximum
} USRA_ADDR_TYPE;

typedef enum {
  UsraWidth8  = 0,
  UsraWidth16,
  UsraWidth32,
  UsraWidth64,
  UsraWidthFifo8,
  UsraWidthFifo16,
  UsraWidthFifo32,
  UsraWidthFifo64,
  UsraWidthFill8,
  UsraWidthFill16,
  UsraWidthFill32,
  UsraWidthFill64,
  UsraWidthMaximum
} USRA_ACCESS_WIDTH;

typedef struct
  {
    UINT32              Offset;             // This Offset  occupies 32 bits. It's platform code's responsibilty to define the meaning of specific
                                            // bits and use them accordingly.
    UINT32              InstId:8;           // The Box Instance, 0 based, Index/Port within the box, Set Index as 0 if the box has only one instances
    UINT32              SocketId:8;         // The socket Id
    UINT32              InstType:8;         // The Instance Type, it can be Box, Memory Channel etc.
    UINT32              RFU:8;              // Reserved for User use or Future Ues

} USRA_CSR_ADDR_TYPE;

typedef struct
  {
    UINT32              RawData32[2];       // RawData of two UINT32 type, place holder
    UINT32              AddrType:8;         // Address type: CSR, PCIE, MMIO, IO, SMBus ...
    UINT32              AccessWidth:4;      // The Access width for 8, 16,32,64 -bit access
    UINT32              FastBootEn:1;       // Fast Boot Flag, can be used to log register access trace for fast boot
    UINT32              S3Enable:1;         // S3 Enable bit, when enabled, it will save the write to script to support S3
    UINT32              HptrType:1;         // Host Pointer type, below or above 4GB
    UINT32              ConvertedType:1;    // The address type was from converted type, use this field for address migration support
    UINT32              RFU3:16;            // Reserved for User use or Future Use

    UINT32              HostPtr:32;         // The Host Pointer, to point to Attribute buffer etc.
} ADDR_ATTRIBUTE_TYPE;

typedef struct
  {
    UINT32              Offset:12;          // The PCIE Register Offset
    UINT32              Func:3;             // The PCIE Function
    UINT32              Dev:5;              // The PCIE Device
    UINT32              Bus:8;              // The PCIE Bus
    UINT32              RFU1:4;             // Reserved for User use or Future Use
    UINT32              Seg:16;             // The PCI Segment
    UINT32              Count:16;           // Access Count

} USRA_PCIE_ADDR_TYPE;

typedef struct
  {
    UINT32              Offset:32;          // The MMIO Offset
    UINT32              OffsetH: 32;        // The MMIO Offset Higher 32-bit
} USRA_MMIO_ADDR_TYPE;

typedef struct
  {
    UINT32              Offset:16;          // The IO Offset
    UINT32              RFU1:16;            // Reserved for User use or Future Use
    UINT32              RFU2:32;            // Reserved for User use or Future Use
} USRA_IO_ADDR_TYPE;

typedef union {
    UINT32                  dwRawData[4];
    ADDR_ATTRIBUTE_TYPE     Attribute;          // The address attribute type.
    USRA_PCIE_ADDR_TYPE     Pcie;
    USRA_PCIE_ADDR_TYPE     PcieBlk;
    USRA_CSR_ADDR_TYPE      Csr;
    USRA_MMIO_ADDR_TYPE     Mmio;
    USRA_IO_ADDR_TYPE       Io;
} USRA_ADDRESS;

#define USRA_ZERO_ADDRESS(Address)  \
    ((UINT32 *)&Address)[3] = (UINT32)0; \
    ((UINT32 *)&Address)[2] = (UINT32)0; \
    ((UINT32 *)&Address)[1] = (UINT32)0; \
    ((UINT32 *)&Address)[0] = (UINT32)0
//
// Assemble macro for USRA_PCIE_ADDR_TYPE
//
#define USRA_PCIE_SEG_ADDRESS(Address, WIDTH, SEG, BUS, DEV, FUNC, OFFSET)  \
    USRA_ZERO_ADDRESS(Address); \
    ((USRA_ADDRESS *)(&Address))->Attribute.AccessWidth  = WIDTH; \
    ((USRA_ADDRESS *)(&Address))->Attribute.AddrType     = AddrTypePCIE; \
    ((USRA_ADDRESS *)(&Address))->Pcie.Seg    = (UINT32)(SEG); \
    ((USRA_ADDRESS *)(&Address))->Pcie.Bus    = (UINT32)(BUS)  & 0xFF; \
    ((USRA_ADDRESS *)(&Address))->Pcie.Dev    = (UINT32)(DEV)  & 0x1F; \
    ((USRA_ADDRESS *)(&Address))->Pcie.Func   = (UINT32)(FUNC) & 0x07; \
    ((USRA_ADDRESS *)(&Address))->Pcie.Offset = (UINT32)(OFFSET) & 0x0FFF

#endif
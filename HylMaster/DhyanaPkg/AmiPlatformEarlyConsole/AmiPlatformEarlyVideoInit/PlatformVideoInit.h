//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PlatformVideoInit.h
    Definitions used by PlatformVideoInit.

**/

#ifndef _PEI_VIDEO_INIT_H__
#define _PEI_VIDEO_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiPeiLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <PiPei.h>
//---------------------------------------------------------------------------

#define VIDEO_SUCCESS               0x00
#define VIDEO_ERR_BAD_DEVPATH       0x01
#define VIDEO_ERR_BAD_PARM          0x02
#define VIDEO_ERR_OUT_OF_RESOURCES  0x03

typedef struct {
    UINT8   Bus;
    UINT8   Dev;
    UINT8   Func;
} DEV_PATH;

typedef struct {
    UINT8   *FontMap;
    UINT8   StartChar;
    UINT16  CharCount;
    UINT8   CharSize;
    UINT8   TargetTable;
} FONT_MAP_INFO;

typedef struct {
    UINT8       RootBus;            //Bus Number of the root bus containing the VGA controller.
    DEV_PATH    *DevPath;           //Pointer to a buffer of dev path structure that define the location of the video controller in the PCI topology
    UINT8       DevPathEntries;     //Count of entries in the preceeding buffer.
    UINT32      PciExpressCfgBase;  //The base address of PCI Express Memory Mapped Configuration Space.
    UINT32      MemBase;            //The base address of the region where MMIO BARs are assigned when configuring the VGA controller
    UINT32      MemSizeMax;         //The maximum amount of memory to allow during BAR assignemnt.
    UINT16      IoBase;             //The base address of the region where IO BARs are assigned when configuring the VGA controller
    UINT16      IoSizeMax;          //The maximum amount of IO to allow during BAR assignment.
    FONT_MAP_INFO *FontMap;         //Pointer to a buffer of font map information structures that define the font map(s) to load.
    UINT8       FontMapCount;       //Count of entries in the preceeding buffer.
// AptioV Server Override Start: Add PEI Services.
    EFI_PEI_SERVICES **PeiServices;
// AptioV Server Override Stop
} VIDEO_PARAMETERS;


#define PCIE_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

/// X86 IO base address
typedef union {
  struct {                ///< MMIO address control bitfields
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 :2;            ///< Reserved
    UINT32 VE:1;          ///< VE
    UINT32 IE:1;          ///< IE
    UINT32 :6;            ///< Reserved
    UINT32 IOBase:13;     ///< IO Base
    UINT32 :7;            ///< Reserved
  } Field;
  UINT32  Value;
} X86IO_BASE_ADDR;

/// X86 IO limit address
typedef union {
  struct {                ///< MMIO address control bitfields
    UINT32 DstFabricID:8; ///< Destination FabricID of the IOS
    UINT32 :4;            ///< Reserved
    UINT32 IOLimit:13;    ///< IO Limit
    UINT32 :7;            ///< Reserved
  } Field;
  UINT32  Value;
} X86IO_LIMIT_ADDR;

#define MMIO_BASE_ADDRESS_REG_0     0x200      // MMIO base address register
#define MMIO_LIMIT_ADDRESS_REG_0    0x204
#define MMIO_CONTROL_ADDRESS_REG_0  0x208
#define X86IO_BASE_ADDRESS_REG0     0xC0       // IO base address register
#define X86IO_LIMIT_ADDRESS_REG0    0xC4       // IO limit address register
#define X86IO_LIMIT                 0x2000000  // IO Limit
#define X86_LEGACY_IO_SIZE          0x1000     // IO size which is reserved for legacy devices

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

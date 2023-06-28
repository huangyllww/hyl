/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for ZEN ZP
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#ifndef _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_
#define _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_

#include "Porting.h"
#include <Library/FabricResourceManagerLib.h>

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define ZEN_MMIO_MIN_SIZE           0x10000l
#define ZEN_NON_PCI_MMIO_ALIGN_MASK 0xFFFFul   // MMIO for non-PCI devices should be 16 bit aligned
#define MMIO_SPACE_FUNC             0          // DF Function number of MMIO Space
#define MMIO_BASE_ADDRESS_REG_0     0x200      // MMIO base address register
#define MMIO_LIMIT_ADDRESS_REG_0    0x204      // MMIO limit address register
#define MMIO_ADDRESS_CONTROL_REG_0  0x208      // MMIO control register
#define MMIO_MIN_NON_PCI_SIZE       0x500000   // 5MB is the minimum size of NonPci MMIO pool
#define X86IO_BASE_ADDRESS_REG0     0xC0       // IO base address register
#define X86IO_LIMIT_ADDRESS_REG0    0xC4       // IO limit address register
#define X86IO_LIMIT                 0x10000    // IO Limit
#define X86_LEGACY_IO_SIZE          0x1000     // IO size which is reserved for legacy devices

#define ZEN_MAX_SOCKET              2
#define ZEN_MAX_DIE_PER_SOCKET      4
#define ZEN_MAX_SYSTEM_DIE_COUNT    (ZEN_MAX_SOCKET * ZEN_MAX_DIE_PER_SOCKET)

#define SIZE_16M_ALIGN              0xFFFFFFFFFF000000l

#define BOTTOM_OF_COMPAT            0xFEC00000ul // From BOTTOM_OF_COMPAT to 4G would be leaved as undescribed (COMPAT)

#define ADDITIONAL_POSTED_REGION_UNDER_DIE0_START 0xFED00000l
#define ADDITIONAL_POSTED_REGION_UNDER_DIE0_END   0xFED0FFFFl

#define FABRIC_ID_SOCKET_DIE_MASK   0xE0

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO address control
typedef union {
  struct {                ///< MMIO address control bitfields
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 CpuDis:1;      ///< CPU Disable
    UINT32 :1;            ///< Reserved
    UINT32 DstFabricID:8; ///< Destination FabricID of the IOS
    UINT32 NP:1;          ///< Non-Posted
    UINT32 :19;           ///< Reserved
  } Field;
  UINT32  Value;
} MMIO_ADDR_CTRL;

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

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
FabricZenZpInitMmioBaseOnNvVariable (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *MmioSizeForEachDie,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricZenZpInitMmioEqually (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket
);

EFI_STATUS
FabricZenZpInitIoBaseOnNvVariable (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *IoSizeForEachDie,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricZenZpInitIoEqually (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket
  );

VOID
FabricZenZpAdditionalMmioSetting (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                DieNumberPerSocket,
  IN       UINT64               BottomOfCompat,
  IN       BOOLEAN              ReservedRegionAlreadySet
  );

VOID
FabricZenZpSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  );

VOID
FabricZenZpSetIoReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  DiePerSocket,
  IN       UINT8  RegIndex,
  IN       UINT8  DstFabricIDSysOffset,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  );

#pragma pack (pop)
#endif // _FABRIC_MMIO_MAP_MANAGER_ZEN_ZP_LIB_H_




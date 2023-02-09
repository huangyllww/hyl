/* $NoKeywords:$ */
/**
 * @file
 *
 * Low-level Fabric MMIO map manager Services library
 *
 * Contains interface to the family specific fabric MMIO map manager library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _FABRIC_RESOURCE_MANAGER_LIB_H_
#define _FABRIC_RESOURCE_MANAGER_LIB_H_

#include "Porting.h"
#include "AMD.h"
#include <Uefi/UefiBaseType.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO Region
typedef struct _FABRIC_MMIO_REGION {
  UINT64  BaseNonPci;            ///< Base address of non-discoverable devices
  UINT64  SizeNonPci;            ///< Total size of non-discoverable devices
  UINT64  UsedSizeNonPci;        ///< Already used size of non-discoverable devices
  UINT64  BaseNonPrefetch;       ///< Base address of non prefetchable
  UINT64  SizeNonPrefetch;       ///< Total size of non prefetchable
  UINT64  UsedSizeNonPrefetch;   ///< Already used size of non prefetchable
  UINT64  BasePrefetch;          ///< Base address of prefetchable
  UINT64  SizePrefetch;          ///< Total size of prefetchable
  UINT64  UsedSizePrefetch;      ///< Already used size of prefetchable
  UINT64  AlignNonPrefetch;      ///< Alignment bit map. For example, 0xFFFFF means 1MB alignment
  UINT64  AlignPrefetch;         ///< Alignment bit. For example, 0xFFFFF means 1MB alignment
} FABRIC_MMIO_REGION;

/// MMIO Manager
typedef struct _FABRIC_MMIO_MANAGER {
  FABRIC_MMIO_REGION MmioRegionAbove4G[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET];     ///< MMIO which is above 4G for Socket[0~1], Die[0~3]
  FABRIC_MMIO_REGION MmioRegionBelow4G[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET];     ///< MMIO which is below 4G for Socket[0~1], Die[0~3]
  BOOLEAN            AllocateMmioAbove4GOnThisDie[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET]; ///< Allocate MMIO which is above 4G on this DIE?
  BOOLEAN            AllocateMmioBelow4GOnThisDie[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET]; ///< Allocate MMIO which is below 4G on this DIE?
  BOOLEAN            Die0Has2ndMmioBelow4G;                                         ///< Indicate if Die 0 has a 2nd MMIO which is below 4G
  UINT8              Die02ndMmioPairBelow4G;                                        ///< Since AllocateMmioOnThisDie[i][j] is FALSE, use this one for DIE0's 2nd MMIO, Die02ndMmioPair = (i << 4) | j
} FABRIC_MMIO_MANAGER;

/// IO Region
typedef struct _FABRIC_IO_REGION {
  UINT32  IoBase;                ///< IO base address
  UINT32  IoSize;                ///< IO size
  UINT32  IoUsed;                ///< IO used
} FABRIC_IO_REGION;

/// IO Manager
typedef struct _FABRIC_IO_MANAGER {
  BOOLEAN          GlobalCtrl;                                                      ///< FALSE - forbid FabricAllocateIo
  FABRIC_IO_REGION IoRegion[MAX_SOCKETS_SUPPORTED][MAX_DIES_PER_SOCKET];            ///< IO ragion for Socket[0~1], Die[0~3]
} FABRIC_IO_MANAGER;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

EFI_STATUS
FabricAllocateMmio (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       UINT64                 Alignment,
  IN       FABRIC_TARGET          Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  );

EFI_STATUS
FabricAllocateIo (
  IN OUT   UINT32 *BaseAddress,
  IN OUT   UINT32 *Length,
  IN       FABRIC_TARGET          Target
  );

EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE    *ResourceSizeForEachDie  ///< Get available DF resource size for each Die
  );

EFI_STATUS
FabricReallocateResourceForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE    *ResourceSizeForEachDie, ///< User request
  IN       FABRIC_ADDR_SPACE_SIZE          *SpaceStatus             ///< Report current status
  );

EFI_STATUS
FabricResourceRestoreDefault (
  );

EFI_STATUS
FabricEnableVgaMmio (
  IN       FABRIC_TARGET          Target
  );

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_LIB_H_



/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/
#ifndef _GNBNBINITV4LIB_H_
#define _GNBNBINITV4LIB_H_

#pragma pack (push, 1)

/// Firmware header
typedef struct {
  UINT32  Version;                  ///< Version
  UINT32  HeaderLength;             ///< Header length
  UINT32  FirmwareLength;           ///< Firmware length
  UINT32  EntryPoint;               ///< Entry point
  UINT32  MessageDigest[5];         ///< Message digest
  UINT32  Reserved_A[3];            ///< Reserved
  UINT32  CurrentSystemState;       ///< Current system state
  UINT32  DpmCacHistory;            ///< DpmCac History
  UINT32  DpmResidencyCounters;     ///< DPM recidency counters
  UINT32  Reserved_B[16];           ///< Reserved
  UINT32  Reserved_C[16];           ///< Reserved
  UINT32  Reserved_D[16];           ///< Reserved
  UINT32  HeaderEnd;                ///< Header end signature
} FIRMWARE_HEADER_V4;

/// SMU service request contect
typedef struct {
  PCI_ADDR          GnbPciAddress;   ///< PCIe address of GNB
  UINT32            RequestId;       ///< Request/Msg ID
} SMU_MSG_CONTEXT;

#pragma pack (pop)

BOOLEAN
GnbCheckPhantomFuncSupport (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbGetTopologyInfoV4 (
  IN       PCI_ADDR                  StartPciAddress,
  IN       PCI_ADDR                  EndPciAddress,
     OUT   GNB_TOPOLOGY_INFO         *TopologyInfo,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

VOID
GnbSmuServiceRequestV4 (
  IN       PCI_ADDR                 GnbPciAddress,
  IN       UINT32                   RequestId,
  IN       UINT32                   AccessFlags,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

VOID
GnbSmuServiceRequestV4S3Script (
  IN      AMD_CONFIG_PARAMS     *StdHeader,
  IN      UINT16                ContextLength,
  IN      VOID                  *Context
  );

AGESA_STATUS
GnbSmuFirmwareLoadV4 (
  IN       PCI_ADDR                 GnbPciAddress,
  IN       FIRMWARE_HEADER_V4       *Firmware,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

PCI_ADDR
GnbGetIommuPciAddressV4 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbEnableIommuMmioV4 (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif

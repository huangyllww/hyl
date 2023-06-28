/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to calculate PCIe topology segment maximum exit latency
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIEASPMEXITLATENCYLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo;
  PCI_ADDR                DownstreamPort;
  UINT8                   LinkCount;
} PCIE_EXIT_LATENCY_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

SCAN_STATUS
PcieAspmGetMaxExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Determine ASPM L-state maximum exit latency for PCIe segment
 *
 *  Scan through all link in segment to determine maxim exit latency requirement by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[out]  AspmLatencyInfo   Latency info
 * @param[in]   StdHeader         Standard configuration header
 *
 */

VOID
PcieAspmGetMaxExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_EXIT_LATENCY_DATA  PcieExitLatencyData;
  PcieExitLatencyData.AspmLatencyInfo = AspmLatencyInfo;
  PcieExitLatencyData.ScanData.StdHeader = StdHeader;
  PcieExitLatencyData.LinkCount = 0;
  PcieExitLatencyData.ScanData.GnbScanCallback = PcieAspmGetMaxExitLatencyCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieExitLatencyData.ScanData);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get maximum exit latency for device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieAspmGetMaxExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_EXIT_LATENCY_DATA  *PcieExitLatencyData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   L1AcceptableLatency;

  PcieExitLatencyData = (PCIE_EXIT_LATENCY_DATA*) ScanData;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmGetMaxExitLatencyCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieExitLatencyData->DownstreamPort = Device;
    PcieExitLatencyData->LinkCount++;
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    PcieExitLatencyData->LinkCount--;
    break;
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
    ASSERT (PcieCapPtr != 0);
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    if ((Value & PCIE_ASPM_L1_SUPPORT_CAP) != 0) {
      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
        AccessWidth32,
        &Value,
        ScanData->StdHeader
        );
      L1AcceptableLatency = (UINT8) (1 << ((Value >> 9) & 0x7));
      if (PcieExitLatencyData->LinkCount > 1) {
        L1AcceptableLatency = L1AcceptableLatency + PcieExitLatencyData->LinkCount;
      }
      if (PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency <  L1AcceptableLatency) {
        PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency = L1AcceptableLatency;
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "  Device max exit latency  L1 - %d us\n",
        L1AcceptableLatency
        );
    }
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}


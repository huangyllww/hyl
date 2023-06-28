/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO Ancillary Data handlers
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
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV1.h>

#define FILECODE LIBRARY_DXIOLIBV1_DXIOANCDATAV1_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Function to calculate the ancillary data size for a PCIe/SATA engine
 *
 *
 * @param[in]  PortPointer   Pointer to port descriptor
 */
UINT32
DxioAncDataCalcSize (
  IN       DXIO_PORT_DESCRIPTOR    *PortPointer
  )
{
  DXIO_PHY_PARAM    *PhyParam;
  UINT32            DataSize;
  UINT32            LaneCount;

  PhyParam = &PortPointer->Phy.PhyParam[0];

  DataSize = 0;
  while (PhyParam->ParamType != 0) {
    switch (PhyParam->ParamType) {
      case   rx_vref_ctrl:
      case   SSC_OFF_FRUG1:
      case   SSC_OFF_PHUG1:
      case   GEN1_txX_iboost_lvl:
      case   GEN1_txX_vboost_en:
      case   GEN1_Unsupported1:
      case   GEN1_Unsupported2:
      case   GEN1_rxX_eq_att_lvl:
      case   GEN1_rxX_eq_vga1_gain:
      case   GEN1_rxX_eq_vga2_gain:
      case   GEN1_rxX_eq_dfe_tap1:
      case   GEN1_tx_vboost_lvl:
      case   GEN1_Unsupported3:
      case   GEN2_txX_iboost_lvl:
      case   GEN2_txX_vboost_en:
      case   GEN2_Unsupported1:
      case   GEN2_Unsupported2:
      case   GEN2_rxX_eq_att_lvl:
      case   GEN2_rxX_eq_vga1_gain:
      case   GEN2_rxX_eq_vga2_gain:
      case   GEN2_rxX_eq_dfe_tap1:
      case   GEN2_tx_vboost_lvl:
      case   GEN2_Unsupported3:
      case   GEN3_txX_iboost_lvl:
      case   GEN3_txX_vboost_en:
      case   GEN3_rxX_adapt_afe_en:
      case   GEN3_rxX_adapt_dfe_en:
      case   GEN3_rxX_eq_att_lvl:
      case   GEN3_rxX_eq_vga1_gain:
      case   GEN3_rxX_eq_vga2_gain:
      case   GEN3_rxX_eq_dfe_tap1:
      case   GEN3_tx_vboost_lvl:
      case   GEN3_OtherSide_txX_Preset:
      case   GEN4_txX_iboost_lvl:
      case   GEN4_txX_vboost_en:
      case   GEN4_rxX_adapt_afe_en:
      case   GEN4_rxX_adapt_dfe_en:
      case   GEN4_rxX_eq_att_lvl:
      case   GEN4_rxX_eq_vga1_gain:
      case   GEN4_rxX_eq_vga2_gain:
      case   GEN4_rxX_eq_dfe_tap1:
      case   GEN4_tx_vboost_lvl:
      case   GEN4_OtherSide_txX_Preset:
        // For a single-entry parameter
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        break;
      case   txX_term_cntl:
      case   rxX_term_cntl: 
      case   rxX_los_threshold:
      case   GEN1_txX_eq_pre:
      case   GEN1_txX_eq_main:
      case   GEN1_txX_eq_post:
      case   GEN1_rxX_eq_ctle_pole:
      case   GEN1_rxX_eq_ctle_boost:
      case   GEN2_txX_eq_pre:
      case   GEN2_txX_eq_main:
      case   GEN2_txX_eq_post:
      case   GEN2_rxX_eq_ctle_pole:
      case   GEN2_rxX_eq_ctle_boost:
      case   GEN3_txX_eq_pre:
      case   GEN3_txX_eq_main:
      case   GEN3_txX_eq_post:
      case   GEN3_rxX_eq_ctle_pole:
      case   GEN3_rxX_eq_ctle_boost:
      case   GEN4_txX_eq_pre:
      case   GEN4_txX_eq_main:
      case   GEN4_txX_eq_post:
      case   GEN4_rxX_eq_ctle_pole:
      case   GEN4_rxX_eq_ctle_boost:
        // For a per-lane parameter
        if (PortPointer->EngineData.StartLane > PortPointer->EngineData.EndLane) {
          LaneCount = (PortPointer->EngineData.StartLane - PortPointer->EngineData.EndLane) + 1;
        } else {
          LaneCount = (PortPointer->EngineData.EndLane - PortPointer->EngineData.StartLane) + 1;
        }
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        break;

      default:
        ASSERT (FALSE);
        break;
    }
    PhyParam++;
  }
  if (DataSize != 0) {
    DataSize += sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
  }
  return DataSize;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Function to populate the ancillary data for a PCIe/SATA engine
 *
 *
 * @param[in]  EnginePointer   Pointer to engine descriptor
 * @param[in]  PortPointer     Pointer to engine topology
 * @param[in]  AncData         Pointer to next available location in ancillary data
 */
UINT32
DxioAncDataPopulate (
  IN       DXIO_ENGINE_DESCRIPTOR  *EnginePointer,
  IN       DXIO_PORT_DESCRIPTOR    *PortPointer,
  IN       UINT8                   *AncData
  )
{
  DXIO_PHY_PARAM                *PhyParam;
  UINT32                        DataSize;
  UINT32                        Index;
  UINT32                        StartLane;
  UINT32                        EndLane;
  UINT32                        LaneCount;
  DXIO_OVRSSET_ENTRY            *AncDataEntry;
  DXIO_ANCILLARYDATA_SUBHEAP    *AncDataHeader;

  AncDataEntry = (DXIO_OVRSSET_ENTRY *) (AncData + sizeof (DXIO_ANCILLARYDATA_SUBHEAP));
  PhyParam = &PortPointer->Phy.PhyParam[0];

  DataSize = 0;
  StartLane = EnginePointer->StartLane;
  EndLane = EnginePointer->EndLane;
  if (StartLane > EndLane) {
    //Handle the port reversal case
    StartLane = EnginePointer->EndLane;
    EndLane = EnginePointer->StartLane;
  }
  LaneCount = (EndLane - StartLane) + 1;
  while (PhyParam->ParamType != 0) {
    switch (PhyParam->ParamType) {
      case   SSC_OFF_FRUG1:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_SSC_OFF_FRUG1;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   SSC_OFF_PHUG1:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_SSC_OFF_PHUG1;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   rx_vref_ctrl:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rx_vref_ctrl;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;
        
      case   txX_term_cntl:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_TERM_CNTL;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;
        
      case   rxX_term_cntl:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_TERM_CNTL;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;
        
      case   GEN1_txX_iboost_lvl:
      case   GEN2_txX_iboost_lvl:
      case   GEN3_txX_iboost_lvl:
      case   GEN4_txX_iboost_lvl:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_iboost_lvl;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_txX_vboost_en:
      case   GEN2_txX_vboost_en:
      case   GEN3_txX_vboost_en:
      case   GEN4_txX_vboost_en:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_vboost_en;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_rxX_eq_att_lvl:
      case   GEN2_rxX_eq_att_lvl:
      case   GEN3_rxX_eq_att_lvl:
      case   GEN4_rxX_eq_att_lvl:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_att_lvl;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_rxX_eq_vga1_gain:
      case   GEN2_rxX_eq_vga1_gain:
      case   GEN3_rxX_eq_vga1_gain:
      case   GEN4_rxX_eq_vga1_gain:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_vga1_gain;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_rxX_eq_vga2_gain:
      case   GEN2_rxX_eq_vga2_gain:
      case   GEN3_rxX_eq_vga2_gain:
      case   GEN4_rxX_eq_vga2_gain:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_vga2_gain;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_rxX_eq_dfe_tap1:
      case   GEN2_rxX_eq_dfe_tap1:
      case   GEN3_rxX_eq_dfe_tap1:
      case   GEN4_rxX_eq_dfe_tap1:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_dfe_tap1;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN1_tx_vboost_lvl:
      case   GEN2_tx_vboost_lvl:
      case   GEN3_tx_vboost_lvl:
      case   GEN4_tx_vboost_lvl:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_tx_vboost_lvl;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN3_rxX_adapt_afe_en:
      case   GEN4_rxX_adapt_afe_en:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_adapt_afe_en;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN3_rxX_adapt_dfe_en:
      case   GEN4_rxX_adapt_dfe_en:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_adapt_dfe_en;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;

      case   GEN3_OtherSide_txX_Preset:
      case   GEN4_OtherSide_txX_Preset:
        DataSize += sizeof (DXIO_OVRSSET_ENTRY);
        AncDataEntry->overrideField = DXIO_OVRSSET_FLD_OtherSide_txX_Preset;
        AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
        AncDataEntry->laneNumber = StartLane;
        AncDataEntry->value = PhyParam->ParamValue;
        AncDataEntry++;
        break;
        
      // For a per-lane parameter
      case   rxX_los_threshold:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          DataSize += sizeof (DXIO_OVRSSET_ENTRY);
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_los_threshold;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;

      case   GEN1_txX_eq_pre:
      case   GEN2_txX_eq_pre:
      case   GEN3_txX_eq_pre:
      case   GEN4_txX_eq_pre:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_eq_pre;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;

      case   GEN1_txX_eq_main:
      case   GEN2_txX_eq_main:
      case   GEN3_txX_eq_main:
      case   GEN4_txX_eq_main:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_eq_main;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;

      case   GEN1_txX_eq_post:
      case   GEN2_txX_eq_post:
      case   GEN3_txX_eq_post:
      case   GEN4_txX_eq_post:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_txX_eq_post;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;
      case   GEN1_rxX_eq_ctle_pole:
      case   GEN2_rxX_eq_ctle_pole:
      case   GEN3_rxX_eq_ctle_pole:
      case   GEN4_rxX_eq_ctle_pole:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_ctle_pole;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;

      case   GEN1_rxX_eq_ctle_boost:
      case   GEN2_rxX_eq_ctle_boost:
      case   GEN3_rxX_eq_ctle_boost:
      case   GEN4_rxX_eq_ctle_boost:
        DataSize += LaneCount * sizeof (DXIO_OVRSSET_ENTRY);
        for (Index = StartLane; Index <= EndLane; Index++) {
          AncDataEntry->overrideField = DXIO_OVRSSET_FLD_rxX_eq_ctle_boost;
          AncDataEntry->fieldGen = (UINT8)(PhyParam->ParamType >> 6);
          AncDataEntry->laneNumber = Index;
          AncDataEntry->value = PhyParam->ParamValue;
          AncDataEntry++;
        }
        break;

      default:
        ASSERT (FALSE);
        break;
    }
    PhyParam++;
  }
  if (DataSize != 0) {
    DataSize += sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
    AncDataHeader = (DXIO_ANCILLARYDATA_SUBHEAP *) AncData;
    AncDataHeader->descriptorType = DXIO_ANCDATA_SUBHEAPTYPE_OVRRSET;
    AncDataHeader->version = DXIO_ANCILLARYDATA_SUBHEAP_VERSION;
    AncDataHeader->numDWORDs = DataSize >> 2;
  }
  return DataSize;
}


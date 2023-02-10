/* $NoKeywords:$ */
/**
 * @file
 *
 * DxioInitZP - Initialize DXIO subsystem
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioBaseZPPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
 ******************************************************************************
 */
#include <AMD.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersZP.h>
#include <cpuRegisters.h>
//From AgesaPkg
#include <AmdPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Ppi/AmdMemoryInfoHobPpi.h>


#include "AmdNbioPcieZPPei.h"

#include <Guid/GnbPcieInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioPcieTrainingPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/IoLib.h>
//Dependent on AgesaPkg
#include <Library/DxioLibV1.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbLib.h>
#include <CcxRegistersZp.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Ppi/EqTablePpi.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseCoreLogicalIdLib.h>

#define FILECODE        NBIO_PCIE_AMDNBIOPCIEZPPEI_DXIOINITZP_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
#define NBIF1RCCFG_COMMAND_ADDRESS                              0x10200004UL
#define NBIF1RCCFG_SUB_BUS_ADDRESS                              0x10200018UL
#define NBIF1RCCFG_BASELIMIT_ADDRESS                            0x10200020UL
#define NBIF1EPF3CFG_VENDOR_ID_ADDRESS                          0x10243000UL
#define NBIF1EPF3CFG_COMMAND_ADDRESS                            0x10243004UL
#define NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS                        0x10243010UL
#define NBIF1EPF3CFG_ADAPTER_ID_ADDRESS                         0x1024304CUL

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Comment this line to enable extended debug output if tracing is enabled
#undef GNB_TRACE_ENABLE

#define BRIDGE_DEVICE_TYPE        6
#define DISPLAY_CONTROLLER_TYPE   3

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

UINT32    StrapList[8] = {0xDB, 0x12C, 0x17D, 0x1Ce, 0x21F, 0x270, 0x2C1, 0x312};
UINT16    EnPcie4[] = {0x353,0x354,0x356,0x357};   //enable 10bit tag 
                                                   //  0x353 enable completer_support
                                                   //  0x354 enable requester support
                                                   //  0x356 enable completer_support
                                                   //  0x357 enable requester support
 
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC PEI_AMD_NBIO_PCIE_TRAINING_START_PPI mNbioPcieTrainingStartPpi = {
  AMD_TRAINING_START_STATUS          /// Start Status
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioPcieTrainingStartPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieTrainingStartPpiGuid,
  &mNbioPcieTrainingStartPpi
};

STATIC PEI_AMD_NBIO_PCIE_TRAINING_DONE_PPI mNbioPcieTrainingDonePpi = {
  AMD_TRAINING_DONE_STATUS          /// Done Status
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioPcieTrainingDonePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieTrainingDonePpiGuid,
  &mNbioPcieTrainingDonePpi
};

#define DxioInputParsetGetNextDescriptor(Descriptor) (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)


/*----------------------------------------------------------------------------------------*/
/**
 * Get PCIe Core Preset setting 
 * @param[in]     Die       die number
 * @param[in]     Core      PCIe Core number 
 */
UINT8 
GetPcieCorePresetPcd(
    UINT8 Die,
    UINT8 Core
)
{
    switch(Die){
        case 0:
            if(Core==0){
                return PcdGet8(PcdCfgDie0Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie0Pcie1Preset);
            }
            break;
        case 1:
            if(Core==0){
                return PcdGet8(PcdCfgDie1Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie1Pcie1Preset);
            }
            break;
        case 2:
            if(Core==0){
                return PcdGet8(PcdCfgDie2Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie2Pcie1Preset);
            }
            break;
        case 3:
            if(Core==0){
                return PcdGet8(PcdCfgDie3Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie3Pcie1Preset);
            }
            break;
        case 4:
            if(Core==0){
                return PcdGet8(PcdCfgDie4Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie4Pcie1Preset);
            }
            break;
        case 5:
            if(Core==0){
                return PcdGet8(PcdCfgDie5Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie5Pcie1Preset);
            }
            break;
        case 6:
            if(Core==0){
                return PcdGet8(PcdCfgDie6Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie6Pcie1Preset);
            }
            break;
        case 7:
            if(Core==0){
                return PcdGet8(PcdCfgDie7Pcie0Preset);
            }else{
                return PcdGet8(PcdCfgDie7Pcie1Preset);
            }
            break;
    }
    return 0xff;
}
/**
 * Clean up bridge
 *
 *
 *
 * @param[in]     Endpoint       PCI Address of the bridge
 */
VOID
DxioCleanUpEarlyBridge (
  IN       PCI_ADDR              Endpoint
  )
{
  UINT32      Index;
  UINT32      Value;
  PCI_ADDR    Downstream;

  GnbLibPciRead (Endpoint.AddressValue | 0x08, AccessWidth32, &Value, NULL);
  if ((Value >> 24) == DISPLAY_CONTROLLER_TYPE) {
    Value = 0;
    GnbLibPciWrite (Endpoint.AddressValue | 0x04, AccessWidth32, &Value, NULL);
    for (Index = 0x10; Index < 0x28; Index += 4) {
      GnbLibPciWrite (Endpoint.AddressValue | Index, AccessWidth32, &Value, NULL);
    }
  } else if ((Value >> 24) == BRIDGE_DEVICE_TYPE) {
    GnbLibPciRead (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
    if ((Value & 0x00FFFF00) != 0) {
      Downstream.AddressValue = MAKE_SBDFO (0, (Value & 0x0000FF00) >> 8, 0, 0, 0);
      DxioCleanUpEarlyBridge (Downstream);
      Value = 0;
      GnbLibPciWrite (Endpoint.AddressValue | 0x1C, AccessWidth16, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x20, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x24, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x28, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x2C, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x30, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x04, AccessWidth32, &Value, NULL);
      GnbLibPciRead (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
      Value &= 0xFF0000FF;
      GnbLibPciWrite (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clean up programming from early BMC initialization
 *
 *
 *
 * @param[in]     GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]     DieNumber      InstanceId of the Die that contains the BMC link
 */
VOID
DxioCleanUpEarlyInit (
  IN       GNB_HANDLE            *GnbHandle,
  IN       UINT32                DieNumber
  )
{
  GNB_HANDLE            *NbioHandle;
  PCIe_ENGINE_CONFIG    *Engine;
  UINT32                Value;
  PCI_ADDR              Endpoint;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCleanUpEarlyInit Enter\n");
  NbioHandle = GnbHandle;
  IDS_HDT_CONSOLE (GNB_TRACE, "Looking for Die Number %d\n", DieNumber);
  while (NbioHandle != NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found Die Number %d\n", NbioHandle->InstanceId);
    if (NbioHandle->InstanceId == DieNumber) {
      Engine = PcieConfigGetChildEngine (NbioHandle);
      while (Engine != NULL) {
        GnbLibPciRead (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
        IDS_HDT_CONSOLE (GNB_TRACE, "  Read 0x%x from Device %d Function %d\n", Value, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber);
        if ((Value & 0x00FFFF00) != 0) {
          Endpoint.AddressValue = MAKE_SBDFO (0, (Value & 0x0000FF00) >> 8, 0, 0, 0);
          DxioCleanUpEarlyBridge(Endpoint);
          Value = 0;
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x1C), AccessWidth16, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x20), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x24), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x28), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x2C), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x30), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x04), AccessWidth32, &Value, NULL);
          GnbLibPciRead (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
          Value &= 0xFF0000FF;
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
        }
        Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_GNB);
      }
    }
    NbioHandle = GnbGetNextHandle(NbioHandle);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCleanUpEarlyInit Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enumerate all Pcie connectors for register setting.
 *
 *
 * @param[in]       GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]       Engine         Pointer to engine config descriptor
 * @param[in, out]  Buffer         Not used
 */
VOID
STATIC
PcieIntegratedInterfaceCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT32                            Address;
  UINT8                             MappingPortID;

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieIntegratedInterfaceCallback Enter\n"));

  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;

  Address = (Wrapper->WrapId == 0) ? GPP0_TX_CNTL_ADDRESS : GFX0_TX_CNTL_ADDRESS;
  Address += (MappingPortID * 0x1000);
  NbioRegisterRMW (
    GnbHandle,
    TYPE_SMN,
    Address,
    (UINT32) ~(PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_MASK),
    0 << PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_OFFSET,
    0
    );

  if ((Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen3) ||
     (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGenMaxSupported)) {
    // Gen3 cap enabled
    Engine->Type.Port.LaneEqualizationCntl.DsTxPreset = 7;
    Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint = 2;
    Engine->Type.Port.LaneEqualizationCntl.UsTxPreset = 7;
    Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint = 2;
  }

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieIntegratedInterfaceCallback Exit\n"));

}

/*----------------------------------------------------------------------------------------*/
/**
 * After Pcie Training Enumerate all Pcie connectors for register setting.
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
PcieAfterTrainingDoneCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT8                             MappingPortID;
  UINT32                            Address;
  UINT32                            Value;

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieAfterTrainingDoneCallback Enter\n"));

  Address = Engine->Type.Port.Address.AddressValue;
  if (Address != 0) {
    // Set completion timeout
    GnbLibPciRead (Address | 0x80, AccessWidth32, &Value, NULL);
    Value = (Value & 0xfffffff0) | 0x6;
    GnbLibPciWrite (Address | 0x80, AccessWidth32, &Value, NULL);

    // Set slot_implemented
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x58_ADDRESS,
      AccessWidth32,
      0xffffffff,
      1 << DxF0x58_SlotImplemented_OFFSET,
      (AMD_CONFIG_PARAMS *) NULL
      );
  }

  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;

//  //
//  // L1ImmediateACK
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of L1ImmediateACK  = 0x%x\n",Engine->Type.Port.L1ImmediateACK);
//  Address = (Wrapper->WrapId == 0) ? SMN_0x11140280_ADDRESS : SMN_0x11240280_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address L1ImmediateACK  = 0x%x\n",Address);
//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(SMN_0x11140280_LC_L1_IMMEDIATE_ACK_MASK),
//    (Engine->Type.Port.L1ImmediateACK << SMN_0x11140280_LC_L1_IMMEDIATE_ACK_OFFSET),
//    0
//    );

//  //
//  // EqSearchMode (00) Gradient Search  - Not support on ZP, keep it as inactive
//  //              (01) Exhaustive Search
//  //              (02) Exhaustive Search + 3x3 - Not support on ZP, keep it as inactive
//  //              (03) Preset Search
//  // DisGen3EQPhase
//  // BypassGen3EQ
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of EqSearchMode  = 0x%x\n",Engine->Type.Port.EqSearchMode);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of DisGen3EQPhase  = 0x%x\n",Engine->Type.Port.DisGen3EQPhase);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of EqSearchMode  = 0x%x\n",Engine->Type.Port.BypassGen3EQ);

//  Address = (Wrapper->WrapId == 0) ? SMN_0x111402d8_ADDRESS : SMN_0x112402d8_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address EqSearchMode  = 0x%x\n", Address);

//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(SMN_0x111402d8_LC_EQ_SEARCH_MODE_MASK |
//    SMN_0x111402d8_LC_BYPASS_EQ_MASK |
//    SMN_0x111402d8_LC_BYPASS_EQ_REQ_PHASE_MASK),
//    (Engine->Type.Port.EqSearchMode << SMN_0x111402d8_LC_EQ_SEARCH_MODE_OFFSET) |
//    (Engine->Type.Port.DisGen3EQPhase << SMN_0x111402d8_LC_BYPASS_EQ_OFFSET) |
//    (Engine->Type.Port.BypassGen3EQ << SMN_0x111402d8_LC_BYPASS_EQ_REQ_PHASE_OFFSET),
//    0
//    );

//  //
//  // EnableSafeSearch
//  // LcFomTime (00) 300us
//  //           (01) 200us
//  //           (02) 100us
//  //           (03) Less than 100us
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of LcFomTime  = 0x%x\n",Engine->Type.Port.LcFomTime);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of EnableSafeSearch  = 0x%x\n",Engine->Type.Port.EnableSafeSearch);

//  Address = (Wrapper->WrapId == 0) ? SMN_0x111402f0_ADDRESS : SMN_0x112402f0_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address of LcFomTime  = 0x%x\n", Address);

//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(SMN_0x111402f0_LC_FOM_TIME_MASK |
//    SMN_0x111402f0_LC_SAFE_EQ_SEARCH_MASK),
//    (Engine->Type.Port.LcFomTime << SMN_0x111402f0_LC_FOM_TIME_OFFSET) |
//    (Engine->Type.Port.EnableSafeSearch << SMN_0x111402f0_LC_SAFE_EQ_SEARCH_OFFSET),
//    0
//    );

//  //
//  // Enable3x3Search
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of Enable3x3Search  = 0x%x\n",Engine->Type.Port.Enable3x3Search);
//  Address = (Wrapper->WrapId == 0) ? SMN_0x111402e0_ADDRESS : SMN_0x112402e0_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address of Enable3x3Search  = 0x%x\n",Address);

//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(SMN_0x111402e0_LC_3X3_COEFF_SEARCH_EN_MASK),
//    (Engine->Type.Port.Enable3x3Search << SMN_0x111402e0_LC_3X3_COEFF_SEARCH_EN_OFFSET),
//    0
//    );

//  //
//  // (Implementation notes: due to known bug, we may not support 1SYM on ZP A0, please set 2SYM as default for all 3speeds until further confirmation)
//  // SpcMode.SpcGen1 - (REGS_LC_SPC_MODE_2P5GT = 1 as defalut)
//  // SpcMode.SpcGen2 - (REGS_LC_SPC_MODE_5GT = 1 as defalut)
//  // SpcMode.SpcGen3 - (REGS_LC_SPC_MODE_8GT = 1 as defalut)
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of SpcGen1  = 0x%x\n",Engine->Type.Port.SpcMode.SpcGen1);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of SpcGen2  = 0x%x\n",Engine->Type.Port.SpcMode.SpcGen2);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of SpcGen3  = 0x%x\n",Engine->Type.Port.SpcMode.SpcGen3);

//  Address = (Wrapper->WrapId == 0) ? SMN_0x111402ec_ADDRESS : SMN_0x112402ec_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(SMN_0x112402ec_LC_SPC_MODE_2P5GT_MASK |
//    SMN_0x112402ec_LC_SPC_MODE_5GT_MASK |
//    SMN_0x112402ec_LC_SPC_MODE_8GT_MASK),
//    (Engine->Type.Port.SpcMode.SpcGen1 << SMN_0x111402ec_LC_SPC_MODE_2P5GT_OFFSET) |
//    (Engine->Type.Port.SpcMode.SpcGen2 << SMN_0x111402ec_LC_SPC_MODE_5GT_OFFSET) |
//    (Engine->Type.Port.SpcMode.SpcGen3 << SMN_0x111402ec_LC_SPC_MODE_8GT_OFFSET),
//    0
//    );

//  //
//  // LaneEqualizationCntl.DsTxPreset     - (DOWNSTREAM_PORT_TX_PRESET = 7 as default for each lane if Gen3 cap enabled)
//  //                     .DsRxPresetHint - (DOWNSTREAM_PORT_RX_PRESET_HINT = 2 as default for each lane if Gen3 cap enabled)
//  //                     .UsTxPreset     - (UPSTREAM_PORT_TX_PRESET = 7 as default for each lane if Gen3 cap enabled)
//  //                     .UsRxPresetHint - (UPSTREAM_PORT_RX_PRESET_HINT = 2 as default for each lane if Gen3 cap enabled)
//  //

//  Address = (Wrapper->WrapId == 0) ? SMN_0x1110027C_ADDRESS : SMN_0x1120027C_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address of LaneEqualizationCntl  = 0x%x\n", Address);
//  if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen3) {
//    // Gen3 cap enabled
//    Engine->Type.Port.LaneEqualizationCntl.DsTxPreset = 7;
//    Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint = 2;
//    Engine->Type.Port.LaneEqualizationCntl.UsTxPreset = 7;
//    Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint = 2;
//  }
//  if ((Engine->Type.Port.LaneEqualizationCntl.DsTxPreset != 0) &&
//      (Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint != 0) &&
//      (Engine->Type.Port.LaneEqualizationCntl.UsTxPreset != 0) &&
//      (Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint != 0)) {
//    NbioRegisterRMW (
//      GnbHandle,
//      TYPE_SMN,
//      Address,
//      (UINT32)~(PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_MASK | PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_MASK |
//      PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_MASK | PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_MASK),
//      (Engine->Type.Port.LaneEqualizationCntl.DsTxPreset << PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_OFFSET) |
//      (Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint << PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_OFFSET) |
//      (Engine->Type.Port.LaneEqualizationCntl.UsTxPreset << PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_OFFSET) |
//      (Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint << PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_OFFSET),
//      0
//      );
//  }

//  //
//  // PortReversal (todo)
//  //
//  //
//  // TXDeEmphasis (RSMU  0x4A348)
//  //

//  //
//  // TXMargin
//  //

//  //
//  // ScramblerCntl
//  //

//  //
//  // DynamicLanesPowerState
//  //
//  IDS_HDT_CONSOLE (GNB_TRACE, "Value of DynamicLanesPowerState = 0x%x\n",Engine->Type.Port.DynamicLanesPowerState);
//  Address = (Wrapper->WrapId == 0) ? PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS : PCIE1_LC_LINK_WIDTH_CNTL_ADDRESS;
//  Address += (MappingPortID * 0x1000);
//  IDS_HDT_CONSOLE (GNB_TRACE, "Address of DynamicLanesPowerState= 0x%x\n",Address);
//  NbioRegisterRMW (
//    GnbHandle,
//    TYPE_SMN,
//    Address,
//    (UINT32)~(PCIE_LC_LINK_WIDTH_CNTL_LC_DYN_LANES_PWR_STATE_MASK),
//    (Engine->Type.Port.DynamicLanesPowerState << PCIE_LC_LINK_WIDTH_CNTL_LC_DYN_LANES_PWR_STATE_OFFSET),
//    0
//    );



  // Read current configuration
  Value = 0xFFFE0084;
  NbioRegisterWrite (GnbHandle,
                     TYPE_SMN,
                     (Wrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                     &Value,
                     0
                     );
  NbioRegisterRead (GnbHandle,
                    TYPE_SMN,
                    (Wrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                    &Value,
                    0
                    );
  // Check for 2x8 or 1x16 configuration
  if ((Value == 0) || (Value == 5)) {
    //
    // PowerOff Unused Phy Lanes and PLL L1
    //
    Address = (Wrapper->WrapId == 0) ? PCIE0_LC_CNTL6_ADDRESS : PCIE1_LC_CNTL6_ADDRESS;
    Address += (MappingPortID * 0x1000);
    IDS_HDT_CONSOLE (GNB_TRACE, "PowerOffPhyLanesPllL1 for Wrapper %d Port %d\n", Wrapper->WrapId, MappingPortID);
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      Address,
      ~((UINT32) PCIE_LC_CNTL6_LC_L1_POWERDOWN_MASK),
       (Engine->Type.Port.PowerOffPhyLanesPllL1 << PCIE_LC_CNTL6_LC_L1_POWERDOWN_OFFSET),
      0
      );
  }

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieAfterTrainingDoneCallback Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * After Pcie Training Enumerate all Pcie connectors for register setting.
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
InitBusRanges (
  IN     PCIe_PLATFORM_CONFIG                 *Pcie,
  IN     AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi

  )
{
  GNB_HANDLE                      *GnbHandle;
  GNB_HANDLE                      *InnerHandle;
  PCIE_VDM_NODE_CTRL4_STRUCT      VdmNode;
  UINTN                           SystemFabricID;
  UINTN                           BusNumberBase;
  UINTN                           BusNumberLimit;
  UINT32                          VdmNodeAddress;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InnerHandle = NbioGetHandle (Pcie);
    while (InnerHandle != NULL) {
      FabricTopologyServicesPpi->GetRootBridgeInfo (InnerHandle->SocketId,
                                            InnerHandle->DieNumber,
                                            InnerHandle->RBIndex,
                                            &SystemFabricID,
                                            &BusNumberBase,
                                            &BusNumberLimit
                                            );
      VdmNode.Field.BUS_RANGE_BASE = BusNumberBase;
      VdmNode.Field.BUS_RANGE_LIMIT = BusNumberLimit;
      if (InnerHandle->InstanceId == GnbHandle->InstanceId) {
        VdmNode.Field.NODE_PRESENT = 0;
      } else {
        VdmNode.Field.NODE_PRESENT = 1;
      }
      VdmNodeAddress = PCIE_VDM_NODE0_CTRL4_ADDRESS + (InnerHandle->InstanceId * 0x14);
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         VdmNodeAddress,
                         &VdmNode.Value,
                         0
                         );
      InnerHandle = GnbGetNextHandle (InnerHandle);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
DxioUserDescriptorConfigDump (
  IN      DXIO_PORT_DESCRIPTOR      *EngineDescriptor
  )
{

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %a\n",
    (EngineDescriptor->EngineData.EngineType == DxioPcieEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineData.EngineType == DxioSATAEngine) ? "SATA Port" : (
    (EngineDescriptor->EngineData.EngineType == DxioEthernetEngine) ? "Ethernet Port" : (
    (EngineDescriptor->EngineData.EngineType == DxioUnusedEngine) ? "Unused" : "Invalid")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->EngineData.StartLane,
    EngineDescriptor->EngineData.EndLane
  );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Hotplug - %d\n", EngineDescriptor->EngineData.HotPluggable);
  IDS_HDT_CONSOLE (PCIE_MISC, "    GpioGroupId - %d\n", EngineDescriptor->EngineData.GpioGroupId);
  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    DeviceNumber - %d\n    FunctionNumber - %d\n    LinkSpeedCapability - %d\n    LinkAspm - %d\n    LinkHotplug - %d\n    SB link - %d\n    MiscControls - 0x%02x\n" ,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.PortPresent,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.DeviceNumber,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.FunctionNumber,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkSpeedCapability,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkAspm,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkHotplug,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls.SbLink,
      ((DXIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls
    );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to debug out
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
DxioUserConfigConfigDump (
  IN      DXIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  DXIO_PORT_DESCRIPTOR      *EngineDescriptor;
  UINTN                     Index;
  DXIO_COMPLEX_DESCRIPTOR   *LocalDescriptor;

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config Start------------->\n");
  IDS_HDT_CONSOLE(PCIE_MISC, " BMC Link Location is %x\n", ComplexDescriptor->BmcLinkLocation);
  LocalDescriptor = ComplexDescriptor;
  Index = 0;
  while (LocalDescriptor != NULL) {
    IDS_HDT_CONSOLE (PCIE_MISC, "<---------------- Socket %d ----------------->\n", Index);
    EngineDescriptor = LocalDescriptor->PciePortList;
    while (EngineDescriptor != NULL) {
      DxioUserDescriptorConfigDump (EngineDescriptor);
      EngineDescriptor = DxioInputParsetGetNextDescriptor (EngineDescriptor);
    }
    LocalDescriptor = DxioInputParsetGetNextDescriptor (LocalDescriptor);
    Index++;
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config End-------------->\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Force all ComplexDescriptor entries to enable TurnOffUnusedLanes
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
PcieForceTurnOffUnusedLanes (
  IN      DXIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  DXIO_PORT_DESCRIPTOR      *EngineDescriptor;
  DXIO_COMPLEX_DESCRIPTOR   *LocalDescriptor;

  LocalDescriptor = ComplexDescriptor;
  while (LocalDescriptor != NULL) {
    EngineDescriptor = LocalDescriptor->PciePortList;
    while (EngineDescriptor != NULL) {
      EngineDescriptor->Port.MiscControls.TurnOffUnusedLanes = 1;
      IDS_HDT_CONSOLE (PCIE_MISC, "PcieForceTurnOffUnusedLanes(TurnOffUnusedLanes=%x)\n", EngineDescriptor->Port.MiscControls.TurnOffUnusedLanes);
      EngineDescriptor = DxioInputParsetGetNextDescriptor (EngineDescriptor);
    }
    LocalDescriptor = DxioInputParsetGetNextDescriptor (LocalDescriptor);
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting
 *
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
SataChannelTypeSetting (
  IN      DXIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  UINT8                   SataChannelType;
  DXIO_PORT_DESCRIPTOR    *EngineDescriptor;

  SataChannelType = PcdGet8 (PcdCfgSataPhyTuning);
  if (SataChannelType != 0) {
    while (ComplexDescriptor != NULL) {
      EngineDescriptor = ComplexDescriptor->PciePortList;
      while (EngineDescriptor != NULL) {
        if (EngineDescriptor->EngineData.EngineType == DxioSATAEngine) {
          EngineDescriptor->Port.MiscControls.ChannelType = SataChannelType;
          IDS_HDT_CONSOLE (GNB_TRACE, "SataChannelTypeSetting(ChannelType=%x)\n", SataChannelType);
        }
        EngineDescriptor = DxioInputParsetGetNextDescriptor (EngineDescriptor);
      }
      ComplexDescriptor = DxioInputParsetGetNextDescriptor (ComplexDescriptor);
    }
  }
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 *PcieCfgBeforeTraining
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval
 */
 /**----------------------------------------------------------------------------------------*/

VOID
PcieCfgBeforeTraining (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT8                MappingPortID;
  UINT32               Address;
  UINT8                CpuGen; 
  CPUID_DATA           CpuId;
  
  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieCfgBeforeTraining Enter\n"));
  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );   
  CpuGen = (UINT8)((CpuId.EAX_Reg&0xF0)>>4);
  
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
      PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
      while (PcieEngine != NULL) {
          if(CpuGen == CPU_HG3){ 
              MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
              Address = (PcieWrapper->WrapId == 0) ? PCIE0_PCIE_LC_CNTL2_ADDRESS : PCIE1_PCIE_LC_CNTL2_ADDRESS;
              Address += (MappingPortID * 0x1000);
              NbioRegisterRMW (
                  GnbHandle,
                  TYPE_SMN,
                  Address,
                  ~((UINT32)  PCIE_LC_CNTL2_LC_WAIT_FOR_LANES_IN_LW_NEG_MASK),
                  3 << PCIE_LC_CNTL2_LC_WAIT_FOR_LANES_IN_LW_NEG_OFFSET,
                  0
                  );
          }
          PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
      }
      PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieCfgBeforeTraining Exit\n"););
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /**----------------------------------------------------------------------------------------*/

VOID
PcieInterfaceInitSetting (
  IN       GNB_HANDLE       *GnbHandle,
  IN       SOC_LOGICAL_ID   LogicalCpuId
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               Index;
  UINT32               Value;
  //UINT32               Address;
  UINT8                 EnPcie4Index;
  //UINT32                linkcntl;
  UINT8   CpuGen;
  CPUID_DATA CpuId;
  
  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceInitSetting Enter\n"));
  
  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );   
  CpuGen = (UINT8)((CpuId.EAX_Reg&0xF0)>>4);
  
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      PcieIntegratedInterfaceCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    
    for (EnPcie4Index = 0; EnPcie4Index < 4; EnPcie4Index++) {
      Index = DXIO_STRAP_BIF_BASEADDRSS | EnPcie4[EnPcie4Index]; //enable completer_support
      NbioRegisterWrite (GnbHandle,
          TYPE_SMN,
          (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
              &Index,
              0
      );
      Value = 1;
      NbioRegisterWrite (GnbHandle,
          TYPE_SMN,
          (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
              &Value,
              0
      );
    }

    if (PcdGetBool (PcdCfgACSEnable)) {
    //enable AER
    //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_AER_EN_offset, 1);
    Index = DXIO_STRAP_BIF_BASEADDRSS | 0x02; //0xFFFE0002;
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                       &Index,
                       0
                       );
    Value = 1;
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                       &Value,
                       0
                       );


	  //enable ACS
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x68;//0xFFFE0068;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 1;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_SOURCE_VALIDATION_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_TRANSLATION_BLOCKING_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_DIRECT_TRANSLATED_P2P_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_P2P_COMPLETION_REDIRECT_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_P2P_REQUEST_REDIRECT_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_UPSTREAM_FORWARDING_offset, 1);
      //dxio_fw_mp1_pcie_strapBif_write( pcieMapData->pcieCoreId, dxio_fw_e_STRAP_BIF_ACS_EN_offset, 1);
      for (Index = 0xFFFE0069; Index <= 0xFFFE006E; Index++) {
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                           &Index,
                           0
                           );
        Value = 1;
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                           &Value,
                           0
                           );
      }
      //debug tx_preset rx_preset
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x88;//0xfffe0088;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 2;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x89;//0xfffe0089;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 2;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x8a;//0xfffe0090;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 7;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x8b;//0xfffe0091;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 7;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x30;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 1;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );	  
      //end
    }

    if((CpuGen == CPU_HG3) && (GetPcieCorePresetPcd(GnbHandle->InstanceId, PcieWrapper->WrapId) != 0xff)){
        Index = DXIO_STRAP_BIF_BASEADDRSS | 0x347;
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                           &Index,
                           0
                           );
        Value = GetPcieCorePresetPcd(GnbHandle->InstanceId,PcieWrapper->WrapId);
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                           &Value,
                           0
                           );
        Index = DXIO_STRAP_BIF_BASEADDRSS | 0x348;
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                           &Index,
                           0
                           );
        Value = GetPcieCorePresetPcd(GnbHandle->InstanceId,PcieWrapper->WrapId);
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                           &Value,
                           0
                           );
    }
    
    //set D3 PCIe Gen4 default Tx Preset to 5/5
    if((CpuGen == CPU_HG4)){
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x347;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                        (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 5;
      NbioRegisterWrite (GnbHandle,
                        TYPE_SMN,
                       (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                        &Value,
                         0
                         );
      Index = DXIO_STRAP_BIF_BASEADDRSS | 0x348;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                        (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                         &Index,
                         0
                         );
      Value = 5;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                        (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                         &Value,
                         0
                         );
      }

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     (PcieWrapper->WrapId == 0) ?  PCIE0_CNTL2_ADDRESS : PCIE1_CNTL2_ADDRESS,
                     ~((UINT32) PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_MASK),
                     1 << PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_OFFSET,
                     0
                     );
	//enable 10bit tag
	for (Index = 0; Index < 8; Index++) {
      NbioRegisterRMW(GnbHandle,
                     TYPE_SMN,
                     (PcieWrapper->WrapId == 0) ?  (PCIE0_DEVICE_CNTL2_ADDRESS | Index << 12): (PCIE1_DEVICE_CNTL2_ADDRESS | Index << 12),
                     ~((UINT32) DEVICE_CNTL2_10BIT_TAG_REQUESTER_EN_MASK),
                     1 << DEVICE_CNTL2_10BIT_TAG_REQUESTER_EN_OFFSET,
                     0
                     );
	}
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  if (PcdGetBool (PcdCfgACSEnable)) {
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF0_RCC_DEV0_PORT_STRAP0_ADDRESS,
                     ~((UINT32)(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK)),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF1_RCC_DEV0_PORT_STRAP0_ADDRESS,
                     ~((UINT32) (RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK)),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIF2_RCC_DEV0_PORT_STRAP0_ADDRESS,
                     ~((UINT32) (RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK)),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    for (Index = 0; Index < 8; Index++) {
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF0_RCC_DEV0_EPF0_STRAP2_ADDRESS | (Index * 0x200),
                       ~((UINT32) (RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK)),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF1_RCC_DEV0_EPF0_STRAP2_ADDRESS | (Index * 0x200),
                       ~((UINT32) (RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK)),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS | (Index * 0x200),
                       ~((UINT32) (RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK)),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
    }
  }

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceInitSetting Exit\n"););
}

/**----------------------------------------------------------------------------------------*/
/**
 * Get Downstream PCIe Device vendor ID and Device ID
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    Downstream vendor ID and Device ID
 */
 /**----------------------------------------------------------------------------------------*/
UINT32
GetTargetVidDid (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT32                            RpSubBusCfgAddress, TargetPciVidDidAddr;
    UINT32                            VidDid;
    UINT32                            OldValue, NewValue;
    UINT32                            MmCfg;
    UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
    UINT8                             MappingPortID;

    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;

    RpSubBusCfgAddress = (Wrapper->WrapId == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
    RpSubBusCfgAddress += (MappingPortID * 0x1000);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Get Target VidDid: ");
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIeCore%02x ", Wrapper->WrapId);
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIePort%02x\n", MappingPortID);
    //save old sub bus config
    NbioRegisterRead(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    //allocate bus for target
    PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
    SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    NewValue = 0;
    NewValue |= PrimaryBus;
    NewValue |= (SecondaryBus<<8);
    NewValue |= (SubBusNum<<16);
    NbioRegisterWrite(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &NewValue, 0);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Cfg RP: Address=%08x CfgValue=%x\n", RpSubBusCfgAddress, NewValue);
    //read target viddid
    MmCfg = (UINT32)(AsmReadMsr64(MSR_MMIO_Cfg_Base) & 0xfff00000);
    TargetPciVidDidAddr = PCIE_VIDDID_ADDR(MmCfg,SecondaryBus, 0, 0);
    VidDid = MmioRead32(TargetPciVidDidAddr);
    IDS_HDT_CONSOLE (GNB_TRACE, "Target VidDid: %08x\n", VidDid);
    //restore RP sub bus config
    NbioRegisterWrite(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    
    return VidDid;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Get Downstream PCIe Device subsystem vendor ID and Device ID
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    Downstream subsystem vendor ID and Device ID
 */
 /**----------------------------------------------------------------------------------------*/
UINT32
GetTargetSubSystemVidDid(
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT32                            RpSubBusCfgAddress; 
    UINT32                            TargetPciSubSystemVidDidAddr;
    UINT32                            SubSystemVidDid;
    UINT32                            OldValue,NewValue;
    UINT32                            MmCfg;
    UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
    UINT8                             MappingPortID;

    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;

    RpSubBusCfgAddress = (Wrapper->WrapId == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
    RpSubBusCfgAddress += (MappingPortID * 0x1000);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Get Target SubSystem VidDid: ");
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIeCore%02x ", Wrapper->WrapId);
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIePort%02x\n", MappingPortID);
    //save old sub bus config
    NbioRegisterRead(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    //allocate bus for target
    PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
    SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    NewValue = 0;
    NewValue |= PrimaryBus;
    NewValue |= (SecondaryBus<<8);
    NewValue |= (SubBusNum<<16);
    NbioRegisterWrite(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &NewValue, 0);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Cfg RP: Address=%08x CfgValue=%x\n", RpSubBusCfgAddress, NewValue);
    //read target viddid
    MmCfg = (UINT32)(AsmReadMsr64(MSR_MMIO_Cfg_Base) & 0xfff00000);
    TargetPciSubSystemVidDidAddr = PCIE_SUBSYSTEM_VIDDID_ADDR(MmCfg, SecondaryBus, 0, 0);
    SubSystemVidDid = MmioRead32(TargetPciSubSystemVidDidAddr);
    IDS_HDT_CONSOLE(GNB_TRACE, "Target SubSystem VidDid: %08x\n", SubSystemVidDid);
    //restore RP sub bus config
    NbioRegisterWrite(GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    
    return SubSystemVidDid;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Configuration EQ before PCIe training
 *
 /**----------------------------------------------------------------------------------------*/
EFI_STATUS
PcieEqConfigurationBeforeTraining()
{
    return EFI_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Get EQ Configuration Table
 *
 *
 *
 * @param[out]  EqCfgTable     Pointer to EQ Configuration Table
 * 
 * @retval    EFI_STATUS
 */
 /**----------------------------------------------------------------------------------------*/
EFI_STATUS
GetEqCfgTable(
    OUT HYGON_CPM_EQ_CONFIG_TABLE**  EqCfgTable
)
{
    EFI_PEI_SERVICES            **PeiServices;
    EFI_STATUS                  Status;
    HYGON_CPM_EQ_CONFIG_TABLE*  pEqCfgTable;
    EFI_PEI_EQ_TABLE_PPI        *EqTablePpi;

    PeiServices = (EFI_PEI_SERVICES**)GetPeiServicesTablePointer();

    Status = (*PeiServices)->LocatePpi (
                                PeiServices,
                                &gEqTablePpiGuid,
                                0,
                                NULL,
                                (VOID**)&EqTablePpi
                                );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    Status = EqTablePpi->GetCfgTable(PeiServices, &pEqCfgTable);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    EqTablePpi->DumpEqCfgTable(PeiServices, pEqCfgTable);
    
    EqTablePpi->PreprocessEqCfgTable(PeiServices, pEqCfgTable);
    
    *EqCfgTable = pEqCfgTable;

    return EFI_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIe redo EQ
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    void
 */
 /**----------------------------------------------------------------------------------------*/
VOID
PciePerformEqualization(
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIE_LINK_CNTL_STRUCT             LinkCtl;
    PCIE_LINK_CNTL3_STRUCT            LinkCtl3;
    PCIe_WRAPPER_CONFIG               *PcieWrapper;
    UINT8                             MappingPortID;
    
    PcieWrapper   = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;
    
    //redo EQ
    IDS_HDT_CONSOLE (PCIE_MISC, "PCIe Perform Equalization\n");
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
    LinkCtl3.Field.PERFORM_EQUALIZATION = 1;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
    LinkCtl.Field.RETRAIN_LINK =1;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIe Link Speed change
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  NewSpeed       Link Speed change value
 * 
 * @retval    Old PCIe link speed
 */
 /**----------------------------------------------------------------------------------------*/
UINT8
PcieSpeedChange(
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     UINT8                      NewSpeed
)
{
    PCIE_LINK_CNTL2_STRUCT            LinkCtl2;
    PCIe_WRAPPER_CONFIG               *PcieWrapper;
    UINT8                             MappingPortID;
    PCIE_LINK_CNTL_STRUCT             LinkCtl;
    PCIE_LINK_STATUS_STRUCT           LinkStatus;
    UINT32                            SmnAddress;
    UINT8                             OldSpeed;
    
    PcieWrapper   = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;
    
    //read current link speed
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET;
    NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LinkStatus.Value, 0);
    OldSpeed = (UINT8)LinkStatus.Field.CURRENT_LINK_SPEED;
    
    //speed change to NewSpeed
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL2_OFFSET;
    NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl2.Value, 0);
    LinkCtl2.Field.TARGET_LINK_SPEED = NewSpeed;
    NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl2.Value, 0);
    
    //retraining link
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET;
    NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl.Value, 0);
    LinkCtl.Field.RETRAIN_LINK = 1;
    NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl.Value, 0);
    
    //return old link speed
    return OldSpeed;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Get The Index of EQ Configuration Table witch match current PCIe Engine
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[out] Index          Index of EQ Configuration Table witch match current PCIe Engine
 * 
 * @retval    EFI_STATUS
 */
 /**----------------------------------------------------------------------------------------*/
EFI_STATUS
PcieMatchEqCfg(
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
    OUT    UINT32                     *Index
)
{
    CPU_GEN                    CpuGen;
    UINT32                     VidDid, SubSystemVidDid;
    UINT32                     i;
    UINT32                     StartPhyLane, EndPhyLane;
    UINT32                     ForceSearchIndex = 0xffffffff;
    UINT32                     SpecifiedSearchIndex = 0xffffffff;
    
    CpuGen = GetModelId(); 
    StartPhyLane = Engine->EngineData.StartLane;
    EndPhyLane   = Engine->EngineData.EndLane;
    IDS_HDT_CONSOLE (GNB_TRACE, "StartPhyLane=%x EndPhyLane=%x\n", StartPhyLane, EndPhyLane);
    StartPhyLane %= 32;
    EndPhyLane   %= 32;
    IDS_HDT_CONSOLE (GNB_TRACE, "After StartPhyLane=%x EndPhyLane=%x\n", StartPhyLane, EndPhyLane);
    
    for (i=0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
        if (EqCfgTable->EqCfgList[i].Flag & (FLAG_THROW | FLAG_USED))
            continue;
        if (CpuGen != EqCfgTable->EqCfgList[i].CpuGen)
            continue;
        if (GnbHandle->InstanceId != EqCfgTable->EqCfgList[i].Die)
            continue;
        if ((StartPhyLane == EqCfgTable->EqCfgList[i].StartLane) && (EndPhyLane == EqCfgTable->EqCfgList[i].EndLane)){
            
            if (EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED){
                VidDid = GetTargetVidDid(GnbHandle, Engine);
                if ((VidDid == 0xffffffff) || (VidDid != EqCfgTable->EqCfgList[i].VidDid)) {
                    EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
                    continue;
                }
                if (EqCfgTable->EqCfgList[i].SubSysVidDid != 0xffffffff) {
                    SubSystemVidDid = GetTargetSubSystemVidDid(GnbHandle, Engine);
                    if (SubSystemVidDid != EqCfgTable->EqCfgList[i].SubSysVidDid)
                        continue;
                    EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                    IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SSID SpecifiedSearchIndex=%x\n", i);
                    *Index = i;
                    return EFI_SUCCESS;
                }
                if (SpecifiedSearchIndex != 0xffffffff)
                    continue;
                SpecifiedSearchIndex = i;
                EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SpecifiedSearchIndex=%x\n", SpecifiedSearchIndex);
            } else if (EqCfgTable->EqCfgList[i].Flag & FLAG_FORCE) {
                if (ForceSearchIndex != 0xffffffff)
                    continue;              
                ForceSearchIndex = i;
                EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: ForceSearchIndex=%x\n", ForceSearchIndex);
            }
        } else {
            //EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
            continue;
        }
    }
    if (SpecifiedSearchIndex != 0xffffffff) {
        *Index = SpecifiedSearchIndex;
        return EFI_SUCCESS;
    }
    if (ForceSearchIndex != 0xffffffff) {
        *Index = ForceSearchIndex;
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Configure EQ for current PCIe Engine 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[in] Index           Index of EQ Configuration Table witch match current PCIe Engine
 * 
 * @retval    EFI_STATUS
 */
 /**----------------------------------------------------------------------------------------*/
EFI_STATUS
DoPcieEqCfg(
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
    IN     UINT32                     Index 
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT8                             MappingPortID;
    UINT32                            SmnAddress;
    PCIE_LC_CNTL4                     PcieLcCntl4;
    PCIE_LC_CNTL8                     PcieLcCntl8;  
    PCIE_LINK_CNTL3_STRUCT            LinkCtl3;
    UINT8                             OldLinkSpeed;
    UINT32                            i;
    
    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;

    switch (EqCfgTable->EqCfgList[Index].Type) {
        case pcie_gen3_ttx_cursor:
            break;
        case pcie_gen4_ttx_cursor:
            break;
        case pcie_gen3_ttx_force_preset:
            break;
        case pcie_gen3_ttx_force_otherside_preset:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_force_otherside_preset\n");
            
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL4_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                PcieLcCntl4.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 1;
                PcieLcCntl4.Reg.LC_FORCE_PRESET_VALUE = EqCfgTable->EqCfgList[Index].CfgValue[0];
                PcieLcCntl4.Reg.LC_EQ_SEARCH_MODE = 3;
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
            } else {
                for (i=EqCfgTable->EqCfgList[Index].StartLane; i<=EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            
            //clear PERFORM_EQUALIZATION and LINK_EQUALIZATION_REQ_INT_EN
            SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
            NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            LinkCtl3.Field.PERFORM_EQUALIZATION         = 0;
            LinkCtl3.Field.LINK_EQUALIZATION_REQ_INT_EN = 0;
            NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            //speed change to Gen2
            OldLinkSpeed = PcieSpeedChange(GnbHandle,Engine,2);
            //delay 50ms
            MicroSecondDelay(50*1000);
            //set PERFORM_EQUALIZATION to 1
            SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
            NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            LinkCtl3.Field.PERFORM_EQUALIZATION         = 1;
            NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            //speed change to Gen3
            PcieSpeedChange(GnbHandle,Engine,3);
            //speed change to old speed
            PcieSpeedChange(GnbHandle,Engine,OldLinkSpeed);
            break;
        case pcie_gen4_ttx_force_preset:
            break;
        case pcie_gen4_ttx_force_otherside_preset:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_force_otherside_preset\n");
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL8_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                PcieLcCntl8.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 1;
                PcieLcCntl8.Reg.LC_FORCE_PRESET_VALUE_16GT = EqCfgTable->EqCfgList[Index].CfgValue[0];
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
            } else {
                for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            PciePerformEqualization (GnbHandle, Engine);
            break;
        default:
            return EFI_UNSUPPORTED;
    }
    
    return EFI_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Configure EQ for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */
 /**----------------------------------------------------------------------------------------*/
EFI_STATUS
PcieEqConfigurationAfterTraining(
    IN    GNB_HANDLE            *GnbHandle,
    IN    PCIe_ENGINE_CONFIG    *Engine
)
{
    EFI_STATUS                  Status;
    HYGON_CPM_EQ_CONFIG_TABLE*  gEqCfgTable;
    UINT32                      SearchIndex;
    
    //get Eq Cfg Table
    Status = GetEqCfgTable(&gEqCfgTable);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    //match cfg table?
    Status = PcieMatchEqCfg(GnbHandle, Engine, gEqCfgTable, &SearchIndex);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    //setting Eq
    DoPcieEqCfg(GnbHandle, Engine, gEqCfgTable, SearchIndex);

    return EFI_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Dxio training done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
DxioCfgAfterTraining
 (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               Address;
  UINT8   MappingPortID;
  UINT16  LinkStatus;
  UINT8   LinkSpeed,LinkWidth;
  UINT32  SmnAddress;
  UINT32  TargetSupportSpeedGen3;
  UINT32  Data32;
  UINT8   CpuGen,Stepping;
  CPUID_DATA CpuId;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterTraining Enter\n");
  AsmCpuid (
      0x80000001,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CpuGen    = (UINT8)((CpuId.EAX_Reg & 0xF0) >> 4);
  Stepping  = (UINT8)((CpuId.EAX_Reg >> 0) & 0xF);   // bit 3:0
  
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated(PcieEngine)) {
	      PcieEqConfigurationAfterTraining(GnbHandle, PcieEngine);
          //workaround for PCIe X4/X2/X1 AER with D2 SOC
          if((CpuGen == CPU_HG3) && (Stepping <= 1)){
              MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
              SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_STATUS_SMN_ADDRESS : PCIE1_LINK_STATUS_SMN_ADDRESS;
              SmnAddress += (MappingPortID<<12);
              NbioRegisterRead(GnbHandle,TYPE_SMN,SmnAddress,&LinkStatus,0);
              //read current link speed
              LinkSpeed = LinkStatus&0xF;
              //read current link width
              LinkWidth = (LinkStatus>>0x4)&0x3F;
              //check link width
              if((LinkWidth==4)){
    
                  //set PCIE_LC_CNTL6 bit4 LC_SPC_MODE_8GT = 0
                  MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                  SmnAddress =  (PcieWrapper->WrapId == 0) ? SMN_0x111402ec_ADDRESS : SMN_0x112402ec_ADDRESS;
                  SmnAddress += (MappingPortID<<12);
                  NbioRegisterRMW (GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32) BIT4), 0, 0);
                  
                  //set gen4 strip to 0
                  MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                  SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LC_SPEED_CNTL_ADDRESS : PCIE1_LC_SPEED_CNTL_ADDRESS;
                  SmnAddress += (MappingPortID<<12);
                  NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32) BIT2), 0, 0);
    
                  if(LinkSpeed==3) {
                      //4.speed change to Gen1
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL2_ADDRESS : PCIE1_LINK_CNTL2_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xfffffff0, 1, 0);
                      
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL_ADDRESS : PCIE1_LINK_CNTL_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xffffffff, (UINT32)BIT5, 0);
                      MicroSecondDelay(200000);
                      //6.speed change to Gen3
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL2_ADDRESS : PCIE1_LINK_CNTL2_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xfffffff0, 3, 0);
                      
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL_ADDRESS : PCIE1_LINK_CNTL_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xffffffff, (UINT32)BIT5,0);
                  }
                  if(LinkSpeed==4) {
                      //retrain link
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL_ADDRESS : PCIE1_LINK_CNTL_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32) BIT5), (UINT32)BIT5, 0);
    
                  }
    
              }
              if((LinkWidth==4)||(LinkWidth==2)||(LinkWidth==1)){
                  
                  MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                  SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LC_SPEED_CNTL_ADDRESS : PCIE1_LC_SPEED_CNTL_ADDRESS;
                  SmnAddress += (MappingPortID<<12);
                  NbioRegisterRead(GnbHandle,TYPE_SMN,SmnAddress,&Data32,0);
                  TargetSupportSpeedGen3 = Data32&BIT20;
                  
                  if(TargetSupportSpeedGen3) {
                      //mask bad TLP
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_CE_MASK_ADDRESS : PCIE1_CE_MASK_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32)(BIT6|BIT0)), (UINT32)(BIT6|BIT0), 0);
                      
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? SMN_0x11180008_ADDRESS : SMN_0x11280008_ADDRESS;
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32)(BIT6|BIT0)), (UINT32)(BIT6|BIT0), 0);
                  }
              }
          }
          if((CpuGen == CPU_HG4) && (Stepping <= 1)){
              MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
              SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_STATUS_SMN_ADDRESS : PCIE1_LINK_STATUS_SMN_ADDRESS;
              SmnAddress += (MappingPortID<<12);
              NbioRegisterRead(GnbHandle,TYPE_SMN,SmnAddress,&LinkStatus,0);
              //read current link speed
              LinkSpeed = LinkStatus&0xF;
              //read current link width
              LinkWidth = (LinkStatus>>0x4)&0x3F;
              //check link width
              if((LinkWidth==4)){
    
                  //set PCIE_LC_CNTL6 bit4 LC_SPC_MODE_8GT = 0
                  MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                  SmnAddress =  (PcieWrapper->WrapId == 0) ? SMN_0x111402ec_ADDRESS : SMN_0x112402ec_ADDRESS;
                  SmnAddress += (MappingPortID<<12);
                  NbioRegisterRMW (GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32) BIT4), 0, 0);
                  if(LinkSpeed==3) {
                      //4.speed change to Gen1
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL2_ADDRESS : PCIE1_LINK_CNTL2_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xfffffff0, 1, 0);
                      
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL_ADDRESS : PCIE1_LINK_CNTL_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xffffffff, (UINT32)BIT5, 0);
                      MicroSecondDelay(200000);
                      //6.speed change to Gen3
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL2_ADDRESS : PCIE1_LINK_CNTL2_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xfffffff0, 3, 0);
                      
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LINK_CNTL_ADDRESS : PCIE1_LINK_CNTL_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, 0xffffffff, (UINT32)BIT5,0);
                  }
              }
              if((LinkWidth==4)||(LinkWidth==2)||(LinkWidth==1)){
                  
                  MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                  SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_LC_SPEED_CNTL_ADDRESS : PCIE1_LC_SPEED_CNTL_ADDRESS;
                  SmnAddress += (MappingPortID<<12);
                  NbioRegisterRead(GnbHandle,TYPE_SMN,SmnAddress,&Data32,0);
                  TargetSupportSpeedGen3 = Data32&BIT20;
                  
                  if(TargetSupportSpeedGen3) {
                      //mask bad TLP
                      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? PCIE0_CE_MASK_ADDRESS : PCIE1_CE_MASK_ADDRESS;
                      SmnAddress += (MappingPortID<<12);
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32)(BIT6|BIT0)), (UINT32)(BIT6|BIT0), 0);
                      
                      SmnAddress =  (PcieWrapper->WrapId == 0) ? SMN_0x11180008_ADDRESS : SMN_0x11280008_ADDRESS;
                      NbioRegisterRMW(GnbHandle, TYPE_SMN, SmnAddress, ~((UINT32)(BIT6|BIT0)), (UINT32)(BIT6|BIT0), 0);
                  }
              }
          }
          
        if (PcdGetBool(PcdCfgDisableRcvrResetCycle)) {
          Address = (PcieWrapper->WrapId == 0) ? SMN_0x1220b0c8_ADDRESS : SMN_0x1230b0c8_ADDRESS;
          if (PcieEngine->Type.Port.PortData.PortPresent) {
            NbioRegisterRMW(
            GnbHandle,
            TYPE_SMN,
            Address,
            ~((UINT32) 0x00040000),
            (1 << 18),
             0
            );
          }
        }

      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterTraining Exit\n");
}


/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Dxio init done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieInterfaceAfterDxioDoneSetting (
  IN       GNB_HANDLE       *GnbHandle,
  IN       SOC_LOGICAL_ID   LogicalCpuId
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               Index;
  UINT32               Value;
  UINT32               Address;
  UINT32               PortIndex;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceAfterDxioDoneSetting Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      PcieAfterTrainingDoneCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    Value = (PcieWrapper->WrapId == 0) ? (UINT32) PcdGet8 (PcdGppAtomicOps) : (UINT32) PcdGet8(PcdGfxAtomicOps);
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_F0_ADDRESS : PCIE1_STRAP_F0_ADDRESS,
                     ~((UINT32) (PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK)),
                     (1 << PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_OFFSET) | (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET) ,
                     0
                     );

    if (PcdGetBool (PcdPcieEcrcEnablement)) {
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_F0_ADDRESS : PCIE1_STRAP_F0_ADDRESS,
                       ~((UINT32) (PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK)),
                       (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET),
                       0
                       );
    }


    if (((LogicalCpuId.Family & AMD_FAMILY_ZP) != 0) && ((LogicalCpuId.Revision & AMD_REV_F17_ZP_A0) != 0)) {
      // Set per port strap: STRAP_BIF_MAX_PAYLOAD_SUPPORT
      Value = 1;
      for (PortIndex = 0; PortIndex < 8; PortIndex++) {
        Index = 0xFFFE0000 + StrapList[PortIndex];
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_INDEX_ADDRESS : PCIE1_STRAP_INDEX_ADDRESS,
                           &Index,
                           0
                           );
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           (PcieWrapper->WrapId == 0) ?  PCIE0_STRAP_DATA_ADDRESS : PCIE1_STRAP_DATA_ADDRESS,
                           &Value,
                           0
                           );
      }
    }

    Address = (PcieWrapper->WrapId == 0) ? PCIEP_STRAP_MISC_instPCIE0_link_ADDRESS : PCIEP_STRAP_MISC_instPCIE1_link_ADDRESS;
    for (PortIndex = 0; PortIndex < 8; PortIndex++) {
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | (PortIndex << 12),
                     ~((UINT32) (PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_MASK | PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_MASK)),
                     (1 << PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_OFFSET) | (1 << PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_OFFSET),
                     0
                     );
    }

    for (PortIndex = 0; PortIndex < 8; PortIndex++) {
      Address = (PcieWrapper->WrapId == 0) ? PCIE0_SLOT_APERTURE : PCIE1_SLOT_APERTURE;
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | DEVICE_CNTL2_ADDRESS | (PortIndex << 12),
                     ~((UINT32) DEVICE_CNTL2_CPL_TIMEOUT_VALUE_MASK),
                     (0x6 << DEVICE_CNTL2_CPL_TIMEOUT_VALUE_OFFSET),
                     0
                     );
      Address = (PcieWrapper->WrapId == 0) ? SMN_0x11140280_ADDRESS : SMN_0x11240280_ADDRESS;
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | (PortIndex << 12),
                     ~((UINT32) PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_MASK),
                     (0x1 << PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_OFFSET),
                     0
                     );
      Address = (PcieWrapper->WrapId == 0) ? PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS : PCIE1_LC_LINK_WIDTH_CNTL_ADDRESS;
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | (PortIndex << 12),
                     ~((UINT32) PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_MASK),
                     (0x1 << PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_OFFSET),
                     0
                     );
      Address = (PcieWrapper->WrapId == 0) ? PCIE0_PCIE_LC_CNTL2_ADDRESS : PCIE1_PCIE_LC_CNTL2_ADDRESS;
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | (PortIndex << 12),
                     ~((UINT32) PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_MASK),
                     (0x1 << PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_OFFSET),
                     0
                     );
	  Address = (PcieWrapper->WrapId == 0) ? PCIE0_PCIEP_HW_DEBUG_ADDRESS : PCIE1_PCIEP_HW_DEBUG_ADDRESS;
	  NbioRegisterRMW (GnbHandle,
				     TYPE_SMN,
				     Address | (PortIndex << 12),
				     ~((UINT32) PCIEP_HW_DEBUG_HW_15_DEBUG_MASK),
				     (0x1 << PCIEP_HW_DEBUG_HW_15_DEBUG_OFFSET),
				     0
				     );					 
      //ECRC_ERR_SEVERITY
      Address = (PcieWrapper->WrapId == 0) ? PCIE0_ECRC_ERR_SEVERITY : PCIE1_ECRC_ERR_SEVERITY;
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address | (PortIndex << 12),
                     ~((UINT32) 0x80000),
                     (0x1 << 19),
                     0
                     ); 
      
    }

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     (PcieWrapper->WrapId == 0) ?  PCIE0_CNTL_ADDRESS : PCIE1_CNTL_ADDRESS,
                     ~((UINT32) PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_MASK),
                     0x0 << PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_OFFSET,
                     0
                     );

    if ( PcdGetBool (PcdAmdNbioReportEdbErrors) ) {
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_P_CNTL_ADDRESS : PCIE1_P_CNTL_ADDRESS,
                         ~((UINT32) PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
                         0x0 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
                         0
                         );
    } else {
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         (PcieWrapper->WrapId == 0) ?  PCIE0_P_CNTL_ADDRESS : PCIE1_P_CNTL_ADDRESS,
                         ~((UINT32) PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
                         0x1 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
                         0
                         );
    }

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     (PcieWrapper->WrapId == 0) ?  PCIE0_P_CNTL_ADDRESS : PCIE1_P_CNTL_ADDRESS,
                     ~((UINT32) PCIE_P_CNTL_P_ELEC_IDLE_MODE_MASK),
                     0x1 << PCIE_P_CNTL_P_ELEC_IDLE_MODE_OFFSET,//change PCIE_P_CNTL from 3 to 0 math PCIE Spec
                     0
                     );

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceAfterDxioDoneSetting Exit\n");
}


/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Dxio init done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
SubsystemIdSetting (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  UINT32      Value;
  UINT32      Index;
  UINT32      SubsystemDeviceId;
  UINT32      SubsystemVendorId;

  IDS_HDT_CONSOLE (GNB_TRACE, "SubsystemIdSetting Entry\n");

  // NB ADAPTER D0F0
  Value = PcdGet32 (PcdCfgNbioSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbioSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBADAPTER_SSID_ADDRESS, &Value, 0);
  }

  // IOMMU
  Value = PcdGet32 (PcdCfgIommuSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgIommuSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_IOMMU_SSID_ADDRESS, &Value, 0);
  }

  // PSPCCP
  Value = PcdGet32 (PcdCfgPspccpSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgPspccpSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF0_PSPCCP_SSID_ADDRESS, &Value, 0);
  }

  // NTBCCP
  Value = PcdGet32 (PcdCfgNtbccpSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbccpSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_NTBCCP_SSID_ADDRESS, &Value, 0);
  }

  // XGBE
  Value = PcdGet32 (PcdCfgXgbeSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgXgbeSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE0_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE1_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE2_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE3_SSID_ADDRESS, &Value, 0);
  }

  // NBIF0/2 Dummy Functions
  Value = PcdGet32 (PcdCfgNbifF0Ssid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifF0Ssid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF0_SSID_ADDRESS, &Value, 0); 
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF2_SSID_ADDRESS, &Value, 0);
  }
  
  // NBIF1 Dummy Functions
  Value = PcdGet32 (PcdCfgNbifF1Ssid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifF1Ssid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_SSID_ADDRESS, &Value, 0);
  }
  
  // NBIF Root Bridge Functions 
  Value = PcdGet32 (PcdCfgNbifRCSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifRCSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF0RC_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1RC_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF2RC_SSID_ADDRESS, &Value, 0);
  }

  // NTB
  Value = PcdGet32 (PcdCfgNtbSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF0_NTB_SSID_ADDRESS, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF2_NTB_SSID_ADDRESS, &Value, 0);
  }

  Value = PcdGet32 (PcdCfgAzaliaSsid);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_ADAPTER_ID_ADDRESS, &Value, 0);
    GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "NBIO HDAudio SSID = %x\n", Value));
  }

  // PCIE
  SubsystemDeviceId = (UINT32) PcdGet16 (PcdAmdPcieSubsystemDeviceID);
  if (SubsystemDeviceId != 0) {
    Index = 0xFFFE0083;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_STRAP_INDEX_ADDRESS, &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_STRAP_DATA_ADDRESS, &SubsystemDeviceId, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_STRAP_INDEX_ADDRESS, &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_STRAP_DATA_ADDRESS, &SubsystemDeviceId, 0);
  }
  SubsystemVendorId = (UINT32) PcdGet16 (PcdAmdPcieSubsystemVendorID);
  if (SubsystemVendorId != 0) {
    Index = 0xFFFE0082;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_STRAP_INDEX_ADDRESS, &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_STRAP_DATA_ADDRESS, &SubsystemVendorId, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_STRAP_INDEX_ADDRESS, &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_STRAP_DATA_ADDRESS, &SubsystemVendorId, 0);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "SubsystemIdSetting Exit\n");
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * configure HDAudio verb table
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
HDAudioVerbTableSetting (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  EFI_STATUS            Status;
  FABRIC_TARGET         MmioTarget;
  FABRIC_MMIO_ATTRIBUTE MmioAttr;
  UINT64                HDAudioMmioBase;
  UINT64                HDAudioMmioSize;
  UINT64                VerbTableAddress;
  UINT32                Value;
  UINT32                MmioBase;
  UINT64                Align;

  IDS_HDT_CONSOLE (GNB_TRACE, "NBIO HDAudio Config\n");
  NbioRegisterRead (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_VENDOR_ID_ADDRESS, &Value, 0);
  if (Value != 0xFFFFFFFF) {
    IDS_HDT_CONSOLE (GNB_TRACE, "HDAudio ID = 0x%x\n", Value);
    MmioBase = 0xFFFFFFFF;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS, &MmioBase, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS, &MmioBase, 0);
    GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "   MmioBase read from BAR10 %x\n", MmioBase));

    MmioTarget.TgtType = TARGET_DIE;
    MmioTarget.SocketNum = 0;
    MmioTarget.DieNum = 0;
    MmioAttr.ReadEnable = 1;
    MmioAttr.WriteEnable = 1;
    MmioAttr.NonPosted = 0;
    MmioAttr.MmioType = NON_PCI_DEVICE_BELOW_4G;
    HDAudioMmioSize = 0xFFFFFFFF - MmioBase + 1;
    Align = ~MmioBase;

    Status = FabricAllocateMmio (&HDAudioMmioBase, &HDAudioMmioSize, Align, MmioTarget, &MmioAttr);
    GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "   HDAudioMmioBase = %x,HDAudioMmioSize = %x\n", (UINT32)HDAudioMmioBase, (UINT32)HDAudioMmioSize));
    if (Status == EFI_SUCCESS) {
      MmioBase = (UINT32) HDAudioMmioBase;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS, &MmioBase, 0);
      Value = (MmioBase & 0xFFF00000) | ((MmioBase & 0xFFF00000) >> 16);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1RCCFG_BASELIMIT_ADDRESS, &Value, 0);
      NbioRegisterRead (GnbHandle, TYPE_SMN, NBIF1RCCFG_SUB_BUS_ADDRESS, &Value, 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIF1RCCFG_SUB_BUS_ADDRESS, 0xFF000FF, 0x0a0a00, 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIF1RCCFG_COMMAND_ADDRESS, ~((UINT32) (BIT1 | BIT2)), (UINT32)(BIT1 | BIT2), 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_COMMAND_ADDRESS, ~((UINT32) (BIT1 | BIT2)), (UINT32)(BIT1 | BIT2), 0);
      VerbTableAddress = (UINT64) PcdGet32 (PcdCfgAzaliaCodecVerbTable);
      NbioConfigureVerbTable (MmioBase, VerbTableAddress);
      MmioBase = 0;
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_COMMAND_ADDRESS, ~((UINT32) (BIT1 | BIT2)), 0, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS, &MmioBase, 0);
      NbioRegisterRMW (GnbHandle, TYPE_SMN, NBIF1RCCFG_COMMAND_ADDRESS, ~((UINT32) (BIT1 | BIT2)), 0, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1RCCFG_SUB_BUS_ADDRESS, &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIF1RCCFG_BASELIMIT_ADDRESS, &MmioBase, 0);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set the operating link speed for Promontory based on topology config.
 * Initial training is Gen1.
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
DxioSetPtGenSpeed (
  IN      DXIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  DXIO_PORT_DESCRIPTOR      *EngineDescriptor;
  DXIO_COMPLEX_DESCRIPTOR   *LocalComplex;

  IDS_HDT_CONSOLE (PCIE_MISC, "DxioSetPtGenSpeed Enter\n");
  LocalComplex = ComplexDescriptor;
  while (LocalComplex != NULL) {
    EngineDescriptor = LocalComplex->PciePortList;
    while (EngineDescriptor != NULL) {
      if (EngineDescriptor->Port.MiscControls.SbLink == 1) {
        IDS_HDT_CONSOLE (PCIE_MISC, "  Setting PT to Gen%d\n",EngineDescriptor->Port.LinkSpeedCapability);
        if (EngineDescriptor->Port.LinkSpeedCapability == DxioGenMaxSupported) {
          PcdSet8S (PcdPTTargetSpeed, DxioGen3);
          IDS_HDT_CONSOLE (PCIE_MISC, "  Setting PT to Gen3\n");
        } else {
          PcdSet8S (PcdPTTargetSpeed, EngineDescriptor->Port.LinkSpeedCapability);
          IDS_HDT_CONSOLE (PCIE_MISC, "  Setting PT to Gen%d\n",EngineDescriptor->Port.LinkSpeedCapability);
        }
      }
      EngineDescriptor = DxioInputParsetGetNextDescriptor (EngineDescriptor);
    }
    LocalComplex = DxioInputParsetGetNextDescriptor (LocalComplex);
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "DxioSetPtGenSpeed Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback for NbioSmuServicesPpi installation
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
DxioInitializationCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_STATUS                        ReturnStatus;
  AGESA_STATUS                      DxioStatus;
  GNB_HANDLE                        *GnbHandle;
  UINT32                            SmuArg[6];
  PCIe_PLATFORM_CONFIG              *Pcie;
  PEI_AMD_NBIO_PCIE_SERVICES_PPI    *PcieServicesPpi;
  PEI_AMD_NBIO_SMU_SERVICES_PPI     *SmuServicesPpi;
  AMD_MEMORY_INFO_HOB_PPI           *AmdMemoryInfoHob;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi;
  DXIO_COMPLEX_DESCRIPTOR           *PcieTopologyData;
  PEI_AMD_NBIO_PCIE_COMPLEX_PPI     *NbioPcieComplexPpi;
  AMD_PEI_SOC_LOGICAL_ID_PPI        *SocLogicalIdPpi;
  SOC_LOGICAL_ID                    LogicalId;
  UINT32                            PackageType;
  BOOLEAN                           MultiSocket;
  //UINT32                            Value;
//  UINT32                            linkcntl;
  AGESA_TESTPOINT (TpDxioInitializationCallbackPpiEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioInitializationCallbackPpi Entry\n");
  ReturnStatus = EFI_SUCCESS;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdMemoryInfoHobPpiGuid, 0, NULL, &AmdMemoryInfoHob);
  if (!EFI_ERROR (Status)) {
    // Install PPI to notify other drivers that PCIe training is starting
    Status = (**PeiServices).InstallPpi (PeiServices, &mNbioPcieTrainingStartPpiList);
    GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "AmdMemoryInfoHobPpiGuid is installed\n"));
    Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioSmuServicesPpiGuid, 0, NULL, &SmuServicesPpi);
    if (!EFI_ERROR (Status)) {
      GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "gAmdNbioSmuServicesPpiGuid is installed\n"));
      Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioPcieComplexPpiGuid, 0, NULL, &NbioPcieComplexPpi);
      if (!EFI_ERROR (Status)) {
        GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "gAmdNbioPcieComplexPpiGuid is installed\n"));
        NbioPcieComplexPpi->PcieGetComplex (NbioPcieComplexPpi, &PcieTopologyData);
        // Need topology structure to get GnbHandle
        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdNbioPcieServicesPpiGuid,
                                   0,
                                   NULL,
                                   (VOID **)&PcieServicesPpi
                                   );
        PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
        Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

        if (TRUE == PcdGetBool (PcdEarlyBmcLinkTraining)) {
          PcieTopologyData->BmcLinkLocation = (PcdGet8 (PcdEarlyBmcLinkSocket) << 2) + PcdGet8(PcdEarlyBmcLinkDie);
          GnbHandle = NbioGetHandle (Pcie);
          DxioCleanUpEarlyInit (GnbHandle, PcieTopologyData->BmcLinkLocation);
        }
        else {
          PcieTopologyData->BmcLinkLocation = 0xFF;
        }
        PcieForceTurnOffUnusedLanes (PcieTopologyData);
        DxioUserConfigConfigDump (PcieTopologyData);

        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdSocLogicalIdPpiGuid,
                                   0,
                                   NULL,
                                   &SocLogicalIdPpi
                                   );
        Status = SocLogicalIdPpi->GetLogicalIdOnCurrentCore (&LogicalId);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[0] = 0xAA55AA55;
        SmuServicesPpi->SmuServiceRequest (SmuServicesPpi, 0, 1, SmuArg, SmuArg);

        GnbHandle = NbioGetHandle (Pcie);
        PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
        if (GnbHandle->InstanceId == 0) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "Disable  DxioHDaudioEngine\n");
          NbioDisableNbifDevice (GnbHandle, DxioHDaudioEngine, 0);
        }
        MultiSocket = FALSE;
        while (GnbHandle != NULL) {
          IDS_HOOK (IDS_HOOK_NBIO_PCIE_USER_CONFIG, (VOID *)GnbHandle, (VOID *)PcieTopologyData);
          // Call KPNP workaround
          PcieInterfaceInitSetting (GnbHandle, LogicalId);
          if (GnbHandle->SocketId != 0) {
            MultiSocket = TRUE;
          }
          GnbHandle = GnbGetNextHandle(GnbHandle);
        }
        DxioSetPtGenSpeed (PcieTopologyData);
        SataChannelTypeSetting  (PcieTopologyData);
        InitNTBFeature (PeiServices, PcieTopologyData, Pcie);
        GnbHandle = NbioGetHandle (Pcie);
        if(PcdGetBool(PcdH3PcieSITestMode) == FALSE) {
          while (GnbHandle != NULL) {
              PcieCfgBeforeTraining(GnbHandle);
              GnbHandle = GnbGetNextHandle(GnbHandle);
          }
        } 
        // At this point we know the NBIO topology and SMU services are available
        // We can now use SMU to initialize DXIO

        DxioStatus = DxioEarlyInitV1 (PeiServices, SmuServicesPpi, Pcie, PcieTopologyData);

        if (DxioStatus != AGESA_SUCCESS) {
          IDS_HDT_CONSOLE (GNB_TRACE, "DXIO ERROR!!\n");
          ReturnStatus = EFI_INVALID_PARAMETER;
          ASSERT (FALSE);
        }
        PcieConfigDebugDump(Pcie);

        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
//          IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *)GnbHandle, (VOID *)Pcie);
          DxioCfgAfterTraining (GnbHandle);
          PciePortsVisibilityControlZP (HidePorts, GnbHandle);
          if ((1 << ZP_SOCKET_AM4) != PackageType) {
            // XGMI ADAPTATION
            LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
            SmuArg[1] = DXIO_ARG_RNTM_ENABLEFEATURE_XGMIADAPTATION;
            if (MultiSocket) {
              if ((GnbHandle->InstanceId == 0) ||
                  (GnbHandle->InstanceId == 1) ||
                  (GnbHandle->InstanceId == 4) ||
                  (GnbHandle->InstanceId == 5)) {
                SmuArg[2] = 0x2F;
              } else {
                SmuArg[2] = 0x1F;
              }
            } else {
              SmuArg[2] = 0x0F;
            }
            SmuArg[3] = 0;  //tAdapt_in default to 0
            SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_RNTM_ENABLEFEATURE, SmuArg, SmuArg);
          }
          PcieInterfaceAfterDxioDoneSetting (GnbHandle, LogicalId);
          SubsystemIdSetting (GnbHandle);
          GnbHandle = GnbGetNextHandle (GnbHandle);
        }
        PcieConfigureHotplugPortsZP (Pcie);
        InitNTBAfterTraining (PeiServices, PcieTopologyData, Pcie);
        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdFabricTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &FabricTopologyServicesPpi
                                   );
        InitBusRanges (Pcie, FabricTopologyServicesPpi);

        // Initialize ARI
        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (TRUE == PcdGetBool (PcdCfgPcieAriSupport)) {
            NbioRegisterRMW (
              GnbHandle,
              TYPE_SMN,
              IOC_FEATURE_CNTL_ADDRESS,
              ~((UINT32) NBMISC_0118_IOC_ARI_SUPPORTED_MASK),
              1 << NBMISC_0118_IOC_ARI_SUPPORTED_OFFSET,
              0
              );
            
			//PcieAriInit (GnbHandle);
			
          }
          GnbHandle = GnbGetNextHandle (GnbHandle);
        }

        // Install PPI to notify other drivers that PCIe training is complete
        Status = (**PeiServices).InstallPpi (PeiServices, &mNbioPcieTrainingDonePpiList);
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioInitializationCallbackPpi Exit Status = 0x%x\n", ReturnStatus);
  AGESA_TESTPOINT (TpDxioInitializationCallbackPpiExit, NULL);
  return ReturnStatus;
}


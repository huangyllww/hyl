/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieZPDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieZPDxe
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
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/AmdNbioPcieAerProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersZP.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioSmuV9Lib.h>
#include "AmdNbioPcieAerZP.h"
#include "AmdNbioPcieSrisZP.h"
#include "AmdNbioPcieClkReqZP.h"
#include <Library/PciLib.h>//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3  >

#define FILECODE        NBIO_PCIE_AMDNBIOPCIEZPDXE_AMDNBIOPCIEZPDXE_FILECODE
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 + >
#define PCIE0_LC_SPEED_CNTL_ADDRESS                                          0x11140290
#define PCIE1_LC_SPEED_CNTL_ADDRESS                                          0x11240290
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 - >

extern AMD_NBIO_PCIE_AER_PROTOCOL mAmdNbioPcieAerProtocol;
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

VOID
NbioRASControlZP (
  IN GNB_HANDLE                 *GnbHandle
  );

EFI_STATUS
AmdPcieGetTopology (
  IN       DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL  *This,
     OUT   UINT32                               **DebugOptions
  )
{
  EFI_STATUS                           Status;
  GNB_PCIE_INFORMATION_DATA_HOB        *GnbPcieInfoDataHob;
  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &GnbPcieInfoDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GnbPcieInfoDataHob = GetNextGuidHob (&gGnbPcieHobInfoGuid, GnbPcieInfoDataHob);
  if (GnbPcieInfoDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *DebugOptions = (UINT32*) GnbPcieInfoDataHob;
  return Status;
}

DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL mDxeAmdNbioPcieServicesProtocol = {
  AmdPcieGetTopology
};

/*----------------------------------------------------------------------------------------*/
/**
 * Clock gating
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PciePwrClockGating (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG            *Wrapper;
  PCIEDIR_460_STRUCT             PcieDir460;
  PCIEDIR_044_STRUCT             PcieDir044;
  PCIECORE_18C_STRUCT            PcieCore18C;
  UINT32                         Address0;
  UINT32                         Address1;
  UINT32                         Address2;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Enter\n");

  Wrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (Wrapper != NULL) {
    // LCLK Gating
    if (Wrapper->WrapId == 1) {
      Address0 = CPM_CONTROL_PCIECORE0_ADDRESS;
      Address1 = PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS;
      Address2 = PCIE_SDP_CTRL_PCIECORE0_ADDRESS;
    } else {
      Address0 = CPM_CONTROL_PCIECORE1_ADDRESS;
      Address1 = PCIE_CONFIG_CNTL_PCIECORE1_ADDRESS;
      Address2 = PCIE_SDP_CTRL_PCIECORE1_ADDRESS;
    }

    NbioRegisterRead ( GnbHandle, CPM_CONTROL_TYPE, Address0, &PcieDir460.Value, 0);
    NbioRegisterRead ( GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &PcieDir044.Value, 0);
    NbioRegisterRead ( GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &PcieCore18C.Value, 0);

    if (Wrapper->Features.LclkGating == 1) {
      PcieDir460.Field.LCLK_DYN_GATE_ENABLE = 0x1;
      PcieDir460.Field.LCLK_GATE_TXCLK_FREE = 0x0;
      PcieDir460.Field.LCLK_DYN_GATE_LATENCY = 0x1;
    }
    // TXCLK+REFCLK
    if (Wrapper->Features.ClkGating == 1) {
      // TXCLK_PERM gating enable
      PcieDir460.Field.TXCLK_PERM_GATE_ENABLE = 0x1;
      PcieDir460.Field.TXCLK_PERM_GATE_LATENCY = 0x0;
      PcieDir460.Field.RCVR_DET_CLK_ENABLE = 0x1;
      PcieDir044.Field.DYN_CLK_LATENCY = 0xf;

      // TXCLK DYN gating enable
      PcieDir460.Field.TXCLK_DYN_GATE_ENABLE = 0x1;
      PcieDir460.Field.TXCLK_DYN_GATE_LATENCY = 0x0;

      // TXCLK REGS gating enable
      PcieDir460.Field.TXCLK_REGS_GATE_ENABLE = 0x1;
      PcieDir460.Field.TXCLK_REGS_GATE_LATENCY = 0x0;

      // TXCLK PRBS gating enable
      PcieDir460.Field.TXCLK_PRBS_GATE_ENABLE = 0x1;

      // TXCLK LCNT gating enable
      PcieDir460.Field.TXCLK_LCNT_GATE_ENABLE = 0x1;

      // TXCLK PIF1X gating enable
      // PcieDir460.Field.TXCLK_PIF_GATE_ENABLE = 0x1;

      // REFCLK REGS gating enable
      PcieDir460.Field.REFCLK_REGS_GATE_ENABLE = 0x1;
      PcieDir460.Field.REFCLK_REGS_GATE_LATENCY = 0x0;

      PcieCore18C.Field.CI_SLV_REQR_PART_DISCONNECT_EN = 0x1;
      PcieCore18C.Field.CI_MSTSDP_CLKGATE_ONESIDED_ENABLE = 0x1;
    }
    NbioRegisterWrite (GnbHandle, CPM_CONTROL_TYPE, Address0, &PcieDir460.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &PcieDir044.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &PcieCore18C.Value, GNB_REG_ACC_FLAG_S3SAVE);

    Wrapper = PcieLibGetNextDescriptor (Wrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Lock registers
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PcieLockRegisters (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  NbioRegisterRMW (GnbHandle, TYPE_SMN, PCIE0_CNTL_ADDRESS, ~((UINT32) BIT0), (UINT32) BIT0, GNB_REG_ACC_FLAG_S3SAVE);
  NbioRegisterRMW (GnbHandle, TYPE_SMN, PCIE1_CNTL_ADDRESS, ~((UINT32) BIT0), (UINT32) BIT0, GNB_REG_ACC_FLAG_S3SAVE);
}

//huangjin002-20190904 Set OPA link speed as Gen1 at PciIoEvent + >
VOID
EFIAPI
SetOmniPathLnkSpeed_Gen1 (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol; 
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;
  UINT32                              VendorID;
  UINT8                               Port_Index;
  UINT32                              LcSpeedCntl;
  UINT8                               BridgeBusNum = 0;
  UINT8                               LanBusNumber = 0;

  Port_Index = 0;
  LcSpeedCntl = 0;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "SetOmniPathLnkSpeed_Gen1 Enter\n");
  
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
          
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      if (GnbHandle->InstanceId == 3 || GnbHandle->InstanceId == 7) {

        for (Port_Index = 0; Port_Index < 16; Port_Index++){
                  
          LcSpeedCntl = 0x00; 
          BridgeBusNum = (GnbHandle->InstanceId) * 0x10;
          LanBusNumber = PciRead8(PCI_LIB_ADDRESS(BridgeBusNum, 3, 1, 0x19));
          if (LanBusNumber != 0xff)
          {
             VendorID = PciRead32(PCI_LIB_ADDRESS(LanBusNumber, 0, 0, 0x0));
          }
                 
          if (VendorID == 0x808624F0 || VendorID == 0x24F08086){

            if (Port_Index > 7) {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
              LcSpeedCntl |= 1 << 2;
              LcSpeedCntl |= 1 << 3;
              LcSpeedCntl &= 0 << 4;
              NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
              IDS_HDT_CONSOLE (MAIN_FLOW, "Enable LC_TARGET_LINK_SPEED_OVERRIDE_EN and change Target Link Speed to Gen 1\n");
            } else {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
              LcSpeedCntl |= 1 << 2;
              LcSpeedCntl |= 1 << 3;
              LcSpeedCntl &= 0 << 4;
              NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
              IDS_HDT_CONSOLE (MAIN_FLOW, "Enable LC_TARGET_LINK_SPEED_OVERRIDE_EN and change Target Link Speed to Gen 1\n");
            }
          } 
        }
      }

      if (GnbHandle->InstanceId == 0x07){
        break;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  gBS->CloseEvent (Event);
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "SetOmniPathLnkSpeed_Gen1 Exit\n");
  return;
  
}
//huangjin002-20190904 Set OPA link speed as Gen1 at PciIoEvent - >

//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 + >
VOID
EFIAPI
SetOmniPathLnkSpeed (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS						  Status;
  PCIe_PLATFORM_CONFIG				  *Pcie;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol; 
  GNB_PCIE_INFORMATION_DATA_HOB 	  *PciePlatformConfigHobData;
  GNB_HANDLE						  *GnbHandle;
  EFI_HANDLE 						  *Handle;
  UINTN 							  Number;
  UINTN 							  i;
  UINT32        					  VendorID;
  UINT8						          Port_Index;
  UINT32                              LcSpeedCntl;
  UINT16						      DevStatus;
  UINT16 							  NextItemPtr;
  UINT16 							  CapOffset;
  UINT16 							  LnkCtlOffs;
  UINT16 							  LinkControl;

  Port_Index = 0;
  LcSpeedCntl = 0;
  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: AMD Tiger Solution Start ...\n");
  
  Status = gBS->LocateProtocol (
				  &gAmdNbioPcieServicesProtocolGuid,
				  NULL,
				  &PcieServicesProtocol
				  );
	
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
	GnbHandle = NbioGetHandle (Pcie);
	while (GnbHandle != NULL) {
	  if (GnbHandle->InstanceId == 3 || GnbHandle->InstanceId == 7) {

		// Set PCIE_LC_SPEED_CNTL[27] = 1
		IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Port[%d] PCIE_LC_SPEED_CNTL[27] = 1 -- START ...\n", GnbHandle->InstanceId);
		for (Port_Index = 0; Port_Index < 16; Port_Index++){
			
		  LcSpeedCntl = 0x00;
		  
          if (Port_Index > 7) {
		    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: First Read LcSpeedCntl = 0x%x\n", LcSpeedCntl);
			LcSpeedCntl |= 1 << 27;
			NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: After Write and Read Back Again LcSpeedCntl = 0x%x\n", LcSpeedCntl);
		  } else {
		    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: First Read LcSpeedCntl = 0x%x\n", LcSpeedCntl);
			LcSpeedCntl |= 1 << 27;
			NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: After Write and Read Back Again LcSpeedCntl = 0x%x\n", LcSpeedCntl);
		  }
		}
		IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Port[%d] PCIE_LC_SPEED_CNTL[27] = 1 -- END ...\n\n", GnbHandle->InstanceId);

		IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Port[%d] PCIE_LC_SPEED_CNTL[1] = 1 -- START ...\n\n", GnbHandle->InstanceId);
		for (Port_Index = 0; Port_Index < 16; Port_Index++){
			
		  LcSpeedCntl = 0x00;
		  
          if (Port_Index > 7) {
		    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: First Read LcSpeedCntl = 0x%x\n", LcSpeedCntl);
			LcSpeedCntl |= 1 << 1;
			NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((Port_Index - 8) << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: After Write and Read Back Again LcSpeedCntl = 0x%x\n", LcSpeedCntl);
		  } else {
		    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: First Read LcSpeedCntl = 0x%x\n", LcSpeedCntl);
			LcSpeedCntl |= 1 << 1;
			NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (Port_Index << 12), &LcSpeedCntl, 0);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: After Write and Read Back Again LcSpeedCntl = 0x%x\n", LcSpeedCntl);
		  }
		}
		IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Port[%d] PCIE_LC_SPEED_CNTL[1] = 1 -- END ...\n", GnbHandle->InstanceId);
	  }

	  if (GnbHandle->InstanceId == 0x07){
        break;
	  }
	  GnbHandle = GnbGetNextHandle (GnbHandle);
	}
  }

  //	
  // Set Link Control 2 Register
  //
  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Link Control 2 Register START ...\n\n");
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL, &Number, &Handle);
  if (EFI_ERROR(Status)) return;

  for(i = 0; i < Number; i++){
    EFI_PCI_IO_PROTOCOL *PciIo;
	Status= gBS->HandleProtocol(Handle[i],&gEfiPciIoProtocolGuid,(VOID**)&PciIo);
	if (EFI_ERROR(Status)) continue;

	Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x00, 1, &VendorID);
	if (VendorID == 0x808624F0 || VendorID == 0x24F08086){
	   //PciIo->GetLocation (
       //                 PciIo,
       //                 &SegmentNumber,
       //                 &BusNumber,
       //                 &DeviceNumber,
       //                 &FunctionNumber
       //);

	  //IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: 100G Omni Path - Bus[0x%x], Device[0x%x], Function[0x%x]\n", BusNumber, DeviceNumber, FunctionNumber);

	  //RootPortNum = BusNumber - 1;


      // Set 100G Omni Path Card Target Link Speed To Gen3
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0x06, 1, &DevStatus);
	  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: 100G Omni Path Device Status = 0x%x\n", DevStatus);
	  if (EFI_ERROR(Status)) continue;
			  
	  if ((DevStatus & 0x10) == 0x10){ // Support capabilities 
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0x34, 1, &NextItemPtr);
		if (EFI_ERROR(Status)) continue;
        CapOffset = NextItemPtr;
		
		IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: 100G Omni Path NextItemPtr = 0x%x\n", NextItemPtr);
		Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapOffset, 1, &NextItemPtr);
				
		while ((NextItemPtr & 0xff) != 0x00){ 
	      if ((NextItemPtr & 0xff) == 0x10){
            LnkCtlOffs = CapOffset + 0x30;
			Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, LnkCtlOffs, 1, &LinkControl);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: 100G Omni Path Link Control = 0x%x\n", LinkControl);
			LinkControl &= 0xFFF0;
	  	    LinkControl |= 0x03;
			Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, LnkCtlOffs, 1, &LinkControl);
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: 100G Omni Path Link Control Read Back = 0x%x\n", LinkControl);
			break;
		  }
		  
		  CapOffset = (NextItemPtr&0xff00) >> 8;
		  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapOffset, 1, &NextItemPtr);
		}
	  }
			
	  /*for(j = 0; j < Number; j++){
	    Status= gBS->HandleProtocol(Handle[j],&gEfiPciIoProtocolGuid,(VOID**)&PciIo);
		if (EFI_ERROR(Status)) continue;
		PciIo->GetLocation (
                        PciIo,
                        &R_SegmentNumber,
                        &R_BusNumber,
                        &R_DeviceNumber,
                        &R_FunctionNumber
        );
        
		if(RootPortNum == R_BusNumber){
		  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port - Bus[0x%x], Device[0x%x], Function[0x%x]\n", R_BusNumber, R_DeviceNumber, R_FunctionNumber);
          Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x0E, 1, &Header);
		  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port Header = 0x%x\n", Header);
		  if (EFI_ERROR(Status)) continue;
		  
		  if ((Header & 0x01) == 0x01){
            Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x19, 1, &SecBus);
			if (EFI_ERROR(Status)) continue;
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port SecBus = 0x%x\n", SecBus);
			
			Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x1A, 1, &SubBus);
			if (EFI_ERROR(Status)) continue;
			IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port SubBus = 0x%x\n", SubBus);
			
			if(SecBus == SubBus && SecBus == BusNumber){
              // Find the Root Port
              Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0x06, 1, &DevStatus);
			  if (EFI_ERROR(Status)) continue;
			  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port DevStatus = 0x%x\n", DevStatus);
			  
			  if ((DevStatus & 0x10) == 0x10){ // Support capabilities 
                Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0x34, 1, &NextItemPtr);
				if (EFI_ERROR(Status)) continue;

				CapOffset = NextItemPtr;
				Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapOffset, 1, &NextItemPtr);
				IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port NextItemPtr = 0x%x\n", NextItemPtr);
				
				while ((NextItemPtr & 0xff) != 0x00){
				  if ((NextItemPtr & 0xff) == 0x10){
                    LnkCtlOffs = CapOffset + 0x30;
					Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, LnkCtlOffs, 1, &LinkControl);
					IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port Link Control = 0x%x\n", LinkControl);
					LinkControl &= 0xFFF0;
	  				LinkControl |= 0x03;
					Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, LnkCtlOffs, 1, &LinkControl);
					IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Root Port Link Control Read Back = 0x%x\n", LinkControl);
					Found_Flag = TRUE;
					break;
				  }
				  CapOffset = (NextItemPtr&0xff00)>>8;
                  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapOffset, 1, &NextItemPtr);
				}
			  }
			}
		  }
		}
		
		if (Found_Flag == TRUE){
          Found_Flag = FALSE;
		  break;
		}
	  }*/
	}
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: Set Link Control 2 Register END ...\n\n");
  
  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "[OemDbg]: AMD Tiger Solution End ...\n");

  return;
  
}
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 - >
/**
 *---------------------------------------------------------------------------------------
 *
 *  AmdPcieMiscInit
 *
 *  Description:
 *     Calls AmdInitMid as part of Agesa DXE Driver Initialization.
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/

VOID
EFIAPI
AmdPcieMiscInit (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;
  UINT32                              SmuArg[6];
  EFI_STATUS                          CalledStatus;

  AGESA_TESTPOINT (TpAmdPcieMiscInitEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdPcieMiscInit Entry\n");
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      // Slot power limit
      PcieSlotPowerLimitInterface (GnbHandle);
      // Enabling PCIe Core Power Saving Features
      PciePwrClockGating (GnbHandle);
      // Initialize AER
      NbioPcieAerInitZP (GnbHandle);
      // configure MaxReadRequestSize on PCIE interface
      PcieMaxReadRequestInterface (GnbHandle);
      // configure MaxPayloadSize on PCIE interface
      PcieMaxPayloadInterface (GnbHandle);
      // configure CI_MAX_CPL_PAYLOAD_SIZE on PCIE core
      PcieMaxCplPayloadInterface (GnbHandle);
      // enable Clock Power Managment
      PcieClkPmInterface (GnbHandle);

      PcieSrisInit (GnbHandle);
      PcieAspmL1SSInterface (GnbHandle);
      PcieClkReqInterface (GnbHandle);
      PcieAspmInterface (GnbHandle);
      PcieGen4Workaround (GnbHandle);
      // program coomon clock configuration
      PcieCommClkCfgInterface (GnbHandle);
      // Enable ARI
      if (PcdGetBool (PcdAmdNbioRASControl)) {
        NbioRASControlZP (GnbHandle);
      }

      NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
      SmuArg[1] = 1;
      SmuArg[2] = 0;
      SmuArg[3] = PcdGetBool (PcdAmdXgmiReadaptation);
      CalledStatus = NbioDxioServiceRequestV9 (GnbHandle, SMC_DXIO_MSG_XGMI_READAPTATION, SmuArg, 0);

      PcieLockRegisters (GnbHandle);
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdPcieMiscInit Exit\n");
  AGESA_TESTPOINT (TpAmdPcieMiscInitExit, NULL);

  return;
}

EFI_STATUS
EFIAPI
AmdNbioPcieZPDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  EFI_EVENT           PciIoEvent;
  EFI_EVENT           PciIoEventForOpa;//<huangjin001-20190918 Optimize the relevant code for OPA card  >  
  VOID                *Registration;
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 + >
  EFI_EVENT           OmniPathEvent;//<huangjin003-20190412 Change Intel 100G OPA card speed as Gen2 + >
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 - >

  AGESA_TESTPOINT (TpNbioPCIeDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieZPDxeEntry\n");

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioPcieServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioPcieServicesProtocol
                  );

  //
  // produce AMD_NBIO_PCIE_AER_PROTOCOL
  //
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioPcieAerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdNbioPcieAerProtocol
                  );

  //
  // Register the event handling function for AmdPcieMiscInit
  //
  Status = gBS->CreateEventEx (
             EVT_NOTIFY_SIGNAL,
             TPL_NOTIFY,
             AmdPcieMiscInit,
             NULL,
             NULL,
             &PciIoEvent
             );
 
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 + > 
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  SetOmniPathLnkSpeed,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &OmniPathEvent
                  );
//<huangjin004-20190613 Change Intel 100G OPA card speed as Gen3 - >

  Status = gBS->RegisterProtocolNotify (
              &gEfiPciIoProtocolGuid,
              PciIoEvent,
              &Registration
              );

//<huangjin001-20190918 Optimize the relevant code for OPA card + >  
//huangjin002-20190904 Set OPA link speed as Gen1 at PciIoEvent + >  
    Status = gBS->CreateEventEx (
               EVT_NOTIFY_SIGNAL,
               TPL_NOTIFY,
               SetOmniPathLnkSpeed_Gen1,
               NULL,
               NULL,
               &PciIoEventForOpa
               );   
    
    Status = gBS->RegisterProtocolNotify (
                &gEfiPciIoProtocolGuid,
                PciIoEventForOpa,
                &Registration
                ); 
//huangjin002-20190904 Set OPA link speed as Gen1 at PciIoEvent - >    
//<huangjin001-20190918 Optimize the relevant code for OPA card - >   
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieZPDxeExit\n");
  AGESA_TESTPOINT (TpNbioPCIeDxeExit, NULL);
  return EFI_SUCCESS;
}

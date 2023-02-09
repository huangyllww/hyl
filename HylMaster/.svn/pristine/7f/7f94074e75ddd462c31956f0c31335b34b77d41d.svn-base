//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <AmiDxeLib.h>
#include <Setup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <GnbDxio.h>
#include <Library/GnbCommonLib.h>
#include <Library/GnbPcieConfigLib.h>
#include <Library/PcieMiscCommLib.h>
#include <OptionGnb.h>
#include <GnbRegistersZP.h>
#include <Library/NbioRegisterAccLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/PcdLib.h>

VOID
STATIC
PcieMaxCplPayloadReInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  UINT8                           EngineMaxPayload=0xff;
  UINT16                          linkwidth;
  SETUP_DATA                      SetupData;
  UINT64                          SetupVariableSize = sizeof(SETUP_DATA);
  UINT32                          SetupVariableAttributes=0;
  EFI_GUID                        gSetupGuid = SETUP_GUID;
  EFI_STATUS                      Status;
  
  Status = pRS->GetVariable (L"Setup",
                              &gSetupGuid,
                              &SetupVariableAttributes,
                              &SetupVariableSize,
                              &SetupData);
  if (Status == EFI_SUCCESS) {
      if(SetupData.MaxPayload == 55){
          if (Engine->EngineData.EndLane >= Engine->EngineData.StartLane) {
            linkwidth = Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1;
          } else {
            linkwidth = Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1;
          }
          if (linkwidth >= 4) {
            EngineMaxPayload =  MAX_PAYLOAD_512;
          } else if (linkwidth == 2) {
            EngineMaxPayload =  MAX_PAYLOAD_256;
          } else if (linkwidth == 1) {
            EngineMaxPayload =  MAX_PAYLOAD_128;
          }
      }       
      else
          EngineMaxPayload =  SetupData.MaxPayload;
  }
  else
      EngineMaxPayload =  Engine->Type.Port.MaxPayloadSize;
 
    *((UINT8 *)Buffer) = EngineMaxPayload;
}

VOID
PcieMaxCplPayloadReconfigureInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  UINT8                           MaxCplPayload=0xff,CurrMaxCplPayload;
  UINT32                          Value, ValueMask, PCIEPSMNAddress;
  PCI_ADDR                        RPIndexAddress, RPDataAddress;
  UINT32                          SmnAddress;

  
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxCplPayloadReconfigureInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      CurrMaxCplPayload=0;
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        SmnAddress = (PcieWrapper->WrapId == 0) ?  GPP0_LINK_CTL_STS_ADDRESS : GPP1_LINK_CTL_STS_ADDRESS;
        SmnAddress |= (PcieEngine->Type.Port.PortId % 8) << 12;
        NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Value, 0);
        if (Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) {
            PcieMaxCplPayloadReInitCallback (PcieEngine, &CurrMaxCplPayload);
        }
        
        if (CurrMaxCplPayload) {
            if (CurrMaxCplPayload < MaxCplPayload)
                MaxCplPayload = CurrMaxCplPayload;
        }
      }
      if (MaxCplPayload != 0xff) {
        RPIndexAddress = GnbHandle->Address;
        RPDataAddress = RPIndexAddress;
        RPIndexAddress.Address.Register = D0F0xB8_ADDRESS;
        RPDataAddress.Address.Register = D0F0xB8_ADDRESS + 4;
        PCIEPSMNAddress = PCIEP_PORT_CNTL_ADDRESS + (PcieEngine->Type.Port.CoreId << 20) \
          + ((PcieEngine->Type.Port.PortId - (8 * PcieEngine->Type.Port.CoreId)) << 12);
        IDS_HDT_CONSOLE (GNB_TRACE, "PCIEPSMNAddress: 0x%x\n", PCIEPSMNAddress);
        GnbLibPciWrite (RPIndexAddress.AddressValue, AccessWidth32, &PCIEPSMNAddress, NULL);
        GnbLibPciRead (RPDataAddress.AddressValue, AccessWidth32, &Value, NULL);
        ValueMask = ~((UINT32) PCIEP_PORT_CNTL_CI_PRIV_MAX_CPL_PAYLOAD_SIZE_MASK \
          | (UINT32) PCIEP_PORT_CNTL_CI_MAX_CPL_PAYLOAD_SIZE_MODE_MASK);
        Value &= ValueMask;
        Value |= (MaxCplPayload << PCIEP_PORT_CNTL_CI_PRIV_MAX_CPL_PAYLOAD_SIZE_OFFSET) \
          | (1 << PCIEP_PORT_CNTL_CI_MAX_CPL_PAYLOAD_SIZE_MODE_OFFSET);
        GnbLibPciWrite (RPDataAddress.AddressValue, AccessWidth32, &Value, NULL);
        GnbLibPciRead (RPDataAddress.AddressValue, AccessWidth32, &Value, NULL);
      }
      MaxCplPayload = 0xff;
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
    }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxCplPayloadReconfigureInterface Exit\n");
}

EFI_STATUS
EFIAPI   
SetCplMaxPayloadEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                          Status;
    PCIe_PLATFORM_CONFIG                *Pcie;
    DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
    GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
    GNB_HANDLE                          *GnbHandle;
    
    DEBUG ((-1,"SetCplMaxPayload Enter\n"));
    
    InitAmiLib(ImageHandle,SystemTable);
 
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
          if (PcdGetBool (PcdCfgMaxPayloadEnable) == 0)
              PcieMaxCplPayloadReconfigureInterface (GnbHandle);
          GnbHandle = GnbGetNextHandle (GnbHandle);
      }
    } 
       
    DEBUG ((-1,"SetCplMaxPayload Exit\n"));  
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

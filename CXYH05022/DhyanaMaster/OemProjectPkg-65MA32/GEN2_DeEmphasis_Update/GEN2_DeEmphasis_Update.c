#include <AmdPcieComplex.h>
#include <AmdServerHotplug.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/PciLib.h>

#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
//#include <GnbRegistersSSP/PCIEPORT.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersZP.h>
#include <Library/PcieConfigLib.h>
#include <Library/SmnAccessLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------

typedef struct  _SLOT_DESCRIPTION{
  UINT8       InstanceId;
  UINT8       DeviceNumber;
  UINT8       FunctionNumber;
}SLOT_DESCRIPTION;


SLOT_DESCRIPTION      SlotDescription[]={
  {0, 3, 1},
  {3, 1, 3},
  {3, 1, 4},
  {6, 3, 1},
  {6, 3, 2},
  {6, 3, 3},
  {6, 3, 4},
  {0xff, 0xff, 0xff}
};


/**
    Search and return the Capability ID Address by FindCapNo.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Device Address for finding CapNo.
    @param FindCapNo - The Capability ID would be searched.
    @param CapPtr8 - Pointer to the Capability ID Address.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
GetLegCapStrucAddr (
    IN UINT64                   PciAddr64,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 )
{
    UINT8                       Buffer8;

    Buffer8 = PciRead8(PciAddr64+0x34); //get 0x50

    *CapPtr8 = Buffer8;
    if ((*CapPtr8 == 0) || (*CapPtr8 == 0xFF)) return EFI_NOT_FOUND;

    Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    
    while (Buffer8 != 0) {
        if (Buffer8 == FindCapNo) 
          return EFI_SUCCESS;

        Buffer8 = PciRead8(PciAddr64 | (UINT8)(*CapPtr8+1));
        *CapPtr8 = Buffer8;
        if (*CapPtr8 == 0) break;
        Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    }
    return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
GEN2DeEmphasisUpdateEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;
  PCIe_ENGINE_CONFIG                  *PcieEngine;
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;

  UINT32                          RootAddress;
  UINT8                           iPortNumber;
  UINT8                           CapPtr=0;
  UINT32                          PcieLinkCap;

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
          DEBUG((DEBUG_INFO, "GNBHANDLE InstanceId %x\n", GnbHandle->InstanceId));
          if ((GnbHandle->InstanceId == 0) || (GnbHandle->InstanceId == 3) || (GnbHandle->InstanceId == 6))
          {
              PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
              while (PcieWrapper != NULL) {
                  PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
                  while (PcieEngine != NULL) {
                      for (iPortNumber = 0; SlotDescription[iPortNumber].InstanceId != 0xff; iPortNumber++)
                      {
                          if ((PcieLibIsEngineAllocated (PcieEngine)) && (GnbHandle->InstanceId == SlotDescription[iPortNumber].InstanceId)
                            && (PcieEngine->Type.Port.PortData.DeviceNumber == SlotDescription[iPortNumber].DeviceNumber)
                            && (PcieEngine->Type.Port.PortData.FunctionNumber == SlotDescription[iPortNumber].FunctionNumber))
                          {
                              RootAddress = PCI_LIB_ADDRESS(GnbHandle->Address.Address.Bus,SlotDescription[iPortNumber].DeviceNumber,SlotDescription[iPortNumber].FunctionNumber,0x00);
                              GetLegCapStrucAddr(RootAddress,0x10,&CapPtr);
                              PcieLinkCap = PciRead32(RootAddress+CapPtr+0x0C);
                              if(((PcieLinkCap >> 4) & 0x3F) != 0x04) //X4
                                  continue;
                              SmnRegisterRMW (GnbHandle->Address.Address.Bus,
                                    PORT_SPACE (GnbHandle, PcieWrapper, (PcieEngine->Type.Port.PortId % 8), SMN_NBIO0PCIE0FUNC0_PCIE_LC_CNTL3_ADDRESS),
                                    (UINT32) ~PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_CNTL_MASK,
                                    (2 << PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_CNTL_OFFSET),
                                    0
                                    );
                              DEBUG((DEBUG_INFO, "Updating  for Bus %x Device %x Function %x\n", GnbHandle->Address.Address.Bus, PcieEngine->Type.Port.PortData.DeviceNumber, PcieEngine->Type.Port.PortData.FunctionNumber));
                          }

                      }
                      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
                  }
                  PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
              }
          }
          GnbHandle = GnbGetNextHandle (GnbHandle);
      }
  }

  return Status;
}

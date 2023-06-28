#include "OemPCIeACSInit.h"

#include <Setup.h>
#include "Protocol\PciIo.h"
#include "IndustryStandard\PciExpress21.h"
#include "IndustryStandard\Pci22.h"
#include "IndustryStandard\Pci30.h"
#include "PCIEXPRESS.h"


EFI_STATUS
LocateCapRegBlock(
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT8                CapID,
  OUT    UINT8                *PciExpressOffset,
  OUT    UINT8                *NextRegBlock)
/**

    GC_TODO: add routine description

    @param PciIo            - GC_TODO: add arg description
    @param CapID            - GC_TODO: add arg description
    @param PciExpressOffset - GC_TODO: add arg description
    @param NextRegBlock     - GC_TODO: add arg description

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_FOUND - GC_TODO: add retval description

**/
{
  UINT16  CapabilityID;
  UINT32  Temp;
  UINT8   CapabilityPtr;
  UINT16  CapabilityEntry;

  PciIo->Pci.Read (
            PciIo,
            EfiPciIoWidthUint32,
            PCI_CAPBILITY_POINTER_OFFSET,
            1,
            &Temp
            );

  CapabilityPtr = (UINT8)Temp;

  while ((CapabilityPtr >= 0x40) && (CapabilityPtr < 0x100)) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFC;
    PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint16,
               CapabilityPtr,
               1,
               &CapabilityEntry
               );

    CapabilityID = (UINT8) CapabilityEntry;

    if (CapabilityID == CapID) {
      *PciExpressOffset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (UINT8) ((CapabilityEntry >> 8) & 0xFC);
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (UINT8) ((CapabilityEntry >> 8) & 0xFC);
  }

  return EFI_NOT_FOUND;
}


/**

    GC_TODO: add routine description

    @param PciIo        - GC_TODO: add arg description
    @param CapID        - GC_TODO: add arg description
    @param Offset       - GC_TODO: add arg description
    @param NextRegBlock - GC_TODO: add arg description

    @retval EFI_SUCCESS   - GC_TODO: add retval description
    @retval EFI_NOT_FOUND - GC_TODO: add retval description

**/
EFI_STATUS
LocatePciExpressCapRegBlock (
  IN     EFI_PCI_IO_PROTOCOL  *PciIo,
  IN     UINT16               CapID,
  OUT    UINT32               *Offset,
  OUT    UINT32               *NextRegBlock
)
{
  UINT32  CapabilityPtr;
  UINT32  CapabilityEntry;
  UINT16  CapabilityID;

  CapabilityPtr = EFI_PCIE_CAPABILITY_BASE_OFFSET;

  while ((CapabilityPtr != 0) && (CapabilityPtr < 0x1000)) {
    //
    // Mask it to DWORD alignment per PCI spec
    //
    CapabilityPtr &= 0xFFC;
    PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               CapabilityPtr,
               1,
               &CapabilityEntry
               );

    CapabilityID = (UINT16) CapabilityEntry;
    // APTIOV_SERVER_OVERRIDE_RC_START : Check if Extended PciExpress Config. Space is accessible.
    if(CapabilityID == 0xFFFF) break;
    //APTIOV_SERVER_OVERRIDE_RC_END : Check if Extended PciExpress Config. Space is accessible.

    if (CapabilityID == CapID) {
      *Offset = CapabilityPtr;
      if (NextRegBlock != NULL) {
        *NextRegBlock = (CapabilityEntry >> 20) & 0xFFF;
      }

      return EFI_SUCCESS;
    }

    CapabilityPtr = (CapabilityEntry >> 20) & 0xFFF;
  }

  return EFI_NOT_FOUND;
}


EFI_STATUS 
PCIe_ACS_Init (
IN EFI_EVENT    Event,
IN VOID         *Context 
  )
{

  EFI_STATUS                  Status;
  UINTN                       HandleCount;
  EFI_HANDLE                  *HandleBuffer = NULL;
  UINTN                       Index;
  EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
  PCI_TYPE01                  PciConfigHeader;
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Device;
  UINTN                       Function;
  UINT8                       PciExpressOffset;
  UINT32                      AcsOffset;
  UINT16                      PciExpressCapabilityReg;
  UINT16                      RequiredAcsCap;
  UINT32                      AcsCapRegValue;
  UINT16                      AcsConRegValue;
  USRA_PCIE_ADDR_TYPE         *AcsDevArray = NULL;  
  USRA_ADDRESS                Address;
  SETUP_DATA                  SetupData;
  UINTN                       BufferSize = sizeof(SETUP_DATA);
  EFI_GUID                    gEfiSetupGuid = SETUP_GUID;  
  UINT8                       ACSControl = 0;

  TRACE((-1, "PCIe_ACS_Init\n"));
  PciExpressOffset = 0;
  AcsOffset        = 0;
  AcsCapRegValue   = 0;
  AcsConRegValue   = 0;
  RequiredAcsCap   =  ACS_SOURCE_VALIDATION | ACS_P2P_REQUEST_REDIRECT | ACS_P2P_COMPLETION_REDIRECT | ACS_UPSTREAM_FORWARDING;

  //
  // Locate all PciIo protocol
  //
  Status = pBS->LocateHandleBuffer (
               ByProtocol,
               &gEfiPciIoProtocolGuid,
               NULL,
               &HandleCount,
               &HandleBuffer
               );
  TRACE((-1, "Status=%d\n",Status));
  TRACE((-1, "HandleCount=%d\n",HandleCount));
  for (Index = 0; Index < HandleCount; Index ++) {      
    pBS->HandleProtocol (
          HandleBuffer[Index],
          &gEfiPciIoProtocolGuid,
          &PciIo
          );
    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint32,
                0,
                sizeof (PciConfigHeader) / sizeof (UINT32),
                &PciConfigHeader
                );
    if ((PciConfigHeader.Hdr.ClassCode[0] == 0x00 || PciConfigHeader.Hdr.ClassCode[0] == 0x01) && PciConfigHeader.Hdr.ClassCode[1] == 0x04 && PciConfigHeader.Hdr.ClassCode[2] == 0x06) {
      //
      // 060400h or 060401h indicates it's PCI-PCI bridge, get its bus number, device number and function number
      //
      PciIo->GetLocation (
              PciIo,
              &Segment,
              &Bus,
              &Device,
              &Function
              );
      USRA_PCIE_SEG_ADDRESS(Address, UsraWidth16, Segment, Bus, Device, Function, 0);
      TRACE((-1, "Bus=%X,Device=%X,Function=%X\n",Bus,Device,Function));
      if (PciConfigHeader.Hdr.Status == EFI_PCI_STATUS_CAPABILITY) {
              //
              // the bridge support Capability list and offset 0x34 is the pointer to the data structure
              //
              //
              // Detect if PCI Express Device
              //
              Status = LocateCapRegBlock (PciIo, EFI_PCI_CAPABILITY_ID_PCIEXP, &PciExpressOffset, NULL);

              if (Status == EFI_SUCCESS) {
                //
                // this bridge device is a PCI Express bridge
                // Check if it is downstream port of PCIE switch
                //
                PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint16,PciExpressOffset + EFI_PCI_EXPRESS_CAPABILITY_REGISTER,1,&PciExpressCapabilityReg);
                //
                // BIT 7:4 indicate Device/port type, 0110b indicates downstream port of PCI express switch
                //
                if ((PciExpressCapabilityReg & 0x00F0) == 0x60) {
                  //
                  // it is downstream port of PCI Express switch
                  // Look for ACS capability register in PCI express configuration space
                  //
                  Status = LocatePciExpressCapRegBlock (PciIo, EFI_PCIE_CAPABILITY_ID_ACS, &AcsOffset, NULL);
                  DEBUG((EFI_D_ERROR, "ACS capable port is B%x.D%x.F%x - ACS Cap offset - 0x%x\n", Bus, Device, Function, AcsOffset));

                  if (Status == EFI_SUCCESS) {
                    //
                    // Read ACS capability register
                    //                 
                    PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint32,AcsOffset + ACS_CAPABILITY_REGISTER,1,&AcsCapRegValue);
                    DEBUG((EFI_D_INFO, "Bus =%x, Device=%x, Function=%x, AcsCapRegValue = %x \n", Bus, Device, Function, AcsCapRegValue));
                    if ((AcsCapRegValue & RequiredAcsCap) == RequiredAcsCap) {                        
                        Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, NULL, &BufferSize, &SetupData);
                          if(!EFI_ERROR(Status)){
                              ACSControl = SetupData.ACSControl;
                          }
                          DEBUG((EFI_D_INFO, "SetupData.ACSControl = %d \n", ACSControl ));
                          if(ACSControl == 1){//Enabled ACS                       
                            PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                            DEBUG((EFI_D_INFO, "Write_Before AcsConRegValue = %x \n", AcsConRegValue ));
                        
                            AcsConRegValue |= (ACS_SOURCE_VALIDATION_ENABLE|ACS_P2P_REQUEST_REDIRECT_ENABLE|ACS_P2P_COMPLETION_REDIRECT_ENABLE|ACS_UPSTREAM_FORWARDING_ENABLE);
                            PciIo->Pci.Write( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                        
                            PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                            DEBUG((EFI_D_INFO, "Write_After AcsConRegValue = %x \n", AcsConRegValue ));
                          }else  {
                            PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                            DEBUG((EFI_D_INFO, "Write_Before AcsConRegValue = %x \n", AcsConRegValue ));
                              
                            AcsConRegValue = 0;
                            PciIo->Pci.Write( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                              
                            PciIo->Pci.Read ( PciIo,EfiPciIoWidthUint16,AcsOffset + ACS_CONTROL_REGISTER,1,&AcsConRegValue );
                            DEBUG((EFI_D_INFO, "Write_After AcsConRegValue = %x \n", AcsConRegValue ));
                          }
                    }
                        
     
                    }
                  }
	           }
            }
        }
    }
  
  
  //
    //Free the Handle buffer
    //
    if (HandleBuffer != NULL) {
      pBS->FreePool (HandleBuffer);
    }

  return EFI_SUCCESS;
}

EFI_STATUS OemPCIeACSInitEntry(
  IN  EFI_SYSTEM_TABLE *SystemTable
)
{ 
  EFI_STATUS      Status = EFI_SUCCESS;
  EFI_GUID        gEfiEndOfDxeGuid = { 0x2ce967a, 0xdd7e, 0x4ffc, { 0x9e, 0xe7, 0x81, 0xc, 0xf0, 0x47, 0x8, 0x80 } };
  EFI_EVENT       EndOfDxeEvent; 
  
  Status = pBS->CreateEventEx(
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK, 
                      PCIe_ACS_Init,
                      NULL, 
                      &gEfiEndOfDxeGuid,
                      &EndOfDxeEvent
                      );

  return Status;
}
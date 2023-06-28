#include "WriteCache.h"
#include <ProcessorBind.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <AmiDxeLib.h>
#include <AmiLib.h>
#include <Protocol/AmiAhciBus.h>
//#include <Guid/PchRcVariable.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Setup.h>

EFI_STATUS
ConfigureWriteCache ()
{
	EFI_STATUS                          Status;
	UINTN                               HandleCount;
	EFI_HANDLE                          *HandleBuffer;
	UINT8                               i;
	AMI_AHCI_BUS_PROTOCOL               *AhciBusInterface;
    SATA_DEVICE_INTERFACE               *SataDevInterface = NULL;
	DLINK                               *dlink;
	SETUP_DATA                          SetupData;
    COMMAND_STRUCTURE                   CommandStructure;
    UINTN                               BufferSize; 
    UINT8                               DetectController;
    EFI_GUID                            gEfiSetupGuid           = SETUP_GUID;
    
    BufferSize = sizeof(SETUP_DATA); 
    
    Status = pRS->GetVariable(
            L"Setup", 
            &gEfiSetupGuid,
            NULL, 
            &BufferSize, 
            &SetupData
            );
    
    if (EFI_ERROR(Status))
       return Status;
    DetectController = SetupData.VolatileWriteCache;
    Status = pBS->LocateHandleBuffer (
                                       ByProtocol,
                                       &gAmiAhciBusProtocolGuid,
                                       NULL,
                                       &HandleCount,
                                       &HandleBuffer
                                        );

        if(EFI_ERROR(Status) || HandleCount == 0 ) {
            return EFI_INVALID_PARAMETER;
        }
            for (i = 0; i < HandleCount; i++) {
                   Status = pBS->HandleProtocol (HandleBuffer[i], &gAmiAhciBusProtocolGuid, (VOID**)&AhciBusInterface);
             if (EFI_ERROR(Status)) {
                   continue;
               }
             dlink = AhciBusInterface->SataDeviceList.pHead; 
                     if (dlink){ 
                          do {
                                 SataDevInterface = OUTTER(dlink, SataDeviceLink, SATA_DEVICE_INTERFACE);
                             if (DetectController == 1){
                                 ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE));
                                 CommandStructure.Command = SET_FEATURE_COMMAND;
                                 CommandStructure.Features = VWC_ENABLE;
                                 DEBUG ((EFI_D_ERROR, " WC enable cmd execute\n"));
                                 AhciBusInterface->ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                               }
                             else{
                                  ZeroMemory(&CommandStructure, sizeof(COMMAND_STRUCTURE));
                                  DEBUG ((EFI_D_ERROR, " WC disable cmd execute\n"));
                                  CommandStructure.Command = SET_FEATURE_COMMAND;
                                  CommandStructure.Features = VWC_DISABLE;
                                  AhciBusInterface->ExecuteNonDataCommand (SataDevInterface, CommandStructure);
                             }
                                  dlink = dlink-> pNext;
                             } while (dlink);
                         }
                    }        
     return Status;
}


/**
    Write cache dxe driver entry point
    @param    ImageHandle 
    @param    SystemTable 
    @retval   EFI_STATUS
**/

EFI_STATUS
WriteCacheEntryPoint(
    IN EFI_HANDLE         ImageHandle,   
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
  
    EFI_STATUS  Status = EFI_SUCCESS;
    InitAmiLib(ImageHandle, SystemTable);
    Status = ConfigureWriteCache ();          
    
  return Status;
}

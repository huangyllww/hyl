#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include "Library\UefiLib.h"
#include <Protocol/AmdApcbProtocol.h>
#include "Library/AmdCbsVariable.h"
#include "Library/AmdCbsSetupLib.h"
//<<duanjun 20210721 sce tool fail to change setting +>
VOID SceToolFlashApcb ( )
{
  CBS_CONFIG                  CbsSetupData;
  UINTN                       VariableSize;
  UINT32                      VariableAttribute;
  UINTN                       Flag = 0;
  EFI_STATUS                  Status;
  AMD_APCB_SERVICE_PROTOCOL   *ApcbDxeServiceProtocol;
  UINT8                       data8 = 0;
   
  
  VariableSize = 1;
  Status = pRS->GetVariable(
                       L"SceSetBios", 
                       &gCbsSystemConfigurationGuid,
                       &VariableAttribute,
                       &VariableSize,
                       &data8);
 

  if ( Status == EFI_SUCCESS ) {
      DEBUG ((DEBUG_ERROR , "xxx SceSetBios found\n"));
      VariableSize = sizeof(data8);
      Status = pRS->SetVariable (L"SceSetBios", &gCbsSystemConfigurationGuid, 0, 0, 0); 
  } else {
      return; 
  }
           
  
  DEBUG ((DEBUG_ERROR , "xxx sce ApcbDxeServiceCallBack Startd\n"));
  Status = pBS->LocateProtocol(
                &gAmdApcbDxeServiceProtocolGuid,
                NULL,
                (VOID **) &ApcbDxeServiceProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR , "xxx sce Locate ApcbDxeServiceProtocol failed\n"));
    return;
  }

  VariableSize = sizeof(CBS_CONFIG);
  Status = pRS->GetVariable(
                       L"AmdSetup", 
                       &gCbsSystemConfigurationGuid,
                       &VariableAttribute,
                       &VariableSize,
                       &CbsSetupData);

  if ( Status == EFI_SUCCESS ) {
      AmdSaveCbsConfigData((UINT8*)&CbsSetupData);
      DEBUG ((DEBUG_ERROR , "xxx sce EfiResetCold Startd\n"));
      pRS->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
} 
//<<duanjun 20210721 sce tool fail to change setting ->

VOID OemReadyToBootNotify(
  IN EFI_EVENT Event, 
    IN VOID *Context)
{   
  	SceToolFlashApcb();  //<<duanjun 20210721 sce tool fail to change setting +>
    return;
}

EFI_STATUS
OemSceSettingSync(
  IN  EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS    Status=EFI_SUCCESS;

  Status = EfiNamedEventListen (
               &gAmiLegacyBootProtocolGuid,
               TPL_CALLBACK,
               OemReadyToBootNotify,
               NULL,
               NULL );
  if(EFI_ERROR (Status))
    return Status;

  Status = EfiNamedEventListen (
               &gAmiTseEventBeforeBootGuid,
               TPL_CALLBACK,
               OemReadyToBootNotify,
               NULL,
               NULL );
  
  return Status;
}
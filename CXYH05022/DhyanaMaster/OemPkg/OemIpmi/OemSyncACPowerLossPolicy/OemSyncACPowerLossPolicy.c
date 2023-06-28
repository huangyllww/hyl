#include "OemSyncACPowerLossPolicy.h"
#include "Library/Family/0x17/ZP/External/Resource/AmdCbsVariable.h"

//IPMI_OPTION_TABLE gOptTable[] = {
//	{ ONE_OF_TWO_SELECTOR, 0x1/*GROUP_ONE_OF_TWO*/, 0x1, ONE_OF_TWO_COUNT, 0x1, 0x5},
//	{ ONE_OF_FOUR_SELECTOR, 0x2/*GROUP_ONE_OF_FOUR*/, 0x2, ONE_OF_FOUR_COUNT, 0x3, 0xa},
//	{ ONE_OF_SIXTEEN_SELECTOR, 0x4/*GROUP_ONE_OF_SIXTEEN*/, 0x4, ONE_OF_SIXTEEN_COUNT, 0xf, 0x14}
//};

#define OPT_TABLE_LENGTH        3

//
// Extern Variables
//
extern  EFI_SYSTEM_TABLE        *gST;
extern  EFI_BOOT_SERVICES       *gBS;
extern  EFI_RUNTIME_SERVICES    *gRT;

EFI_IPMI_TRANSPORT  *mIpmiTransportProtocol;
EFI_GUID    gOemNvramUpdateGuid       = OEM_NVRAM_UPDATE_GUID;
/*
EFI_GUID    gEfiIpmiBootGuid            = IPMI_BOOT_GUID;
EFI_GUID    gEfiBdsEventBeforeSetupGuid = EFI_BDS_EVENT_BEFORE_SETUP_GUID;
EFI_GUID    gOemNvramUpdateGuid       = OEM_NVRAM_UPDATE_GUID;
//Machong Notify OemIPMI Module
EFI_GUID    gEfiRtcPowerLossProtocolGuid = RTC_POWER_LOSS_PROTOCOL_GUID;
//Machong Notify OemIPMI Module
 */
 
/**************************************************************************
 * Function Name : MemCopy  *
 * Parameters:   VOID   * Dest  *
 *               VOID   * Src  *
 *               UINTN  numBytes  *
 **************************************************************************/
VOID
MemCopy (
  UINT8   *dest,
  UINT8   *src,
  UINTN  numBytes
  )
{
    while (numBytes--) {
        *(dest++) = *(src++);
    }
}


//<Wangyia001-20170425 change for AMD platform +>
EFI_STATUS
GetACPowerLossPolicyFromIPMI (
  CBS_CONFIG                            *CbsConfig
  )
{
  UINT8 Temp[10];
  UINT8 ResponseSize;
  EFI_STATUS Status;
  
  ResponseSize = 10;  
  Status = mIpmiTransportProtocol->SendIpmiCommand (
                                   mIpmiTransportProtocol,	
                                   IPMI_NETFN_CHASSIS,
                                   0,
                                   IPMI_CHASSIS_GET_STATUS,//Get Chassis Status Command
                                   NULL,
                                   0,
                                   (UINT8*) Temp,
                                   &ResponseSize
                                   );
  DEBUG((-1, "Get Chassis Status Command Status = %x\n", Status));
  if (EFI_ERROR (Status)) {
      return Status;
  }
  
  DEBUG((-1, "Get Chassis Status Command  Temp[0] =  %x, Status=%d\n", Temp[0], Status));

  switch ( (Temp[0]>>5)&0x3 ) {
    case 0x0:
      CbsConfig->CbsCmnFchSystemPwrFailShadow = 1;  //StateAfterG3 = 1  s5
      break;
    case 0x1:
      CbsConfig->CbsCmnFchSystemPwrFailShadow = 0xFF;  //last status //<lubl001_20161123 +>
      break;
    case 0x2:
      CbsConfig->CbsCmnFchSystemPwrFailShadow = 0;  //StateAfterG3 = 1  s0
      break;
  }

  DEBUG((-1, "CbsCmnFchSystemPwrFailShadow =  %x\n", CbsConfig->CbsCmnFchSystemPwrFailShadow));
  
  return EFI_SUCCESS;
} 


//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   SetIpmiSetupConfig
//
// Description: Check for the Ipmi OEM Setup Configuration and Set 
// certain variable bmc specified
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
SetIpmiSetupConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BufferSize;
  CBS_CONFIG                            CbsConfig;
  UINTN                                 Size = sizeof(CBS_CONFIG);
  CBS_CONFIG                            *OriginCbsConfigData;
  UINT32                                Attributes;


  DEBUG((-1, "enter SetIpmiSetupConfig\n"));
  
  Status = gBS->LocateProtocol (
                  &gEfiDxeIpmiTransportProtocolGuid,
                  NULL,
                  &mIpmiTransportProtocol
                  );

  if (EFI_ERROR (Status)) {
      return Status;
  }

  DEBUG((-1, "Status = %r \n", Status));

  MemSet( &CbsConfig, sizeof (CBS_CONFIG), 0);
  
  BufferSize = sizeof (CBS_CONFIG);
  Status = gRT->GetVariable (L"AmdSetup",&gCbsSystemConfigurationGuid, &Attributes, &BufferSize, &CbsConfig);

  DEBUG((-1, "Get variable Status = %r \n", Status));
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gBS->AllocatePool(EfiBootServicesData, Size, &OriginCbsConfigData);
  MemCopy((UINT8 *)OriginCbsConfigData, (UINT8 *)&CbsConfig, Size);
  GetACPowerLossPolicyFromIPMI(&CbsConfig);

  if(OriginCbsConfigData->CbsCmnFchSystemPwrFailShadow != CbsConfig.CbsCmnFchSystemPwrFailShadow){
    Status = gRT->SetVariable(
                    L"AmdSetup", 
                    &gCbsSystemConfigurationGuid,
                    Attributes,
                    sizeof (CBS_CONFIG),
                    &CbsConfig
                    );
  }
 
  gBS->FreePool(OriginCbsConfigData);
  return Status;
}

//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   SyncIpmiSetupConfig
//
// Description: Check for the Ipmi OEM Setup Configuration and Set 
// certain variable bmc specified
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
SyncIpmiSetupConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BufferSize;
  CBS_CONFIG                            CbsConfig;
  UINT32                                Attributes;
  UINT8                                 ResponseSize;
  UINT8                                 Temp[2] = {0};
  
  DEBUG((-1, "enter  SyncIpmiSetupConfig\n"));

  if(mIpmiTransportProtocol == NULL) 
  {
    Status = gBS->LocateProtocol (
                    &gEfiDxeIpmiTransportProtocolGuid,
                    NULL,
                    &mIpmiTransportProtocol
                    );
    if(EFI_ERROR(Status))
      return  Status;
  }

  DEBUG((-1, "Status = %r \n", Status));
  
  MemSet( &CbsConfig, sizeof (CbsConfig), 0);
  
  BufferSize = sizeof (CbsConfig);

  Status  = gRT->GetVariable ( L"AmdSetup", &gCbsSystemConfigurationGuid, &Attributes, &BufferSize, &CbsConfig);
  if (EFI_ERROR(Status)) 
    return Status;
  
  switch ( CbsConfig.CbsCmnFchSystemPwrFailShadow ) {
    case 0x0:
      Temp[0] |= 2;   //base on IPMI spec 28.8, bit[2:0]=010b is s0
      break;
    case 0x1:
      Temp[0] &= ~0x7;   //base on IPMI spec 28.8, bit[2:0]=000b  is s5 //<lubl001_20161123 >
      break;
    case 0xFF:    //base on bios setting, if 0xff = last status  //<lubl001_20161123 >
      Temp[0] |= 0x1;
        break;
  } 

  Status = mIpmiTransportProtocol->SendIpmiCommand (
                                   mIpmiTransportProtocol,	
                                   IPMI_NETFN_CHASSIS,
                                   0,
                                   IPMI_CHASSIS_SET_POWER_RESTORE_POLICY,//Set Power Restore Policy Command
                                   (UINT8*) Temp,
                                   1,
                                   (UINT8*) Temp,
                                   &ResponseSize
                                   );
  DEBUG((-1, "Set Power Restore Policy Command = %r \n", Status));
  if (EFI_ERROR (Status)) {
      return Status;
  }
  return  Status;
}
//<Wangyia001-20170425 change for AMD platform ->

//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   OemSetupIPMIEntryPoint
//
// Description: Entry point for IpmiBoot
//
// Input: IN EFI_HANDLE ImageHandle - Handle for the image of this driver
//        IN EFI_SYSTEM_TABLE *SystemTable - Pointer to the EFI System Table
//
// Output:  EFI_SUCCESS - Protocol Callback is successfully registered
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
OemSetupIPMIEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  NewHandle;
  
  VOID        *NotifyRegistration;

  NewHandle = NULL;

  InitAmiLib (ImageHandle, SystemTable);
  
  DEBUG((-1, "enter OemSetupIPMIEntryPoint\n"));
  //
  // Register the event to Enable IPMI force boot.
  //
  EfiCreateProtocolNotifyEvent (
      &gEfiDxeIpmiTransportProtocolGuid,
      TPL_CALLBACK,
      SetIpmiSetupConfig,
      NULL,
      &NotifyRegistration
      );
  
  Status =  EfiNamedEventListen (
              &gOemNvramUpdateGuid,
              EFI_TPL_CALLBACK,
              SyncIpmiSetupConfig,
              NULL,
              NULL
              );
  return Status;
}


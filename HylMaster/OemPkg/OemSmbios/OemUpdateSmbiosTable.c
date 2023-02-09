
//**********************************************************************
// $Header: OemUpdateSmbiosTable.c
//
// $Function: SMBIOS table is updated here
//
// $Revision:01
//
// $Date:  Lizhq 2017/03/23
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Smbios.h>
#include <Protocol/Smbios.h>
#include <Library\UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <OemIpmiNetFnDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>

EFI_SMBIOS_PROTOCOL      *mEfiSmbiosProtocol;

#define MAX_SMBIOS_STRING_SIZE  0x40

//<Xuran001-20180609-Update Type39 table+>
#if defined (SYSTEM_POWER_SUPPLY_INFO) && (SYSTEM_POWER_SUPPLY_INFO == 1) && defined (NUMBER_OF_POWER_SUPPLY)
/**
  Update the SMBIOS type 39 by IPMI PSU command.

**/
EFI_STATUS
UpdateSmbiosType39Table(
  VOID
)
{
  EFI_STATUS                          Status;
  SMBIOS_TABLE_TYPE39                 *Type39Structure;
  EFI_SMBIOS_HANDLE                   SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  EFI_SMBIOS_TYPE                     SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
  UINT8                               PsuIndex, StrIndex;
  UINT8                               DataBuffer[4], CompletionCode;
  UINT8                               ResponseData[0xFF];
  UINT8                               ResponseDataSize;
  PACKED_OEM_IPMI_PSU_INFO          *PsuInfo;
  CHAR8                               IpmiPsuStr[7][MAX_SMBIOS_STRING_SIZE] = {0};
  PACKED_OEM_IPMI_PSU_MODULE_STATE  *PsuModuleState;
  UINTN                               StrNumber;

  DEBUG ((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));

  for(PsuIndex = 0; PsuIndex < NUMBER_OF_POWER_SUPPLY; PsuIndex++){
    // Read Type 39 structure
    Status = mEfiSmbiosProtocol->GetNext( mEfiSmbiosProtocol, &SmbiosHandle, &SmbiosType, (EFI_SMBIOS_TABLE_HEADER **) &Type39Structure, NULL);
    DEBUG ((EFI_D_INFO, "SmbiosProtocol Get Next Status: %r \n", Status));
    DEBUG ((EFI_D_INFO, "SmbiosProtocol Get Next SmbiosHandle: 0x%X \n", SmbiosHandle));
    DEBUG ((EFI_D_INFO, "  Type39Structure: 0x%p \n", Type39Structure));
    if (EFI_ERROR(Status))
      return Status;

    if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED)
      break;

    //Use IPMI CMD get PSU Info
    ZeroMem(DataBuffer, sizeof(DataBuffer));
    ResponseDataSize = sizeof(ResponseData);
    ZeroMem(ResponseData, ResponseDataSize);
    DataBuffer[0] = SreverRespositorySubTypePsu;
    DataBuffer[1] = PsuIndex;
    DEBUG ((EFI_D_INFO, "PSU Index : %d\n", PsuIndex));

    Status = SendIpmiCommand (
        NETFN_OEM,
        BMC_LUN,
        CMD_OEM_GET_SERVER_REPOSITORY_INFO,
        (UINT8 *)DataBuffer,
        sizeof(DataBuffer),
        ResponseData,
        &ResponseDataSize,
        &CompletionCode
      );
    DEBUG ((EFI_D_INFO, "SendIpmiCommand Status:%r \n", Status));
    DEBUG ((EFI_D_INFO, "CompletionCode: 0x%X \n", CompletionCode));
    DEBUG ((EFI_D_INFO, "ResponseDataSize 0x%X \n", ResponseDataSize));
    if(EFI_ERROR(Status))
      return Status;
    PsuInfo = (PACKED_OEM_IPMI_PSU_INFO *)(ResponseData + 1);
    DEBUG ((EFI_D_INFO, "\n PsuPresent: %d \n", PsuInfo->PsuPresent));
    DEBUG ((EFI_D_INFO, "PsuRatedPower: %d \n", PsuInfo->PsuRatedPower));
    DEBUG ((EFI_D_INFO, "MaxPowerCapacity 0x%X \n", Type39Structure->MaxPowerCapacity));
    
    ZeroMem(IpmiPsuStr, sizeof(IpmiPsuStr));
    if(PsuInfo->PsuPresent == 1){
      AsciiStrnCpyS(IpmiPsuStr[0], sizeof(IpmiPsuStr[0]), PsuInfo->PsuLocation, sizeof(PsuInfo->PsuLocation));        //Location
      AsciiStrnCpyS(IpmiPsuStr[1], sizeof(IpmiPsuStr[1]), PsuInfo->PsuModel, sizeof(PsuInfo->PsuModel));              //DeviceName
      AsciiStrnCpyS(IpmiPsuStr[2], sizeof(IpmiPsuStr[2]), PsuInfo->PsuVendor, sizeof(PsuInfo->PsuVendor));            //Manufacturer
      AsciiStrnCpyS(IpmiPsuStr[3], sizeof(IpmiPsuStr[3]), PsuInfo->PsuSn, sizeof(PsuInfo->PsuSn));                    //SerialNumber
      //To update by DMI edit tool
//    AsciiStrnCpyS(IpmiPsuStr[4], sizeof(IpmiPsuStr[4]), SmBiosDefaultString, AsciiStrLen(SmBiosDefaultString));     //AssetTabNumber
//    AsciiStrnCpyS(IpmiPsuStr[5], sizeof(IpmiPsuStr[5]), SmBiosDefaultString, AsciiStrLen(SmBiosDefaultString));     //ModelPartNumber
      AsciiStrnCpyS(IpmiPsuStr[6], sizeof(IpmiPsuStr[6]), PsuInfo->PsuRevision, sizeof(PsuInfo->PsuRevision));        //RevisionLevel

      Type39Structure->MaxPowerCapacity = PsuInfo->PsuRatedPower;
    }

    //Use IPMI CMD get PSU State
    ZeroMem(DataBuffer, sizeof(DataBuffer));
    ResponseDataSize = sizeof(ResponseData);
    ZeroMem(ResponseData, ResponseDataSize);
    DataBuffer[0] = SreverRespositorySubTypePsu;
    DataBuffer[1] = PsuIndex;
    DEBUG ((EFI_D_INFO, "PSU Index : %d\n", PsuIndex));

    Status = SendIpmiCommand (
        NETFN_OEM,
        BMC_LUN,
        CMD_OEM_GET_MODULE_STATE,
        (UINT8 *)DataBuffer,
        sizeof(DataBuffer),
        ResponseData,
        &ResponseDataSize,
        &CompletionCode
      );
    DEBUG ((EFI_D_INFO, "SendIpmiCommand Status:%r \n", Status));
    DEBUG ((EFI_D_INFO, "CompletionCode: 0x%X \n", CompletionCode));
    DEBUG ((EFI_D_INFO, "ResponseDataSize 0x%X \n", ResponseDataSize));
    if(EFI_ERROR(Status))
      return Status;
    PsuModuleState = (PACKED_OEM_IPMI_PSU_MODULE_STATE *)ResponseData;
    DEBUG ((EFI_D_INFO, "Present 0x%X \n", PsuModuleState->Present));
    DEBUG ((EFI_D_INFO, "CriticalLevel 0x%X \n", PsuModuleState->CriticalLevel));
   *((UINT16 *)&Type39Structure->PowerSupplyCharacteristics) = 0;
    if(PsuModuleState->Present == 1){
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = 1;
      Type39Structure->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 4;    //Auto-switch
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyType = 4;            //Switching
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyPresent = 1;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyUnplugged = 0;

      switch(PsuModuleState->CriticalLevel){
        case 0: //Normal
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 3;  //OK
          break;
        case 1: //Not critical
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 4;  //Non-critical
          break;
        case 2: //Critical
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 5;  //critical
          break;
        case 3: //Urgency
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 5;  //critical
          break;
        default:
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 2;  //unknown
      }

    }else{
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = 1;
      Type39Structure->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 2;    //unknown
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyType = 2;            //unknown
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyPresent = 0;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyUnplugged = 1;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 2;          //unknown
    }
    DEBUG ((EFI_D_INFO, "PowerSupplyCharacteristics: 0x%X \n", *((UINT16 *)&Type39Structure->PowerSupplyCharacteristics)));

    //
    //Update the strings
    //
    for(StrIndex = 0; StrIndex < 7; StrIndex++){
      if(IpmiPsuStr[StrIndex][0] == 0) continue;
      StrNumber = StrIndex + 1;
      DEBUG ((EFI_D_INFO, "Str[%d]: %a \n", StrNumber, IpmiPsuStr[StrIndex]));
      Status = mEfiSmbiosProtocol->UpdateString ( mEfiSmbiosProtocol, &SmbiosHandle, &StrNumber, IpmiPsuStr[StrIndex]);
    }

  }

  DEBUG ((EFI_D_INFO, "%a End...\n", __FUNCTION__));
  return Status;
}
#endif
//<Xuran001-20180609-Update Type39 table->
/**
  Register callback function upon gEfiSmbiosProtocolGuid to update SMBIOS information

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
UpdateSmbiosTables (
    IN  EFI_EVENT Event,
    IN  VOID      *Context
)
{
  EFI_STATUS       Status;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));
  //Close the event if it is present
  if (Event != NULL)
      pBS->CloseEvent(Event);
    
  Status = pBS->LocateProtocol( &gEfiSmbiosProtocolGuid, NULL, &mEfiSmbiosProtocol);
  if (EFI_ERROR(Status))
      return;

//<Xuran001-20180609-Update Type39 table+>
#if defined (SYSTEM_POWER_SUPPLY_INFO) && (SYSTEM_POWER_SUPPLY_INFO == 1) && defined (NUMBER_OF_POWER_SUPPLY)
  //Update SMBIOS Type 39 Structures
  Status = UpdateSmbiosType39Table();
  DEBUG((EFI_D_ERROR, "UpdateSmbiosType39Table Status: %r\n", Status));
#endif
//<Xuran001-20180609-Update Type39 table->
  DEBUG((EFI_D_ERROR, __FUNCTION__" End\n"));
  return;
}

/**
  This function is the entry point for this driver.  And initializes the notify event of update SMBIOS table

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS OemSmbiosEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  VOID                *UpdateSmbiosTableReg;
  EFI_EVENT           UpdateSmbiosTableEvent;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));

  InitAmiLib(ImageHandle, SystemTable);

  UpdateSmbiosTableEvent = EfiCreateProtocolNotifyEvent (
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      UpdateSmbiosTables,
      NULL,
      &UpdateSmbiosTableReg );

  DEBUG((EFI_D_ERROR, __FUNCTION__" End\n"));
  return Status;
}

#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include "Setup.h"

EFI_GUID gEfiSetupGuid = SETUP_GUID;

EFI_STATUS
SyncSeupSetting(
  IN  EFI_SYSTEM_TABLE *SystemTable
)
{
  SETUP_DATA      SetupData;
  UINTN           VariableSize = sizeof(SETUP_DATA);
  EFI_STATUS      Status;
  UINT32          Attributes;

  Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, &Attributes, &VariableSize, &SetupData);
  if(EFI_ERROR(Status))
    return Status;

  
  PcdSet8(PCIeMaxPayloadSetting,SetupData.MaxPayload); //mizl-20200705-Mantis:0039264 FIO Test Fail+>

  return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols
#include <Protocol\PciIo.h>
#include <Uefi\UefiBaseType.h>
#include <Token.h>
#include "Setup.h"
#include "Library/PcdLib.h" //Mizl-Mantis:0040070- When BMC web sets PXE as the first boot item, it cannot enter the installation UEFI system interface+>
// External data definitions
EFI_GUID gEfiSetupGuid = SETUP_GUID;


/**
    This function contain all the initializes code in DXE Stage for SyncSetupSetting.

    @param ImageHandle - Image Handle.
    @param SystemTable - Pointer to the System table.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
SyncSetupSetting_Entry (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    SETUP_DATA      SetupData;
    UINTN           VariableSize = sizeof(SETUP_DATA);
    EFI_STATUS      Status;
    UINT32          Attributes;

    InitAmiLib(ImageHandle, SystemTable);

    Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, &Attributes, &VariableSize, &SetupData);
    if(EFI_ERROR(Status))
        return Status;

    DEBUG((EFI_D_ERROR, "[%a][%d]VideoOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.VideoOpRom));
    DEBUG((EFI_D_ERROR, "[%a][%d]PxeOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.PxeOpRom));
    DEBUG((EFI_D_ERROR, "[%a][%d]MassStorageOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.MassStorageOpRom));
    DEBUG((EFI_D_ERROR, "[%a][%d]Video:%x\n\n",__FUNCTION__,__LINE__,SetupData.OpROMEN[VIDEOINDX]));
    DEBUG((EFI_D_ERROR, "[%a][%d]PXE:%x\n\n",__FUNCTION__,__LINE__,SetupData.OpROMEN[PXEINDX]));
    DEBUG((EFI_D_ERROR, "[%a][%d]Mass Storage:%x\n\n",__FUNCTION__,__LINE__,SetupData.OpROMEN[MASSINDX]));

    SetupData.VideoOpRom = SetupData.OpROMEN[VIDEOINDX];
    SetupData.PxeOpRom = SetupData.OpROMEN[PXEINDX];
    SetupData.MassStorageOpRom = SetupData.OpROMEN[MASSINDX];
    SetupData.OldOpRom = SetupData.OpROMEN[OLDINDX]; //#Mizl-20200617- PXE/Storage => Both Mode Video/Other =>Legacy In Default->
    
    PcdSet8(PXEBootPolicy,SetupData.OpROMEN[PXEINDX]); //Mizl-Mantis:0040070- When BMC web sets PXE as the first boot item, it cannot enter the installation UEFI system interface+>

    Status = pRS->SetVariable(L"Setup",&gEfiSetupGuid,Attributes,VariableSize,&SetupData);
    if(EFI_ERROR(Status))
        return Status;

    DEBUG((EFI_D_ERROR, "[%a][%d]VideoOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.VideoOpRom));
    DEBUG((EFI_D_ERROR, "[%a][%d]PxeOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.PxeOpRom));
    DEBUG((EFI_D_ERROR, "[%a][%d]MassStorageOpRom:%x\n\n",__FUNCTION__,__LINE__,SetupData.MassStorageOpRom));

    PcdSet8(PCIeMaxPayloadSetting,SetupData.MaxPayload); //mizl-20200705-Mantis:0039264 FIO Test Fail+>
    
    return EFI_SUCCESS;
}

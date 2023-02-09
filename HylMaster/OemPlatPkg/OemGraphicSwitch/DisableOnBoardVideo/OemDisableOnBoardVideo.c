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
// Name:  OemDisableOnBoardVideo.c 
//<AMI_FHDR_END>
//**********************************************************************

#include <Library/PeiServicesLib.h>
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include "AmiCspLib.h"
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Include/Setup.h>
#include <Library/PcdLib.h>

EFI_GUID  gSetupGuid              = SETUP_GUID;
#define   GPIO_BANK_BASE          0x1500
#define   BIOS_DIS_BMCGPU_GPIO_OFFSET  ACPI_MMIO_BASE + GPIO_BANK_BASE + (BIOS_DIS_BMCGPU_PIN << 2)

EFI_STATUS EFIAPI OemDisableOnBoardVideo(
    IN EFI_PEI_FILE_HANDLE        FileHandle,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS        Status;
    SETUP_DATA        SetupData;
    UINT32            SetupDataAttributes = 0;
    UINTN             SetupDataSize = sizeof(SETUP_DATA);
    UINT8             Value;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    
    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status); 

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &gSetupGuid, &SetupDataAttributes,
                                          &SetupDataSize, &SetupData);
    if (EFI_ERROR(Status)) return Status;

    if ((SetupData.OnExtVgaSelect == 0) && (SetupData.OnBoardVideoEn == 0)){ //'Onboard or External, External first' , and OnBoardVideoEn is 0
#if defined(ONBOARD_VIDEO_DISABLE_BY_GPIO) && (ONBOARD_VIDEO_DISABLE_BY_GPIO == 1)
    //get current gpio value
    Value = MmioRead8(BIOS_DIS_BMCGPU_GPIO_OFFSET + 2);
    DEBUG ((DEBUG_INFO, "Value = %x\n",Value));
                            
    //the highest two bits are reserved
    Value = Value & 0xC7; 
                            
    //pull up
    Value |= 0x10; 
                            
    //output enable
    Value |= 0x80;
                            
    //output state
    Value &= 0xBF;
                            
    MmioWrite8(BIOS_DIS_BMCGPU_GPIO_OFFSET + 2, Value);
                            
    //Set Multi-function IO pin function select of GPIO
    MmioWrite8 (ACPI_MMIO_BASE + IOMUX_BASE + BIOS_DIS_BMCGPU_PIN, 0);
                            
    Value = MmioRead8 (BIOS_DIS_BMCGPU_GPIO_OFFSET + 2);
    DEBUG ((DEBUG_INFO, "Value = %x\n",Value));
                            
    DEBUG ((DEBUG_INFO, "Disable OnBoard Video Complete !!\n"));
#endif
#if defined(ONBOARD_VIDEO_DISABLE_BY_DXIO) && (ONBOARD_VIDEO_DISABLE_BY_DXIO == 1)
    Value = 0;
    DEBUG((-1, "ORG PcdOnBoardVideoEn %x\n", PcdGet8(PcdOnBoardVideoEn)));
    PcdSet8(PcdOnBoardVideoEn, Value);
#endif
    }
    
    return Status;
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

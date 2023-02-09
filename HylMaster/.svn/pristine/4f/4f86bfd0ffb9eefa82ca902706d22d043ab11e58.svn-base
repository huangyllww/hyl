//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SbSetup.c
    South Bridge Setup Routines

**/
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol\PciIo.h>
#include <Protocol\DevicePath.h>
#include "AmiCspLib.h"
#include "sb.h"
#include <Token.h>

#include <Library/PcdLib.h>
#include <Guid/AmdCbsConfig.h>

static EFI_GUID gSetupGuid = SETUP_GUID;

// The device path refer to CbsSetupDxe.c
//  to identify if AMD-CBS-SETUP exist.
struct {
    VENDOR_DEVICE_PATH           VendorDevicePath;
    EFI_DEVICE_PATH_PROTOCOL     End;
} gCbsVendorDp = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    CBS_SYSTEM_CONFIGURATION_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
    Init South Bridge strings

    @param HiiHandle 
    @param Class 

    @retval VOID

**/
VOID InitSBStrings(IN EFI_HII_HANDLE HiiHandle, IN UINT16 Class)
{
    if (Class == ADVANCED_FORM_SET_CLASS) {

        EFI_STATUS                  Status;
        UINT8                       Var;
        EFI_DEVICE_PATH_PROTOCOL    *Dp;
        EFI_HANDLE                  Handle;

        UINTN                       SetupSize;
        SETUP_DATA                  SetupData;
        UINT32                      VarAttr;

        //
        // To identify if AMD-CBS-SETUP exist.
        //
        Dp = DuplicateDevicePath (&gCbsVendorDp.VendorDevicePath.Header);
        Status = pBS->LocateDevicePath(
                        &gEfiHiiConfigAccessProtocolGuid,
                        &Dp,
                        &Handle);
        if (EFI_ERROR(Status)) Var = 0;
        else Var = 1;

        SetupSize = sizeof(SETUP_DATA);
        Status = pRS->GetVariable(L"Setup", &gSetupGuid, &VarAttr, &SetupSize, &SetupData);
        if (!EFI_ERROR(Status)) {
            SetupData.AmdCbsPresent = Var;
            pRS->SetVariable(L"Setup", &gSetupGuid, VarAttr, sizeof(SETUP_DATA), &SetupData);
        }

    }
}



//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

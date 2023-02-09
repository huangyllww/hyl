#include "OemShell.h"


BOOLEAN ShowShellDevice(void)
{   
    EFI_STATUS          Status = EFI_SUCCESS;
    SETUP_DATA          SetupData;
    UINTN               BufferSize = sizeof(SETUP_DATA);
    EFI_GUID            gEfiSetupGuid           = SETUP_GUID;

    Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, NULL, &BufferSize, &SetupData);
    if (EFI_ERROR (Status))
        return FALSE;

    if(SetupData.UEFIShellDisableSelect == 1)
        return TRUE;

    return FALSE;
}


static int WStrcmp( CHAR16 *string1, CHAR16 *string2 ) {
    while(*string1 && *string2 ) {
        if(*string1!=*string2) return 1;
        string1++;string2++;
    }
    return 0;
}

/**
    Based on the Setup setting to determin wheter to hide the device or not.
**/
VOID FilterShellDevice(){
    DLINK               *Link;
    BOOT_OPTION         *Option;
    CHAR16              BootStr[9];
    EFI_STATUS          Status;
    //UINT8               NVMEBootSwitch =0;

    DEBUG((DEBUG_INFO,"\r%a",__FUNCTION__));

    if(ShowShellDevice() == TRUE)
        return;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){

        if(WStrcmp(Option->Description,L"UEFI: Built-in EFI Shell") == 0)
        {
            Swprintf(BootStr,L"Boot%04X",Option->BootOptionNumber);
            Status = pRS->SetVariable(
                    BootStr, &EfiVariableGuid,
                    BOOT_VARIABLE_ATTRIBUTES, 0, NULL);

            DeleteBootOption(BootOptionList,Option);
        }

    }

    return;
}

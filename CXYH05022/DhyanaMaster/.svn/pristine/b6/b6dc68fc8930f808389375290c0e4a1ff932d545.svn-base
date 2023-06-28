#include "OemNVMe.h"


BOOLEAN ShowNVMeDevice(void)
{   
    EFI_STATUS          Status = EFI_SUCCESS;
    SETUP_DATA          SetupData;
    UINTN               BufferSize = sizeof(SETUP_DATA);
    EFI_GUID            gEfiSetupGuid           = SETUP_GUID;

    Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, NULL, &BufferSize, &SetupData);
    if (EFI_ERROR (Status))
        return FALSE;

    if(SetupData.NVMEBootSwitch == 1)
        return TRUE;

    return FALSE;
}
/**
    Got the device based on the Class,Sub-Class and interface.

    @param  Options       boot device
**/
BOOLEAN NVMePolicyFilter(
    IN BOOT_DEVICE *Device
)
{
    UINT8               FilterDevInterface;

    if((Device->BbsEntry->Class == OemNvme_PCI_CLASS_MASS_STORAGE) && \
        (Device->BbsEntry->SubClass == PCI_CLASS_MASS_STORAGE_SOLID_STATE))
        {

            //Try to read the PCI Interface
            FilterDevInterface = PciRead8(PCI_LIB_ADDRESS(  Device->BbsEntry->Bus,\
                                                            Device->BbsEntry->Device,\
                                                            Device->BbsEntry->Function,\
                                                            PCI_IFT));

            if(FilterDevInterface == PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI)
                return TRUE;
        }

    return FALSE;
}

/**
    Based on the Setup setting to determin wheter to delete the device from BootDeviceList.
**/

VOID UpdateBootDeviceTable()
{
    DLINK               *Link;
    BOOT_DEVICE         *Device;
    UINT8               NVMEBootSwitch =0;


    if(ShowNVMeDevice() == TRUE)
        return;

    FOR_EACH_BOOT_DEVICE(BootDeviceList, Link, Device){
        if (NVMePolicyFilter(Device)){
            DeleteBootDevice(BootDeviceList, Device);
        }
    }

    return;
}
/**
    Update BootOption Table based on the Type & SubType

    @param  Option          Boot option to check
    @param  FilterType      Device Type
    @param  FilterSubType   Device SubType

    @retval  TRUE   Match the map
    @retval  FALSE  Doesn't match the map
**/
BOOLEAN FilterBootOptionbyDeviceType(
    IN BOOT_OPTION* Option,
    IN UINT8        FilterType,
    IN UINT8        FilterSubType)
{
    EFI_STATUS          Status;
    EFI_DEVICE_PATH_PROTOCOL    *FilterDevicePath;

    Status = pBS->HandleProtocol(Option->DeviceHandle,
                                &gEfiDevicePathProtocolGuid,
                                &FilterDevicePath);
    if(EFI_ERROR(Status))
        return FALSE;

    if(Option)
    {
        // check option device path.
        EFI_DEVICE_PATH_PROTOCOL *Dp = FilterDevicePath ;
        while(!isEndNode(Dp))
        {
            DEBUG((DEBUG_INFO,"\r %a Type%x,SubType:%x",__FUNCTION__,Dp->Type,Dp->SubType));
            DEBUG((DEBUG_INFO,"\r %a,Description:%s!!",__FUNCTION__,Option->Description));
            if (Dp->Type == FilterType && Dp->SubType == FilterSubType)
            {
                return TRUE;
            }

            Dp = NEXT_NODE(Dp);
        }
    }

    return FALSE;
}

/**
    Based on the Setup setting to determin wheter to hide the device or not.
**/
VOID FilterNVMeDevice(){
    DLINK               *Link;
    BOOT_OPTION         *Option;
    CHAR16              BootStr[9];
    EFI_STATUS          Status;
    //UINT8               NVMEBootSwitch =0;

    DEBUG((DEBUG_INFO,"\r%a",__FUNCTION__));

    if(ShowNVMeDevice() == TRUE)
        return;

    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){

        if(FilterBootOptionbyDeviceType(Option,MESSAGING_DEVICE_PATH,OemNvme_MSG_NVME_DP))
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

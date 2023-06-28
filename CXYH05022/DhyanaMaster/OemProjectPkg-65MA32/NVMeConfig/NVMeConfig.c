#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <AmdServerHotplug.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include  "Token.h"
#include "Uefi/UefiSpec.h"
//#include "PCIeTopyConfig.c"
#include "AmdServerHotplug.h"
#include "Protocol/NbioHotplugDesc.h"

//Include OEM NVMe Topy Porting
#include "NVMeTopy.c"
#include "AutoNVMePortingList.h"
#include "AutoDxioConfig.h"

#include <Library\Protocol\AmdCpmTableProtocol\AmdCpmTableProtocol.h>
#include <Include\AmdCpmFunction.h>

//NVME_CONFIG   *NvmeConfigList[] = {AUTO_NVME_PORTING_LIST};
BOARD_NVME_TOPY_CONFIG   *NvmeConfigList[] = {AUTO_NVME_PORTING_LIST NULL};
CHAR8         *NvmeConfigNameList[] = {AUTO_NVME_PORTING_LIST_NAMES NULL};

UINT8       DieLanNumber[8]={0,32,80,112,0,32,80,112};

void DumpHotplugDescriptor(HOTPLUG_DESCRIPTOR *HotplugDescriptor, UINTN Count)
{
    UINTN       i;

    for(i=0; (i <= Count) && (HotplugDescriptor[i].Flags != DESCRIPTOR_TERMINATE_LIST); i++)
    {
        DEBUG((EFI_D_ERROR, "==================================================================================\n"));
        DEBUG((DEBUG_INFO, "[%d].Flags                            = %x\n", i, HotplugDescriptor[i].Flags));

        DEBUG((DEBUG_INFO, "[%d].Engine.StartLane                 = %d\n", i, HotplugDescriptor[i].Engine.StartLane));
        DEBUG((DEBUG_INFO, "[%d].Engine.EndLane                   = %d\n", i, HotplugDescriptor[i].Engine.EndLane));
        DEBUG((DEBUG_INFO, "[%d].Engine.SocketNumber              = %x\n", i, HotplugDescriptor[i].Engine.SocketNumber));

        DEBUG((DEBUG_INFO, "[%d].Mapping.HotPlugFormat            = %x\n", i, HotplugDescriptor[i].Mapping.HotPlugFormat));
        DEBUG((DEBUG_INFO, "[%d].Mapping.Revision                 = %x\n", i, HotplugDescriptor[i].Mapping.Revision));
        DEBUG((DEBUG_INFO, "[%d].Mapping.GpioDescriptorValid      = %x\n", i, HotplugDescriptor[i].Mapping.GpioDescriptorValid));
        DEBUG((DEBUG_INFO, "[%d].Mapping.ResetDescriptorValid     = %x\n", i, HotplugDescriptor[i].Mapping.ResetDescriptorValid));
        DEBUG((DEBUG_INFO, "[%d].Mapping.PortActive               = %x\n", i, HotplugDescriptor[i].Mapping.PortActive));
        DEBUG((DEBUG_INFO, "[%d].Mapping.MasterSlaveAPU           = %x\n", i, HotplugDescriptor[i].Mapping.MasterSlaveAPU));
        DEBUG((DEBUG_INFO, "[%d].Mapping.DieNumber                = %x\n", i, HotplugDescriptor[i].Mapping.DieNumber));
        DEBUG((DEBUG_INFO, "[%d].Mapping.ApertureID               = %x\n", i, HotplugDescriptor[i].Mapping.ApertureID));
        DEBUG((DEBUG_INFO, "[%d].Mapping.AlternateSlotNumber      = %x\n", i, HotplugDescriptor[i].Mapping.AlternateSlotNumber));
        DEBUG((DEBUG_INFO, "[%d].Mapping.PrimarySecondaryLink     = %x\n", i, HotplugDescriptor[i].Mapping.PrimarySecondaryLink));
        DEBUG((DEBUG_INFO, "[%d].Mapping.Reserved1                = %x\n", i, HotplugDescriptor[i].Mapping.Reserved1));

        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioBitSelector      = %x\n", i, HotplugDescriptor[i].Function.I2CGpioBitSelector));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioByteMapping      = %x\n", i, HotplugDescriptor[i].Function.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioDeviceMappingExt = %x\n", i, HotplugDescriptor[i].Function.I2CGpioDeviceMappingExt));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioDeviceMapping    = %x\n", i, HotplugDescriptor[i].Function.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CDeviceType           = %x\n", i, HotplugDescriptor[i].Function.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CBusSegment           = %x\n", i, HotplugDescriptor[i].Function.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Function.FunctionMask            = %x\n", i, HotplugDescriptor[i].Function.FunctionMask));
        DEBUG((DEBUG_INFO, "[%d].Function.PortMapping             = %x\n", i, HotplugDescriptor[i].Function.PortMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.Reserved1               = %x\n", i, HotplugDescriptor[i].Function.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CBusSegmentExt        = %x\n", i, HotplugDescriptor[i].Function.I2CBusSegmentExt));

        DEBUG((DEBUG_INFO, "[%d].Reset.Reserved0                  = %x\n", i, HotplugDescriptor[i].Reset.Reserved0));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CGpioByteMapping         = %x\n", i, HotplugDescriptor[i].Reset.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Reset.Reserved1                  = %x\n", i, HotplugDescriptor[i].Reset.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CGpioDeviceMapping       = %x\n", i, HotplugDescriptor[i].Reset.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CDeviceType              = %x\n", i, HotplugDescriptor[i].Reset.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CBusSegment              = %x\n", i, HotplugDescriptor[i].Reset.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Reset.ResetSelect                = %x\n", i, HotplugDescriptor[i].Reset.ResetSelect));

        DEBUG((DEBUG_INFO, "[%d].Gpio.Reserved0                   = %x\n", i, HotplugDescriptor[i].Gpio.Reserved0));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CGpioByteMapping          = %x\n", i, HotplugDescriptor[i].Gpio.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Gpio.Reserved1                   = %x\n", i, HotplugDescriptor[i].Gpio.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CGpioDeviceMapping        = %x\n", i, HotplugDescriptor[i].Gpio.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CDeviceType               = %x\n", i, HotplugDescriptor[i].Gpio.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CBusSegment               = %x\n", i, HotplugDescriptor[i].Gpio.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Gpio.GpioSelect                  = %x\n", i, HotplugDescriptor[i].Gpio.GpioSelect));

    }
}


STATIC NBIO_HOTPLUG_DESC_PROTOCOL mHotplugDescriptorProtocol = {
  AMD_NBIO_HOTPLUG_DESC_VERSION,  ///< revision
  NULL
};

EFI_STATUS SetNbioHotplugDescriptorProtocol(VOID)
{
    EFI_STATUS                       Status;
    HOTPLUG_DESCRIPTOR               *HotplugDescriptor;
 //   UINTN                            HotplugUserDescriptorCount=NVME_PORTS;
    UINTN                            iVaildNVmePortFlag,iHotplugDescriptor;
    UINT32                           VaildNVmePortFlag=0;
    UINT8                            InstalledNVMeCount=PcdGet8(NVMeInstalledCount);
//#mizl-20201014-Update For Signle CPU NVMe Config +>
    NVME_CONFIG                       *NvmeConfig=NULL;
    UINT8                                CountNumber=0xFF;
    BOARD_NVME_TOPY_CONFIG              *BoardNVMETopyTemp=NULL;
/*    UINTN                               NumberOfInstalledProcessors;
    NumberOfInstalledProcessors = PcdGet32(PcdNumberOfInstalledProcessors);
    if ((NumberOfInstalledProcessors == 1)&& (NVME_CONFIG_MAP_COUNT == 2))
    {
      NvmeConfig = NvmeConfigList[1];
      DEBUG((EFI_D_ERROR, "Table:%a(%x) will be installed\n\n",NvmeConfigNameList[1],NvmeConfig));
    }
*/
    VaildNVmePortFlag = PcdGet32(ValidNvmePortMap);
//#mizl-20201014-Update For Signle CPU NVMe Config ->
    CountNumber=PcdGet8(PcdBoardIdSwitchNumber);
    if(CountNumber==0xFF)
    {
        DEBUG((EFI_D_ERROR, "SetNbioHotplugDescriptorProtocol:Don't find the correct NVME Topy List Number\n"));
        return EFI_SUCCESS;
    }
    BoardNVMETopyTemp=NvmeConfigList[CountNumber];
    NvmeConfig=BoardNVMETopyTemp->NVMETopy;
    if(NvmeConfig==NULL)
    {
        DEBUG((EFI_D_ERROR, "SetNbioHotplugDescriptorProtocol:Don't find the correct NVME Topy List\n"));
        return EFI_SUCCESS;
    }
    DEBUG((EFI_D_ERROR, "\nVaild NVme Topy List:%a\n\n",NvmeConfigNameList[CountNumber]));
    DEBUG((EFI_D_ERROR, "\nVaildNVmePortFlag:%x\n\n",VaildNVmePortFlag));
    DEBUG((EFI_D_ERROR, "Installed NVMe Count:%x\n\n",InstalledNVMeCount));
    
    Status = gBS->AllocatePool (
            EfiBootServicesData,
            InstalledNVMeCount * sizeof(HOTPLUG_DESCRIPTOR),
            (VOID **) &HotplugDescriptor
    );

    if(EFI_ERROR(Status) || HotplugDescriptor == NULL) {
        ASSERT(FALSE);
        return Status;
    }

    for(iVaildNVmePortFlag=0,iHotplugDescriptor=0; NvmeConfig[iVaildNVmePortFlag].StartLane  != INVALID_SETTING; iVaildNVmePortFlag++)
    {        
      if(!(VaildNVmePortFlag & (1 << iVaildNVmePortFlag)))
        continue;
      
      HotplugDescriptor[iHotplugDescriptor].Flags = 0;

      HotplugDescriptor[iHotplugDescriptor].Engine.StartLane                 = NvmeConfig[iVaildNVmePortFlag].StartLane + DieLanNumber[NvmeConfig[iVaildNVmePortFlag].DieNumber];
      HotplugDescriptor[iHotplugDescriptor].Engine.EndLane                   = NvmeConfig[iVaildNVmePortFlag].EndLane + DieLanNumber[NvmeConfig[iVaildNVmePortFlag].DieNumber];
      HotplugDescriptor[iHotplugDescriptor].Engine.SocketNumber              = (NvmeConfig[iVaildNVmePortFlag].DieNumber > 3) ? 1:0;

      HotplugDescriptor[iHotplugDescriptor].Mapping.HotPlugFormat            = HotplugExpressModuleB & 0x7;
      HotplugDescriptor[iHotplugDescriptor].Mapping.Revision                 = 1;
      HotplugDescriptor[iHotplugDescriptor].Mapping.GpioDescriptorValid      = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.ResetDescriptorValid     = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.PortActive               = 1;
      HotplugDescriptor[iHotplugDescriptor].Mapping.MasterSlaveAPU           = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.DieNumber                = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.ApertureID               = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.AlternateSlotNumber      = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.PrimarySecondaryLink     = 0;
      HotplugDescriptor[iHotplugDescriptor].Mapping.Reserved1                = 0;

      HotplugDescriptor[iHotplugDescriptor].Function.I2CGpioBitSelector      = 0;//(NvmeConfig[iVaildNVmePortFlag].SlaveAddress >> 4) & 0xF;
      HotplugDescriptor[iHotplugDescriptor].Function.I2CGpioByteMapping      = (NvmeConfig[iVaildNVmePortFlag].ByteMap == Low8BitGpio) ? 0 : 1;
      HotplugDescriptor[iHotplugDescriptor].Function.I2CGpioDeviceMappingExt = 0;
      
      HotplugDescriptor[iHotplugDescriptor].Function.I2CGpioDeviceMapping    = (NvmeConfig[iVaildNVmePortFlag].SlaveAddress >> 1) & 0x7;
      HotplugDescriptor[iHotplugDescriptor].Function.I2CDeviceType           = Pca9555 & 0x3;
      HotplugDescriptor[iHotplugDescriptor].Function.I2CBusSegment           = NvmeConfig[iVaildNVmePortFlag].ChannelNumber & 7;
      HotplugDescriptor[iHotplugDescriptor].Function.FunctionMask            = 0xff;
      HotplugDescriptor[iHotplugDescriptor].Function.PortMapping             = 0;
      HotplugDescriptor[iHotplugDescriptor].Function.Reserved1               = 0;
      HotplugDescriptor[iHotplugDescriptor].Function.I2CBusSegmentExt        = NvmeConfig[iVaildNVmePortFlag].ChannelNumber >> 3;

      HotplugDescriptor[iHotplugDescriptor].Reset.Reserved0                  = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.I2CGpioByteMapping         = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.Reserved1                  = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.I2CGpioDeviceMapping       = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.I2CDeviceType              = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.I2CBusSegment              = 0;
      HotplugDescriptor[iHotplugDescriptor].Reset.ResetSelect                = 0;

      HotplugDescriptor[iHotplugDescriptor].Gpio.Reserved0                   = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.I2CGpioByteMapping          = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.Reserved1                   = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.I2CGpioDeviceMapping        = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.I2CDeviceType               = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.I2CBusSegment               = 0;
      HotplugDescriptor[iHotplugDescriptor].Gpio.GpioSelect                  = 0;

      #if defined(NVME_CONFIG_DEBUG) && (NVME_CONFIG_DEBUG == 1)
      DEBUG((EFI_D_ERROR, "==================================================================================\n"));
      DEBUG((EFI_D_ERROR, "Config Port:%d As HotPlug\n",iVaildNVmePortFlag));
      DEBUG((EFI_D_ERROR, "Die Number:%x\n",NvmeConfig[iVaildNVmePortFlag].DieNumber));
      DEBUG((EFI_D_ERROR, "Start Lane:%d  TransLated:%d\n",NvmeConfig[iVaildNVmePortFlag].StartLane,HotplugDescriptor[iHotplugDescriptor].Engine.StartLane));
      DEBUG((EFI_D_ERROR, "End Lane:%d TransLated:%d\n",NvmeConfig[iVaildNVmePortFlag].EndLane,HotplugDescriptor[iHotplugDescriptor].Engine.EndLane));
      DEBUG((EFI_D_ERROR, "Slave Address:%x\n",NvmeConfig[iVaildNVmePortFlag].SlaveAddress));
      DEBUG((EFI_D_ERROR, "Byte Mapping:%x\n",NvmeConfig[iVaildNVmePortFlag].ByteMap));
      DEBUG((EFI_D_ERROR, "Channel Mapping:%x\n",NvmeConfig[iVaildNVmePortFlag].ChannelNumber));
      DEBUG((EFI_D_ERROR, "ValidPortFlag:%x\n",NvmeConfig[iVaildNVmePortFlag].ValidPortFlag));
      #endif
      
      iHotplugDescriptor++;
    }
    
    HotplugDescriptor[iHotplugDescriptor - 1].Flags = DESCRIPTOR_TERMINATE_LIST;

    DumpHotplugDescriptor(HotplugDescriptor, InstalledNVMeCount);

    mHotplugDescriptorProtocol.NbioHotplugDesc = HotplugDescriptor;

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
HotplugDescEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{

  EFI_HANDLE                       Handle;
  EFI_STATUS                       Status;

  Handle = NULL;

  SetNbioHotplugDescriptorProtocol();
  // Fixup HOTPLUG_DESCRIPTOR here
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdHotplugDescProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHotplugDescriptorProtocol
                  );

  return Status;
}

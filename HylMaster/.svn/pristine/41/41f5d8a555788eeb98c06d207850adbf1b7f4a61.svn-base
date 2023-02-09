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

/** @file ASM1061RDxe.c
    Initialize the ASM1061R Pcie to SATA bridge in the DXE stage.

*/

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
#include <PCICommon.h>
#include <ASM106xLib.h>
#include <Library/UefiLib.h>
#include <Include/Setup.h>
#include <Protocol/LegacyBiosExt.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define ASM1061RVendorID 0x1B21
#define ASM1061RDeviceID_IDE 0x0621
#define ASM1061RDeviceID_AHCI 0x0622
#define ASM1061RDeviceID_RAID 0x0623

SETUP_DATA  static SetupData = { NULL };

#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
VOID        static *Registration = NULL;
/**
    This callback function is called before/after processing all
    PCI optonal ROM.

    @param Event Event of callback
    @param Context Context of callback.

    @retval VOID
**/

VOID ASM1061RCheckOprom (
    IN EFI_EVENT   Event,
    IN VOID        *Context )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    CSM_PLATFORM_POLICY_DATA    *Data = NULL;
    UINT8                       ConfigSpaceHeader[0x10];
    UINT16                      DID;
    UINT16                      VID;
    UINTN                       BufferSize = sizeof(EFI_HANDLE);
    EFI_HANDLE                  Handle;
    SETUP_DATA                  *pSetupData = Context;

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
    Status = gBS->LocateHandle( ByRegisterNotify, \
                                NULL, \
                                Registration, \
                                &BufferSize, \
                                &Handle );
    if (EFI_ERROR(Status)) return;

    // Locate CSM Platform Policy data
    Status = gBS->HandleProtocol( Handle, &gOpromStartEndProtocolGuid, &Data );
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    if (Data == NULL) return;
    if (Data->PciIo == NULL) return;

    //Status = Data->PciIo->GetLocation( Data->PciIo, &Seg, &Bus, &Dev, &Fun );
    //ASSERT_EFI_ERROR(Status);
    Status = Data->PciIo->Pci.Read( Data->PciIo, EfiPciIoWidthUint8, 0x0, 0x10, ConfigSpaceHeader);
    ASSERT_EFI_ERROR(Status);

    if (!EFI_ERROR(Status)) {
        VID=ConfigSpaceHeader[1]*0x100+ConfigSpaceHeader[0];
        DID=ConfigSpaceHeader[3]*0x100+ConfigSpaceHeader[2];
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]VID:%x,DID:%x\n\n",__FUNCTION__,__LINE__,VID,DID));
        if ((VID == ASM1061RVendorID) && ((DID & 0xFFF0) == 0x0620))
        {
            if (pSetupData != NULL) {
                if (pSetupData->ASM1061R_SataMode == 1) Data->ExecuteThisRom = FALSE;
                DEBUG((-1, "VID %x DID %x Data->ExecuteThisRom %x\n", VID, DID, Data->ExecuteThisRom));
            }
        }
    }
}
#endif

/**
    This function contain all the initializes code in DXE Stage for ASM1061R.

    @param ImageHandle - Image Handle.
    @param SystemTable - Pointer to the System table.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
ASM1061RDxeInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
)
{
    EFI_STATUS              Status;
    
    UINT64                  SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                  SetupVariableAttributes=0;
    EFI_GUID                gSetupGuid = SETUP_GUID;
    UINT8                   ASM106RxRootBusNumber = 0,ASM106RxBusNumber;
    UINTN                   PciAddress;
    UINT32                  PCIReg32;
    UINT8                   ASM106R_Temp_BusNumber=0;
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    EFI_EVENT               Event = NULL;
#endif
    
    Status = gRT->GetVariable (L"Setup",
                                &gSetupGuid,
                                &SetupVariableAttributes,
                                &SetupVariableSize,
                                &SetupData);
    if (Status != EFI_SUCCESS) {
        return EFI_SUCCESS;
    }
    

    //Get Data Fabric B00D0x18+DieNumber F00 Location
    PciAddress = PCI_LIB_ADDRESS(0,ASM1061R_ROOT_DIE_NUMBER+0x18,0,FCH_CFG_REG000);

    //Try to Read did/vid
    PCIReg32 = PciRead32(PciAddress);
    DEBUG((EFI_D_ERROR, "[%a][%d]Data Fabric :%x\n\n",__FUNCTION__,__LINE__,PCIReg32));

    // Get Bus Number For Die #1
    //PciAddress = PCI_LIB_ADDRESS(0,ASM1061R_ROOT_DIE_NUMBER+0x18,0,FCH_CFG_REG084);
    PCIReg32 = PciRead32(PciAddress+FCH_CFG_REG084);
    //Low 8Bit is SecBus Number
    ASM106RxRootBusNumber = PCIReg32 & 0xFF;
    DEBUG((EFI_D_ERROR, "[%a][%d]Secondary Bus Number:%x\n\n",__FUNCTION__,__LINE__,PCIReg32));

    //Calculate ASM106R Temp Bus Number
    ASM106R_Temp_BusNumber = ASM106RxRootBusNumber + ASM1061R_SUB_BUS_NUMBER;
    //Now We Can Read  the ASM106X RootBridge Default BxxD01F04
    PciAddress = PCI_LIB_ADDRESS(ASM106RxRootBusNumber,ASM1061R_UPPER_STREAM_PORT_DEV_NUM,ASM1061R_UPPER_STREAM_PORT_FUN_NUM,FCH_CFG_REG000);
    PCIReg32 = PciRead32(PciAddress);
    DEBUG((EFI_D_ERROR, "[%a][%d]ASM106x Root DID VID:%x  Bus Number:%x\n\n",__FUNCTION__,__LINE__,PCIReg32,ASM106RxRootBusNumber));
    if(PCIReg32 != 0x14531d94)
    {
        DEBUG((EFI_D_ERROR, "[%a][%d]Invalid Root Bridge Information,You may need Double Check It!!!\n\n",__FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }

    //Now Try To Get The Device's ASM106x's Bus Information
    //PciWrite8(PCIReg32|FCH_CFG_REG019,ASM106R_Temp_BusNumber);
    //PciWrite8(PCIReg32|FCH_CFG_REG01A,ASM106R_Temp_BusNumber);
    SetPciBridgerBusNum(PciAddress,ASM106R_Temp_BusNumber);
    ASM106RxBusNumber = PciRead8(PciAddress|FCH_CFG_REG019);
    DEBUG((EFI_D_ERROR, "[%a][%d]ASM106x Bus Number:%x\n\n",__FUNCTION__,__LINE__,ASM106RxBusNumber));

    //Now Try to Config ASM106x Setting
    PciAddress = PCI_LIB_ADDRESS(ASM106RxBusNumber,0,0,FCH_CFG_REG000);
    PCIReg32 = PciRead32(PciAddress);
    DEBUG((EFI_D_ERROR, "[%a][%d]ASM106x ID:%x\n\n",__FUNCTION__,__LINE__,PCIReg32));
    if((PCIReg32&0xFFFF) != 0x1B21) //We Just Compare Vendor ID Instead of VendorID & Device ID
    {
        DEBUG((EFI_D_ERROR, "[%a][%d]Invalid ASM106x Vendor Information,You may need Double Check It!!!\n\n",__FUNCTION__,__LINE__));
        return EFI_DEVICE_ERROR;
    }
    
	//1.Program PCI Class Code
    ASM1061RSetAhciClass(PciAddress);

    //2.Program PCI SVID and SSID (optional, porting later)   
    //    (1). Enable SVID and SSID to be programmed
    //    Set PCI RxF0[0] = 1
    //    (2). Set SVID and SSID
    //    Modify PCI Rx2C ~ Rx2F to required values
    //    (3). Disable SVID and SSID to be programmed
    //    Set PCI RxF0[0] = 0

    //3. Spin-Up SATA device 
    ASM1061RSpinUp( PciAddress);   
    
    //4. Disable Safety Removal for AHCI Mode
    ASM1061RDisableSafetyRemoval( PciAddress);   
        
    //5. Disable SATA Device Sleep
    ASM1061RDisableDeviceSleep( PciAddress);
    
    //6.SATA SSC Control (Porting later)
    
    //7.Set SATA Driving Registers (Porting later)
    
    PciAddress = PCI_LIB_ADDRESS(ASM1061R_ROOT_DIE_NUMBER,\
                            ASM1061R_UPPER_STREAM_PORT_DEV_NUM,\
                            ASM1061R_UPPER_STREAM_PORT_FUN_NUM,\
                            FCH_CFG_REG000);

    SetPciBridgerBusNum(PciAddress,0);
//<Hujf001-20210323 ASM1061RCheckOprom event may cause system hang when ASM1061R training fail +>
    #if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    Event = EfiCreateProtocolNotifyEvent (
                &gOpromStartEndProtocolGuid,
                TPL_NOTIFY,
                ASM1061RCheckOprom,
                &SetupData,
                &Registration
                );
    ASSERT (Event != NULL);
    #endif
//<Hujf001-20210323 ASM1061RCheckOprom event may cause system hang when ASM1061R training fail ->
    return EFI_SUCCESS;
}

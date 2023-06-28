//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
 
//**********************************************************************
//
/** @file AmiPspDxe.c
    PSP Device driver   

**/

#include "token.h"
#include <PCI.h>
#include <AmiDxeLib.h>
#include <protocol/DriverBinding.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <BootScriptCommon.h>
#include <Library/DebugLib.h>
#include <Library/AmdPspApobLib.h>

EFI_STATUS
PspDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
);


EFI_STATUS
EFIAPI AmiPspDriverSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath );

EFI_STATUS
EFIAPI AmiPspDriverStart (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath );

EFI_STATUS
EFIAPI AmiPspDriverStop (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN UINTN                       NumberOfChildren,
    IN EFI_HANDLE                  *ChildHandleBuffer );


typedef struct {
    UINT64                  Address;
    EFI_BOOT_SCRIPT_WIDTH   Width;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

#define PSP_PCI_CFG_ADDRESS(bus, dev, func, reg) \
            (UINT64) ((((UINT8)(bus) << 24) + ((UINT8)(dev) << 16) + \
            ((UINT8)(func) << 8) + ((UINT8)(reg))) & 0xffffffff)

#define PSP_REG(Reg)    PSP_PCI_CFG_ADDRESS(0,8,0,Reg)




// EFI Driver Binding Protocol Instance
EFI_DRIVER_BINDING_PROTOCOL gAmiPspDriverBinding = {
    AmiPspDriverSupported,
    AmiPspDriverStart,
    AmiPspDriverStop,
    0x10,
    NULL,
    NULL
};

EFI_S3_SAVE_STATE_PROTOCOL              *gBootScriptSave = NULL;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL         *gPciRootBridgeIo;

EFI_EVENT                               gReadyToBootEvent;


//
// ready to boot call back boot script table
//
BOOT_SCRIPT_PCI_REGISTER_SAVE gReadyToBootPsp_ScriptTbl[] = {
//-    PSP_REG(0x48), EfiBootScriptWidthUint32,          // BAR control
    PSP_REG(0x10), EfiBootScriptWidthUint32,          // BAR64
    PSP_REG(0x18), EfiBootScriptWidthUint32,          // BAR1
    PSP_REG(0x1C), EfiBootScriptWidthUint32,          // BAR2
    PSP_REG(0x24), EfiBootScriptWidthUint32,          // MSIX
    PSP_REG(0x3C), EfiBootScriptWidthUint32,          // Int line
//-    PSP_REG(0x04), EfiBootScriptWidthUint32,          // Command
};
UINTN gReadyToBootPsp_ScriptSize = sizeof(gReadyToBootPsp_ScriptTbl)/sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE);


//**********************************************************************
/**
    PSP Driver Supported function
    This function is a part of DriverBinding protocol

        
    @param This 
    @param ControllerHandle 
    @param RemainingDevicePath 

         
    @retval EFI_STATUS Return the EFI  Status

**/
//**********************************************************************

EFI_STATUS
EFIAPI AmiPspDriverSupported(
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath )
{
    EFI_STATUS                  Status;
    EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
    PCI_STD_DEVICE              Pci;

    // Get PciIo pointer
    Status = pBS->OpenProtocol( ControllerHandle,
                    &gEfiPciIoProtocolGuid,
                    (void **)&PciIo,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_BY_DRIVER );

    if  ( EFI_ERROR(Status) ) {
        return  EFI_UNSUPPORTED;
    }
    
    Status = PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint32,
                    0,
                    sizeof (Pci) / sizeof (UINT32),
                    &Pci);

    if  ( EFI_ERROR(Status) ) {
        goto CloseProtocols;
        return Status;
    }
    
    // Check if this is PSP device
    if(((Pci.Header.VendorId != 0x1022) && (Pci.Header.VendorId != 0x1D94)) || (Pci.Header.DeviceId != 0x1578)){
        goto CloseProtocols;
        return  EFI_UNSUPPORTED;
    }
    
    Status = EFI_SUCCESS;

    return  Status;

CloseProtocols:

    pBS->CloseProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );
    return EFI_UNSUPPORTED;
}


//**********************************************************************
/**
    PSP Driver Start function
				

        
    @param This 
    @param ControllerHandle 
    @param RemainingDevicePath 

         
    @retval EFI_STATUS Return the EFI  Status

**/
//**********************************************************************

EFI_STATUS
EFIAPI AmiPspDriverStart(
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath )
{
    EFI_STATUS               Status;
    EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
    UINT64                      Attributes;
    UINT32                      Temp32 = 0;

    // Open PCI I/O Protocol
    Status = pBS->OpenProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                (void **)&PciIo,
                This->DriverBindingHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_BY_DRIVER );

    if  ( EFI_ERROR(Status) ) {
        if((Status != EFI_ALREADY_STARTED) || (PciIo == NULL)){
            return Status;
        }
    }


    if ((EFI_ERROR (Status)) && (Status != EFI_ALREADY_STARTED)) {
        pBS->CloseProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );

        return Status;
    }
    
    Status = PciIo->Attributes( PciIo,
                                EfiPciIoAttributeOperationGet,
                                0,
                                &Attributes );
    if (EFI_ERROR (Status)) {
        goto    CloseProtocols;
    }
    
    if( (Attributes & EFI_PCI_IO_ATTRIBUTE_MEMORY) != 0){
        Status = EFI_ALREADY_STARTED;
        return Status;
    }
    
    Attributes = EFI_PCI_IO_ATTRIBUTE_MEMORY | EFI_PCI_IO_ATTRIBUTE_BUS_MASTER;
    
    Status = PciIo->Attributes( PciIo,
                                    EfiPciIoAttributeOperationEnable,
                                    Attributes,
                                    NULL );
    if(EFI_ERROR(Status)) {
            goto CloseProtocols;
    }

    return  Status;
    
CloseProtocols:

    pBS->CloseProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );
                
    return  Status;
 }
 
 //**********************************************************************
/**
    PSP Driver Stop function
				

        
    @param This 
    @param ControllerHandle 
    @param NumberOfChildren 
    @param ChildHandleBuffer 

         
    @retval EFI_STATUS Return the EFI  Status

**/
//**********************************************************************

EFI_STATUS
EFIAPI AmiPspDriverStop(
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  ControllerHandle,
    IN UINTN                       NumberOfChildren,
    IN EFI_HANDLE                  *ChildHandleBuffer )
{
    EFI_STATUS                  Status;
    UINT64                      Attributes;
    EFI_PCI_IO_PROTOCOL         *PciIo = NULL;
    
    // Open PCI I/O Protocol
    Status = pBS->OpenProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                (void **)&PciIo,
                This->DriverBindingHandle,
                ControllerHandle,
                EFI_OPEN_PROTOCOL_BY_DRIVER );

    if  ( EFI_ERROR(Status) ) {
        if((Status != EFI_ALREADY_STARTED) || (PciIo == NULL)){
            return Status;
        }
    }
    
    Attributes = EFI_PCI_IO_ATTRIBUTE_MEMORY;
    Status = PciIo->Attributes( PciIo,
                                    EfiPciIoAttributeOperationDisable,
                                    Attributes,
                                    NULL );
    
    // close protocols, opened in Start function
    pBS->CloseProtocol( ControllerHandle,
                &gEfiPciIoProtocolGuid,
                This->DriverBindingHandle,
                ControllerHandle );
                
    return  Status;
}  


VOID    
AmiPspReadyToBootCallBack(
    IN EFI_EVENT Event, 
    IN VOID *Context)
{
    EFI_STATUS      Status;
    UINTN           i;
    UINT64          Buffer;

    if (gPciRootBridgeIo == NULL) {
        Status = pBS->LocateProtocol(
                            &gEfiPciRootBridgeIoProtocolGuid,
                            NULL,
                            &gPciRootBridgeIo
                            );
        ASSERT_EFI_ERROR(Status);
    }

    if (gBootScriptSave == NULL) {
        Status = pBS->LocateProtocol(&gEfiS3SaveStateProtocolGuid, NULL, \
                    &gBootScriptSave);
        ASSERT_EFI_ERROR(Status);
    }

    for (i = 0; i < gReadyToBootPsp_ScriptSize; i++) {
        gPciRootBridgeIo->Pci.Read(
                        gPciRootBridgeIo,
                        gReadyToBootPsp_ScriptTbl[i].Width,
                        gReadyToBootPsp_ScriptTbl[i].Address,
                        1,
                        &Buffer);
                        
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave,
                        gReadyToBootPsp_ScriptTbl[i].Width,
                        gReadyToBootPsp_ScriptTbl[i].Address,
                        1,
                        &Buffer);
    }

    pBS->CloseEvent(Event);
}

/**
    PSP Device driver entry point

               
    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Return the EFI  Status

**/

EFI_STATUS
PspDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_STATUS  Status;
    
    gAmiPspDriverBinding.DriverBindingHandle = ImageHandle;
    gAmiPspDriverBinding.ImageHandle = ImageHandle;

    InitAmiLib(ImageHandle, SystemTable);
    
    Status = pBS->InstallMultipleProtocolInterfaces(
                        &gAmiPspDriverBinding.DriverBindingHandle,
                        &gEfiDriverBindingProtocolGuid, &gAmiPspDriverBinding,
//                        &gComponentNameProtocolGuid, &gAmiPspDriverName,
                        NULL
                        );
    ASSERT_EFI_ERROR(Status);

    //
    // Create event for boot script
    //
    Status = CreateReadyToBootEvent(
                TPL_NOTIFY, \
                AmiPspReadyToBootCallBack, \
                NULL, \
                &gReadyToBootEvent \
                );
    ASSERT_EFI_ERROR(Status);
    
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

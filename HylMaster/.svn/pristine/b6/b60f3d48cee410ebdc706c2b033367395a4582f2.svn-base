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

//**********************************************************************
/** @file SataController.c
    Initialize and provide a protocol for the SATA controller.


**/
//**********************************************************************

// Module specific Includes
#include "IdeController.h"

CHAR16  *gIdeDriverName     = L"AMI SATA Controller Driver";
CHAR16  *gIdeControllerName = L"HYGON Taishan SATA Controller";

extern EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *gPciRootBridgeIo;
extern HYGONSB_POLICY_PROTOCOL            *gHygonSbPolicy;
#if PI_SPECIFICATION_VERSION < 0x0001000A
extern   EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *gBootScriptSave;
#else
extern   EFI_S3_SAVE_STATE_PROTOCOL       *gBootScriptSave;
#endif
//
// Pci io protocol call back boot script table
//
BOOT_SCRIPT_PCI_REGISTER_SAVE gPciCB_ScriptTbl[] = {
    // Save resource and decoded after PciBus Enumerate
    SATA_REG(0x10),  EfiBootScriptWidthUint32,
    SATA_REG(0x14),  EfiBootScriptWidthUint32,
    SATA_REG(0x18),  EfiBootScriptWidthUint32,
    SATA_REG(0x1C),  EfiBootScriptWidthUint32,
    SATA_REG(0x20),  EfiBootScriptWidthUint32,
    SATA_REG(0x24),  EfiBootScriptWidthUint32
};
UINTN gPciCB_ScriptSize = \
        sizeof(gPciCB_ScriptTbl) / sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE);

//
// ready to boot call back boot script table
//
BOOT_SCRIPT_PCI_REGISTER_SAVE gReadyToBootCB_ScriptTbl[] = {
    SATA_REG(0x4), EfiBootScriptWidthUint8
};
UINTN gReadyToBootCB_ScriptSize = \
                sizeof(gReadyToBootCB_ScriptTbl)/  \
                sizeof(BOOT_SCRIPT_PCI_REGISTER_SAVE);

static PCI_STD_DEVICE   gPciData;

//**********************************************************************
/**
**/
//**********************************************************************
BOOLEAN IsAHCI(VOID)
{
    if ((gPciData.Header.ClassCode[1] == HYGONSB_CLASS_CODE_AHCI) && \
        ((gPciData.Header.DeviceId == FCH_SATA_AHCI_DID) || (gPciData.Header.DeviceId == FCH_SATA_AMDAHCI_DID) )) {
        return TRUE;
    }else return FALSE;
}

/**

    @param VOID

        BOOLEAN
    @retval TRUE is Raid controller
    @retval FALSE not a Raid controller

**/

BOOLEAN IsRAID(VOID)
{
    BOOLEAN     IsRaid = FALSE;
#ifdef HYGON_SB_RAID_IDE_BUS_SUPPORT
    if ((gPciData.Header.ClassCode[1] == PCI_CL_MASS_STOR_SCL_RAID) && \
        ((gPciData.Header.DeviceId == FCH_SATA_RAID_DID) || \
        (gPciData.Header.DeviceId == FCH_SATA_RAID5_DID) || \
        (gPciData.Header.DeviceId == RAID_FC_DEVID))) {
        IsRaid = TRUE;
    }
#endif
    return IsRaid;
}

/**
    return the current the MAX Channels for SATA controller.

    @param VOID

    @retval UINT8 Number of MAX CHANNELS


**/

UINT8
GetMaxChannels(VOID)
{
    if(IsAHCI() || IsRAID()) return CONTROLLER_MAX_CHANNELS_AHCI;
    else return CONTROLLER_MAX_CHANNELS;
}

//**********************************************************************
/**
    Check for the desired IDE controller.

        
    @param PciIo Pointer the PCI IO prococol

         
    @retval EFI_SUCCESS Is PATA controller
    @retval EFI_ERROR not PATA controller

    @note  
  This routine is specific to South Bridge. If multiple IDE controllers
  are not handled by the same IDE controller driver, then this routine is
  not needed.

**/
//**********************************************************************
EFI_STATUS
IdeControllerCheck (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
    EFI_STATUS      Status;
    //
    // Now further check the PCI header: Base class (offset 0x0B) and
    // Sub Class (offset 0x0A). This controller should be an SATA controller
    //
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          0,
                          sizeof (gPciData),
                          &gPciData
                          );
    if (EFI_ERROR (Status)) return Status;

    Status = EFI_UNSUPPORTED;
    if ((gPciData.Header.ClassCode[2] == PCI_CL_MASS_STOR) && \
        //(gPciData.Header.VendorId == FCH_SATA_VID )) {
        (gPciData.Header.VendorId == AMD_VID || gPciData.Header.VendorId == HYGON_VID )) {

        if ((gPciData.Header.ClassCode[1] == PCI_CL_MASS_STOR_SCL_IDE) && \
            (gPciData.Header.DeviceId == FCH_SATA_DID) \
            || (gPciData.Header.DeviceId == FCH_SATA_AHCI_DID) // For ide2ahci
            ) {
            Status = EFI_SUCCESS;
        }
        if (IsAHCI()) Status = EFI_SUCCESS;
        if (IsRAID()) Status = EFI_SUCCESS;
    }

    return Status;
}

//**********************************************************************
/**
    Initialize the given IDE Controller

        
    @param PciIo Pointer the PCI IO prococol

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  
  Identify the IDE controller to be Initialized and do the initialization.


**/
//**********************************************************************
EFI_STATUS
InitIdeController (
  IN EFI_PCI_IO_PROTOCOL    *PciIo
  )
{
    EFI_STATUS  Status;
    UINT64      Attr;

    //
    // Enable the Controller
    //
    Status = PciIo->Attributes(PciIo,
                               EfiPciIoAttributeOperationSupported,
                               0,&Attr
                                );

    if (!EFI_ERROR (Status)) {
        Attr &= (EFI_PCI_IO_ATTRIBUTE_MEMORY | EFI_PCI_IO_ATTRIBUTE_IO \
                | EFI_PCI_IO_ATTRIBUTE_BUS_MASTER);

        Status = PciIo->Attributes( PciIo,
                                    EfiPciIoAttributeOperationSet,
                                    Attr,
                                    NULL
                                    );
    }

    return Status;
}

//**********************************************************************
/**
    This function is used to set appropriate UDMA timing on SATA
    controller according supported UDMA modes

        
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param PciIo - Pointer to PciIo protocol opened by SATA controller driver
    @param Modes - The UDMA mode collection supported by IDE device

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
IdeInitSetUdmaTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
{
    EFI_STATUS  Status;
    UINT64      Attr;
    UINTN       Buffer;
    UINT32      UdmaMode;
    UINT32      MWDma;

    UdmaMode = (Modes->UdmaMode.Valid) ? \
                        Modes->UdmaMode.Mode:0xFF;
    MWDma = (Modes->MultiWordDmaMode.Valid) ? \
                        Modes->MultiWordDmaMode.Mode:0xFF;

    Status=PciIo->Attributes(   PciIo,
                                EfiPciIoAttributeOperationSupported,
                                0,&Attr
                                );
    if (!EFI_ERROR (Status)) {

        Attr&=(EFI_PCI_IO_ATTRIBUTE_BUS_MASTER);

        Status=PciIo->Attributes(   PciIo,
                                    EfiPciIoAttributeOperationSet,
                                    Attr,
                                    NULL);
    }
    // Enable DMA capable bit in Bus Master Status Register
    Status = PciIo->Io.Read (
            PciIo,
            EfiPciIoWidthUint8,
            4,                              // BarIndex
            Channel == 0 ? 2 : 0xA,         // Offset
            1,                              // Count
            &Buffer);

    if ((UdmaMode != 0xff) ||  (MWDma != 0xff)) {
        Buffer |= (Device == 0 ? 0x20 : 0x40); // Master/Slave
    }
    else {
        Buffer &= (Device == 0 ? 0xDF : 0xBF); // Master/Slave
    }

    Status = PciIo->Io.Write(
            PciIo,
            EfiPciIoWidthUint8,
            4,                              // BarIndex
            Channel == 0 ? 2 : 0xA,         // Offset
            1,                              // Count
            &Buffer);

    return Status;
}


//**********************************************************************
/**
    This function is used to set appropriate PIO timing on Ide
    controller according supported PIO modes

        
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param PciIo - Pointer to PciIo protocol opened by Ide controller driver
    @param IdentifyData - The identify struct submitted by IDE device
    @param Modes - The PIO mode collection supported by IDE device

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
IdeInitSetPioTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_IDENTIFY_DATA           *IdentifyData,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
{
    ASSERT (IdentifyData);
    return EFI_SUCCESS;
}

//**********************************************************************
/**
    User manual select the Ide Mode

        
    @param SetupData BIOS Setup data
    @param Channel IDE channel number (  based, either 0 or 1)
    @param Device IDE device number
    @param SupportedModes Modes collection supported by IDE device

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
UserIdeMode(
  IN SB_SETUP_DATA                  *SetupData,
  IN  UINT8                         Channel,
  IN  UINT8                         Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE    **SupportedModes
  )
{
#if SHOW_IDE_MODE_SETUP
    UINT8 selChannel;
    // [channel][device]
    UINT8 UserDMA[2][2] = {
        // [0][x] Sata
        {SetupData->Sata0DmaMode, SetupData->Sata2DmaMode},
        // [1][x] Sata
        {SetupData->Sata1DmaMode, SetupData->Sata3DmaMode},
    };
    UINT8 UserPIO[2][2] = {
        // [0][x] Sata
        {SetupData->Sata0PioMode, SetupData->Sata2PioMode},
        // [1][x] Sata
        {SetupData->Sata1PioMode, SetupData->Sata3PioMode},
    };

    selChannel = Channel;
    //
    // User DMA mode
    //
    if(UserDMA[selChannel][Device]) {
        // non-zero User select. set default off
        (*SupportedModes)->UdmaMode.Valid = \
        (*SupportedModes)->SingleWordDmaMode.Valid = \
        (*SupportedModes)->MultiWordDmaMode.Valid = FALSE;

        // alos set it 0xFF, because IdeBus.c use 0xFF.
        (*SupportedModes)->UdmaMode.Mode = \
        (*SupportedModes)->SingleWordDmaMode.Mode = \
        (*SupportedModes)->MultiWordDmaMode.Mode = 0xFF;

    }
    switch(UserDMA[selChannel][Device]) {
        // for AUTO
        case 0:
        break;
        // for SWDmaMode 0,1,2 >> Taishan not support ??
        case 1:case 2:case 3:
            (*SupportedModes)->SingleWordDmaMode.Valid = TRUE;
            (*SupportedModes)->SingleWordDmaMode.Mode = \
                                    UserDMA[selChannel][Device]-1;
        break;
        // for MWDmaMode 0,1,2
        case 4:case 5:case 6:
            (*SupportedModes)->MultiWordDmaMode.Valid = TRUE;
            (*SupportedModes)->MultiWordDmaMode.Mode = \
                                    UserDMA[selChannel][Device]-4;
        break;
        // for UDmaMode 0,1,2,3,4,5,6
        case 7:case 8:case 9:case 10:case 11:case 12:case 13:
            (*SupportedModes)->UdmaMode.Valid = TRUE;
            (*SupportedModes)->UdmaMode.Mode = \
                                    UserDMA[selChannel][Device]-7;
        break;
        default:
        break;
    }
    //
    // User PIO mode
    //
    if(UserPIO[selChannel][Device]) {
        (*SupportedModes)->PioMode.Valid = TRUE;
        (*SupportedModes)->PioMode.Mode = \
                                    UserPIO[selChannel][Device]-1;
    }

#endif
    return EFI_SUCCESS;
}

//**********************************************************************
/**
    Pci Io CallBack Script Save

    @param VOID

    @retval VOID

**/
//**********************************************************************
VOID
PciIoCallBackScriptSave(VOID)
{
    UINTN                           Buffer,i;
    for (i = 0; i < gPciCB_ScriptSize; i++) {
        gPciRootBridgeIo->Pci.Read(
                        gPciRootBridgeIo,
                        gPciCB_ScriptTbl[i].Width,
                        gPciCB_ScriptTbl[i].Address,
                        1,
                        &Buffer);
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave,
                        gPciCB_ScriptTbl[i].Width,
                        gPciCB_ScriptTbl[i].Address,
                        1,
                        &Buffer);
    }

    // save controller bar base address for Taishan CIMx module S3 resume
    // That is unnecessary. Taishan CIMx should save base by itself.
    // If move out sataInitAfterPciEnum from sbAfterPciInit
    // and call it in InitIdeController that will be easier.
    gPciRootBridgeIo->Pci.Read(
                    gPciRootBridgeIo,
                    EfiBootScriptWidthUint8,
                    SATA_REG(0x4),
                    1,
                    &Buffer);
    Buffer |= 0x7;
    BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                    gBootScriptSave,
                    EfiBootScriptWidthUint8,
                    SATA_REG(0x4),
                    1,
                    &Buffer);
}

//**********************************************************************
/**
    Ready To Boot CallBackScriptSave

    @param VOID

    @retval VOID

**/
//**********************************************************************
VOID
ReadyToBootCallBackScriptSave(VOID)
{
    UINTN                           Buffer,i;
    for (i = 0; i < gReadyToBootCB_ScriptSize; i++) {
        gPciRootBridgeIo->Pci.Read(
                        gPciRootBridgeIo,
                        gReadyToBootCB_ScriptTbl[i].Width,
                        gReadyToBootCB_ScriptTbl[i].Address,
                        1,
                        &Buffer);
        BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                        gBootScriptSave,
                        gReadyToBootCB_ScriptTbl[i].Width,
                        gReadyToBootCB_ScriptTbl[i].Address,
                        1,
                        &Buffer);
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

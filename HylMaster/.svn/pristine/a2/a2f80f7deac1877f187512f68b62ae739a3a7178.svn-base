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
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include "ASM1061RDriver.h"
#include <Include/Setup.h>
EFI_GUID gControllerDriverGuid = CONTROLLER_DRIVER_GUID;
extern EFI_COMPONENT_NAME2_PROTOCOL gASM1061RComponentNameProtocol;
//-------------------------------------------------------
//  EFI_DRIVER_BINDING_PROTOCOL instance
//-------------------------------------------------------
EFI_DRIVER_BINDING_PROTOCOL gASM1061RIdeControllerDriverBinding = {
  ASM1061RIdeControllerSupported,
  ASM1061RIdeControllerStart,
  ASM1061RIdeControllerStop,
  1,
  NULL,
  NULL
};

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
ASM1061RIdeControllerCheck (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
    EFI_STATUS      Status;
   PCI_STD_DEVICE   PciData;
    //
    // Now further check the PCI header: Base class (offset 0x0B) and
    // Sub Class (offset 0x0A). This controller should be an SATA controller
    //
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          0,
                          sizeof (PciData),
                          &PciData
                          );
    if (EFI_ERROR (Status)) return Status;

    Status = EFI_UNSUPPORTED;
    if ((PciData.Header.ClassCode[2] == PCI_CL_MASS_STOR) && \
        ((PciData.Header.VendorId == ASM1061RVendorID) && (PciData.Header.DeviceId == ASM1061RDeviceID ))) {

        //if ((PciData.Header.ClassCode[1] == PCI_CL_MASS_STOR_SCL_IDE) 
         //   ) {
            Status = EFI_SUCCESS;
       // }
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
ASM1061RInitIdeController (
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

/**
    This function can be used to obtain information about a specified channel.
    It's usually used by IDE Bus driver during enumeration process.

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Channel - Channel number (0 based, either 0 or 1)
    @param Enabled - TRUE if the channel is enabled. If the channel is disabled,
        then it will no be enumerated.
    @param MaxDevices - The Max number of IDE devices that the bus driver can expect
        on this channel. For ATA/ATAPI, this number is either 1 or 2.

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN   UINT8                               Channel,
  OUT  BOOLEAN                             *Enabled,
  OUT  UINT8                               *MaxDevices
  )
{
    //
    // Channel number (0 based, either 0 or 1)
    //

    if (Channel < This->ChannelCount) {
        *Enabled    = TRUE;
        *MaxDevices = CONTROLLER_MAX_DEVICES;

        return EFI_SUCCESS;

    } else {
        *Enabled = FALSE;
        return EFI_INVALID_PARAMETER;
    }
}

//**********************************************************************
/**
    This function is called by IdeBus driver before executing certain actions.
    This allows IDE Controller Init to prepare for each action.

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Phase - phase indicator defined by IDE_CONTROLLER_INIT protocol
    @param Channel - Channel number (0 based, either 0 or 1)

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  IN  UINT8                             Channel
  )
{
    if (Channel >= This->ChannelCount) {
        return EFI_INVALID_PARAMETER;
    }

    switch (Phase) {

    case EfiIdeBeforeChannelEnumeration:
    case EfiIdeAfterChannelEnumeration:
    case EfiIdeBeforeChannelReset:
    case EfiIdeAfterChannelReset:
    case EfiIdeBusBeforeDevicePresenceDetection:
    case EfiIdeBusAfterDevicePresenceDetection:
    case EfiIdeResetMode:
        //
        // Do nothing at present
        //
        break;

    default:
        return EFI_UNSUPPORTED;
        break;
    }

    return EFI_SUCCESS;
}

UINT8
ASM1061RGetMaxChannels(VOID)
{
 return CONTROLLER_MAX_CHANNELS;
}
/**
    This function is called by IdeBus driver to submit EFI_IDENTIFY_DATA data structure
    obtained from IDE deivce. This structure is used to set IDE timing

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param IdentifyData - A pointer to EFI_IDENTIFY_DATA data structure

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitSubmitData (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_IDENTIFY_DATA                   *IdentifyData
  )
{
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;
    PrivateData = ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
    ASSERT (PrivateData);

    if ((Channel >= This->ChannelCount) || \
                            (Device >= CONTROLLER_MAX_DEVICES)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Make a local copy of device's IdentifyData and mark the valid flag
    //
    if (IdentifyData != NULL) {
        pBS->CopyMem (
          &(PrivateData->IdentifyData[Channel][Device]),
          IdentifyData,
          sizeof (EFI_IDENTIFY_DATA)
          );

        PrivateData->IdentifyValid[Channel][Device] = TRUE;
    } else {
        PrivateData->IdentifyValid[Channel][Device] = FALSE;
    }

    return EFI_SUCCESS;
}

//**********************************************************************
/**
    This function is called by IdeBus driver to disqualify unsupported operation
    mode on specfic IDE device

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param BadModes - Operation mode indicator

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitDisqualifyMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_ATA_COLLECTIVE_MODE             *BadModes
  )
{
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;
    PrivateData = ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
    ASSERT (PrivateData);

    if ((Channel >= This->ChannelCount) || \
        (BadModes == NULL) || (Device >= CONTROLLER_MAX_DEVICES)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Record the disqualified modes per channel per device. From ATA/ATAPI spec,
    // if a mode is not supported, the modes higher than it is also not
    // supported
    //
    pBS->CopyMem (
      &(PrivateData->DisqulifiedModes[Channel][Device]),
      BadModes,
      sizeof (EFI_ATA_COLLECTIVE_MODE)
      );

    return EFI_SUCCESS;
}

//**********************************************************************
/**
    This function is called by IdeBus driver to calculate the best operation mode
    supported by specific IDE device

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param SupportedModes - Modes collection supported by IDE device

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitCalculateMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL       *This,
  IN  UINT8                                  Channel,
  IN  UINT8                                  Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE             **SupportedModes
  )
{
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;
    EFI_IDENTIFY_DATA                   *IdentifyData;
    BOOLEAN                             IdentifyValid;
    EFI_ATA_COLLECTIVE_MODE             *DisqulifiedModes;
    UINT16                              SelectedMode;
    EFI_STATUS                          Status;

    PrivateData = ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
    ASSERT (PrivateData);

    if ((Channel >= This->ChannelCount) || \
        (SupportedModes == NULL) || (Device >= CONTROLLER_MAX_DEVICES)) {
        return EFI_INVALID_PARAMETER;
    }

    *SupportedModes = MallocZ (sizeof (EFI_ATA_COLLECTIVE_MODE));

    IdentifyData      = &(PrivateData->IdentifyData[Channel][Device]);
    DisqulifiedModes  = &(PrivateData->DisqulifiedModes[Channel][Device]);
    IdentifyValid     = PrivateData->IdentifyValid[Channel][Device];

    //
    // Make sure we've got the valid identify data
    // of the device from SubmitData()
    //
    if (!IdentifyValid) {
        return EFI_NOT_READY;
    }

    Status = ASM1061RCalculateBestPioMode (
              IdentifyData,
              (DisqulifiedModes->PioMode.Valid ?\
                ((UINT16 *) &(DisqulifiedModes->PioMode.Mode)) : NULL),
              &SelectedMode
              );
    if (!EFI_ERROR (Status)) {
        (*SupportedModes)->PioMode.Valid  = TRUE;
        (*SupportedModes)->PioMode.Mode   = SelectedMode;

    } else {
        (*SupportedModes)->PioMode.Valid = FALSE;
    }

    Status = ASM1061RCalculateBestUdmaMode (
              IdentifyData,
              (DisqulifiedModes->UdmaMode.Valid ? \
                ((UINT16 *) &(DisqulifiedModes->UdmaMode.Mode)) : NULL),
              &SelectedMode
              );

    if (!EFI_ERROR (Status)) {
        (*SupportedModes)->UdmaMode.Valid = TRUE;
        (*SupportedModes)->UdmaMode.Mode  = SelectedMode;

    } else {
        (*SupportedModes)->UdmaMode.Valid = FALSE;
    }

    //
    // User select IDE mode
    //
  //  UserIdeMode(&gSetupData, Channel, Device, SupportedModes);

    //
    // The modes other than PIO and UDMA are not supported by Ide controller
    //
    return EFI_SUCCESS;
}


//**********************************************************************
/**
    This function is called by IdeBus driver to set appropriate timing on IDE
    controller according supported operation mode

        
    @param This - the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
    @param Channel - IDE channel number (0 based, either 0 or 1)
    @param Device - IDE device number
    @param Modes 

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RIdeInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_ATA_COLLECTIVE_MODE             *Modes
  )
{
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;
    PrivateData = ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
    ASSERT (PrivateData);

    if ((Channel >= This->ChannelCount) || \
            (Modes == NULL) || (Device >= CONTROLLER_MAX_DEVICES)) {
        return EFI_INVALID_PARAMETER;
    }

    //
    // Make sure we've got the valid identify data of the device
    // from SubmitData()
    //
    if (!(PrivateData->IdentifyValid[Channel][Device]) ) {
        return EFI_NOT_READY;
    }

    //
    // Only PIO mode and UDMA mode are supported
    //

    //if UDMA is supported, Set UDMA Mode
//    if (Modes->UdmaMode.Valid) {
//        IdeInitSetUdmaTiming (Channel, Device, PrivateData->PciIo, Modes);
//    }

//;<JohnZhang Debug Begin ----->
//    //If PIO is supported, Set PIO Mode
//    if (Modes->PioMode.Valid) {
//        IdeInitSetPioTiming(Channel, Device, PrivateData->PciIo, &(PrivateData->IdentifyData[Channel][Device]), Modes);
//    }
//;<JohnZhang Debug End   <-----

//    //If Pio and UDMA are not supported, return EFI_UNSUPPORTED
//    if ((!Modes->PioMode.Valid) && (!Modes->UdmaMode.Valid)) {
//        return EFI_UNSUPPORTED;
//    }

    return EFI_SUCCESS;
}

//**********************************************************************
/**
    This function is used to calculate the best PIO mode supported by
    specific IDE device

        
    @param IdentifyData The identify data of specific IDE device
    @param DisPioMode OPTIONAL   - Disqualified PIO modes collection
    @param SelectedMode Available PIO modes collection

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RCalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA    * IdentifyData,
  IN  UINT16               *DisPioMode OPTIONAL,
  OUT UINT16               *SelectedMode
  )
{
    UINT16  PioMode;
    UINT16  AdvancedPioMode;
    UINT16  Temp;
    UINT16  Index;
    UINT16  MinimumPioCycleTime;

    Temp    = 0xff;

    //######  PioMode = (UINT8) (IdentifyData->AtaData.pio_cycle_timing >> 8);
    //###### no such item in MdePkg use following item instead.
    PioMode = (UINT8) (IdentifyData->AtaData.obsolete_51_52[0] >> 8);

    //
    // see whether Identify Data word 64 - 70 are valid
    //
    if ((IdentifyData->AtaData.field_validity & 0x02) == 0x02) {

        AdvancedPioMode = IdentifyData->AtaData.advanced_pio_modes;

        for (Index = 0; Index < 8; Index++) {
            if ((AdvancedPioMode & 0x01) != 0) {
                Temp = Index;
            }

            AdvancedPioMode >>= 1;
        }

        //
        // if Temp is modified, meant the advanced_pio_modes is not zero;
        // if Temp is not modified, meant the no advanced
        // PIO Mode is supported,
        // the best PIO Mode is the value in pio_cycle_timing.
        //
        if (Temp != 0xff) {
            AdvancedPioMode = (UINT16) (Temp + 3);
        } else {
            AdvancedPioMode = PioMode;
        }

        //
        // Limit the PIO mode to at most PIO4.
        //
        PioMode = (UINT16) (AdvancedPioMode < 4 ? AdvancedPioMode : 4);

        MinimumPioCycleTime = \
            IdentifyData->AtaData.min_pio_cycle_time_with_flow_control;

        if (MinimumPioCycleTime <= 120) {
            PioMode = (UINT16) (4 < PioMode ? 4 : PioMode);
        } else if (MinimumPioCycleTime <= 180) {
            PioMode = (UINT16) (3 < PioMode ? 3 : PioMode);
        } else if (MinimumPioCycleTime <= 240) {
            PioMode = (UINT16) (2 < PioMode ? 2 : PioMode);
        } else {
            PioMode = 0;
        }

        //
        // Degrade the PIO mode if the mode has been disqualified
        //
        if (DisPioMode != NULL) {

          if (*DisPioMode < 2) {
                return EFI_UNSUPPORTED; // no mode below ATA_PIO_MODE_BELOW_2
          }

          if (PioMode >= *DisPioMode) {
                PioMode = (UINT16)(*DisPioMode - 1);
          }
        }

        if (PioMode < 2) {
            *SelectedMode = 1;       // ATA_PIO_MODE_BELOW_2;

        } else {
            *SelectedMode = PioMode; // ATA_PIO_MODE_2 to ATA_PIO_MODE_4;
        }

    } else {
        //
        // Identify Data word 64 - 70 are not valid
        // Degrade the PIO mode if the mode has been disqualified
        //
        if (DisPioMode != NULL) {

          if (*DisPioMode < 2) {
                return EFI_UNSUPPORTED; // no mode below ATA_PIO_MODE_BELOW_2
          }

          if (PioMode == *DisPioMode) {
                PioMode--;
          }
        }

        if (PioMode < 2) {
            *SelectedMode = 1; // ATA_PIO_MODE_BELOW_2;
        } else {
            *SelectedMode = 2; // ATA_PIO_MODE_2;
        }

    }

    return EFI_SUCCESS;
}

//**********************************************************************
/**
    This function is used to calculate the best UDMA mode supported by
    specific IDE device

        
    @param IdentifyData The identify data of specific IDE device
    @param DisUDmaMode OPTIONAL  - Disqualified UDMA modes collection
    @param SelectedMode Available UMDA modes collection

          
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
//**********************************************************************
EFI_STATUS
ASM1061RCalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA    * IdentifyData,
  IN  UINT16               *DisUDmaMode OPTIONAL,
  OUT UINT16               *SelectedMode
  )
{
    UINT16  TempMode;
    UINT16  DeviceUDmaMode;

    DeviceUDmaMode = 0; // flag for 'Udma mode is not supported'

    //
    // Check whether the WORD 88 (supported UltraDMA by drive) is valid
    //
    if ((IdentifyData->AtaData.field_validity & 0x04) == 0x00) {
        return EFI_UNSUPPORTED;
    }

    DeviceUDmaMode = IdentifyData->AtaData.ultra_dma_mode;
    DeviceUDmaMode &= 0x7f;
    TempMode        = 0; // initialize it to UDMA-0

    while ((DeviceUDmaMode >>= 1) != 0) {
        TempMode++;
    }

    //
    // Degrade the UDMA mode if the mode has been disqualified
    //
    if (DisUDmaMode != NULL) {
        if (*DisUDmaMode == 0) {
            *SelectedMode = 0;
            return EFI_UNSUPPORTED; // no mode below ATA_UDMA_MODE_0
        }

        if (TempMode >= *DisUDmaMode) {
            TempMode = (UINT16) (*DisUDmaMode - 1);
        }
    }

    //
    // Possible returned mode is between ATA_UDMA_MODE_0 and ATA_UDMA_MODE_5
    //
    *SelectedMode = TempMode;

    return EFI_SUCCESS;
}
/**
    Checks whether the given controller is IDE  or not

        
    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param Controller handle of controller
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  
  Here is the control flow of this function:
  1. Check whether PCI and Devicepath protocol has been installed on this controller
  2. Check if the controller is IDE controller
  3. If IDEController protocol already installed, return EFI_ALREADY_STARTED
  4. Return EFI_SUCCESS

**/
//**********************************************************************
EFI_STATUS
EFIAPI
ASM1061RIdeControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  Controller,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
    EFI_STATUS                  Status;
//    EFI_DEVICE_PATH_PROTOCOL    *ParentDevicePath;
    EFI_PCI_IO_PROTOCOL         *PciIo;

    //
    // Ide Controller is a device driver, and should ingore the
    // "RemainingDevicePath" according to EFI spec
    //
    //DEBUG((-1,"JOY DEBGU 11\n"));
/*    Status = pBS->OpenProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &ParentDevicePath,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_BY_DRIVER
                    );
    if (EFI_ERROR (Status)) {
      //
      // EFI_ALREADY_STARTED is also an error
      //
      return Status;
    }

    //
    // Close the protocol because we don't use it here
    //
    pBS->CloseProtocol (
          Controller,
          &gEfiDevicePathProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );
*/
    //
    // Now test the EfiPciIoProtocol
    //
    Status = pBS->OpenProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Check for the desired IDE controller
    //
    Status = ASM1061RIdeControllerCheck(PciIo);
    //
    // close protocol to avoid to carry it on for multiple exit points.
    //
    pBS->CloseProtocol (
          Controller,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );

    if (EFI_ERROR (Status)) return  EFI_UNSUPPORTED;


  return Status;
}


//**********************************************************************
/**
    Installs IdeControllerProtocol

        
    @param This pointer to EFI_DRIVER_BINDING_PROTOCOL structure
    @param Controller handle of controller
    @param RemainingDevicePath pointer to EFI_DEVICE_PATH_PROTOCOL structure

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  
  Here is the control flow of this function:
  1. Check whether PCI and Devicepath protocol has been installed on this controller
  2. Check if the controller is IDE controller
  3. If "No", return  EFI_UNSUPPORTED
  5.Install IDEControllerprotocol and return EFI_SUCCESS.

**/
//**********************************************************************
EFI_STATUS
EFIAPI
ASM1061RIdeControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
    EFI_STATUS                          Status;
    EFI_PCI_IO_PROTOCOL                 *PciIo;
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData = NULL;

    //
    // Now test and open the EfiPciIoProtocol
    //
    Status = pBS->OpenProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    //
    // Status == 0 - A normal execution flow, SUCCESS and the program proceeds.
    // Status == ALREADY_STARTED - A non-zero Status code returned. It indicates
    //           that the protocol has been opened and should be treated as a
    //           normal condition and the program proceeds. The Protocol will not
    //           opened 'again' by this call.
    // Status != ALREADY_STARTED - Error status, terminate program execution
    //
    if (EFI_ERROR (Status)) {
        //
        // EFI_ALREADY_STARTED is also an error
        //
        return Status;
    }
    //
    // Allocate Ide controller private data structure
    //

    PrivateData = MallocZ(sizeof(ASM1061R_IDE_CONTROLLER_PRIVATE_DATA));
    if (PrivateData == NULL) {
        TRACE ((TRACE_ALWAYS,
            "Ide Controller START ERROR: \
                Allocating pool for IdePrivateData failed!\n"));
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
    }

    //
    // Initialize Ide controller private data
    //
    PrivateData->Signature              = CONTROLLER_SIGNATURE;
    PrivateData->PciIo                  = PciIo;
    PrivateData->IdeInit.GetChannelInfo = ASM1061RIdeInitGetChannelInfo;
    PrivateData->IdeInit.NotifyPhase    = ASM1061RIdeInitNotifyPhase;
    PrivateData->IdeInit.SubmitData     = ASM1061RIdeInitSubmitData;
    PrivateData->IdeInit.DisqualifyMode = ASM1061RIdeInitDisqualifyMode;
    PrivateData->IdeInit.CalculateMode  = ASM1061RIdeInitCalculateMode;
    PrivateData->IdeInit.SetTiming      = ASM1061RIdeInitSetTiming;
    PrivateData->IdeInit.EnumAll        = IDE_ENUMER_ALL;
    PrivateData->IdeInit.ChannelCount   = ASM1061RGetMaxChannels();
    // post code
   // PROGRESS_CODE(DXE_SB_DEVICES_INIT);
    //
    // Enable Ide controller working in appropriate mode
    //
    Status = ASM1061RInitIdeController(PciIo);

    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }

    //
    // Install IDE_CONTROLLER_INIT protocol & private data to this instance
    //
    Status = pBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gControllerDriverGuid,
                  PrivateData,
                  &gEfiIdeControllerInitProtocolGuid,
                  &(PrivateData->IdeInit),
                  NULL
                  );

Done:

    if (EFI_ERROR (Status)) {

        pBS->CloseProtocol (
              Controller,
              &gEfiPciIoProtocolGuid,
              This->DriverBindingHandle,
              Controller
              );
        if (PrivateData != NULL) {
            pBS->FreePool (PrivateData);
        }
    }

    return Status;
}

//**********************************************************************
/**
    Installs IdeControllerProtocol

        
    @param This Protocol instance pointer.
    @param Controller Handle of device to stop driver on
    @param ChildHandleBuffer Number of Handles in ChildHandleBuffer. If number of
        children is zero stop the entire bus driver.
    @param ChildHandleBuffer List of Child Handles to Stop.

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  
  Here is the control flow of this function:
  1. Check whether PCI and Devicepath protocol has been installed on this controller
  2. Check if the controller is IDE controller
  3. If "No", return  EFI_UNSUPPORTED
  5.Install IDEControllerprotocol and return EFI_SUCCESS.

**/
//**********************************************************************
EFI_STATUS
EFIAPI
ASM1061RIdeControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
  IN  EFI_HANDLE                      Controller,
  IN  UINTN                           NumberOfChildren,
  IN  EFI_HANDLE                      *ChildHandleBuffer
  )
{
    EFI_STATUS                          Status;
    ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;

    //
    // Get private data
    //
    Status = pBS->OpenProtocol (
                    Controller,
                    &gControllerDriverGuid,
                    (VOID **) &PrivateData,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );

    if (!EFI_ERROR (Status)) {
        pBS->UninstallMultipleProtocolInterfaces (
              Controller,
              &gControllerDriverGuid,
              PrivateData,
              &gEfiIdeControllerInitProtocolGuid,
              &(PrivateData->IdeInit),
              NULL
              );
    }

    //
    // Close protocols opened by Ide controller driver
    //
    pBS->CloseProtocol (
          Controller,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );

    if (PrivateData != NULL) {
        pBS->FreePool (PrivateData);
    }

    return EFI_SUCCESS;
}


EFI_STATUS ASM1061REntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
    UINT64                  SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                  SetupVariableAttributes=0;
    EFI_GUID                gSetupGuid = SETUP_GUID;
    SETUP_DATA              SetupData;

    InitAmiLib(ImageHandle, SystemTable);
    Status = pRS->GetVariable (L"Setup",
                                &gSetupGuid,
                                &SetupVariableAttributes,
                                &SetupVariableSize,
                                &SetupData);
    if ((Status != EFI_SUCCESS) || (SetupData.ASM1061R_SataMode == 2)) {
        return EFI_SUCCESS;
    }
    //
    // Driver Binding Protocol install
    //
    gASM1061RIdeControllerDriverBinding.DriverBindingHandle=ImageHandle;
    gASM1061RIdeControllerDriverBinding.ImageHandle=ImageHandle;
    Status = pBS->InstallMultipleProtocolInterfaces(
                &ImageHandle,
                &gEfiDriverBindingProtocolGuid,
                &gASM1061RIdeControllerDriverBinding,
                &gEfiComponentNameProtocolGuid,
                &gASM1061RComponentNameProtocol,
                NULL
                );
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

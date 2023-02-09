/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 ******************************************************************************
 */

#include "ASM1061RDriver.h"

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
  EFI_STATUS                      Status;
  PCI_DEVICE_INDEPENDENT_REGION   PciData;
  //
  // Now further check the PCI header: Base class (offset 0x0B) and
  // Sub Class (offset 0x0A). This controller should be an SATA controller
  //
  Status = PciIo->Pci.Read (
      PciIo,
      EfiPciIoWidthUint8,
      0,
      sizeof (PCI_DEVICE_INDEPENDENT_REGION),
      &PciData
  );
  if (EFI_ERROR (Status)) return Status;

  Status = EFI_UNSUPPORTED;
  if ((PciData.ClassCode[2] == PCI_CLASS_MASS_STORAGE) && 
     ((PciData.VendorId == ASM1061RVendorID) && (PciData.DeviceId == ASM1061RDeviceID ))) {
    Status = EFI_SUCCESS;
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
    gBS->CopyMem (
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
  gBS->CopyMem (
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

  *SupportedModes = (EFI_ATA_COLLECTIVE_MODE *)AllocateZeroPool (sizeof (EFI_ATA_COLLECTIVE_MODE));

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
    Read Byte from Index-Data Pair

    @param PciIo - PciIo protocol.
    @param Register - The Register will be read.

    @Output: Read value.

 **/
UINT8
ASM1061RIdpReadByte (
    IN EFI_PCI_IO_PROTOCOL   *PciIo,
    IN UINT32                Register
)
{
  UINT32 Index;
  UINT8 bValue;
  UINT8 Offset;

  //Index shall be DWORD alignment
  Index = Register & (~0x03);
  Offset = Register % 4;  

  DEBUG((EFI_D_ERROR, "[%a][%d]Index:%x Offsert:%x\n\n",__FUNCTION__,__LINE__,Index,Offset));
  //Write Index    
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint32, ASM1061R_MMIO_INDEX, 1, &Index);

  //Read BYTE Data
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, (ASM1061R_MMIO_DATA + Offset), 1, &bValue);

  DEBUG((EFI_D_ERROR, "[%a][%d]Register %04x  DataRead:%x\n\n",__FUNCTION__,__LINE__,Register,bValue));
  return bValue;

}

/**
    Write Byte from Index-Data Pair
    @param PciIo - PciIo protocol.
    @param Register - The Register will be read.
    @param bValue - The value to write to Register.

    @Output: none.

 **/
VOID
ASM1061RIdpWriteByte (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT32                   Register,
    IN UINT8                    bValue
)
{

  UINT32 Index;
  UINT8 Offset;
  //Index shall be DWORD alignment
  Index = Register & (~0x03);
  Offset = Register % 4;

  DEBUG((EFI_D_ERROR, "[%a][%d]Index:%x Offsert:%x\n\n",__FUNCTION__,__LINE__,Index,Offset));
  //Write Index
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint32, ASM1061R_MMIO_INDEX, 1, &Index);

  //Write BYTE Data
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, (ASM1061R_MMIO_DATA + Offset), 1, &bValue);
}

/**
    Set the AHCI class code.
    @param PciIo - PciIo protocol.

    @retval None.

    @note  None.

 **/
VOID
ASM1061RSetAhciClass (
    IN EFI_PCI_IO_PROTOCOL   *PciIo
)
{
  UINT8                       Data8;
  //(1)Enable Subclass to be programmed:Set PCI RxF0[0] = 1 (BACKDOOR_CTL enable)
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xF0, 1, &Data8);
  Data8 |= 0x01;
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0xF0, 1, &Data8);

  //(2)Set Subclass Code:Modify PCI Rx0A = 06h and Rx0B = 01h
  Data8 = 0x06;
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0x0A, 1, &Data8);
  Data8 = 0x01;
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0x0B, 1, &Data8);

  //(3)Disable Subclass to be programmed:Set PCI RxF0[0] = 0        
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xF0, 1, &Data8);
  Data8 &= 0xFE;
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0xF0, 1, &Data8);
}

/**
    Set the Spin-Up SATA device.

    @param PciIo - PciIo protocol.

    @retval None.

    @note  None.

 **/
VOID
ASM1061RSpinUp (
    IN EFI_PCI_IO_PROTOCOL   *PciIo
)
{
  UINT8                   bData;
  // Set MMIO Rx0D0A[1:0] = 2:11b
  bData = ASM1061RIdpReadByte(PciIo, 0x0D0A);
  // you can delay some time to program bit 1 after bit 0 was programed to supported staggered spin-up
  bData |= 0x03; //bit[1:0] = 3:11b
  ASM1061RIdpWriteByte(PciIo, 0x0D0A, bData);
}

/**
    Disable Safety Removal for AHCI Mode.

    @param PciIo - PciIo protocol.

    @retval None.

    @note  None.

 **/
VOID
ASM1061RDisableSafetyRemoval (
    IN EFI_PCI_IO_PROTOCOL   *PciIo
)
{
  UINT8                   bData;

  // MMIO Rx0D00[5] = 0 Set HBA CAP.SXS = 0
  bData = ASM1061RIdpReadByte(PciIo, 0x0D00);
  bData &= ~0x20; //bit 5 = 0
  ASM1061RIdpWriteByte(PciIo, 0x0D00, bData);
  // MMIO Rx0DC8[3] = 0 Set Port 0 PxCMD.HPCP = 0
  // MMIO Rx0DC8[6] = 1 Set Port 0 PxCMD.ESP = 1
  bData = ASM1061RIdpReadByte(PciIo, 0x0DC8);
  bData &= ~0x08; //bit 3 = 0
  bData |= 0x40; //bit 6 = 1
  ASM1061RIdpWriteByte(PciIo, 0x0DC8, bData);
  // MMIO Rx0DC9[3] = 0 Set Port 0 PxCMD.HPCP = 0
  // MMIO Rx0DC9[6] = 1 Set Port 0 PxCMD.ESP = 1
  bData = ASM1061RIdpReadByte(PciIo, 0x0DC9);
  bData &= ~0x08; //bit 3 = 0
  bData |= 0x40; //bit 6 = 1
  ASM1061RIdpWriteByte(PciIo, 0x0DC9, bData);
}

/**
    Disable SATA Device Sleep.

    @param PciIo - PciIo protocol.

    @retval None.

    @note  None.

 **/
VOID
ASM1061RDisableDeviceSleep (
    IN EFI_PCI_IO_PROTOCOL   *PciIo
)
{
  UINT8                   bData;

  // Set MMIO Rx0D08[4:3] = 2 00b
  bData = ASM1061RIdpReadByte(PciIo, 0x0D08);
  bData &= ~0x18; //bit[4:3] = 2 00b
  ASM1061RIdpWriteByte(PciIo, 0x0D08, bData);
}


/**
    Set the Bus Number for PCI Bridge.

    @param PciIo - PciIo protocol.
    @param PciBusNum - The Bus Number would be set.

    @retval None.

    @note  None.

 **/
VOID
SetPciBridgerBusNum (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT8                    PciBusNum )
{
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0x19, 1, &PciBusNum);
  PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0x1A, 1, &PciBusNum);
}


/**
    Search and return the Capability ID Address by FindCapNo.

    @param PciIo - PciIo protocol.
    @param FindCapNo - The Capability ID would be searched.
    @param CapPtr8 - Pointer to the Capability ID Address.

    @retval EFI_STATUS.

    @note  None.

 **/
EFI_STATUS
GetLegCapStrucAddr (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 )
{
  UINT8                       Data8;

  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x34, 1, &Data8);
  *CapPtr8 = Data8;
  if ((*CapPtr8 == 0) || (*CapPtr8 == 0xFF)) return EFI_NOT_FOUND;

  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, *CapPtr8, 1, &Data8);
  while (Data8 != 0) {
    if (Data8 == FindCapNo) return EFI_SUCCESS;

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, (*CapPtr8 + 1), 1, &Data8);
    *CapPtr8 = Data8;
    if (*CapPtr8 == 0) break;

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, (*CapPtr8), 1, &Data8);
  }
  return EFI_NOT_FOUND;
}

/**
    Init SATA Device ASM1061.

    @param PciIo - PciIo protocol.

    @retval None.

    @note  None.

 **/
EFI_STATUS
ASM1061RInit (
    EFI_PCI_IO_PROTOCOL   *PciIo
)
{
  EFI_STATUS              Status;
  UINT32                  Reg32;
  UINTN                   SegNum, Bus, Dev, Func;

  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &Reg32);
  //DEBUG((EFI_D_ERROR, "ASM1061RInit VID/DID:0x%08X\n", Reg32));

  if((Reg32 != ASM1061RDIDVID)) {
    return EFI_DEVICE_ERROR;
  }

  Status = PciIo->GetLocation(PciIo, &SegNum, &Bus, &Dev, &Func);
  DEBUG((EFI_D_ERROR, "ASM1061RInit BDF:%x:%x.%x\n", Bus, Dev, Func));

  //1.Program PCI Class Code
  ASM1061RSetAhciClass(PciIo);

  //2.Program PCI SVID and SSID (optional, porting later)   
  //    (1). Enable SVID and SSID to be programmed
  //    Set PCI RxF0[0] = 1
  //    (2). Set SVID and SSID
  //    Modify PCI Rx2C ~ Rx2F to required values
  //    (3). Disable SVID and SSID to be programmed
  //    Set PCI RxF0[0] = 0

  //3. Spin-Up SATA device 
  ASM1061RSpinUp (PciIo);   

  //4. Disable Safety Removal for AHCI Mode
  ASM1061RDisableSafetyRemoval (PciIo);   

  //5. Disable SATA Device Sleep
  ASM1061RDisableDeviceSleep (PciIo);

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
  EFI_PCI_IO_PROTOCOL         *PciIo;

  //
  // Now test the EfiPciIoProtocol
  //
  Status = gBS->OpenProtocol (
      Controller,
      &gEfiPciIoProtocolGuid,
      (VOID **) &PciIo,
      This->DriverBindingHandle,
      Controller,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL
  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  //Init device
  Status = ASM1061RInit(PciIo);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Check for the desired IDE controller
  //
  //Status = ASM1061RIdeControllerCheck(PciIo);

  //
  // close protocol to avoid to carry it on for multiple exit points.
  //
  gBS->CloseProtocol (
      Controller,
      &gEfiPciIoProtocolGuid,
      This->DriverBindingHandle,
      Controller
  );

  return  (EFI_ERROR (Status)) ? EFI_UNSUPPORTED : EFI_SUCCESS;
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
  Status = gBS->OpenProtocol (
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

  PrivateData = (ASM1061R_IDE_CONTROLLER_PRIVATE_DATA *)AllocateZeroPool(sizeof(ASM1061R_IDE_CONTROLLER_PRIVATE_DATA));
  if (PrivateData == NULL) {
    DEBUG ((DEBUG_INFO, "Ide Controller START ERROR: Allocating pool for IdePrivateData failed!\n"));
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
  Status = gBS->InstallMultipleProtocolInterfaces (
      &Controller,
      &gControllerDriverGuid,
      PrivateData,
      &gEfiIdeControllerInitProtocolGuid,
      &(PrivateData->IdeInit),
      NULL
  );

  Done:

  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
    );
    if (PrivateData != NULL) {
      gBS->FreePool (PrivateData);
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
  EFI_STATUS                             Status;
  ASM1061R_IDE_CONTROLLER_PRIVATE_DATA   *PrivateData;

  //
  // Get private data
  //
  Status = gBS->OpenProtocol (
      Controller,
      &gControllerDriverGuid,
      (VOID **) &PrivateData,
      This->DriverBindingHandle,
      Controller,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL
  );

  if (!EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
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
  gBS->CloseProtocol (
      Controller,
      &gEfiPciIoProtocolGuid,
      This->DriverBindingHandle,
      Controller
  );

  if (PrivateData != NULL) {
    gBS->FreePool (PrivateData);
  }

  return EFI_SUCCESS;
}


EFI_STATUS ASM1061REntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS  Status;

  //
  // Driver Binding Protocol install
  //
  gASM1061RIdeControllerDriverBinding.DriverBindingHandle=ImageHandle;
  gASM1061RIdeControllerDriverBinding.ImageHandle=ImageHandle;
  Status = gBS->InstallMultipleProtocolInterfaces(
      &ImageHandle,
      &gEfiDriverBindingProtocolGuid,
      &gASM1061RIdeControllerDriverBinding,
      &gEfiComponentNameProtocolGuid,
      &gASM1061RComponentNameProtocol,
      NULL
  );
  return Status;
}


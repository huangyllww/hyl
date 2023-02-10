//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file EfiUsbMass.c
    EFI USB Mass Storage Driver

**/

#include "AmiDef.h"
#include "UsbDef.h"
#include "Uhcd.h"
#include "ComponentName.h"
#include "UsbBus.h"
#include <Setup.h>
#include "UsbMass.h"

#if (defined(BOOT_SECTOR_WRITE_PROTECT) && (BOOT_SECTOR_WRITE_PROTECT != 0))
#include <Protocol/AmiBlockIoWriteProtection.h>
AMI_BLOCKIO_WRITE_PROTECTION_PROTOCOL *AmiBlkWriteProtection = NULL;
#endif

#define USBMASS_DRIVER_VERSION 1
#define READ 1
#define WRITE 0

extern  USB_GLOBAL_DATA     *gUsbData;
extern  EFI_USB_PROTOCOL    *gAmiUsbController;


/**
    Installs BlkIo protocol on a USB Mass Storage device

    @param  Controller - Handle number of controller
            DevInfo - pointer to a USB device structure to install the protocol.

    @retval EFI_STATUS Status of the operation

**/


EFI_STATUS
InstallUsbMass(
    EFI_HANDLE Controller,
    DEV_INFO* DevInfo
)
{
    USB_MASS_DEV    *MassDev;
    EFI_STATUS      Status;
    UINT8           LogicalAddress;
    URP_STRUC       Parameters;

    //Applying check to media not present device only
    if (!(DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT)) {
        if (gUsbData->dUSBStateFlag & USB_FLAG_MASS_MEDIA_CHECK) {
            if (gUsbData->dUSBStateFlag & USB_FLAG_MASS_SKIP_FDD_MEDIA_CHECK) {
                if (!(DevInfo->bSubClass == SUB_CLASS_UFI)) {
                    return EFI_DEVICE_ERROR;
                }
            } else {
                return EFI_DEVICE_ERROR;
            }
        }
    }

    for (LogicalAddress=1; LogicalAddress < MAX_DEVICES; LogicalAddress++) {
        if (&gUsbData->aDevInfoTable[LogicalAddress] == DevInfo) {
            break;
        }
    }
    ASSERT(LogicalAddress < MAX_DEVICES);
    
    if (LogicalAddress >= MAX_DEVICES) {
        return EFI_DEVICE_ERROR;
    }
    
    Parameters.bFuncNumber = USB_API_MASS_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_MASSAPI_GET_MEDIA_STATUS;
    Parameters.ApiData.MassGetDevSts.bDevAddr = LogicalAddress;
    Parameters.ApiData.MassGetDevSts.bDeviceStatus = 0;
    InvokeUsbApi(&Parameters);

    gBS->AllocatePool(EfiBootServicesData, sizeof(USB_MASS_DEV), (VOID**)&MassDev);

    //
    // Handshaking...
    //
    MassDev->Signature  = USB_MASS_DEV_SIGNATURE;
    MassDev->DevInfo    = DevInfo;
    DevInfo->MassDev    = (VOID*)&MassDev->BlockIoProtocol;
    MassDev->Handle     = Controller;
    MassDev->DevString  = (UINT8*)&DevInfo->DevNameString;
    MassDev->StorageType = DevInfo->bStorageType;

    //
    // Install BLOCK_IO protocol interface
    //
    gBS->AllocatePool(EfiBootServicesData, sizeof(EFI_BLOCK_IO_MEDIA), (VOID**)&MassDev->Media);

    MassDev->Media->MediaId             = 0;        // Media change indicator
    MassDev->Media->LogicalPartition    = FALSE;
    MassDev->Media->ReadOnly            = FALSE;
    MassDev->Media->WriteCaching        = FALSE;
    MassDev->Media->BlockSize           = DevInfo->wBlockSize;
    MassDev->Media->IoAlign             = 0;

    if (DevInfo->bLastStatus & USB_MASS_MEDIA_REMOVEABLE) {
        MassDev->Media->RemovableMedia = TRUE;
    } else {
        MassDev->Media->RemovableMedia = FALSE;
    }
    if ((DevInfo->bLastStatus & USB_MASS_MEDIA_PRESENT) && 
        (DevInfo->MaxLba != 0) && (DevInfo->wBlockSize != 0)) {
        // For SCSI devices, this is reported in the READ CAPACITY (16) parameter 
        // data Returned Logical Block Address field (see SBC-3) minus one.
        MassDev->Media->LastBlock = DevInfo->MaxLba - 1;   // LastBlock is 0-based
        MassDev->Media->MediaPresent = TRUE;
    } else {
        MassDev->Media->LastBlock = 0;
        MassDev->Media->MediaPresent = FALSE;
    }
  
    if (pST->Hdr.Revision >= 0x0002001F) {
        MassDev->BlockIoProtocol.Revision    = EFI_BLOCK_IO_PROTOCOL_REVISION3;
        //
        // Default value set to 1 logical blocks per PhysicalBlock
        //
        MassDev->Media->LogicalBlocksPerPhysicalBlock = 1;

        //
        // Default value set to 0 for Lowest Aligned LBA
        //
        MassDev->Media->LowestAlignedLba = 0;

        MassDev->Media->OptimalTransferLengthGranularity = MassDev->Media->BlockSize;
    } else {
        MassDev->BlockIoProtocol.Revision = 1;
    }

    MassDev->BlockIoProtocol.Media        = MassDev->Media;
    MassDev->BlockIoProtocol.Reset        = AmiUsbBlkIoReset;
    MassDev->BlockIoProtocol.ReadBlocks   = AmiUsbBlkIoReadBlocks;
    MassDev->BlockIoProtocol.WriteBlocks  = AmiUsbBlkIoWriteBlocks;
    MassDev->BlockIoProtocol.FlushBlocks  = AmiUsbBlkIoFlushBlocks;
    
    MassDev->DiskInfoProtocol.Interface = gEfiDiskInfoUsbInterfaceGuid;
    MassDev->DiskInfoProtocol.Inquiry   = DiskInfoInquiry;
    MassDev->DiskInfoProtocol.Identify  = DiskInfoIdentify;
    MassDev->DiskInfoProtocol.SenseData = DiskInfoSenseData;
    MassDev->DiskInfoProtocol.WhichIde  = DiskInfoWhichIDE;

    MassDev->LogicalAddress = LogicalAddress;

    MassDev->PciBDF = gUsbData->HcTable[DevInfo->bHCNumber - 1]->wBusDevFuncNum;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "InstallUSBMass(%x): BS %d, MaxLBA %lx, LA: %x %a\n",
        DevInfo, DevInfo->wBlockSize, DevInfo->MaxLba,
        MassDev->LogicalAddress, &DevInfo->DevNameString);

    Status = gBS->InstallMultipleProtocolInterfaces(
                    &MassDev->Handle,
                    &gEfiBlockIoProtocolGuid, &MassDev->BlockIoProtocol,
                    &gEfiDiskInfoProtocolGuid, &MassDev->DiskInfoProtocol,
                    NULL);
    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Install MassDev on %x status = %r\n", Controller, Status);
    ASSERT_EFI_ERROR(Status);

    return Status;
}

/**
    Removes BlkIo protocol from USB Mass Storage device

    @param MassDev - pointer to a USB device structure

    @retval EFI_STATUS Status of the operation

**/

EFI_STATUS
UninstallUSBMass(USB_MASS_DEV *MassDev)
{
    EFI_STATUS      Status;
	DEV_INFO		*DevInfo = MassDev->DevInfo;
    HC_STRUC*       HcData;
    UINT8           UsbStatus;

    HcData = gUsbData->HcTable[DevInfo->bHCNumber - 1];
 	UsbStatus = UsbDevDriverDisconnect(HcData, DevInfo);
	ASSERT(UsbStatus == USB_SUCCESS);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Uninstall mass storage device  %x: ", MassDev->Handle);
    
    Status = gBS->UninstallMultipleProtocolInterfaces(
                MassDev->Handle,
                &gEfiBlockIoProtocolGuid, &MassDev->BlockIoProtocol,
                &gEfiDiskInfoProtocolGuid, &MassDev->DiskInfoProtocol,
                NULL);

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "%r\n", Status);
    if (!EFI_ERROR(Status)) {
        gBS->FreePool(MassDev->Media);
        gBS->FreePool(MassDev);
        DevInfo->MassDev = NULL;
    }

    return Status;
}

/**
    Provides inquiry information for the controller type.

    @param  This: A pointer to the DiskInfo protocol USB interface
            InquiryData: A pointer to inquiry data
            InquiryDataSize: A pointer to inquiry data size
    @retval EFI_SUCCESS: The command was accepted without any errors.
            EFI_NOT_FOUND: Device does not support this data class
            EFI_DEVICE_ERROR: Error reading InquiryData from device
            EFI_BUFFER_TOO_SMALL: InquiryDataSize not big enough

**/
EFI_STATUS
EFIAPI
DiskInfoInquiry (
    IN  EFI_DISK_INFO_PROTOCOL  *This,
    IN  OUT VOID                *InquiryData,
    IN  OUT UINT32              *InquiryDataSize)
{
    URP_STRUC    Parameters;   
    USB_MASS_DEV *MassDev = USB_MASS_DEV_FROM_THIS(This, DiskInfoProtocol);

    if(*InquiryDataSize < sizeof (MASS_INQUIRY)) {
        *InquiryDataSize = sizeof (MASS_INQUIRY);
        return EFI_BUFFER_TOO_SMALL;
    }

    Parameters.bFuncNumber = USB_API_MASS_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_MASSAPI_GET_DEV_PARMS;
    Parameters.ApiData.MassGetDevParms.fpDevInfo = (DEV_INFO*)(MassDev->DevInfo);
    Parameters.ApiData.MassGetDevParms.fpInqData = NULL;
    InvokeUsbApi(&Parameters);

    if(Parameters.ApiData.MassGetDevParms.fpInqData != NULL) {
        gBS->CopyMem(InquiryData, Parameters.ApiData.MassGetDevParms.fpInqData, sizeof (MASS_INQUIRY));
        *InquiryDataSize = sizeof (MASS_INQUIRY);
        return EFI_SUCCESS;
    } else {
        return EFI_UNSUPPORTED;
    }
}

/**
    Provides identify information for the controller type.

    @param  This: A pointer to the DiskInfo protocol USB interface
            IdentifyData: A pointer to identify data
            IdentifyDataSize: A pointer to identify data size
    @retval EFI_SUCCESS: The command was accepted without any errors.
            EFI_NOT_FOUND: Device does not support this data class
            EFI_DEVICE_ERROR: Error reading IdentifyData from device
            EFI_BUFFER_TOO_SMALL: IdentifyDataSize not big enough

**/
EFI_STATUS
EFIAPI
DiskInfoIdentify (
    EFI_DISK_INFO_PROTOCOL  *This,
    IN  OUT VOID            *IdentifyData,
    IN  OUT UINT32          *IdentifyDataSize)
{
    URP_STRUC    Parameters;
    USB_MASS_DEV *MassDev = USB_MASS_DEV_FROM_THIS(This, DiskInfoProtocol);

    if(*IdentifyDataSize < sizeof (ATA_IDENTIFY_DATA)) {
        *IdentifyDataSize = sizeof (ATA_IDENTIFY_DATA);
        return EFI_BUFFER_TOO_SMALL;
    }

    if((((DEV_INFO*)(MassDev->DevInfo))->bProtocol == PROTOCOL_CBI) ||
       (((DEV_INFO*)(MassDev->DevInfo))->bProtocol == PROTOCOL_CBI_NO_INT)) {
        return EFI_UNSUPPORTED;
    }

    Parameters.bFuncNumber = USB_API_MASS_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_MASSAPI_GET_IDENTIFY_DATA;
    Parameters.ApiData.MassGetIdentifyData.DevInfo = (DEV_INFO*)(MassDev->DevInfo);
    Parameters.ApiData.MassGetIdentifyData.Identify = NULL;
    InvokeUsbApi(&Parameters);

    if(Parameters.ApiData.MassGetIdentifyData.Identify != NULL ) {
        gBS->CopyMem(IdentifyData, Parameters.ApiData.MassGetIdentifyData.Identify, sizeof (ATA_IDENTIFY_DATA));
        *IdentifyDataSize = sizeof (ATA_IDENTIFY_DATA);
        return EFI_SUCCESS;   
    } else {        
        return EFI_NOT_FOUND;
    }
}

/**
    Provides identify information for the controller type.

    @param  This: A pointer to the DiskInfo protocol USB interface
            SenseData: Pointer of sense data
            SenseDataSize: Pointer of sense data size
            SenseDataNumber: Pointer of sense data number
    @retval EFI_NOT_FOUND Not support this data

**/
EFI_STATUS
EFIAPI
DiskInfoSenseData (
    IN  EFI_DISK_INFO_PROTOCOL  *This,
    OUT VOID                    *SenseData,
    OUT UINT32                  *SenseDataSize,
    OUT UINT8                   *SenseDataNumber
)
{
    return EFI_NOT_FOUND;
}

/**
    Provides identify information for the controller type.

    @param  This: A pointer to the DiskInfo protocol USB interface
            IdeChannel: Pointer of ide channel
            IdeDevice: Pointer of ide device
    @retval EFI_NOT_FOUND Not support this data

**/
EFI_STATUS
EFIAPI
DiskInfoWhichIDE (
    IN  EFI_DISK_INFO_PROTOCOL  *This,
    OUT UINT32                  *IdeChannel,
    OUT UINT32                  *IdeDevice
)
{
    return EFI_UNSUPPORTED;
}

/************ BlockIO Protocol implementation routines******************/
/**
    Reset the USB Logic Drive

    @param This: A pointer to the Block I/O protocol interface

              ExtendedVerification: Indicate that the driver may perform
              an exhaustive verification operation of the device during
              reset

    @retval EFI_SUCCESS: The USB Logic Drive is reset
              EFI_DEVICE_ERROR: The Floppy Logic Drive is not functioning
              correctly and can not be reset

**/

EFI_STATUS
EFIAPI
AmiUsbBlkIoReset (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  BOOLEAN                ExtendedVerification
  )

{
    return  EFI_SUCCESS;
}


/**
    Flush USB Mass Storage Device

    @param This: A pointer to the Block I/O protocol interface

    @retval EFI_SUCCESS: The USB Logic Drive successfully flushed

**/

EFI_STATUS
EFIAPI
AmiUsbBlkIoFlushBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This
  )
{
    return EFI_SUCCESS;
}


/**
    This routine is invoked from AmiUsbBlkIoReadBlocks and
    AmiUsbBlkIoWriteBlocks. See these for parameters reference.
    @param  This: A pointer to the Block I/O protocol interface
            MediaId: Id of the media, changes every time the media is replaced
            Lba: The starting logic block address to read from on the device
            BufferSize: The size of the Buffer in bytes
            Buffer: A pointer to the destination buffer for the data
            ReadWrite: Read or Write data flag
            
    @retval EFI_SUCCESS: The command was accepted without any errors.
            EFI_INVALID_PARAMETER: Invalid parameter used
            EFI_BAD_BUFFER_SIZE: Wrong buffer size used 
            EFI_DEVICE_ERROR: Error happened on device
            EFI_NO_MEDIA: Media can not be found
            EFI_MEDIA_CHANGED: Media has been changed
**/

EFI_STATUS
AmiUsbBlkIoReadWrite (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  IN OUT VOID                *Buffer,
  UINT8                     ReadWrite
  )
{
    USB_MASS_DEV            *MassDev;
    URP_STRUC               Parameters;
    EFI_STATUS              Status = EFI_SUCCESS;
    UINTN                   Buf;
    UINT32                  BytesToTransfer;
    UINT32                  BytesRemaining;
    UINT16                  BlockSize;
    UINTN                   BufferAddress;
	UINT8					*DataBuffer;
	UINTN					Pages;
    DEV_INFO                *DevInfo;

    MassDev   = USB_MASS_DEV_FROM_THIS(This, BlockIoProtocol);

    //
    // Check if media id matches
    //
    if ( MassDev->BlockIoProtocol.Media->MediaId != MediaId ) {
        return EFI_MEDIA_CHANGED;
    }

    if (BufferSize == 0) return EFI_SUCCESS;
    if (Buffer == NULL) return EFI_INVALID_PARAMETER;

    //
    // If IoAlign values is 0 or 1, means that the buffer can be placed 
    // anywhere in memory or else IoAlign value should be power of 2. To be
    // properly aligned the buffer address should be divisible by IoAlign  
    // with no remainder. 
    // 
    BufferAddress = (UINTN)Buffer;
    if ((MassDev->BlockIoProtocol.Media->IoAlign > 1 ) && (BufferAddress % MassDev->BlockIoProtocol.Media->IoAlign)) {
        return EFI_INVALID_PARAMETER;
    }

    if (!(((DEV_INFO*)(MassDev->DevInfo))->Flag & DEV_INFO_DEV_PRESENT)) {
        return EFI_DEVICE_ERROR;
    }
    

    //
    // Get media status
    //
    MassDev->BlockIoProtocol.Media->MediaPresent = TRUE;   // Initialize, to be updated if no media

    Parameters.bFuncNumber = USB_API_MASS_DEVICE_REQUEST;
    Parameters.bSubFunc = USB_MASSAPI_GET_MEDIA_STATUS;
    Parameters.ApiData.MassGetDevSts.bDevAddr = (UINT8)MassDev->LogicalAddress;
    Parameters.ApiData.MassGetDevSts.bDeviceStatus = 0;

 	InvokeUsbApi(&Parameters);

    DevInfo = (DEV_INFO*)MassDev->DevInfo;

    if ((Parameters.ApiData.MassGetDevSts.bDeviceStatus & USB_MASS_MEDIA_PRESENT) && 
        (DevInfo->MaxLba != 0) && (DevInfo->wBlockSize != 0)) {
        if ((DevInfo->wBlockSize != MassDev->BlockIoProtocol.Media->BlockSize) ||
           ((DevInfo->MaxLba -1)!= MassDev->BlockIoProtocol.Media->LastBlock) ||
            (Parameters.ApiData.MassGetDevSts.bDeviceStatus & USB_MASS_MEDIA_CHANGED)) {
            MassDev->BlockIoProtocol.Media->MediaId++;
            MassDev->BlockIoProtocol.Media->ReadOnly = FALSE;
            MassDev->BlockIoProtocol.Media->LastBlock = DevInfo->MaxLba - 1;
            MassDev->BlockIoProtocol.Media->BlockSize = DevInfo->wBlockSize;
        }
    } else {
        MassDev->BlockIoProtocol.Media->LastBlock = 0; 
    }

    if ((!(Parameters.ApiData.MassGetDevSts.bDeviceStatus & USB_MASS_MEDIA_PRESENT)) 
        ||(MassDev->BlockIoProtocol.Media->LastBlock == 0) || (MassDev->BlockIoProtocol.Media->BlockSize == 0)) {
        MassDev->BlockIoProtocol.Media->MediaPresent = FALSE;
        MassDev->BlockIoProtocol.Media->LastBlock = 0;
        return EFI_NO_MEDIA;
    }
    
    if (MediaId != MassDev->BlockIoProtocol.Media->MediaId) {
        return EFI_MEDIA_CHANGED;
    }
    //
    // Check Parameter to comply with EFI 1.1 Spec
    //
    if (Lba > MassDev->BlockIoProtocol.Media->LastBlock) {
        return EFI_INVALID_PARAMETER;
    }

    BlockSize = ((DEV_INFO*)(MassDev->DevInfo))->wBlockSize;

    if ((Lba + (BufferSize / BlockSize) - 1) > MassDev->BlockIoProtocol.Media->LastBlock) {
        return EFI_INVALID_PARAMETER;
    }

    if (BufferSize % BlockSize != 0) {
        return EFI_BAD_BUFFER_SIZE;
    }

	DataBuffer = (UINT8*)(UINTN)Buffer;
	if (RShiftU64((UINTN)Buffer, 32)) {
		Pages = EFI_SIZE_TO_PAGES(BufferSize);
		DataBuffer = (UINT8*)0xFFFFFFFF;
		Status = gBS->AllocatePages(AllocateMaxAddress, EfiBootServicesData,
                		Pages, (EFI_PHYSICAL_ADDRESS*)&DataBuffer);
		if (!(EFI_ERROR(Status))) {
    		if (ReadWrite == WRITE) {
    			CopyMem(DataBuffer, Buffer, BufferSize);
    		}
		}  else {
		    DataBuffer = (UINT8*)(UINTN)Buffer;
		}
	}

    BytesRemaining = (UINT32)BufferSize;
    Buf = (UINTN)DataBuffer;
    while (BytesRemaining) {
        BytesToTransfer = (BytesRemaining > 0x10000)? 0x10000 : BytesRemaining;
        //
        // Prepare URP_STRUC with USB_MassRead attributes
        //
        Parameters.bFuncNumber = USB_API_MASS_DEVICE_REQUEST;
        Parameters.bSubFunc = (ReadWrite == READ)? USB_MASSAPI_EFI_READ_DEVICE : USB_MASSAPI_EFI_WRITE_DEVICE;
        Parameters.ApiData.EfiMassRead.DevAddr = (UINT8)MassDev->LogicalAddress;//MassDev->DevInfo->bDeviceAddress;
        Parameters.ApiData.EfiMassRead.StartLba = Lba;
        Parameters.ApiData.EfiMassRead.NumBlks = (UINT16)(BytesToTransfer/((DEV_INFO*)MassDev->DevInfo)->wBlockSize);
        Parameters.ApiData.EfiMassRead.PreSkipSize = 0;
        Parameters.ApiData.EfiMassRead.PostSkipSize = 0;
        Parameters.ApiData.EfiMassRead.BufferPtr = Buf;

        /*
        if (ReadWrite == READ) {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Reading...%x bytes, Lba %x ", BytesToTransfer, CurrentLba);
        } else {
            USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Writng...%x bytes, Lba %x ", BytesToTransfer, CurrentLba);
        }
        */
		InvokeUsbApi(&Parameters);
		
        switch (Parameters.bRetValue) {
            case USB_ATA_NO_MEDIA_ERR:
                    Status = EFI_NO_MEDIA;  // No media in drive
                    MassDev->BlockIoProtocol.Media->MediaPresent = FALSE;
                    break;
            case USB_ATA_WRITE_PROTECT_ERR:
                    Status = (ReadWrite == READ)? EFI_SUCCESS : EFI_WRITE_PROTECTED;
                    if (Status == EFI_WRITE_PROTECTED)  
                        MassDev->BlockIoProtocol.Media->ReadOnly = TRUE;
                    break;
            case USB_ATA_TIME_OUT_ERR:          // 0x080 Command timed out error
            case USB_ATA_DRIVE_NOT_READY_ERR:   // 0x0AA Drive not ready error
            case USB_ATA_DATA_CORRECTED_ERR:    // 0x011 Data corrected error
            case USB_ATA_PARAMETER_FAILED:      // 0x007 Bad parameter error
            case USB_ATA_MARK_NOT_FOUND_ERR:    // 0x002 Address mark not found error
            case USB_ATA_READ_ERR:              // 0x004 Read error
            case USB_ATA_UNCORRECTABLE_ERR:     // 0x010 Uncorrectable data error
            case USB_ATA_BAD_SECTOR_ERR:        // 0x00A Bad sector error
            case USB_ATA_GENERAL_FAILURE:       // 0x020 Controller general failure
                    Status = EFI_DEVICE_ERROR;
                    break;
            default:
                    Status = EFI_SUCCESS;
        }
        //  USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "Status= %r\n", Status);
        if (EFI_ERROR(Status)) {
            break;
        }
        BytesRemaining = BytesRemaining - BytesToTransfer;
        Buf = Buf + BytesToTransfer;
        Lba = Lba + (UINT32)BytesToTransfer/((DEV_INFO*)(MassDev->DevInfo))->wBlockSize;
    }

	if (DataBuffer != Buffer) {
		if (ReadWrite == READ) {
			CopyMem(Buffer, DataBuffer, BufferSize - BytesRemaining);
		}
		gBS->FreePages((EFI_PHYSICAL_ADDRESS)DataBuffer, Pages);
	}

    return  Status;
}



/**
    Read the requested number of blocks from the device

    @param This EFI_BLOCK_IO *: A pointer to the Block I/O protocol
        interface
        MediaId UINT32: The media id that the read request is for
        LBA EFI_LBA:    The starting logic block address to read from
        on the device
        BufferSize UINTN:   The size of the Buffer in bytes
        Buffer VOID *:  A pointer to the destination buffer for the data


    @retval EFI_SUCCESS:     The data was read correctly from the device
              EFI_DEVICE_ERROR:The device reported an error while attempting
                                  to perform the read operation
              EFI_NO_MEDIA:    There is no media in the device
              EFI_MEDIA_CHANGED:   The MediaId is not for the current media
              EFI_BAD_BUFFER_SIZE: The BufferSize parameter is not a multiple
                              of the intrinsic block size of the device
              EFI_INVALID_PARAMETER:The read request contains LBAs that are
                          not valid, or the buffer is not on proper alignment

**/

EFI_STATUS
EFIAPI
AmiUsbBlkIoReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL  *This,
  IN  UINT32                 MediaId,
  IN  EFI_LBA                Lba,
  IN  UINTN                  BufferSize,
  OUT VOID                   *Buffer
)

{
    EFI_STATUS  Status;
    EFI_TPL     OldTpl;
    USB_MASS_DEV *MassDev;

    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    MassDev = USB_MASS_DEV_FROM_THIS(This, BlockIoProtocol);
    Status = AmiUsbBlkIoReadWrite(&MassDev->BlockIoProtocol, MediaId, Lba, BufferSize, Buffer, READ);
    
    gBS->RestoreTPL(OldTpl);
    
    return Status;
}


/**
    Write a specified number of blocks to the device

    @param This EFI_BLOCK_IO *: A pointer to the Block I/O protocol
        interface
        MediaId UINT32: The media id that the write request is for
        LBA EFI_LBA:    The starting logic block address to written
        BufferSize UINTN:   The size of the Buffer in bytes
        Buffer VOID *:  A pointer to the destination buffer for the data


    @retval EFI_SUCCESS:     The data were written correctly to the device
              EFI_WRITE_PROTECTED: The device can not be written to
              EFI_NO_MEDIA:    There is no media in the device
              EFI_MEDIA_CHANGED:   The MediaId is not for the current media
              EFI_DEVICE_ERROR:  The device reported an error while attempting
                                  to perform the write operation
              EFI_BAD_BUFFER_SIZE: The BufferSize parameter is not a multiple
                                  of the intrinsic block size of the device
              EFI_INVALID_PARAMETER:The read request contains LBAs that are
                          not valid, or the buffer is not on proper alignment

**/

EFI_STATUS
EFIAPI
AmiUsbBlkIoWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This,
  IN UINT32                 MediaId,
  IN EFI_LBA                Lba,
  IN UINTN                  BufferSize,
  IN VOID                   *Buffer
  )
{
    EFI_STATUS  Status;
    EFI_TPL     OldTpl;
    USB_MASS_DEV *MassDev;
    MassDev = USB_MASS_DEV_FROM_THIS(This, BlockIoProtocol);
    
#if (defined(BOOT_SECTOR_WRITE_PROTECT) && (BOOT_SECTOR_WRITE_PROTECT != 0))
    if (AmiBlkWriteProtection != NULL) {
        // Get user input
        Status = AmiBlkWriteProtection->BlockIoWriteProtectionCheck( 
                                                    AmiBlkWriteProtection,
                                                    &MassDev->BlockIoProtocol,
                                                    Lba,
                                                    BufferSize
                                                    );
        // Abort operation if denied
        if (Status == EFI_ACCESS_DENIED) {
            return Status;
        }
    }
#endif
    
    OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
    
    Status = AmiUsbBlkIoReadWrite(&MassDev->BlockIoProtocol, MediaId, Lba, BufferSize, Buffer, WRITE);
    
    gBS->RestoreTPL(OldTpl);
    
    return Status;    
}


/**
    Verifies if usb mouse support can be installed on a device


**/

EFI_STATUS
EFIAPI
UsbMassSupported (
    EFI_DRIVER_BINDING_PROTOCOL *This,
    EFI_HANDLE                  Controller,
    EFI_DEVICE_PATH_PROTOCOL    *Dp)
{
    EFI_USB_INTERFACE_DESCRIPTOR        Desc;
    EFI_STATUS                          Status;
    EFI_USB_IO_PROTOCOL                 *UsbIo;
    DEV_INFO                            *DevInfo;

    if (!gUsbData->UsbSetupData.UsbMassDriverSupport) {
        return EFI_UNSUPPORTED;
    }

    Status = gBS->OpenProtocol(Controller, &gEfiUsbIoProtocolGuid,
        (VOID**)&UsbIo, This->DriverBindingHandle,
        Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = gBS->CloseProtocol(
        Controller, &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle, Controller);
    
    ASSERT(Status == EFI_SUCCESS);

    DevInfo = FindDevStruc(Controller);

    if (DevInfo == NULL) {
        return EFI_UNSUPPORTED;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor(UsbIo, &Desc);
    if (EFI_ERROR(Status)) {
        return EFI_UNSUPPORTED;
    }

    if ( Desc.InterfaceClass == BASE_CLASS_MASS_STORAGE &&
        (
        Desc.InterfaceProtocol == PROTOCOL_CBI ||
        Desc.InterfaceProtocol == PROTOCOL_CBI_NO_INT   ||
        Desc.InterfaceProtocol == PROTOCOL_BOT )) {
        return EFI_SUCCESS;
    } else {
        return EFI_UNSUPPORTED;
    }
}


/**
    Starts USB mass storage device

**/

EFI_STATUS
EFIAPI
UsbMassStart(
    EFI_DRIVER_BINDING_PROTOCOL  *This,
    EFI_HANDLE Controller,
    EFI_DEVICE_PATH_PROTOCOL *Dp
)
{
    EFI_STATUS              Status;
    EFI_USB_IO_PROTOCOL     *UsbIo;
    DEV_INFO                *DevInfo;
    USBDEV_T*               Dev;
    HC_STRUC*               HcData;
    UINT8                   UsbStatus;

    USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB: UsbMassStart: starting...\n");
    //
    // Open Protocols
    //
    Status = gBS->OpenProtocol(Controller, &gEfiUsbIoProtocolGuid,
                (VOID**)&UsbIo, This->DriverBindingHandle,
                Controller, EFI_OPEN_PROTOCOL_BY_DRIVER);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Dev = UsbIo2Dev(UsbIo);

    if (Dev == NULL) {
        return EFI_DEVICE_ERROR;
    }

    DevInfo = Dev->dev_info;
    
    if (DevInfo->bLUN) {
        USB_DEBUG(DEBUG_INFO, DEBUG_LEVEL_3, "USB: Skiping LUN %d\n", DevInfo->bLUN);
    } else {
        HcData = gUsbData->HcTable[Dev->dev_info->bHCNumber - 1];
        UsbStatus = UsbSmiReConfigDevice(HcData, Dev->dev_info);
        if (UsbStatus != USB_SUCCESS) {
            USB_DEBUG(DEBUG_ERROR, DEBUG_LEVEL_3, 
                    "USB: UsbMassStart: failed to Reconfigure: %d\n", UsbStatus);
            gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
                            This->DriverBindingHandle, Controller);
            return EFI_DEVICE_ERROR;
        }
    } //End Reconfigure

    if (!(DevInfo->Flag & DEV_INFO_DEV_PRESENT)) {
        gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
                        This->DriverBindingHandle, Controller);
        return EFI_DEVICE_ERROR;
    }

    Status = InstallUsbMass(Controller, DevInfo);
    
    if (EFI_ERROR(Status)) {    
        gBS->CloseProtocol(Controller, &gEfiUsbIoProtocolGuid,
                        This->DriverBindingHandle, Controller);
        return Status;
    }
    
#if (defined(BOOT_SECTOR_WRITE_PROTECT) && (BOOT_SECTOR_WRITE_PROTECT != 0))
    if(AmiBlkWriteProtection == NULL) {
        Status = gBS->LocateProtocol(&gAmiBlockIoWriteProtectionProtocolGuid, 
                                      NULL, 
                                      (VOID**)&AmiBlkWriteProtection); 
        if(EFI_ERROR(Status)) {
            AmiBlkWriteProtection = NULL;
        }
    }
#endif

    return Status;
}

/**
    Stops USB mass storage device and removes BlkIo
    @param  Binding: Pointer of driver binding protocol
            Controller: Handle number of controller
            NumberOfChildren: Number of children in driver binding
            Children: Handle number of children
    @retval EFI_STATUS Status of the operation
**/

EFI_STATUS
EFIAPI
UsbMassStop(
    EFI_DRIVER_BINDING_PROTOCOL *Binding,
    EFI_HANDLE Controller,
    UINTN NumberOfChildren,
    EFI_HANDLE *Children
)
{
    EFI_STATUS Status;
    EFI_BLOCK_IO_PROTOCOL   *BlockIo;

    Status = gBS->OpenProtocol(Controller, &gEfiBlockIoProtocolGuid,
        (VOID**)&BlockIo, Binding->DriverBindingHandle,
        Controller, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    
    ASSERT(Status == EFI_SUCCESS);
    
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = UninstallUSBMass(USB_MASS_DEV_FROM_THIS(BlockIo, BlockIoProtocol));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    Status = gBS->CloseProtocol(
        Controller, &gEfiUsbIoProtocolGuid,
        Binding->DriverBindingHandle, Controller);
    
    ASSERT(Status == EFI_SUCCESS);

    return Status;
}


CHAR16*
UsbMassGetControllerName(
    EFI_HANDLE Controller,
    EFI_HANDLE Child
)
{
    return NULL;
}


/**
    USB Mass storage driver entry point

**/

EFI_STATUS
UsbMassInit(
    EFI_HANDLE  ImageHandle,
    EFI_HANDLE  ServiceHandle
)
{
                                        //(EIP59272)>
    static NAME_SERVICE_T Names;
    static EFI_DRIVER_BINDING_PROTOCOL Binding = {
        UsbMassSupported,
        UsbMassStart,
        UsbMassStop,
        USBMASS_DRIVER_VERSION,
        NULL,
        NULL };

    Binding.DriverBindingHandle = ServiceHandle;
    Binding.ImageHandle = ImageHandle;

    return gBS->InstallMultipleProtocolInterfaces(
        &Binding.DriverBindingHandle,
        &gEfiDriverBindingProtocolGuid, &Binding,
        &gEfiComponentName2ProtocolGuid, InitNamesProtocol(&Names,
                L"USB Mass Storage driver", UsbMassGetControllerName),
        NULL);
                                        //<(EIP59272)
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

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

//**********************************************************************
// $Header: 
//
// $Revision: 1 $
//
// $Date: 
//**********************************************************************
// Revision History
// ----------------
// 
// 
//
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  MountFs.c
//
// Description:	To create temporary filesystem from memory
//
//<AMI_FHDR_END>
//**********************************************************************

#include "MountFs.h"

MOUNT_FS_PROTOCOL gMountFsProtocol = { Mount, Unmount,SetFsVolumeName,GetFsVolumeName,GetMountFsHandlle };

MOUNT_FS_DEVICE *gMountDiskDevice = (MOUNT_FS_DEVICE *)NULL;
UINT32 gNumPages = 0 ;
EFI_GUID gBlockIOGuid = EFI_BLOCK_IO_PROTOCOL_GUID;
UINTN gMountedFsCount = 0;

EFI_HANDLE gMountFsImageHandle = NULL;
// EFI device path definition
static MOUNT_FS_DEVICE_PATH gMountFsDevicePath =
{
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    sizeof(MOUNT_FS_DEVICE_PATH) - END_DEVICE_PATH_LENGTH,
    0,
    // {06ED4DD0-FF78-11d3-BDC4-00A0C94053D1}
    //0x6ed4dd0, 0xff78, 0x11d3, 0xbd, 0xc4, 0x0, 0xa0, 0xc9, 0x40, 0x53, 0xd1,
    MOUNT_FS_DEVICE_PATH_GUID,
    0,0,0,0,0,0,0,0,    // ID assigned below
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH
};

// Sector per cluster structure for FAT16
static FAT16TABLE fat16tbl[] =
{
    /* {0x000020D0, 0}, */
    {0x00000800, 1},    /* 800 sectors * 1 sec/cluster * 512 bytes = 1 M */
    {0x00001000, 1},    /* 1000 sectors * 1 sec/cluster * 512 bytes = 2 M */
    {0x00001800, 1},    /* 1800 sectors * 1 sec/cluster * 512 bytes = 3 M */
    {0x00007FA8, 2},
    {0x00040000, 4},
    {0x00080000, 8},
    {0x00100000,16},
    {0xFFFFFFFF, 0}
};

// Sector per cluster structure for FAT32
static FAT32TABLE fat32tbl[]= {
{ 0x00010428, 0}, /* disks up to 32.5 MB, the 0 value for SecPerClusVal trips an error */
{ 0x00082000, 1}, /* disks up to 260 MB, .5k cluster */
{ 0x01000000, 8}, /* disks up to 8 GB, 4k cluster */
{ 0x02000000, 16}, /* disks up to 16 GB, 8k cluster */
{ 0x04000000, 32}, /* disks up to 32 GB, 16k cluster */
{ 0xFFFFFFFF, 64}/* disks greater than 32GB, 32k cluster */
};

static BOOTSEC32 g_bs32 =
{
    /* BS_jmpBoot     */ {0xeb,0x3c,0x90},
    /* BS_OEMName     */ {'E','F','I','R','D','I','S','K'},
    /* BPB_BytsPerSec */ 512,
    /* BPB_SecPerClus */ 0,
    /* BPB_RsvdSecCnt */ 32,    
    /* BPB_NumFATs    */ 2,
    /* BPB_RootEntCnt */ 0,  
    /* BPB_TotSec16   */ 0,    
    /* BPB_Media      */ 0xF8, 
    /* BPB_FATSz16    */ 0,   
    /* BPB_SecPerTrk  */ 0,
    /* BPB_NumHeads   */ 0,
    /* BPB_HiddSec    */ 0,
    /* BPB_TotSec32   */ 0, 
    
    /* BPB_FATSz32    */ 0,
    /* BPB_ExtFlags   */ 0, 
    /* BPB_FSVer      */ 0, 
    /* BPB_RootClus   */ 2, 
    /* BPB_FSInfo     */ 1, 
    /* BPB_BkBootSec  */ 6, 
    /* BPB_Reserved   */ {0}, 
    
    /* BS_DrvNum      */ 0,
    /* BS_Reserved1   */ 0,
    /* BS_BootSig     */ 0x29,
    /* BS_VolID       */ 0,
    /* BS_VolLab      */ {'N','O',' ','N','A','M','E',' ',' ',' '},
    /* BS_FilSysType  */ {'F','A','T','3','2',' ',' ',' '}
};

static BOOTSEC16 g_bs16 =
{
    /* BS_jmpBoot     */ {0xeb,0x0,0x90},
    /* BS_OEMName     */ {'E','F','I','R','D','I','S','K'},
    /* BPB_BytsPerSec */ 512,
    /* BPB_SecPerClus */ 0,
    /* BPB_RsvdSecCnt */ 1,
    /* BPB_NumFATs    */ 2,
    /* BPB_RootEntCnt */ 512,
    /* BPB_TotSec16   */ 0,
    /* BPB_Media      */ 0xF8,
    /* BPB_FATSz16    */ 0,
    /* BPB_SecPerTrk  */ 0,
    /* BPB_NumHeads   */ 0,
    /* BPB_HiddSec    */ 0,
    /* BPB_TotSec32   */ 0,
    /* BS_DrvNum      */ 0,
    /* BS_Reserved1   */ 0,
    /* BS_BootSig     */ 0x29,
    /* BS_VolID       */ 0,
    /* BS_VolLab      */ {'N','O',' ','N','A','M','E',' ',' ',' '},
    /* BS_FilSysType  */ {'F','A','T','1','6',' ',' ',' '}
};


static FSINFO_SECTOR gFSInfo =
{
    /* FSI_LeadSig     */ 0x41615252,
    /* FSI_Reserved1   */ {0},
    /* FSI_StrucSig    */ 0x61417272,
    /* FSI_Free_Count  */ 0,
    /* FSI_Nxt_Free    */ 2,
    /* FSI_Reserved2   */ {0},
    /* FSI_TrailSig    */ 0xAA550000
};


/**
    Function to configure and update FAT16 / FAT32 file system
     
    @param In VOID*, UINT32, UINTN

    @retval VOID
     
**/

VOID FormatMountFs(    IN VOID*  pStart,    IN UINT32 Size,    IN UINTN FileSysType)
{
    UINT32 TotalSectors, RootDirSectors, FatSz, tmp1, tmp2 ; 
    UINT8 *Fat1,*Fat2;
    UINT8 FatCount = 0 ;
   
    // The boot signature needs to be filled out
    if(FileSysType == FAT32_FILE_SYSTEM)
    {
        g_bs32.BS_Sig = 0xAA55;
        // Compute the total sectors and appropriate cluster size
        TotalSectors = Size / g_bs32.BPB_BytsPerSec;
        if(FileSysType == FAT32_FILE_SYSTEM)
            g_bs32.BPB_SecPerClus = size2spc32(TotalSectors);

        ASSERT(g_bs32.BPB_SecPerClus != 0);
        
        // Compute how many root directory sectors are needed
        RootDirSectors = (g_bs32.BPB_RootEntCnt * 32 + g_bs32.BPB_BytsPerSec - 1) / g_bs32.BPB_BytsPerSec;
    
        // Compute how many sectors are required per FAT
        tmp1 = TotalSectors - (g_bs32.BPB_RsvdSecCnt + RootDirSectors);
        tmp2 = 256 * g_bs32.BPB_SecPerClus + g_bs32.BPB_NumFATs;
        tmp2 = tmp2 / 2;
        FatSz = (tmp1 + tmp2 - 1) / tmp2;
        ASSERT(FatSz <= 0xFFFF);
    }
    else
    {
        g_bs16.BS_Sig = 0xAA55;
        // Compute the total sectors and appropriate cluster size
        TotalSectors = Size / g_bs16.BPB_BytsPerSec;
        g_bs16.BPB_SecPerClus = size2spc16(TotalSectors);
        ASSERT(g_bs16.BPB_SecPerClus != 0);
        
        // Compute how many root directory sectors are needed
        RootDirSectors = (g_bs16.BPB_RootEntCnt * 32 + g_bs16.BPB_BytsPerSec - 1) / g_bs16.BPB_BytsPerSec;
    
        // Compute how many sectors are required per FAT
        tmp1 = TotalSectors - (g_bs16.BPB_RsvdSecCnt + RootDirSectors);
        tmp2 = 256 * g_bs16.BPB_SecPerClus + g_bs16.BPB_NumFATs;
        FatSz = (tmp1 + tmp2 - 1) / tmp2;
        ASSERT(FatSz <= 0xFFFF);
    }

    // Store the total sectors and fat size values
    if(FileSysType == FAT32_FILE_SYSTEM)
    {
        g_bs32.BPB_TotSec32 = TotalSectors;
        g_bs32.BPB_FATSz32 = FatSz; 
        
        gFSInfo.FSI_Free_Count = ((TotalSectors - (g_bs32.BPB_RsvdSecCnt + (2*FatSz))) / g_bs32.BPB_SecPerClus)-1;
    }
    else
    {
        // Store the total sectors and fat size values
        if(TotalSectors > 0xFFFF)
            g_bs16.BPB_TotSec32 = TotalSectors;
        else
            g_bs16.BPB_TotSec16 = (UINT16)TotalSectors;
        
        g_bs16.BPB_FATSz16 = (UINT16)FatSz;
    }
    
    // The FAT table and root directory need to be all zeroes.
    ZeroMem(pStart,Size);
    
    // Write the FSInfo structure in the 2nd sector to the ramdisk for FAT32
    if(FileSysType == FAT32_FILE_SYSTEM)
    {
        CopyMem(pStart,&g_bs32,512);
        CopyMem((UINT8*)pStart+512,&gFSInfo,512);
        
        // Compute the starting offsets of the two FATs
        Fat1 = (UINT8*)pStart + g_bs32.BPB_RsvdSecCnt * 512;
        Fat2 = (UINT8*)pStart + (g_bs32.BPB_RsvdSecCnt + FatSz) * 512;
        
        //Initialize FAT1 and FAT2
        for( FatCount = 0 ; FatCount <12 ; FatCount++ )
        {
            if( FatCount == 0 )
            {
                Fat1[FatCount] = Fat2[FatCount] = g_bs32.BPB_Media;
            }
            else if( (FatCount == 3) || (FatCount == 7) || (FatCount == 11) )
            {
                Fat1[FatCount] = Fat2[FatCount] = 0x0F;
            }
            else
            {
                Fat1[FatCount] = Fat2[FatCount] = 0xFF;
            }
        }
    }
    else
    {
        CopyMem(pStart,&g_bs16,512);
        
        // Compute the starting offsets of the two FATs
        Fat1 = (UINT8*)pStart + g_bs16.BPB_RsvdSecCnt * 512;
        Fat2 = (UINT8*)pStart + (g_bs16.BPB_RsvdSecCnt + FatSz) * 512;
      
        //Initialize FAT1 and FAT2
        for( FatCount = 0 ; FatCount <12 ; FatCount++ )
        {
            if( FatCount == 0 )
            {
                Fat1[FatCount] = Fat2[FatCount] = g_bs32.BPB_Media;
            }
            else
            {
                Fat1[FatCount] = Fat2[FatCount] = 0xFF;
            }
        }
    }
}

/**
    Function to mount FAT16 / FAT32 File system from memory
     
    @param In EFI_HANDLE, UINTN, UINTN

    @retval EFI_STATUS
     
**/


EFI_STATUS Mount(IN EFI_HANDLE ImgHandle,IN UINTN FileSysType, IN UINT32 DiskSize,void* Buffer,CHAR16 *VolumeName,UINTN *HandleIndex)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 MountDiskSize = 0 ;
    UINT32 BlockSize = 0 ;
    UINT64 DiskId = 0 ;
    
    // File system type is must, but size may be ZERO. It will create default partition
    if( !((FileSysType == FAT32_FILE_SYSTEM)||(FileSysType == FAT16_FILE_SYSTEM)) )
        return EFI_INVALID_PARAMETER ;
    
    // If file system size is zero it will create partition with default size
    if(DiskSize == 0)
    {
        if(FileSysType == FAT32_FILE_SYSTEM)
        	MountDiskSize = MIN32_DEFAULT_DISK_SIZE * DEFAULT_BYTE_VALUE * DEFAULT_BYTE_VALUE ;
        else if(FileSysType == FAT16_FILE_SYSTEM)
        	MountDiskSize = MIN16_DEFAULT_DISK_SIZE * DEFAULT_BYTE_VALUE * DEFAULT_BYTE_VALUE ;
        else
            return EFI_INVALID_PARAMETER ;
    }
    else
    {
    	if(NULL == Buffer)
    		MountDiskSize = DiskSize * DEFAULT_BYTE_VALUE * DEFAULT_BYTE_VALUE ;
    	else
    		MountDiskSize = DiskSize;
    }
    BlockSize   = 512;
    
    gMountDiskDevice = AllocateZeroPool(sizeof(MOUNT_FS_DEVICE));
    if(gMountDiskDevice == NULL)
        return EFI_OUT_OF_RESOURCES;
    

    //Compute the number of 4KB pages needed by the ramdisk and allocate the memory.
    gNumPages = MountDiskSize / EFI_PAGE_SIZE;
    if(gNumPages % MountDiskSize)
    	gNumPages++;
    
    // Allocate pages to create RAM drive

    Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, gNumPages, &gMountDiskDevice->Start);

    if(EFI_ERROR(Status)) {
        FreePool(gMountDiskDevice);
        return Status;
    }

    //Initialize the ramdisk's device info.
    (void)gBS->GetNextMonotonicCount(&DiskId);
    CopyMem(&gMountFsDevicePath.DiskId, &DiskId, sizeof(DiskId));

    gMountDiskDevice->Signature            = PBLOCK_DEVICE_SIGNATURE;
    gMountDiskDevice->BlkIo.Revision       = EFI_BLOCK_IO_PROTOCOL_REVISION;
    gMountDiskDevice->BlkIo.Media          = &gMountDiskDevice->Media;
    gMountDiskDevice->Media.RemovableMedia = FALSE;
    gMountDiskDevice->Media.MediaPresent   = TRUE;
    
    gMountDiskDevice->Media.LastBlock        = MountDiskSize/BlockSize - 1;
    gMountDiskDevice->Media.BlockSize        = BlockSize;
    gMountDiskDevice->Media.LogicalPartition = TRUE;
    gMountDiskDevice->Media.ReadOnly         = FALSE;
    gMountDiskDevice->Media.WriteCaching     = TRUE;
    
    gMountDiskDevice->BlkIo.ReadBlocks  = ReadBlockIo;
    gMountDiskDevice->BlkIo.WriteBlocks = WriteBlockIo;
    gMountDiskDevice->BlkIo.FlushBlocks = FlushBlockIo;
    
    gMountDiskDevice->DevicePath = DuplicateDevicePath((EFI_DEVICE_PATH*)&gMountFsDevicePath);
    
    //Build a FAT32 file system on the ramdisk.
    if(NULL == Buffer)
    	FormatMountFs((VOID*)gMountDiskDevice->Start, MountDiskSize, FileSysType);
    else
    {
    	ZeroMem((VOID*)gMountDiskDevice->Start,MountDiskSize);
    	CopyMem((VOID*)gMountDiskDevice->Start,Buffer,MountDiskSize);
    }
    //Install the device.
    Status = gBS->InstallMultipleProtocolInterfaces(&ImgHandle, &gBlockIOGuid, &gMountDiskDevice->BlkIo, &gEfiDevicePathProtocolGuid, gMountDiskDevice->DevicePath, NULL);
    
    //gBS->Stall (MOUNT_UPDATE_DURATION);
    //Connects one or more drivers to a controller.
    Status = gBS->ConnectController(ImgHandle,NULL,NULL,TRUE);
    
    Status = GetMountFsHandlle(NULL,&DiskId,HandleIndex);
    if(Status == EFI_SUCCESS)
    {
    	gMountedFsCount++;
    	if(NULL!= VolumeName)
    	{
    		SetFsVolumeName(*HandleIndex,VolumeName);
    	}
    }
    
    return Status;
}

/**
    Function to un-install protocol interface and freeing memory
     
    @param In EFI_HANDLE

    @retval EFI_STATUS
     
**/

EFI_STATUS Unmount( EFI_HANDLE ImageHandle ,EFI_HANDLE MsFshandle )
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    // Disconnects drivers from a controller.
	UINTN Count = 0;
    UINTN NoOfHandles = 0;
    EFI_HANDLE *HandleBuffer = NULL;

    if(ImageHandle && MsFshandle)
    {
		Status = gBS->DisconnectController(MsFshandle,NULL,NULL);
	
		Status = gST->BootServices->LocateHandleBuffer( AllHandles, NULL, NULL, &NoOfHandles, (EFI_HANDLE**)&HandleBuffer );
	
		if ( Status == EFI_SUCCESS )
		{
			for ( Count = 0; Count < NoOfHandles; Count++ )
			{
				Status = gST->BootServices->DisconnectController( HandleBuffer[Count], MsFshandle, NULL );
			}
	
			Status = gST->BootServices->FreePool(HandleBuffer);
	
		}
	

		// Deallocating memories which are allocated time of FAT creation
		
		if( gMountDiskDevice != NULL )
		{
			Status = gBS->UninstallMultipleProtocolInterfaces(ImageHandle, &gBlockIOGuid, &gMountDiskDevice->BlkIo, &gEfiDevicePathProtocolGuid, gMountDiskDevice->DevicePath, NULL);
	
			if( ((EFI_PHYSICAL_ADDRESS *)gMountDiskDevice->Start) != NULL )
			{
				gBS->FreePages( gMountDiskDevice->Start, gNumPages) ;
			}
			FreePool(gMountDiskDevice);
			gMountDiskDevice = NULL;
		}
    }
    
    return Status;
}


/**
    Function to compute cluster size vs. total sectors on drive for FAT16
     
    @param In UINT32

    @retval EFI_STATUS
     
**/

UINT8 size2spc16(UINT32 ts)
{
    int i = 0;
    
    while(fat16tbl[i].size != 0xFFFFFFFF)
    {
        if(ts <= fat16tbl[i].size)
            return fat16tbl[i].spc;
        ++i;
    }
    
    return 0;
}


/**
    Function to compute cluster size vs. total sectors on drive for FAT32
     
    @param In UINT32

    @retval EFI_STATUS
     
**/

UINT8 size2spc32(UINT32 ts)
{
    int i = 0;
    
    while(fat32tbl[i].size != 0xFFFFFFFF)
    {
        if(ts <= fat32tbl[i].size)
            return fat32tbl[i].spc;
        ++i;
    }
    
    return 0;
}

/**
    Implementation of block I/O write
     
    @param In EFI_BLOCK_IO *, UINT32, EFI_LBA, UINTN, VOID *

    @retval EFI_STATUS
     
**/

EFI_STATUS EFIAPI WriteBlockIo(
    IN EFI_BLOCK_IO *This,
    IN UINT32       MediaId,
    IN EFI_LBA      LBA,
    IN UINTN        BufferSize,
    IN VOID         *Buffer)
{
    EFI_BLOCK_IO_MEDIA   *Media;
    MOUNT_FS_DEVICE         *MountDiskDevice;
    EFI_PHYSICAL_ADDRESS RamDiskLBA;

    Media = This->Media;
    if(Media->ReadOnly)
        return EFI_WRITE_PROTECTED;

    if(BufferSize % Media->BlockSize != 0)
        return EFI_BAD_BUFFER_SIZE;

    if(LBA > Media->LastBlock)
        return EFI_DEVICE_ERROR;

    if(LBA + BufferSize / Media->BlockSize - 1 > Media->LastBlock)
        return EFI_DEVICE_ERROR;

    MountDiskDevice = GET_MOUNT_FS_DEVICE_INTERFACE(This);
    RamDiskLBA = MountDiskDevice->Start + MultU64x32(LBA,Media->BlockSize);
    CopyMem((VOID*)RamDiskLBA,Buffer,BufferSize);

    return EFI_SUCCESS;
}

/**
    Implementation of block I/O read
     
    @param In EFI_BLOCK_IO *, UINT32, EFI_LBA, UINTN, VOID *

    @retval EFI_STATUS
     
**/

EFI_STATUS EFIAPI ReadBlockIo(
    IN EFI_BLOCK_IO *This,
    IN UINT32       MediaId,
    IN EFI_LBA      LBA,
    IN UINTN        BufferSize,
    OUT VOID        *Buffer)
{
    EFI_BLOCK_IO_MEDIA   *Media;
    MOUNT_FS_DEVICE         *MountDiskDevice;
    EFI_PHYSICAL_ADDRESS RamDiskLBA;

    Media = This->Media;

    if(BufferSize % Media->BlockSize != 0)
        return EFI_BAD_BUFFER_SIZE;

    if(LBA > Media->LastBlock)
        return EFI_DEVICE_ERROR;

    if(LBA + BufferSize / Media->BlockSize - 1 > Media->LastBlock)
        return EFI_DEVICE_ERROR;

    MountDiskDevice = GET_MOUNT_FS_DEVICE_INTERFACE(This);
    RamDiskLBA = MountDiskDevice->Start + MultU64x32(LBA,Media->BlockSize);
    CopyMem(Buffer,(VOID*)RamDiskLBA,BufferSize);

    return EFI_SUCCESS;
}

/**
    Implementation of block I/O flush
     
    @param In EFI_BLOCK_IO *

    @retval EFI_STATUS
     
**/

EFI_STATUS EFIAPI FlushBlockIo(    IN EFI_BLOCK_IO *This)
{
    return EFI_SUCCESS;
}

/**
    Entry point function for Mount Filesystem from memory
     
    @param In EFI_HANDLE, EFI_SYSTEM_TABLE *

    @retval EFI_STATUS
     
**/

EFI_STATUS MountFsEntryPoint(	IN EFI_HANDLE ImageHandle,	IN EFI_SYSTEM_TABLE *SystemTable)
{  	
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImageProtocol = NULL;
	EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleCount=0;
    UINTN i = 0;
    EFI_HANDLE *HandleBuffer = (EFI_HANDLE *)NULL;
    gMountFsImageHandle = ImageHandle;
    Status = gBS->HandleProtocol( ImageHandle, &gEfiLoadedImageProtocolGuid, (void **)&LoadedImageProtocol );

    if( Status == EFI_SUCCESS )
    {
    	LoadedImageProtocol->Unload = UnloadMountFsDriver;
    	// Installing RAM disk protocol
    	Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle, &gMountFsProtocolGuid, (void *)&gMountFsProtocol, NULL);
    }

	return Status;
}

/**
    Unloads Driver from Memory
     
    @param In EFI_HANDLE ImageHandle

    @retval EFI_STATUS
     
**/

EFI_STATUS UnloadMountFsDriver ( EFI_HANDLE ImageHandle )
{
    EFI_STATUS Status = EFI_SUCCESS ;
    UINTN Count = 0;
    UINTN NoOfHandles = 0;
    EFI_HANDLE *HandleBuffer = NULL;

    Status = gST->BootServices->LocateHandleBuffer( AllHandles, NULL, NULL, &NoOfHandles, (EFI_HANDLE**)&HandleBuffer );

    if ( Status == EFI_SUCCESS )
    {
        for ( Count = 0; Count < NoOfHandles; Count++ )
        {
            Status = gST->BootServices->DisconnectController( HandleBuffer[Count], ImageHandle, NULL );
        }

        Status = gST->BootServices->FreePool(HandleBuffer);

    }
    
    // Removes protocol interfaces into the boot services environment.
    if( gMountDiskDevice != NULL )
    {
        Status = gBS->UninstallMultipleProtocolInterfaces(ImageHandle, &gBlockIOGuid, &gMountDiskDevice->BlkIo, &gEfiDevicePathProtocolGuid, gMountDiskDevice->DevicePath, NULL);

        if( ((EFI_PHYSICAL_ADDRESS *)gMountDiskDevice->Start) != NULL )
        {
            gBS->FreePages( gMountDiskDevice->Start, gNumPages) ;
        }
        FreePool(gMountDiskDevice);
        gMountDiskDevice = NULL;
    }
    
    //if(gMountDiskDevice)
    	//Status = gBS->UninstallMultipleProtocolInterfaces(ImageHandle, &gBlockIOGuid, &gMountDiskDevice->BlkIo, &gEfiDevicePathProtocolGuid, gMountDiskDevice->DevicePath, NULL);
	Status = gBS->InstallMultipleProtocolInterfaces(gMountFsImageHandle, &gMountFsProtocolGuid, (void *)&gMountFsProtocol, NULL);

    return EFI_SUCCESS;
}

/**
    get file protocol interface from the file syatem handle index
     
    @param In UINTN HandleIndex

    @retval EFI_FILE_PROTOCOL *
     
**/

EFI_FILE_PROTOCOL* GetFileProtocolInterface(UINTN HandleIndex)
{
    EFI_HANDLE						 *mFs = (EFI_HANDLE*)NULL;
    UINTN							 mFsCount = 0;
	EFI_GUID 						SimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_STATUS 						Status = EFI_SUCCESS;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemInterface = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
	EFI_FILE_PROTOCOL *				FileInerface = (EFI_FILE_PROTOCOL *)NULL;
	EFI_HANDLE						InFSHandle = NULL;
	
    Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &mFsCount, &mFs);
    if(!EFI_ERROR(Status))
    {
    	if(HandleIndex < mFsCount)
    	{
			InFSHandle = mFs[HandleIndex];
			Status = gBS->HandleProtocol(InFSHandle, &SimpleFileSystemGuid,(void**) &SimpleFileSystemInterface);
			if(!EFI_ERROR(Status))
			{
				Status = SimpleFileSystemInterface->OpenVolume(SimpleFileSystemInterface, &FileInerface);
			}
    	}
    }
    if(mFs)
    	FreePool(mFs);
	return FileInerface;
}

/**
    set volume name of filesystem
     
    @param In UINTN HandleIndex,CHAR16* VolName

    @retval EFI_STATUS
     
**/

EFI_STATUS SetFsVolumeName(UINTN HandleIndex,CHAR16* VolName )
{
	EFI_GUID 						SimpleFileSystemGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_STATUS 						Status = EFI_SUCCESS;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemInterface = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
	EFI_FILE_PROTOCOL *				FileInerface = (EFI_FILE_PROTOCOL *)NULL;
    EFI_GUID 						VolumeGuid = EFI_FILE_SYSTEM_VOLUME_LABEL_ID;

    if(NULL != VolName)
    {
    	FileInerface = GetFileProtocolInterface(HandleIndex);
    	if(FileInerface)
    		Status = FileInerface->SetInfo(FileInerface, &VolumeGuid, (UINTN)(StrLen(VolName)+1), (VOID*)VolName);
    }
    return Status;
}

/**
    get volume name of filesystem,
     
    @param In UINTN HandleIndex,CHAR16* VolName

    @retval EFI_STATUS
     
**/

EFI_STATUS GetFsVolumeName(UINTN HandleIndex,CHAR16* VolName )
{
	EFI_STATUS 						Status = EFI_SUCCESS;
	EFI_FILE_PROTOCOL *				FileInerface = (EFI_FILE_PROTOCOL *)NULL;		
    EFI_GUID 						FileSystemInfoType = EFI_FILE_SYSTEM_INFO_ID;
    UINTN    						FileSystemInfoSize = (UINTN)sizeof(EFI_FILE_SYSTEM_INFO) + 1024;
    EFI_FILE_SYSTEM_INFO 			*FileSystemInfo = (EFI_FILE_SYSTEM_INFO*)NULL;
    BOOLEAN 						IsValidFileName = FALSE;

    if(NULL != VolName)
    {
    	FileInerface = GetFileProtocolInterface(HandleIndex);
    	if(FileInerface)
    	{
    		Status = gBS->AllocatePool(EfiBootServicesData,FileSystemInfoSize,(void **)&FileSystemInfo);
    		if (!EFI_ERROR(Status))
    		{
    			Status = FileInerface->GetInfo(FileInerface, &FileSystemInfoType, &FileSystemInfoSize, FileSystemInfo);
                if (!EFI_ERROR(Status))
                {  	
                	if(FileSystemInfo->VolumeLabel)
                		CopyMem(VolName,FileSystemInfo->VolumeLabel,(StrLen(FileSystemInfo->VolumeLabel)+1)*2);
                }
                gBS->FreePool(FileSystemInfo);
                FileSystemInfo = (EFI_FILE_SYSTEM_INFO*)NULL;
    		}
    	}
    }

    return Status;
}

/**
    get newly mounted file system index 
     
    @param In CHAR16 *VolName,UINT64 *MountFsID,UINTN *HandleIndex

    @retval EFI_STATUS
     
**/

EFI_STATUS GetMountFsHandlle(CHAR16 *VolName,UINT64 *MountFsID,UINTN *HandleIndex)
{
    EFI_HANDLE				*mFs = (EFI_HANDLE*)NULL;
    UINTN					mFsCount = 0;
    EFI_STATUS 				Status = EFI_SUCCESS;
    CHAR16					CurVolName[512];
    UINT32 					FileHandleIndex = 0;
    MOUNT_FS_DEVICE_PATH  	*DevicePathInterface=(MOUNT_FS_DEVICE_PATH*)NULL;
    EFI_GUID                MountedFsGuid = MOUNT_FS_DEVICE_PATH_GUID;

    Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &mFsCount, &mFs);

    if(EFI_SUCCESS == Status)
    {    		
    	for(FileHandleIndex = 0; FileHandleIndex < mFsCount;FileHandleIndex++)
    	{
    		Status= gBS->HandleProtocol(mFs[FileHandleIndex],&gEfiDevicePathProtocolGuid,(VOID **)&DevicePathInterface);
    		if(EFI_SUCCESS == Status)
    		{
    			//check for ramdisk type and subtype only
    			
    			if(!MemCmp (&(DevicePathInterface->Guid), &MountedFsGuid, sizeof (EFI_GUID) ))
    			{
    				if(NULL != VolName)
    				{
						if(TRUE == GetFsVolumeName(FileHandleIndex,(CHAR16*)CurVolName))
						{
							if(TRUE == StrnCmp(CurVolName,VolName,(UINT32)StrLen(VolName) ))
							{
								*HandleIndex = FileHandleIndex;
								break;
							}    		    			
						}
    				}
    				else if(NULL!=MountFsID)
    				{
						if(!MemCmp (DevicePathInterface->DiskId, MountFsID, sizeof (DevicePathInterface->DiskId) ))
						{
							*HandleIndex = FileHandleIndex;
							break;
						}
    				}
					else
					{
						*HandleIndex = FileHandleIndex;
						break;
					}
    			}
    		}
    	}
    }
    if(mFs)
    	FreePool(mFs);
    return Status;
}

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

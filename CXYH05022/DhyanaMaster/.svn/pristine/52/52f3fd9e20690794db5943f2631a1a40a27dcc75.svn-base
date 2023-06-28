//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************

//************************************************************************
// Revision History
// ----------------
// $Log:
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  FileSystemInfo.c
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
#include "AmiDxeLib.h"
#include "FileSystemInfo.h"
#include <Protocol/BlockIo.h>
#include "Library/UefiLib.h"
#include <Protocol/UsbIo.h>
#include "Protocol/DiskIo.h" 
#include "Protocol/DevicePathToText.h"
#include "Token.h"
#include "AmiFwUpdateBmcCoreIntf.h"

#define SET_MEM_PATTERN 0xf6

#define FLOPPY_LAST_BLOCK   2880
#define LOGICAL_BLOCK_SIZE              512
#define MAX_TARGETDISK_COUNT    16

extern EFI_BOOT_SERVICES     *gBS;
EFI_GUID    gEfiDevicePathToTextProtocolGuid = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
TARGET_FSINFO *gTargetDIskInfo = NULL;
UINT32 *gDiskCount;


//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   InitGlobals
//
// Description: Initialize the variables 
//
// Input:       (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * UefiSysTable
//              
// Output:      void
//---------------------------------------------------------------------------
void InitGlobals(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * UefiSysTable)
{
    InitAmiLib(ImageHandle,UefiSysTable);
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AMI_FS_GetFileSystemsInformation
//
// Description: Gets the available file system information   
//
// Input:       TARGET_FSINFO **TagetDIskInfo,UINT32 *DiskCount
//              
// Output:      EFI_STATUS
//---------------------------------------------------------------------------
EFI_STATUS AMI_FS_GetFileSystemsInformation(TARGET_FSINFO **TargetDIskInfo, UINT32 *DiskCount)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN   NoHandles = 0;
    UINT32	Index = 0;
    UINT32  HandleIndex = 0;
    UINT32 TagetDiskCount = 0;
    UINT32 HandleCount = 0;
    UINT32 TotalDiskCount = 0;
    EFI_HANDLE      *pHandleBuffer = NULL;
    EFI_HANDLE *FsHandle = (EFI_HANDLE *)NULL;
    CHAR16 *VolumeName = (CHAR16 *)NULL;
    CHAR16 *FriendlyName =  (CHAR16 *)NULL;
    EFI_BLOCK_IO_PROTOCOL  *Interface = NULL;
    EFI_USB_IO_PROTOCOL    *Usbioprotocol=NULL;
    EFI_GUID Usbioguid = EFI_USB_IO_PROTOCOL_GUID;
    BOOLEAN IsFixedUSB = FALSE;
    
    *TargetDIskInfo = (TARGET_FSINFO*)AllocateZeroPool(MAX_TARGETDISK_COUNT * sizeof(TARGET_FSINFO));

    if( (*TargetDIskInfo) == NULL)
    {
        return EFI_OUT_OF_RESOURCES;
    }
	  
    Status = gBS->LocateHandleBuffer (ByProtocol,&gEfiBlockIoProtocolGuid,NULL,&NoHandles,&pHandleBuffer);
    if(EFI_ERROR(Status))
    {
	return Status;
    }
    
    for (Index = 0; Index < NoHandles; Index++)
    {
	// get the protocol device instances..
	Status = gBS->HandleProtocol (pHandleBuffer[Index],&gEfiBlockIoProtocolGuid,&Interface);
	if(EFI_ERROR(Status))
	{
	    continue;
	}
	
	Status = gBS->HandleProtocol(pHandleBuffer[Index],&Usbioguid,(VOID **)&Usbioprotocol);
	if(EFI_SUCCESS == Status)  //Include Fixed USB devices
		IsFixedUSB = TRUE;
    
	// search for the device DISK Handle (not the partition handle)
	if ((TRUE == IsFixedUSB) || ((TRUE == Interface->Media->RemovableMedia) &&(Interface->Media->LogicalPartition == 0)))
	{
	    //Exclude CD and Floppy
	    if( (Interface->Media->BlockSize != LOGICAL_BLOCK_SIZE) ||(Interface->Media->LastBlock <= FLOPPY_LAST_BLOCK))
	    {
		IsFixedUSB = FALSE;
		continue;
	    }
	    
	    AMI_FS_GetFSHandleforBlock(pHandleBuffer[Index],&FsHandle,&HandleCount);
	    for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
	    {
		if(VolumeName)
		{
			FreePool(VolumeName);
		    VolumeName = (CHAR16 *)NULL;
		}
		VolumeName = AMI_FS_GetVolumeNameForFSHandle(FsHandle[HandleIndex]);
		if(VolumeName != NULL)
		{
			if(L'\0' == VolumeName[0])//If no volume label is Assigned
			{
				Wcscpy(VolumeName,L"USB");  
			}
   
			(*TargetDIskInfo)[TagetDiskCount].FsHandle = FsHandle[HandleIndex];
			Wcscpy((*TargetDIskInfo)[TagetDiskCount].VolumeName, VolumeName);
	 
			FriendlyName = (CHAR16 *)AMI_FS_GetDiskName(pHandleBuffer[Index]);
			if(FriendlyName != NULL)
			{
				if(L'\0' == FriendlyName[0])//If no Friendly Name, copy VolumeName.
				{
					Wcscpy((*TargetDIskInfo)[TagetDiskCount].FriendlyName, VolumeName);
				}
				else
				{
					Wcscpy((*TargetDIskInfo)[TagetDiskCount].FriendlyName, FriendlyName);
				}
			}
			TagetDiskCount++;
		}
	    }
	    if(FsHandle)
	    {
	    	FreePool(FsHandle);
	    	FsHandle = (EFI_HANDLE *)NULL;
	    }
	    IsFixedUSB = FALSE;
	}
    }
    
#if (_NT32_BUILD == 1)
      //List all additional FS without disk - for simulator testing
    if(pHandleBuffer)
	    FreePool(pHandleBuffer);
        
    Status = gBS->LocateHandleBuffer (ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&NoHandles,&pHandleBuffer);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
    for (Index = 0; Index < NoHandles; Index++)
    {
        BOOLEAN MatchFound = FALSE;
        
        // Check if the FS is already added
        for(HandleIndex = 0; HandleIndex < TagetDiskCount; HandleIndex++)
        {
            if(((*TargetDIskInfo)[HandleIndex].FsHandle)== pHandleBuffer[Index])
            {
                MatchFound = TRUE;
                break;
            }
        }
        
        // Already added continue
        if(MatchFound)
            continue;
        
    
        if(VolumeName)
        {
        	FreePool(VolumeName);
            VolumeName = (CHAR16 *)NULL;
        }
        
        // New entry add details with dummy names
        VolumeName = AMI_FS_GetVolumeNameForFSHandle(pHandleBuffer[Index]);
        (*TargetDIskInfo)[TagetDiskCount].FsHandle = pHandleBuffer[Index];
        if(VolumeName)
            Wcscpy((*TargetDIskInfo)[TagetDiskCount].VolumeName,VolumeName);
        else
            Wcscpy((*TargetDIskInfo)[TagetDiskCount].VolumeName,L"NO_LABEL");

        Wcscpy((*TargetDIskInfo)[TagetDiskCount].FriendlyName,L"Emulated_Volume");
        TagetDiskCount++;
        
    }
#endif

    if(pHandleBuffer)
	    FreePool(pHandleBuffer);

    if(VolumeName)
	    FreePool(VolumeName);
    //if(FriendlyName)
    //    gBS->FreePool(FriendlyName);
    if(0 == TagetDiskCount)
	    FreePool((*TargetDIskInfo));
  
	*DiskCount = TagetDiskCount;//Update the Disk Count
		
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:       AMI_FS_GetFSHandleforBlock
//
// Description:     To take the handle and return the FS Handle
//
// Parameter:       EFI_HANDLE BlockIOHandle    EFI_HANDLE **FsHandle   UINT32 *HandleCount
//
// Return Value:    EFI_STATUS
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AMI_FS_GetFSHandleforBlock(EFI_HANDLE BlockIOHandle,EFI_HANDLE **FsHandle,UINT32 *HandleCount)
{
    EFI_GUID g_DiskIO = EFI_DISK_IO_PROTOCOL_GUID;
    EFI_GUID g_FsProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenEntry = NULL;
    EFI_FILE *Fs;
    UINTN   EntryCount = 0;
    UINTN OpenEntryCount = 0;
    UINTN FsCount = 0;
    EFI_STATUS Status = EFI_NOT_FOUND;

    Status = gBS->OpenProtocolInformation(BlockIOHandle,&g_DiskIO,&OpenEntry,&OpenEntryCount);
    if (EFI_SUCCESS == Status)
    {

	    *FsHandle = AllocatePool(16 * sizeof(EFI_HANDLE));
        for(EntryCount=0;EntryCount<OpenEntryCount;EntryCount++)
        {
            if(OpenEntry[EntryCount].Attributes == EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER)
            {
                Status = gBS->HandleProtocol (OpenEntry[EntryCount].ControllerHandle, &g_FsProtocol, (VOID**)&Fs);
                if (EFI_SUCCESS == Status)
                {                   
                    {
                        //Print device path for blkio
                        EFI_STATUS TempStatus;
                        EFI_DEVICE_PATH_PROTOCOL  *DevicePathInterface=NULL;
                        EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevicePathToText = NULL;
                        CHAR16  *BlkIODevicePath = NULL;
                        TempStatus = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid,NULL,(VOID **) &DevicePathToText);
                        if(EFI_SUCCESS == TempStatus)
                        {
                            TempStatus= gBS->HandleProtocol(BlockIOHandle,&gEfiDevicePathProtocolGuid,(VOID **)&DevicePathInterface);
                            if(EFI_SUCCESS == TempStatus)
                            {
                                BlkIODevicePath = DevicePathToText->ConvertDevicePathToText (DevicePathInterface,FALSE,TRUE);
                            }                       
                        }                              
                    }
                    //Copy Handle to FsHandle
                    *((*FsHandle)+FsCount) = OpenEntry[EntryCount].ControllerHandle;
                    FsCount++;
                }
            }
        }
        FreePool(OpenEntry);
    }
    *HandleCount = (UINT32)FsCount;

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
// Procedure:   AMI_FS_GetVolumeNameForFSHandle
//
// Description: Gets the Volume name of the Provided Handle
//
// Input:       EFI_HANDLE FsHandle
//              
// Output:      CHAR16 *
//---------------------------------------------------------------------------
CHAR16 * AMI_FS_GetVolumeNameForFSHandle( EFI_HANDLE FsHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)NULL;
    EFI_FILE_PROTOCOL *FileSystem = (EFI_FILE_PROTOCOL *)NULL;
    CHAR16 *VolName = (CHAR16 *)NULL;
    UINTN      FileSystemInfoSize = (UINTN)sizeof(EFI_FILE_SYSTEM_INFO) + 1024;
    EFI_FILE_SYSTEM_INFO    *FileSystemInfo = (EFI_FILE_SYSTEM_INFO*)NULL;
    EFI_GUID FileSystemInfoType = EFI_FILE_SYSTEM_INFO_ID;
    
    if(FsHandle == NULL)
        return NULL;
    Status = gBS->HandleProtocol(FsHandle, &gEfiSimpleFileSystemProtocolGuid,(void**) &SimpleFileSystem);
    if(!EFI_ERROR(Status))
    {
        Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileSystem);
    }
    
    if(Status == EFI_SUCCESS && FileSystem)
    {
	    FileSystemInfo = AllocatePool(FileSystemInfoSize);
       if(FileSystemInfo)
       {
           Status = FileSystem->GetInfo(FileSystem, &FileSystemInfoType, &FileSystemInfoSize, FileSystemInfo);
           if(Status == EFI_BUFFER_TOO_SMALL)
           {
               if (FileSystemInfo != NULL)
               {
        	       FreePool(FileSystemInfo);
                   FileSystemInfo = (EFI_FILE_SYSTEM_INFO *)NULL;
               }
               FileSystemInfo = AllocatePool(FileSystemInfoSize); 
               if ( !FileSystemInfo )
                   return NULL;
               Status = FileSystem->GetInfo(FileSystem, &FileSystemInfoType, &FileSystemInfoSize, FileSystemInfo);
               if ( Status != EFI_SUCCESS ) 
               {
                   return NULL;
               }
           }
           if (!EFI_ERROR(Status))
           {
                if(FileSystemInfo->VolumeLabel)
                {
                VolName = AllocatePool((Wcslen(FileSystemInfo->VolumeLabel)+1)*2);
                    if ( !VolName ) 
                    {
                    return NULL;
                    }
                    Wcscpy(VolName,FileSystemInfo->VolumeLabel);
                }
                else
                   return NULL;
           }
       }
    }
    if(FileSystemInfo)
	    FreePool(FileSystemInfo);
    
    return VolName;
}

//-------------------------------------------------------------------
// Procedure: void AMI_FS_GetDiskName()
//
// Description: Used to return the controller name of the given handle
//
// Parameter: EFI_HANDLE Handle
//
// Return Value: CHAR16*
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR16* AMI_FS_GetDiskName(EFI_HANDLE Handle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    CHAR16 *CtrlName = (CHAR16 *)NULL;
    BOOLEAN BoolStatus;
    CHAR16 *DeviceTextPath = (CHAR16 *)NULL;
    BoolStatus = GetControllerName(Handle,&CtrlName);
    if(BoolStatus == FALSE)
    {
        //AMI_OS_AllocateZeroPool((VOID PTR PTR)&CtrlName,sizeof(CHAR16) * 10); //10 to hold the name "OEM"
	    CtrlName = AllocatePool (  sizeof(CHAR16) * 10);
        if(!CtrlName)
            return NULL;
        Wcscpy(CtrlName,(CHAR16*)L"OEM");
    }
    return CtrlName;
}

#if 0
//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:       EFI_HANDLE  USBBlockioHandle()
//
// Description:     Used to return the USB Block IO handle
//
// Parameter:       EFI_HANDLE  InFSHandle
//
// Return Value:    EFI_HANDLE
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_HANDLE USBBlockioHandle(EFI_HANDLE InFSHandle)
{
    EFI_BLOCK_IO_PROTOCOL  *BlockIOProtocolInterface = NULL ;
    
    EFI_STATUS Status=0;
    EFI_HANDLE *HandleBuffer = NULL;
    EFI_HANDLE   *FsHandles = NULL;
    EFI_HANDLE   BlkIOHandle = NULL;
    EFI_DEVICE_PATH_PROTOCOL  *FsDevicePathInterface=NULL;
    EFI_DEVICE_PATH_PROTOCOL  *BlkioDevicePathInterface=NULL;
    EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevicePathToText;
    UINTN   NoOfHandles=0,i=0,BlockSize=0;
    UINT32      FsHandleCount = 0;
    UINTN Count = 0 ;
    BOOLEAN LEPTFsFound = FALSE;
    CHAR16  *BlkIODevicePath = NULL;
    CHAR16  *FsDevicePath = NULL;
    BOOLEAN HandleFound = FALSE;
    
    Status = gBS->LocateHandleBuffer(ByProtocol,&gEfiBlockIoProtocolGuid ,NULL,&NoOfHandles,(EFI_HANDLE **)&HandleBuffer);
    if(Status == EFI_SUCCESS)
    {             
        //Print((CHAR16*)L"LogicalPartition 1\n");
        for(i=0;i<NoOfHandles;i++)
        {
            LEPTFsFound = FALSE ;
            Status= gBS->HandleProtocol(HandleBuffer[i],&gEfiBlockIoProtocolGuid,(VOID **)&BlockIOProtocolInterface);
            if(Status == EFI_SUCCESS)
            {
               /* if(BlockIOProtocolInterface->Media->LogicalPartition)
                {
                    Print((CHAR16*)L"LogicalPartition\n");
                    continue;
                }*/
                
                // Commented to allow removable USB storage devices
                //if(!BlockIOProtocolInterface->Media->RemovableMedia)
                //{
                //  continue;
                //}
                //else
                //{
                    BlockSize = BlockIOProtocolInterface->Media->BlockSize;
                    if(BlockSize == LOGICAL_BLOCK_SIZE)                                              ////////////////checking for the USB device
                    {
                        if (BlockIOProtocolInterface->Media->LastBlock  < 5760)
                                continue;
                        Status= gBS->HandleProtocol(HandleBuffer[i],&gEfiDevicePathProtocolGuid,(VOID **)&BlkioDevicePathInterface);
                        if(Status == EFI_SUCCESS)
                        {
                            Status= gBS->HandleProtocol(InFSHandle,&gEfiDevicePathProtocolGuid,(VOID **)&FsDevicePathInterface);
                            if(Status == EFI_SUCCESS)
                            {             
                                Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid,NULL,(VOID **) &DevicePathToText);
                                if(Status == EFI_SUCCESS)
                                {
                                    BlkIODevicePath = DevicePathToText->ConvertDevicePathToText (BlkioDevicePathInterface,  FALSE,TRUE);
                                    FsDevicePath = DevicePathToText->ConvertDevicePathToText (FsDevicePathInterface,FALSE,TRUE);

                                    if(!MemCmp(FsDevicePath,BlkIODevicePath,Wcslen(BlkIODevicePath)*2))
                                    {
                                        BlkIOHandle = HandleBuffer[i];
                                        HandleFound = TRUE;
                                        Print((CHAR16*)L"AvailableFSDetails HandleFound = TRUE;\n%s",FsDevicePath);
                                        if(BlkIODevicePath)
                                        	FreePool(BlkIODevicePath);
                                        
                                        if(FsDevicePath)
                                        	FreePool(FsDevicePath);

                                        break;                                  
                                    }
                                    
                                    if(BlkIODevicePath)
                                	    FreePool(BlkIODevicePath);

                                    if(FsDevicePath)
                                	    FreePool(FsDevicePath);
                                }
                            }
                        }                           
                    }
                //}
            }
        }
        if(HandleBuffer)
        	FreePool(HandleBuffer);
    }
    if(HandleFound == TRUE)
        return BlkIOHandle;     
    else
        return 0;
}
#endif

//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

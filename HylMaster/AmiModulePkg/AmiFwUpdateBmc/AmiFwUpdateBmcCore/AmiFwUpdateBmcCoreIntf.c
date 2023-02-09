//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateCoreIntf.c
//
// Description: All the UI and Core interfaces are available here
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
#include "AmiFwUpdateBmcCoreIntf.h"
#include "AmiFwUpdateBmcStrDefs.h"
#include "AmiFwUpdateBmcUIIntf.h" 
#include "FileSystemInfo.h"
#include  <Protocol/AmiAfuProtocol.h>
#include <Protocol/SmiFlash.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>

#define		OTHERFW_RESET_REQUIRED	02
extern FLASH_POLICY_INFO_BLOCK	gFlashingPolicy ;
extern AMI_AFU_PROTOCOL *gAFUFlash;

extern TARGET_FSINFO *gUITargetDIskInfo;
extern UINT32 gUIDiskCount;
extern BOOLEAN gBcpPreserveFlag;

UINTN gFlashOPtions = 0;
UINTN gFlashMethod = 0;
UINTN gUpdateFlag = 0;
UINTN gSelection = 0;
EFI_HANDLE gSourceDiskHandle;
BOOLEAN IsRestartFlagEnabled = FALSE;
BOOLEAN gBlosFlagEnabled =  TRUE;
CHAR16 gROMfilename[MAX_LARGE_STRING_SIZE];

UINTN   gNcbBlockNumber = 0;
UINTN   gRomHoleBlockNumber = 0;

extern BOOLEAN IsSecureUpdateEnabled();
extern RETURN_STATUS UnicodeStrToAsciiStrS(); //kuaibai ++

extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
//extern CHAR16 *HiiGetString( VOID* handle, UINT16 token );
extern VOID* gHiiHandle;
extern VOID     EfiStrCpy (    CHAR16   *Destination,    CHAR16   *Source    );

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: FWCore_IsSecureFlashSupported
//
// Description	: To check whether secure flash is supported by the selected flashing method or not
//
// Parameter	: UINT8 
//				
// Return Value  : BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN FWCore_IsSecureFlashSupported(UINT8 Policy)
{	
    TRACE((-1, "AMIFW:  gFlashingPolicy.FlUpdPolicy.FlashUpdate = %x gFlashingPolicy.FlUpdPolicy.BBUpdate = %x Policy = %x\n", gFlashingPolicy.FlUpdPolicy.FlashUpdate, gFlashingPolicy.FlUpdPolicy.BBUpdate ,Policy));
	if (0 == (gFlashingPolicy.FlUpdPolicy.FlashUpdate & Policy))
	{	
		return FALSE;
	}
	
	if (0 == (gFlashingPolicy.FlUpdPolicy.BBUpdate & Policy))
	{	
		return FALSE;
	}
	
	return TRUE;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : UpdateEraseStatus
//
// Description  : To set the progress of flashing process
//
// Parameter    : UINT16 token, UINT64 CurLoc, UINT64 Progress 
//              
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateWriteStatus(UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag)
{
    UINT64 Progress;
    
    if (CurLoc == 0xffffffff) 
    {
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHBOOTBLKDONE);
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHMAINBLKDONE);
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHNVRAMDONE);
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHMEDONE);
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHOABLKDONE);
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_FLASHNCBDONE);
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_FLASHNCBDONE);
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_FLASHROMHOLESDONE);
            }
            break;
        }
    }
    else
    {
        Progress = (((MaxLoc - CurLoc)*100)/Size);
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_FLASHBOOTBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_FLASHMAINBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_FLASHNVRAM, CurLoc, (100 - Progress));
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_FLASHME, CurLoc, (100 - Progress));
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_FLASHOABLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_FLASHNCB, CurLoc, (100 - Progress));
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_FLASHNCB, CurLoc, (100 - Progress));
                UpdateFlashingSuccess(STR_FLASHNCBDONE);
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_FLASHROMHOLES, CurLoc, (100 - Progress));
            }
            break;
        }
    }
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : UpdateEraseStatus
//
// Description  : To set the progress of flashing process
//
// Parameter    : UINT16 token, UINT64 CurLoc, UINT64 Progress 
//              
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateVerifyStatus(UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag)
{
    UINT64 Progress;
    
    if (CurLoc == 0xffffffff) 
    {
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYBOOTBLKDONE);
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYMAINBLKDONE);
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYNVRAMDONE);
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYMEDONE);
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYOABLKDONE);
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_VERIFYNCBDONE);
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_VERIFYNCBDONE);
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_VERIFYROMHOLESDONE);
            }
            break;
        }
    }
    else
    {
        Progress = (((MaxLoc - CurLoc)*100)/Size);
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_VERIFYBOOTBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_VERIFYMAINBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_VERIFYNVRAM, CurLoc, (100 - Progress));
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_VERIFYME, CurLoc, (100 - Progress));
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_VERIFYOABLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_VERIFYNCB, CurLoc, (100 - Progress));
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_VERIFYNCB, CurLoc, (100 - Progress));
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_VERIFYROMHOLES, CurLoc, (100 - Progress));
            }
            break;
        }
    }
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : UpdateEraseStatus
//
// Description  : To set the progress of flashing process
//
// Parameter    : UINT16 token, UINT64 CurLoc, UINT64 Progress 
//              
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateEraseStatus(UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag)
{
    UINT64 Progress;
    
    if (CurLoc == 0xffffffff) 
    {
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASEBOOTBLKDONE);
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASEMAINBLKDONE);
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASENVRAMDONE);
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASEMEDONE);
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASEOABLKDONE);
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
            UpdateFlashingSuccess(STR_ERASENCBDONE);
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_ERASENCBDONE);
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_FLASHCLEARMESSAGE, 0, 0);
                UpdateFlashingSuccess(STR_ERASEROMHOLESDONE);
            }
            break;
        }
    }
    else
    {
        Progress = (((MaxLoc - CurLoc)*100)/Size);
        switch(Flag & FLASHING_REGION)
        {
        case FLASH_BOOT_BLOCK:
            UpdateFlashingStatus(STR_ERASEBOOTBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_MAIN_BLOCK:
            UpdateFlashingStatus(STR_ERASEMAINBLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NVRAM:
            UpdateFlashingStatus(STR_ERASENVRAM, CurLoc, (100 - Progress));
            break;
        case FLASH_FULL_ME:
            UpdateFlashingStatus(STR_ERASEME, CurLoc, (100 - Progress));
            break;
        case FLASH_OA3_BLOCK:
            UpdateFlashingStatus(STR_ERASEOABLK, CurLoc, (100 - Progress));
            break;
        case FLASH_NON_CRITICAL_BLOCK:
            UpdateFlashingStatus(STR_ERASENCB, CurLoc, (100 - Progress));
            break;
        default:
            if(Flag & 0x00FFFF00)
            {
                UpdateFlashingStatus(STR_ERASENCB, CurLoc, (100 - Progress));
            }
            else if( ((Flag & FLASH_ROM_HOLE) == FLASH_ROM_HOLE) || (Flag & 0x00FFFF000000) )
            {
                UpdateFlashingStatus(STR_ERASEROMHOLES, CurLoc, (100 - Progress));
            }
            break;
        }
    }
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: DisplayCommonProgress
//
// Description	: To display the progress of flashing process
//
// Parameter	: UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag 
//				
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID DisplayCommonProgress (UINT64 CurLoc , UINT64 MaxLoc , UINT64 Size ,UINTN Flag)
{
	switch (Flag & FLASHING_OPERATION)
	{
	case ERASE_BLOCK:
        UpdateEraseStatus(CurLoc, MaxLoc, Size, Flag);
        break;
    case WRITE_BLOCK:
        UpdateWriteStatus(CurLoc, MaxLoc, Size, Flag);
        break;
    case VERIFY_BLOCK:
        UpdateVerifyStatus(CurLoc, MaxLoc, Size, Flag);
        break;
    default:
        break;
	}
	
	return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: UpdateFlashingStatus
//
// Description	: To set the progress of flashing process
//
// Parameter	: UINT16 token, UINT64 CurLoc, UINT64 Progress 
//				
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateFlashingStatus(UINT16 token, UINT64 CurLoc, UINT64 Progress)
{
	CHAR16	*Text = 0;
	CHAR16	*TempText = (CHAR16 *)NULL;

	Text = AllocatePool(MAX_LARGE_STRING_SIZE);
	
	TempText = FW_HiiGetString( gHiiHandle,token );

		Swprintf(Text,TempText, CurLoc, Progress);
	 
	if(TempText)
	{	
		FreePool(TempText);
		TempText = (CHAR16 *)NULL;
	}
	
	FWCore_ReportStatus(0, Text);
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: UpdateFlashingSuccess
//
// Description	: To display the success string of flashing
//
// Parameter	: UINT16 token 
//				
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateFlashingSuccess(UINT16 token)
{
	CHAR16	*TempText = (CHAR16 *)NULL;
	
	TempText = FW_HiiGetString( gHiiHandle,token );
	
	//FWCore_ReportStatus(0, TempText);
	FWCore_ReportEndStatus(0, TempText);
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: FWCore_SetSourceDiskHandle
//
// Description	: To set the Target disk handle
//
// Parameter	: EFI_HANDLE 
//				
// Return Value  : void
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void FWCore_SetSourceDiskHandle(EFI_HANDLE FsHandle)
{
	gSourceDiskHandle = FsHandle;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: FWCore_OpenROMImageFile
//
// Description	: To set the ROM file name 
//
// Parameter	: void **Buffer, UINT32 *Size
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FWCore_OpenROMImageFile(void **Buffer, UINTN *Size)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystemInterface;
	EFI_FILE_PROTOCOL	 *mRoot = NULL;
	EFI_FILE_PROTOCOL  *FileHandle = NULL;
	UINTN FileInfoSize = 0;
	EFI_FILE_INFO *FileInfo = (EFI_FILE_INFO *)NULL;
	UINTN FileSize = 0;
    UINTN Index = 0;
    UINTN NumberFileSystemHandles = 0;
    EFI_HANDLE *FileSystemHandles = 0;
    void *TempBuffer = NULL;
    
    Status = gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &NumberFileSystemHandles,
        &FileSystemHandles
        );
    DEBUG((EFI_D_ERROR, "[SK] LocateHandleBuffer FileSystem = %r. NumberFileSystemHandles = %x\n",Status,NumberFileSystemHandles)); //Spiral
    if (EFI_ERROR (Status))
    {
    	return Status;
    }
    for (Index = 0; Index < NumberFileSystemHandles; Index++) {
        gSourceDiskHandle = FileSystemHandles[Index];
        Status = gBS->HandleProtocol(gSourceDiskHandle, &gEfiSimpleFileSystemProtocolGuid,(VOID**)&SimpleFileSystemInterface);
        DEBUG((EFI_D_ERROR, "[SK] HandleProtocol FileSystem = %r.\n",Status)); //Spiral
        if(EFI_SUCCESS == Status)
        {
            Status = SimpleFileSystemInterface->OpenVolume(SimpleFileSystemInterface, &mRoot);
            DEBUG((EFI_D_ERROR, "[SK] OpenVolume FileSystem = %r.\n",Status)); //Spiral
            if(EFI_SUCCESS == Status)
            {
                DEBUG((EFI_D_ERROR, "[SK] before Open gROMfilename = %s.\n",gROMfilename)); //Spiral
                Status = mRoot->Open(mRoot, &FileHandle, gROMfilename,( EFI_FILE_MODE_READ), 0);
                DEBUG((EFI_D_ERROR, "[SK] Open gROMfilename = %r %s.\n",Status,gROMfilename)); //Spiral
                if(EFI_SUCCESS == Status)
                {
                    FileInfoSize =  (UINTN)(sizeof(EFI_FILE_INFO) + 128);
                    FileInfo = AllocatePool(FileInfoSize);
                    Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
                    DEBUG((EFI_D_ERROR, "[SK] GetInfo FileSystem = %r.\n",Status)); //Spiral
                    if(EFI_BUFFER_TOO_SMALL==Status)
                    {
                        FileInfo = AllocatePool(FileInfoSize);
                        Status = FileHandle->GetInfo(FileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
                        DEBUG((EFI_D_ERROR, "[SK] GetInfo Adjust size FileSystem = %r.\n",Status)); //Spiral
                    }
                    
                    if(EFI_SUCCESS == Status)
                    {
                        FileSize = (UINTN)(FileInfo->FileSize);
                        TempBuffer =  AllocatePool(FileSize);
                        Status = FileHandle->Read(FileHandle, &FileSize, TempBuffer);
                        DEBUG((EFI_D_ERROR, "[SK] Read File = %r.\n",Status)); //Spiral
                        if(EFI_BUFFER_TOO_SMALL == Status)
                        {
                            TempBuffer = AllocatePool((UINTN)FileSize);
                            Status = FileHandle->Read(FileHandle, &FileSize, TempBuffer);
                            DEBUG((EFI_D_ERROR, "[SK] Read Adjust size FileSystem = %r.\n",Status)); //Spiral
                        }
                        
                        if(EFI_SUCCESS == Status)
                        {
                            //Buffer[FileSize] = '\0';
                            *Buffer = TempBuffer;
                            *Size = FileSize;
                            return EFI_SUCCESS;
                        }
                        else
                            FWCore_ReportError(STR_ROM_FILE_READING_FAILURE, (CHAR16*)NULL);	
                    }
                }
//                else
//                    FWCore_ReportError(STR_FILE_NOTFOUND, (CHAR16*)NULL);
            }
            else
                FWCore_ReportError(STR_SRCDISK_ROOTVOL_OPENFAILED, (CHAR16*)NULL);
        }
        else
            FWCore_ReportError(STR_SRCDISK_HDLNOTFOUND, (CHAR16*)NULL);
    
        if(FileInfo)
            FreePool(FileInfo);
    
        if(FileHandle)
            FileHandle->Close(FileHandle);
    }
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: FWCore_ValidateBIOSImage
//
// Description	: To validate BIOS image 
//
// Parameter	: VOID *, UINT32
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FWCore_ValidateBIOSImage(VOID* FileBuffer, UINT32 FileSize)
{
	EFI_STATUS Status = EFI_SUCCESS;

	Status = gAFUFlash->ValidateRomFile((UINTN)(VOID *)FileBuffer,(UINT32)FileSize );  
	
	return Status;
}

void UpdateFlashingFlags(UINTN FlashMethod, UINTN FlashOPtions, UINTN UpdateFlag, CHAR16 *RomFileName, UINTN SelctionFlag)
{
   gFlashOPtions = FlashOPtions;
   gFlashMethod = FlashMethod ;
   gUpdateFlag = UpdateFlag;
   gSelection = SelctionFlag;
   if(RomFileName)
       Swprintf(gROMfilename, (CHAR16*)L"%s", (CHAR16*)RomFileName);
}

void AppendStringToList(COMMAND_ARG_LIST **List,UINTN ArgIndex, CHAR16 *Str)
{
    COMMAND_ARG_LIST *Visit = (COMMAND_ARG_LIST*)NULL;

    if(NULL == *List)
        *List = (COMMAND_ARG_LIST*)AllocateZeroPool(MAX_ARG_COUNT * sizeof(COMMAND_ARG_LIST));
    
    Visit = *List;
    
    if(ArgIndex< MAX_ARG_COUNT)
        Visit[ArgIndex] = (COMMAND_ARG_LIST)Str;
}

/**
      returns the NCB block flash option value based on the input Block number provided
    
      @param  BlockNumber     Block number
    
      @retval UINTN           Flash index value to set for NCB block number to flash
**/
UINTN GetNcbBlock(UINTN   BlockNumber)
{
    switch(BlockNumber)
    {
    case 0:
        return FLASH_NON_CRITICAL_BLOCK_0;
    case 1:
        return FLASH_NON_CRITICAL_BLOCK_1;
    case 2:
        return FLASH_NON_CRITICAL_BLOCK_2;
    case 3:
        return FLASH_NON_CRITICAL_BLOCK_3;
    case 4:
        return FLASH_NON_CRITICAL_BLOCK_4;
    case 5:
        return FLASH_NON_CRITICAL_BLOCK_5;
    case 6:
        return FLASH_NON_CRITICAL_BLOCK_6;
    case 7:
        return FLASH_NON_CRITICAL_BLOCK_7;
    case 8:
        return FLASH_NON_CRITICAL_BLOCK_8;
    case 9:
        return FLASH_NON_CRITICAL_BLOCK_9;
    case 10:
        return FLASH_NON_CRITICAL_BLOCK_10;
    case 11:
        return FLASH_NON_CRITICAL_BLOCK_11;
    case 12:
        return FLASH_NON_CRITICAL_BLOCK_12;
    case 13:
        return FLASH_NON_CRITICAL_BLOCK_13;
    case 14:
        return FLASH_NON_CRITICAL_BLOCK_14;
    case 15:
        return FLASH_NON_CRITICAL_BLOCK_15;
    default:
        break;
    }
    
    return 0;
}

/**
      returns the Rom hole block flash option based on the input Block number provided
    
      @param  BlockNumber     Block number
    
      @retval UINTN           Flash index value to set for Rom hole block number to flash
**/
UINTN GetRomHoleBlock(UINTN   BlockNumber)
{
    switch(BlockNumber)
    {
    case 0:
        return FLASH_ROM_HOLE_0;
    case 1:
        return FLASH_ROM_HOLE_1;
    case 2:
        return FLASH_ROM_HOLE_2;
    case 3:
        return FLASH_ROM_HOLE_3;
    case 4:
        return FLASH_ROM_HOLE_4;
    case 5:
        return FLASH_ROM_HOLE_5;
    case 6:
        return FLASH_ROM_HOLE_6;
    case 7:
        return FLASH_ROM_HOLE_7;
    case 8:
        return FLASH_ROM_HOLE_8;
    case 9:
        return FLASH_ROM_HOLE_9;
    case 10:
        return FLASH_ROM_HOLE_10;
    case 11:
        return FLASH_ROM_HOLE_11;
    case 12:
        return FLASH_ROM_HOLE_12;
    case 13:
        return FLASH_ROM_HOLE_13;
    case 14:
        return FLASH_ROM_HOLE_14;
    case 15:
        return FLASH_ROM_HOLE_15;
    default:
        break;
    }

    return 0;
}

/**
      Parse the Argument string and returns the Argument type
    
      @param  ArgStr     Argument string provided as input
    
      @retval FLASH_OPTION          Argument type (anyone of ENUM value from FLASH_OPTION)
**/
FLASH_OPTION GetArgumentType(COMMAND_ARG_LIST ArgStr)
{
    FLASH_OPTION RetType = FLASH_OP_UNKNOWN;
    CHAR8 *ArgStr8 = NULL;
    EFI_STATUS Status = EFI_NOT_FOUND;
    UINTN ArgLen = 0;
    
    if( ArgStr[0] == L'/')
    {
        ArgLen = StrLen(ArgStr) + 1;
        ArgStr8 = (CHAR8 *)AllocateZeroPool(ArgLen * sizeof (CHAR8));
        if (ArgStr8 == NULL) {
            DEBUG((DEBUG_ERROR, "Memory Allocate failed\n"));
            return FLASH_OP_UNKNOWN;
        }
        Status = UnicodeStrToAsciiStrS(ArgStr, ArgStr8, ArgLen);
        
        if(!EFI_ERROR(Status))
        {
            if(!AsciiStriCmp(ArgStr8, "/p"))
                RetType = FLASH_OP_MAIN_BLOCK;
            else if(!AsciiStriCmp(ArgStr8, "/b"))
                RetType = FLASH_OP_BOOT_BLOCK;
            else if(!AsciiStriCmp(ArgStr8, "/n"))
                RetType = FLASH_OP_NVRAM;
            else if(!AsciiStriCmp(ArgStr8, "/me"))
                RetType = FLASH_OP_ME;
            else if(!AsciiStriCmp(ArgStr8, "/k"))
                RetType = FLASH_OP_NCB;
            else if( (ArgStr[1] == L'k') && (ArgStr[2]!= L'\0') || (ArgStr[1] == L'K') && (ArgStr[2]!= L'\0') )
            {
                gNcbBlockNumber = StrDecimalToUintn(ArgStr+2);
                RetType = FLASH_OP_NCBN;
            }
            else if(!AsciiStriCmp(ArgStr8, "/l"))
                RetType = FLASH_OP_ROM_HOLES;
            else if( (ArgStr[1] == L'l') && (ArgStr[2]!= L'\0') || (ArgStr[1] == L'L') && (ArgStr[2]!= L'\0') )
            {
                gRomHoleBlockNumber = StrDecimalToUintn(ArgStr+2);
                RetType = FLASH_OP_ROM_HOLESN;
            }
            else if(!AsciiStriCmp(ArgStr8, "/r"))
                RetType = FLASH_OP_PRESERVE_SMBIOS;
            else if(!AsciiStriCmp(ArgStr8, "/sp"))
                RetType = FLASH_OP_PRESERVE_SETUP;
            else if(!AsciiStriCmp(ArgStr8, "/capsule"))
                RetType = FLASH_OP_CAPSULE;
            else if(!AsciiStriCmp(ArgStr8, "/recovery"))
                RetType = FLASH_OP_RECOVERY;
            else if(!AsciiStriCmp(ArgStr8, "/reboot"))
                RetType = FLASH_OP_REBOOT;
            else if(!AsciiStriCmp(ArgStr8, "/clrcfg"))
                RetType = FLASH_OP_NO_PRESERVE_BCP;
            else if(!AsciiStriCmp(ArgStr8, "/bcpall"))
                RetType = FLASH_OP_PRESERVE_BCP_ALL;
        }
        if(ArgStr8)
        {
            FreePool(ArgStr8);
        }
    }
    
    return RetType;
}

UINTN GetCommandLineStr(CHAR16 *SrcStr,CHAR16 DelChar,COMMAND_ARG_LIST **List, UINT32 OptionSize)
{
    CHAR16 *Str = (CHAR16*)NULL;
    UINTN StartOffset = 0;
    UINTN DestLength = 0;
    UINTN SrcLength = 0;
    UINTN ArgCount = 0;
    UINTN Index = 0;

    if(OptionSize)
        SrcLength = OptionSize;
    else if(NULL != SrcStr)
        SrcLength = StrLen(SrcStr);
    
    if((NULL != SrcStr) && (0 != SrcLength) )
    {
        for(Index = 0; Index <= SrcLength; Index++)
        {
            if((SrcStr[Index] == DelChar) || (!SrcStr[Index]))
            {
                DestLength = Index - StartOffset;
                Str = (CHAR16*)AllocateZeroPool((DestLength + 1) * sizeof(CHAR16));
                StrnCpy(Str, SrcStr + StartOffset, DestLength);
                AppendStringToList(List,ArgCount,Str);
                StartOffset = Index + 1;
                ArgCount++;
            }
        }
    }
    return ArgCount;
}

/**
      Parses the arguments and store the Flash options in global variables
    
      @param  CommandArgStr     Arguments string
      @param  OptionSize        Length of arguments string
    
      @retval BOOLEAN           returns TRUE if all arguments are parsed successfully
**/
BOOLEAN SetGlobalFlagForFlash(CHAR16 *CommandArgStr, UINT32 OptionSize)
{
    UINTN Index = 0;
    UINTN CheckBoxPreserve = 0x00000000 ;
    UINTN CheckBoxOptions = 0x00000000 ;
    COMMAND_ARG_LIST *ArgList = (COMMAND_ARG_LIST*)NULL;
    UINTN ArgCount = 0; 
    UINTN FlashMethod = 0;
    UINTN FlashOPtions = 0; 
    UINTN UpdateFlag = 0;
    UINTN SelectionFlag = 0;
    CHAR16 *ROMfilename = (CHAR16 *)NULL;
    CHAR16 RomFileDriveName[128];
    UINTN SelectedDiskIndex = 0;
    UINTN ArgStrLen = 0;
	COMMAND_ARG_LIST TempArgList = (COMMAND_ARG_LIST)NULL;
    
    if(*CommandArgStr == (CHAR16)L' ')
        CommandArgStr++;		//TODO: if increased OptionSize should be decremented
    
    ArgCount = GetCommandLineStr(CommandArgStr,(CHAR16)L' ',&ArgList, OptionSize);
    DEBUG((DEBUG_ERROR, "[SK] ArgCount = %x;\n",ArgCount)); //Spiral
    if(ArgCount <= 1)
        return FALSE;
    DEBUG((DEBUG_ERROR, "[SK] 1.\n")); //Spiral
    FlashMethod = 0;
    
    if(NULL == gUITargetDIskInfo)
        AMI_FS_GetFileSystemsInformation(&gUITargetDIskInfo, (UINT32*)&gUIDiskCount) ;
    
    for(Index = 0;Index<ArgCount;Index++ )
    {
    	DEBUG((DEBUG_ERROR, "Argument %d = %s\n",Index, ArgList[Index]));
    	TempArgList = ArgList[Index] ;
		if((Index == 0) && (TempArgList[0] != L'/') )
		{	
			ArgStrLen = StrLen(ArgList[0]);
			if( ((CHAR16 *)L'[' == (CHAR16 *)ArgList[0]) && ((CHAR16 *)L']' == (CHAR16 *)ArgList[ArgStrLen-1]) )
			{
				UINTN SrcLength = 0;
				if(NULL != ArgList[Index])
					SrcLength = StrLen(ArgList[Index]);
				if(SrcLength)
				{
					StrnCpy(RomFileDriveName,ArgList[Index]+1,SrcLength-2);
					RomFileDriveName[SrcLength-2] = (CHAR16)L'\0';
                	SelectedDiskIndex = GetHandleIndexFromVol(RomFileDriveName);
                
              	  	if(NULL != gUITargetDIskInfo)
                	{
                    	if(SelectedDiskIndex < gUIDiskCount)
                        	FWCore_SetSourceDiskHandle(gUITargetDIskInfo[SelectedDiskIndex].FsHandle) ;
                	}

				}
				continue ;
			}
			else
			{
				ROMfilename = ArgList[Index];
				continue ;
			}
		}

        switch(GetArgumentType(ArgList[Index]))
        {
        case FLASH_OP_MAIN_BLOCK:
            CheckBoxOptions = CheckBoxOptions | FLASH_MAIN_BLOCK ;
            break;
        case FLASH_OP_BOOT_BLOCK:
            CheckBoxOptions = CheckBoxOptions | FLASH_BOOT_BLOCK ;
            break;
        case FLASH_OP_NVRAM:
            CheckBoxOptions = CheckBoxOptions | FLASH_NVRAM ;
            break;
        case FLASH_OP_NCB:
            CheckBoxOptions = CheckBoxOptions | FLASH_NON_CRITICAL_BLOCK ;
            break;
        case FLASH_OP_NCBN:
            CheckBoxOptions = CheckBoxOptions | GetNcbBlock(gNcbBlockNumber) ;
            break;
        case FLASH_OP_ROM_HOLES:
            CheckBoxOptions = CheckBoxOptions | FLASH_ROM_HOLE ;
            break;
        case FLASH_OP_ROM_HOLESN:
            CheckBoxOptions = CheckBoxOptions | GetRomHoleBlock(gRomHoleBlockNumber) ;
            break;
        case FLASH_OP_ME:
            CheckBoxOptions = CheckBoxOptions | FLASH_FULL_ME ;
            break;
        case FLASH_OP_PRESERVE_SMBIOS:
            CheckBoxPreserve = CheckBoxPreserve | PRESERVE_SMBIOS ;
            break;
        case FLASH_OP_PRESERVE_SETUP:
            CheckBoxPreserve = CheckBoxPreserve | PRESERVE_SETUP ;
            break;
        case FLASH_OP_NO_PRESERVE_BCP:
            SelectionFlag = 0;
            SelectionFlag = SelectionFlag | CLRCFG ;
            break;
        case FLASH_OP_PRESERVE_BCP_ALL:
            if((SelectionFlag & CLRCFG) != CLRCFG)
                SelectionFlag = SelectionFlag | BCPALL ;
            break;
        case FLASH_OP_REBOOT:
            IsRestartFlagEnabled = TRUE;
            break;
        case FLASH_OP_FILENAME:
            ROMfilename = ArgList[Index];   
            break;
        case FLASH_OP_CAPSULE:
            FlashMethod = 1;
            break;
        case FLASH_OP_RECOVERY:
            FlashMethod = 2;
            break;
        case FLASH_OP_BIOS:
            gBlosFlagEnabled = TRUE;
            break;
        case FLASH_OP_OTH:
            gBlosFlagEnabled = FALSE;
            break;
        default:
            break;
        }
    }
    TRACE((DEBUG_ERROR, "[SK] FlashOPtions = %x.\n",CheckBoxOptions)); //Spiral
    FlashOPtions = CheckBoxOptions;
    UpdateFlag = CheckBoxPreserve;
    
    UpdateFlashingFlags(FlashMethod, FlashOPtions, UpdateFlag, ROMfilename, SelectionFlag);
    
    return TRUE;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: FWCore_StartBIOSUpdate
//
// Description	: To start BIOS flashing 
//
// Parameter	: void
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS FWCore_StartBIOSUpdate()
{
	EFI_STATUS Status = EFI_SUCCESS;
	VOID *fBuffer = (VOID *)NULL;
	UINTN BiosImageSize = 0;;
	//Get the ROM file buffer and size
	Status = FWCore_OpenROMImageFile(&fBuffer, &BiosImageSize);	
	DEBUG((EFI_D_ERROR, "[SK] FWCore_OpenROMImageFile = %r.\n",Status)); //Spiral
	if(EFI_SUCCESS == Status)
	{
		//Validate the ROM file
		Status = FWCore_ValidateBIOSImage(fBuffer, (UINT32)BiosImageSize);
		if(EFI_SUCCESS == Status)
		{
			if (IsSecureUpdateEnabled())
			{
				switch(gFlashMethod)
				{
				case 0:	
					if(gFlashOPtions != 0)
					  Status = SECURE_FLASH_RUNTIME(fBuffer ,(UINT32)BiosImageSize );
					else
					  FWCore_ReportError(STR_NO_FLASHOPT, (CHAR16*)NULL);
					break;
				case 1:
					Status = SECURE_FLASH_CAPSULE(fBuffer ,(UINT32)BiosImageSize );
					break;
				case 2:
					Status = SECURE_FLASH_RECOVERY(gROMfilename ,(UINT32)BiosImageSize );
					break;
				default:
					break;
				}
			}
			else
			{
				if(gFlashOPtions != 0)								
				  Status = NON_SECURE_FLASH_BIOS(fBuffer ,(UINT32)BiosImageSize );
				else
				  FWCore_ReportError(STR_NO_FLASHOPT, (CHAR16*)NULL);
			}

			if((EFI_SUCCESS == Status) && (gFlashOPtions != 0))
			{
			    UpdateFlashingSuccess(STR_FLASHDONE_SUCCESS);
			}
		}
		else
			FWCore_ErrorMsg( Status ) ;
		
		if(fBuffer)
		{
			FreePool(fBuffer);
			fBuffer = (VOID *)NULL;
		}
	}
	
	return Status;
}

 //<AMI_PHDR_START>
 //--------------------------------------------------------------------------------
 // Procedure	: FWCore_ErrorMsg
 //
 // Description	: To display the proper error message
 //
 // Parameter	: EFI_STATUS
 // Return Value  : void
 //
 //---------------------------------------------------------------------------------
 //<AMI_PHDR_END>
 void FWCore_ErrorMsg(EFI_STATUS Status)
 {
	 switch(Status)
	 {
	 case ERROR_FILE_SIZE :
		 FWCore_ReportError(STR_VALIDATE_ROM_SIZE, (CHAR16*)NULL);
		 break ;
	 case ERROR_ROM_ID :
		 FWCore_ReportError(STR_VALIDATE_ROM_ID, (CHAR16*)NULL);
		 break ;
	 case ERROR_FILE_CHECKSUM :
		 FWCore_ReportError(STR_VALIDATE_ROM_CHECKSUM, (CHAR16*)NULL);
		 break ;
	 case ERROR_BUFFER_ALLOC :
		 FWCore_ReportError(STR_VALIDATE_ROM_BUFFER, (CHAR16*)NULL);
		 break ;
	 case ERROR_ME_FILE_SIZE :
		 FWCore_ReportError(STR_VALIDATE_ROM_ME_SIZE, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_ME :
		 FWCore_ReportError(STR_VALIDATE_FLASH_ME, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_BOOTBLOCK :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_BOOT, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_MAINBIOS :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_MAIN, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_NVRAM :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_NVRAM, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_NCB :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_NCB, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_RH :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_RH, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_EC :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_EC, (CHAR16*)NULL);
		 break ;
	 case ERROR_FLASH_OA :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_OA, (CHAR16*)NULL);
		 break ;
	 case ERROR_EC_UNSUPPORT :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_EC_UNSUPPORT, (CHAR16*)NULL);
		 break ;
	 case ERROR_NCB_UNSUPPORT :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_NCB_UNSUPPORT, (CHAR16*)NULL);
		 break ;
	 case ERROR_RH_UNSUPPORT :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_RH_UNSUPPORT, (CHAR16*)NULL);
		 break ;
	 case ERROR_ME_UNSUPPORT :
		 FWCore_ReportError(STR_UPDATE_BIOS_FLASH_ME_UNSUPPORT, (CHAR16*)NULL);
		 break ;
	 default :
		 FWCore_ReportError(STR_UPDATE_UNKNOWN_ERROR, (CHAR16*)NULL);
		 break ;
	 }
 }
 
 //<AMI_PHDR_START>
 //--------------------------------------------------------------------------------
 // Procedure    :   Get_BCP_CheckBoxState
 //
 // Description  :   To get user need BCP support or not
 //
 // Parameter    :   void
 //
 // Return Value  :  UINTN
 //
 //---------------------------------------------------------------------------------
 //<AMI_PHDR_END>
 UINTN Get_BCP_CheckBoxState()
 {
 	UINTN ReturnValue = 0 ;
 	
 	ReturnValue = UI_GetBCPCheckBoxValues() ;
 	return ReturnValue;
 }
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








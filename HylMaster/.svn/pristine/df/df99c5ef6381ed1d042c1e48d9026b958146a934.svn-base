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
// Name:  AMIFileXRef.c
//
// Description:	This file has the functions that support FW read.
//
//<AMI_FHDR_END>
//**********************************************************************
#include "Efi.h"
#include "Token.h"
#include "string.h"
#include <Protocol/FirmwareVolume2.h>
#include "AMIFileXRef.h"
#include "AmiStdLibFileXRef.h"

EFI_GUID ZeroGuid = {0};

// New OPEN_MAX for Firmware files.
#define FV_OPEN_MAX OPEN_MAX

//#define NULL_GUID \
//    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

extern EFI_BOOT_SERVICES *gBS;

// Structure to hold the ELINKS generated.
struct _filexref gAMIStdLibFileXRef[] = STDLIB_FILEXREF;

// structure for firmware file descriptors.
typedef struct _FW_FILE_DESCRIPTORS{
	INT16 FVdes;
}FW_FILE_DESCRIPTORS;

// global array to hold firmware file descriptors.
FW_FILE_DESCRIPTORS gFWDescriptors[FV_OPEN_MAX];

BOOLEAN gFWDescriptorsInitialized = FALSE;
//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    	:   IsAFirmwareFile
//
// Description  	:   Checks whether the FILE is present in firmware volume(ELINKS).
//
// Parameter    	:   char *FileName
//
// Return Value  	:  TRUE if firmware file otherwise FALSE.
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsAFirmwareFile(char *FileName)
{
	int Index;
	BOOLEAN IsFwFile = FALSE;
	UINT16 FVCount = (sizeof(gAMIStdLibFileXRef)/sizeof(struct _filexref));
	
	if(FileName == NULL)	// NULL check
		return IsFwFile;

	for (Index = 0; Index < FVCount; Index++)
	{
		if(!AmiStrICaseCmp(gAMIStdLibFileXRef[Index].Filename,FileName))
		{
			return TRUE;
		}
	}
	return IsFwFile;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    	:  GetFreeFD
//
// Description  	:  This function calls standard FindFreeFD() to get the available descriptor.
//
// Parameter    	:  VOID
//
// Return Value  	:  Free fd range 0 - 19
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

int GetFreeFD()
{
	return FindFreeFD(0);
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   IsAValidFWMode
//
// Description  		:   Checks whether the mode is valid or not.
//
// Parameter    	:   char *Mode
//
// Return Value  	:  TRUE if try to open in Read Only(r) Mode FALSE otherwise.
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsAValidFWMode(char *Mode)
{
	if(Mode == NULL)	// NULL check
		return FALSE;
	else	// return TRUE if and only if the opening mode is "r, even "r+" should not be allowed.
		return ((*Mode == 0x72 && (*(++Mode) == '\0')) ? TRUE : FALSE);
}

//---------------------------------------------------------------------------------------
//<AMI_PHDR_START>
//---------------------------------------------------------------------------------------
// Procedure		: 	AmiStrICaseCmp
//
// Description	: 	Compares two strings irrespective of the cases.
//
// Parameters	: 	char *Buffer1,char *Buffer2
//
// Return			: 	ZERO if the strings are same.
//								DIFFERENCE between the characters if the strings are not same.
//								0xFF if either of the buffer is NULL.
//---------------------------------------------------------------------------------------
//<AMI_PHDR_END>
//---------------------------------------------------------------------------------------

UINT8 AmiStrICaseCmp (char *Buffer1,char *Buffer2)
{
	char		FirstStringToUpper , SecondStringToUpper;
	if ((Buffer1 == NULL) || (Buffer2 == NULL) || (strlen(Buffer1) != strlen(Buffer2)))	// NULL check
	{
		return 0xFF;
	}
	FirstStringToUpper  = AmiCharToUppercase (*Buffer1);
	SecondStringToUpper = AmiCharToUppercase (*Buffer2);
	while ((*Buffer2 != '\0') && (FirstStringToUpper == SecondStringToUpper))
	{
		Buffer1++;
		Buffer2++;
		FirstStringToUpper  = AmiCharToUppercase (*Buffer1);
		SecondStringToUpper = AmiCharToUppercase (*Buffer2);
	}
	return FirstStringToUpper - SecondStringToUpper;
}

//---------------------------------------------------------------------------------------
//<AMI_PHDR_START>
//---------------------------------------------------------------------------------------
// Procedure		: AmiCharToUppercase
//
// Description	: Converts a character to UPPERCASE CHARACTER (a -> A).
//
// Parameters	: char Chr
//
// Return			: char
//
//---------------------------------------------------------------------------------------
//<AMI_PHDR_END>
//---------------------------------------------------------------------------------------

char AmiCharToUppercase(char Chr)
{
	return (UINT8) ((Chr >= 'a' && Chr <= 'z') ? Chr - ('a' - 'A') : Chr);
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   InitializeFWDescriptors
//
// Description  		:   Initializes gFWDescriptors with FREE_TO_USE (-1) which denotes available for use.
//
// Parameter    	:   VOID
//
// Return Value  :  EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InitializeFWDescriptors()
{
	UINT8 Des;
	for (Des = 0;Des < FV_OPEN_MAX; Des++)
		gFWDescriptors[Des].FVdes = FREE_TO_USE;
	gFWDescriptorsInitialized = TRUE;
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   FVfopen
//
// Description  		:   Reads a firmware file and updates _p and _r.
//
// Parameter    	:   FILE *fp,char *file
//
// Return Value  :  int
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

int FVfopen(FILE *fp,char *file)
{
	  EFI_STATUS Status = EFI_UNSUPPORTED;
	  EFI_GUID GetGuid = GetGUIDWithFVName(((char *)file));
	  UINT8* FWFileBuffer = NULL;
	  UINTN BufferSize;
	  
	  short int fd = INVALID_FILE_DESC;
	  if(fp == NULL || file == NULL)	// NULL check
		  return fd;
	  if (StdLibCompareGuid(&GetGuid,&ZeroGuid) != 1)
	  {
		  Status = ReadFromFVWithGuid(&GetGuid,&FWFileBuffer,(UINTN*)&BufferSize);
		  if (Status == EFI_SUCCESS)
		  {
			  fd = GetFreeFD();
			  fp->_p = FWFileBuffer;
			  fp->_r = (int)BufferSize;
			  fp->_bf._base = FWFileBuffer;
			  fp->_offset = 0;
			  CopyFDToFWDescriptors(fd);

		  }
	  }
	  return fd;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   fseek_FV
//
// Description  		:   Seek the given file to the given offset.'Whence' must be one of the three SEEK_* macros.
//
// Parameter    	:   FILE *fp, off_t offset, int whence
//
// Return Value  :  int
//
//---------------------------------------------------------------------------------

int
fseek_FV(FILE *fp, off_t offset, int whence)
{
	int FseekStatus = (EOF);
		if(IsAFirmwareDescriptor(fp->_file))  // Checking Firmware volume file Descriptor status.
		{
			off_t Total_Offset;
			Total_Offset = fp->_r +(int)fp->_offset;
			switch(whence)
			{
				case SEEK_SET:  if(offset < 0)     //if offset is negative value We can't able to set the position to Starting
									return (EOF);
								else
								{
									if(offset > Total_Offset )  //if offset value is more than the file size then it will reset to starting of the file
									{
										fp->_p =  fp->_bf._base;
										fp->_r += (int)fp->_offset;
										fp->_offset = 0;
										FseekStatus = 0;
									}
									else                        // It will set the offset value in stream from starting position
									{
										fp->_r += (int)fp->_offset - (int)offset; 
										fp->_offset = (int)offset;
										fp->_p =  fp->_bf._base + fp->_offset;
										FseekStatus = 0;
									}
								}
								break;
				case SEEK_CUR:  if( offset < -fp->_offset) //if offset value is more than fp->_offset then it will return EOF
									return (EOF);
								else if(offset > fp->_r) //if offset value is more than the fp->_r then it will reset to starting of the file
								{
									fp->_p =  fp->_bf._base;
									fp->_r += (int)fp->_offset;
									fp->_offset = 0;
									FseekStatus = 0;
								}
								else  //it will set the offset value from current stream position
								{
									fp->_r -=  (int)offset;
									fp->_offset +=  (int)offset;
									fp->_p =  fp->_bf._base + fp->_offset;
									FseekStatus = 0;
								}
								break;
				case SEEK_END:  if(offset > 0) //if offset value is positive value then it will reset to starting of the file
								{
									fp->_p =  fp->_bf._base;
									fp->_r += (int)fp->_offset;
									fp->_offset = 0;
									FseekStatus = 0;
								}
								else	
								{
									if(offset < -Total_Offset )  //if offset value is more than the file size then it will return EOF
									{
										return (EOF);
									}
									else    // It will set the offset value in stream from EOF
									{
										fp->_offset += (int)fp->_r + (int)offset;
										fp->_r = -(int)offset;
										fp->_p =  fp->_bf._base + fp->_offset;
										FseekStatus = 0;
									}
								}
								break;
			}
		}
		return FseekStatus;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   GetGUIDWithName
//
// Description  		:   Returns EFI_GUID if the SearchFile is present.
//
// Parameter    	:   char *SearchFile - Required file to search.
//
// Return Value  :  EFI_GUID of the SearchFile.
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_GUID GetGUIDWithFVName(char *SearchFile)
{
	UINT16 FVCount = (sizeof(gAMIStdLibFileXRef)/sizeof(struct _filexref));
	UINT16 Index = 0;
	EFI_GUID RetGuid = NULL_GUID;
	if(SearchFile == NULL)	// NULL check
		return RetGuid;
//	EFI_GUID Ret = gAMIStdLibFileXRef[FVCount-1].ResourceGUID;
	for (Index = 0; Index < FVCount; Index++)
	{
		if(!AmiStrICaseCmp(gAMIStdLibFileXRef[Index].Filename,SearchFile))
			return gAMIStdLibFileXRef[Index].ResourceGUID;
	}
	return RetGuid;
}

//---------------------------------------------------------------------------------------
//<AMI_PHDR_START>
//---------------------------------------------------------------------------------------
// Procedure		: 	StdLibCompareGuid
//
// Description	:	compares two EFI GUIDs
//
// Parameters	: 	EFI_GUID *Guid1,EFI_GUID *Guid2
//
// Return			: 	TRUE if GUIDs are same otherwise FALSE
//
//---------------------------------------------------------------------------------------
//<AMI_PHDR_END>
//---------------------------------------------------------------------------------------

BOOLEAN StdLibCompareGuid (EFI_GUID *Guid1, EFI_GUID *Guid2)
{
	return (BOOLEAN)(0==memcmp((UINT8*)Guid1,(UINT8*)Guid2,sizeof(EFI_GUID)));
}

//<AMI_PHDR_START>
//-------------------------------------------------------------------------------------------------------------
// Procedure		:	ReadFromFVWithGuid
//
// Description	:	Return the graphics image file named FileNameGuid into Image
//								and return it's size in ImageSize. All Firmware Volumes (FV)
//								in the system are searched for the file name.
//
// Input				:	FileNameGuid  	- 	File Name of graphics file in the FV(s).
//								Image					- 	Pointer to pointer to return graphics image.
//																	If NULL, a buffer will be allocated.
//								ImageSize     		- 	Size of the graphics Image in bytes. Zero if no image found.
//
// Output			:	EFI_SUCCESS							-	Image and ImageSize are valid.
//								EFI_BUFFER_TOO_SMALL 	- 	Image not big enough. ImageSize has required size
//								EFI_NOT_FOUND        			- 	FileNameGuid not found
//
//-------------------------------------------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
ReadFromFVWithGuid ( IN EFI_GUID *FileNameGuid, OUT VOID **Image, OUT UINTN *ImageSize )
{
	EFI_STATUS			Status;
	UINTN                  	FvProtocolCount;
	EFI_HANDLE        *FvHandles;
	UINTN                  Index;
	UINT32                 AuthenticationStatus;

#if (PI_SPECIFICATION_VERSION < 0x00010000)
	EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
#else
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
#endif

	Status = gBS->LocateHandleBuffer (
			ByProtocol,
#if (PI_SPECIFICATION_VERSION < 0x00010000)
			&gEfiFirmwareVolumeProtocolGuid,
#else
            &gEfiFirmwareVolume2ProtocolGuid,
#endif
            NULL,
            &FvProtocolCount,
            &FvHandles);
	
	if (EFI_ERROR (Status)) 
		return EFI_NOT_FOUND;

	for (Index = 0; Index < FvProtocolCount; Index++)
	{
		Status = gBS->HandleProtocol (
                   FvHandles[Index],
#if (PI_SPECIFICATION_VERSION < 0x00010000)
                   &gEfiFirmwareVolumeProtocolGuid,
#else
                   &gEfiFirmwareVolume2ProtocolGuid,
#endif
                   (VOID **) &Fv);

		// Assuming Image and ImageSize are correct on input.
		Status = Fv->ReadSection (
				Fv,
				FileNameGuid,
				EFI_SECTION_RAW,
				0,
				Image,
				ImageSize,
				&AuthenticationStatus);

		if (!EFI_ERROR (Status))
		{
			gBS->FreePool(FvHandles);
			return EFI_SUCCESS;
		}
		else if (Status == EFI_BUFFER_TOO_SMALL) // ImageSize updated to needed size so return
		{
			gBS->FreePool(FvHandles);
			return EFI_BUFFER_TOO_SMALL;
		}
	}

	gBS->FreePool(FvHandles);
	return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   CopyFDToFWDescriptors
//
// Description  		:   Check which descriptor is free and assign it to FILEDes
//
// Parameter    	:   int FILEDes
//
// Return Value  :  EFI_SUCCESS on successful copy otherwise ERROR.
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS CopyFDToFWDescriptors(int FILEDes)
{
	EFI_STATUS Status = EFI_VOLUME_FULL;
	int Index;
	if(FILEDes < 0)
		return Status;
	for (Index = 0; Index < FV_OPEN_MAX; Index++)
	{
		if (gFWDescriptors[Index].FVdes == FREE_TO_USE)
		{
			gFWDescriptors[Index].FVdes = (INT16)FILEDes;
			Status = EFI_SUCCESS;
			break;
		}
	}
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   IsAFirmwareDescriptor
//
// Description  		:   Checks whether the FILE descriptor (fp->_file) represents a firmware file.
//
// Parameter    	:   int Descriptor
//
// Return Value  :  TRUE if it represents a firmware file otherwise FALSE.
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsAFirmwareDescriptor(int FILEDes)
{
	int Index;
	BOOLEAN IsFwFile = FALSE;
	for (Index = 0; Index < FV_OPEN_MAX; Index++)
	{
		if (gFWDescriptors[Index].FVdes == FILEDes)
			return TRUE;
	}
	return IsFwFile;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   ReleaseFWDescriptor
//
// Description  		:   Releases the fp->_file from gFWDescriptors. This will be called from fclose().
//
// Parameter    	:   FILE *fp
//
// Return Value  :  EFI_SUCCESS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS ReleaseFWDescriptor(int FILEDes)
{
	UINT8 Index;
	struct __filedes *Des;
	if(FILEDes < 0)
		return EFI_INVALID_PARAMETER;
	for (Index = 0;Index < FV_OPEN_MAX ; Index++)
	{
		if(gFWDescriptors[Index].FVdes == FILEDes)
		{
			gFWDescriptors[Index].FVdes = FREE_TO_USE;
			Des = &gMD->fdarray[FILEDes];
			// Here setting to ZERO because if a fp is closed, 
			// then fp->_file should be available for next fopen() call.
			if(Des)
				Des->f_iflags = 0;
			break;
		}
	}	
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    		:   BOOLEAN IsShellProtocolAvailable()
//
// Description  		:   To check shell protocol installed or not.
//
// Parameter    	:   NONE
//
// Return Value  :  BOOLEAN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN IsShellProtocolAvailable() 
{
#if MINI_LIB_SUPPORT == 0
{
	extern EFI_SHELL_PROTOCOL  *gEfiShellProtocol;
	return (gEfiShellProtocol !=NULL);
}
#else
return FALSE; // if MINI_LIB_SUPPORT is ON means no shell support and no file system returning false.
#endif
}

//**********************************************************************
//**********************************************************************
//**                                                                  						   **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.       **
//**                                                                  						   **
//**                       All Rights Reserved.                       				   **
//**                                                                  						   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093   **
//**                                                                  						   **
//**                       Phone: (770)-246-8600                      			   **
//**                                                                  						   **
//**********************************************************************
//**********************************************************************

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
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
// $Header:  $
//
// $Revision:  $
//
// $Date:  $
//**********************************************************************
// Revision History
// ----------------
// $Log:  $
// 
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

#include <AmiDxeLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include "Library/MemoryAllocationLib.h"
#include <Library/BaseMemoryLib.h>
#include "BootOptions.h"
#include "wchar.h"
#include "string.h"
#include "Token.h"

#define MAX_LENGTH 32

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// Firmware file name
  ///
  EFI_GUID                        FvFileName;
} AMITSE_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;


typedef struct _BOOT_OPTION_TSE
{
    UINT32  Active;
    UINT16  PathLength;
    CHAR16  Name[1];
}
BOOT_OPTION_TSE;

// {6c09fb39-a03e-444b-88de-598dabeda6b8} 
static EFI_GUID gFFSguid = { 0x1DE64B8E, 0x138B, 0x4258, 0xB7, 0xDD, 0xF2, 0xD8, 0xEC, 0x14, 0x2A, 0x9E};
//static EFI_GUID gFFSguid = { 0X6C09FB39, 0XA03E, 0X444B, 0X88, 0XDE, 0X59, 0X8D, 0XAB, 0XED, 0XA6, 0XB8};

/**
  According to a file guild, check a Fv file device path is valid. If it is invalid,
  try to return the valid device path.
  FV address maybe changes for memory layout adjust from time to time, use this function
  could promise the Fv file device path is right.

  @param  DevicePath             on input, the Fv file device path need to check on
                                 output, the updated valid Fv file device path
  @param  FileGuid               the Fv file guild

  @retval EFI_INVALID_PARAMETER  the input DevicePath or FileGuid is invalid
                                 parameter
  @retval EFI_UNSUPPORTED        the input DevicePath does not contain Fv file
                                 guild at all
  @retval EFI_ALREADY_STARTED    the input DevicePath has pointed to Fv file, it is
                                 valid
  @retval EFI_SUCCESS            has successfully updated the invalid DevicePath,
                                 and return the updated device path in DevicePath

**/
EFI_STATUS  BdsLibUpdateFvFileDevicePath (
  IN  OUT EFI_DEVICE_PATH_PROTOCOL      ** DevicePath,
  IN  EFI_GUID                          *FileGuid   )
{
    EFI_STATUS                    Status;
    UINTN                         Index;
    UINTN                         FvHandleCount;
    EFI_HANDLE                    *FvHandleBuffer;
    EFI_FV_FILETYPE               Type;
    UINTN                         Size;
    EFI_FV_FILE_ATTRIBUTES        Attributes;
    UINT32                        AuthenticationStatus;
    BOOLEAN                       FindFvFile = FALSE;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
    MEDIA_FW_VOL_FILEPATH_DEVICE_PATH FvFileNode;
    EFI_HANDLE                    FoundFvHandle;
    EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
    
    if (FileGuid == NULL) 
    {
      return EFI_INVALID_PARAMETER;
    }
    
    //
    // Second, if fail to find, try to enumerate all FV
    //
    if (!FindFvFile) 
    {
      FvHandleBuffer = NULL;
      pBS->LocateHandleBuffer (
            ByProtocol,
            &gEfiFirmwareVolume2ProtocolGuid,
            NULL,
            &FvHandleCount,
            &FvHandleBuffer
            );
      for (Index = 0; Index < FvHandleCount; Index++)
      {
          pBS->HandleProtocol (
              FvHandleBuffer[Index],
              &gEfiFirmwareVolume2ProtocolGuid,
              (VOID **) &Fv
              );
    
          Status = Fv->ReadFile (
                      Fv,
                      FileGuid,
                      NULL,
                      &Size,
                      &Type,
                      &Attributes,
                      &AuthenticationStatus
                      );
        if (EFI_ERROR (Status))
        {
          //
          // Skip if input Fv file not in the FV
          //
          continue;
        }
        FindFvFile = TRUE;
        FoundFvHandle = FvHandleBuffer[Index];
        break;
      }
    
      if (FvHandleBuffer != NULL)
      {
        FreePool (FvHandleBuffer);
      }
    }
    
    if (FindFvFile)
    {
      //
      // Build the shell device path
      //
        static struct
        {
                    VENDOR_DEVICE_PATH Media;
                    EFI_DEVICE_PATH_PROTOCOL End;
        } 
        FwLoadFileDp = 
        {
            {
                {
                    MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
                    sizeof(VENDOR_DEVICE_PATH)
                },
                AMI_MEDIA_DEVICE_PATH_GUID // include BootOptions.h to include definition of this macro
             },
             {
                 END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
                 sizeof(EFI_DEVICE_PATH_PROTOCOL)
             }
        };
        
        if (FileGuid == NULL) 
        {
            return EFI_INVALID_PARAMETER;
        }

     
        EfiInitializeFwVolDevicepathNode (&FvFileNode, FileGuid);
        NewDevicePath = AppendDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *)&FwLoadFileDp, (EFI_DEVICE_PATH_PROTOCOL *) &FvFileNode);
        ASSERT (NewDevicePath != NULL);
        *DevicePath = NewDevicePath;
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;
}

/**
    Function to return the data for the Corresponding NVRAM Variable

    @param CHAR16 *VarName
    @param EFI_GUID *Guid
    @param void **NVData
    @param UINTN *Size
    @param UINT32 *Attrib 
    @retval EFI_STATUS
**/
EFI_STATUS AMI_LIB_GetNVRamVariable (CHAR16 *VarName, EFI_GUID *Guid,void **NVData,UINTN *Size,UINT32 *Attrib)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT32 DataAttribute=0;
    UINTN VariableSize=0;

    VariableSize = *Size; 
    *NVData = AllocateZeroPool (VariableSize);
    if (NULL == *NVData)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    Status = pRS->GetVariable (VarName, Guid, &DataAttribute, &VariableSize, *NVData);
    if (EFI_BUFFER_TOO_SMALL == Status)
    {
        FreePool(*NVData);
        *NVData = AllocateZeroPool  (VariableSize);
        if (NULL == *NVData)
        {
            return EFI_OUT_OF_RESOURCES;
        }
        Status = pRS->GetVariable (VarName, Guid, &DataAttribute, &VariableSize, *NVData);
    }
    if(EFI_SUCCESS == Status)
    {
        if (Attrib)
        {
            *Attrib = DataAttribute;
        }
        *Size = VariableSize ;
    }
    return Status;
}

/**
    Function to Set the data for the Corresponding NVRAM Variable

    @param CHAR16 *VarName
    @param EFI_GUID *Guid
    @param void **NVData
    @param UINTN *Size
    @param UINT32 *Attrib 
    @retval EFI_STATUS
**/
EFI_STATUS AMI_LIB_SetNVRamVariable (CHAR16 *VarName, EFI_GUID *Guid,void *NVData,UINTN Size,UINT32 Attrib)
{
    EFI_STATUS  Status = EFI_SUCCESS;

    Status = pRS->SetVariable (VarName, Guid, Attrib, Size, NVData);

    return Status;
}

/**
    Function to find whether AMIFWUpdate boot option is already available or not

    @retval EFI_STATUS - EFI_SUCCESS : Boot Option available
                       - EFI_NOT_FOUND: Boot option not available
**/
EFI_STATUS FindFwUpdateBootOption()
{
    UINT8 *BootOrder = (UINT8 *)NULL;
    UINT64 Size = 0xFFFE;
    UINT16 Index = 0;
    UINT8 *BootXXXX = (UINT8 *)NULL;
    UINT64 BootSize = 0;
    UINT16 BootStr[MAX_LENGTH] = {0};
    UINT32 Attributes = 0;
    BOOT_OPTION_TSE *BootOptions;
    EFI_STATUS Status = EFI_NOT_FOUND;

    // For each Boot Option retrieve the BootXXXX varaible
    for ( Index = 0; Index <= Size; Index ++ )
    {
        // UEFI spec defines Boot Order Index as Upper case eg. "Boot000A".
        // But Aptio uses it in lower case. Change L"Boot%02x%02x" to L"Boot%02X%02X"
        // once Aptio changes it.
        BootSize = 0;
        Attributes = 0;
        swprintf( BootStr, MAX_LENGTH, L"Boot%04X", Index);
        //DEBUG((DEBUG_ERROR, "\n BootStr: %s", BootStr));
        Status = pRS->GetVariable (BootStr, &gEfiGlobalVariableGuid, NULL, &BootSize, (VOID *)BootXXXX);
        //DEBUG((DEBUG_ERROR, "\n GetVariable %s: %r", BootStr, Status));
        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            BootXXXX = AllocateZeroPool(BootSize+1);
            Status = pRS->GetVariable (BootStr, &gEfiGlobalVariableGuid, NULL, &BootSize, (VOID *)BootXXXX);
        }
        //DEBUG((DEBUG_ERROR, "\n GetVariable %s: %r", BootStr, Status));
        
        if(!EFI_ERROR(Status) && BootXXXX)
        {
            BootOptions = (BOOT_OPTION_TSE *)AllocateZeroPool(BootSize+1);
            DEBUG((DEBUG_ERROR, "\n BootStr: %s %d %d memcpy", BootStr, BootSize, sizeof( BOOT_OPTION * )));
            memcpy( BootOptions, BootXXXX, BootSize);
            DEBUG((DEBUG_ERROR, "\n memcpy done"));
            FreePool(BootXXXX);
            BootXXXX = (UINT8 *)NULL;
        }
        
        if ( BootOptions && BootSize != 0 )
        {
            DEBUG((DEBUG_ERROR, "\n BootOptions->Name: %a, %s", BootOptions->Name, BootOptions->Name));
            // If BootOption Name is AMIFWUpdate then return Success
            if(!(StrCmp(BootOptions->Name, L"AMIFWUpdate")))
            {
                Status = EFI_SUCCESS;
                FreePool(BootOptions);
                BootOptions = (BOOT_OPTION_TSE *)NULL;
                break;
            }
            FreePool(BootOptions);
            BootOptions = (BOOT_OPTION_TSE *)NULL;
        }
    }
    return Status;
}

/**
    Function to add FirmUpdate as a boot option

    @retval EFI_STATUS
**/
EFI_STATUS AddFwUpdBootOption()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_DEVICE_PATH_PROTOCOL *devicePath = (EFI_DEVICE_PATH_PROTOCOL *)NULL;
    UINT16          VarIndex = 0;
    UINTN           ImagePathLen = 2,size=0;
    UINTN           DataLength=0, VarNameSize=80;
    BOOT_OPTION_TSE *FwUpdateBoot, *NewFwBootOption;
    UINT32          VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    UINT32          BootVarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    CHAR16          *VarName = NULL;
    UINT8           Data;
    EFI_GUID        BootVarGuid = { 0x86dc248f, 0xe598, 0x4acd, { 0x94, 0xe3, 0x7d, 0x1, 0x6c, 0x39, 0xbb, 0x44 }};
    //CHAR16 *Options = L"ami.rom /p /b /n";
    _fltused              = 1;
    
    Status = pRS->GetVariable (L"FwUpdateBootVarSet", &BootVarGuid, NULL, &DataLength, &Data);
    if( EFI_NOT_FOUND != Status )
    {
        return Status;
    }

	//Verify whether the AMIFWUpdate Boot option is already available
    Status = FindFwUpdateBootOption();
    if(!EFI_ERROR(Status))
    {
        Data = 123;
        DataLength = sizeof(UINT8);
        Status= pRS->SetVariable (L"FwUpdateBootVarSet", &BootVarGuid, VarAttrib, DataLength, &Data);
        return Status;
    }
    
    BdsLibUpdateFvFileDevicePath(&devicePath, &gFFSguid);
    
    if( devicePath != NULL )
    { 
        VarNameSize = ((Wcslen(L"BOOTXXXX")+1)*2);
        Status = pBS->AllocatePool(EfiBootServicesCode,VarNameSize, (VOID **)&VarName);
        if (EFI_ERROR (Status))
        {
            return Status;
        }

        do
        {
        	DataLength = 0;
        	FwUpdateBoot = NULL;
            Swprintf(VarName, L"Boot%04X",VarIndex);
                
            Status = AMI_LIB_GetNVRamVariable((CHAR16*)VarName, &gEfiGlobalVariableGuid, (void**)&FwUpdateBoot, &DataLength, &BootVarAttrib);
            if( EFI_NOT_FOUND == Status )
            {
                Data = 123;
                DataLength = sizeof(UINT8);
                Status= pRS->SetVariable (L"FwUpdateBootVarSet", &BootVarGuid, VarAttrib, DataLength, &Data);
                break;
            }
            else
                VarIndex++;
        }while (VarIndex <= 0xFFFE);
               
        ImagePathLen = DPLength(devicePath);

        size = sizeof(BOOT_OPTION_TSE) + sizeof(CHAR16)*Wcslen(L"AMIFWUpdate") + ImagePathLen;
        
        //size = size + StrLen(Options)*2;
        NewFwBootOption = (BOOT_OPTION_TSE *)AllocateZeroPool(size);
        //size = size - StrLen(Options)*2;
        if(NewFwBootOption != NULL)
		{
#if HIDE_FWUPDATE_BOOT_OPTION
            NewFwBootOption->Active = 9; //Active and Hidden
#else
			NewFwBootOption->Active = 1; //Active
#endif
			NewFwBootOption->PathLength = (UINT16)ImagePathLen;
			
			StrCpy( NewFwBootOption->Name, L"AMIFWUpdate");
			MemCpy( (UINT8 *)NewFwBootOption + (size - ImagePathLen), devicePath, ImagePathLen );
			
			//MemCpy( (UINT8 *)NewFwBootOption + (size), Options, StrLen(Options)*2); //Add Options
			//size = size + StrLen(Options)*2;
			
			Status = AMI_LIB_SetNVRamVariable((CHAR16*)VarName,&gEfiGlobalVariableGuid,(void*)NewFwBootOption,size, (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE));
			if ( EFI_ERROR(Status) )
				pRS->SetVariable (L"FwUpdateBootVarSet", &BootVarGuid, VarAttrib, 0, &Data);

			if(NewFwBootOption != NULL)
				FreePool(NewFwBootOption);
		}

        pBS->FreePool(VarName);
    }
    
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

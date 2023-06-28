
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, 	             **//
//**					Georgia 30093  					         **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log:  $
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiPldmDriver.c
//
// Description:	Exports and Imports the Pldm Tables from Setup from file system and vice versa.
//Provides AMISCE driver Inerfaces for Export,Import,SaveUserDefaults and Restore User Defaults.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "AmiLib.h"
#include "Efi.h"
#include "PldmApi.h"
#include "AmiPldmInterfaces.h"
#include "Protocol/LoadedImage.h"
#include "Token.h"
#include "AmiDxeLib.h"
#include "Library/BaseMemoryLib.h"
#include "AmiPldmInterfaceDriver.h"

extern EFI_BOOT_SERVICES *pBS;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES    *gRT;

AMIPLDM_INTERFACE_PROTOCOL gAmiPldmInterfaceProtocol;
EFI_GUID AmiPldmInterfaceProtocolGuid = AMIPLDM_BMC_DRIVER_PROTOCOL_GUID;

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: 	AmiPldmDriverEntryPoint
//
// Description :  Entry Point of the application
//
// Parameter : 	EFI_HANDLE ImageHandle , EFI_SYSTEM_TABLE *SystemTable
//
// Return Value: EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiPldmInterfaceDriverEntryPoint(
	EFI_HANDLE ImageHandle,
	EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_GUID LoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImageProtocol = NULL;
	UINTN Index = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_STATUS RetStatus = EFI_SUCCESS;
	
	InitAmiLib(ImageHandle,SystemTable);

	RetStatus = gBS->HandleProtocol( ImageHandle, &LoadedImageProtocolGuid, (void **)&LoadedImageProtocol );

	if ( RetStatus == EFI_SUCCESS )
	{
		LoadedImageProtocol->Unload = PldmInterfaceUnload;

		gAmiPldmInterfaceProtocol.Revision = PLDM_INTERFACE_REVISION;
		//gAmiPldmInterfaceProtocol.GetHiiData = PldmGetHiiData;
		gAmiPldmInterfaceProtocol.SetPldmMode = PldmSetPldmMode;
		gAmiPldmInterfaceProtocol.CreatePldmTables = PldmCreatePldmTables;
		gAmiPldmInterfaceProtocol.CreatePldmByHiiHandle = PldmCreatePldmByHiiHandle;
		gAmiPldmInterfaceProtocol.ImportPldmTables = PldmImportPldmTables;
		gAmiPldmInterfaceProtocol.ImportPldmByHiiHandle = PldmImportPldmByHiiHandle;
		gAmiPldmInterfaceProtocol.SaveConfigurationInBmc = PldmSaveConfigurationInBmc;
		gAmiPldmInterfaceProtocol.RestoreConfigurationFromBmc = PldmRestoreConfigurationFromBmc;
		gAmiPldmInterfaceProtocol.SetTagsInBmc = PldmSetTagsInBmc;
		gAmiPldmInterfaceProtocol.GetTagsFromBmc = PldmGetTagsFromBmc;
		gAmiPldmInterfaceProtocol.SavePldmByCustomMethod = SavePldmByCustomMethod;
		gAmiPldmInterfaceProtocol.RestorePldmByCustomMethod = RestorePldmByCustomMethod;

		RetStatus = gBS->InstallMultipleProtocolInterfaces(&ImageHandle, &AmiPldmInterfaceProtocolGuid, (void *)&gAmiPldmInterfaceProtocol, NULL);

		if ( RetStatus != EFI_SUCCESS )
		{
			DEBUG((DEBUG_ERROR, "\nAmiPldmInterfaceProtocol Install Failed.\n" ));
		}
	}
	else
	{
		DEBUG((DEBUG_ERROR, "\nHandleProtocol Failed.\n" ));
	}

	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: PldmInterfaceUnload
//
// Description	: Unloads Driver from Memory
//
// Parameter	: IN EFI_HANDLE ImageHandle
//
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PldmInterfaceUnload ( EFI_HANDLE ImageHandle )
{
	EFI_STATUS RetVal = (EFI_STATUS) - 1;
	UINTN Count = 0;
	UINTN NoOfHandles = 0;
	EFI_HANDLE *HandleBuffer = NULL;

	RetVal = gST->BootServices->LocateHandleBuffer( AllHandles, NULL, NULL, &NoOfHandles, (EFI_HANDLE**)&HandleBuffer );

	if ( RetVal == EFI_SUCCESS )
	{
		for ( Count = 0; Count < NoOfHandles; Count++ )
		{
			RetVal = gST->BootServices->DisconnectController( HandleBuffer[Count], ImageHandle, NULL );

			if (RetVal != EFI_SUCCESS)
			{
				//TRACE((//TRACE_ALWAYS, "\n\rDisconnectController Failed.\n\r"));
			}
		}

		RetVal = gST->BootServices->FreePool(HandleBuffer);

		if (RetVal != EFI_SUCCESS)
		{
			//TRACE((//TRACE_ALWAYS, "\n\rMemory Free Failed.\n\r"));
		}
	}
	else
	{
		DEBUG((DEBUG_ERROR, "\n\rLocateHandleBuffer Failed .\n\r"));
	}

	RetVal = gBS->UninstallMultipleProtocolInterfaces( ImageHandle, &AmiPldmInterfaceProtocolGuid, (void *)&gAmiPldmInterfaceProtocol, NULL);

	if (RetVal != EFI_SUCCESS)
	{
		DEBUG((DEBUG_ERROR, "\n\rUninstallProtocolInterface Failed.\n\r" ));
	}

	return EFI_SUCCESS;
}


//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, 	             **//
//**					Georgia 30093  					         **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//

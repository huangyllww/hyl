//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  			**
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

/** @file 
PldmImport.c

Responsible for reading Pldm table and import it

*/

//************************************************************************
//					Include Section
//************************************************************************


#include "AmiDxeLib.h"
#include "PldmSceStruct.h"
#include "PldmPreserve.h"
#include "Pi/PiBootMode.h"
#include "Framework/Hob.h"
#include "Token.h"
#include "Library/PcdLib.h"

AMI_BCP_SERVICE_FLASH_PROTOCOL AmiBcpServiceProtocol={CreatePldmForConfigPreserving, UpdateCapsuleToPush, Preserve_flash_Update,Get_FV_Data};
/**
   	Function to find capsule hob is present or not
*/
#if BCP_DEBUG
void FindCapsuleHob ()
{
	EFI_GUID 	gHobListGuid = HOB_LIST_GUID;
	UINT64      CapsuleLength = 0;
	EFI_HOB_CAPSULE_VOLUME *gHobList = NULL;
	EFI_CAPSULE_HEADER     *FWCapsuleVolume = NULL;
	
	gHobList = GetEfiConfigurationTable(gST, &gHobListGuid);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gHobList is %x\n", gHobList));
	if (gHobList == NULL)
	{
		return;
	}
	while (!EFI_ERROR(FindNextHobByType(EFI_HOB_TYPE_UEFI_CAPSULE, &gHobList)))
	{
		CapsuleLength  = gHobList->Length;
		FWCapsuleVolume = (EFI_CAPSULE_HEADER*) gHobList->BaseAddress;
		
		DEBUG((DEBUG_ERROR, "BiosConfigPreserve: gHobList %g\n", &FWCapsuleVolume->CapsuleGuid));
		DEBUG((DEBUG_ERROR, "BiosConfigPreserve: CapsuleLength %x\n", CapsuleLength));
	}
}
#endif

/**
   	Function is the module entry point to preserve bios configuration

	@param EFI_HANDLE
  	@param EFI_SYSTEM_TABLE *
  	
	@retval	EFI_STATUS
*/ 
EFI_STATUS PldmPreserveEntry(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS  Status = EFI_SUCCESS ;
    EFI_HANDLE  BcpHandle=NULL;
    EFI_GUID    AmiBcpCapsuleGuid = AMI_BCP_FLASH_PROTOCOL_GUID ;
	EFI_BOOT_MODE BootMode;
    UINTN       Size = 1;
    UINT8       NvramFlashFlag = 0;
    EFI_GUID    NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
    UINT32      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
    BCP_QUERYVARIABLE_INFO           VarData  = {0};
    UINT64        MaxVarStorageSize = 0;
    UINT64        RemainingVarStorageSize = 0;
    UINT64        MaxVarSize = 0;
    
	InitAmiLib(ImageHandle, SystemTable);

	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In PldmPreserveEntry\n"));

#if BCP_DEBUG
	{
		Status = pRS->GetVariable (L"NvramFlashed",
					&NvramFlashGuid,
					NULL,
					&Size, 
					&NvramFlashFlag);
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for NvramFlashed variable is  %r Size is %x\n", Status, Size));
		
		FindCapsuleHob ();
	}
#endif
	
#if BCP_TOKEN_DEPENDENCY_ERRORS
#if ALWAYS_PUBLISH_HII_RESOURCES == 0
	#error ALWAYS_PUBLISH_HII_RESOURCES token should be enabled
#endif
#if RT_ACCESS_SUPPORT_IN_HPKTOOL  == 0
	#error RT_ACCESS_SUPPORT_IN_HPKTOOL token should be enabled
#endif
#endif	
	
    if(FALSE == PcdGetBool(PcdUseNewImageOnFlashUpdate))
    {
        Status = pRS->QueryVariableInfo ( Attributes, &MaxVarStorageSize, &RemainingVarStorageSize, &MaxVarSize );
        if (!EFI_ERROR (Status)) 
        {
            VarData.MaximumVariableSize = MaxVarSize;
            VarData.MaximumVariableStorageSize = MaxVarStorageSize;
            VarData.RemainingVariableStorageSize = RemainingVarStorageSize - VAR_STORAGE_RESERVE_SIZE;
    
            Status = pRS->SetVariable (L"BcpQueryVariableInfo", &gBcpQueryVariableInfoGuid, Attributes, sizeof(BCP_QUERYVARIABLE_INFO), &VarData);
        }
    }
    
	BootMode = GetBootModeHob ();
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: BootMode is %x\n", BootMode));
	
	Status = gBS->InstallProtocolInterface(&BcpHandle, &AmiBcpCapsuleGuid, EFI_NATIVE_INTERFACE, &AmiBcpServiceProtocol);	// Installing protocol for BIOS Configuration preserve
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AmiBcpServiceProtocol is %x\n", Status));
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AmiBcpServiceProtocol Address is %x\n", AmiBcpServiceProtocol));
	
	Status = InstallPreserveInterface(ImageHandle) ;
    if ((BOOT_IN_RECOVERY_MODE == BootMode) || (BOOT_ON_FLASH_UPDATE == BootMode))  //Pldm import for recovery and capsule done in reflash module
    {
		//Preserving should haven't happened in last flash if PcdUseNewImageOnFlashUpdate is FALSE in recovery or capsule update
		//This will help in continuous flash entering into recovery mode
        Status = pRS->GetVariable (L"NvramFlashed",
                    &NvramFlashGuid,
                    NULL,
                    &Size, 
                    &NvramFlashFlag);
        if (EFI_ERROR (Status))
        {
            ImportUpdateModePreservingToRuntime ();
        }
        return EFI_UNSUPPORTED;
    }
	
	RuntimePldmImport ();			//Import for runtime update
	return EFI_SUCCESS;
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

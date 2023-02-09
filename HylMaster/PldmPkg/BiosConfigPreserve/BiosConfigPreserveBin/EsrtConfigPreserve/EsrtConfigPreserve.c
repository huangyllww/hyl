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
EsrtConfigPreserve.c

Responsible for preserving setup configuration in ESRT flash update

*/

//************************************************************************
//					Include Section
//************************************************************************


#include "AmiDxeLib.h"
#include "PldmSceStruct.h"
#include "Token.h"
#include "Capsule.h"
#include <Library/BaseMemoryLib.h>
#include "PldmApi.h"
#include <Library/MemoryAllocationLib.h>
#include "Protocol/AmiDigitalSignature.h"
#include "PldmPreserve.h"

extern void SetPreserveMode(UINTN Mode) ;

EFI_UPDATE_CAPSULE gUpdateCapsule;
EFI_GUID W8FwUpdateImageCapsuleGuid = W8_FW_UPDATE_IMAGE_CAPSULE_GUID;

EFI_STATUS EsrtUpdateCapsule(
  IN EFI_CAPSULE_HEADER     **CapsuleHeaderArray,
  IN UINTN                  CapsuleCount,
  IN EFI_PHYSICAL_ADDRESS   ScatterGatherList   OPTIONAL
  );

/**
   	Function is the module entry point to preserve bios configuration in ESRT flash update

	@param EFI_HANDLE
  	@param EFI_SYSTEM_TABLE *
  	
	@retval	EFI_STATUS
*/
EFI_STATUS EsrtConfigPreserveEntry (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
	InitAmiLib(ImageHandle, SystemTable);
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In EsrtConfigPreserveEntry\n"));
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pRS->UpdateCapsule is %x\n", pRS->UpdateCapsule));
	
	gUpdateCapsule = pRS->UpdateCapsule;
	pRS->UpdateCapsule = EsrtUpdateCapsule;
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gUpdateCapsule is %x pRS->UpdateCapsule is %x EsrtUpdateCapsule is %x\n", gUpdateCapsule, pRS->UpdateCapsule, EsrtUpdateCapsule));
	return EFI_SUCCESS;
}

/**
   	Function to call the original UpdateCapsule interface with ESRT and Pldm table capsules

	@param EFI_CAPSULE_HEADER **
  	@param UINTN
	@param EFI_PHYSICAL_ADDRESS
*/
EFI_STATUS EsrtUpdateCapsule(
  IN EFI_CAPSULE_HEADER     **CapsuleHeaderArray,
  IN UINTN                  CapsuleCount,
  IN EFI_PHYSICAL_ADDRESS   ScatterGatherList   OPTIONAL
  )
{
	UINTN		ArrayNumber;
	EFI_STATUS  Status;
	UINT32 		Version = 0x1;
	UINT32 		Signature = 'MDLP';
	UINT8 		*HIIDBData = {0};
	UINT32 		CRC32 = 0;
	UINTN 		DBSize = 0;
	BOOLEAN 	SaveVariable = 0;
	UINT8		*StringTable = NULL;
	UINT32 		StringSize = 0;
	UINT8 		*AttributeTable = NULL;
	UINT32 		AttributeSize = 0;
	UINT8 		*ValueTable = NULL;
	UINT32 		ValueSize = 0;
	UINT8 		*MapStringTable = NULL;
	UINT32 		MapStringSize = 0;
	UINT32 		PldmBufferSize = 0; 
	UINTN       Size = 0;
	EFI_GUID    PldmCapsuleGuid = EFI_PLDM_CAPSULE_GUID;
	EFI_GUID	NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
	UINT8		NvramFlashFlag = 1;
	UINT8		*HashDB = NULL;
	UINT32  	HashDBSize = 32;
	EFI_GUID 	PldmHashGuid = EFI_PLDM_HASH_GUID;
	EFI_CAPSULE_HEADER   	*CapsuleHeader;
	SCE_PLDM_DATA_HEADER 	*PldmBuffer = NULL;
	EFI_CAPSULE_BLOCK_DESCRIPTOR  *PldmCapsuleHdr = NULL;
	AMI_DIGITAL_SIGNATURE_PROTOCOL *AmiSig = NULL;
	
	
//	If capsule has reset flag set then pldm capsule will not be pushed. So moving at last
//	Status = gUpdateCapsule (CapsuleHeaderArray, CapsuleCount, ScatterGatherList);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In EsrtUpdateCapsule\n"));
	
//	if (!EFI_ERROR (Status))
//	{
	for (ArrayNumber = 0; ArrayNumber < CapsuleCount; ArrayNumber++)
	{
		CapsuleHeader = CapsuleHeaderArray [ArrayNumber];
	
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: ArrayNumber is %x CapsuleCount is %x\n", ArrayNumber, CapsuleCount));
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: CapsuleHeader->CapsuleGuid is %g\n", CapsuleHeader->CapsuleGuid));
		
		if (CompareGuid (&CapsuleHeader->CapsuleGuid, &W8FwUpdateImageCapsuleGuid))
		{
			Status = AMI_RetrieveHIIData (&HIIDBData, &DBSize, &CRC32);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for AMI_RetrieveHIIData is %r\n", Status));
			if (EFI_ERROR (Status))
			{
				break;
			}
			
#if PRESERVE_WITH_BCPALL_IN_ESRT
            SetPreserveMode(BCP_ALL) ;
            DEBUG((DEBUG_ERROR, "\n BiosConfigPreserve: ESRT BCP_ALL"));
#else
            SetPreserveMode(BCP_ONLY_DEFAULTS) ;
            DEBUG((DEBUG_ERROR, "\n BiosConfigPreserve: ESRT BCP_ONLY_DEFAULTS"));
#endif
            
			Status = CreatePldmTables (HIIDBData, (UINT32)DBSize, &StringTable, &StringSize, &AttributeTable, &AttributeSize, &ValueTable, &ValueSize, &MapStringTable, &MapStringSize, SaveVariable);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for CreatePldmTables is %r\n", Status));
			if (EFI_ERROR (Status))
			{
				break;
			}
			PldmBufferSize = sizeof (SCE_PLDM_DATA_HEADER) + StringSize + AttributeSize + ValueSize + MapStringSize;
			Status = pBS->AllocatePool (EfiReservedMemoryType, PldmBufferSize, (void**)&PldmBuffer);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for allocate is %r PldmBufferSize is %x PldmBuffer is %x\n", Status, PldmBufferSize, PldmBuffer));
			if ((EFI_ERROR (Status)) || (NULL == PldmBuffer))
			{
				break;
			}
			MemSet((void*)PldmBuffer, PldmBufferSize, 0 );
			PldmBuffer->Version = Version;
			PldmBuffer->Signature = Signature;
			PldmBuffer->StringOffset = sizeof (SCE_PLDM_DATA_HEADER);
			PldmBuffer->StringSize = StringSize;
			PldmBuffer->AttribOffset = PldmBuffer->StringOffset + PldmBuffer->StringSize ;
			PldmBuffer->AttribSize = AttributeSize;
			PldmBuffer->ValueOffset = PldmBuffer->AttribOffset + PldmBuffer->AttribSize; 
			PldmBuffer->ValueSize = ValueSize;
			PldmBuffer->MappingString = PldmBuffer->ValueOffset + PldmBuffer->ValueSize;
			PldmBuffer->MapStringSize = MapStringSize;
			
			MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->StringOffset) , StringTable, StringSize);
			MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->AttribOffset), AttributeTable, AttributeSize);
			MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->ValueOffset), ValueTable, ValueSize);
			MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->MappingString), MapStringTable, MapStringSize);
			
//Set pldm hash
			Status = pBS->LocateProtocol(&gAmiDigitalSignatureProtocolGuid, NULL, &AmiSig);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for locating gAmiDigitalSignatureProtocolGuid is %r \n", Status));
			if (EFI_ERROR (Status))
			{
				break;
			}
			Status = pBS->AllocatePool (EfiReservedMemoryType, 50, (void**)&HashDB);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for AllocatePool HashDB is %r \n", Status));
			
			if (EFI_ERROR (Status) || (NULL == HashDB))
			{
				break;
			}
			
			Status = AmiSig->Hash (AmiSig, &gEfiHashAlgorithmSha256Guid, 1, (const UINT8 **)&PldmBuffer, (const UINTN*)&PldmBufferSize, HashDB);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for Hash is %r \n", Status));
			if (EFI_ERROR (Status))
			{
				break;
			}
			Status = pRS->SetVariable(
						L"PldmHash",
						&PldmHashGuid,
						EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
						HashDBSize,
						HashDB
						);
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for PldmHash SetVariable %r \n", Status));
//Forming capsule to push
			CapsuleHeader = NULL;
			Status = pBS->AllocatePool (EfiReservedMemoryType, sizeof (EFI_CAPSULE_HEADER), (void**)&CapsuleHeader);
			if ((!EFI_ERROR (Status)) && (NULL != CapsuleHeader))
			{
				MemSet((void*)CapsuleHeader, sizeof (EFI_CAPSULE_HEADER), 0 );
				MemCpy (&(CapsuleHeader->CapsuleGuid), &PldmCapsuleGuid, sizeof (EFI_GUID));
				CapsuleHeader->HeaderSize = sizeof (EFI_CAPSULE_HEADER);
				CapsuleHeader->Flags = CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE | CAPSULE_FLAGS_PERSIST_ACROSS_RESET;
				CapsuleHeader->CapsuleImageSize = PldmBufferSize + sizeof (EFI_CAPSULE_HEADER);
			
				DEBUG((DEBUG_ERROR, "BiosConfigPreserve: CapsuleHeader->HeaderSize is %x CapsuleHeader->Flags is %x CapsuleHeader->CapsuleImageSize is %x\n",
						CapsuleHeader->HeaderSize, CapsuleHeader->Flags, CapsuleHeader->CapsuleImageSize));
				
				Size = 4 * sizeof (EFI_CAPSULE_BLOCK_DESCRIPTOR) + sizeof (EFI_CAPSULE_HEADER); // (4*16)+28
				DEBUG((DEBUG_ERROR, "BiosConfigPreserve: Size for capsule %x\n", Size));
				Status = pBS->AllocatePool (EfiReservedMemoryType, Size, (void**)&PldmCapsuleHdr);
				if ((!EFI_ERROR (Status)) && (NULL != PldmCapsuleHdr))
				{
					MemSet ((void*)PldmCapsuleHdr, Size, 0 );
					PldmCapsuleHdr [0].Length = CapsuleHeader->HeaderSize;
					PldmCapsuleHdr [0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)CapsuleHeader;
					
					PldmCapsuleHdr [1].Length = PldmBufferSize;
					PldmCapsuleHdr [1].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)PldmBuffer;
					
					PldmCapsuleHdr [2].Length = 0;
					PldmCapsuleHdr [2].Union.DataBlock = 0;
					
					//Calling original updatecapsule
					Status = gUpdateCapsule (&CapsuleHeader, 1, (EFI_PHYSICAL_ADDRESS)PldmCapsuleHdr);
					DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for PLDM UpdateCapsule is %r\n", Status));
					//Setting variables
					
					Status = pRS->SetVariable(
								L"NvramFlashed",
								&NvramFlashGuid,
								EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
								sizeof (UINT8),
								&NvramFlashFlag
								);
				}
			}
			break;
		}
	}
//	}
	Status = gUpdateCapsule (CapsuleHeaderArray, CapsuleCount, ScatterGatherList);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for original gUpdateCapsule is %r\n", Status));
	return Status;
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

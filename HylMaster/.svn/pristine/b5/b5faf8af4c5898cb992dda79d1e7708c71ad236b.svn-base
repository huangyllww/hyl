//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
AfuCommunication.c

AfuCommunication modulepart works together with OFBD to gather PLDM table
data from AFU

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "AfuCommunication.h"
#include "Ofbd/Ofbd.h"
#include "PldmSceStruct.h"
#include "Protocol/AmiDigitalSignature.h"
#include "Protocol/Hash.h"
#include <Guid/CapsuleVendor.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Library/BaseMemoryLib.h>

#define AfuAlign4(Value) (((Value)+3) & ~3)
#define AfuAlign8(Value) (((Value)+7) & ~7)
UINT8   *gPldmData = NULL;
UINT32  gPldmDataRecvSize = 0;
UINT32  gPldmDataSize = 0;
UINT8	gFlashUpdType = 0;
CHAR16 	gCapsuleNameBuffer [30];

UINT8   *gAfuPldmFileName = NULL;
UINT32  gPldmFileNameSize = 0;
UINT8	*gHashDB = NULL;
UINT32  gHashDBSize = 0;
CHAR16  *gFwCapsuleVarName = NULL;
EFI_CAPSULE_BLOCK_DESCRIPTOR  *gEfiCapsuleHdr = NULL;
EFI_CAPSULE_BLOCK_DESCRIPTOR  *gBackUpEfiCapsuleHdr = NULL;
EFI_SMM_SYSTEM_TABLE2           *mAfuSmst;
EFI_SMM_BASE2_PROTOCOL          *gSmmBase;

FLASH_PROTOCOL                  *mFlash = NULL;

EFI_GUID gAfuFlashDataFile  =  EFI_BCP_FV_DATA_GUID;
ROM_INFO    gRomInfoData;
void        *gRomDataLocation;
void        *gFvLocation;


EFI_STATUS AfuBcpGetVariable (IN AFU_BCP_SHARING_PROTOCOL *This, IN CHAR16 *VarName, OUT void **Buffer, OUT UINT32 *BufferSize, OUT CHAR16 **CapsuleDataName, OUT void **RomLoc, OUT void **FVLoc);

AFU_BCP_SHARING_PROTOCOL  gAfuBcpSharingProtocol = {
		AfuBcpGetVariable
};

EFI_GUID AmiSmmDigitalSignatureProtocolGuid = { 0x91ABC830, 0x16FC, 0x4D9E, {0xA1, 0x89, 0x5F, 0xC8, 0xBB, 0x41, 0x14, 0x02 }};

EFI_GUID gPldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
EFI_GUID gPldmHashGuid = EFI_PLDM_HASH_GUID;
extern EFI_GUID gEfiCapsuleVendorGuid;

/**
    Find the RAW section

    @param Section
    @param End

    @return Pointer to Raw Section if found

**/
VOID*
AfuFindRawSection (
    IN VOID *Section,
    IN VOID *End
)
{
    EFI_COMMON_SECTION_HEADER   *p = Section;
    while((UINTN)p < (UINTN)End)    //Use signed because 0 = 0x100000000
    {
        if (p->Type == EFI_SECTION_RAW) return (p+1);

        if (AfuAlign4(SECTION_SIZE(p)) == 0x00) {
            return 0;               // Section size = 0 indicates data is corrupted
        }

        p = (EFI_COMMON_SECTION_HEADER*)((UINT8*)p+AfuAlign4(SECTION_SIZE(p)));
    }
    return 0;
}


/**
    Find file in the FV with the input GUID

    @param Guid
    @param File
    @param EndOfFiles

    @return Pointer to File if found

**/
VOID*
AfuFindFile (
    IN EFI_GUID *Guid,
    IN VOID     *File,
    IN VOID     *EndOfFiles
)
{
    EFI_FFS_FILE_HEADER *p = File;

    while(((UINTN)p < (UINTN)EndOfFiles) &&
          ((UINTN)(p+sizeof(EFI_FFS_FILE_HEADER)) < (UINTN)EndOfFiles)) // Use signed because 0 = 0x100000000
    {
        if (CompareGuid(Guid,&p->Name)) {
            // Found File.
            return AfuFindRawSection(
                        p+1,
                        (UINT8*)p + (*(UINT32*)p->Size & 0xffffff) - sizeof(*p)
                        );
        }

        p = (EFI_FFS_FILE_HEADER*)((UINT8*)p + AfuAlign8((*(UINT32*)p->Size & 0xffffff)));
        if((*(UINT32 *)p == 0xFFFFFFFF))    //If Next file is not there, then break.
            break;
    }

    return NULL;
}



/**
    PLDM table gathering in SMM

*/ 
VOID BiosConfigPreserveInSmm (
	VOID
)
{
    EFI_STATUS                          Status = EFI_SUCCESS ;
    EFI_GUID                            gEfiFirmwareVolumeBlockProtocolGuid = EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL_GUID;
    EFI_HANDLE                          *HandleBuffer;
    UINTN                               NumHandles;
    EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *FvBlock;
    EFI_PHYSICAL_ADDRESS                Address;
    UINTN                               i;
    void                                *FirstFile;
    void                                *EndOfFiles;
    void                                *File;
    BOOLEAN                             Flag = 0 ;
    
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In BiosConfigPreserveInSmm\n"));
    
    Status = pBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &gSmmBase);
    DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (gSmmBase success) %r\n", Status));
    
    if(gSmmBase != NULL)
    {
        DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (LocateProtocol success)\n"));
        gSmmBase->GetSmstLocation (gSmmBase, &mAfuSmst);   // Save the system table pointer
    }
	 
    if (FVDataAccessType())
    {
        Status = pBS->LocateHandleBuffer(ByProtocol,
                                        &gEfiFirmwareVolumeBlockProtocolGuid,
                                        NULL,
                                        &NumHandles,
                                        &HandleBuffer);
        DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (LocateHandleBuffer success) %r\n", Status));
        if(Status == EFI_SUCCESS)
        {
            DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: (LocateHandleBuffer success) NumHandles = %d\n", NumHandles));
            for (i = 0; i < NumHandles; ++i) {
                Status = pBS->HandleProtocol(HandleBuffer[i],
                                            &gEfiFirmwareVolumeBlockProtocolGuid,
                                            (void **)&FvBlock);
                if (EFI_ERROR(Status)) continue;
                DEBUG((DEBUG_ERROR,"In BiosConfigPreserveInSmm: ( %d HandleProtocol success)\n", i));
        
                Status = FvBlock->GetPhysicalAddress(FvBlock, &Address);
                
                if (Status == EFI_SUCCESS) {
                    FirstFile = (UINT8*)Address
                                + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->HeaderLength;
        
                    EndOfFiles = (UINT8*)Address
                                + ((EFI_FIRMWARE_VOLUME_HEADER*)Address)->FvLength;
        
                    File = AfuFindFile(&gAfuFlashDataFile, FirstFile, EndOfFiles);
                    if (File) {
                        gRomInfoData = *(ROM_INFO*)File;
                        gRomDataLocation = (ROM_INFO*)File + 1;
                        Flag = 1 ;
                        gFvLocation = (void *)Address;
                    }
                    
                }
                if( Flag == 1 )
                    break ;
            }
            
            if( HandleBuffer )
            {
                pBS->FreePool(HandleBuffer);
            }
        }
    }
}
/**
    Deletes the variables which are not needed for current flash operations

*/ 
void DeleteExistingInfos (void)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status = pRS->SetVariable(
					L"PldmFileName",
					&gPldmFilePathGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
					0,
					NULL
					);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for deleting PldmFileName is %r\n", Status));

	if (NULL != gAfuPldmFileName)
	{
		Status = mAfuSmst->SmmFreePool (gAfuPldmFileName);
		gAfuPldmFileName = NULL;
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing gAfuPldmFileName is %r\n", Status));
	}
	Status = pRS->SetVariable(
				L"PldmHash",
				&gPldmHashGuid,
				EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
				0,
				NULL
				);
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for deleting PldmHash is %r\n", Status));
	if (NULL != gHashDB)
	{
		Status = mAfuSmst->SmmFreePool (gHashDB);
		gHashDB = NULL;
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing gHashDB is %r\n", Status));
	}
	
	if (NULL != gFwCapsuleVarName)
	{
		Status = pRS->SetVariable(
					gFwCapsuleVarName,
					&gEfiCapsuleVendorGuid,
					EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
					0,
					NULL
					);
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for deleting %S is %r\n", gFwCapsuleVarName, Status));
		gFwCapsuleVarName = NULL;
	}
	if (NULL != gPldmData)
	{
		Status = mAfuSmst->SmmFreePool (gPldmData);
		gPldmData = NULL;
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing gPldmData is %r\n", Status));
		
	}
	if (NULL != gBackUpEfiCapsuleHdr)
	{
		Status = mAfuSmst->SmmFreePool (gBackUpEfiCapsuleHdr);
		gBackUpEfiCapsuleHdr = NULL;
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing in gBackUpEfiCapsuleHdr is %r\n", Status));
	}
	gPldmDataRecvSize = 0;
	gPldmDataSize = 0;
}

/**
   	Function to gather PLDM table data

	@param Buffer - Bios preserve header
  	@param pOFBDDataHandled - handle value returns
  	
*/ 
VOID BiosConfigPreserveFnc (
	VOID		*Buffer,
	UINT8		*pOFBDDataHandled
)
{
	OFBD_HDR					*pOFBDHdr;
	OFBD_EXT_HDR				*pOFBDExtHdr;
	VOID						*pOFBDTblEnd;
	OFBD_TC_80_BCP_STRUCT		*AfuBcpStructPtr;
	EFI_STATUS					Status = EFI_SUCCESS;
	SCE_PLDM_DATA_HEADER		*ScePldmHeader = NULL;
	EFI_GUID					NvramFlashGuid = AFU_BCP_NVRAM_FLASH_GUID;
	UINT8						NvramFlashFlag = 1;
	
	if (*pOFBDDataHandled == 0)
	{
		pOFBDHdr = (OFBD_HDR *)Buffer;
		pOFBDExtHdr = (OFBD_EXT_HDR *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_HDR_SIZE));
		pOFBDTblEnd = (VOID *)((UINT8 *)Buffer + (pOFBDHdr->OFBD_Size));
		AfuBcpStructPtr = (OFBD_TC_80_BCP_STRUCT *)((UINT8 *)pOFBDExtHdr + sizeof(OFBD_EXT_HDR));

//		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pOFBDHdr->OFBD_FS is %x\n", pOFBDHdr->OFBD_FS));
//		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pOFBDHdr->OFBD_FS & is %x\n", pOFBDHdr->OFBD_FS & OFBD_FS_BIOS_CFG_PRESERVE));
		
		if (pOFBDHdr->OFBD_FS & OFBD_FS_BIOS_CFG_PRESERVE)
		{
			//Check Type Code ID
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pOFBDExtHdr->TypeCodeID is %x\n",pOFBDExtHdr->TypeCodeID));
			
			if (OFBD_EXT_TC_BIOS_CFG_PRESERVE == pOFBDExtHdr->TypeCodeID)
			{
				DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpStructPtr->Command is %x\n",AfuBcpStructPtr->Command));
				AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_OK;			//Make ok as current if error change it afterwards
				
				DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpStructPtr->Command is %x\n", AfuBcpStructPtr->Command));
				DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpStructPtr->Status is %x\n", AfuBcpStructPtr->Status));
				
				switch (AfuBcpStructPtr->Command)
				{
					case OFBD_TC_BCP_SET_CONFIG_PRESERVE_INFO:
					{
						BIOS_CONFIG_PRESERVE_INFO *BcpInfo;
						
						///setting dummy variable which informs us whether NVRAM flashed or not. If NVRAM not flashed no need to import PLDM
						
						Status = pRS->SetVariable(
									L"NvramFlashed",
									&NvramFlashGuid,
									EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
									sizeof (UINT8),
									&NvramFlashFlag
									);
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for setting NvramFlashed variable is %x\n", Status));
						
						BcpInfo = (BIOS_CONFIG_PRESERVE_INFO *)pOFBDTblEnd;
						if ((RUNTIME_FLASH_UPDATE == BcpInfo->FlashOpType) || (RECOVERY_FLASH_UPDATE == BcpInfo->FlashOpType))
						{
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gFlashUpdType %X\n", gFlashUpdType));
							if (0 != gFlashUpdType)
							{
								DeleteExistingInfos ();
							}
							gFlashUpdType = RUNTIME_FLASH_UPDATE;
							InstallAfuBcpSharingProtocol ();
							
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: size of BIOS_CONFIG_PRESERVE_INFO is %x\n", sizeof (BIOS_CONFIG_PRESERVE_INFO)));
							
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: BcpInfo->Length is %x BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO) %x\n", BcpInfo->Length, BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO)));
							Status = mAfuSmst->SmmAllocatePool (EfiRuntimeServicesData, BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO), (void**)&gAfuPldmFileName);
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In recovery or runtime gAfuPldmFileName %X\n", gAfuPldmFileName));
							if ((!EFI_ERROR (Status)) && (NULL != gAfuPldmFileName))
							{
								gPldmFileNameSize = BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO);
								MemCpy (gAfuPldmFileName, ((UINT8 *)BcpInfo + sizeof (BIOS_CONFIG_PRESERVE_INFO)), BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO));
							}
							else
							{
								AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
							}
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In recovery or runtime gPldmFileNameSize %x gAfuPldmFileName %x gAfuPldmFileName %a \n", gPldmFileNameSize, gAfuPldmFileName, gAfuPldmFileName));
						}
						else if (CAPSULE_FLASH_UPDATE == BcpInfo->FlashOpType)
						{
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gFlashUpdType %X\n", gFlashUpdType));
							if (0 != gFlashUpdType)
							{
								DeleteExistingInfos ();
							}
							gFlashUpdType = CAPSULE_FLASH_UPDATE;
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: In capsule mode gAfuPldmFileName %x\n", gAfuPldmFileName));
							InstallAfuBcpSharingProtocol ();
						}
						if( FVDataAccessType() ){
							AfuBcpStructPtr->Status |= OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED;
							DEBUG((DEBUG_ERROR,"BiosConfigPreserve: OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED executed\n"));
						}
						break;
					}
					case OFBD_TC_BCP_LOAD_PLDM_DATA:
					{
						BCP_PLDM_DATA_BLOCK *BcpPldmData = NULL;
						
						BcpPldmData = (BCP_PLDM_DATA_BLOCK *)pOFBDTblEnd;
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: BcpPldmData->Length is %x\n", BcpPldmData->Length));

						ScePldmHeader = (SCE_PLDM_DATA_HEADER *)((UINT8 *)BcpPldmData + sizeof (BCP_PLDM_DATA_BLOCK));
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: size of BCP_PLDM_DATA_BLOCK is %x\n", sizeof (BCP_PLDM_DATA_BLOCK)));
						if (NULL == gPldmData)
						{
							if (NULL != mAfuSmst)
							{
								gPldmDataSize = ScePldmHeader->MappingString + ScePldmHeader->MapStringSize;
								DEBUG((DEBUG_ERROR,"BiosConfigPreserve: MappingString %d, MapStringSize %d\n", ScePldmHeader->MappingString, ScePldmHeader->MapStringSize));
								Status = mAfuSmst->SmmAllocatePool (EfiRuntimeServicesData, (ScePldmHeader->MappingString + ScePldmHeader->MapStringSize), (void**)&gPldmData);
								DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for allocating gPldmData is %r Size is %x\n", Status, (ScePldmHeader->MappingString + ScePldmHeader->MapStringSize)));
								if (EFI_ERROR (Status))
								{
									gPldmData = NULL;
									AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
								}
								MemCpy (gPldmData, (UINT8 *)BcpPldmData + sizeof (BCP_PLDM_DATA_BLOCK), BcpPldmData->Length - sizeof (BCP_PLDM_DATA_BLOCK));
//								gPldmFileNameSize = BcpInfo->Length - sizeof (BIOS_CONFIG_PRESERVE_INFO);
								gPldmDataRecvSize += BcpPldmData->Length - sizeof (BCP_PLDM_DATA_BLOCK);
							}
							else
							{
								AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
							}
						}
						else
						{
							MemCpy ((gPldmData + gPldmDataRecvSize), ((UINT8 *)BcpPldmData + sizeof (BCP_PLDM_DATA_BLOCK)), BcpPldmData->Length - sizeof (BCP_PLDM_DATA_BLOCK));
							gPldmDataRecvSize += (BcpPldmData->Length - sizeof (BCP_PLDM_DATA_BLOCK));
//							gPldmDataRecvSize += BcpPldmData->Length;
						}
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gPldmDataRecvSize is %x gPldmDataSize is %x\n", gPldmDataRecvSize, gPldmDataSize));
						DEBUG((DEBUG_ERROR,"BiosConfigPreserve: FVDataAccessType is %d\n", FVDataAccessType()));
						if ((gPldmDataRecvSize >= gPldmDataSize) && (gPldmDataSize))
						{
							AMI_DIGITAL_SIGNATURE_PROTOCOL *AmiSig;
							Status = mAfuSmst->SmmLocateProtocol(&AmiSmmDigitalSignatureProtocolGuid, NULL, &AmiSig);
							if (!EFI_ERROR (Status))
							{
								Status = mAfuSmst->SmmAllocatePool (EfiRuntimeServicesData, 50, (void**)&gHashDB);
								if (EFI_ERROR (Status))
								{
									AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
									break;
								}
								Status = AmiSig->Hash (AmiSig, &gEfiHashAlgorithmSha256Guid, 1, &gPldmData, (const UINTN*)&gPldmDataRecvSize, gHashDB);

								if (!EFI_ERROR (Status))
								{
									gHashDBSize = 32;
									if (CAPSULE_FLASH_UPDATE == gFlashUpdType)
									{
										EFI_CAPSULE_HEADER      *CapsuleHeader;
										EFI_GUID                PldmCapsuleGuid = EFI_PLDM_CAPSULE_GUID;
										UINTN                   Size = 0;
										
										Status = mAfuSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (EFI_CAPSULE_HEADER), (void**)&CapsuleHeader);
										DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Alloc Status for capsule header %r\n", Status));
										if (!EFI_ERROR (Status) && (NULL != CapsuleHeader))
										{
											MemCpy (&(CapsuleHeader->CapsuleGuid), &PldmCapsuleGuid, sizeof (EFI_GUID));
											CapsuleHeader->HeaderSize = sizeof (EFI_CAPSULE_HEADER);
											CapsuleHeader->Flags = CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE | CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET;
											CapsuleHeader->CapsuleImageSize = gPldmDataRecvSize + sizeof (EFI_CAPSULE_HEADER);
											DEBUG((DEBUG_ERROR,"BiosConfigPreserve: CapsuleHeader->CapsuleImageSize %x\n", CapsuleHeader->CapsuleImageSize));
											
											Size = 4 * sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR) + sizeof(EFI_CAPSULE_HEADER); // (4*16)+28
											DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Size for capsule %x\n", Size));
											Status = mAfuSmst->SmmAllocatePool (EfiRuntimeServicesData, Size, (void**)&gEfiCapsuleHdr);
											DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Alloc Status for gEfiCapsuleHdr %r\n", Status));
											DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gEfiCapsuleHdr is %x\n", gEfiCapsuleHdr));
											if (!EFI_ERROR (Status)&& (NULL != gEfiCapsuleHdr))
											{
												MemSet((void*)gEfiCapsuleHdr, Size, 0 );
												gEfiCapsuleHdr[0].Length = CapsuleHeader->HeaderSize;
												gEfiCapsuleHdr[0].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)CapsuleHeader;
												
												gEfiCapsuleHdr[1].Length = gPldmDataRecvSize;
												gEfiCapsuleHdr[1].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)gPldmData;
												
												gEfiCapsuleHdr[2].Length = 0;
												gEfiCapsuleHdr[2].Union.DataBlock = 0;
											}
											else
											{
												if (gPldmData)
												{
													Status = mAfuSmst->SmmFreePool (gPldmData);
													gPldmData = NULL;
													DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing in NULL header is %r\n", Status));
												}
												AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
											}
										}
										else
										{
											if (gPldmData)
											{
												Status = mAfuSmst->SmmFreePool (gPldmData);
												DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing NULL capsule is %r\n", Status));
												gPldmData = NULL;
											}
											AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
										}
									}
									else
									{
									    if (FVDataAccessType())
									    {
					                        UINTN FlashDataSize = 0;
					                        FlashDataSize = FVFlashDataSize();
					                        if(gPldmDataSize > FlashDataSize)
					                        {
					                            DEBUG((DEBUG_ERROR,"BiosConfigPreserve: (ERROR) PLDM Data size(%x) is more than BCP FV Size(%x)\n", gPldmDataSize, FlashDataSize));
	                                            if (gPldmData)
	                                            {
	                                                Status = mAfuSmst->SmmFreePool (gPldmData);
	                                                gPldmData = NULL;
	                                            }
	                                            AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_FV_SIZE_EXCEED_FAILED;
	                                            DEBUG((DEBUG_ERROR,"BiosConfigPreserve: OFBD_TC_BCP_RS_FV_SIZE_EXCEED_FAILED executed\n"));
					                        }
									    }
										if (gEfiCapsuleHdr)
										{
											Status = mAfuSmst->SmmFreePool (gEfiCapsuleHdr);
											gEfiCapsuleHdr = NULL;
											DEBUG((DEBUG_ERROR,"BiosConfigPreserve: Status for freeing gEfiCapsuleHdr is %r\n", Status));
										}
									}
								}
								else
								{
									AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
								}
							}
							else
							{
								AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED;
							}
						}
						break;
					}
				}
			}
			else
			{
				AfuBcpStructPtr->Status = OFBD_TC_BCP_RS_NOT_SUPPORTED;		//Found BCP flash status but wrong command
			}
		}
	}
}

/**
	Function to check the CapsuleUpdateDataN variable presence
	Capsule NVRAM variables are owned by runtime Capsule Update service. 
	Each service must not override previously created instances 
	of the variable. Create a new one with n+1 index
  
	@retval	CHAR16*
*/ 
CHAR16 *BCPGetLastFwCapsuleVariable (void)
{
    EFI_STATUS 	Status = EFI_SUCCESS;
    UINTN 		Index = 0;
    CHAR16    	CapsuleName [30];
    UINTN       Size;

    MemSet ((void*)gCapsuleNameBuffer, sizeof (gCapsuleNameBuffer), 0 );
    Swprintf_s (gCapsuleNameBuffer, 30, L"%s", EFI_CAPSULE_VARIABLE_NAME);
    // get any NVRAM variable of the format L"CapsuleUpdateDataN" where N is an integer
    while(!EFI_ERROR(Status)) {
        if(Index > 0 )
            Swprintf_s(gCapsuleNameBuffer, 30, L"%s%d", EFI_CAPSULE_VARIABLE_NAME, Index);
        
        Size = sizeof (CapsuleName);
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: gCapsuleNameBuffer %S Size %d\n", gCapsuleNameBuffer, Size));
        Status = pRS->GetVariable (	gCapsuleNameBuffer,
        							&gEfiCapsuleVendorGuid,
                                    NULL,
                                    &Size, 
                                    CapsuleName);
        
        DEBUG((DEBUG_ERROR,"BiosConfigPreserve: status for get variable %x\n", Status));
        if (EFI_ERROR(Status))		//CapsuleUpdateDataN variable no found return it
        {
            break;
        }
        Index++;
    }
    return gCapsuleNameBuffer;
}

/**
	Installs a SMM protocol which shared between AFUCommunication and AFUBcpPreserve
  
	@retval	EFI_STATUS
*/
EFI_STATUS InstallAfuBcpSharingProtocol (void)
{
	EFI_HANDLE  DummyHandle = NULL;
	EFI_GUID 	AfuBcpSharingProtocolGuid = AFU_BCP_SHARING_PROTOCOL_GUID;
	EFI_STATUS Status = EFI_SUCCESS;
	void		*DummyInterface = NULL;
	
    Status = mAfuSmst->SmmLocateProtocol (&AfuBcpSharingProtocolGuid, NULL, &DummyInterface);		//To avoid multiple install
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: status for locating protocol is %r DummyInterface %x\n", Status, DummyInterface));
    if (!EFI_ERROR (Status))
    {
    	return Status;
    }
    
    Status = mAfuSmst->SmmInstallProtocolInterface(
                 &DummyHandle,
                 &AfuBcpSharingProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &gAfuBcpSharingProtocol
             );
    DEBUG((DEBUG_ERROR,"BiosConfigPreserve: status for installing protocol is %r gAfuBcpSharingProtocol %x \n", Status, gAfuBcpSharingProtocol));
    return Status;
}

/**
   	Function to gather PLDM table data

	@param AFU_BCP_SHARING_PROTOCOL - Bios preserve header
  	@param CHAR16 * - Name of the variable to be read
  	@param void ** - Buffer to return
  	@param UINT32 * - Size of the retuning buffer
  	@param CHAR16 * - Filled only in capsule update case
  	
  	@retval	EFI_STATUS
  	
*/ 
EFI_STATUS AfuBcpGetVariable (IN AFU_BCP_SHARING_PROTOCOL *This, IN CHAR16 *VarName, OUT void **Buffer, OUT UINT32 *BufferSize, OUT CHAR16 **CapsuleDataName, OUT void **RomLoc, OUT void **FVLoc)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;
	
	DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpGetVariable VarName is %S\n", VarName));
	
	if ( !StrCmp (VarName, L"PldmFileName"))
	{
		if( !FVDataAccessType () )
		{
			if (NULL != gAfuPldmFileName)
			{
				DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pldm filename available\n"));
				*Buffer = gAfuPldmFileName;
				*BufferSize = gPldmFileNameSize;
				gAfuPldmFileName = NULL;		//Freeing will be taken care in AFUBcpPreserveVar
				Status = EFI_SUCCESS;
			}
		}
		else
		{
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pldm data available\n"));
			*Buffer = gPldmData;
			*BufferSize = gPldmDataSize;
			gAfuPldmFileName = NULL;		//Freeing will be taken care in AFUBcpPreserveVar
			Status = EFI_SUCCESS;
			*RomLoc = gRomDataLocation ;
			*FVLoc = gFvLocation ;
		}
	}
	else if (!StrCmp (VarName, L"PldmHash"))
	{
		if (NULL != gHashDB)
		{
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pldm hash available\n"));
			*Buffer = gHashDB;
			*BufferSize = gHashDBSize;
			gHashDB = NULL;				//Freeing will be taken care in AFUBcpPreserveVar
			Status = EFI_SUCCESS;
		}
	}
	else if (!StrCmp (VarName, L"PldmCapsule"))
	{
		DEBUG((DEBUG_ERROR,"BiosConfigPreserve: AfuBcpGetVariable Check Capsule\n"));
		if (NULL != gEfiCapsuleHdr)
		{
			DEBUG((DEBUG_ERROR,"BiosConfigPreserve: pldm capsule available\n"));
			*Buffer = gEfiCapsuleHdr;
			*BufferSize = sizeof (EFI_PHYSICAL_ADDRESS);
			gFwCapsuleVarName = BCPGetLastFwCapsuleVariable ();
			*CapsuleDataName = gFwCapsuleVarName;
			gBackUpEfiCapsuleHdr = gEfiCapsuleHdr;
			gEfiCapsuleHdr = NULL;
			Status = EFI_SUCCESS;
		}
	}
	return Status;
}

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

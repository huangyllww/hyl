//*****************************************************************//
//*****************************************************************//
//**                                                                                            **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.**//
//**                                                                                            **//
//**                     All Rights Reserved.                                       **//
//**                                                                                            **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                                                            **//
//**                     Phone: (770)-246-8600                                  **//
//**                                                                                            **//
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
// Name:  AmiFwUpdateCore.c
//
// Description: All the flashing methods are available here
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
#include "AmiFwUpdateBmcCore.h"
#include "AmiFwUpdateBmcStrDefs.h"
#include <Protocol/AmiAfuProtocol.h>
#include <Protocol/SmiFlash.h>
#include "AmiFwUpdateBmcUIIntf.h"
#include <Library/UefiLib.h>
#include "AmiFwUpdateBmcCoreIntf.h"
#include "Token.h"

//*************GLOBAL VARIABLES*************//
extern AMI_AFU_PROTOCOL *gAFUFlash;
extern FLASH_POLICY_INFO_BLOCK		gFlashingPolicy ;
FUNC_BLOCK  *pFuncBlock = NULL;
FUNC_FLASH_SESSION_BLOCK  gFlashSessionBlock ;
extern EFI_RUNTIME_SERVICES  *gRT;
extern UINTN gFlashOPtions;
extern UINTN gUpdateFlag;
extern UINTN gSelection;

extern EFI_BOOT_SERVICES     *gBS;

typedef enum {FwRuntime,FwRecovery,FwCapsule}Flash_Type;
void PreserveDatainFlash(Flash_Type Flashtype);
EFI_STATUS SendPatchReq ();

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: SECURE_FLASH_CAPSULE
//
// Description	: To start secure CAPSULE flashing
//
// Parameter	: VOID *, UINT32 
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SECURE_FLASH_CAPSULE(VOID *FileBuffer, UINT32 FileSize)
{
	EFI_STATUS	Status = EFI_SUCCESS;

    TRACE((-1, "AMIFW:  SECURE_FLASH_CAPSULE\n"));
    
    pFuncBlock = NULL;

/* Load ROM image data block into BIOS. */	
	pFuncBlock->BufAddr = (UINTN)FileBuffer;
	pFuncBlock->BlockAddr = 0 ;
	pFuncBlock->BlockSize = FileSize ;
	pFuncBlock->ErrorCode = 1;
	
	Status = gAFUFlash->LoadFirmwareImage(  pFuncBlock );
	
/* Inform BIOS which flash method will be taken. /
		/* Capsule  Flash*/		
	gFlashSessionBlock.FlUpdBlock.FlashOpType = FlCapsule ;
	gFlashSessionBlock.FlUpdBlock.ROMSection = (UINT32)gSelection;
	gFlashSessionBlock.FlUpdBlock.ROMSection = gFlashSessionBlock.FlUpdBlock.ROMSection << 24;
	gFlashSessionBlock.FlUpdBlock.ROMSection |= ((1 << BOOT_BLOCK ) | (1 <<  MAIN_BLOCK) |  (1 << NV_BLOCK));
	DEBUG((DEBUG_ERROR,"FwUpdCore: ROMSection is %x\n", gFlashSessionBlock.FlUpdBlock.ROMSection));
	gFlashSessionBlock.FlUpdBlock.ImageSize =  FileSize ;
	gFlashSessionBlock.ErrorCode = 1;
	Status = gAFUFlash->SetFlashMethod(  &gFlashSessionBlock );
	if(gFlashSessionBlock.ErrorCode == 1)
	{
	    Status = EFI_NOT_FOUND;
		FWCore_ReportError(STR_SECURE_FLASH_FAILED, (CHAR16*)NULL);
	}
	else
	{
#if SAVE_PLDM_DATA_FROM_FWUPDATE
		//preserving data in CAPSULE flash
		PreserveDatainFlash(FwCapsule);
#endif
        SendPatchReq();

		FWCore_ReportError(STR_SYSTEM_RESET, (CHAR16*)NULL);
		gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0,  0);
	}
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: SECURE_FLASH_RECOVERY
//
// Description	: To start secure RECOVERY flashing
//
// Parameter	: VOID *, UINT32 
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SECURE_FLASH_RECOVERY(CHAR16 *FileName, UINT32 FileSize)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    CHAR8 *RomFileName = NULL;

    TRACE((-1, "AMIFW:  SECURE_FLASH_RECOVERY\n"));
    /* Inform BIOS which flash method will be taken. */
    /*UnicodeToAsciii*/
    RomFileName = (CHAR8 *) AllocateZeroPool ((StrLen (FileName) + 1) * sizeof (CHAR8));	
    UnicodeStrToAsciiStr(FileName , RomFileName );			

    /* Recovery  Flash*/		
    CopyMem( &(gFlashSessionBlock.FlUpdBlock.FwImage.AmiRomFileName[0]) , RomFileName, 16);
    
    gFlashSessionBlock.FlUpdBlock.FlashOpType = FlRecovery ;
    gFlashSessionBlock.FlUpdBlock.ROMSection = (UINT32)gSelection;
    gFlashSessionBlock.FlUpdBlock.ROMSection = gFlashSessionBlock.FlUpdBlock.ROMSection << 24;
    gFlashSessionBlock.FlUpdBlock.ROMSection |= ((1 << BOOT_BLOCK ) | (1 <<  MAIN_BLOCK) |  (1 << NV_BLOCK));
    DEBUG((DEBUG_ERROR,"FwUpdCore: ROMSection is %x\n", gFlashSessionBlock.FlUpdBlock.ROMSection));
    gFlashSessionBlock.FlUpdBlock.ImageSize =  FileSize ;
    gFlashSessionBlock.ErrorCode = 1;
    Status = gAFUFlash->SetFlashMethod(  &gFlashSessionBlock );

    if(gFlashSessionBlock.ErrorCode == 1)
    {
        Status = EFI_NOT_FOUND;
    	FWCore_ReportError(STR_SECURE_FLASH_FAILED, (CHAR16*)NULL);
    }
    else
    {
#if SAVE_PLDM_DATA_FROM_FWUPDATE
        //preserve data in RECOVERY flash
        PreserveDatainFlash(FwRecovery);
#endif
        SendPatchReq();

    	FWCore_ReportError(STR_SYSTEM_RESET, (CHAR16*)NULL);
    	gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0,  0);
    }
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: SECURE_FLASH_RUNTIME
//
// Description	: To start secure RUNTIME flashing
//
// Parameter	: VOID *, UINT32 
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SECURE_FLASH_RUNTIME(VOID *FileBuffer, UINT32 FileSize)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	TRACE((-1, "AMIFW:  SECURE_FLASH_RUNTIME\nAMIFW:  gFlashOPtions = %x\n", gFlashOPtions));
	
	pFuncBlock = NULL;
	// Load ROM image data block into BIOS. 	
	pFuncBlock->BufAddr = (UINTN)FileBuffer;
	pFuncBlock->BlockAddr = 0 ;
	pFuncBlock->BlockSize = FileSize ;
	pFuncBlock->ErrorCode = 1;
	
	Status = gAFUFlash->LoadFirmwareImage(  pFuncBlock );
	/* Inform BIOS which flash method will be taken. */
	/*  Runtime  Flash */
	gFlashSessionBlock.FlUpdBlock.FlashOpType = FlRuntime ;
    gFlashSessionBlock.FlUpdBlock.ROMSection = (UINT32)CLRCFG;
    gFlashSessionBlock.FlUpdBlock.ROMSection = gFlashSessionBlock.FlUpdBlock.ROMSection << 24;
	gFlashSessionBlock.FlUpdBlock.ROMSection |= (UINT32)gFlashOPtions;
	DEBUG((DEBUG_ERROR,"FwUpdCore: ROMSection is %x\n", gFlashSessionBlock.FlUpdBlock.ROMSection));
	gFlashSessionBlock.FlUpdBlock.ImageSize =  FileSize ;
	gFlashSessionBlock.ErrorCode = 1;
	Status = gAFUFlash->SetFlashMethod(  &gFlashSessionBlock );
	if(gFlashSessionBlock.ErrorCode == 1)
	{
	    Status = EFI_NOT_FOUND;
		FWCore_ReportError(STR_SECURE_FLASH_FAILED, (CHAR16*)NULL);	
	}
	else
	{
#if SAVE_PLDM_DATA_FROM_FWUPDATE
	    //preserve data in RUNTIME flash
	    PreserveDatainFlash(FwRuntime);
#endif

		Status = gAFUFlash->BiosUpdate((UINTN)(VOID *)FileBuffer, (UINT32)FileSize, gFlashOPtions, gSelection, &DisplayCommonProgress, 0, (UINT32)gUpdateFlag);	

		if( Status != EFI_SUCCESS)
			FWCore_ErrorMsg( Status ) ;
	}

	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: NON_SECURE_FLASH_BIOS
//
// Description	: To start Non-Secure bios flashing process
//
// Parameter	: VOID *, UINT32 
//				
// Return Value  : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NON_SECURE_FLASH_BIOS(VOID *FileBuffer, UINT32 FileSize)
{	
	EFI_STATUS Status = EFI_SUCCESS;
	TRACE((-1, "AMIFW:  NON_SECURE_FLASH_BIOS\nAMIFW:  gFlashOPtions = %x\n", gFlashOPtions));
	
#if SAVE_PLDM_DATA_FROM_FWUPDATE
    //preserve data in RUNTIME flash
    PreserveDatainFlash(FwRuntime);
#endif
    
	//BIOS update	
	Status = gAFUFlash->BiosUpdate((UINTN)(VOID *)FileBuffer, (UINT32)FileSize, gFlashOPtions, gSelection, &DisplayCommonProgress, 0, (UINT32)gUpdateFlag);
	
	if( Status != EFI_SUCCESS)
		FWCore_ErrorMsg( Status ) ;
	
	return Status;
}

//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2014, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************

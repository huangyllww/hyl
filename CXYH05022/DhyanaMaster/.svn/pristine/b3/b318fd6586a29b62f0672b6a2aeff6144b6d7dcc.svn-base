//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
/** @file PspNcbRecoveryFuncs.c

**/
//*************************************************************************
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/SmiFlash.h>
#include <Protocol/AmiPostMgr.h>
#include <Protocol/FlashProtocol.h>
#include <Library/PspNcbUpdate.h>
#include <Capsule.h>
#include <AmiHobs.h>

extern FLASH_PROTOCOL *Flash;
extern UINT8 *RecoveryBuffer;

#define PSP_FLASH_ADDR_TRANSFER(addr)   (addr - (0xFFFFFFFF - FLASH_SIZE + 1))

#define ROMSIG_BASE                     (0xFFFFFFFF - FLASH_SIZE + FCH_FIRMWARE_OFFSET + 1)

PSP_UPDATE_BLOCK_TABLE_STRUCT gPspUpdateBlockTable[] =
{
#if PSP_CRISIS_RECOVERY_SUPPORT == 0
		// { Start address, End address }
		{ PSP_DATA_BASE, PSP_DATA_BASE + PSP_DATA_SIZE, PSP_DATA_OFFSET },
		{ ROMSIG_BASE, ROMSIG_BASE + ROMSIG_SIZE, FCH_FIRMWARE_OFFSET}
#else
		// { Start address, End address },
		// Although APOB shared block is not supported but the offset of shared APOB is same updateable PSP FW offset
		{ PSP_APOB_SHARED_ADDRESS, PSP_DATA_BASE + PSP_DATA_SIZE, PSP_APOB_SHARED_OFFSET }
#endif
};

VOID PspUpdateBlockTable()
{
    UINTN                       i, j;
    EFI_STATUS                  Status;
    VOID                        *DisplayHandle;
    AMI_POST_MGR_KEY            OutKey;
    AMI_POST_MANAGER_PROTOCOL   *AmiPostMgr = NULL;
    UINT32                      UpdateBlockAddr = 0, k = 0;
    UINT32                      StartBlock, EndBlock;
    UINT8                       PspUpdatBlockTblEntries;
    EFI_GUID                    ImageCapsuleGuid = W8_SCREEN_IMAGE_CAPSULE_GUID;
    EFI_GUID                    guidHob = HOB_LIST_GUID;
    EFI_HOB_UEFI_CAPSULE        *Hob;
    EFI_CAPSULE_HEADER          *Capsule;
    EFI_TPL                     OldTpl;
    UINT8                       *Source;

    PspUpdatBlockTblEntries =
            sizeof(gPspUpdateBlockTable) / sizeof(PSP_UPDATE_BLOCK_TABLE_STRUCT);

    // No NCB need to be updated.
    if (!PspUpdatBlockTblEntries)
        return;

    //Verify if we're on OS firmware update path
    Hob = GetEfiConfigurationTable(pST, &guidHob);
    
    if (Hob != NULL) {
        do {
            Status = FindNextHobByType(EFI_HOB_TYPE_UEFI_CAPSULE, &Hob);
            if (!EFI_ERROR(Status)) { 
                Capsule = (EFI_CAPSULE_HEADER *)(VOID *)(UINTN)Hob->BaseAddress;
                if (!guidcmp(&(Capsule->CapsuleGuid), &ImageCapsuleGuid))
                    break;
            }
          } while(!EFI_ERROR(Status));
  
       if (EFI_ERROR(Status)) {   //no image hob - we're not on OS FW update path
          Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL, &AmiPostMgr);
          if (EFI_ERROR(Status))  AmiPostMgr = NULL;
       }
    }

    if (AmiPostMgr)
    {
        AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_INIT,
                    L"PSP NCB Update Progress",
                    NULL,
                    NULL,
                    0,
                    &DisplayHandle,
                    &OutKey);
    }

    for (i = 0; i < PspUpdatBlockTblEntries; i++)
    {
        StartBlock = PSP_FLASH_ADDR_TRANSFER(gPspUpdateBlockTable[i].StartAddress) / FLASH_BLOCK_SIZE;
        EndBlock = PSP_FLASH_ADDR_TRANSFER(gPspUpdateBlockTable[i].EndAddress) / FLASH_BLOCK_SIZE;

        TRACE((TRACE_ALWAYS,"Table%d, StartBlock: %08X, EndBlock: %08X\n"
        		,i ,gPspUpdateBlockTable[i].StartAddress, gPspUpdateBlockTable[i].EndAddress));

        // Update NCBs
        for (j = StartBlock, k = 0; j < EndBlock; j++, k++)
        {
            UpdateBlockAddr = gPspUpdateBlockTable[i].StartAddress + (k * FLASH_BLOCK_SIZE);
            TRACE((TRACE_ALWAYS,"UpdateBlockAddr: %08X\n",UpdateBlockAddr));

            Source = RecoveryBuffer + gPspUpdateBlockTable[i].BlockOffset + (k * FLASH_BLOCK_SIZE);
            TRACE((TRACE_ALWAYS,"Source: %08X\n", Source));

            if (AmiPostMgr)
            {
                AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_UPDATE,
                         L"PSP NCB Update Progress",
                         NULL,
                         NULL,
                         ((j-StartBlock)+1)*100/(EndBlock-StartBlock),
                         &DisplayHandle,
                         &OutKey);
            }

            OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
            Status = Flash->Update((UINT8*)UpdateBlockAddr, FLASH_BLOCK_SIZE, Source);
            pBS->RestoreTPL(OldTpl);
            TRACE((TRACE_ALWAYS,"PSP Flash->Update Status: %08x\n",Status));
        }
    }
    if (AmiPostMgr)
    {
        AmiPostMgr->DisplayProgress(AMI_PROGRESS_BOX_CLOSE,
                     L"PSP NCB Update Progress",
                     NULL,
                     NULL,
                     0,
                     &DisplayHandle,
                     &OutKey);
    }

}

VOID PspRecoveryInsertOEMTableBeforeFlash (VOID)
{
    PspUpdateBlockTable();
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

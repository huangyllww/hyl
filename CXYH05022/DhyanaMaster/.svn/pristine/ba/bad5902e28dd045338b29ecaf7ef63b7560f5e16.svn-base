//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
Afufile.c

This provides generic functions .

*///**********************************************************************
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/Cpu.h>
#include <Ppi/FwVersion.h>
#include <Flash.h>
#include <Setup.h>
#include <Ffs.h>
#include <AmiLib.h>
#include <Efi.h>

#include <Protocol/AmiAfuProtocol.h>
#include <AmiAfuProtocolElinks.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include "Afufile.h"
#include "Afusysrom.h"

static INT32            AFU_FileRHCnt;                  ///< AFU File Rom Hole Count.
static BOOLEAN          AFU_RomFile_bHaveSLP20 = FALSE; ///< AFU File Have SLP 2.0

extern ROMInfo * gpFlashInfo;

extern BOOLEAN  bUseRomFile;

ROMHOLEREC   AFU_RomFileRHRECList[MAXNCBLK];        ///< AFU Rom File Rom Hole Record List.
ROMHOLEREC   AFU_RomFileSLPRHRECList[SLP_NUMBER];   ///< AFU Rom File SLP2.0 Record List.

/**
 *  Search Rom File NCB.
 *
 *  Search Rom File Rom Hole and NCB.
 *  
 *  @param[out] lpFilebuff Rom File Buffer.
 *  @retval TRUE Have Rom Hole or NCB.
 *  @retval FALSE Have not Rom Hole and NCB.
 */
BOOLEAN        AFU_RomFile_SearchNCB(PBYTE lpFilebuff)
{
    UINT32  StrAddr = 0;
    UINT32  BlockSize = 0;
    UINT8   HaveRH = 0;
    DWORD   MainRH = 0;
    DWORD   BootRH = 0;
    UINT16  PrevBlockIndex = 0;
    UINT16  TotalBlock = gpFlashInfo->TotalBlocks;
    BOOLEAN    bSacnEndFlag = FALSE;
    UINT8   SLPNumber = 0;

    DWORD i = 0;
    DWORD j = 0;
    
    ROMHOLEREC  TempRHRECList[MAXNCBLK] = {0};
    AFU_FileRHCnt = 0;
    AFU_RomFile_bHaveSLP20 = FALSE;

    
    
    do
    {
        i = MainRH;

        if (AFU_SysRom_GetFVRang(MAINBIOS, &StrAddr, &BlockSize, &PrevBlockIndex))
        {
            bUseRomFile = TRUE;
            MainRH += AFU_SysRom_GetRomHole(lpFilebuff, StrAddr, BlockSize, TempRHRECList);
            if (MainRH)
                HaveRH++;
        }
        //__AFUFILEDBG(WriteOut("[DBG]MainRH = %d\n", MainRH);)
        for ( ; i < MainRH ; i++)
        {
            if (RH_ROMHOLE == TempRHRECList[i].Type)
            {
                AFU_RomFileRHRECList[AFU_FileRHCnt] = TempRHRECList[i];
                AFU_FileRHCnt++;
            }
            else if (RH_SLP == TempRHRECList[i].Type)
            {
                AFU_RomFileSLPRHRECList[SLPNumber] = TempRHRECList[i];
                SLPNumber++;
            }
        }

        if (PrevBlockIndex >= TotalBlock)
            bSacnEndFlag = TRUE;
        else
            PrevBlockIndex++;

    }while(!bSacnEndFlag);

    bSacnEndFlag = FALSE;
    PrevBlockIndex = 0;
    MemSet(TempRHRECList, sizeof(ROMHOLEREC)*MAXNCBLK, 0);

    

    do
    {
        j = BootRH;        

        if (AFU_SysRom_GetFVRang(BOOTBLOCK, &StrAddr, &BlockSize, &PrevBlockIndex))
        {
            bUseRomFile = TRUE;
            BootRH += AFU_SysRom_GetRomHole(lpFilebuff, StrAddr, BlockSize, TempRHRECList);
    
            if (BootRH)
                HaveRH++;
        }
        //__AFUFILEDBG(WriteOut("[DBG]BootRH = %d\n", BootRH);)
        for ( ; j < BootRH ; j++)
        {
            if (RH_ROMHOLE == TempRHRECList[j].Type)
            {
                AFU_RomFileRHRECList[AFU_FileRHCnt] = TempRHRECList[j];
                AFU_FileRHCnt++;
            }
            else if (RH_SLP == TempRHRECList[j].Type)
            {
                AFU_RomFileSLPRHRECList[SLPNumber] = TempRHRECList[j];
                SLPNumber++;
            }

        }

        if (PrevBlockIndex >= TotalBlock)
            bSacnEndFlag = TRUE;
        else
            PrevBlockIndex++;

    }while(!bSacnEndFlag);

    //AFU_FileNCBCnt = AFU_SysRom_GetNCBNumber();

    if (AFU_FileRHCnt > MAXNCBLK)
        return FALSE;

    if (2 == SLPNumber) AFU_RomFile_bHaveSLP20 = TRUE;
    
    /*__AFUFILEDBG(
        WriteOut("AFU_FileRHCnt = %d, AFU_FileNCBCnt = %d\n", AFU_FileRHCnt, AFU_FileNCBCnt);
        WriteOut("HaveRH = %d\n", HaveRH);
    )*/
    
    if (AFU_FileRHCnt) return TRUE;

    //if (AFU_FileNCBCnt) return TRUE;

    return FALSE;
}

UINT8   AFU_RomFile_GetRomHoleNumber()
{
    return AFU_FileRHCnt;
}

BOOLEAN AFU_RomFile_HaveSLP20()
{
    return AFU_RomFile_bHaveSLP20;
}

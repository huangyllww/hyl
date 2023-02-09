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
Afusysrom.c

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

#include <Guid/FirmwareFileSystem2.h>
#include <Library/UefiLib.h>

#include "Afusysrom.h"
#include "ofbd_cfg.h"

static BOOLEAN          gAFU_SysRom_bOA31GUIDInFind;///< AFU OA 3.1 GUID in System Rom.
static BOOLEAN          AFU_bSLPIsExist = FALSE;       ///< AFU SLP 2.0 Key is Exist.

static BOOLEAN          AFU_SysRom_bSearchRomHole = FALSE;
static BOOLEAN          AFU_SysRom_bHaveSLP20 = FALSE;  ///< AFU Have SLP 2.0
static INT32            AFU_SysRHCnt;                   ///< AFU System Rom Hole Count.

ROMHOLEREC   AFU_SysRHRECList[MAXNCBLK];            ///< AFU System Rom Rom Hole Record List.
ROMHOLEREC   AFU_SysSLPRHRECList[SLP_NUMBER];       ///< AFU System SLP2.0 Record List.

ROMHOLEREC  *gAFU_SysRom_DMIGuidList_Head = NULL;
ROMHOLEREC  *gAFU_SysRom_DMIGuidList_Current = NULL;
INT32       DmiNumber_SysRom = 0; 
ROMHOLEREC  *gAFU_RomFile_DMIGuidList_Head = NULL;
ROMHOLEREC  *gAFU_RomFile_DMIGuidList_Current = NULL;
INT32       DmiNumber_RomFile = 0;
BOOLEAN     bUseRomFile = FALSE;

extern ROMInfo * gpFlashInfo;

static EFI_GUID FFS_HOLE_GUID_RANGE[] = {
                                 {0x05ca01fc, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_00
                                 {0x05ca01fd, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_01
                                 {0x05ca01fe, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_02
                                 {0x05ca01ff, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_03
                                 {0x05ca0200, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_04
                                 {0x05ca0201, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_05
                                 {0x05ca0202, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_06
                                 {0x05ca0203, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_07
                                 {0x05ca0204, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_08
                                 {0x05ca0205, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_09
                                 {0x05ca0206, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_10
                                 {0x05ca0207, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_11
                                 {0x05ca0208, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_12
                                 {0x05ca0209, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_13
                                 {0x05ca020a, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_14
                                 {0x05ca020b, 0x0fc1, 0x11dc, 0x90, 0x11, 0x00, 0x17, 0x31, 0x53, 0xeb, 0xa8},  //HOLE_15
};

// For SLP keys (OA2.0) EIP 63677
static EFI_GUID FFS_OEM_SLP_PUBLIC_KEY_GUID[] = {{0x996aa1e0, 0x1e8c, 0x4f36, 0xb5, 0x19, 0xa1, 0x70, 0xa2, 0x06, 0xfc, 0x14}};  //HOLE_16   //OEM_SLP_PUBLIC_KEY_GUID
static EFI_GUID FFS_OEM_SLP_MARKER_GUID[] = {{0x69009842, 0x63f2, 0x43db, 0x96, 0x4b, 0xef, 0xad, 0x1c, 0x39, 0xec, 0x85}};  //HOLE_17   //OEM_SLP_MARKER_GUID
                                 
// For OA 3.1 Module Identify to use + EIP 70134
static EFI_GUID EFI_OA31_MODULE_GUID[] = {0xc8ca0bb8, 0x67da, 0x4883, { 0x8c, 0xfc, 0x91, 0x80, 0xcb, 0x9e, 0xec, 0x68 } };

static EFI_GUID EFI_DMI_MODULE_GUID[] = {0xfd44820b, 0xf1ab, 0x41c0, {0xae, 0x4e, 0x0c, 0x55, 0x55, 0x6e, 0xb9, 0xbd}};

/**
 *  Get System Rom FV Rang.
 *
 *  Get System Rom RV Rang.
 *
 *  @param[in] Fvtype FV Type.
 *  @param[out] pFVStAddr n Type Fv Start Address.
 *  @param[out] pFV_AllSize n Type Fv Rang Size.
 *  @retval TRUE Have Type FV.
 *  @retval FALSE Not Have Type FV.
 */     
BOOLEAN AFU_SysRom_GetFVRang(UINT8 Fvtype, UINT32* pFVStAddr, UINT32* pFV_AllSize, UINT16* pPrevBlockIndex)
{
    //__AFUSRDBG(WriteOut("FVRang()\n"));

    UINT32 FV_Start = 0xffffffff;
    UINT32 FV_Size = 0;
    UINT16 TotalBlock = gpFlashInfo->TotalBlocks;
    UINT16 BlockIndex = 0;
    BlockInfo_T* pBlockInfo = (BlockInfo_T*)&(gpFlashInfo->BlockInfo);
    
    for(BlockIndex = (*pPrevBlockIndex) ; BlockIndex < TotalBlock ; BlockIndex++)
    {
        if (pBlockInfo[BlockIndex].Type == Fvtype) //find start and end of these blocks
        {           
            if (FV_Start == 0xffffffff)
                FV_Start = BlockIndex;

            FV_Size += pBlockInfo[BlockIndex].BlockSize;
            
            if(((BlockIndex != TotalBlock - 1) && pBlockInfo[BlockIndex + 1].Type != Fvtype) || (BlockIndex == TotalBlock - 1))
            {
                *pFVStAddr = pBlockInfo[FV_Start].StartAddress;
                *pFV_AllSize = FV_Size;
                *pPrevBlockIndex = BlockIndex;
                return TRUE;
            }
        }
    }

    *pPrevBlockIndex = BlockIndex;
    
    return FALSE;
}

BOOLEAN AFU_SysRom_FV_FFS_DMIGUID(LPFFSHDR lpFfsHand)
{
    EFI_GUID* Guid = &(lpFfsHand->Name);
/*
__AFUSRDBG(
    WriteOut("System GUID:");
    PrintGuid2((UINT8*)Guid);
    WriteOut("DMI GUID :");
    PrintGuid2((UINT8*)(&EFI_DMI_MODULE_GUID));
)
*/
    EFI_GUID* DMI_Guid = EFI_DMI_MODULE_GUID;

    if (0 == MemCmp(Guid, DMI_Guid, sizeof(EFI_GUID)))
    {
//__AFUSRDBG(WriteOut("EFI_DMI_MODULE_GUID : TRUE\n");)
        return TRUE;
    }

//__AFUSRDBG(WriteOut("EFI_DMI_MODULE_GUID : FALSE\n");)
    return FALSE;    
}

BOOLEAN AFU_SysRom_FV_FFS_OA31KEYGUID(LPFFSHDR lpFfsHand)
{
    EFI_GUID* Guid = &(lpFfsHand->Name);
/*
__AFUSRDBG(
    WriteOut("System GUID:");
    PrintGuid2((UINT8*)Guid);
    WriteOut("OA3.1 GUID :");
    PrintGuid2((UINT8*)(&EFI_OA31_MODULE_GUID));
)
*/
    EFI_GUID* OA31_Guid = EFI_OA31_MODULE_GUID;

    if (0 == MemCmp(Guid, OA31_Guid, sizeof(EFI_GUID)))
    {
//__AFUSRDBG(WriteOut("EFI_OA31_MODULE_GUID : TRUE\n");)
        return TRUE;
    }

//__AFUSRDBG(WriteOut("EFI_OA31_MODULE_GUID : FALSE\n");)
    return FALSE;
}

/**
 *  Check FFS GUID Rang
 *
 *  Check FFS GUID In ROM Hole GUID List.
 *
 *  @param[in] lpFfsHand FFS Hand
 *  @retval TRUE Rang is ok.
 *  @retval FALSE Rang is fail.
 */     
INT32 AFU_SysRom_FV_FFS_GUIDRang(LPFFSHDR lpFfsHand)
{
    //_FVDBUG(WriteOut("ChkFFSGUIDRang()\n"));
    int i;

    EFI_GUID* guid = &(lpFfsHand->Name);    
    
    if ((guid->Data1 >= FFS_HOLE_GUID_RANGE[0].Data1 ) &&
        (guid->Data1 <= FFS_HOLE_GUID_RANGE[15].Data1 ))
    {
        for (i = 0 ; i < 16 ; i++)
        {
            if (0 == MemCmp(guid, &(FFS_HOLE_GUID_RANGE[i]), sizeof(EFI_GUID)))
            {   
                return RH_ROMHOLE;
            }
        }
    }
    
    // Check SLP 2.0 keys (OA2.0)
    if (0 == MemCmp(guid, &(FFS_OEM_SLP_PUBLIC_KEY_GUID[0]), sizeof(EFI_GUID)))
    {
        //AFU_SetSLPKeyIsExist(TRUE);
        AFU_bSLPIsExist = TRUE;
        return RH_SLP;
    }

    // Check SLP 2.0 keys (OA2.0)
    if (0 == MemCmp(guid, &(FFS_OEM_SLP_MARKER_GUID[0]), sizeof(EFI_GUID)))
    {
        //AFU_SetSLPKeyIsExist(TRUE);
        AFU_bSLPIsExist = TRUE;
        return RH_SLP;
    }

    return RH_NULL;
}

VOID PrintGuid2(UINT8 *pGuid)
{
    EFI_GUID *RomGuid = (EFI_GUID*) pGuid;

    if (RomGuid == NULL) return;

    Print(L"%08lx-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n",
              RomGuid->Data1,
              RomGuid->Data2,RomGuid->Data3,
              RomGuid->Data4[0],RomGuid->Data4[1],RomGuid->Data4[2],RomGuid->Data4[3],
              RomGuid->Data4[4],RomGuid->Data4[5],RomGuid->Data4[6],RomGuid->Data4[7]);
}

DWORD AFU_SysRom_GetRomHole(PBYTE lpSysbuff, DWORD StartAddr, DWORD BlockSize, LPROMHOLEREC lpRomHoleRec)
{
    PBYTE pBuffer;
    PBYTE lpTempBuf;
    DWORD offset = 0;   
    DWORD FFS_Size = 0; 
    DWORD VolLength;
    DWORD i;
    DWORD count = 0;
    FVHDR   FvHand;
    FFSHDR  FfsHand;
    WORD Data[sizeof(FVHDR)/2];
    WORD Checksum = 0;
    int iii=0;
    DWORD OffsetForFVH = 0;
    DWORD SearchOffset=0;   
    DWORD TempMaxBlkSize;

    EFI_GUID gEFI_FFS_GUID = EFI_FFS_GUID;
    EFI_GUID gEFI_FFS_GUID2 = EFI_FIRMWARE_FILE_SYSTEM2_GUID;

    INT32 HoleType;
    
    LPROMHOLEREC lpDMIFFS = (LPROMHOLEREC)NULL;
    
    pBuffer = lpSysbuff;

    offset = StartAddr;
    TempMaxBlkSize = AFU_MAXBLOCKSIZE;
    lpTempBuf = AllocMem(TempMaxBlkSize);
    
    if (!lpTempBuf) return FALSE;

    for(OffsetForFVH = StartAddr; OffsetForFVH < StartAddr+BlockSize;)
    {
        if((OffsetForFVH + TempMaxBlkSize) > StartAddr + BlockSize )
            TempMaxBlkSize = StartAddr + BlockSize - OffsetForFVH;      

        MemCpy(lpTempBuf, pBuffer + OffsetForFVH, TempMaxBlkSize);

        SearchOffset = 0;
        while(SearchOffset < TempMaxBlkSize)
        {
            //MemContent = *( ((DWORD *)(lpTempBuf+SearchOffset)) );
            //if(MemContent==0x4856465f)  //0x5f465648 = _FVH
            if (0 == MemCmp (lpTempBuf + SearchOffset, "_FVH", 4))
            {
                //check FVH checksum
                SearchOffset -= 40;

                MemCpy(&FvHand, pBuffer + OffsetForFVH + SearchOffset, sizeof(FVHDR));
                
                //check for valid checksum
                MemCpy((PBYTE)Data, &FvHand, sizeof(FVHDR));

                for(iii = 0 ; iii < (sizeof(FVHDR)/2) ; iii++)
                    Checksum += Data[iii];

                if(Checksum != 0)
                {
                    SearchOffset += 44;//StartAddr search again beyond the last FVH found
                    continue;
                }

                offset = OffsetForFVH + SearchOffset;
                break;
            }
            else
                SearchOffset++;
        }
        //EOF encounter        
        if(SearchOffset >= TempMaxBlkSize)
        {
            OffsetForFVH += TempMaxBlkSize;
            continue;
        }

        //check for FFS file system GUID
        if ((MemCmp(&(FvHand.FileSystemGuid), &gEFI_FFS_GUID, sizeof(EFI_GUID)) != 0) &&
            (MemCmp(&(FvHand.FileSystemGuid), &gEFI_FFS_GUID2, sizeof(EFI_GUID)) != 0))
        {
            offset += FvHand.FvLength;
            OffsetForFVH += FvHand.FvLength;
            continue;
        }
        
        offset += FvHand.HeaderLength;
        VolLength = FvHand.FvLength;

        for(i = FvHand.HeaderLength ; i < VolLength ; )
        {
            MemCpy(&FfsHand, pBuffer + offset, sizeof(FFSHDR));

            if (!gAFU_SysRom_bOA31GUIDInFind)
            {
                if (AFU_SysRom_FV_FFS_OA31KEYGUID(&FfsHand))
                {
                    gAFU_SysRom_bOA31GUIDInFind = TRUE;
                }
            }

            if (AFU_SysRom_FV_FFS_DMIGUID(&FfsHand))
            {
                if (bUseRomFile)
                {
                    lpDMIFFS = (LPROMHOLEREC)AllocMem(sizeof(ROMHOLEREC));
                    lpDMIFFS->RomOffset = offset;
                    lpDMIFFS->Size = (*(UINT32 *)FfsHand.Size) & 0xffffff;
                    if (NULL == gAFU_RomFile_DMIGuidList_Head)
                    {
                        gAFU_RomFile_DMIGuidList_Head = lpDMIFFS;
                        gAFU_RomFile_DMIGuidList_Current = lpDMIFFS;
                    }
                    else
                    {
                        gAFU_RomFile_DMIGuidList_Current->Next = lpDMIFFS;
                        gAFU_RomFile_DMIGuidList_Current = lpDMIFFS;
                    }
                    DmiNumber_SysRom += 1;
                }
                else
                {
                    lpDMIFFS = (LPROMHOLEREC)AllocMem(sizeof(ROMHOLEREC));
                    lpDMIFFS->RomOffset = offset;
                    lpDMIFFS->Size = (*(UINT32 *)FfsHand.Size) & 0xffffff;
                    if (NULL == gAFU_SysRom_DMIGuidList_Head)
                    {
                        gAFU_SysRom_DMIGuidList_Head = lpDMIFFS;
                        gAFU_SysRom_DMIGuidList_Current = lpDMIFFS;
                    }
                    else
                    {
                        gAFU_SysRom_DMIGuidList_Current->Next = lpDMIFFS;
                        gAFU_SysRom_DMIGuidList_Current = lpDMIFFS;
                    }
                    DmiNumber_RomFile += 1;
                }
            }
            
            HoleType = AFU_SysRom_FV_FFS_GUIDRang(&FfsHand);

            if (RH_ROMHOLE == HoleType)
            {
                MemCpy(&(lpRomHoleRec[count].Guid),&(FfsHand.Name),sizeof(EFI_GUID));   
                lpRomHoleRec[count].Size = (*(UINT32 *)FfsHand.Size) & 0xffffff;
                lpRomHoleRec[count].RomOffset = offset;
                lpRomHoleRec[count].Type = RH_ROMHOLE;                                
                
                //__AFUSRDBG(
                 /*   
                  * Print(L"Rom Hole Count : %d\n", count);
                    Print(L"Rom Hole GUID : ");
                    PrintGuid2((UINT8*)&(lpRomHoleRec[count].Guid));
                    Print(L"lpRomHoleRec[count].Size = %X\n", lpRomHoleRec[count].Size);
                    Print(L"lpRomHoleRec[count].RomOffset = %X\n", lpRomHoleRec[count].RomOffset);
                    Print(L"lpRomHoleRec[count].Type = %X\n", lpRomHoleRec[count].Type);
                    */
                //)
                count++;
                
                if (count >= MAXNCBLK)
                    break;
            }
            else if (RH_SLP == HoleType)
            {
                MemCpy(&(lpRomHoleRec[count].Guid),&(FfsHand.Name),sizeof(EFI_GUID));
                lpRomHoleRec[count].Size = (*(UINT32 *)FfsHand.Size) & 0xffffff;
                lpRomHoleRec[count].RomOffset = offset;
                lpRomHoleRec[count].Type = RH_SLP;

                //__AFUSRDBG(
                /*
                    Print(L"Rom Hole Count : %d\n", count);
                    Print(L"Rom Hole GUID : ");
                    PrintGuid2((UINT8*)&(lpRomHoleRec[count].Guid));
                    Print(L"lpRomHoleRec[count].Size = %X\n", lpRomHoleRec[count].Size);
                    Print(L"lpRomHoleRec[count].RomOffset = %X\n", lpRomHoleRec[count].RomOffset);
                    Print(L"lpRomHoleRec[count].Type = %X\n", lpRomHoleRec[count].Type);
                    */
                //)
                count++;
                
                if (count >= MAXNCBLK)
                    break;            
            }

            FFS_Size = (*(UINT32 *)FfsHand.Size) & 0xffffff;
            FFS_Size = (FFS_Size + 7) & ~7;

            // if(FFS_Size != 0xffffff)
            // If rom NULL data is 0x00 not 0xff
            if((FFS_Size != 0xffffff) && (FFS_Size != 0x0))
            {                
                offset += FFS_Size;
                i += FFS_Size;
            }
            else
            {
                i = VolLength;                
            }
        }

        OffsetForFVH += VolLength;
        offset = OffsetForFVH ;
    }
    
    FreeMem(lpTempBuf);

    return count;
}

/**
 *  Search NCB
 *
 *  Search System Rom NCB
 *
 *  @param[in] lpSysbuff System buffer. 
 *  @retval TRUE Have NCB.
 *  @retval FALSE NO NCB.
 */
BOOLEAN            AFU_SysRom_SearchNCB(PBYTE lpSysbuff)
{
    UINT32  StrAddr = 0;
    UINT32  BlockSize = 0;
    DWORD   MainRH = 0;
    DWORD   BootRH = 0;
    UINT16  PrevBlockIndex = 0;
    UINT16  TotalBlock = gpFlashInfo->TotalBlocks;
    BOOLEAN    bSacnEndFlag = FALSE;
    UINT8   SLPNumber = 0;
    
    DWORD i = 0;
    DWORD j = 0;
    
    ROMHOLEREC  TempRHRECList[MAXNCBLK] = {0};
    
    AFU_SysRom_bSearchRomHole = FALSE;
    AFU_SysRHCnt = 0;
    AFU_SysRom_bHaveSLP20 = FALSE;

    //__AFUSRDBG(WriteOut("AFU_SysRom_SearchNCB\n");)

    

    do
    {
        i = MainRH;
        //__AFUSRDBG(WriteOut("PrevBlockIndex = %d, StrAddr = %x, BlockSize = %x\n", PrevBlockIndex, StrAddr, BlockSize));

        if (AFU_SysRom_GetFVRang(MAINBIOS, &StrAddr, &BlockSize, &PrevBlockIndex))
        {
        //__AFUSRDBG(WriteOut("PrevBlockIndex = %d, StrAddr = %x, BlockSize = %x\n", PrevBlockIndex, StrAddr, BlockSize));
            bUseRomFile = FALSE;
            MainRH += AFU_SysRom_GetRomHole(lpSysbuff, StrAddr, BlockSize, TempRHRECList);          
        }
    
        for ( ; i < MainRH ; i++)
        {
            if (RH_ROMHOLE == TempRHRECList[i].Type)
            {
                AFU_SysRHRECList[AFU_SysRHCnt] = TempRHRECList[i];
                AFU_SysRHCnt++;
            }
            else if (RH_SLP == TempRHRECList[i].Type)
            {
                AFU_SysSLPRHRECList[SLPNumber] = TempRHRECList[i];
                SLPNumber++;
            }
        }
    
        //__AFUSRDBG(WriteOut("Main Block AFU_RHCount = %d\n", MainRH);)

        if (PrevBlockIndex >= TotalBlock)
            bSacnEndFlag = TRUE;
        else
            PrevBlockIndex++;

    } while(!bSacnEndFlag);
    
    bSacnEndFlag = FALSE;
    PrevBlockIndex = 0;
    MemSet(TempRHRECList, sizeof(ROMHOLEREC)*MAXNCBLK, 0);



    do
    {
        j = BootRH;

        //__AFUSRDBG(WriteOut("PrevBlockIndex = %d, StrAddr = %x, BlockSize = %x\n", PrevBlockIndex, StrAddr, BlockSize));

        if (AFU_SysRom_GetFVRang(BOOTBLOCK, &StrAddr, &BlockSize, &PrevBlockIndex))
        {
        //__AFUSRDBG(WriteOut("PrevBlockIndex = %d, StrAddr = %x, BlockSize = %x\n", PrevBlockIndex, StrAddr, BlockSize));
            bUseRomFile = FALSE;
            BootRH += AFU_SysRom_GetRomHole(lpSysbuff, StrAddr, BlockSize, TempRHRECList);          
        }
    
        for ( ; j < BootRH ; j++)
        {
            if (RH_ROMHOLE == TempRHRECList[j].Type)
            {
                AFU_SysRHRECList[AFU_SysRHCnt] = TempRHRECList[j];
                AFU_SysRHCnt++;
            }
            else if (RH_SLP == TempRHRECList[j].Type)
            {
                AFU_SysSLPRHRECList[SLPNumber] = TempRHRECList[j];
                SLPNumber++;
            }
        }
    
        //__AFUSRDBG(WriteOut("Boot Block AFU_RHCount = %d\n", BootRH);)        

        if (PrevBlockIndex >= TotalBlock)
            bSacnEndFlag = TRUE;
        else
            PrevBlockIndex++;

    }while(!bSacnEndFlag);

    AFU_SysRom_bSearchRomHole = TRUE;

    if (2 == SLPNumber) AFU_SysRom_bHaveSLP20 = TRUE;

    //AFU_SysRHCnt = MainRH + BootRH;
    //AFU_SysNCBCnt = AFU_SysRom_GetNCBNumber();

    //if ((AFU_SysRHCnt + AFU_SysNCBCnt) > MAXNCBLK) return FALSE;

    if (AFU_SysRHCnt) return TRUE;

    //if (AFU_SysNCBCnt) return TRUE;

    return FALSE;
}

UINT8   AFU_SysRom_GetRomHoleNumber()
{
    return AFU_SysRHCnt;
}

BOOLEAN AFU_SysRom_HaveSLP20()
{
    return AFU_SysRom_bHaveSLP20;
}

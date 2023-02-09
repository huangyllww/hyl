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
SmiInterface.c

This is the SMI flash function file for X64.

*/
//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
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

#include <Protocol/SimpleFileSystem.h>
#include <Library/FileHandleLib.h>

#include "basictypes.h"
#include "AfuDriver.h"
#include "SmiInterface.h" 
#include "MeInterface.h"
#include "ofbd_cfg.h"



#define PLDMDATA_FILENAME "PldmData_AfuP.bin"
#define PLDMDATA_FILENAME_WIDTH L"PldmData_AfuP.bin"

static BOOLEAN  SmiFlashEnabled = FALSE;
UINT8 * gpIntrBuf = (UINT8*)NULL;
UINTN   gIntrBufPhyAddr = 0;
ROMInfo * gpFlashInfo = 0;        // pointer to flash information structure
UINT32  gFlashBlockSize = 0;

UINT8 * gpMEBuff = (UINT8 *)NULL;
UINT32  gMESize = 0;
UINT32  gMERealFlashSize = 0;

UINT32  gRomSize = 0;

BYTE    g_OnBoardROMProjectIDLength = 0;
BOOLEAN g_OnBoardRomIsAptioV = FALSE;

BOOLEAN gReStoreSMBIOS = FALSE;
BOOLEAN gReStoreSETUP = FALSE;

static UINT32   BbStartAddress = 0xFFFFFFFF;
static UINT32   MbStartAddress = 0xFFFFFFFF;
static UINT32   NvRamStartAddress = 0xFFFFFFFF;
static UINT32   EcStartAddress = 0xFFFFFFFF;
static UINT32   OA3StartAddress = 0xFFFFFFFF;
static UINT32   OA3BlockSize = 0;
static BOOLEAN  bIsGotBiosInfo = FALSE;

static NCBREC   AFU_SysNCBRecList[MAXNCBLK];    ///< AFU System NCB Record List.
static INT32    AFU_SysNCBCnt;                  ///< AFU System NCB Count.

BOOLEAN         gRHFF_FV_BootEnable;
BOOLEAN         gRHFF_FV_MainEnable;


extern ROMHOLEREC   AFU_SysRHRECList[MAXNCBLK];
extern ROMHOLEREC   AFU_RomFileRHRECList[MAXNCBLK];

extern ROMHOLEREC   AFU_SysSLPRHRECList[SLP_NUMBER];       ///< AFU System SLP2.0 Record List.
extern ROMHOLEREC   AFU_RomFileSLPRHRECList[SLP_NUMBER];   ///< AFU Rom File SLP2.0 Record List.

extern ROMHOLEREC  *gAFU_SysRom_DMIGuidList_Head;
extern ROMHOLEREC  *gAFU_SysRom_DMIGuidList_Current;
extern INT32        DmiNumber_SysRom;
extern ROMHOLEREC  *gAFU_RomFile_DMIGuidList_Head;
extern ROMHOLEREC  *gAFU_RomFile_DMIGuidList_Current;
extern INT32        DmiNumber_RomFile;

extern FUNC_BLOCK  *gpFuncBlock ;
extern UINTN pPhyAddr;
extern UINT32 BootBlockSize;
extern UINT32 MainBlockSize;
extern UINT32 NvRamSize;
extern UINT32 EcBlockSize;

extern UINT32 gSmiMailBoxAddress;
extern UC2SmiMailbox gSmiMailBox;
extern BOOLEAN gMEEnabled; // EIP 377144 +

extern BOOLEAN  AFU_bMLAP_APL_Enable;   ///< AFU Acpi Table MLAP string is exist. and value = 0 APL platform

extern BOOLEAN CallBackExecuteFrequency( UINTN MicroSecond, UINT64 Ticker );
extern EFI_STATUS FileSizeCheck( UINT32 FileSize , UINT32 *FlashCapacity );

extern EFI_STATUS GetFlashPolicy(IN  FLASH_POLICY_INFO_BLOCK     *pFlashPolicy);
extern EFI_STATUS LoadFirmwareImage(IN  FUNC_BLOCK      *pFuncBlock);
extern EFI_STATUS SetFlashMethod_Internal(IN  FUNC_FLASH_SESSION_BLOCK        *pFlashSessionBlock);
extern VOID    AFU_ACPITable_CheckFlow();

FLASH_POLICY_INFO_BLOCK     gFlashPolicy ;
FUNC_BLOCK  *pFuncBlock = NULL;
FUNC_FLASH_SESSION_BLOCK  gFlashSessionBlock;

typedef VOID (BIOS_Pre_Update) (UINT32 Flag);
typedef VOID (BIOS_End_Update) (UINT32 Flag);
extern BIOS_Pre_Update* BIOSPreUpdate[];
extern BIOS_End_Update* BIOSEndUpdate[];

/**
    SmiAllocateMemory()
    
    This function allocates buffer for SMI.
    
    @param VOID
    
    @retval VOID
*/
void SmiAllocateMemory()
{
    UC2RegBuffer regBuffer;
    UC2SmiMailbox * pMailBox = (UC2SmiMailbox *)NULL;

    MemSet(&regBuffer, sizeof(UC2RegBuffer), 0);
        
    regBuffer.Ecx = 0xC0000001; 
    regBuffer.Esi = FLASH_PAGE_SIZE * FLASH_PAGE_COUNT;
    regBuffer.Edi = 0x00;
    

    OutPortByte(SW_SMI_IO_ADDRESS, 0xD9, &regBuffer);


    if(regBuffer.Ecx==0)
    {
        gSmiMailBoxAddress = regBuffer.Esi;
        pMailBox = (UC2SmiMailbox *)((PBYTE)regBuffer.Esi);
        MemCpy(&gSmiMailBox, pMailBox, sizeof(UC2SmiMailbox));
    }
    else
        gSmiMailBoxAddress = 0;
    

    if (gSmiMailBoxAddress)
    {
        gpIntrBuf = (UINT8*)((PBYTE)pMailBox->PhysicalAddress);
        MemSet(pMailBox, sizeof(UC2SmiMailbox), 0);
    }
    else
    {
        gpIntrBuf = MallocZ(FLASH_PAGE_SIZE * FLASH_PAGE_COUNT);
    }
    
    gIntrBufPhyAddr = (UINTN)gpIntrBuf;
    pPhyAddr = gIntrBufPhyAddr;
    MemSet(gpIntrBuf, FLASH_PAGE_SIZE * FLASH_PAGE_COUNT, 0);
    
    return;
    
}

/**
    SmiFreeMemory()
    
    This function frees buffer.
    
    @param VOID
    
    @retval VOID
*/
void SmiFreeMemory()
{
    UC2RegBuffer regBuffer;
    UC2SmiMailbox * pMailBox = (UC2SmiMailbox *)NULL; 
    
    if(gSmiMailBoxAddress)
    {
        pMailBox = (UC2SmiMailbox *)gSmiMailBoxAddress;
        
        MemCpy(pMailBox, &gSmiMailBox, sizeof(UC2SmiMailbox));
    
        MemSet(&regBuffer, sizeof(UC2RegBuffer), 0);
    
        regBuffer.Ecx = 0xC0000002; 
        OutPortByte(SW_SMI_IO_ADDRESS, 0xD9, &regBuffer);
        
        gSmiMailBoxAddress = 0;   
        gpIntrBuf = NULL;
        
        return;
    }
    else    
        pBS->FreePool((PBYTE)pPhyAddr);
}

/**
    SendSmi()
    
    This function issues SMI.
    
    @param SMINumber - 
    
    @retval VOID
*/
void SendSmi( UINT8 SMINumber )
{
    UC2RegBuffer    regBuffer;  
    
    MemSet(&regBuffer, sizeof(UC2RegBuffer), 0);
    
    regBuffer.Ebx = (UINT32)(gIntrBufPhyAddr & 0xffffffff);
    regBuffer.Ecx = (UINT32)(gIntrBufPhyAddr >> 32);            
    
    OutPortByte(SW_SMI_IO_ADDRESS, SMINumber, &regBuffer);
}

/**
    EnableFlashIfc()
    
    This function enables flash interface.
    
    @param VOID 
    
    @retval VOID
*/
int EnableFlashIfc()
{
    
    ((FUNC_BLOCK *)gpIntrBuf)->ErrorCode = 1;
    
    if(SmiFlashEnabled){
        return 0;
    }
    
    SendSmi(SMIFLASH_ENABLE_FLASH);
    
    switch (((FUNC_BLOCK *)gpIntrBuf)->ErrorCode) {
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 3;
            break;
    }
    
    SmiFlashEnabled = TRUE;
    
    return 0;
}

/**
    DisableFlashIfc()
    
    This function disables flash interface.
    
    @param VOID 
    
    @retval VOID
*/
int DisableFlashIfc()
{
    
    ((FUNC_BLOCK *)gpIntrBuf)->ErrorCode = 0;
    
    if(!SmiFlashEnabled){
        return 0;
    }
    
    SendSmi(SMIFLASH_DISABLE_FLASH);

    switch (((FUNC_BLOCK *)gpIntrBuf)->ErrorCode) {
        case 1:
            return 1;
            break;
        case 2:
            return 2;
            break;
        case 3:
            return 3;
            break;
    }
    
    SmiFlashEnabled = FALSE;
    
    return 0;
}

/**
    EraseBlock()
    
    This function erases the ROM by block.
    
    @param BlockRegionOffset - The FV address.
    @param BlockRegionSize - The size of FV.
    @param Section - The FV type.
    @param Frequency - The executed frequency of call back function.
    @param CallBackFunc - The call back function.
    
    @retval EFI_STATUS
*/
EFI_STATUS EraseBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc )
{
    UINT32 BlockSize;
    UINT32 Offset;
    UINT64 Timer;
    UINTN Second;
    
    
    if(NULL != CallBackFunc)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }
    
    //Erase flashpart by SMI
    //Each 4K size, erase once
    BlockSize = SmiBufferBlockSize;
    
    for(Offset = 0; Offset < BlockRegionSize; Offset += BlockSize)  
    {
        if ( BlockRegionSize < (Offset + BlockSize)){
            BlockSize = BlockRegionSize - Offset;
        }

        gpFuncBlock = (FUNC_BLOCK *)gpIntrBuf;
        gpFuncBlock->ErrorCode = 0xFF;
        gpFuncBlock->BlockAddr = BlockRegionOffset + Offset;
        gpFuncBlock->BlockSize = BlockSize;
        
        SendSmi(SMIFLASH_ERASE_FLASH);
        
        if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            CallBackFunc((UINT64)(BlockRegionOffset + Offset),(BlockRegionOffset + BlockRegionSize),(UINT64)BlockRegionSize,(UINTN)(Section | ERASE_BLOCK));
        }

        if (gpFuncBlock->ErrorCode != 0)
        {   
            return EFI_DEVICE_ERROR;
        }
    }   
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | ERASE_BLOCK));
    
    return EFI_SUCCESS;
}

/**
    ReadBlock()
    
    This function reads the rom by block.
    
    @param BlockRegionOffset - The FV address.
    @param BlockRegionSize - The size of FV.
    @param *DataBuffer - The data buffer.
    @param Section - The FV type.
    @param Frequency - The executed frequency of call back function.
    @param CallBackFunc - The call back function.
    
    @retval EFI_STATUS
*/
EFI_STATUS ReadBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINT8 *DataBuffer, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc )
{
    UINT32 BlockSize;
    UINT32 Offset;
    UINT64 Timer;
    UINTN Second;
        
    if(NULL != CallBackFunc)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }
    
    BlockSize = SmiBufferBlockSize;

    for(Offset = 0; Offset < BlockRegionSize; Offset += BlockSize)  
    {
        if ( BlockRegionSize < (Offset + BlockSize)){
            BlockSize = BlockRegionSize - Offset;
        }
    
        gpFuncBlock = (FUNC_BLOCK *)gpIntrBuf;
        gpFuncBlock->BufAddr = (UINT64)gpIntrBuf + sizeof(FUNC_BLOCK);
        gpFuncBlock->BlockAddr = BlockRegionOffset + Offset;
        gpFuncBlock->BlockSize = BlockSize;
        gpFuncBlock->ErrorCode = 0xFF;

        SendSmi(SMIFLASH_READ_FLASH);
        
        /*MemCpy((VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), (VOID *)(DataBuffer + BlockRegionOffset + Offset), BlockSize);
        
        SendSmi(SMIFLASH_WRITE_FLASH);*/
        
        MemCpy((VOID *)(DataBuffer + BlockRegionOffset + Offset), (VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), BlockSize);

        if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            CallBackFunc((UINT64)(BlockRegionOffset + Offset),(BlockRegionOffset + BlockRegionSize),(UINT64)BlockRegionSize,(UINTN)(Section | READ_BLOCK));
        }
        
        if (gpFuncBlock->ErrorCode != 0)
        {
            DisableFlashIfc();
            return EFI_DEVICE_ERROR;        
        }
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | READ_BLOCK));
    
    return EFI_SUCCESS;
}

/**
    WriteBlock()
    
    This function writes the ROM by block.
    
    @param BlockRegionOffset - The FV address.
    @param BlockRegionSize - The size of FV.
    @param *DataBuffer - The ROM file data buffer.
    @param Section - The FV type.
    @param Frequency - The executed frequency of call back function.
    @param CallBackFunc - The call back function.
    
    @retval EFI_STATUS
*/
EFI_STATUS WriteBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINT8 *DataBuffer, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc )
{
    UINT32 BlockSize;
    UINT32 Offset;
    UINT64 Timer;
    UINTN Second;

    if(NULL != CallBackFunc)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }
    
    //Write flashpart by SMI
    //Each 4K size, write once
    BlockSize = SmiBufferBlockSize;

    for(Offset = 0; Offset < BlockRegionSize; Offset += BlockSize)  
    {
        if ( BlockRegionSize < (Offset + BlockSize)){
            BlockSize = BlockRegionSize - Offset;
        }
        gpFuncBlock = (FUNC_BLOCK *)gpIntrBuf;
        gpFuncBlock->BufAddr = (UINT64)gpIntrBuf + sizeof(FUNC_BLOCK);
        gpFuncBlock->BlockAddr = BlockRegionOffset + Offset;
        gpFuncBlock->BlockSize = BlockSize;
        gpFuncBlock->ErrorCode = 0xFF;

        if (Section & FLASH_OA3_BLOCK)
            MemCpy((VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), (VOID *)(DataBuffer + Offset), BlockSize);
        else
            MemCpy((VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), (VOID *)(DataBuffer + BlockRegionOffset + Offset), BlockSize);
        
        SendSmi(SMIFLASH_WRITE_FLASH);
        
        if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            CallBackFunc((UINT64)(BlockRegionOffset + Offset),(BlockRegionOffset + BlockRegionSize),(UINT64)BlockRegionSize,(UINTN)(Section | WRITE_BLOCK));
        }
        
        if (gpFuncBlock->ErrorCode != 0)
        {
            DisableFlashIfc();
            return EFI_DEVICE_ERROR;        
        }
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | WRITE_BLOCK));
    
    return EFI_SUCCESS;
}



/**
    VerifyBlock()
    
    This function reads the rom by block.
    
    @param BlockRegionOffset - The FV address.
    @param BlockRegionSize - The size of FV.
    @param *DataBuffer - The data buffer.
    @param Section - The FV type.
    @param Frequency - The executed frequency of call back function.
    @param CallBackFunc - The call back function.
    
    @retval EFI_STATUS
*/
EFI_STATUS VerifyBlock( UINT32 BlockRegionOffset, UINT32 BlockRegionSize, UINT8 *DataBuffer, UINTN Section, UINTN Frequency, CallBackFn_T CallBackFunc )
{
    UINT32 BlockSize;
    UINT32 Offset;
    UINT64 Timer;
    UINTN Second;
    INTN Result;
    
    if(NULL != CallBackFunc)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }
    
    BlockSize = SmiBufferBlockSize;

    for(Offset = 0; Offset < BlockRegionSize; Offset += BlockSize)  
    {
        if ( BlockRegionSize < (Offset + BlockSize)){
            BlockSize = BlockRegionSize - Offset;
        }
    
        gpFuncBlock = (FUNC_BLOCK *)gpIntrBuf;
        gpFuncBlock->BufAddr = (UINT64)gpIntrBuf + sizeof(FUNC_BLOCK);
        gpFuncBlock->BlockAddr = BlockRegionOffset + Offset;
        gpFuncBlock->BlockSize = BlockSize;
        gpFuncBlock->ErrorCode = 0xFF;

        SendSmi(SMIFLASH_READ_FLASH);
        
        if (Section & FLASH_OA3_BLOCK)
            Result = MemCmp((VOID *)(DataBuffer + Offset) , (VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), BlockSize);
        else
            Result = MemCmp((VOID *)(DataBuffer + BlockRegionOffset + Offset) , (VOID *)(gpIntrBuf + sizeof(FUNC_BLOCK)), BlockSize);
      
        
        if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            CallBackFunc((UINT64)(BlockRegionOffset + Offset),(BlockRegionOffset + BlockRegionSize),(UINT64)BlockRegionSize,(UINTN)(Section | VERIFY_BLOCK));
        }
        
        if ((gpFuncBlock->ErrorCode != 0) || (Result != 0))
        {
            DisableFlashIfc();
            return EFI_DEVICE_ERROR;        
        }
        
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | VERIFY_BLOCK));
    
    return EFI_SUCCESS;
}

VOID RomInfoEx2RomInfo(ROMInfoEX* pRomInfoEX, BlockInfo_T* pBlockInfo)
{
    UINT32 TotalBlock = 0;
    UINT32 i = 0,j = 0;
    BlockInfo_T* pRomInfoExBlockInfo = &(pRomInfoEX->BlockInfo);
    
    UINT32 BlockIndexSt = 0;
    UINT32 BlockIndexEd;
    UINT32 Basecnt;
        
    TotalBlock = pRomInfoEX->TotalBlocks;
            
    for (i = 0 ; i < TotalBlock ; i++)
    {
        BlockIndexEd = BlockIndexSt + pRomInfoExBlockInfo[i].BlockSize / pRomInfoEX->BaseBlockSize;
        Basecnt = BlockIndexSt;
        
        for(j = BlockIndexSt ; j < BlockIndexEd ; j++)
        {           
            pBlockInfo[j].StartAddress = pRomInfoExBlockInfo[i].StartAddress + (pRomInfoEX->BaseBlockSize * (j - Basecnt));
            pBlockInfo[j].BlockSize = pRomInfoEX->BaseBlockSize;
            pBlockInfo[j].Type = pRomInfoExBlockInfo[i].Type;
            
            BlockIndexSt++;
        }       
    }
    
    gpFlashInfo->TotalBlocks = pRomInfoEX->BiosRomSize / pRomInfoEX->BaseBlockSize;
    gFlashBlockSize = pRomInfoEX->BaseBlockSize;
    
}

VOID GetSystemROMIDLength(PBYTE pBuffer, DWORD ddSize)
{
    DWORD i;
    g_OnBoardROMProjectIDLength = 0xFF;
    
    for (i = 0; i < ddSize - 4; i++) 
    {
        if (0 == MemCmp (pBuffer + i, "$BPT", 4)) 
        {
            // Get $BPT information ...
            g_OnBoardROMProjectIDLength = *(BYTE*)(&pBuffer[i+4]);
            
            g_OnBoardRomIsAptioV = FALSE;

            // Get AptioV
            if (i < ddSize - 5)
            {
                if (0 == MemCmp(pBuffer + i + 5, "V", 1))
                {
                    g_OnBoardRomIsAptioV = TRUE;                
                }
                
                /*if (g_OnBoardRomIsAptioV)
                    WriteOut("AptioV = TRUE\n");    
                else
                    WriteOut("AptioV = FALSE\n");*/
            }

            
            break;
        }
    }
}

void GetBiosInfo()
{
    ROMInfoEX   *pRomInfoEX = 0;
    ROMInfo     *pRomInfo = (ROMInfo *)NULL;
    BlockInfo_T *pBlkInfo = (BlockInfo_T *)NULL;
    UINT32      RomSize = 0;
    UINT32      i = 0;
    
    DWORD       LastType = 0;
    
    int         FunctionRes = 0;
    
    UINT32      BlockCount, FlashInfoSize;
    
    BOOLEAN  bOnBoardRomIsLarge = FALSE;
    
    if(bIsGotBiosInfo)
        return; 
    
    pRomInfo = (ROMInfo *)gpIntrBuf;

    pRomInfo->Length    = FLASH_PAGE_SIZE * FLASH_PAGE_COUNT;
    pRomInfo->Implemented   = TRUE;
    
    SendSmi(SMIFLASH_GET_FLASH_INFO);   //SMI 0x25 to get bios info
    
    if (0 != (pRomInfo)->Implemented)
    {
        if (14 == (pRomInfo)->Version)
        {
            pRomInfoEX = (ROMInfoEX *)gpIntrBuf;
            pRomInfoEX->Version = 14;
            SendSmi(SMIFLASH_GET_FLASH_INFO);
            
            if (0 == pRomInfoEX->Implemented)
            {
                bOnBoardRomIsLarge = TRUE;
            }
        }
    }
    
    if ((bOnBoardRomIsLarge)&&(pRomInfo->Version == 14))
    {
        BlockCount = pRomInfoEX->BiosRomSize / pRomInfoEX->BaseBlockSize;
        FlashInfoSize = sizeof(ROMInfoEX) + (BlockCount * sizeof(BlockInfo_T));
    }
    else
    {
        BlockCount      = pRomInfo->TotalBlocks;                                // total number of blocks
        FlashInfoSize   = sizeof(ROMInfo) + (BlockCount * sizeof(BlockInfo_T)); // total size of total flash info
    }
    
    gpFlashInfo = (ROMInfo *)AllocMem(FlashInfoSize);
    MemSet(gpFlashInfo, FlashInfoSize, 0);
    
    if (pRomInfo->Version <= 10)
    {
        ROMInfo_Old *RomInfoOld     = (ROMInfo_Old *)pRomInfo;
        gpFlashInfo->Length         = RomInfoOld->Length;
        gpFlashInfo->Implemented    = RomInfoOld->Implemented;
        gpFlashInfo->TotalBlocks    = BlockCount;
        gpFlashInfo->Version        = RomInfoOld->Version;
        MemCpy((VOID *)&gpFlashInfo->BlockInfo, (VOID *)&RomInfoOld->BlockInfo, BlockCount * sizeof(BlockInfo_T));
    }
    else if ((bOnBoardRomIsLarge)&&(pRomInfo->Version == 14))
    {
        ROMInfoEX* pRomInfoEX       = (ROMInfoEX*)pRomInfo;
        gpFlashInfo->Length         = pRomInfoEX->Length;
        gpFlashInfo->Implemented    = pRomInfoEX->Implemented;
        gpFlashInfo->TotalBlocks    = pRomInfoEX->TotalBlocks;
        gpFlashInfo->Version        = pRomInfoEX->Version;
        gpFlashInfo->ECVersionOffset = pRomInfoEX->ECVersionOffset;
        gpFlashInfo->ECVersionMask  = pRomInfoEX->ECVersionMask;
        
        RomInfoEx2RomInfo(pRomInfoEX, (BlockInfo_T*)&(gpFlashInfo->BlockInfo));
    }
    else
        MemCpy((VOID *)gpFlashInfo, (VOID *)pRomInfo, FlashInfoSize);
    
    pBlkInfo = (BlockInfo_T *)&gpFlashInfo->BlockInfo;

    AFU_SysNCBCnt = 0;
    for (i = 0 ; i < MAXNCBLK ; i++)
    {
        AFU_SysNCBRecList[i].Offset = 0xffffffff;
    }

    for (i = 0; i < gpFlashInfo->TotalBlocks; i++){
        RomSize += pBlkInfo[i].BlockSize;
        
        //To get BB, main block, NVRAM size and address
        if(pBlkInfo[i].Type == BOOT_BLOCK){
            
            BootBlockSize += pBlkInfo[i].BlockSize;
            
            if (pBlkInfo[i].StartAddress < BbStartAddress)
                BbStartAddress = pBlkInfo[i].StartAddress;
        }
        
        if(pBlkInfo[i].Type == MAIN_BLOCK){
            MainBlockSize += pBlkInfo[i].BlockSize;
            
            if (pBlkInfo[i].StartAddress < MbStartAddress)
                MbStartAddress = pBlkInfo[i].StartAddress;
        }
        
        if(pBlkInfo[i].Type == NV_BLOCK){
            NvRamSize += pBlkInfo[i].BlockSize;
            
            if (pBlkInfo[i].StartAddress < NvRamStartAddress)
                NvRamStartAddress = pBlkInfo[i].StartAddress;
        }
        
        if(pBlkInfo[i].Type == EC_BLOCK){
            EcBlockSize += pBlkInfo[i].BlockSize;
            
            if (pBlkInfo[i].StartAddress < EcStartAddress)
                EcStartAddress = pBlkInfo[i].StartAddress;
        }
        
        if(pBlkInfo[i].Type == OA3_FLASH_BLOCK_DESC_TYPE){
            OA3BlockSize += pBlkInfo[i].BlockSize;
            
            if (pBlkInfo[i].StartAddress < OA3StartAddress)
                OA3StartAddress = pBlkInfo[i].StartAddress;
        }
        
        if (pBlkInfo[i].Type >= NC_BLOCK)
        {
            if (pBlkInfo[i].Type != LastType)
            {
                AFU_SysNCBRecList[AFU_SysNCBCnt].BlockType = pBlkInfo[i].Type;

                if(AFU_SysNCBRecList[AFU_SysNCBCnt].Offset == 0xffffffff)
                    AFU_SysNCBRecList[AFU_SysNCBCnt].Offset = pBlkInfo[i].StartAddress;

                AFU_SysNCBRecList[AFU_SysNCBCnt].Size += pBlkInfo[i].BlockSize;

                LastType = pBlkInfo[i].Type;
                AFU_SysNCBCnt++;
            }
            else if (pBlkInfo[i].Type == LastType)
            {
                AFU_SysNCBRecList[AFU_SysNCBCnt].Size += pBlkInfo[i].BlockSize;
            }
        }
    }
    
    gRomSize = RomSize;
    /*
    Print(L"gRomSize: %x\n",gRomSize);
    Print(L"BbStartAddress: %x BootBlockSize:%x\n",BbStartAddress,BootBlockSize);
    Print(L"MbStartAddress: %x MainBlockSize:%x\n",MbStartAddress,MainBlockSize);
    Print(L"NvRamStartAddress: %x NvRamSize:%x\n",NvRamStartAddress,NvRamSize);
    Print(L"EcStartAddress: %x EcBlockSize:%x\n",EcStartAddress,EcBlockSize);
     */

    // Get RrojectID Length
    GetSystemROMIDLength(gpIntrBuf, FLASH_PAGE_SIZE * FLASH_PAGE_COUNT);
      
    bIsGotBiosInfo = TRUE;
    return;
}

EFI_STATUS SmiNCBFlash(UINTN RomSection, UINT8 * pFlashBuffer, CallBackFn_T CallBackFunc, UINTN Frequency)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8 i;
    UINT32 TotalFlashSize = 0;
    UINT32 RemainFlashSize = 0;
    UINT32 BlockSize;
    UINT32 Offset;   
    UINTN   FlashRegion;
    BOOLEAN FlashNCB[MAXNCBLK-1];
    
    UINT64 Timer;
    UINTN Second;
    
    BOOLEAN FlashNCBExist = FALSE;
    
    MemSet(FlashNCB,sizeof(FlashNCB),FALSE);
    
    FlashRegion = (RomSection & 0x00FFFF00) >> 8;
    
    if(NULL != CallBackFunc)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }
    
    if (RomSection & FLASH_NON_CRITICAL_BLOCK)
    {
        for(i = 0; i < AFU_SysNCBCnt; i++)
        {
            TotalFlashSize += AFU_SysNCBRecList[i].Size;
            FlashNCB[i] = TRUE;
            FlashNCBExist = TRUE;
        }
    }
    else if (FlashRegion)
    {
        for(i = 0; i < AFU_SysNCBCnt; i++)
        {
            if (FlashRegion & (UINTN)(1<<i))
            {
                TotalFlashSize += AFU_SysNCBRecList[i].Size;
                FlashNCB[i] = TRUE;
                FlashNCBExist = TRUE;
            }
        }
    }
    
    if (!FlashNCBExist)//no NCB to flash
        return EFI_SUCCESS;
    
    BlockSize = SmiBufferBlockSize;
    
    //erase
    RemainFlashSize = TotalFlashSize;
    
    for(i = 0; i < AFU_SysNCBCnt; i++)
    {
        if (FlashNCB[i])
        { 
            for(Offset = 0; Offset < AFU_SysNCBRecList[i].Size; Offset += BlockSize)
            {
                Status = EraseBlock(AFU_SysNCBRecList[i].Offset + Offset, BlockSize, (UINTN)FLASH_NON_CRITICAL_BLOCK,(UINTN)Frequency, NULL);
                
                RemainFlashSize = RemainFlashSize - BlockSize;
                if (Status != EFI_SUCCESS)
                    return Status;
                
                if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
                {
                    Timer = GetCpuTimer();
                    CallBackFunc((UINT64)(AFU_SysNCBRecList[i].Offset + Offset),(AFU_SysNCBRecList[i].Offset + Offset + RemainFlashSize),(UINT64)TotalFlashSize,(UINTN)(FLASH_NON_CRITICAL_BLOCK | ERASE_BLOCK));
                }
            }
        }
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(FLASH_NON_CRITICAL_BLOCK | ERASE_BLOCK));
    
    //flash
    RemainFlashSize = TotalFlashSize;
        
    for(i = 0; i < AFU_SysNCBCnt; i++)
    {
        if (FlashNCB[i])
        {
            for(Offset = 0; Offset < AFU_SysNCBRecList[i].Size; Offset += BlockSize)
            {
                Status = WriteBlock(AFU_SysNCBRecList[i].Offset + Offset, BlockSize, pFlashBuffer, (UINTN)FLASH_NON_CRITICAL_BLOCK,(UINTN)Frequency, NULL);
                
                RemainFlashSize = RemainFlashSize - BlockSize;
                if (Status != EFI_SUCCESS)
                    return Status;
                
                if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
                {
                    Timer = GetCpuTimer();
                    CallBackFunc((UINT64)(AFU_SysNCBRecList[i].Offset + Offset),(AFU_SysNCBRecList[i].Offset + Offset + RemainFlashSize),(UINT64)TotalFlashSize,(UINTN)(FLASH_NON_CRITICAL_BLOCK | WRITE_BLOCK));
                }
            }
        }
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(FLASH_NON_CRITICAL_BLOCK | WRITE_BLOCK));
    
    //verify
    RemainFlashSize = TotalFlashSize;
        
    for(i = 0; i < AFU_SysNCBCnt; i++)
    {
        if (FlashNCB[i])
        {
            for(Offset = 0; Offset < AFU_SysNCBRecList[i].Size; Offset += BlockSize)
            {
                Status = VerifyBlock(AFU_SysNCBRecList[i].Offset + Offset, BlockSize, pFlashBuffer, (UINTN)FLASH_NON_CRITICAL_BLOCK,(UINTN)Frequency, NULL);
                
                RemainFlashSize = RemainFlashSize - BlockSize;
                if (Status != EFI_SUCCESS)
                    return Status;
                
                if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
                {
                    Timer = GetCpuTimer();
                    CallBackFunc((UINT64)(AFU_SysNCBRecList[i].Offset + Offset),(AFU_SysNCBRecList[i].Offset + Offset + RemainFlashSize),(UINT64)TotalFlashSize,(UINTN)(FLASH_NON_CRITICAL_BLOCK | VERIFY_BLOCK));
                }
            }
        }
    }
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(FLASH_NON_CRITICAL_BLOCK | VERIFY_BLOCK));
         

    return EFI_SUCCESS;
}

//--------------------------------------------------------------------------    
// Procedure:       RHF_Proc_CheckFlashAddr                                         
//
// Description:     Check need flash Rom Hole Flash's Address
//
// Parameter:       BlockInfo_T* pBlockInfo - system block
//                  LPROMHOLEREC lpInputRHREC - need updata rom hole info
//
// Return Value:    TRUE
//                  FALSE
//--------------------------------------------------------------------------    
BOOLEAN RomHoleCheckFlashAddr(BlockInfo_T* pBlockInfo, LPROMHOLEREC lpInputRHREC)
{
    DWORD  bs = 0;
    DWORD  be = 0;

    bs = pBlockInfo->StartAddress;
    be = bs + pBlockInfo->BlockSize - 1;


    if (lpInputRHREC->RomOffset >= bs)
    {
        if (lpInputRHREC->RomOffset <=  be)
        {
            return TRUE;
        }
    }
    else
    {
        if ((lpInputRHREC->RomOffset + lpInputRHREC->Size - 1) >= bs)
        {
            return TRUE;
        }
    }
                
    return FALSE;
}

//----------------------------------------------------------------------------
// Procedure: SWSMI_SETUP
//
// Description: 
//
// Input:   BYTE dbGetSts. 
//
// Output: ENUM_ERR_ID
//
//----------------------------------------------------------------------------
EFI_STATUS OFBD_SWSMI_SETUP(BYTE dbGetSts)
{
    PBYTE           pOFBD_Buffer = (PBYTE)NULL;
    DWORD           dwTCOffset = 0;
    POFBD_STRUCT_V2 pOFBDV2  = (POFBD_STRUCT_V2)NULL;
    POFBD_SETUP     pOFBD_SETUP= (POFBD_SETUP)NULL;
    WORD            dwOFBD_RS = 0x00;
    ENUM_ERR_ID     ErrorID = eEID_NO_ERROR;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    if (!Ofbd_GetVer())
    {          
        return ERROR_SETUP_UNSUPPORT;
    }

    pOFBD_Buffer = AllocMem(FLASH_PAGE_SIZE);
    if (pOFBD_Buffer == (PBYTE)NULL) return ERROR_BUFFER_ALLOC;         
    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE, 0);
    
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_SETUP);
    // Ofbd_Ext
    Ofbd_Ext_Header(pOFBD_Buffer, OFBD_EXT_TC_SETUP, 1);
    
    dwTCOffset = ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize;
    // Ofbd_TypeCode
    Ofbd_TC_End_Header(pOFBD_Buffer, OFBD_EXT_TC_SETUP, 1);
    
    pOFBD_SETUP = (POFBD_SETUP)((char *)pOFBD_Buffer + (UINT64) dwTCOffset);
    
    pOFBD_SETUP->dbGetSts = dbGetSts;
    pOFBD_SETUP->dbRetSts = 0x00;    
    //Copy Data To gpIntrBuf (Send SMI Buffer)
    MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE);

    Ofbd_SMI_26();
    
    dwOFBD_RS = ((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus;
    
    if (!(dwOFBD_RS & OFBD_RS_SUPPORT)) {
        //__DBG(WriteOut("- OFBD_RS_SUPPORT : 0x%X , no support !!\n", dwOFBD_RS);)

        //WriteOut("Error: BIOS donn't support NVRAM/SETUP preserve funtion.\n");
        FreeMem(pOFBD_Buffer);
        //return eEID_COMMAND_LINE;
        return ERROR_SETUP_UNSUPPORT;
    }

    pOFBD_SETUP = (POFBD_SETUP)((UINT64)gpIntrBuf + (UINT64)dwTCOffset);
    //__DBG(WriteOut("- pOFBD_SETUP->dbRetSts : 0x%04X!!\n", pOFBD_SETUP->dbRetSts));

    switch(pOFBD_SETUP->dbRetSts)
    {
        case OFBD_TC_SETUP_OK:              // Bit 0
            Status = EFI_SUCCESS;
            break;
        case OFBD_TC_SETUP_NOT_SUPPORT:     // Bit 1
            Status = ERROR_SETUP_UNSUPPORT;           
            break;
        case OFBD_TC_SETUP_STORE_ERROR:     // Bit 2;
            Status = ERROR_SETUP_STORE;
            break;
        case OFBD_TC_SETUP_RESTORE_ERROR:   // Bit 3
            Status = ERROR_SETUP_RESTORE;
            break;
        case OFBD_TC_DMI_STORE_ERROR:       // Bit 4;
            Status = ERROR_DMI_STORE;
            break;
        case OFBD_TC_DMI_RESTORE_ERROR:     // Bit 5
            Status = ERROR_DMI_RESTORE;
            break;
        default:
            Status = ERROR_SETUP_UNSUPPORT;
            break;
    }

    FreeMem(pOFBD_Buffer);

    return Status;
}

EFI_STATUS PrePareFlashBuf(UINT8 *pFlashBuffer, UINT8 *pSysRomDataBuffer, UINT8 *pFileRomDataBuffer, UINTN RomSection, UINT32 UpdateFlag)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    
    DWORD i = 0;
    UINT8 MaxRomFileRHCnt = 0;
    
    UINT16          BlockIndex = 0;
    UINT16          TotalBlock = 0;
    BlockInfo_T*    pBlockInfo = (BlockInfo_T*)NULL;

    BOOLEAN  RhInputList[MAXNCBLK + SLP_NUMBER] = {0};
    BOOLEAN  RhValidList[MAXNCBLK + SLP_NUMBER] = {0};
    
    ROMHOLEREC  tmpRHREC;
    
    DWORD  bs = 0;
    DWORD  be = 0;
    
    BOOLEAN Store_re = FALSE;    
    BOOLEAN Restore_re = FALSE;
        
    UINT32 DMIFFSOffset = 0;
    UINT32 DMIFFSSize =  0;
    DWORD SearchEnd;
    LPDMIDATA lpdmidata = (LPDMIDATA)AllocMem(sizeof(LPDMIDATA));
    
    DWORD  DataLength = 0;
    DWORD   DMISize = 0;
    
    TotalBlock = gpFlashInfo->TotalBlocks;
    pBlockInfo = (BlockInfo_T *)&gpFlashInfo->BlockInfo;
        
    MaxRomFileRHCnt = AFU_RomFile_GetRomHoleNumber();
    
    gRHFF_FV_BootEnable = FALSE;
    gRHFF_FV_MainEnable = FALSE;
    
    //EIP 382530
    if (UpdateFlag & PRESERVE_SMBIOS)
    {
        if ((RomSection & FLASH_MAIN_BLOCK) || (RomSection & FLASH_BOOT_BLOCK)) 
        {
            //store DMI data
            if (gAFU_SysRom_DMIGuidList_Current)
            {
                DMIFFSOffset = gAFU_SysRom_DMIGuidList_Current->RomOffset;
                DMIFFSSize = gAFU_SysRom_DMIGuidList_Current->Size;
                SearchEnd = DMIFFSOffset + DMIFFSSize;
                
                if (SearchEnd > gRomSize)
                    SearchEnd = gRomSize;
                
                for (i = DMIFFSOffset ; i < (SearchEnd - 4) ; i++)
                {
                    if (0 == MemCmp(pSysRomDataBuffer + i, "BSA_", 4))
                    {
                        //dmi signature found
                        DataLength = ((struct sDMIStruct *)(pSysRomDataBuffer + i))->DataLength;
                        
                        DMISize = DataLength + sizeof(DMIBlock);                    
    
                        lpdmidata->lpData = AllocMem(DMISize);
    
                        if(lpdmidata->lpData == NULL)
                        {
                            return ERROR_BUFFER_ALLOC;//AFUExit (eEID_MEM_ALLOC);
                        }
    
                        MemCpy(lpdmidata->lpData, (pSysRomDataBuffer + i), DMISize);    //   write it
                        
                        lpdmidata->Size = DMISize;
                        
                        Store_re = TRUE;
                        break;
                    }
                }
            }
            
            //restore DMI data
            if ((gAFU_RomFile_DMIGuidList_Current) && (Store_re))
            {
                if (lpdmidata->DMIPlace == 0xFF)
                    return ERROR_DMI_DATA_STORE;
                
                DMIFFSOffset = gAFU_RomFile_DMIGuidList_Current->RomOffset;
                DMIFFSSize = gAFU_RomFile_DMIGuidList_Current->Size;
                SearchEnd = DMIFFSOffset + DMIFFSSize;
                
                if (SearchEnd > gRomSize)
                    SearchEnd = gRomSize;
                
                for (i = DMIFFSOffset ; i < (SearchEnd - 4) ; i++)
                {
                    if (0 == MemCmp(pFileRomDataBuffer + i, "BSA_", 4))
                    {
                        DataLength = (size_t)((struct sDMIStruct *)(pFileRomDataBuffer + i))->DataLength;

                        //if image's data length doesnt match source's data length do nothing
                        if(DataLength < ((struct sDMIStruct *)lpdmidata->lpData)->DataLength)
                        {
                            //__SMBDBG(WriteOut("if(DataLength != ((struct sDMIStruct *)gDmiDataBuf)->DataLength)\n");)               
                            return ERROR_DMI_DATA_STORE;
                        }
                        
                        DataLength = ((struct sDMIStruct *)lpdmidata->lpData)->DataLength;
                        
                        //__SMBDBG(WriteOut("lpDestbuff = %x, i = %x, DMIBlockSize = %x\n", (lpDestbuff + i + sizeof(DMIBlock)), i, sizeof(DMIBlock));)

                        MemCpy(pFlashBuffer + i, lpdmidata->lpData, DataLength);
                        
                        Restore_re = TRUE;
                        break;
                    }
                }
            }
            
            if ((Store_re) && (FALSE == Restore_re))
                return ERROR_DMI_DATA_STORE;
        }
        
        if (RomSection & ERROR_FLASH_NVRAM)
        {
            Status = OFBD_SWSMI_SETUP(OFBD_TC_DMI_STORE);
            
            if (Status == ERROR_SETUP_UNSUPPORT)
                gReStoreSMBIOS = FALSE;
            else if (Status == EFI_SUCCESS)
                gReStoreSMBIOS = TRUE;
            else
                return Status;
        }
    }
 
    //EIP 382530
    if (UpdateFlag & PRESERVE_SETUP)
    {
        Status = OFBD_SWSMI_SETUP(OFBD_TC_SETUP_SUPPORT);
        if (EFI_ERROR(Status))
            return Status;
        
        Status = OFBD_SWSMI_SETUP(OFBD_TC_SETUP_STORE);
        if (EFI_ERROR(Status))
            return Status;
        
        gReStoreSETUP = TRUE;
    }
            
    if ((RomSection & FLASH_BOOT_BLOCK) && (RomSection & FLASH_MAIN_BLOCK) && (RomSection & FLASH_ROM_HOLE))
        return EFI_SUCCESS;
        
    if (!(RomSection & FLASH_BOOT_BLOCK))
        MemCpy(pFlashBuffer + BbStartAddress, pSysRomDataBuffer + BbStartAddress, BootBlockSize);
    
    if (!(RomSection & FLASH_MAIN_BLOCK))
        MemCpy(pFlashBuffer + MbStartAddress, pSysRomDataBuffer + MbStartAddress, MainBlockSize);
        
    
    for (i = 0 ; i < MaxRomFileRHCnt ; i++)
    {
        if (  (RomSection & FLASH_ROM_HOLE) || ((UINT64)(1<<i) & (RomSection >> 24))   )//check if this ROM hole has to be flashed
            RhInputList[i] = TRUE;
    }
    
    for (i = 0 ; i < SLP_NUMBER ; i++)
    {
        RhInputList[i + MAXNCBLK] = TRUE;
    }
    
    for (i = 0 ; i < MaxRomFileRHCnt ; i++)
    {
        if (0 == MemCmp(&(AFU_SysRHRECList[i].Guid), &(AFU_RomFileRHRECList[i].Guid), sizeof(EFI_GUID)))
        {
            /*       Print(L"RomHole.....\n");
                                Print(L"AFU_SysRHRECList[%d].Size = %x, AFU_RomFileRHRECList[%d].Size = %x\n", i, AFU_SysRHRECList[i].Size, i, AFU_RomFileRHRECList[i].Size);
                                Print(L"AFU_SysRHRECList[%d].FileOffset = %x, AFU_RomFileRHRECList[%d].FileOffset = %x\n", i, AFU_SysRHRECList[i].FileOffset, i, AFU_RomFileRHRECList[i].FileOffset);
                                Print(L"AFU_SysRHRECList[%d].RomOffset = %x, AFU_RomFileRHRECList[%d].RomOffset = %x\n", i, AFU_SysRHRECList[i].RomOffset, i, AFU_RomFileRHRECList[i].RomOffset);
                        */
            if (AFU_SysRHRECList[i].Size == AFU_RomFileRHRECList[i].Size)
            {
                if (AFU_SysRHRECList[i].RomOffset == AFU_RomFileRHRECList[i].RomOffset)
                    RhValidList[i] = TRUE;
            }
        }
    }
    
    if (AFU_SysRom_HaveSLP20() && AFU_RomFile_HaveSLP20())
    {
        for (i = 0 ; i < SLP_NUMBER ; i++)
        {
            if (0 == MemCmp(&(AFU_SysSLPRHRECList[i].Guid), &(AFU_RomFileSLPRHRECList[i].Guid), sizeof(EFI_GUID)))
            {
                if (AFU_SysSLPRHRECList[i].Size == AFU_RomFileSLPRHRECList[i].Size)
                {
                    if (AFU_SysSLPRHRECList[i].RomOffset == AFU_RomFileSLPRHRECList[i].RomOffset)
                        RhValidList[i + MAXNCBLK] = TRUE;
                }
            }
        }
    }
    
    for (i = 0 ; (UINT8)i < MaxRomFileRHCnt + SLP_NUMBER ; i++)
    {
        if (RhValidList[i])
        {
            if (RhInputList[i])
            {
                if (i < MaxRomFileRHCnt)
                    MemCpy(&tmpRHREC, &(AFU_SysRHRECList[i]), sizeof(ROMHOLEREC));
                else
                    MemCpy(&tmpRHREC, &(AFU_SysSLPRHRECList[i]), sizeof(ROMHOLEREC));
                
                MemCpy(pFlashBuffer + tmpRHREC.RomOffset,
                        pFileRomDataBuffer + tmpRHREC.RomOffset, 
                        tmpRHREC.Size); 
                
                for (BlockIndex = 0; BlockIndex < TotalBlock; BlockIndex++)
                {
                    if (RomHoleCheckFlashAddr(&(pBlockInfo[BlockIndex]), &tmpRHREC) )
                    {
                        if ( (!(RomSection & FLASH_BOOT_BLOCK)) &&  (!(RomSection & FLASH_MAIN_BLOCK)) )
                        {
                            if (pBlockInfo[BlockIndex].Type = BOOT_BLOCK)
                                gRHFF_FV_BootEnable = TRUE;
                            if (pBlockInfo[BlockIndex].Type = MAIN_BLOCK)
                                gRHFF_FV_MainEnable = TRUE;
                        }
                        
                    }
                }
            }
            else
            {
                if (i < MaxRomFileRHCnt)
                    MemCpy(&tmpRHREC, &(AFU_SysRHRECList[i]), sizeof(ROMHOLEREC));
                else
                    MemCpy(&tmpRHREC, &(AFU_SysSLPRHRECList[i]), sizeof(ROMHOLEREC));
                
                MemCpy(pFlashBuffer + tmpRHREC.RomOffset,
                        pSysRomDataBuffer + tmpRHREC.RomOffset, 
                        tmpRHREC.Size);
            }
        }
    }
    
    return EFI_SUCCESS;
}


EFI_FILE * SearchRoot()
{
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

    EFI_HANDLE  *HandleBuffer;
    UINTN       BufferSize = 0;
    EFI_HANDLE  device = NULL;
    EFI_STATUS  Status;
    UINT8       i = 0;
    EFI_FILE    *root_dir;
    

    Status = gBS->LocateHandleBuffer(ByProtocol,&gEfiSimpleFileSystemProtocolGuid,NULL,&BufferSize,&HandleBuffer);
    if (!EFI_ERROR(Status))
    {
        for (i = 0; i < BufferSize; i++)
        {
            Status = gBS->HandleProtocol(HandleBuffer[i],&gEfiSimpleFileSystemProtocolGuid,(VOID **)&SimpleFileSystem);
            if (!EFI_ERROR(Status))
            {
                device = HandleBuffer[i];
                Status = SimpleFileSystem->OpenVolume(SimpleFileSystem,&root_dir);

                if (!root_dir)
                    return NULL;
                
                return root_dir;
            }
        }
        pBS->FreePool(HandleBuffer);
    }
        
    return NULL;
}

UINTN FileWrite(EFI_FILE_HANDLE dir,CHAR16 *FileName, CHAR8 *content, UINTN len, UINTN FileAttributes)
{
    EFI_FILE_HANDLE FileHandle;
    EFI_STATUS Status;

    if (NULL == dir)
        return 0;
    Status = dir->Open(dir, &FileHandle, FileName,  EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ|EFI_FILE_MODE_CREATE, FileAttributes);

    if (EFI_ERROR(Status))
        goto out;
    
    Status = FileHandle->Write(FileHandle, &len, content);

out:
    FileHandle->Close(FileHandle);
    return len;
}

EFI_STATUS FileDelete(EFI_FILE_HANDLE dir,CHAR16 *FileName,UINTN FileAttributes)
{
    EFI_FILE_HANDLE FileHandle;
    EFI_STATUS Status;

    if (NULL == dir)
        return EFI_SUCCESS;
    Status = dir->Open(dir, &FileHandle, FileName,  EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ|EFI_FILE_MODE_CREATE, FileAttributes);

    if (EFI_ERROR(Status))
        goto out;
    
    Status = FileHandle->Delete(FileHandle);

out:
    return Status;
}

#if defined (BIOS_CONFIG_PRESERVE_BIN_SUPPORT) && (BIOS_CONFIG_PRESERVE_BIN_SUPPORT == 1)
//<AMI_PHDR_START>
//-------------------------------------------------------------------------------------------
// Procedure: OFBDHeaderGen
//
// Description: Helper generates OFBD BIOS Preserve Setup Config packet header.
//
// Input:
//      IN UINT8 Command    The Command being sent to Preserve Setup Config module in BIOS.
//
// Output:
//      POFBD_STRUCT_V2     The pointer points to the OFBD header.
//
//--------------------------------------------------------------------------------------------
//<AMI_PHDR_END>
static POFBD_STRUCT_V2 OFBDHeaderGen(UINT8 Command)
{
    PBYTE pOFBD_Buffer = gpIntrBuf;
    POFBD_PRESERVE_SETUP_CONFIG pOFBD_PreserveConfig = (POFBD_PRESERVE_SETUP_CONFIG)NULL;;
    
    if (pOFBD_Buffer == NULL) 
    {
        return 0;
    }
    
    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE * FLASH_PAGE_COUNT, 0);
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_BIOS_CFG_PRESERVE);
    // Ofbd_Ext
    Ofbd_Ext_Header(pOFBD_Buffer, OFBD_EXT_TC_BIOS_CFG_PRESERVE, 1);
    pOFBD_PreserveConfig = (POFBD_PRESERVE_SETUP_CONFIG)(pOFBD_Buffer + ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize);
    // Ofbd_TypeCode
    Ofbd_TC_End_Header(pOFBD_Buffer, OFBD_EXT_TC_BIOS_CFG_PRESERVE, 1);
    
    pOFBD_PreserveConfig->Command = Command;
    
    return (POFBD_STRUCT_V2)pOFBD_Buffer;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: OFBD_SetBiosConfigPreserveInfo
//
// Description: Pass BiosConfig Preserve Information in BIOS.
//
// Input:       BIOS_PRESERVE_INFO *pBiosPreserveInfo
//                                 Pointer to BiosPreserveInfo Structure
//
// Output:
//              UINT8 - 0, Success
//                    - 0xff, Unable to generate header
//                    - 1, If data passed to BIOS is not correct
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 OFBD_SetBiosConfigPreserveInfo(
    IN BIOS_CONFIG_PRESERVE_INFO *pBiosConfigPreserveInfo
)
{
    POFBD_STRUCT_V2 pHeader = (POFBD_STRUCT_V2)NULL;
    PBYTE pBuffer = (PBYTE)NULL;
    UINT32 Length = 0;
    POFBD_PRESERVE_SETUP_CONFIG pOFBD_PreserveSetupConfig = (POFBD_PRESERVE_SETUP_CONFIG)NULL;

    
    // Prepare the OFBD header
    pHeader = OFBDHeaderGen(OFBD_TC_BCP_SET_BIOS_CFG_PRESERVE_INFO);
    if (NULL == pHeader)
    {
        return 0xff;
    }
    
    pBuffer = (PBYTE)pHeader + pHeader->dwSize;
    MemCpy(pBuffer, pBiosConfigPreserveInfo, sizeof(BIOS_CONFIG_PRESERVE_INFO));
    pBuffer += sizeof(BIOS_CONFIG_PRESERVE_INFO);
    MemCpy(pBuffer, PLDMDATA_FILENAME, AsciiStrLen(PLDMDATA_FILENAME)+1);
    pOFBD_PreserveSetupConfig = (POFBD_PRESERVE_SETUP_CONFIG)((PBYTE)pHeader + sizeof(OFBD_STRUCT_V2) + sizeof(OFBD_TYPECODE_V2));  
    // Filling the status to OFBD_TC_BCP_RS_NOT_SUPPORTED to identify the case when BiosConfigPreserve module is not present.
    pOFBD_PreserveSetupConfig->Status = OFBD_TC_BCP_RS_NOT_SUPPORTED;
    
    Ofbd_SMI_26();
    
    return pOFBD_PreserveSetupConfig->Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: OFBD_LoadPldmData
//
// Description: Pass Pldm Data Block in BIOS.
//
// Input:       BCP_PLDM_DATA_BLOCK *pPldmDataBlock
//                                 Pointer to PldmDataBlock Structure
//
// Output:
//              UINT8 - 0, Success
//                    - 0xff, Unable to generate header
//                    - 1, If data passed to BIOS is not correct
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 OFBD_LoadPldmData(
    IN BCP_PLDM_DATA_BLOCK *pPldmDataBlock
)
{
    POFBD_STRUCT_V2 pHeader = (POFBD_STRUCT_V2)NULL;
    PBYTE pBuffer = (PBYTE)NULL;
    UINT32 Length = 0;
    POFBD_PRESERVE_SETUP_CONFIG pOFBD_PreserveSetupConfig = (POFBD_PRESERVE_SETUP_CONFIG)NULL;
    
    pHeader = OFBDHeaderGen(OFBD_TC_BCP_LOAD_PLDM_DATA);
    if(NULL == pHeader)
    {
        return 0xff;
    }
    
    pBuffer = (PBYTE)pHeader + pHeader->dwSize;
    MemCpy(pBuffer, pPldmDataBlock, sizeof(BCP_PLDM_DATA_BLOCK));

    pBuffer += sizeof(BCP_PLDM_DATA_BLOCK);

    Length = pPldmDataBlock->Length - sizeof(BCP_PLDM_DATA_BLOCK);
    MemCpy(pBuffer, pPldmDataBlock + sizeof(BCP_PLDM_DATA_BLOCK), Length);
    pOFBD_PreserveSetupConfig = (POFBD_PRESERVE_SETUP_CONFIG)((PBYTE)pHeader + sizeof(OFBD_STRUCT_V2) + sizeof(OFBD_TYPECODE_V2));  
    
    pOFBD_PreserveSetupConfig->Status = OFBD_TC_BCP_RS_NOT_SUPPORTED;
    
    Ofbd_SMI_26();
    
    return pOFBD_PreserveSetupConfig->Status;
}

EFI_STATUS CreatePldmTable(UINTN Selection, UINT8 FlashOpType)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    
    AMIPLDM_PRESERVE_INTERFACE_PROTOCOL *AmiPreservePldmInterfaceProtocol = NULL;
    
    EFI_GUID  gAmiPldmBmcProtocolGuid = AMIPLDM_BMC_DRIVER_PROTOCOL_GUID;
    AMIPLDM_INTERFACE_PROTOCOL *AmiPldmInterfaceProtocol = NULL;
    
    UINT32      PldmBufferSize = 0; 
    SCE_PLDM_DATA_HEADER    *PldmBuffer = NULL;
        
    PresrvePLDMTables GetPldmData;
    
    BIOS_CONFIG_PRESERVE_INFO *BiosPreserveInfo;
    BCP_PLDM_DATA_BLOCK *PldmDataBlock;
    
    UINT8   *AfuPldmFileName = PLDMDATA_FILENAME;
    CHAR16  *AfuPldmFileName_Uni = PLDMDATA_FILENAME_WIDTH;
    EFI_FILE    *root_dir = NULL;
    UINTN Len = 0;
    
    UINT32      Version = 0x1;
    UINT32      Signature = 'MDLP';
    
    UINT8 RetStatus = 0;              // Return value
    UINT32 Size = 0;
    UINT32 BlockSize;
    UINT32 Count = 0;
    
    UINTN Operation;
    
    Status = pBS->LocateProtocol(&gAmiPreservePldmInterfaceProtocolGuid, NULL, &AmiPreservePldmInterfaceProtocol);
    if (EFI_ERROR (Status))
        return ERROR_PLDM_LOCATE_PROTOCOL;
    

    //Create the Pldm tables(last parameter for BCP ALL or changed only)
    //keep second parameter FALSE like AFU tool
    //keep 3th and 4th parameter NULL and 0, it is reserved for future use.
    if (Selection & BCPALL)
        Operation = BCP_ALL;
    else
        Operation = BCP_ONLY_DEFAULTS;

    Status = AmiPreservePldmInterfaceProtocol->CreatePldm(&GetPldmData,FALSE, NULL, 0, Operation);
    if (EFI_ERROR (Status))
        return ERROR_PLDM_CREATE_TABLE;
/*
    Print(L"GetPldmData.StringSize: %d\n",GetPldmData.StringSize);
    Print(L"GetPldmData.AttributeSize: %d\n",GetPldmData.AttributeSize);
    Print(L"GetPldmData.ValueSize: %d\n",GetPldmData.ValueSize);
    Print(L"GetPldmData.MapSize: %d\n",GetPldmData.MapSize);
  */  
    PldmBufferSize = sizeof (SCE_PLDM_DATA_HEADER) + GetPldmData.StringSize + GetPldmData.AttributeSize + GetPldmData.ValueSize + GetPldmData.MapSize;
    PldmBuffer = (SCE_PLDM_DATA_HEADER *)Malloc(PldmBufferSize) ;
        
    if (NULL == PldmBuffer)
        return ERROR_BUFFER_ALLOC;
    
    MemSet((void*)PldmBuffer, PldmBufferSize, 0 );
    PldmBuffer->Version = Version;
    PldmBuffer->Signature = Signature;
    PldmBuffer->StringOffset = sizeof (SCE_PLDM_DATA_HEADER);
    PldmBuffer->StringSize = GetPldmData.StringSize;
    PldmBuffer->AttribOffset = PldmBuffer->StringOffset + PldmBuffer->StringSize ;
    PldmBuffer->AttribSize = GetPldmData.AttributeSize;
    PldmBuffer->ValueOffset = PldmBuffer->AttribOffset + PldmBuffer->AttribSize; 
    PldmBuffer->ValueSize = GetPldmData.ValueSize;
    PldmBuffer->MappingString = PldmBuffer->ValueOffset + PldmBuffer->ValueSize;
    PldmBuffer->MapStringSize = GetPldmData.MapSize;
            
        
    if( (NULL == GetPldmData.StringTable) || (NULL == GetPldmData.AttributeTable) || (NULL == GetPldmData.ValueTable) || (NULL == GetPldmData.MapStringTable) )
    {
        return ERROR_PLDM_UNSUPPORT;
    }
    else
    {
        MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->StringOffset) , GetPldmData.StringTable, GetPldmData.StringSize);
        MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->AttribOffset), GetPldmData.AttributeTable, GetPldmData.AttributeSize);
        MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->ValueOffset), GetPldmData.ValueTable, GetPldmData.ValueSize);
        MemCpy ((UINT8 *)PldmBuffer + (PldmBuffer->MappingString), GetPldmData.MapStringTable, GetPldmData.MapSize); 
    }
    
    BiosPreserveInfo = Malloc(sizeof(BIOS_CONFIG_PRESERVE_INFO) + AsciiStrLen(AfuPldmFileName) + 1);
    if (NULL == BiosPreserveInfo)
        return ERROR_BUFFER_ALLOC;
    BiosPreserveInfo->FlashOpType = FlashOpType;
    BiosPreserveInfo->Length = sizeof(BIOS_CONFIG_PRESERVE_INFO) + (UINT32)(AsciiStrLen(AfuPldmFileName)) + 1;
    //Print(L"BiosPreserveInfo->Length: %d\n",BiosPreserveInfo->Length);
    MemCpy (  BiosPreserveInfo + sizeof (BIOS_CONFIG_PRESERVE_INFO), AfuPldmFileName, AsciiStrLen(AfuPldmFileName));

    RetStatus = OFBD_SetBiosConfigPreserveInfo(BiosPreserveInfo); //Pass filled BiosPreserveInfo structure to BIOS

    if(OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED != (RetStatus & OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED) && OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED != RetStatus 
                && OFBD_TC_BCP_RS_NOT_SUPPORTED != RetStatus && 0 != RetStatus )
    {
        root_dir = SearchRoot();
    
        if (NULL != root_dir)
        {
            FileDelete(root_dir,AfuPldmFileName_Uni,0ULL);
            Len = FileWrite(root_dir, AfuPldmFileName_Uni, (CHAR8 *)PldmBuffer, PldmBufferSize, 0ULL);
            if (Len != PldmBufferSize)
            {
                FileDelete(root_dir,AfuPldmFileName_Uni,0ULL);
                return ERROR_PLDM_VOLUME_FULL;
            }
        }
        else
            return ERROR_PLDM_NO_MEDIA;
    }
    

    //Print(L"OFBD_SetBiosConfigPreserveInfo RetStatus: %x\n",RetStatus);
    if(OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED == RetStatus)
    {
        FileDelete(root_dir,AfuPldmFileName_Uni,0ULL);
        return ERROR_PLDM_PRESERVE_CFG_FAILED;
    }
        
    if(OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED == (RetStatus & OFBD_TC_BCP_RS_FV_SUPPORT_ENABLED) )
    {
        FileDelete(root_dir,AfuPldmFileName_Uni,0ULL);
    }
    else if (OFBD_TC_BCP_RS_OK == (RetStatus & OFBD_TC_BCP_RS_OK))
    {
    }
    else
        return ERROR_PLDM_PRESERVE_CFG_FAILED;

    
    Size = PldmBufferSize; //Size of Pldm Data
    BlockSize = (FLASH_PAGE_SIZE * FLASH_PAGE_COUNT) / 2; //Max no of bytes to be passed to bios at a time
    while(Size > 0)
    {    
        if(Size < BlockSize)
        {
            BlockSize = Size;
        }
        
        PldmDataBlock = Malloc(BlockSize + sizeof(BCP_PLDM_DATA_BLOCK));
        if (NULL == PldmDataBlock)
            return ERROR_BUFFER_ALLOC;
        

        MemCpy(PldmDataBlock + sizeof(BCP_PLDM_DATA_BLOCK), (UINT8*)PldmBuffer + Count, BlockSize);
    
        PldmDataBlock->Length = sizeof(BCP_PLDM_DATA_BLOCK) + BlockSize;

        RetStatus = OFBD_LoadPldmData(PldmDataBlock); //Pass filled PldmDataBlock structure to BIOS
        //Print(L" OFBD_LoadPldmDataRetStatus: %x\n",RetStatus);
        if(OFBD_TC_BCP_RS_PRESERVE_CFG_FAILED == RetStatus)
        {
            return ERROR_PLDM_PRESERVE_CFG_FAILED;
        }
        
        if(OFBD_TC_BCP_RS_FV_SIZE_EXCEED_FAILED == RetStatus)
        {
            return ERROR_PLDM_EXCEEDS_PLDM_SIZE;
        }

        Size = Size - BlockSize;
        Count = Count + BlockSize;
        

        //pBS->FreePool(PldmDataBlock); //free PldmDataBlock.Data buffer

    }   


    //pBS->FreePool(PldmBuffer); //free PldmData buffer to avoid memory leak

    
    return Status;
}
#endif

BOOLEAN IsSecureFlashSupported( UINT8  Policy)
{
            
    if (0 == (gFlashPolicy.FlUpdPolicy.FlashUpdate & Policy))
    {   
            return FALSE;
    }
    
    if (0 == (gFlashPolicy.FlUpdPolicy.BBUpdate & Policy))
    {   
        return FALSE;
    }
    
    return TRUE;
}

EFI_STATUS SecureFlashValidate  ( UINT8 *FileBuffer  ,UINT32 FileSize, UINT32 RomSection)
{

    EFI_STATUS  Status = EFI_SUCCESS;
     
    GetFlashPolicy( &gFlashPolicy );
                        
    if (IsSecureFlashSupported(FlRuntime))
    {     
        /* Load ROM image data block into BIOS. */ 
        pFuncBlock = Malloc(sizeof(FUNC_BLOCK));
        pFuncBlock->BufAddr = (UINTN)FileBuffer;
        pFuncBlock->BlockAddr = 0 ;
        pFuncBlock->BlockSize = FileSize ;
        pFuncBlock->ErrorCode = 1;

        Status = LoadFirmwareImage(  pFuncBlock );
        //Print(L"LoadFirmwareImage: %r\n",Status);
            
        /* Inform BIOS which flash method will be taken. */
        /*  Runtime  Flash */
        gFlashSessionBlock.FlUpdBlock.FlashOpType = FlRuntime ;
        gFlashSessionBlock.FlUpdBlock.ROMSection = RomSection;
        gFlashSessionBlock.FlUpdBlock.ImageSize =  FileSize ;
        gFlashSessionBlock.ErrorCode = 1;
        Status = SetFlashMethod_Internal(  &gFlashSessionBlock );
        //Status = SetFlashMethod(  &gFlashSessionBlock );
        
        /*
        if(gFlashSessionBlock.ErrorCode == 1)
            Print(L" Error: Secure Flash Rom Verify fail.  \n");
        else
            Print(L" SECURE_FLASH_Validate success\n");
            */
            
    }
       

    return Status;
}

EFI_STATUS CheckRomSection(UINTN RomSection, BOOLEAN MEEnabled, UINT32 RomFileSize, UINT32 RomSize)
{

    if ((RomSection & FLASH_BOOT_BLOCK) && (BootBlockSize == 0))
        return ERROR_BOOTBLOCK_UNSUPPORT;
    
    if ((RomSection & FLASH_MAIN_BLOCK) && (MainBlockSize == 0))
        return ERROR_MAINBIOS_UNSUPPORT;
    
    if ((RomSection & FLASH_NVRAM) && (NvRamSize == 0))
        return ERROR_NVRAM_UNSUPPORT;
    

    if (RomSection & FLASH_FULL_ME)
    {
        if (!MEEnabled)
            return ERROR_ME_UNSUPPORT;
        
        AFU_bMLAP_APL_Enable = FALSE;
        AFU_ACPITable_CheckFlow();
        if (AFU_bMLAP_APL_Enable)
            return ERROR_ME_UNSUPPORT;
                
        if (RomFileSize <= RomSize)
            return ERROR_ME_FILE_SIZE;
        
        
    }
    
    return EFI_SUCCESS;
}

GlobalParametersInit()
{
    gAFU_SysRom_DMIGuidList_Head = NULL;
    gAFU_SysRom_DMIGuidList_Current = NULL;
    DmiNumber_SysRom = 0; 
    gAFU_RomFile_DMIGuidList_Head = NULL;
    gAFU_RomFile_DMIGuidList_Current = NULL;
    DmiNumber_RomFile = 0;
    
    gReStoreSMBIOS = FALSE;
    gReStoreSETUP = FALSE;
}
/**
    SmiBiosUpdate()

    This function programs BIOS by SMI.
    
    @param RomFileBufAddr -  The  address of RomFile buffer
    @param RomFileSize - The size of RomFile buffer
    @param RomSection - The BIOS region which section would be update.
    @param Selection - The selection for RomHole and NCB 
    @param CallBackFunc - The CallBack function
    @param Frequency - The executed frequency of call back function.
    @param UpdateFlag - The ability to retain data.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS SmiBiosUpdate( UINTN RomFileBufAddr, UINT32 RomFileSize, UINTN RomSection, UINTN Selection, CallBackFn_T CallBackFunc, UINTN Frequency, UINT32 UpdateFlag )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *pFileRomDataBuffer = NULL;
    UINT8       *pSysRomDataBuffer = NULL;
    UINT8       *pFlashBuffer = NULL;
    UINT8       *MEBuf = NULL;
    UINT32      BiosSize = 0 ,BiosBase = 0;
    UINT32      FlashCapacity = 0,MEBase = 0 ,MESize = 0;
    UINTN       i = 0;
    int         FunctionRes = 0;
    // EIP 377144 +>>>
    DWORD BiosStartAddress = 0;
    DWORD BiosBlockSize    = 0;
    // EIP 377144 <<<+
    
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT && SMI_FLASH_INTERFACE_VERSION > 10
    UINT8 *EcBlockBuff = NULL;
    UINT32 RomEcVersion = 0 , FileEcVersion = 0;
#endif
    
    GlobalParametersInit();
    
    SmiAllocateMemory();
    
    GetBiosInfo();  //SMI 0x25 to get on board bios size and block info 

    
    
    // EIP 377144 +>>>
    if(!gMEEnabled)
    {
        Status = MERF_Check();
        if(Status == EFI_SUCCESS)
            gMEEnabled = TRUE;
    }

    Status = CheckRomSection(RomSection,gMEEnabled,RomFileSize,gRomSize);
    if (EFI_ERROR(Status))
        goto Exit;
        
    if(RomFileSize != gRomSize)
    {
        if(gMEEnabled)
        {
            MER_GetBlkInfo(BIOS_BLK, &BiosStartAddress, &BiosBlockSize);
            ///debug+
            //Print(L"***SmiBiosUpdate()\n");
            //Print(L"   BiosStartAddress = 0x%08X\n", BiosStartAddress);
            //Print(L"   BiosBlockSize    = 0x%08X\n", BiosBlockSize);
        }
    }
    
    if(!BiosStartAddress)
    {
        Status = FileSizeCheck( RomFileSize , &FlashCapacity ); 
        if(Status != EFI_SUCCESS)
            goto Exit;
        if(FlashCapacity == RomFileSize)
            BiosStartAddress = FlashCapacity - gRomSize ;
    }
    BiosBase = BiosStartAddress;
    BiosSize = gRomSize ;
    ///debug+
    //Print(L"***SmiBiosUpdate()\n");
    //Print(L"   BiosBase = 0x%X, BiosSize = 0x%X\n", BiosBase, BiosSize);
    // EIP 377144 <<<+
    
    //Enable flash
    FunctionRes = EnableFlashIfc();
    if(FunctionRes != 0)
    {
        DisableFlashIfc();
        Status = EFI_DEVICE_ERROR;
        goto Exit;
    }
    
    //Start update BIOS 
    pFileRomDataBuffer = Malloc(BiosSize);  
    if(pFileRomDataBuffer == NULL) 
    {
        Status = ERROR_BUFFER_ALLOC;
        goto Exit;
    }
    
    pSysRomDataBuffer = Malloc(BiosSize);
    if(pSysRomDataBuffer == NULL) 
    {
        Status = ERROR_BUFFER_ALLOC;
        goto Exit;
    }
    
    pFlashBuffer = Malloc(BiosSize);
    if(pFlashBuffer == NULL) 
    {
        Status = ERROR_BUFFER_ALLOC;
        goto Exit;
    }
        
    MemCpy(pFileRomDataBuffer, (UINT8 *)(RomFileBufAddr + BiosBase), BiosSize);
    
    
    /*//EIP 382530
    for (i = 0; BIOSPreUpdate[i] != NULL; i++) 
        BIOSPreUpdate[i](UpdateFlag);
    */
    
    
    MemCpy(pFlashBuffer, (UINT8 *)(RomFileBufAddr + BiosBase), BiosSize);
    
    if (RomSection != FLASH_FULL_ME)
    {
        //read system rom
        Status = ReadBlock(0, BiosSize, pSysRomDataBuffer, 0,(UINTN)Frequency, CallBackFunc);
        
        AFU_SysRom_SearchNCB(pSysRomDataBuffer);
        AFU_RomFile_SearchNCB(pFileRomDataBuffer);
            
        Status = PrePareFlashBuf(pFlashBuffer, pSysRomDataBuffer, pFileRomDataBuffer, RomSection, UpdateFlag);
        if (EFI_ERROR(Status))
            goto Exit;
        
        SmiFreeMemory();
        Status = SecureFlashValidate(pFlashBuffer,BiosSize,(UINT32)RomSection);
        if (EFI_ERROR(Status))
            goto Exit;
        SmiAllocateMemory();
        
#if defined (BIOS_CONFIG_PRESERVE_BIN_SUPPORT) && (BIOS_CONFIG_PRESERVE_BIN_SUPPORT == 1)
        if (RomSection & FLASH_NVRAM )
        {
            if (!(Selection & CLRCFG))
                Status = CreatePldmTable(Selection, RUNTIME_FLASH_UPDATE);
            
            if (EFI_ERROR(Status))
                goto Exit;
        }
#endif
            
    }
    
    //Boot Block update
    if( ( (RomSection & FLASH_BOOT_BLOCK) || (gRHFF_FV_BootEnable) ) && (Status == EFI_SUCCESS))
    {    
        //Erase BootBlock
        Status = EraseBlock(BbStartAddress, BootBlockSize, (UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_BOOTBLOCK;
        }
        
        //Write BootBlock
        Status = WriteBlock(BbStartAddress, BootBlockSize, pFlashBuffer, (UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_BOOTBLOCK;
        }
        
        //Verify BootBlock
        Status = VerifyBlock(BbStartAddress, BootBlockSize, pFlashBuffer, (UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency, CallBackFunc);

        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_BOOTBLOCK;
        }
    }

    //Main BIOS update
    if( ( (RomSection & FLASH_MAIN_BLOCK ) || (gRHFF_FV_MainEnable) ) && (Status == EFI_SUCCESS))
    {
        //Erase MainBlock
        Status = EraseBlock(MbStartAddress, MainBlockSize, (UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_MAINBIOS;
        }
        
        //Write MainBlock
        Status = WriteBlock(MbStartAddress, MainBlockSize, pFlashBuffer, (UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_MAINBIOS;  
        }
        
        //Verify BootBlock
        Status = VerifyBlock(MbStartAddress, MainBlockSize, pFlashBuffer, (UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_MAINBIOS;
        }
    }
    
    
    //NVram update
    if((RomSection & FLASH_NVRAM ) && (Status == EFI_SUCCESS))
    {

        //Erase NVRAM
        Status = EraseBlock(NvRamStartAddress, NvRamSize, (UINTN)FLASH_NVRAM,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_NVRAM; 
        }
        
        //Write NVRAM
        Status = WriteBlock(NvRamStartAddress, NvRamSize, pFlashBuffer, (UINTN)FLASH_NVRAM,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_NVRAM; 
        }
        
        //Verify NVRAM
        Status = VerifyBlock(NvRamStartAddress, NvRamSize, pFlashBuffer, (UINTN)FLASH_NVRAM,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_NVRAM; 
        }
    }
    
    if ((RomSection & FLASH_ROM_HOLE) || (RomSection & 0xFFFF000000))//Flash ROM hole
    {
        //Only display string
        //Erase ROM hole
        Status = EraseBlock(0, 0, (UINTN)FLASH_ROM_HOLE,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_RH; 
        }
        
        //Write ROM hole
        Status = WriteBlock(0, 0, pFlashBuffer, (UINTN)FLASH_ROM_HOLE,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_RH; 
        }
        
        //Verify ROM hole
        Status = VerifyBlock(0, 0, pFlashBuffer, (UINTN)FLASH_ROM_HOLE,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_RH; 
        }
    }
    
    if((RomSection & FLASH_NEWER_EC_BLOCK) && (Status == EFI_SUCCESS))
    {   
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT && SMI_FLASH_INTERFACE_VERSION > 10    

            EcBlockBuff = Malloc(EcBlockSize + 1);
            Status = ReadBlock(EcStartAddress, EcBlockSize, EcBlockBuff, (UINTN)FLASH_NEWER_EC_BLOCK,(UINTN)Frequency, NULL);
            
            if(Status == EFI_SUCCESS)
            {           
                RomEcVersion = *(UINT32*) (EcBlockBuff + EC_VERSION_OFFSET) ;
                FileEcVersion = *(UINT32*) ( RomFileBufAddr + EcBlockAdds + EC_VERSION_OFFSET) ;
                
                if ((RomEcVersion & EC_VERSION_MASK) < (FileEcVersion & EC_VERSION_MASK))  // newer version
                {
                    Status = EraseBlock(EcStartAddress, EcBlockSize, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc);
                    if(EFI_SUCCESS != Status)
                    {
                        DisableFlashIfc();
                        Status = ERROR_FLASH_EC ;   
                    }
                    
                    Status = WriteBlock(EcStartAddress, EcBlockSize, pFlashBuffer, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc);
                    if(EFI_SUCCESS != Status)
                    {
                        DisableFlashIfc();
                        Status = ERROR_FLASH_EC ;   
                    }
                    
                    Status = VerifyBlock(EcStartAddress, EcBlockSize, pFlashBuffer, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc);
                    if(EFI_SUCCESS != Status)
                    {
                        DisableFlashIfc();
                        Status = ERROR_FLASH_EC ;   
                    }
                }
            }
            
            if(Status != EFI_SUCCESS)
                Status = ERROR_FLASH_EC ;
#else       
            Status =  ERROR_EC_UNSUPPORT;       
#endif
    }
    
    if((RomSection & FLASH_EC_BLOCK ) && (Status == EFI_SUCCESS))
    {
#if EC_FIRMWARE_UPDATE_INTERFACE_SUPPORT && SMI_FLASH_INTERFACE_VERSION > 10    
        Status = EraseBlock(EcStartAddress, EcBlockSize, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc);
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_EC ;       
        }
        
        Status = WriteBlock(EcStartAddress, EcBlockSize, pFlashBuffer, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc);   
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_EC ;   
        }
        
        Status = VerifyBlock(EcStartAddress, EcBlockSize, pFlashBuffer, (UINTN)FLASH_EC_BLOCK,(UINTN)Frequency, CallBackFunc); 
        if(EFI_SUCCESS != Status)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_EC ;   
        }
#else       
        Status =  ERROR_EC_UNSUPPORT;       
#endif
    }
    
    if((RomSection & FLASH_NON_CRITICAL_BLOCK ) && (Status == EFI_SUCCESS))
    {
        Status = SmiNCBFlash(RomSection,pFlashBuffer,CallBackFunc,Frequency);
        if (Status != EFI_SUCCESS)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_NCB;
        }
    }
    
    if( ((RomSection & 0x00FFFF00)) && (Status == EFI_SUCCESS) )//K0~K15
    {
        Status = SmiNCBFlash(RomSection,pFlashBuffer,CallBackFunc,Frequency);
        if (Status != EFI_SUCCESS)
        {
            DisableFlashIfc();
            Status = ERROR_FLASH_NCB;
        }
    }
    
    if((RomSection & FLASH_FULL_ME ) && (Status == EFI_SUCCESS))
    {   
#if defined (AFU_MEUD_SUPPORT) && (AFU_MEUD_SUPPORT == 1)       
//TRACE((-1,"MERF_Check 3-1. %d\n", FlashCapacity));
        gpMEBuff = (UINT8 *)NULL;
///
        //Status = MERF_Check();
        //if(Status==EFI_SUCCESS)
        if(gMEEnabled)
        {
            /*if(RomFileSize>FLASH_SIZE)
                gMESize = RomFileSize - FLASH_SIZE;
            else*/
            
            gMESize = RomFileSize;
            gMERealFlashSize = RomFileSize - gRomSize;
            gpMEBuff = Malloc(gMESize);
                
            if(gpMEBuff)
            {
                MemCpy(gpMEBuff, (UINT8 *)RomFileBufAddr, gMESize); 
                Status = MERF_Flash(Frequency, CallBackFunc);
                
                if(Status==EFI_SUCCESS)
                {
                    if(NULL != CallBackFunc)
                        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,FLASH_FULL_ME | WRITE_BLOCK);
                }

                
                pBS->FreePool(gpMEBuff);
                gpMEBuff = (UINT8 *)NULL;
            }

        }
        else
            Status = ERROR_ME_UNSUPPORT ;
#else
        Status = ERROR_ME_UNSUPPORT ;
#endif
    }// update ME end

    
    
    /*//EIP 382530
    for (i = 0; BIOSEndUpdate[i] != NULL; i++) 
        BIOSEndUpdate[i](UpdateFlag);
     */
    if (gReStoreSMBIOS)
    {
        Status = OFBD_SWSMI_SETUP(OFBD_TC_DMI_RESTORE);
        if (EFI_ERROR(Status))
            goto Exit;
    }
    
    if (gReStoreSETUP)
    {
        Status = OFBD_SWSMI_SETUP(OFBD_TC_SETUP_RESTORE);
        if (EFI_ERROR(Status))
            goto Exit;
    }
    
    if(pFileRomDataBuffer)
        pBS->FreePool(pFileRomDataBuffer);
    
    if(pSysRomDataBuffer)
        pBS->FreePool(pSysRomDataBuffer);
    
    if(pFlashBuffer)
        pBS->FreePool(pFlashBuffer);
    
Exit:
    //Disable flash
    //FunctionRes = DisableFlashIfc();
    //if(FunctionRes != 0)
    //    return EFI_DEVICE_ERROR;
    DisableFlashIfc();
    SmiFreeMemory();

    return Status;
}

/**
    SmiOA3Update()

    This function programs OEM Activation 3 Blocks by SMI.
    
    @param OAFileBufAddr -  The  address of RomFile buffer
    @param OAFileSize - The size of RomFile buffer
    @param CallBackFunc - The Call back function
    @param Frequency - The executed frequency of call back function.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS SmiOA3Update( UINTN OAFileBufAddr, UINT32 OAFileSize, CallBackFn_T CallBackFunc, UINTN Frequency )
{
    
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *pDataBuffer = NULL;
    UINT8       *MEBuf = NULL;
    UINT32      BiosSize = 0 ,BiosBase = 0;
    UINT32      FlashCapacity = 0,MEBase = 0 ,MESize = 0;
    UINTN       i = 0;
    int         FunctionRes = 0;

#if OEM_ACTIVATION_TABLE_LOCATION ==1
    
    SmiAllocateMemory();
    
    GetBiosInfo();  //SMI 0x25 to get on board BIOS size and block info 
        
    //Start update BIOS 
    pDataBuffer = Malloc(OA3BlockSize); 
    if(pDataBuffer == NULL)
    {
        Status =  ERROR_BUFFER_ALLOC;
        goto Exit;
    }

    MemSet(pDataBuffer, OA3BlockSize, 0xFF);
    MemCpy(pDataBuffer, (UINT8 *)(OAFileBufAddr), OAFileSize);  

    //Enable flash
    FunctionRes = EnableFlashIfc();
    
    if(FunctionRes != 0)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    Status = EraseBlock(OA3StartAddress, OA3BlockSize, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    Status = WriteBlock(OA3StartAddress, OA3BlockSize, pDataBuffer, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    Status = VerifyBlock(OA3StartAddress, OA3BlockSize, pDataBuffer, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }

    FunctionRes = DisableFlashIfc();
    
    if(FunctionRes != 0)
    {
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    pBS->FreePool(pDataBuffer);

Exit:
    SmiFreeMemory();
    
    return Status;
#else
    return EFI_UNSUPPORTED;
#endif
}

/**
    SmiOA3Delete()

    This function deletes OEM Activation 3 Blocks by SMI.
    
    @param CallBackFunc - The Call back function
    @param Frequency - The executed frequency of call back function.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS SmiOA3Delete(CallBackFn_T CallBackFunc, UINTN Frequency )
{
    
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8       *pDataBuffer = NULL;
    UINT8       *MEBuf = NULL;
    UINT32      BiosSize = 0 ,BiosBase = 0;
    UINT32      FlashCapacity = 0,MEBase = 0 ,MESize = 0;
    UINTN       i = 0;
    int         FunctionRes = 0;

#if OEM_ACTIVATION_TABLE_LOCATION ==1
    
    SmiAllocateMemory();
    
    GetBiosInfo();  //SMI 0x25 to get on board BIOS size and block info 
        
    //Start update BIOS 
    pDataBuffer = Malloc(OA3BlockSize);   
    if(pDataBuffer == NULL)
    {
        Status =  ERROR_BUFFER_ALLOC;
        goto Exit;
    }

    MemSet(pDataBuffer, OA3BlockSize, 0xFF);
    //Enable flash
    FunctionRes = EnableFlashIfc();
    
    if(FunctionRes != 0)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    Status = EraseBlock(OA3StartAddress, OA3BlockSize, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    Status = WriteBlock(OA3StartAddress, OA3BlockSize, pDataBuffer, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }

    Status = VerifyBlock(OA3StartAddress, OA3BlockSize, pDataBuffer, (UINTN)FLASH_OA3_BLOCK,(UINTN)Frequency, CallBackFunc);
    if(EFI_SUCCESS != Status)
    {
        DisableFlashIfc();
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    FunctionRes = DisableFlashIfc();
    
    if(FunctionRes != 0)
    {
        Status = ERROR_FLASH_OA;
        goto Exit;
    }
    
    pBS->FreePool(pDataBuffer);

Exit:
    SmiFreeMemory();
    
    return Status;
#else
    return EFI_UNSUPPORTED;
#endif
}

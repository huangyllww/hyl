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
AmiAfuProtocol.c

This file is the main source file for  AmiAfuProtocol module.

Its entry point at AFUProtocolEntryPoint.

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

#include <Library/UefiLib.h>

#if AFU_PROTOCOL_ARCH  != 1
#include "AfuDriver.h"
#include "SmiInterface.h"
#include "MeInterface.h"
#include "ofbd_cfg.h"
#endif

//---------------------------------------------------------------------------
// Module specific global variable
//----------------------------------------------------------------------
static EFI_GUID gAFUProtocolGuid = AMI_AFU_PROTOCOL_GUID;
//---------------------------------------------------------------------------
// Module defined global variables
//---------------------------------------------------------------------------

static FLASH_PROTOCOL *Flash = NULL;
static UINT64 Freq;
static UINT32 Flash4GBMapStart;
UINT32 BootBlockAdds,BootBlockSize;
UINT32 MainBlockAdds,MainBlockSize;
UINT32 NvRamStart,NvRamSize;
UINT32 EcBlockAdds,EcBlockSize;

#if AFU_PROTOCOL_ARCH  == 0
FLASH_POLICY_INFO_BLOCK     *gpFlashPolicyInfo;
FUNC_BLOCK  *gpFuncBlock ;
FUNC_FLASH_SESSION_BLOCK *gpFlashSessionBlock;
UINT8 *gOFBDBuffer ;
UINTN pPhyAddr ;

UINT32 gSmiMailBoxAddress = 0;
UC2SmiMailbox gSmiMailBox;
BOOLEAN gMEEnabled = FALSE; // EIP 377144 +

extern UINT8 *  gpIntrBuf;
extern BYTE    g_OnBoardROMProjectIDLength;
extern UINT32  gRomSize;

#endif

typedef VOID (BIOS_Pre_Update) (UINT32 Flag);
typedef VOID (BIOS_End_Update) (UINT32 Flag);
extern BIOS_Pre_Update BIOS_Pre_Update_List EndOfBIOSPreUpdateList;
extern BIOS_End_Update BIOS_End_Update_List EndOfBIOSEndUpdateList;
BIOS_Pre_Update* BIOSPreUpdate[] = {BIOS_Pre_Update_List NULL};
BIOS_End_Update* BIOSEndUpdate[] = {BIOS_End_Update_List NULL};

typedef VOID (ME_Pre_Update) (UINT32 Flag);
typedef VOID (ME_End_Update) (UINT32 Flag);
extern ME_Pre_Update ME_Pre_Update_List EndOfMEPreUpdateList;
extern ME_End_Update ME_End_Update_List EndOfMEEndUpdateList;
ME_Pre_Update* MEPreUpdate[] = {ME_Pre_Update_List NULL};
ME_End_Update* MEEndUpdate[] = {ME_End_Update_List NULL};
//----------------------------------------------------------------------




#define     OFBD_FS_SECURE_FLASH        BIT06
#define     OFBD_EXT_TC_SECURE_FLASH    0x70


/**
    CallBackExecuteFrequency()
    
    Execute Frequency.
    
    @param MicroSecond -  Timer to run the call back function.
    @param Ticker -  The previous gotten CPU time.
    
    @retval BOOLEAN - The result to decide if runs call back function.
*/
BOOLEAN CallBackExecuteFrequency(
  IN UINTN      MicroSecond,
  IN UINT64     Ticker
)
{
    UINT64  CurrentTicker;
    UINTN   Second;
    
    if(Ticker == 0)
        return TRUE;

    CurrentTicker = GetCpuTimer() - Ticker;
    
    Second = (UINTN) Div64 (
                          CurrentTicker,
                          (UINT32) Freq,
                          NULL
                          );
    
    if(Second >=MicroSecond)
        return TRUE;

    return FALSE;
}

/**
    EraseBlock2()
    
    Erase the ROM by block.
    
    @param Location -  The FV address.
    @param Address -  The address of ROM file buffer which will be used to write into the flash part.
    @param Buffer - The ROM file buffer.
    @param Size - The ROM file size.
    @param Section - The FV type.
    @param Frequency - The executed frequency of Call back function.
    @param Progress - The call back function.
    
    @retval EFI_STATUS - The result
*/
EFI_STATUS EraseBlock2(
IN UINT64 Location,
IN UINT64 Address,
IN UINT64 Buffer,
IN UINT64 Size,
IN UINTN Section,
IN UINTN Frequency,
IN CallBackFn_T Progress
)
{
    EFI_STATUS Status;
    UINTN TempSize;
    UINTN Offset;
    UINT64 Timer;
    UINTN Second;

    if(NULL != Progress)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }

    Flash->DeviceWriteEnable(); 

    TempSize = FLASH_BLOCK_SIZE;

    for(Offset = 0;Offset < Size;Offset += TempSize )
    {
        if ( Size <(Offset + TempSize))
            TempSize = Size - Offset;

        if(NULL != Progress && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            Progress((UINT64)(Address + Offset),(Address + Size),(UINT64)Size,(UINTN)(Section | ERASE_BLOCK));
        }

        Status = Flash->Erase((UINT8*)(Location + Offset), (UINTN)TempSize);
        
        if(Status != EFI_SUCCESS)
        {
            Flash->DeviceWriteDisable();
            return Status;
        }
    }
    Flash->DeviceWriteDisable();

    if(NULL != Progress)
        Progress((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | ERASE_BLOCK));

    return EFI_SUCCESS;
}

/**
    ReadBlock2()
    
    Read the ROM by block.
    
    @param Location -  The FV address.
    @param Address -  The address of ROM file buffer which will be used to write into the flash part.
    @param Buffer - The ROM file buffer.
    @param Size - The ROM file size.
    @param Section - The FV type.
    @param Frequency - The executed frequency of Call back function.
    @param Progress - The call back function.
    
    @retval EFI_STATUS - The result
*/
EFI_STATUS ReadBlock2(
IN UINT64 Location,
IN UINT64 Address,
IN UINT64 Buffer,
IN UINT64 Size,
IN UINTN Section,
IN UINTN Frequency,
IN CallBackFn_T Progress
)
{
    EFI_STATUS Status;
    UINTN TempSize;
    UINTN Offset;
    UINT64 Timer;
    UINTN Second;

    if(NULL != Progress)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }

    Flash->DeviceWriteEnable(); 

    TempSize = FLASH_BLOCK_SIZE;

    for(Offset = 0;Offset < Size;Offset += TempSize )
    {
        if ( Size <(Offset + TempSize))
            TempSize = Size - Offset;

        if(NULL != Progress && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            Progress((UINT64)(Address + Offset),(Address + Size),(UINT64)Size,(UINTN)(Section | READ_BLOCK));
        }

        Status = Flash->Read((UINT8*)(Location + Offset), (UINTN)TempSize,(UINT8*)(Buffer + Offset));
        
        if(Status != EFI_SUCCESS)
        {
            Flash->DeviceWriteDisable();
            return Status;
        }
    }
    Flash->DeviceWriteDisable();

    if(NULL != Progress)
        Progress((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | READ_BLOCK));

    return EFI_SUCCESS;
}

/**
    WriteBlock2()
    
    Update the ROM by block.
    
    @param Location -  The FV address.
    @param Address -  The address of ROM file buffer which will be used to write into the flash part.
    @param Buffer - The ROM file buffer.
    @param Size - The ROM file size.
    @param Section - The FV type.
    @param Frequency - The executed frequency of Call back function.
    @param Progress - The call back function.
    
    @retval EFI_STATUS - The result
*/
EFI_STATUS WriteBlock2(
IN UINT64 Location,
IN UINT64 Address,
IN UINT64 Buffer,
IN UINT64 Size,
IN UINTN Section,
IN UINTN Frequency,
IN CallBackFn_T Progress
)
{
    EFI_STATUS Status;
    UINTN TempSize;
    UINTN Offset;
    UINT64 Timer;
    UINTN Second;

    if(NULL != Progress)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }

    Flash->DeviceWriteEnable(); 

    TempSize = FLASH_BLOCK_SIZE;

    for(Offset = 0;Offset < Size;Offset += TempSize )
    {
        if ( Size <(Offset + TempSize))
            TempSize = Size - Offset;

        if(NULL != Progress && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            Progress((UINT64)(Address + Offset),(Address + Size),(UINT64)Size,(UINTN)(Section | WRITE_BLOCK));
        }


        /*
        if(Status == EFI_SUCCESS)
        {
            if(Section & FLASH_OA3_BLOCK)
                Status = Flash->Write((UINT8*)(Location + Offset), (UINTN)TempSize,(UINT8*)(Buffer + Offset));
            else
                Status = Flash->Write((UINT8*)(Location + Offset), (UINTN)TempSize,(UINT8*)(Buffer + Address + Offset));
        }
        */
        
        Status = Flash->Write((UINT8*)(Location + Offset), (UINTN)TempSize,(UINT8*)(Buffer + Address + Offset));
        
        if(Status != EFI_SUCCESS)
        {
            Flash->DeviceWriteDisable();
            return Status;
        }
    }
    Flash->DeviceWriteDisable();

    if(NULL != Progress)
        Progress((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | WRITE_BLOCK));

    return EFI_SUCCESS;
}

/**
    VerifyBlock2()
    
    Verify the ROM by block.
    
    @param Location -  The FV address.
    @param Address -  The address of ROM file buffer which will be used to write into the flash part.
    @param Buffer - The ROM file buffer.
    @param Size - The ROM file size.
    @param Section - The FV type.
    @param Frequency - The executed frequency of Call back function.
    @param Progress - The call back function.
    
    @retval EFI_STATUS - The result
*/
EFI_STATUS VerifyBlock2(
IN UINT64 Location,
IN UINT64 Address,
IN UINT64 Buffer,
IN UINT64 Size,
IN UINTN Section,
IN UINTN Frequency,
IN CallBackFn_T Progress
)
{
    EFI_STATUS Status;
    UINTN TempSize;
    UINTN Offset;
    UINT64 Timer;
    UINTN Second;
    INTN Result;
    VOID *Buf;

    Buf = Malloc(FLASH_BLOCK_SIZE);
   
    if (Buf == NULL)
    {
        Status = ERROR_BUFFER_ALLOC;
        return Status;
    }
    
    if(NULL != Progress)
    {
        Timer = 0;
        if(Frequency!= 0)
            Second = Frequency;
        else
            Second = (UINTN)CALLBACK_TIMER_Default;
    }

    Flash->DeviceWriteEnable(); 

    TempSize = FLASH_BLOCK_SIZE;

    for(Offset = 0;Offset < Size;Offset += TempSize )
    {
        if ( Size <(Offset + TempSize))
            TempSize = Size - Offset;

        if(NULL != Progress && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            Progress((UINT64)(Address + Offset),(Address + Size),(UINT64)Size,(UINTN)(Section | VERIFY_BLOCK));
        }

        Status = Flash->Read((UINT8*)(Location + Offset), (UINTN)TempSize,(UINT8*)(Buf));
        
        if(Status != EFI_SUCCESS)
        {
            Flash->DeviceWriteDisable();
            return Status;
        }
        
        Result = MemCmp((VOID *)(Buffer + Location + Offset) , (VOID *)(Buf), TempSize);
        if (Result != 0)
        {
            Flash->DeviceWriteDisable();
            return EFI_DEVICE_ERROR;
        }
    }
    Flash->DeviceWriteDisable();

    if(NULL != Progress)
        Progress((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(Section | VERIFY_BLOCK));

    if(Buf)
        pBS->FreePool(Buf);
    
    return EFI_SUCCESS;
}
/**
    VerifyHeaderChecksum()
    
    Verify checksum of the FFS file header.
    
    @param *FfsHeader -  The FFS header buffer

    @retval BOOLEAN - The result
*/
BOOLEAN
VerifyHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FfsHeader
  )
{
    UINT32            Index;
    UINT8             *ptr;
    UINT8             HeaderChecksum;
    
    ptr = (UINT8 *)FfsHeader;
    HeaderChecksum = 0;
    for (Index = 0; Index < sizeof(EFI_FFS_FILE_HEADER); Index++) {
    HeaderChecksum = (UINT8)(HeaderChecksum + ptr[Index]);
    }
    
    HeaderChecksum = HeaderChecksum - FfsHeader->State - FfsHeader->IntegrityCheck.Checksum.File;
    
    if (HeaderChecksum == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
    IsBufferErased()
    Check if a block of buffer is erased.
    @param ErasePolarity -  
    @param *InBuffer - The buffer to be checked
    @retval BOOLEAN - The result
*/
BOOLEAN
IsBufferErased (
  IN UINT8    ErasePolarity,
  IN VOID     *InBuffer,
  IN UINTN    BufferSize
  )
{
    UINTN   Count;
    UINT8   EraseByte;
    UINT8   *Buffer;
    
    if(ErasePolarity == 1) {
        EraseByte = 0xFF;
    } else {
        EraseByte = 0;
    }
    
    Buffer = InBuffer;
    for (Count = 0; Count < BufferSize; Count++) {
        if (Buffer[Count] != EraseByte) {
            return FALSE;
        }
    }
    
    return TRUE;
}
#if defined (AFU_MEUD_SUPPORT) && (AFU_MEUD_SUPPORT == 1)
//
//Reference from "Meud\Platform\CspMeud.c" file code +>>>
//
//----------------------------------------------------------------------------
// Local prototypes
typedef struct {
    UINT32  SpiBar;         // 
    UINT32  SpiRegrFdoc;    // 
    UINT32  SpiRegrFdod;    // 
    UINT32  HeciBus;        // 
    UINT32  HeciDevice;     // 
    UINT32  HeciFunction;   // 
    UINT32  HeciRegrHfs;   // 
    UINT32  MeDevCtrlBase;  // 
    UINT32  MeDisableBit;   // 
} ME_CFG;

#if !defined ME_DEFINITIONS
#define ME_DEFINITIONS {0, 0, 0, 0, 0, 0, 0, 0, 0}
#endif

const ME_CFG    MeCfg[] = ME_DEFINITIONS;

/**
    GetFlashCapacity()
    
    This function calculates the flash part size.
    
    @param VOID
    
    @retval UINT32 - Flash part size
*/
UINT32 GetFlashCapacity(VOID)
{   
    volatile UINT32 *FDOC;
    volatile UINT32 *FDOD;
    UINT32  FlashDescriptorSig = 0x0FF0A55A;
    static  FlashDensity = 0;
    UINT16  Components;
    UINT8   i,j;
    
    if(FlashDensity)
    return FlashDensity;
    
    FDOC = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdoc);
    FDOD = (UINT32*)(MeCfg->SpiBar + MeCfg->SpiRegrFdod);
    
    *FDOC = 0;
    
    if (*FDOD != FlashDescriptorSig)
    return 0;
    
    *FDOC = 0x04;
    Components = (*FDOD >> 8) & 0x03;
    
    *FDOC = 0x1000;
    j = *FDOD;
    
    for (i=0; i<(Components + 1); i++)
    {
        switch (j & 0x07)
        {
        case 0:
            FlashDensity += 0x80000;
            break;
        case 1:
            FlashDensity += 0x100000;
            break;
        case 2:
            FlashDensity += 0x200000;
            break;
        case 3:
            FlashDensity += 0x400000;
            break;
        case 4:
            FlashDensity += 0x800000;
            break;
        case 5:
            FlashDensity += 0x1000000;
            break;
        case 6:
            FlashDensity += 0x2000000;
            break;
        case 7:
            FlashDensity += 0x4000000;
            break;
        default:
            break;
        }
        j = j >> 4;
    }
    
    return  FlashDensity;   
}
#endif
//
// Reference from "Meud\Platform\CspMeud.c" file code <<<+
//----------------------------------------------------------------------

/**
    FileSizeCheck()

    This function check the if the ROM file size is valid.
    
    @param FileSize -  The size of ROM file
    @param *FlashCapacity - The pointer of flash part size.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS FileSizeCheck(UINT32 FileSize , UINT32 *FlashCapacity )
{
    UINT32 FlashPartSize = 0;

#if AFU_PROTOCOL_ARCH  == 1
    *FlashCapacity = FlashPartSize ;
    return EFI_SUCCESS;
#else


    if( FileSize < gRomSize)      
        return ERROR_FILE_SIZE;
#if defined (AFU_MEUD_SUPPORT) && (AFU_MEUD_SUPPORT == 1)
    FlashPartSize = ME_GetFlashCapacity();
    //FlashPartSize = GetFlashCapacity();   // Get Flash Base
    if((FileSize > gRomSize ) && (FileSize != FlashPartSize) )
        return ERROR_FILE_SIZE;
#endif      
    if((FileSize > gRomSize) && (FlashPartSize == 0))
        return ERROR_FILE_SIZE;

    *FlashCapacity = FlashPartSize ;
    return EFI_SUCCESS;

#endif

}
//----------------------------------------------------------------------


//---------------------------------------------------------------------------
// AFU Protocol Function Prototype Define
//---------------------------------------------------------------------------

EFI_STATUS  ValidateRomFile(
    IN  UINTN   RomFileBufAddr,
    IN  UINT32  RomFileSize
);
EFI_STATUS  BiosUpdate(
    IN  UINTN   RomFileBufAddr,
    IN  UINT32  RomFileSize,
    IN  UINTN   RomSection,     /* Providing /P /B /N options */
    IN  UINTN   Selection,      /* RomHole/NCB  flash options */
    IN  CallBackFn_T    CallBackFunc,
    IN  UINTN   Frequency,  /* Unit is millisecond*/
    IN  UINT32  UpdateFlag      /* Check preserve data by user hook */
);

EFI_STATUS  OA3Update(
    IN  UINTN  OAFileBufAddr,
    IN  UINT32  OAFileSize,
    IN  CallBackFn_T CallBackFunc,
    IN  UINTN   Frequency   /* Unit is micro second*/
);

EFI_STATUS  OA3Delete(
    IN  CallBackFn_T CallBackFunc,
    IN  UINTN   Frequency   /* Unit is micro second*/
);

EFI_STATUS GetFlashPolicy(
    IN  FLASH_POLICY_INFO_BLOCK     *pFlashPolicy
);

EFI_STATUS LoadFirmwareImage (
    IN  FUNC_BLOCK          *pFuncBlock     
);

EFI_STATUS SetFlashMethod (
    IN  FUNC_FLASH_SESSION_BLOCK    *pFlashSessionBlock
);

AMI_AFU_PROTOCOL  gAFUProtocol = {
    ValidateRomFile,
    BiosUpdate,
    OA3Update,
    OA3Delete,
    GetFlashPolicy,
    LoadFirmwareImage,
    SetFlashMethod
};
//---------------------------------------------------------------------------

/**
    AFUProtocol.ValidateRomFile()

    This function validates RomFile for ROM IDD, FFS checksum and ROM Size 
    
    @param RomFileBufAddr -  The  address of RomFile buffer
    @param RomFileSize - The size of RomFile buffer
    
    @retval EFI_STATUS, based on the result.
*/ 
EFI_STATUS ValidateRomFile(
    IN  UINTN   RomFileBufAddr,
    IN      UINT32  RomFileSize
)
{
    EFI_STATUS  Status = EFI_SUCCESS;   

    UINT8 Checksum8 = 0;
    UINT16 FvHeadSize = 0 , Checksum16 = 0;
    UINT32 BiosBase = 0 ,BiosSize = 0 ,FlashCapacity = 0;
    UINT32 FileLength ,OccupiedFileLength ,TailSize = 0 , TempRomSize = 0;
    UINT64 offset = 0 ,VolLength = 0 ,TempFfsSize = 0, i ,j;
    UINT64 OffsetForFVH = 0 ,TempMaxBlkSize = 0,SearchOffset = 0; 
    UINT8 *BiosBuf = NULL;
    UINT8 *TempBuf = NULL;
    UINT8 *FvTempBuf = NULL;
    EFI_FIRMWARE_VOLUME_HEADER *FVHeader;
    EFI_FFS_FILE_HEADER *FFSHeader;
    BOOLEAN ErasePolarity = FALSE;
    //CHAR8  *RomId[FWROMPROJECTID_LENGTH+1] = {0};
    CHAR8  *RomId[32] = {0};
    BOOLEAN BreakLoop = FALSE;
    CHAR8  *ProjectID = NULL;
    UINT16 Sun8 = 0 ,Sum16 = 0;

    UINT8        AFU_CheckRomIDCount;
    
    UINT32 BiosStartAddress = 0;
    UINT32 BiosBlockSize    = 0;
    
#if AFU_PROTOCOL_ARCH  == 1
    RomFileBufAddr = RomFileBufAddr + OEM_FIRMWARE_HEADER_SIZE;
    RomFileSize = RomFileSize - OEM_FIRMWARE_HEADER_SIZE;
#endif

#if AFU_PROTOCOL_ARCH  == 0
    SmiAllocateMemory();
    GetBiosInfo();  //SMI 0x25 to get on board bios size and block info 

    
    if(!gMEEnabled)
    {
        Status = MERF_Check();
        if(Status == EFI_SUCCESS)
            gMEEnabled = TRUE;
    }
    
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
        
#else
    Status = FileSizeCheck( RomFileSize , &FlashCapacity );
    if(Status != EFI_SUCCESS)
        goto Exit;
    /*
    if(FlashCapacity == RomFileSize)
        BiosStartAddress = FlashCapacity - gRomSize ;
    gRomSize = FLASH_SIZE;
    */
    BiosBase = 0;
    BiosSize = FLASH_SIZE;
#endif

    TempMaxBlkSize = MaxBlkSize;
    
    BiosBuf = Malloc(BiosSize);
    TempBuf = Malloc(TempMaxBlkSize);
    FvTempBuf = Malloc(TempMaxBlkSize);

    if(BiosBuf == NULL || TempBuf == NULL || FvTempBuf == NULL) 
    {
        Status = ERROR_BUFFER_ALLOC;
        goto Exit;
    }
    
    MemCpy(BiosBuf, (UINT8 *)(RomFileBufAddr + BiosBase),BiosSize);
    //Check FFS checksum

    for(OffsetForFVH = 0; OffsetForFVH < BiosSize;)
    {
        if((OffsetForFVH + TempMaxBlkSize) > BiosSize )
            TempMaxBlkSize = BiosSize - OffsetForFVH;

        MemCpy(TempBuf,BiosBuf + OffsetForFVH,TempMaxBlkSize);
        SearchOffset = 0;

        while(SearchOffset<TempMaxBlkSize)
        {
            if (0 == MemCmp (TempBuf + SearchOffset, "_FVH", 4))
            {//check FVH checksum

                if (SearchOffset < 40 )
                {
                    SearchOffset++;
                    continue;
                }

                MemCpy(TempBuf,BiosBuf + OffsetForFVH + SearchOffset - 40,TempMaxBlkSize);
                FVHeader = (EFI_FIRMWARE_VOLUME_HEADER *)TempBuf;

                if ((MemCmp(&FVHeader->FileSystemGuid, &gEfiFirmwareFileSystemGuid, sizeof(EFI_GUID)) != 0)&&
                    (MemCmp(&FVHeader->FileSystemGuid, &gEfiFirmwareFileSystem2Guid, sizeof(EFI_GUID)) != 0))
                {
                    SearchOffset++;
                    continue;
                }

                FvHeadSize = FVHeader->HeaderLength;

                if(FVHeader->Attributes & EFI_FVB_ERASE_POLARITY)
                    ErasePolarity = TRUE;
                else
                    ErasePolarity = FALSE;          
                Checksum16 = CalculateSum16 ((UINT16 *)TempBuf, FvHeadSize );
                if(Checksum16 != 0)
                    Status =  ERROR_FILE_CHECKSUM;

                offset = OffsetForFVH + SearchOffset - 40;
                break;
            }
            else
                SearchOffset++;

        }//while
        
        if( Status != EFI_SUCCESS)
        {
            break;
        }
        //EOF encounter
        if(SearchOffset >= TempMaxBlkSize)
        {
            OffsetForFVH += TempMaxBlkSize;
            continue;
        }
        offset += FVHeader->HeaderLength;
        VolLength = FVHeader->FvLength;

        for(i = FVHeader->HeaderLength; i<VolLength;)
        {
            if( (VolLength - i ) < sizeof (EFI_FFS_FILE_HEADER) )
                break;
            MemCpy(FvTempBuf,BiosBuf + offset,sizeof (EFI_FFS_FILE_HEADER));
            FFSHeader = (EFI_FFS_FILE_HEADER *)FvTempBuf;

            if(IsBufferErased(ErasePolarity,FFSHeader,sizeof (EFI_FFS_FILE_HEADER)))
                break;
            // Verify file header checksum
            if(VerifyHeaderChecksum(FFSHeader)== FALSE)
            {
                Status =  ERROR_FILE_CHECKSUM;
                break;
            }       
            // Verify file checksum
            FileLength = (*(UINT32 *)FFSHeader->Size) & 0xffffff;
            OccupiedFileLength  = (FileLength + 7) & ~7;

            if (FFSHeader->Attributes & FFS_ATTRIB_CHECKSUM ) 
            {
                // Verify file data checksum
                TempFfsSize = TempMaxBlkSize;
                Checksum8 = 0;

                for(j = 0; j < FileLength - TailSize; j += TempFfsSize)
                {
                    if((j + TempFfsSize) > FileLength - TailSize)
                        TempFfsSize = FileLength - TailSize - j;

                    MemCpy(TempBuf,BiosBuf + offset + j,TempFfsSize);
                    Checksum8 += CalculateSum8 ((UINT8 *)TempBuf, (UINT32)TempFfsSize);
                }               

                Checksum8 = (UINT8) (Checksum8 - FFSHeader->State);

                if (Checksum8 != 0)
                {
                    Status =  ERROR_FILE_CHECKSUM;
                    break;
                }
            } 
            else
            {
                // File does not have a checksum, Verify contents are 0x5A/0xAA as spec'd
                if (FFSHeader->IntegrityCheck.Checksum.File != FFS_FIXED_CHECKSUM && 
                     FFSHeader->IntegrityCheck.Checksum.File != FFS_FIXED_CHECKSUM_1_0)
                {
                    Status =  ERROR_FILE_CHECKSUM;
                    break;
                }
            }
            offset += OccupiedFileLength;
            i += OccupiedFileLength;
        }
        if( Status != EFI_SUCCESS)
            break;
        OffsetForFVH += VolLength;
        offset = OffsetForFVH ;
    }//FFS checksum end


    if( Status == EFI_SUCCESS)
    {
        Status = ERROR_ROM_ID;
        //Check the ROM ID from the file
        for( TempRomSize = BiosSize - PAGE_SIZE; TempRomSize < BiosSize;
        TempRomSize -= (PAGE_SIZE - sizeof(FW_VERSION)) )
        {
            MemCpy(TempBuf, BiosBuf + TempRomSize, PAGE_SIZE);      
            // Search FWID from ROM file
            for (i = 0; i < (PAGE_SIZE - sizeof(FW_VERSION)); i++)
            {
                if (0 == MemCmp (TempBuf + i, "$FID", FWID_SIG_LENGTH))
                {
#if AFU_PROTOCOL_ARCH  == 0 
                    if (g_OnBoardROMProjectIDLength != 0xFF)
                        AFU_CheckRomIDCount = g_OnBoardROMProjectIDLength;
                    else
                        AFU_CheckRomIDCount = FWROMPROJECTID_LENGTH;
#else
                    AFU_CheckRomIDCount = FWROMPROJECTID_LENGTH;
#endif
                    
                    MemSet(RomId, sizeof(RomId), 0);
                    
                    
                    MemCpy(RomId, &(((FW_VERSION *)(TempBuf+i))->BiosTag[0]), AFU_CheckRomIDCount + 1);
                    //MemCpy(RomId, &(((FW_VERSION *)(TempBuf+i))->BiosTag[2]), AFU_CheckRomIDCount + 1);
                    //it's a workaround, some arm projects write L" in the binary , so we have to set offset 2
                    //for example: L"0ACHD70 in rom file
                    

                    ProjectID = CONVERT_TO_STRING(PROJECT_TAG);

                    if( 0 == MemCmp(RomId,ProjectID ,AFU_CheckRomIDCount )){
                        Status =  EFI_SUCCESS;
                        BreakLoop = TRUE;
                        break;
                    }

                }
            }
        }
    }//Check ROM ID end

Exit:
#if AFU_PROTOCOL_ARCH  == 0
    SmiFreeMemory();
#endif
    
    if(FvTempBuf)
        pBS->FreePool(FvTempBuf);
    if(TempBuf)
        pBS->FreePool(TempBuf);
    if(BiosBuf)
        pBS->FreePool(BiosBuf);
    return Status;
}

/**
    AFUProtocol.BiosUpdate()

    This function programs BIOS region or ME region.
    
    @param RomFileBufAddr -  The  address of RomFile buffer
    @param RomFileSize - The size of RomFile buffer
    @param RomSection - The BIOS region which section would be update.
    @param Selection - The selection for RomHole and NCB 
    @param CallBackFunc - The CallBack function
    @param Frequency - The executed frequency of call back function.
    @param UpdateFlag - The ability to retain data.
    
    @retval EFI_STATUS, based on the result.
*/ 
EFI_STATUS BiosUpdate(
    IN  UINTN   RomFileBufAddr,
    IN  UINT32  RomFileSize,
    IN  UINTN   RomSection,  
    IN  UINTN   Selection,  
    IN  CallBackFn_T    CallBackFunc,
    IN  UINTN   Frequency,
    IN  UINT32  UpdateFlag
)
{
#if defined (AFU_SMIPROTOCOL_SUPPORT) && (AFU_SMIPROTOCOL_SUPPORT == 1)
    EFI_STATUS Status = EFI_SUCCESS;

    Status = SmiBiosUpdate(RomFileBufAddr, RomFileSize, RomSection, Selection, CallBackFunc, Frequency, UpdateFlag);
    
    return Status;
#else   
    
    EFI_STATUS    Status = EFI_SUCCESS;
    UINT8 *BiosBuff = NULL;
    UINT8 *MEBuf = NULL;
    UINT32 BiosSize = 0 ,BiosBase = 0;
    UINT32 FlashCapacity = 0,MEBase = 0 ,MESize = 0;
    UINTN  i;

    
    Status = FileSizeCheck( RomFileSize , &FlashCapacity ); 
    if(Status != EFI_SUCCESS)
        return Status;
    
    if(FlashCapacity == RomFileSize)
        BiosBase = FlashCapacity - FLASH_SIZE ; 
    
    BiosSize = FLASH_SIZE ;

    //Start update BIOS 
    BiosBuff = Malloc(BiosSize);    
    if(BiosBuff == NULL) 
        return ERROR_BUFFER_ALLOC;

    MemCpy(BiosBuff,    (UINT8 *)(RomFileBufAddr + BiosBase),BiosSize); 

    for (i = 0; BIOSPreUpdate[i] != NULL; i++) 
        BIOSPreUpdate[i](UpdateFlag);
    
    //Boot Block update
    if((RomSection & FLASH_BOOT_BLOCK) && (Status == EFI_SUCCESS))
    {   
        Status = EraseBlock2((UINT64)AMI_ROM_LAYOUT_FV_BB_ADDRESS,(UINT64)BootBlockAdds,(UINT64)BiosBuff,(UINT64)BootBlockSize,(UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_BOOTBLOCK ;
        
        Status = WriteBlock2((UINT64)AMI_ROM_LAYOUT_FV_BB_ADDRESS,(UINT64)BootBlockAdds,(UINT64)BiosBuff,(UINT64)BootBlockSize,(UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_BOOTBLOCK ;
        
        Status = VerifyBlock2((UINT64)AMI_ROM_LAYOUT_FV_BB_ADDRESS,(UINT64)BootBlockAdds,(UINT64)BiosBuff,(UINT64)BootBlockSize,(UINTN)FLASH_BOOT_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_BOOTBLOCK ;
    }
    //Main BIOS update
    if((RomSection & FLASH_MAIN_BLOCK ) && (Status == EFI_SUCCESS))
    {
        Status = EraseBlock2((UINT64)AMI_ROM_LAYOUT_FV_MAIN_ADDRESS,(UINT64)MainBlockAdds,(UINT64)BiosBuff,(UINT64)MainBlockSize,(UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_MAINBIOS ;
                
        Status = WriteBlock2((UINT64)AMI_ROM_LAYOUT_FV_MAIN_ADDRESS,(UINT64)MainBlockAdds,(UINT64)BiosBuff,(UINT64)MainBlockSize,(UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_MAINBIOS ;
        
        Status = VerifyBlock2((UINT64)AMI_ROM_LAYOUT_FV_MAIN_ADDRESS,(UINT64)MainBlockAdds,(UINT64)BiosBuff,(UINT64)MainBlockSize,(UINTN)FLASH_MAIN_BLOCK,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_MAINBIOS ;
    }
    //NVram update
    if((RomSection & FLASH_NVRAM ) && (Status == EFI_SUCCESS))
    {
#ifdef FAULT_TOLERANT_NVRAM_UPDATE
#if FAULT_TOLERANT_NVRAM_UPDATE
        NvRamSize = NvRamSize * 2;
#endif
#endif
        Status = EraseBlock2((UINT64)AMI_ROM_LAYOUT_NVRAM_ADDRESS,(UINT64)NvRamStart,(UINT64)BiosBuff,(UINT64)NvRamSize,(UINTN)FLASH_NVRAM,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_NVRAM ;
        
        Status = WriteBlock2((UINT64)AMI_ROM_LAYOUT_NVRAM_ADDRESS,(UINT64)NvRamStart,(UINT64)BiosBuff,(UINT64)NvRamSize,(UINTN)FLASH_NVRAM,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_NVRAM ;
        
        Status = VerifyBlock2((UINT64)AMI_ROM_LAYOUT_NVRAM_ADDRESS,(UINT64)NvRamStart,(UINT64)BiosBuff,(UINT64)NvRamSize,(UINTN)FLASH_NVRAM,(UINTN)Frequency,CallBackFunc);
        if(Status != EFI_SUCCESS)
            Status = ERROR_FLASH_NVRAM ;
    }
    if((RomSection & FLASH_NEWER_EC_BLOCK) && (Status == EFI_SUCCESS))
    {   
            Status =  ERROR_EC_UNSUPPORT;
    }
    
    if((RomSection & FLASH_EC_BLOCK ) && (Status == EFI_SUCCESS))
    {       
        Status =  ERROR_EC_UNSUPPORT;
    }
    
    if((RomSection & FLASH_NON_CRITICAL_BLOCK ) && (Status == EFI_SUCCESS))
    {
        Status = ERROR_NCB_UNSUPPORT;
    }
    
    if((RomSection & FLASH_ROM_HOLE ) && (Status == EFI_SUCCESS))
    {
        Status = ERROR_RH_UNSUPPORT;        
    }

    for (i = 0; BIOSEndUpdate[i] != NULL; i++) 
        BIOSEndUpdate[i](UpdateFlag);
    
    if(BiosBuff)
        pBS->FreePool(BiosBuff);
//update BIOS end

//Start update ME
    if((RomSection & FLASH_FULL_ME ) && (Status == EFI_SUCCESS))
    {   
        Status = ERROR_ME_UNSUPPORT ;
    }// update ME end

    return Status;

#endif    
}

/**
    AFUProtocol.OA3Update()

    This function programs OEM Activation 3 Blocks.
    
    @param OAFileBufAddr -  The  address of RomFile buffer
    @param OAFileSize - The size of RomFile buffer
    @param CallBackFunc - The Call back function
    @param Frequency - The executed frequency of Call back function.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS EFIAPI OA3Update(
    IN  UINTN  OAFileBufAddr,
    IN  UINT32  OAFileSize,
    IN  CallBackFn_T CallBackFunc,
    IN  UINTN   Frequency
)
{

#if defined (AFU_SMIPROTOCOL_SUPPORT) && (AFU_SMIPROTOCOL_SUPPORT == 1)
    EFI_STATUS    Status;
    
    Status = SmiOA3Update(OAFileBufAddr, OAFileSize, CallBackFunc, Frequency);
    
    return Status;
#else   


#if OEM_ACTIVATION_TABLE_LOCATION ==1
    EFI_STATUS    Status;

    Flash->DeviceWriteEnable(); 

    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0,(UINT64)0,(UINT64)OEM_ACTIVATION_TABLE_SIZE,(UINT64)(FLASH_OA3_BLOCK | WRITE_BLOCK));

    Status = Flash->Write((UINT32*)OEM_ACTIVATION_TABLE_ADDRESS,(UINTN)OEM_ACTIVATION_TABLE_SIZE,(UINT8*)OAFileBufAddr);

    Flash->DeviceWriteDisable();

    if(Status != EFI_SUCCESS)
        return ERROR_FLASH_OA;
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(UINT64)(FLASH_OA3_BLOCK | WRITE_BLOCK));

    return EFI_SUCCESS;
#else
    return EFI_UNSUPPORTED;
#endif
    
#endif

}

/**
    AFUProtocol.OA3Delete()

    This function deletes OEM Activation 3 Blocks.
    
    @param CallBackFunc - The Call back function
    @param Frequency - The executed frequency of Call back function.
    
    @retval EFI_STATUS, based on the result.
*/
EFI_STATUS EFIAPI OA3Delete(
    IN  CallBackFn_T CallBackFunc,
    IN  UINTN   Frequency
)
{
    
#if defined (AFU_SMIPROTOCOL_SUPPORT) && (AFU_SMIPROTOCOL_SUPPORT == 1)
    EFI_STATUS    Status;
    
    Status = SmiOA3Delete(CallBackFunc, Frequency);
    
    return Status;
#else   


#if OEM_ACTIVATION_TABLE_LOCATION ==1
    EFI_STATUS    Status;

    UINT8* OAFileBufAddr = NULL;
    
    Flash->DeviceWriteEnable(); 

    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0,(UINT64)0,(UINT64)OEM_ACTIVATION_TABLE_SIZE,(UINT64)(FLASH_OA3_BLOCK | WRITE_BLOCK));

    OAFileBufAddr = Malloc(FLASH_BLOCK_SIZE);
    MemSet(OAFileBufAddr, FLASH_BLOCK_SIZE, 0xFF);
    
    Status = Flash->Write((UINT32*)OEM_ACTIVATION_TABLE_ADDRESS,(UINTN)OEM_ACTIVATION_TABLE_SIZE,(UINT8*)OAFileBufAddr);

    Flash->DeviceWriteDisable();

    if(Status != EFI_SUCCESS)
        return ERROR_FLASH_OA;
    
    if(NULL != CallBackFunc)
        CallBackFunc((UINT64)0xffffffff,(UINT64)0,(UINT64)0,(UINT64)(FLASH_OA3_BLOCK | WRITE_BLOCK));

    return EFI_SUCCESS;
#else
    return EFI_UNSUPPORTED;
#endif
    
#endif
    
}
/**
    GeneratesOFBDHeader()

    This function generates OFBD header.
    
    @param Command -  OFBD command

    @retval VOID
*/ 
VOID * GeneratesOFBDHeader( UINT8 Command )
{
#if AFU_PROTOCOL_ARCH  == 1
    return;
#else

    POFBD_SECURE_FLASH  pOFBD_SecFlash;
    POFBD_TYPECODE_V2 pOFBDTCV2;
    POFBD_EXTHDR_END pOFBD_ExtEnd;

    //MemSet( gOFBDBuffer , 1024 *64 ,0 );
    MemSet( gOFBDBuffer , FLASH_PAGE_SIZE * FLASH_PAGE_COUNT ,0 );
    
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->ddSignature = 0x4442464F ;
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwVersion = OFBD_VERSION ;
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->ddFlashStatus = OFBD_FS_SECURE_FLASH ;
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwSize = sizeof(OFBD_STRUCT_V2);
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwHdrSize = sizeof(OFBD_STRUCT_V2);
    ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwOS = 0 ;
    
    pOFBDTCV2 = (POFBD_TYPECODE_V2)(gOFBDBuffer + ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwSize);
    
    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_SECURE_FLASH);
    pOFBDTCV2->dbCode = OFBD_EXT_TC_SECURE_FLASH;
    pOFBDTCV2->dbNumHole = 1 ;
    
    ((POFBD_STRUCT_V2)gOFBDBuffer)->dwSize += sizeof(OFBD_TYPECODE_V2);

    pOFBD_SecFlash = (POFBD_SECURE_FLASH)(gOFBDBuffer + ((POFBD_STRUCT_V2)gOFBDBuffer)->dwSize);
    
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)gOFBDBuffer;
    
    ((POFBD_STRUCT_V2)gOFBDBuffer)->dwSize += (UINT16)(sizeof(OFBD_SECURE_FLASH) * 1);
    
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)((unsigned long)pOFBD_ExtEnd + (unsigned long) ((POFBD_STRUCT_V2 )gOFBDBuffer)->dwSize);
    
    pOFBD_ExtEnd->ddHdrEnd = OFBD_EXT_TBL_END;      // fill end value
    
    ((POFBD_STRUCT_V2)gOFBDBuffer)->dwSize += sizeof(OFBD_EXT_TBL_END);
    
    pOFBD_SecFlash->Command = Command;
     
    return  gOFBDBuffer;
#endif
}

/**
    SendOFBDSmi()

    This function issue SMI for X64
    
    @param VOID

    @retval VOID
*/ 
VOID SendOFBDSmi( VOID )
{
#if AFU_PROTOCOL_ARCH  == 1
    return;
#else

    UC2RegBuffer    regBuffer;          
    
    MemSet(&regBuffer, sizeof(UC2RegBuffer), 0);
    
    regBuffer.Ebx = (UINT32)(pPhyAddr & 0xffffffff);

    regBuffer.Ecx = (UINT32)(pPhyAddr >> 32);
            
    OutPortByte(0xB2, OFBD_SW_SMI_VALUE, &regBuffer);
#endif
    
}

/**
    AFUProtocol.GetFlashPolicy()

    This function Checks BIOS which policy is supported for secure flash.
    
    @param pFlashPolicy -  FLASH_POLICY_INFO_BLOCK structure

    @retval EFI_STATUS, based on the Secure SMI FLASH PROTOCOL.
*/ 
EFI_STATUS GetFlashPolicy(
        IN  FLASH_POLICY_INFO_BLOCK     *pFlashPolicy
)
{
#if AFU_PROTOCOL_ARCH  == 1
    pFlashPolicy = 0;
    return EFI_UNSUPPORTED;
#else
    EFI_STATUS                  Status = EFI_SUCCESS;

    UINT8 *OFBDBuffer = NULL;
    
    SmiAllocateMemory();
    
    gOFBDBuffer = gpIntrBuf;
    OFBDBuffer = GeneratesOFBDHeader( OFBD_TC_SFU_GET_FLASH_UPDATE_POLICY );
    
    gpFlashPolicyInfo = (FLASH_POLICY_INFO_BLOCK *)((UINTN )OFBDBuffer + ((POFBD_STRUCT_V2)OFBDBuffer)->dwSize );
    
    gpFlashPolicyInfo->ErrorCode = 1;
    
    SendOFBDSmi ();
    
    if (gpFlashPolicyInfo->ErrorCode == 1)
        Status = EFI_SECURITY_VIOLATION;
    
    MemCpy(pFlashPolicy, gpFlashPolicyInfo, sizeof(FLASH_POLICY_INFO_BLOCK));
    
    SmiFreeMemory();
    
    return Status ;
#endif
}

#if AFU_PROTOCOL_ARCH  == 0
/**
    LoadFirmwareImage_Ex()

    This function Loads ROM file into memory.
      
    @param RomData   -  The memory buffer stores ROM data block.
    @param BlockAddr -  The address of the block in ROM file.
    @param BlockSize -  The size of the buffer.

    @retval BOOL
*/ 
BOOL LoadFirmwareImage_Ex (
    IN  UINT8                   *RomData,
    IN  UINT32                  BlockAddr,
    IN  UINT32                  BlockSize
)
{

        UINT8 *OFBDBuffer = NULL;
        FUNC_BLOCK   *pFuncBlock = (FUNC_BLOCK*)NULL;
                
        // Prepare the OFBD header
        gOFBDBuffer = gpIntrBuf;
        OFBDBuffer = GeneratesOFBDHeader(OFBD_TC_SFU_LOAD_FIRMWARE_IMAGE);
        if (NULL == OFBDBuffer) return FALSE;

        pFuncBlock = (FUNC_BLOCK *)((UINTN )OFBDBuffer + ((POFBD_STRUCT_V2)OFBDBuffer)->dwSize );
        pFuncBlock->BufAddr     = (UINT64)gpIntrBuf + ((POFBD_STRUCT_V2)OFBDBuffer)->dwSize + sizeof(FUNC_BLOCK);
        
        // Block offset in ROM file
        pFuncBlock->BlockAddr   = BlockAddr;
        // Block size
        pFuncBlock->BlockSize   = BlockSize;
        pFuncBlock->ErrorCode   = 1;

        MemCpy((UINT8 *)pFuncBlock + sizeof(FUNC_BLOCK), RomData, BlockSize);

        pFuncBlock->ErrorCode = 1;
                
        SendOFBDSmi ();
                
        if (pFuncBlock->ErrorCode == 1)
            return FALSE;
        
        return TRUE;
}
#endif

/**
    AFUProtocol.LoadFirmwareImage()

    This function Loads ROM file into memory for secure flash.
    
    @param pFuncBlock -  FUNC_BLOCK structure

    @retval EFI_STATUS, based on the Secure SMI FLASH PROTOCOL.
*/ 
EFI_STATUS LoadFirmwareImage(
    IN  FUNC_BLOCK      *pFuncBlock
)
{
#if AFU_PROTOCOL_ARCH  == 1
    return EFI_UNSUPPORTED;
#else
    EFI_STATUS                  Status = EFI_SUCCESS;

    // EIP 377144 +>>>
    DWORD BiosStartAddress = 0;
    DWORD BiosBlockSize    = 0;
    // EIP 377144 <<<+

    UINT32 i,FlashCapacity,BufferSize = FLASH_PAGE_SIZE * FLASH_PAGE_COUNT / 2;
    
    SmiAllocateMemory();
    GetBiosInfo();  //SMI 0x25 to get on board bios size and block info 
    
    if( pFuncBlock->BlockSize  !=  gRomSize )
    {
        // EIP 377144 +>>>
        Status = MERF_Check();
        if(Status == EFI_SUCCESS)
        {
            gMEEnabled = TRUE;
            MER_GetBlkInfo(BIOS_BLK, &BiosStartAddress, &BiosBlockSize);
            ///debug+
            //Print(L"***LoadFirmwareImage()\n");
            //Print(L"   BiosStartAddress = 0x%08X\n", BiosStartAddress);
            //Print(L"   BiosBlockSize    = 0x%08X\n", BiosBlockSize);
        }
        if(!BiosStartAddress)
        {
            Status = FileSizeCheck( pFuncBlock->BlockSize , &FlashCapacity );
            if(Status != EFI_SUCCESS)
                goto Exit;
            BiosStartAddress = FlashCapacity - gRomSize;
        }
        
        pFuncBlock->BlockSize = gRomSize ;
        //pFuncBlock->BufAddr = pFuncBlock->BufAddr + FlashCapacity - FLASH_SIZE ;
        pFuncBlock->BufAddr = pFuncBlock->BufAddr + BiosStartAddress;
        // EIP 377144 <<<+
    }

    for (i = 0; i < pFuncBlock->BlockSize / BufferSize; i++)
    {
        if (FALSE == LoadFirmwareImage_Ex(
                        (UINT8*)(pFuncBlock->BufAddr + BufferSize * i),       // ROM Data
                        pFuncBlock->BlockAddr + BufferSize * i,     // Offset in ROM file
                        BufferSize))                    // Size of each block
        {
            Status =  EFI_SECURITY_VIOLATION;
            goto Exit;
        }
    }
    
    if (pFuncBlock->BlockSize % BufferSize)
    {
        if (FALSE == LoadFirmwareImage_Ex(
                        (UINT8*)(pFuncBlock->BufAddr + BufferSize * i),       // ROM Data
                        pFuncBlock->BlockAddr + BufferSize * i,     // Offset in ROM file
                        pFuncBlock->BlockSize % BufferSize))           // Size of each block
        {
            Status =  EFI_SECURITY_VIOLATION;
            goto Exit;
        }
    }

Exit:
    SmiFreeMemory();
    
    return Status ;

#endif
}

#if AFU_PROTOCOL_ARCH  == 0

EFI_STATUS SetFlashMethod_Internal(
    IN  FUNC_FLASH_SESSION_BLOCK        *pFlashSessionBlock
)
{

    EFI_STATUS                  Status = EFI_SUCCESS;
    UINT8 *OFBDBuffer = NULL;

    UINT32 FlashCapacity;
    UINT32 BiosSize = 0,FlashBase = 0;   
    

    SmiAllocateMemory();
    
    if( pFlashSessionBlock->FlUpdBlock.ImageSize  !=  gRomSize )
    {
        Status = FileSizeCheck( pFlashSessionBlock->FlUpdBlock.ImageSize , &FlashCapacity );

        if(Status != EFI_SUCCESS)
            goto Exit;          
    
        pFlashSessionBlock->FlUpdBlock.ImageSize = gRomSize ;
    }
    
    
    gOFBDBuffer = gpIntrBuf;
    
    OFBDBuffer =  GeneratesOFBDHeader( OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD );

    gpFlashSessionBlock = (FUNC_FLASH_SESSION_BLOCK *)((UINTN )OFBDBuffer + ((POFBD_STRUCT_V2)OFBDBuffer)->dwSize );
        
    MemCpy(gpFlashSessionBlock, pFlashSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK));

    SendOFBDSmi ();

    if (gpFlashSessionBlock->ErrorCode == 1)
        Status = EFI_SECURITY_VIOLATION;

    MemCpy(pFlashSessionBlock, gpFlashSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK));

Exit:

    SmiFreeMemory();
    
    return Status ;
}
#endif

/**
    AFUProtocol.SetFlashMethod()

    This function informs BIOS which flash method will be taken.
    
    @param pFlashSessionBlock -  FUNC_FLASH_SESSION_BLOCK structure

    @retval EFI_STATUS, based on the Secure SMI FLASH PROTOCOL.
*/ 
EFI_STATUS SetFlashMethod(
    IN  FUNC_FLASH_SESSION_BLOCK        *pFlashSessionBlock
)
{
#if AFU_PROTOCOL_ARCH  == 1
    return EFI_UNSUPPORTED;
#else
    EFI_STATUS                  Status = EFI_SUCCESS;
    UINT8 *OFBDBuffer = NULL;

    UINT32 FlashCapacity;
    UINT32 BiosSize = 0,FlashBase = 0;   

    UINT8   Selection = 0;
    
#if defined (BIOS_CONFIG_PRESERVE_BIN_SUPPORT) && (BIOS_CONFIG_PRESERVE_BIN_SUPPORT == 1)
    UINT8   FlashOpType = RECOVERY_FLASH_UPDATE;
#endif
    
    int         FunctionRes = 0;
    
    SmiAllocateMemory();
    GetBiosInfo();  //SMI 0x25 to get on board bios size and block info 
         
    Selection = (UINT8)(pFlashSessionBlock->FlUpdBlock.ROMSection >> 24);//get the highest byte of ROMSection
    if (Selection)
        pFlashSessionBlock->FlUpdBlock.ROMSection = pFlashSessionBlock->FlUpdBlock.ROMSection & 0x00FFFFFF;
    
#if defined (BIOS_CONFIG_PRESERVE_BIN_SUPPORT) && (BIOS_CONFIG_PRESERVE_BIN_SUPPORT == 1)
    if (pFlashSessionBlock->FlUpdBlock.FlashOpType == FlRecovery)
        FlashOpType = RECOVERY_FLASH_UPDATE;
    else if (pFlashSessionBlock->FlUpdBlock.FlashOpType == FlCapsule)
        FlashOpType = CAPSULE_FLASH_UPDATE;
    
    if (pFlashSessionBlock->FlUpdBlock.FlashOpType != FlRuntime)
    {
        if (pFlashSessionBlock->FlUpdBlock.ROMSection & FLASH_NVRAM)
        {
            if (!(Selection & CLRCFG))
            {
                //Enable flash
                FunctionRes = EnableFlashIfc();
                if(FunctionRes != 0)
                {
                    DisableFlashIfc();
                    Status = EFI_DEVICE_ERROR;
                    goto Exit;
                }
                
                CreatePldmTable(Selection,FlashOpType);
                
                //Disable flash
                FunctionRes = DisableFlashIfc();
                if(FunctionRes != 0)
                {
                    Status = EFI_DEVICE_ERROR;
                    goto Exit;
                }
            }
        }
    }
#endif
    

    if( pFlashSessionBlock->FlUpdBlock.ImageSize  !=  gRomSize )
    {
        Status = FileSizeCheck( pFlashSessionBlock->FlUpdBlock.ImageSize , &FlashCapacity );

        if(Status != EFI_SUCCESS)
            goto Exit;          
    
        pFlashSessionBlock->FlUpdBlock.ImageSize = gRomSize ;
    }
    
    
    gOFBDBuffer = gpIntrBuf;
    
    OFBDBuffer =  GeneratesOFBDHeader( OFBD_TC_SFU_SET_FLASH_UPDATE_METHOD );

    gpFlashSessionBlock = (FUNC_FLASH_SESSION_BLOCK *)((UINTN )OFBDBuffer + ((POFBD_STRUCT_V2)OFBDBuffer)->dwSize );
        
    MemCpy(gpFlashSessionBlock, pFlashSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK));

    SendOFBDSmi ();

    if (gpFlashSessionBlock->ErrorCode == 1)
        Status = EFI_SECURITY_VIOLATION;

    MemCpy(pFlashSessionBlock, gpFlashSessionBlock, sizeof(FUNC_FLASH_SESSION_BLOCK));

Exit:

    SmiFreeMemory();
    
    return Status ;
#endif
}

/**
    AFUProtocolInit()

    This function initializes all variables
    
    @param VOID

    @retval VOID
*/ 
VOID AFUProtocolInit()
{
    UINT64 CurrentTicker;
    UINT64 TimerPeriod;
    EFI_CPU_ARCH_PROTOCOL *Cpu;

#if defined (AFU_SMIPROTOCOL_SUPPORT) && (AFU_SMIPROTOCOL_SUPPORT == 1)

#else

    //Boot Block Info
    BootBlockAdds = AMI_ROM_LAYOUT_FV_BB_OFFSET;
    BootBlockSize = AMI_ROM_LAYOUT_FV_BB_SIZE;//FV_BB_BLOCKS * FLASH_BLOCK_SIZE;

    //Main Block Info
    MainBlockAdds = AMI_ROM_LAYOUT_FV_MAIN_OFFSET;
    MainBlockSize = AMI_ROM_LAYOUT_FV_MAIN_SIZE;//FV_MAIN_BLOCKS * FLASH_BLOCK_SIZE;

    //NVram Info
    NvRamStart = AMI_ROM_LAYOUT_NVRAM_OFFSET;
    NvRamSize = AMI_ROM_LAYOUT_NVRAM_SIZE;//NVRAM_SIZE;

#endif      
    //Get CPU Frequency
    pBS->LocateProtocol (
          &gEfiCpuArchProtocolGuid,
          NULL,
          &Cpu
          );

    Cpu->GetTimerValue (Cpu, 0, &(CurrentTicker), &TimerPeriod);

    Freq = Div64(1000000000000, (UINTN) TimerPeriod, NULL);
    
    return;
}
/**
    AFUProtocolEntryPoint()

    Entry point of the AFUProtocol
    
    @param ImageHandle - Image handle
    @param SystemTable - Pointer to the system table

    @retval EFI_STATUS, based on the result.
*/ 
EFI_STATUS AFUProtocolEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status;
    EFI_HANDLE  DummyHandle = NULL;
    
    InitAmiLib(ImageHandle,SystemTable);
    
    VERIFY_EFI_ERROR(pBS->LocateProtocol(&gFlashProtocolGuid,NULL, &Flash));

    Status = pBS->InstallMultipleProtocolInterfaces(
                 &DummyHandle,
                 &gAFUProtocolGuid,
                 &gAFUProtocol,
                 NULL
             );
   
    ASSERT_EFI_ERROR(Status);

    AFUProtocolInit();

    return Status;
}

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

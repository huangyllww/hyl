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
MeInterface.c

This is the ME flash function file for X64.

*/
//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/CPU.h>
#include <PPI\FwVersion.h>
#include <Flash.h>
#include <Setup.h>
#include <FFS.h>
#include <AmiLib.h>
#include <EFI.h>

#include <Protocol/AmiAfuProtocol.h>
#include <AmiAfuProtocolElinks.h>

#include <Library/UefiLib.h>

#include "AfuDriver.h"
#include "SmiInterface.h"
#include "MeInterface.h"
#include "ofbd_cfg.h"


static POFBD_MEUD   g_ME_pOFBD_MEInfo = (POFBD_MEUD)NULL;
static PBYTE        g_ME_pMEBuffer = (PBYTE)NULL;
static DWORD        g_ME_dMEBufferSize = 0;

static DWORD        g_ME_dMEFileSize = 0;
static BOOLEAN      g_ME_bIsFoundME = FALSE;
static BOOLEAN      g_ME_bIsUseSingle = FALSE;
static BOOLEAN      g_ME_bISRunGetBIOSAddr = FALSE;
static BOOLEAN      g_ME_bMEIsSupport = FALSE;
static BOOLEAN      g_ME_bIgMEIsSupport = FALSE;
static BOOLEAN      g_ME_bGetBIOSAddrSeting_Ret = FALSE;
static BOOLEAN      g_ME_bCheckMEDataFirst = FALSE;
static BOOLEAN      g_ME_bCoffeeLakeME = FALSE;  // EIP 354005/377144 +
static ENUM_ERR_ID  gErrorID = eEID_NO_ERROR;

static DWORD        g_MER_ddFlash_Page_Size = FLASH_PAGE_SIZE*20;
static MEBlkInfo*   g_MER_BlkDataInfo = (PMEBlkInfo)NULL;

static DWORD        CmdTotalCnt = 0;
static BOOLEAN*     CmdEnable = (BOOLEAN*)NULL;
static CHAR         ErrorStr[4] = {0};

//static UINT32       gPrgressSize = 0;
static UINT32       gProgressOffset = 0;

extern WORD     g_OfbdVer;
extern UINT8 *  gpIntrBuf;
extern UINTN    gIntrBufPhyAddr;

extern UINT8 *  gpMEBuff;
extern UINT32   gMESize;
extern UINT32   gMERealFlashSize;

extern BOOLEAN CallBackExecuteFrequency( UINTN MicroSecond, UINT64 Ticker );

UINT16      ME_CalculateSum16 (UINT16* Buffer, UINT32 Size);

UINT32      ME_GetFlashCapacity();
BOOLEAN     ME_VersionCheck();  // EIP 354005/377144 +
BOOLEAN     ME_CheckSupport();
BOOLEAN     ME_Ignition_CheckSupport();
BOOLEAN     ME_CheckData(PBYTE pMEFileBuf, DWORD FileSize);

BOOLEAN     ME_Full_FlashStart();
BOOLEAN     ME_Full_FlashEnd();

ENUM_ERR_ID     ME_GetLastError();
ENUM_ERR_ID     ME_SWSMI(ENUM_ME_FLASH_ID MFD_Status, POFBD_MEUD* pOFBD_MEInfo);

BOOLEAN     MER_Load();
BOOLEAN     MER_Init();
BOOLEAN     MER_Release();
ENUM_ERR_ID     MER_INTERFACE(PBYTE pMEBuffer, DWORD ddBufferSize, BYTE bBlockType, BYTE bHandleRequest, DWORD ddTotalBlks, DWORD ddBlksIndex, BOOLEAN * IsNeedToCallAgain);
BOOLEAN     MER_Update(BYTE CmdID, UINT32 BlockSize, UINTN Frequency, CallBackFn_T CallBackFunc);
DWORD       MER_GetTotalCmdNum();
PCHAR       MER_GetCmdString(DWORD num);
BYTE        MER_GetCmdID(DWORD num);
BOOLEAN     MER_PushData(PBYTE pMEFileBuf, CallBackFn_T Callback);

/**
    MERF_Check()
    
    This function checks if ME is supported.
    
    @param VOID
    
    @retval EFI_STATUS
*/
EFI_STATUS MERF_Check()
{
    EFI_STATUS Status = EFI_SUCCESS;
    ENUM_ERR_ID ErrorID = eEID_NO_ERROR;
    BOOL bSupport = FALSE;

    bSupport = ME_CheckSupport();
    if(!bSupport)
    {
        ErrorID = ME_GetLastError();
        return ERROR_ME_UNSUPPORT;
    }

    if(!MER_Load())
        return ERROR_ME_UNSUPPORT; 

    ME_VersionCheck();  // EIP 354005/377144 +
    
    return EFI_SUCCESS;
}

/**
    MERF_Flash()
    
    This function flashes ME.
    
    @param Frequency - The executed frequency of Call back function.
    @param CallBackFunc - The call back function.
    
    @retval EFI_STATUS
*/
EFI_STATUS MERF_Flash(UINTN Frequency, CallBackFn_T CallBackFunc)
{   
    EFI_STATUS Status = EFI_SUCCESS;
    ENUM_ERR_ID ErrorID = eEID_NO_ERROR;
    BOOLEAN     bMEState = FALSE;
    DWORD       i = 0;

    ME_BLOCK_T* pMEBlkInfo = NULL;

    
    if(gpMEBuff==NULL)
        return ERROR_BUFFER_ALLOC;

    if(!ME_CheckData(gpMEBuff, gMESize))
        return ERROR_ME_UNSUPPORT;

    CmdTotalCnt = MER_GetTotalCmdNum();

    bMEState = ME_Full_FlashStart();

    if(!MER_Init())
    {
        ErrorID = ME_GetLastError();
        return ERROR_ME_UNSUPPORT;
    }

    if (!bMEState)
    {
        ErrorID = ME_GetLastError();        
        return ERROR_ME_UNSUPPORT;
    }

    //MER_PushData(AFU_pMEBuff, MERF_CallBack_ME_Update);
    if(!MER_PushData(gpMEBuff, NULL))
    {
        ErrorID = ME_GetLastError();        
        return ERROR_ME_UNSUPPORT;
    }

    if (g_MER_BlkDataInfo)
        pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
    /*
    Print(L"gMESize: %x",gMESize);
    gPrgressSize = gMESize;
    for (i = 0 ; i < CmdTotalCnt ; i++)
    {
        Print(L"%d %x",i,pMEBlkInfo[i].BlockSize);
        if (ME_OPR_BLK == MER_GetCmdID(i))
        {
           
            Print(L"ME_OPR_BLK %d %x",i,pMEBlkInfo[i].BlockSize);
            if (g_MER_BlkDataInfo)
            {
                //pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
                //gPrgressSize = gMESize - pMEBlkInfo[i].BlockSize;
                gPrgressSize = gPrgressSize - pMEBlkInfo[i].BlockSize;
                //break;
            }
        }
        
        if (BIOS_BLK == MER_GetCmdID(i))
        {
            Print(L"BIOS_BLK %d %x",i,pMEBlkInfo[i].BlockSize);
        }
    }
    */
    gProgressOffset = 0;
    for (i = 0 ; i < CmdTotalCnt ; i++)
    {
        //if ((TRUE == CmdEnable[i]) && (BIOS_BLK != MER_GetCmdID(i)))
//      if (BIOS_BLK == MER_GetCmdID(i))
//          continue;
//      if (ME_OPR_BLK == MER_GetCmdID(i))
//          continue;
        if (BIOS_BLK != MER_GetCmdID(i) && ME_OPR_BLK != MER_GetCmdID(i))
        {
            if (FALSE == MER_Update(MER_GetCmdID(i), pMEBlkInfo[i].BlockSize, Frequency, CallBackFunc))
            {
                MemCpy(ErrorStr, MER_GetCmdString(i), sizeof(CHAR)*4);
                return ERROR_FLASH_ME;
            }
        }
    }

    MER_Release();
 
    if (!ME_Full_FlashEnd())
    {
        ErrorID = ME_GetLastError();
        return ERROR_FLASH_ME;
    }

    if (ErrorID == eEID_NO_ERROR)   
        return EFI_SUCCESS;
    
    return ERROR_FLASH_ME;
}

/**
    ME_CalculateSum16()
    
    This function calculates checksum.
    
    @param Buffer - The data buffer.
    @param Size - The data size.
    
    @retval UINT16 - The result.
*/
UINT16 ME_CalculateSum16 (UINT16* Buffer, UINT32 Size)
{
    UINT32      Index;
    UINT16      Sum;
    
    Sum = 0;
    for (Index = 0; Index < Size; Index++) 
    {
        Sum = (UINT16) (Sum + Buffer[Index]);
    }
    
    return (UINT16) Sum;
}

EFI_STATUS ME_CheckSysRomSupport()
{
    EFI_STATUS Status = EFI_SUCCESS;
    BOOL bSupport = FALSE;
    
    bSupport = ME_Ignition_CheckSupport();

    if (!bSupport)
        bSupport = ME_CheckSupport();
    
    
    return EFI_UNSUPPORTED;
}

/**
    ME_GetFlashCapacity()
    
    This function calculates the flash part size.
    
    @param VOID
    
    @retval UINT32 - Flash part size
*/
UINT32 ME_GetFlashCapacity()
{
    ENUM_ERR_ID Errid = eEID_NO_ERROR;
    
    
    Errid = ME_SWSMI(eMFD_ME_INIT, &g_ME_pOFBD_MEInfo);
    
    if (eEID_NO_ERROR == Errid)
    {
        /*
        DEBUG ((0xFFFFFFFF, "- Dump ME Info :\n"));
        DEBUG ((0xFFFFFFFF, "  ReturnStatus     : 0x%X\n", g_ME_pOFBD_MEInfo->bReturnStatus));
        DEBUG ((0xFFFFFFFF, "  SubFunction      : 0x%X\n", g_ME_pOFBD_MEInfo->bSubFunction));
        DEBUG ((0xFFFFFFFF, "  RuntimeBase      : 0x%08X\n", g_ME_pOFBD_MEInfo->dMERuntimeBase));
        DEBUG ((0xFFFFFFFF, "  RuntimeLength    : 0x%08X\n", g_ME_pOFBD_MEInfo->dMERuntimeLength));
        DEBUG ((0xFFFFFFFF, "  BiosRegionBase   : 0x%08X\n", g_ME_pOFBD_MEInfo->dMEBiosRegionBase));
        DEBUG ((0xFFFFFFFF, "  BiosRegionLength : 0x%08X\n", g_ME_pOFBD_MEInfo->dMEBiosRegionLength));
        */
        return g_ME_pOFBD_MEInfo->dMERuntimeLength + g_ME_pOFBD_MEInfo->dMEBiosRegionLength;
    }
    
    return 0;
}

// EIP 354005/377144 +>>>
/**
    ME_VersionCheck()
    
    This function checks ME Version.
    
    @param VOID
    
    @retval BOOLEAN
*/
BOOLEAN ME_VersionCheck()
{
    PBYTE               pOFBD_Buffer = 0;
    POFBD_STRUCT_V2     pOFBDV2 = (POFBD_STRUCT_V2)NULL;
    POFBD_TYPECODE_V2   pOFBDTCV2 = (POFBD_TYPECODE_V2)NULL;
    POFBD_ME_VERSION    pOFBD_ME_VERSION = (POFBD_ME_VERSION)NULL;
    POFBD_EXTHDR_END    pOFBD_ExtEnd = (POFBD_EXTHDR_END)NULL;
    DWORD               dwTCOffset = 0;
    
    g_ME_bCoffeeLakeME = FALSE;
        
    pOFBD_Buffer = AllocMem(FLASH_PAGE_SIZE*2);
    
    if (pOFBD_Buffer == NULL)
    {
        gErrorID = eEID_MEM_ALLOC;
        return FALSE;
    }

    //memset(pOFBD_Buffer, 0, FLASH_PAGE_SIZE*2);
    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE*2, 0);
    
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_MEUD);

    pOFBDV2 = (POFBD_STRUCT_V2)pOFBD_Buffer;
    pOFBDTCV2 = (POFBD_TYPECODE_V2)(pOFBD_Buffer+pOFBDV2->dwSize);

    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ME_VERSION);

    pOFBDV2->dwSize += sizeof (OFBD_TYPECODE_V2);           // update size
    pOFBDTCV2->dbCode = OFBD_EXT_TC_MEUD;
    pOFBDTCV2->dbNumHole = 1;
    
    dwTCOffset = ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize;
    
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)pOFBD_Buffer;
    pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ME_VERSION));         // update size  
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)((UINT64)pOFBD_ExtEnd + (UINT64) pOFBDV2->dwSize);
    pOFBD_ExtEnd->ddHdrEnd = OFBD_EXT_TBL_END;              // fill end value
    pOFBDV2->dwSize += sizeof(OFBD_EXT_TBL_END);            // update size
    
    pOFBD_ME_VERSION = (POFBD_ME_VERSION)((char *)pOFBD_Buffer + (UINT64) dwTCOffset);
    /* Fill the default value */
    pOFBD_ME_VERSION->bReturnStatus = 0xff;
    pOFBD_ME_VERSION->bSubFunction = (BYTE)eMFD_ME_VERSION;//0xB
    pOFBD_ME_VERSION->AFUVersion = 5;
    pOFBD_ME_VERSION->AFUSubVersion = 9;
    pOFBD_ME_VERSION->AFUBuildVersion = 3;
    
    //memset(gpIntrBuf,0,(int)g_MER_ddFlash_Page_Size);
    MemSet(gpIntrBuf,(int)g_MER_ddFlash_Page_Size, 0);
    //memcpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE); 
    MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE);
    
    /* Call SMI 0x26 ME update interface to write */
    Ofbd_SMI_26();

    if (!(((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus & OFBD_RS_SUPPORT))
    {
        // Not support new ME Blocks Info report
        gErrorID = eEID_ME_INFO_NO_SUPPORT;
        return FALSE;
    }

    pOFBD_ME_VERSION = (POFBD_ME_VERSION)((char *)gpIntrBuf + (UINT64)dwTCOffset);

    // Check the Return status
    if (pOFBD_ME_VERSION->bReturnStatus != 1) 
    {
        // Not support new ME Version;      
        return FALSE;
    }
    
    g_ME_bCoffeeLakeME = TRUE;

	return TRUE;
}
// EIP 354005/377144 <<<+

/**
    ME_CheckSupport()
    
    This function checks if ME is supported.
    
    @param VOID
    
    @retval BOOLEAN
*/
BOOLEAN ME_CheckSupport()
{
    ENUM_ERR_ID Errid = eEID_NO_ERROR;
    
    
    if (!g_ME_bIgMEIsSupport) 
    {
        g_ME_pOFBD_MEInfo = (POFBD_MEUD)NULL;
        g_ME_pMEBuffer = (PBYTE)NULL;
        g_ME_dMEBufferSize = 0;
        g_ME_dMEFileSize = 0;
        g_ME_bIsFoundME = FALSE;
        g_ME_bMEIsSupport = FALSE;    
    }
    
    Errid = ME_SWSMI(eMFD_ME_INIT, &g_ME_pOFBD_MEInfo);
    
    if (eEID_NO_ERROR == Errid)
    {
        g_ME_bMEIsSupport = TRUE;
        return TRUE;
    }
    
    gErrorID = Errid;
    
    return FALSE;
}   

BOOLEAN ME_Ignition_CheckSupport()
{
    ENUM_ERR_ID Errid = eEID_NO_ERROR;
    
    
    if (!g_ME_bMEIsSupport)
    {
        g_ME_pOFBD_MEInfo = (POFBD_MEUD)NULL;
        g_ME_pMEBuffer = (PBYTE)NULL;
        g_ME_dMEBufferSize = 0;
        g_ME_dMEFileSize = 0;
        g_ME_bIsFoundME = FALSE;
        g_ME_bIgMEIsSupport = FALSE;
    }
    
    Errid = ME_SWSMI(eMFD_IGN_INIT, &g_ME_pOFBD_MEInfo);
    
    if (eEID_NO_ERROR == Errid)
    {
        g_ME_bIgMEIsSupport = TRUE;
        return TRUE;
    }
    
    gErrorID = Errid;
    
    return FALSE;
}

/**
    ME_CheckData()
    
    This function checks if ME data is valid.
    
    @param pMEFileBuf - The ME data buffer.
    @param FileSize - The buffer size.
    
    @retval BOOLEAN
*/
BOOLEAN ME_CheckData(PBYTE pMEFileBuf, DWORD FileSize)
{
    UINT8   i = 0;
    PBYTE   TempBuf = (PBYTE)NULL;
    
    if (FileSize < FLASH_PAGE_SIZE) return FALSE;

    if (!g_ME_bCheckMEDataFirst)
    {
        g_ME_pMEBuffer = pMEFileBuf;
        g_ME_dMEFileSize = FileSize;
        g_ME_bCheckMEDataFirst = TRUE;
    }
    
    TempBuf = AllocMem(FLASH_PAGE_SIZE);
    
    if (NULL == TempBuf)
    {
        gErrorID = eEID_MEM_ALLOC;
        return FALSE;
    }
    
    MemCpy(TempBuf, pMEFileBuf, FLASH_PAGE_SIZE);
    
    // Search for the '5AA5F00F' signature in 0x00 offset or 0x10 offset
    for (i = 0; i < 0x20 - 4; i++) 
    {
        if ((0x5A == TempBuf[i]) && (0xA5 == TempBuf[i+1]) &&
            (0xF0 == TempBuf[i+2]) && (0x0F == TempBuf[i+3])) 
        {
            FreeMem(TempBuf);
            g_ME_bIsFoundME = TRUE;
            return TRUE;            
        }
    }   
    
    FreeMem(TempBuf);   
    return FALSE;
}

/**
    ME_Full_FlashStart()
    
    This function starts ME flash.
    
    @param VOID.
    
    @retval BOOLEAN
*/
BOOLEAN ME_Full_FlashStart()
{
    ENUM_ERR_ID ErrId = eEID_NO_ERROR;
    
    ErrId = ME_SWSMI(eMFD_ME_START, (POFBD_MEUD*)NULL);
    
    if (eEID_NO_ERROR == ErrId)
        return TRUE;
    
    gErrorID = ErrId;
    return FALSE;
}

/**
    ME_Full_FlashEnd()
    
    This function ends ME flash.
    
    @param VOID.
    
    @retval BOOLEAN
*/
BOOLEAN ME_Full_FlashEnd()
{
    BOOLEAN re = FALSE;
    ENUM_ERR_ID ErrId = eEID_NO_ERROR;
    
    ErrId = ME_SWSMI(eMFD_ME_END, (POFBD_MEUD*)NULL);
    
    if (eEID_NO_ERROR == ErrId)
        re = TRUE;
    
    gErrorID = ErrId;
    FreeMem((PBYTE)g_ME_pOFBD_MEInfo);
    g_ME_dMEFileSize = 0;   
    return re;
}

/**
    ME_GetLastError()
    
    This function gets the last ME error.
    
    @param VOID
    
    @retval ENUM_ERR_ID
*/
ENUM_ERR_ID ME_GetLastError()
{
    return gErrorID;
}

/**
    ME_SWSMI()
    
    This function issues SMI and gets ME info.
    
    @param MFD_Status - ME SMI type.
    @param pOFBD_MEInfo - ME data buffer.
    
    @retval ENUM_ERR_ID
*/
ENUM_ERR_ID ME_SWSMI(ENUM_ME_FLASH_ID MFD_Status, POFBD_MEUD* pOFBD_MEInfo)
{
    PBYTE       pOFBD_Buffer = 0;
    POFBD_MEUD  pOFBD_MEUD = (POFBD_MEUD)NULL;
    DWORD       dwTCOffset = 0;

    Ofbd_GetVer();
    
    pOFBD_Buffer = AllocMem(FLASH_PAGE_SIZE);
    if (pOFBD_Buffer == NULL)
        return eEID_MEM_ALLOC;

    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE, 0);
    
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_MEUD);
    // Ofbd_Ext
    Ofbd_Ext_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD, 1);
    
    dwTCOffset = ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize;
    
    // Ofbd_TypeCode
    Ofbd_TC_End_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD, 1);
    
    pOFBD_MEUD = (POFBD_MEUD)((char *)pOFBD_Buffer + (unsigned long) dwTCOffset);
    
    pOFBD_MEUD->bSubFunction = (BYTE)MFD_Status;
        
    MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE);
    
    //printf("- [Send] DMIAccessIOByte start !!\n");
    //DumpNBytesMemory(gpIntrBuf, 0x30); printf("\n"); 
    //__OFBDMEDBG(WriteOut("OFBD_SWSMI_ME 26 in ME\n"));
    //OutPortByte(gSmiPort, OFBD_SWSMI_PORT);
    Ofbd_SMI_26();
    //__OFBDMEDBG(WriteOut("OFBD_SWSMI_ME 26 out ME\n"));
    //printf("- [Get] DMIAccessIOByte end !!\n");
    //DumpNBytesMemory(gpIntrBuf, 0x30); printf("\n");
                
    if (!(((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus & OFBD_RS_SUPPORT)) 
    {
        // 0~2 For ME Ignition Firmware Update
        // 3~5 For ME Entire Firmware Update
        if (MFD_Status < eMFD_ME_INIT)
        {
            FreeMem(pOFBD_Buffer);
            return eEID_ME_IGN_STOP;            
        }
        else
        {
        FreeMem(pOFBD_Buffer);
        return eEID_ME_STOP;            
        }
    }
        
    pOFBD_MEUD = (POFBD_MEUD)((char *)gpIntrBuf + (unsigned long)dwTCOffset);
  
    // Check the Return status
    if (pOFBD_MEUD->bReturnStatus != 1) 
    {
        // 0~2 For ME Ignition Firmware Update
        // 3~5 For ME Entire Firmware Update
        if (MFD_Status < eMFD_ME_INIT)
        {
            FreeMem(pOFBD_Buffer);
            return eEID_ME_IGN_STOP;        
        }
        else
        {
            FreeMem(pOFBD_Buffer);
            return eEID_ME_STOP;            
        }
    }
    
    if ((eMFD_IGN_INIT == MFD_Status) || (eMFD_ME_INIT == MFD_Status))
    {
        
        //__OFBDMEDBG(WriteOut("- Dump ME Info :\n");
        //WriteOut("  ReturnStatus     : 0x%X\n", pOFBD_MEUD->bReturnStatus);
        //WriteOut("  SubFunction      : 0x%X\n", pOFBD_MEUD->bSubFunction);
        //WriteOut("  RuntimeBase      : 0x%08X\n", pOFBD_MEUD->dMERuntimeBase);
        //WriteOut("  RuntimeLength    : 0x%08X\n", pOFBD_MEUD->dMERuntimeLength);
        //WriteOut("  BiosRegionBase   : 0x%08X\n", pOFBD_MEUD->dMEBiosRegionBase);
        //WriteOut("  BiosRegionLength : 0x%08X\n", pOFBD_MEUD->dMEBiosRegionLength);)
        
        if (!*pOFBD_MEInfo)
            *pOFBD_MEInfo = (POFBD_MEUD)AllocMem(sizeof(OFBD_MEUD));
        
        if (!pOFBD_MEInfo)
        {
            FreeMem(pOFBD_Buffer);
            return eEID_MEM_ALLOC;
        }
    
        MemCpy(*pOFBD_MEInfo, pOFBD_MEUD, sizeof(OFBD_MEUD));
    }
    
    FreeMem(pOFBD_Buffer);
    return eEID_NO_ERROR;
}

/**
    ME_SWSMI()
    
    This function loads ME data.
    
    @param VOID
    
    @retval ENUM_ERR_ID
*/
BOOLEAN MER_Load()
{
    PBYTE               pOFBD_Buffer = 0;
    POFBD_STRUCT_V2     pOFBDV2 = (POFBD_STRUCT_V2)NULL;
    POFBD_TYPECODE_V2   pOFBDTCV2 = (POFBD_TYPECODE_V2)NULL;
    POFBD_ME_INFO       pOFBD_ME_INFO = (POFBD_ME_INFO)NULL;
    POFBD_EXTHDR_END    pOFBD_ExtEnd = (POFBD_EXTHDR_END)NULL;
    DWORD               dwTCOffset = 0;
    DWORD               MEInfoSize = 0;
    WORD                NumofBlks = 0;
    ME_BLOCK_T*         pMEBlkInfo = (ME_BLOCK_T*)NULL;
    int                 i = 0;
    
    ME_BLOCK_T*         pgMER_BlockInfo = (ME_BLOCK_T*)NULL;
    ME_BLOCK_T*         pOFBD_ME_BlockInfo = (ME_BLOCK_T*)NULL;
    
    g_MER_BlkDataInfo = (PMEBlkInfo)NULL;

    pOFBD_Buffer = AllocMem(FLASH_PAGE_SIZE*2);
    
    if (pOFBD_Buffer == NULL)
    {
        gErrorID = eEID_MEM_ALLOC;
        return FALSE;
    }

    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE*2, 0);
    
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_MEUD);
    // Ofbd_Ext
    //Ofbd_Ext_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD); 

    pOFBDV2 = (POFBD_STRUCT_V2)pOFBD_Buffer;
    pOFBDTCV2 = (POFBD_TYPECODE_V2)(pOFBD_Buffer+pOFBDV2->dwSize);

    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ME_INFO);

    pOFBDV2->dwSize += sizeof (OFBD_TYPECODE_V2);           // update size
    pOFBDTCV2->dbCode = OFBD_EXT_TC_MEUD;
    pOFBDTCV2->dbNumHole = 1;
    
    dwTCOffset = ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize;
    
    // Ofbd_TypeCode
    //Ofbd_TC_End_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD); 

    pOFBD_ExtEnd = (POFBD_EXTHDR_END)pOFBD_Buffer;
    pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ME_INFO));        // update size  
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)((unsigned long)pOFBD_ExtEnd + (unsigned long) pOFBDV2->dwSize);
    pOFBD_ExtEnd->ddHdrEnd = OFBD_EXT_TBL_END;              // fill end value
    pOFBDV2->dwSize += sizeof(OFBD_EXT_TBL_END);            // update size
    
    pOFBD_ME_INFO = (POFBD_ME_INFO)((char *)pOFBD_Buffer + (unsigned long) dwTCOffset);
    /* Fill the default value */
    pOFBD_ME_INFO->bReturnStatus = 0;
    pOFBD_ME_INFO->bSubFunction = (BYTE)eMFD_ME_INFO;
    pOFBD_ME_INFO->dwTotalBlocks = 0;
    
    MemSet(gpIntrBuf,(int)g_MER_ddFlash_Page_Size,0);
    
    MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE);        
    
    //__OFBDMEDBG(WriteOut("OFBD_MER_Load 0x26 In\n");)
    /* Call SMI 0x26 ME update interface to write */
    //OutPortByte(gSmiPort, OFBD_SWSMI_PORT);
    //__OFBDMEDBG(WriteOut("OFBD_ME Run Case 9\n"));
    Ofbd_SMI_26();
    //__OFBDMEDBG(WriteOut("OFBD_MER_Load 0x26 Out\n");)

    if (!(((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus & OFBD_RS_SUPPORT))
    {
        // Not support new ME Blocks Info report
        // __OFBDMEDBG(WriteOut("!(((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus & OFBD_RS_SUPPORT)....1\n");)
        // __OFBDMEDBG(WriteOut("ME_INFO_NO_SUPPORT....1\n");)
        gErrorID = eEID_ME_INFO_NO_SUPPORT;
        FreeMem(pOFBD_Buffer);
        return FALSE;
    }

    pOFBD_ME_INFO = (POFBD_ME_INFO)((char *)gpIntrBuf + (unsigned long)dwTCOffset);

    //__OFBDMEDBG(pOFBD_ME_INFO = &TestMEINFO;)
    // Check the Return status
    if (pOFBD_ME_INFO->bReturnStatus == 0) 
    {
        // __OFBDMEDBG(WriteOut("pOFBD_ME_INFO->bReturnStatus == 0\n");)
        // __OFBDMEDBG(WriteOut("ME_INFO_NO_SUPPORT....2\n");)      
        // Not support new ME Flash Status
        gErrorID = eEID_ME_INFO_NO_SUPPORT;
        FreeMem(pOFBD_Buffer);
        return FALSE;
    }
    
    NumofBlks = pOFBD_ME_INFO->dwTotalBlocks;                               //total number of blocks
    MEInfoSize = sizeof(MEBlkInfo) + (NumofBlks * sizeof(ME_BLOCK_T));      //total size of info struct
    g_MER_BlkDataInfo = (MEBlkInfo*) AllocMem(MEInfoSize);
    
    if (NULL == g_MER_BlkDataInfo)
    {
        gErrorID = eEID_MEM_ALLOC;
        FreeMem(pOFBD_Buffer);
        return FALSE;
    }
    
    MemSet(g_MER_BlkDataInfo, MEInfoSize, 0x00);
    
    //ME_BLOCK_T* pgMER_BlockInfo = &g_MER_BlkDataInfo->BlockInfo;
    //ME_BLOCK_T* pOFBD_ME_BlockInfo = pOFBD_ME_INFO->tBlockInfo;
    pgMER_BlockInfo = &g_MER_BlkDataInfo->BlockInfo;
    pOFBD_ME_BlockInfo = pOFBD_ME_INFO->tBlockInfo;
    
    MemCpy(pgMER_BlockInfo, pOFBD_ME_BlockInfo, (NumofBlks * sizeof(ME_BLOCK_T)));
    
    g_MER_BlkDataInfo->TotalBlocks = NumofBlks;
    g_MER_BlkDataInfo->METotalSize = 0;
    
    pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;

    //    __OFBDMEDBG(
    //    WriteOut("- [DBG] ME Total Block Count : 0x%X\n", NumofBlks);
    //    WriteOut("- [DBG] ME Block Info : \n");
    //    )

    for(i=0; i<NumofBlks; i++) 
    {        
        //        __OFBDMEDBG(
        //        WriteOut("        [%d] - Addr : 0x%lX, Size : 0x%lX, Type : 0x%X, Lock : 0x%X\n", i, 
        //                    pMEBlkInfo[i].StartAddress, pMEBlkInfo[i].BlockSize, pMEBlkInfo[i].Type, pMEBlkInfo[i].Lock);
        //        WriteOut("             - Cmd : \"%s\", Descriotion : \"%s\"\n", pMEBlkInfo[i].Command, pMEBlkInfo[i].Description);
        //        )
        if (pMEBlkInfo[i].Type != BIOS_BLK)
            g_MER_BlkDataInfo->METotalSize += pMEBlkInfo[i].BlockSize;
    }

    FreeMem(pOFBD_Buffer);
    return TRUE;
}

// EIP 377144 +>>>
BOOLEAN MER_GetBlkInfo(BYTE Type, DWORD* pStartAddress, DWORD* pBlockSize)
{
    ME_BLOCK_T* pMEBlkInfo = (ME_BLOCK_T*)NULL;
    int         i = 0;
   
    if (g_MER_BlkDataInfo)
    {
        pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
        for(i=0; i<g_MER_BlkDataInfo->TotalBlocks; i++)
        {
            ///debug+
            //Print(L"***MER_GetBlkInfo()\n");
            //Print(L"   Type         = 0x%08X\n", pMEBlkInfo[i].Type);
            //Print(L"   StartAddress = 0x%08X\n", pMEBlkInfo[i].StartAddress);
            //Print(L"   BlockSize    = 0x%08X\n", pMEBlkInfo[i].BlockSize);
            if (pMEBlkInfo[i].Type == Type)
            {
                *pStartAddress = pMEBlkInfo[i].StartAddress;
                *pBlockSize    = pMEBlkInfo[i].BlockSize;
            }
        }
        return TRUE;
    }
    
    return FALSE;
}
// EIP 377144 +<<<

/**
    MER_INTERFACE()
    
    This function loads ME data.
    
    @param pMEBuffer - The ROM file buffer.
    @param ddBufferSize - The buffer size.
    @param bBlockType - The ME block type.
    @param bHandleRequest - 
    @param ddTotalBlks - Total blocks of Update data.
    @param ddBlksIndex - Block index of TotalBlocks.
    @param IsNeedToCallAgain - 
    
    @retval ENUM_ERR_ID
*/
ENUM_ERR_ID MER_INTERFACE(PBYTE pMEBuffer, DWORD ddBufferSize, BYTE bBlockType, BYTE bHandleRequest, DWORD ddTotalBlks, DWORD ddBlksIndex, BOOLEAN * IsNeedToCallAgain)
{
    PBYTE               pOFBD_Buffer = 0;
    POFBD_STRUCT_V2     pOFBDV2 = (POFBD_STRUCT_V2)NULL;
    POFBD_TYPECODE_V2   pOFBDTCV2 = (POFBD_TYPECODE_V2)NULL;
    POFBD_ME_UPDATE     pOFBD_ME_UPDATE = (POFBD_ME_UPDATE)NULL;
    POFBD_EXTHDR_END    pOFBD_ExtEnd = (POFBD_EXTHDR_END)NULL;
    DWORD               dwTCOffset = 0;
    DWORD               MEInfoSize = 0;
    DWORD               NumofBlks = 0;
    WORD                UpdateResult = 0;
    WORD                m_dwOFBD_RS = 0x00;
    DWORD               m_dwSize = 0;
    
    *IsNeedToCallAgain = FALSE;
    
    pOFBD_Buffer = AllocMem(FLASH_PAGE_SIZE);
    
    if (pOFBD_Buffer == (PBYTE)NULL)
        return eEID_MEM_ALLOC;

    MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE, 0);
    
    // Ofbd_Desc
    Ofbd_Desc_Header(pOFBD_Buffer, OFBD_FS_MEUD);
    // Ofbd_Ext
    //Ofbd_Ext_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD); 

    pOFBDV2 = (POFBD_STRUCT_V2)pOFBD_Buffer;
    pOFBDTCV2 = (POFBD_TYPECODE_V2)(pOFBD_Buffer+pOFBDV2->dwSize);

    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ME_UPDATE);
    pOFBDV2->dwSize += sizeof (OFBD_TYPECODE_V2);           // update size
    pOFBDTCV2->dbCode = OFBD_EXT_TC_MEUD;
    pOFBDTCV2->dbNumHole = 1;
    
    dwTCOffset = ((POFBD_STRUCT_V2)pOFBD_Buffer)->dwSize;
    
    // Ofbd_TypeCode
    //Ofbd_TC_End_Header(pOFBD_Buffer, OFBD_EXT_TC_MEUD); 

    pOFBD_ExtEnd = (POFBD_EXTHDR_END)pOFBD_Buffer;
    pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ME_UPDATE));      // update size  
    pOFBD_ExtEnd = (POFBD_EXTHDR_END)((unsigned long)pOFBD_ExtEnd + (unsigned long) pOFBDV2->dwSize);
    pOFBD_ExtEnd->ddHdrEnd = OFBD_EXT_TBL_END;              // fill end value
    pOFBDV2->dwSize += sizeof(OFBD_EXT_TBL_END);            // update size
    
    pOFBD_ME_UPDATE = (POFBD_ME_UPDATE)((char *)pOFBD_Buffer + (unsigned long) dwTCOffset);
    
    /* Fill the default value */
    pOFBD_ME_UPDATE->bReturnStatus    = 0;
    pOFBD_ME_UPDATE->bSubFunction     = (BYTE)eMFD_ME_UPDATE;
    pOFBD_ME_UPDATE->TotalBlocks      = ddTotalBlks;
    pOFBD_ME_UPDATE->BlockIndex       = ddBlksIndex;
    pOFBD_ME_UPDATE->bHandleRequest   = bHandleRequest;
    pOFBD_ME_UPDATE->bBlockType       = bBlockType;
    pOFBD_ME_UPDATE->UpdateResult     = 0;
    pOFBD_ME_UPDATE->ddMEDataBuffer   = (DWORD)(gIntrBufPhyAddr + FLASH_PAGE_SIZE);
    pOFBD_ME_UPDATE->ddMEDataSize     = ddBufferSize;
    pOFBD_ME_UPDATE->ddMEDataChecksum = 0;
    pOFBD_ME_UPDATE->ddMessageBuffer  = (DWORD)(gIntrBufPhyAddr + ddBufferSize + FLASH_PAGE_SIZE);
    
//  __OFBDMEDBG(WriteOut("bHandleRequest = 0x%x \n", bHandleRequest);)
        
    switch(bHandleRequest)
    {
        case ME_REQUEST_ALLOC_SMM:      // Notify the BIOS to allocate SMM memory
        break;
        case ME_REQUEST_TRANSFER_DATA:  // Transfer ME data into the BIOS
        pOFBD_ME_UPDATE->ddMEDataChecksum = CalculateSum16 ((UINT16 *)pMEBuffer, ddBufferSize/sizeof(UINT16));
        break;
        case ME_REQUEST_UPD_START:      // Notify the BIOS start to Update ME region
        break;
        case ME_REQUEST_CALL_AGAIN:     // Respond to BIOS requested to call again
        break;
        case ME_REQUEST_FREE_SMM:       // Notify the BIOS to free the SMM memory
        break;
        case ME_REQUEST_MEUL_UPDATE:    // BIOS start to Update ME Local.
        break;
        default:
        break;
    }
    
    MemSet(gpIntrBuf, (int)g_MER_ddFlash_Page_Size, 0);
    
    MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE); 
    
    // Copy ME data
    if (bHandleRequest == ME_REQUEST_TRANSFER_DATA) {
        MemCpy(gpIntrBuf + FLASH_PAGE_SIZE, pMEBuffer, ddBufferSize); 
    }
    
    //__OFBDMEDBG(WriteOut("Befor Call SMI 0x26 ME update interface  \n");)
    // Call SMI 0x26 ME update interface
    //OutPortByte(gSmiPort, OFBD_SWSMI_PORT);
    //    __OFBDMEDBG(WriteOut("OFBD_ME Run Case 10\n"));

    Ofbd_SMI_26();
    //__OFBDMEDBG(WriteOut("After Call SMI 0x26 ME update interface  \n");)
    
    m_dwOFBD_RS = ((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus;
    if (!( m_dwOFBD_RS & OFBD_RS_SUPPORT))
    {
        // Not support new ME Blocks Info report
        FreeMem(pOFBD_Buffer);
        return eEID_ME_INFO_NO_SUPPORT;
    }   
    if ((m_dwOFBD_RS & OFBD_RS_DIS_OEMSTR)) 
    {
        m_dwSize = ((POFBD_STRUCT_V2)gpIntrBuf)->dwSize;
        
        /*if (Strlen((char *)gpIntrBuf + m_dwSize) > 0)
        {
            //WriteOut(" - %s\n", (char *)gpIntrBuf + m_dwSize);
        }*/
    }
            
    pOFBD_ME_UPDATE = (POFBD_ME_UPDATE)((char *)gpIntrBuf + (unsigned long)dwTCOffset);
  
    // Check the Return status
    if (pOFBD_ME_UPDATE->bReturnStatus == 0) {
    // Not support new ME Flash Status
        FreeMem(pOFBD_Buffer);
        return eEID_ME_INFO_NO_SUPPORT;
    }
    
    UpdateResult = pOFBD_ME_UPDATE->UpdateResult;
    /*
    // Bit 0: Update Error
    // Bit 1: AFU need to call again (If BIOS needs AFU to try again, it will set this Bit is true)
    // Note: AFU will set the Bit 3 of bHanleRequest is true, when BIOS requested to call again
    // Bit 2: Tell the AFU to display the process message (If BIOS need to display any message for user to know)
    // Bit 3~15: Reserved
    */
    // Bit 1: AFU need to call again (If BIOS needs AFU to try again, it will set this Bit is true)
    if (UpdateResult & 0x02)
        *IsNeedToCallAgain = TRUE;
    
    // Bit 2: Tell the AFU to display the process message (If BIOS need to display any message for user to know)    
    if (UpdateResult & 0x04)
    {
    //      WriteOut(" - %s\n", (char *)gpIntrBuf + ddBufferSize + FLASH_PAGE_SIZE);
    }

    // Bit 0: Update Error
    if (UpdateResult & 0x01)
    {
        FreeMem(pOFBD_Buffer);
        return eEID_ME_PROCESS_ERROR;
    }
    
    FreeMem(pOFBD_Buffer);
    return eEID_NO_ERROR;
}

/**
    MER_Init()
    
    This function initiates ME.
    
    @param VOID
    
    @retval BOOLEAN
*/
BOOLEAN MER_Init()
{
    if(g_MER_BlkDataInfo)
    {
        BOOLEAN IsNeedToCallAgain = FALSE;
        ENUM_ERR_ID  ErrorID = eEID_NO_ERROR;
        
        DWORD dwMETotalSzie = 0;
    
        if (g_MER_BlkDataInfo->METotalSize >= g_ME_pOFBD_MEInfo->dMERuntimeLength)
            dwMETotalSzie = g_MER_BlkDataInfo->METotalSize;
        else if (g_MER_BlkDataInfo->METotalSize < g_ME_pOFBD_MEInfo->dMERuntimeLength)
            dwMETotalSzie = g_ME_pOFBD_MEInfo->dMERuntimeLength;

        // EIP 354005/377144 +>>>
        if (g_ME_bCoffeeLakeME)
        {
            if (g_MER_BlkDataInfo)
            {
                ME_BLOCK_T* pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
                int i = 0;
                dwMETotalSzie = 0;
                            
                for (i = 0 ; i < g_MER_BlkDataInfo->TotalBlocks ; i++)
                {
                    if (pMEBlkInfo[i].Type != BIOS_BLK)
                    {
                        dwMETotalSzie += pMEBlkInfo[i].BlockSize;
                    }
                }
            }
        }
        // EIP 354005/377144 <<<+
       
        ErrorID = MER_INTERFACE(0, 0, 255, ME_REQUEST_ALLOC_SMM, dwMETotalSzie/(FLASH_PAGE_SIZE*4), 0, &IsNeedToCallAgain);
    
        if (ErrorID == eEID_NO_ERROR)
            return TRUE;
    }
    
    return FALSE;
}

/**
    MER_Release()
    
    This function frees ME lock of SMM.
    
    @param VOID
    
    @retval BOOLEAN
*/
BOOLEAN MER_Release()
{
    if (g_MER_BlkDataInfo)
    {
        BOOLEAN IsNeedToCallAgain = FALSE;
        ENUM_ERR_ID  ErrorID = eEID_NO_ERROR;
    
        //__OFBDMEDBG(WriteOut("OFBD_MER_Release : ME_REQUEST_FREE_SMM\n");)
    
        ErrorID = MER_INTERFACE(0, 0, 255, ME_REQUEST_FREE_SMM, 0, 0, &IsNeedToCallAgain);
    
        if (ErrorID == eEID_NO_ERROR)
            return TRUE;
    
    }
    
    return FALSE;
}

/**
    MER_Update()
    
    This function updates ME.
    
    @param CmdID - The ME block type.
    @param Frequency - The executed frequency of call back function.
    @param CallBackFunc - The call back function.
    
    @retval BOOLEAN
*/
BOOLEAN MER_Update(BYTE CmdID, UINT32 BlockSize, UINTN Frequency, CallBackFn_T CallBackFunc)
{    
    if (g_MER_BlkDataInfo)
    {
        BOOLEAN IsNeedToCallAgain = FALSE;
        BYTE bMERequest = ME_REQUEST_UPD_START;
        ENUM_ERR_ID ret;
        
        UINT32 Offset = 0;
        UINT64 Timer = 0;
        UINTN Second = 0;
        
        UINT32 PregProgressOffset = 0;
        
        if(NULL != CallBackFunc)
        {
            Timer = 0;
            if(Frequency!= 0)
                Second = Frequency;
            else
                Second = (UINTN)CALLBACK_TIMER_Default;
        }
        
        
        PregProgressOffset = gProgressOffset;
        //gProgressOffset += BlockSize;
        //Print(L"\nBlockSize: %X gProgressOffset:%X gPrgressSize:%X\n",BlockSize, gProgressOffset, gPrgressSize);
        do 
        {
            if (IsNeedToCallAgain)
                bMERequest = ME_REQUEST_CALL_AGAIN;    
            else
                bMERequest = ME_REQUEST_UPD_START;    
        
            //__OFBDMEDBG(WriteOut("OFBD_MER_Update : CmdID = 0x%02X, bMERequest = 0x%04X\n", CmdID, bMERequest);)
            //TRACE((-1,"MER_Update CmdID = 0x%02X, bMERequest = 0x%04X\n", CmdID, bMERequest));            
            ret = MER_INTERFACE(0, 0, CmdID, bMERequest, 0, 0, &IsNeedToCallAgain);
        
            //OFBD_MER_FlashProgress(IsNeedToCallAgain, CmdID);

            if ((gProgressOffset - PregProgressOffset) < BlockSize)
            {
                gProgressOffset += FLASH_BLOCK_SIZE*2;//for display percentage
                if (gProgressOffset > (PregProgressOffset + BlockSize))
                    gProgressOffset = PregProgressOffset + BlockSize;
            }
            

            if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
            {
                Timer = GetCpuTimer();
                CallBackFunc((UINT64)(0 + gProgressOffset),(0 + gMERealFlashSize),(UINT64)gMERealFlashSize, FLASH_FULL_ME | WRITE_BLOCK);
                //CallBackFunc((UINT64)(0 + gProgressOffset),(0 + gMESize),(UINT64)gMESize, FLASH_FULL_ME | WRITE_BLOCK);
            }
            
        }while (IsNeedToCallAgain);

 
        if (eEID_NO_ERROR != ret)   
            return FALSE;

        gProgressOffset = PregProgressOffset + BlockSize;
                
        if(NULL != CallBackFunc && TRUE == CallBackExecuteFrequency(Second,Timer))
        {
            Timer = GetCpuTimer();
            CallBackFunc((UINT64)(0 + gProgressOffset),(0 + gMERealFlashSize),(UINT64)gMERealFlashSize, FLASH_FULL_ME | WRITE_BLOCK);
            //CallBackFunc((UINT64)(0 + gProgressOffset),(0 + gMESize),(UINT64)gMESize, FLASH_FULL_ME | WRITE_BLOCK);
        }
        
        return TRUE;
    }

    return FALSE;    
}

DWORD MER_GetTotalCmdNum()
{
    if (g_MER_BlkDataInfo)
        return g_MER_BlkDataInfo->TotalBlocks;
    else
        return 0;
}

PCHAR MER_GetCmdString(DWORD num)
{
    if (g_MER_BlkDataInfo)
    {
        ME_BLOCK_T* pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
    
        return (PCHAR)(pMEBlkInfo[num].Command);
    }
    
    return 0;
}

BYTE MER_GetCmdID(DWORD num)
{
    if (g_MER_BlkDataInfo)
    {
        ME_BLOCK_T* pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
    
        return pMEBlkInfo[num].Type;
    }
    
    return 0;
}

/**
    MER_PushData()
    
    This function pushes ME data.
    
    @param pMEFileBuf - The ME data buffer.
    @param Callback - The call back function.
    
    @retval BOOLEAN
*/
BOOLEAN MER_PushData(PBYTE pMEFileBuf, CallBackFn_T Callback)
{
    if (g_MER_BlkDataInfo)
    {
        DWORD count = 0;
        //DWORD ddBlkNum, ddBlkAddr, ddBlkSize;
        DWORD ddBlkNum, ddBlkSize;
        ME_BLOCK_T * pMEBlkInfo;
        BYTE bType = 0;
        BYTE bStatus = 0;
        
        DWORD TempMaxBlkSize = FILE_BUF_SIZE; // Each Block is 4K
        PBYTE TempRomFileBuf = (PBYTE)NULL;
        DWORD totalsize = 0;
        int i = 0;
        
        pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
        count = g_MER_BlkDataInfo->TotalBlocks;

#if 1
        //totalsize = g_MER_BlkDataInfo->METotalSize;
        totalsize = g_ME_dMEBufferSize;

        //        __OFBDMEDBG(WriteOut("1. totalsize = %X\n", totalsize);)

        if (totalsize <= 0)
        {
        	// EIP 354005/377144 +>>>
            //if (0x220 == g_OfbdVer)
           	if (g_ME_bCoffeeLakeME)
            {
				if (g_MER_BlkDataInfo)
				{
					ME_BLOCK_T* pMEBlkInfo = &g_MER_BlkDataInfo->BlockInfo;
					totalsize = 0;
									
					for (i = 0 ; i < g_MER_BlkDataInfo->TotalBlocks ; i++)
					{
						if (pMEBlkInfo[i].Type != BIOS_BLK)
						{
							//totalsize = pMEBlkInfo[i].BlockSize;
							totalsize += pMEBlkInfo[i].BlockSize;
						}
					}
				}
            }
           	// EIP 354005/377144 <<<+
            else
            {
            totalsize = g_ME_dMEBufferSize = g_ME_pOFBD_MEInfo->dMERuntimeLength;
            }
        }

        // __OFBDMEDBG(WriteOut("2. totalsize = %X\n", totalsize);)

        count = totalsize / FILE_BUF_SIZE;
        ddBlkSize = totalsize % FILE_BUF_SIZE;

        if (ddBlkSize) count++;
#endif
        // __OFBDMEDBG(WriteOut("count = %X\n", count);)        

        //DWORD TempMaxBlkSize = FILE_BUF_SIZE; // Each Block is 4K
        //PBYTE TempRomFileBuf = 0;
        TempRomFileBuf = AllocMem(TempMaxBlkSize);
        
        if (TempRomFileBuf == NULL)
        {
            gErrorID = eEID_MEM_ALLOC;
            return FALSE;
        }
#if 1
    //__OFBDMEDBG(        
    //        FILE *pFile1;                   
    //        pFile1 = fopen("medata.sav", "w+b");        
    //    )

        for(ddBlkNum = 0 ; ddBlkNum < count ; ddBlkNum++)
        {
            DWORD OffSet = 0;            
            DWORD dd4KTotalBlks = FILE_BUF_SIZE;
            DWORD dd4KBlksIndex = 0;
            BOOLEAN  IsNeedToCallAgain = FALSE;

            ENUM_ERR_ID ret = eEID_NO_ERROR;            

            //if (Callback)
            //  Callback (0xfffffffe, g_MER_BlkDataInfo->METotalSize, 0);

            OffSet = TempMaxBlkSize*ddBlkNum;

            // Copy block's data from ROM file to temporary buffer          
            MemCpy (TempRomFileBuf, pMEFileBuf + OffSet, TempMaxBlkSize);

            //    __OFBDMEDBG(
            //            if (OffSet == 0xb80000)
            //            {
            //                FILE *pFile2;
            //                char filename[256];
            //                sprintf(filename, "%x.sav", OffSet);
            //                pFile2 = fopen(filename, "w+b");
            //              fseek(pFile2, 0, SEEK_SET);
            //                WriteOut("Saveing TRANSFER %s DATA\n", filename);
            //                fwrite(TempRomFileBuf, sizeof(BYTE), TempMaxBlkSize, pFile2);
            //                fclose(pFile2);
            //            }
            //
            //            WriteOut("Saveing TRANSFER DATA\n");
            //          fseek(pFile1, OffSet, SEEK_SET);
            //          fwrite(TempRomFileBuf, sizeof(BYTE), TempMaxBlkSize, pFile1);
            //    )

            //            __OFBDMEDBG(
            //                WriteOut("ddBlkNum = 0x%02X, TempMaxBlkSize = 0x%05X, dd4KTotalBlks = 0x%02X, bStatus = 0x%04X, bType = 0x%02X, OffSet = 0x%08X\n", 
            //                          ddBlkNum, TempMaxBlkSize, dd4KTotalBlks, bStatus, bType, OffSet);
            //            )

            do {
                ret = MER_INTERFACE(TempRomFileBuf, TempMaxBlkSize, bType, ME_REQUEST_TRANSFER_DATA, 
                                        dd4KTotalBlks, dd4KBlksIndex, &IsNeedToCallAgain);
            }while (IsNeedToCallAgain);

            //            if (Callback)
            //              Callback(0 + OffSet, g_MER_BlkDataInfo->METotalSize, TempMaxBlkSize);
                        
            if (eEID_NO_ERROR != ret)
            {
                FreeMem(TempRomFileBuf);
                    return FALSE;
            }
        }

//__OFBDMEDBG(
//        fclose(pFile1);
//)


#else
    for(ddBlkNum = 0; ddBlkNum < count; ddBlkNum++)
    {
        // Get Block address and size from BlockInfo structure
        ddBlkAddr = pMEBlkInfo[ddBlkNum].StartAddress;
        ddBlkSize = pMEBlkInfo[ddBlkNum].BlockSize;
        bStatus   = pMEBlkInfo[ddBlkNum].Lock;
        bType     = pMEBlkInfo[ddBlkNum].Type;
        if (bType > MER_BLK) continue;
        
        DWORD OffSet = 0;            
        DWORD dd4KTotalBlks = ddBlkSize/TempMaxBlkSize;
        DWORD dd4KBlksIndex;
        BOOLEAN  IsNeedToCallAgain = FALSE;
        
        
        ENUM_ERR_ID ret;            
        
        if (Callback)
            Callback (0xfffffffe, g_MER_BlkDataInfo->METotalSize, 0);
        
        if (ddBlkSize % TempMaxBlkSize)
        dd4KTotalBlks += 1;
        dd4KBlksIndex = 0;

        __OFBDMEDBG(
                WriteOut("ddBlkNum = 0x%02X, TempMaxBlkSize = 0x%05X, dd4KTotalBlks = 0x%02X, bStatus = 0x%04X, bType = 0x%02X\n", 
                          ddBlkNum, TempMaxBlkSize, dd4KTotalBlks, bStatus, bType);
        )

            for(dd4KBlksIndex = 0 ; dd4KBlksIndex < dd4KTotalBlks ; dd4KBlksIndex++)
            {                
            OffSet = TempMaxBlkSize*dd4KBlksIndex;
            
            if((OffSet + TempMaxBlkSize) > ddBlkSize )
                TempMaxBlkSize = ddBlkSize - OffSet;
            
            // Copy block's data from ROM file to temporary buffer          
            MemCpy (TempRomFileBuf, pMEFileBuf + ddBlkAddr + OffSet, TempMaxBlkSize);
            
            __OFBDMEDBG(WriteOut("dd4KBlksIndex = %d\n", dd4KBlksIndex);)


            __OFBDMEDBG(
            
                FILE *pFile1;
                WriteOut("Saveing TRANSFER DATA\n");
                
                pFile1 = fopen("tdata.sav", "w+b");
                fseek(pFile1, ddBlkAddr + OffSet, SEEK_SET);
                fwrite(TempRomFileBuf, sizeof(BYTE), TempMaxBlkSize, pFile1);
                fclose(pFile1);
            
            )

            do {
                __OFBDMEDBG(
                WriteOut("TempRomFileBuf = 0x%08X, TempMaxBlkSize = 0x%05X, bType = 0x%02X, dd4KTotalBlks = 0x%02X, dd4KBlksIndex = 0x%04X\n", 
                      TempRomFileBuf, TempMaxBlkSize, bType, dd4KTotalBlks, dd4KBlksIndex);
                )                                        
                
                    ret = OFBD_MER_INTERFACE(TempRomFileBuf, TempMaxBlkSize, bType, ME_REQUEST_TRANSFER_DATA, 
                                 dd4KTotalBlks, dd4KBlksIndex, IsNeedToCallAgain);
                
                __OFBDMEDBG(WriteOut("IsNeedToCallAgain = %d\n", IsNeedToCallAgain);)
    
            }while (IsNeedToCallAgain);
                
                    if (Callback)
                        Callback(ddBlkAddr + OffSet, g_MER_BlkDataInfo->METotalSize, TempMaxBlkSize);
                            
                if (eEID_NO_ERROR != ret)
                {
                    FreeMem(TempRomFileBuf);
                    return FALSE;
                }
        }
        }
#endif
        
        FreeMem(TempRomFileBuf);
    }

//    if (Callback)
//      Callback (0xfffffffe, g_MER_BlkDataInfo->METotalSize, 0);    

    return TRUE;
}



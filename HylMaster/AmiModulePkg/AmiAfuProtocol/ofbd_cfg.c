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
ofbd_cfg.c

This is the ofbd function file for X64.

*/
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FlashProtocol.h>
#include <Protocol/CPU.h>
#include <PPI/FwVersion.h>
#include <Flash.h>
#include <Setup.h>
#include <FFS.h>
#include <AmiLib.h>
#include <EFI.h>

#include <Protocol/AmiAfuProtocol.h>
#include <AmiAfuProtocolElinks.h>

/*****************************************************************************
	OFBD_CFG.CPP
*****************************************************************************/
#include "ofbd_cfg.h"
#include "SmiInterface.h"



/*****************************************************************************
	GLOBALS
*****************************************************************************/
static CHAR OfbdOEMString[256] = {0};
static BOOL bRS_DIS_OEMSTR = FALSE;
static BOOL bRS_ERR_OUT = FALSE;

WORD	g_OfbdVer = 0;
WORD	g_PlatformType[] = {
							OFBD_OS_EFI,		// 0 - AFUEFI
#if !AFUGuiMfc_Support
							OFBD_OS_WIN,		// 1 - AFUWIN
#else
                            OFBD_OS_WINGUI,     // 1 - AFUWIN_GUI
#endif
							0,					// 2 - Reserved
							OFBD_OS_DOS,		// 3 - AFUDOS
							OFBD_OS_LINUX,		// 4 - AFULNX
							OFBD_OS_FREEBSD		// 5 - AFUBSD
							};


extern UINT8 *	gpIntrBuf;

UINT32 	StringLength(const CHAR *Src);
BOOL 	CallOemSmi(BYTE smi_value);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: Ofbd_Desc_Header
//
// Description:	initial generic OFBD Header structure with given param ddFS
//				and pMemBuf.
//
// Input: PBYTE pMemBuf, DWORD ddFS.
//
// Output: None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
void Ofbd_Desc_Header(PBYTE pMemBuf, DWORD ddFS)
{
	POFBD_STRUCT_V2 pOFBDV2 = (POFBD_STRUCT_V2)pMemBuf;

//	memcpy(&pOFBDV2->ddSignature, &OFBD_SIG, 4);
	pOFBDV2->ddSignature = 0x4442464F;				// "OFBD" Signature

	pOFBDV2->dwVersion = g_OfbdVer;
	pOFBDV2->ddFlashStatus = ddFS;
	pOFBDV2->dwSize = sizeof(OFBD_STRUCT_V2);		// update size
	pOFBDV2->dwHdrSize = sizeof(OFBD_STRUCT_V2);
	//pOFBDV2->dwOS = g_PlatformType[Platform_Type];
	pOFBDV2->dwOS = g_PlatformType[0];
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: Ofbd_Ext_Header
//
// Description:	initial OFBD TypeCode Header structure with given param dbTypeID
//
//				and pMemBuf and dbNumHole.
//
// Input: PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole.
//
// Output: None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
void Ofbd_Ext_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole)
{
	POFBD_STRUCT_V2 pOFBDV2 = (POFBD_STRUCT_V2)pMemBuf;
	POFBD_TYPECODE_V2 pOFBDTCV2 = (POFBD_TYPECODE_V2)(pMemBuf+pOFBDV2->dwSize);

	switch (dbTypeID) {
		case OFBD_EXT_TC_ROMH :		// 0x50
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ROMHOLE_V2);
			break;
		case OFBD_EXT_TC_CFG :		// 0x51
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_DEFSET);
			break;
		case OFBD_EXT_TC_PWD :		// 0x52
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_PWD);
			break;
		case OFBD_EXT_TC_ORI :		// 0x53
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ORI_BD_Struct);
			break;
		case OFBD_EXT_TC_EC :		// 0x54
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_EC);
			break;
		case OFBD_EXT_TC_MEUD :		// 0x55
            if (Ofbd_GetVer() >= OFBD_VER_21)
			    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_MEUD_NEW);
			else
			    pOFBDTCV2->dwTCStructSize = sizeof(OFBD_MEUD);			
			break;
		case OFBD_EXT_TC_SETUP :	// 0x56
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_SETUP);
			break;
		case OFBD_EXT_TC_OEM_CMD :	// 0x57
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_OEM_CMD);
			break;
        case OFBD_EXT_TC_CLN_ERR_LOG:	// 0x58
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_CEL);
			break;
        case OFBD_EXT_TC_OA_HANDLE : 	//0x5A
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_NOA);
			break;
        case OFBD_EXT_TC_AC_BAT:        //0x5B
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_ECB);
            break;
        case OFBD_EXT_TC_GET_PATTERN:        //0x5C	Add By Terry
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_GETPAT);
            break;
		case OFBD_EXT_TC_SECURE_FLASH :		// 0x70
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_SECURE_FLASH);
			break;		
		case OFBD_EXT_TC_AFUMFG:           // 0x71
            pOFBDTCV2->dwTCStructSize = sizeof(OFBD_AFUMFG);
            break;
		case OFBD_EXT_TC_BIOS_CFG_PRESERVE: // 0x80
			pOFBDTCV2->dwTCStructSize = sizeof(OFBD_EXT_TC_BIOS_CFG_PRESERVE);
			break;		
	}
	pOFBDV2->dwSize += sizeof(OFBD_TYPECODE_V2);

	pOFBDTCV2->dbCode = dbTypeID;
	pOFBDTCV2->dbNumHole = dbNumHole;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: Ofbd_TC_End_Header
//
// Description:
//
// Input: PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole.
//
// Output: None.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
void Ofbd_TC_End_Header(PBYTE pMemBuf, BYTE dbTypeID, BYTE dbNumHole)
{
	POFBD_STRUCT_V2 pOFBDV2 = (POFBD_STRUCT_V2)pMemBuf;
	POFBD_EXTHDR_END pOFBD_ExtEnd = (POFBD_EXTHDR_END)pMemBuf;

	switch (dbTypeID) {
		case OFBD_EXT_TC_ROMH :		// 0x50
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ROMHOLE_V2) * dbNumHole);
			break;
		case OFBD_EXT_TC_CFG :		// 0x51
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_DEFSET) * dbNumHole);
			break;
		case OFBD_EXT_TC_PWD :		// 0x52
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_PWD) * dbNumHole);
			break;
		case OFBD_EXT_TC_ORI :		// 0x53
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ORI_BD_Struct) * dbNumHole);
			break;
		case OFBD_EXT_TC_EC :		// 0x54
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_EC) * dbNumHole);
			break;
		case OFBD_EXT_TC_MEUD :		// 0x55
			if (Ofbd_GetVer() >= OFBD_VER_21)
				pOFBDV2->dwSize += (WORD)(sizeof(OFBD_MEUD_NEW) * dbNumHole); 	// update size
			else
				pOFBDV2->dwSize += (WORD)(sizeof(OFBD_MEUD) * dbNumHole); 		// update size  
			break;
		case OFBD_EXT_TC_SETUP :	// 0x56
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_SETUP) * dbNumHole);
			break;
		case OFBD_EXT_TC_OEM_CMD :	// 0x57
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_OEM_CMD) * dbNumHole);
			break;
		case OFBD_EXT_TC_CLN_ERR_LOG:	// 0x58
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_CEL) * dbNumHole); 		// update size
			break;
		case OFBD_EXT_TC_OA_HANDLE : 	//0x5A
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_NOA) * dbNumHole);
			break;
		case OFBD_EXT_TC_AC_BAT:        //0x5B
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_ECB) * dbNumHole);
			break;
		case OFBD_EXT_TC_GET_PATTERN:        //0x5C	Add By Terry
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_GETPAT) * dbNumHole);
			break;
		case OFBD_EXT_TC_SECURE_FLASH :		// 0x70
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_SECURE_FLASH) * dbNumHole);
			break;
		case OFBD_EXT_TC_AFUMFG:            // 0x71
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_AFUMFG) * dbNumHole);
			break;
		case OFBD_EXT_TC_BIOS_CFG_PRESERVE: // 0x80
			pOFBDV2->dwSize += (WORD)(sizeof(OFBD_PRESERVE_SETUP_CONFIG) * dbNumHole);
			break;
	}

	pOFBD_ExtEnd = (POFBD_EXTHDR_END)((unsigned long)pOFBD_ExtEnd + (unsigned long) pOFBDV2->dwSize);
	pOFBD_ExtEnd->ddHdrEnd = OFBD_EXT_TBL_END;		// fill end value
	pOFBDV2->dwSize += sizeof(OFBD_EXT_TBL_END);	// update size
}

//----------------------------------------------------------------------------
//
// Procedure:	Ofbd_VersionCheck
//
// Description:	Get OFBD version check
//
// Input: None.
//
// Output: None.
//
//----------------------------------------------------------------------------
ENUM_ERR_ID Ofbd_VersionCheck()
{
	PBYTE 	pOFBD_Buffer = (PBYTE)NULL;
	WORD    wOFBD_RS = 0;
	DWORD   dwSize = 0;

	g_OfbdVer = 0;
	
	pOFBD_Buffer = AllocMem (FLASH_PAGE_SIZE+4);		// 1024
	
	if (pOFBD_Buffer == NULL)
		return eEID_MEM_ALLOC;	
	
	MemSet(pOFBD_Buffer, FLASH_PAGE_SIZE, 0);
	
	Ofbd_Desc_Header(pOFBD_Buffer, 0);
	Ofbd_Ext_Header(pOFBD_Buffer, 0, 1);
	Ofbd_TC_End_Header(pOFBD_Buffer, 0, 1);
	//Copy Data To gpIntrBuf (Send SMI Buffer)
	MemCpy(gpIntrBuf, pOFBD_Buffer, FLASH_PAGE_SIZE);
	
	/*POFBD_STRUCT_V2 pOFBDV2 = (POFBD_STRUCT_V2)pOFBD_Buffer;
	DWORD ddEndOffset = (WORD)(pOFBDV2->dwSize - sizeof(OFBD_EXTHDR_END));
	POFBD_EXTHDR_END pOFBD_ExtEnd = (POFBD_EXTHDR_END)((unsigned long)pOFBDV2 + (unsigned long)ddEndOffset);
	pOFBD_ExtEnd->ddHdrEnd = gIntrBufPhyAddr;*/
    	
	//OutPortByte (gSmiPort, OFBD_SWSMI_PORT);
	Ofbd_SMI_26();
	
	wOFBD_RS = ((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus;
	
	//WriteOut("wOFBD_RS = 0x%8X\n", wOFBD_RS);
	
	if (!(((POFBD_STRUCT_V2)gpIntrBuf)->dwRetuenStatus & OFBD_RS_SUPPORT))
	{
		FreeMem(pOFBD_Buffer);
			g_OfbdVer = 0;
		return eEID_NO_ERROR;
	}

	// Add for EIP 40463 requirement +>>> 2010/06/28
	if(wOFBD_RS & OFBD_RS_DIS_OEMSTR) 
	{
		UINT32 strlen = 0;		

		dwSize = ((POFBD_STRUCT_V2)gpIntrBuf)->dwSize;
		strlen = StringLength((CHAR *)gpIntrBuf + dwSize);
		if (strlen > 0)
		{
			bRS_DIS_OEMSTR = TRUE;

			//if (strlen > 256)
			MemCpy(OfbdOEMString, (PCHAR)gpIntrBuf + dwSize, sizeof(CHAR)*strlen);
/*			else	
				WriteOut(" %s \n", (CHAR *)gpIntrBuf + dwSize);*/
		}
	}
	
	if(wOFBD_RS & OFBD_RS_ERR_OUT)
	{
		bRS_ERR_OUT = TRUE;
		FreeMem(pOFBD_Buffer);
			return eEID_OFBD_OEM_ERROR;//AFUExit (eEID_NO_ERROR);
		// Add for EIP 40463 requirement <<<+
	}

	FreeMem(pOFBD_Buffer);
	g_OfbdVer = ((POFBD_STRUCT_V2)gpIntrBuf)->dwVersion;

	return eEID_NO_ERROR;
}


//----------------------------------------------------------------------------
//
// Procedure:	Ofbd_GetVer
//
// Description:	Get OFBD version
//
// Input: None.
//
// Output: None.
//
//----------------------------------------------------------------------------
WORD Ofbd_GetVer()
{
    if (!g_OfbdVer)
        Ofbd_VersionCheck();

	return g_OfbdVer;
}

//----------------------------------------------------------------------------
//
// Procedure:	Ofbd_GetOEMString
//
// Description:	Get OFBD OEM String (Max 256)
//
// Input: None.
//
// Output: None.
//
//----------------------------------------------------------------------------
PCHAR Ofbd_GetOEMString()
{
	return OfbdOEMString;
}

BOOL  Ofbd_GetRS_ERR_OUT()
{
	return bRS_ERR_OUT;
}

BOOL  Ofbd_RS_DIS_OEMSTR()
{
	return bRS_DIS_OEMSTR;
}

VOID  Ofbd_SMI_26()
{
	CallOemSmi(OFBD_SWSMI_PORT);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
PBYTE AllocMem(DWORD Size)
{
	EFI_STATUS		Status;
	UINTN			uSize;
	PBYTE			buffer;

	uSize = Size;
	Status = pBS->AllocatePool(EfiLoaderData, uSize, (VOID**)&buffer);

	if (EFI_ERROR(Status)) return 0;
	return buffer;
}

BOOL FreeMem(PBYTE MemBlk)
{
	if (EFI_SUCCESS == pBS->FreePool(MemBlk)) return TRUE;
	return FALSE;
}

UINT32 StringLength(const CHAR *Src)
{
	UINT32	i = 0;
	while (Src[i] != 0) i++;
	return i;
}

BOOL CallOemSmi(BYTE smi_value)
{
	SendSmi(smi_value);
	return TRUE;
}



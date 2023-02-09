//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Protocol/FlashProtocol.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <token.h>

#ifndef EFI_DEFAULT_BMP_LOGO_GUID
#define EFI_DEFAULT_BMP_LOGO_GUID \
  {0xE1A15429,0xB271,0x4081,0x9C,0x7D,0x05,0xC1,0xE1,0xE3,0xA9,0x24}
#endif

EFI_GUID gBmpLogoGuid = EFI_DEFAULT_BMP_LOGO_GUID;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: FindLogoFfs
//
// Description: Find Logo FFS in FV.
//
// Input: IN EFI_FIRMWARE_VOLUME_HEADER *FvHdr - Firmware volume to search
//
// Output:  UINT8 * - Return FFS.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 *FindLogoFfs(IN EFI_FIRMWARE_VOLUME_HEADER *FvHdr)
{
    UINT8 *FvPtr = (UINT8*)FvHdr + FvHdr->HeaderLength;
    UINT8 *EndOfFv = (UINT8*)FvHdr + FvHdr->FvLength-1;
     DEBUG ((0xffffffff,  "\nSearching....FvPtr=%x, *EndOfFv=%x...", FvPtr,  EndOfFv));
    //Search the FV_BB firmware volume for the microcode file.
    while (FvPtr < EndOfFv) {
//	DEBUG ((0xffffffff,  "\nFFS Address = 0x%08x", FvPtr));
        if (CompareGuid(&gBmpLogoGuid, &((EFI_FFS_FILE_HEADER*)FvPtr)->Name))
            return FvPtr;

        FvPtr += *(UINT32*)&((EFI_FFS_FILE_HEADER*)FvPtr)->Size & 0xffffff;
        FvPtr = (UINT8*)(((UINTN)FvPtr + 7) & ~7);   //8 byte alignment
    }
    return NULL;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: InitBmpLogoImage
//
// Description: Retrieve Logo Image Data.
//
// Input: VOID
//
// Output:  BOOLEAN - TRUE if initialized. FALASE if error.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN InitBmpLogoImage(VOID **BmpImage, UINT32 *Width, UINT32 *Height, UINT16 *Depth, VOID **Palette)
{
	UINT8    *FfsPtr, *BmpFilePtr;
	UINT32  BmpFileSize;
	
	UINT8 *BmpImagePtr;
	EFI_FIRMWARE_VOLUME_HEADER* Fv;//<lvych001-20170807 Revised search the log file hang when bios recovery.>
	
	DEBUG ((0xffffffff,  "\nFV_BB_BASE = 0x%08x", (UINTN)FV_BB_BASE));
	//<lvych001-20170807 Revised search the log file hang when bios recovery.+>
	Fv = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FV_MAIN_BASE;
	if(Fv->Signature != EFI_FVH_SIGNATURE)
	  return FALSE;
	//<lvych001-20170807 Revised search the log file hang when bios recovery.->
	FfsPtr = FindLogoFfs((EFI_FIRMWARE_VOLUME_HEADER*)(UINTN)FV_MAIN_BASE);
	if (FfsPtr == NULL) return FALSE;
	BmpFilePtr = FfsPtr + sizeof(EFI_FFS_FILE_HEADER);
	BmpFileSize = ((*(UINT32*)((EFI_FFS_FILE_HEADER*)FfsPtr)->Size) & 0xffffff);
	
	DEBUG ((0xffffffff,  "\nFound Bmp File at 0x%08x, Size = %d\n", BmpFilePtr, BmpFileSize));
	
	BmpImagePtr = (UINT8 *)*BmpImage;
	BmpImagePtr =  BmpFilePtr + *(UINT32 *)(BmpFilePtr + 0xa); // Bitmap offset is 0xa
	
	
	*BmpImage = BmpFilePtr; //<lizhq040-20170120 Auto identify logo resolution>  //*BmpImage = BmpImagePtr;
	*Palette = BmpFilePtr + 54;
	*Width = *(UINT32 *)(BmpFilePtr + 18);  //Bitmap info Width offset 18
	*Height = *(UINT32 *)(BmpFilePtr + 22); //Bitmap info Height offset 22
	 *Depth = *(UINT16 *)(BmpFilePtr + 28); //Bitmap info 
#if 0	
	//For Debug
	{
		UINT32 i;
		for(i=0; i<256*4; i++)
		{
			if(i%16==0) 	DEBUG ((0xffffffff, "\n"));
			DEBUG((0xffffffff, "%02x ", *((UINT8 *)*Palette+i) ));
		}
	}	
#endif	
	return  TRUE;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

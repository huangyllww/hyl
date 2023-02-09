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
Afusysrom.h

This is the header file of AFU generic functions .

*///**********************************************************************
#ifndef _AFUSYSROM_H_
#define _AFUSYSROM_H_

#include "afufile.h"

#define EFI_FFS_GUID\
        {0x7a9354d9, 0x0468, 0x444a, {0x81, 0xce, 0x0b, 0xf6, 0x17, 0xd8, 0x90, 0xdf}}


#define AFU_MAXBLOCKSIZE    0x10000

#define MAXNCBLK    16

enum{
    RH_NULL = 0,
    RH_ROMHOLE,
    RH_SLP,

    RH_MAX
};

#pragma pack(1)

// struct EFI_FFS_FILE_HEADER;
// Firmware Volume Header definition
typedef struct EFI_FIRMWARE_VOLUME_HEADER {
    UINT8                   ZeroVector[16];
    EFI_GUID                FileSystemGuid; 
    UINT32                  FvLength;
    UINT32                  Reserve;
    UINT32                  Signature;
    UINT32                  Attributes;
    UINT16                  HeaderLength;
    UINT16                  Checksum;
    UINT8                   Reserved[3];
    UINT8                   Revision;
    EFI_FV_BLOCK_MAP_ENTRY  FvBlockMap[1];  //Variable number of entries
}FVHDR, *LPFVHDR;

// Ffs header Definition
typedef struct EFI_FFS_HEADER {
    EFI_GUID                Name; 
    UINT16                  IntegrityCheck;
    UINT8                   Type;
    UINT8                   Attributes;
    UINT8                   Size[3];
    UINT8                   State;
    UINT32                  FileSize;
    UINT32                  AlignedFileSize;
    /*
    UINT32                  FileSize() const { //File size is 24 bits.
        return (*(UINT32 *)Size) & 0xffffff;
    }

    UINT32                  AlignedFileSize() const {
        return (FileSize()+7)&~7;
        
    }*/
}FFSHDR, *LPFFSHDR;

//-----------------------------------------------
//  Rom Hole Block
//-----------------------------------------------
/** Rom Hole Block Struct ROMHOLEREC */
typedef struct _ROMHOLEREC
{
    EFI_GUID Guid;      ///< Rom Hole GUID.
    UINT32 FileOffset;  ///< File Rom Hole Offset Address.
    UINT32 RomOffset;   ///< System Rom Hole Offset Address.
    UINT32 Size;        ///< Rom Hole Size.
    UINT32 Type;        ///< Rom Hole Type.
    struct  _ROMHOLEREC *Next;
}
ROMHOLEREC, *LPROMHOLEREC;

typedef struct _DMIDATA{
    PBYTE   lpData;     ///< DMI Buffer
    UINT8   DMIPlace;   ///< DMI Place
    DWORD   DMIIndex;   ///< DMI Index
    DWORD   Size;       ///< DMI Size
    DWORD   BlockAdd;   ///< DMI from FV block address.
    DWORD   BlockSize;  ///< DMI from FV block size.
}DMIDATA, *LPDMIDATA; 

typedef struct sDMIStruct {
    UINT8       Signature[4];       // _ASB sign
    UINT32      DataLength;
} DMIBlock;


//ROM info struct
typedef struct sBlockInfo {
    UINT32      StartAddress;       // starting address of block in image
    UINT32      BlockSize;          // size of block
    UINT8       Type;               // type of current block
} BlockInfo_T;

typedef struct sROMInfoOld {
    UINT32      Length;             // length of current structure to be assigned by caller
    UINT8       Implemented;        // bios return status on flash interface support
    UINT8       Version;            // core version
    UINT16      TotalBlocks;        // Total no. of blocks in image
    BlockInfo_T BlockInfo;          // pointer to block info
} ROMInfo_Old;

typedef struct sROMInfo {
    UINT32      Length;             // length of current structure to be assigned by caller
    UINT8       Implemented;        // bios return status on flash interface support
    UINT8       Version;            // core version
    UINT16      TotalBlocks;        // Total no. of blocks in image
    UINT32      ECVersionOffset;    // EC version offset
    UINT32      ECVersionMask;      // EC version mask
    BlockInfo_T BlockInfo;          // pointer to block info
} ROMInfo;

typedef struct sROMInfoEX {
    UINT32      Length;             // length of current structure to be assigned by caller
    UINT8       Implemented;        // bios return status on flash interface support
    UINT8       Version;            // core version = 14
    UINT16      TotalBlocks;        // Total no. of blocks in image
    UINT32      ECVersionOffset;    // EC version offset
    UINT32      ECVersionMask;      // EC version mask  
    UINT32      BiosRomSize;        // All BIOS Size
    UINT32      BaseBlockSize;      // Base Block Size  
    UINT8       Reserved[2];        //
    BlockInfo_T BlockInfo;          // pointer to block info    
} ROMInfoEX;

typedef struct _NCBREC
{
    UINT8   BlockType;  ///< NCB Block Type.
    UINT32  Offset;     ///< NCB Block Offset Address.
    UINT32  Size;       ///< NCB Block Size.
}
NCBREC, *LPNCBREC;

#pragma pack()

BOOLEAN     AFU_SysRom_GetFVRang(UINT8 Fvtype, UINT32* pFVStAddr, UINT32* pFV_AllSize, UINT16* pPrevBlockIndex);
BOOLEAN     AFU_SysRom_FV_FFS_DMIGUID(LPFFSHDR lpFfsHand);
BOOLEAN     AFU_SysRom_FV_FFS_OA31KEYGUID(LPFFSHDR lpFfsHand);
INT32       AFU_SysRom_FV_FFS_GUIDRang(LPFFSHDR lpFfsHand);
DWORD       AFU_SysRom_GetRomHole(PBYTE lpSysbuff, DWORD StartAddr, DWORD BlockSize, LPROMHOLEREC lpRomHoleRec);
BOOLEAN     AFU_SysRom_SearchNCB(PBYTE lpSysbuff);
BOOLEAN     AFU_SysRom_HaveSLP20();

#endif

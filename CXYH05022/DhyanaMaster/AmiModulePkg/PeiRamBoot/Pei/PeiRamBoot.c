//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file PeiRamBoot.c
    PEI RAM BOOT Pei driver.
**/
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <PEI.h>
#include <AmiPeiLib.h>
#include <ppi/ReadOnlyVariable2.h>
#include <PeiRamBootElinks.h>
#include <Library/AmiReportFVLib.h>
#include <ppi/RecoveryModule.h>
#include <RomLayout.h>
#include <Token.h>
#include <PeiRamBoot.h>
// "CorePeiMain.h" is a duplicated file from "MdeModulePkg/Core/pei/PeiMain.h" 
// to "Build" Directory when building process..
#include <CorePeiMain.h>
#if defined (SecureBoot_SUPPORT) && SecureBoot_SUPPORT == 1
#include <AmiCertificate.h>
#endif // #if defined (SecureBoot_SUPPORT) && SecureBoot_SUPPORT == 1
#if (CORE_COMBINED_VERSION >= 0x5000a)
#include <AmiHobs.h>
#include <Library/AmiRomLayoutLib.h>
//----------------------------------------------------------------------------
// Function Externs
#else  // #if (CORE_COMBINED_VERSION < 0x5000a)
EFI_STATUS GetRomLayout(
    IN  EFI_PEI_SERVICES **PeiServices,
    OUT ROM_AREA         **Layout
);
#endif // #if (CORE_COMBINED_VERSION >= 0x5000a)

extern
VOID
SwitchPeiServiceDataToRam (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN HOB_ROM_IMAGE                *HobRomImage
);
#if (PI_SPECIFICATION_VERSION >= 0x0001000A)
extern
BOOLEAN
IsPeimDispatched (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_FFS_FILE_HEADER          *FfsFile,
    IN UINTN                        Index
);
#else
extern
UINT64
GetDispatchedPeimBitMap (
    IN EFI_PEI_SERVICES             **PeiServices
);
#endif // #if (PI_SPECIFICATION_VERSION >= 0x0001000A)
typedef BOOLEAN (PEI_RAM_BOOT_ELINK) (EFI_PEI_SERVICES **PeiServices);
extern PEI_RAM_BOOT_ELINK PEI_RAM_BOOT_LIST EndOfPeiRamBootList;
PEI_RAM_BOOT_ELINK* IsMrcColdBooteLink[] = {PEI_RAM_BOOT_LIST NULL};
//----------------------------------------------------------------------------
// Local prototypes
#define ROM_LAYOUT_FFS_GUID \
{ 0x0DCA793A, 0xEA96, 0x42d8, 0xBD, 0x7B, 0xDC, 0x7F, 0x68, 0x4E, 0x38, 0xC1 }
#define FID_FFS_FILE_NAME_GUID \
{ 0x3fd1d3a2, 0x99f7, 0x420b, 0xbc, 0x69, 0x8b, 0xb1, 0xd4, 0x92, 0xa3, 0x32 }
#define PKEY_FILE_GUID \
{ 0xCC0F8A3F, 0x3DEA,  0x4376, 0x96, 0x79, 0x54, 0x26, 0xba, 0x0a, 0x90, 0x7e }
#define KEK_FILE_GUID \
{ 0x9fe7de69, 0xaea, 0x470a, 0xb5, 0xa, 0x13, 0x98, 0x13, 0x64, 0x91, 0x89 }
#define DB_FILE_GUID \
{ 0xfbf95065, 0x427f, 0x47b3, 0x80, 0x77, 0xd1, 0x3c, 0x60, 0x71, 0x9, 0x98 }
#define DBX_FILE_GUID \
{ 0x9d7a05e9, 0xf740, 0x44c3, 0x85, 0x8b, 0x75, 0x58, 0x6a, 0x8f, 0x9c, 0x8e }

static EFI_GUID gHobRomImageGuid = ROM_IMAGE_MEMORY_HOB_GUID;
static EFI_GUID gEfiPeiEndOfPeiPhasePpiGuid = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
static EFI_GUID gRomImageAddressGuid = ROM_IMAGE_ADDRESS_GUID;
static EFI_GUID gSmbiosFlashDataFfsGuid = SMBIOS_FLASH_DATA_FFS_GUID;
static EFI_GUID gRomCacheEnablePpiGuid = ROM_CACHE_ENABLE_PPI_GUID;
static EFI_GUID gLastFfsFileOverrideGuid = \
{0x45B9618F, 0xBAA1, 0x421B, { 0x94, 0xF0, 0xB9, 0xEB, 0xDD, 0x2B, 0xA1, 0x77 }};

EFI_STATUS
PeiRamBootMemoryReady (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *NullPpi
);

EFI_STATUS
PeiRamBootEndOfPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *NullPpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR PeiRamBootMemoryReadyNotify[] =
{
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
        EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiMemoryDiscoveredPpiGuid, // gCacheInstallGuid/gPeiPermanentMemInstalled
        PeiRamBootMemoryReady
    }
};
static EFI_PEI_NOTIFY_DESCRIPTOR PeiRamBootEndOfPeiNotify[] =
{
    // For attribute with ROM_AREA_FV_PEI and ROM_AREA_FV_PEI_MEM
#if COPY_TO_RAM_WHILE_DISPATCH == 1
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
        &gRomCacheEnablePpiGuid,
        PeiRamBootEndOfPei
    },
#endif // #if COPY_TO_RAM_WHILE_DISPATCH == 1
    // For attribute with ROM_AREA_FV_DXE only
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
        EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiEndOfPeiPhasePpiGuid,
        PeiRamBootEndOfPei
    }
};
//----------------------------------------------------------------------------
// Local Variables
UINT32 FvBootBlocksList[] = {
    PEI_RAM_BOOT_FV_BOOTBLOCK_LIST
    FV_BB_BASE,
    { -1 }
};

//-EFI_GUID PreservedFfsGuid[] = {
//-    ROM_LAYOUT_FFS_GUID,
//-    FID_FFS_FILE_NAME_GUID,
//-#if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC == 0
//-    SMBIOS_FLASH_DATA_FFS_GUID,
//-#endif // #if !defined(SMBIOS_DMIEDIT_DATA_LOC) || SMBIOS_DMIEDIT_DATA_LOC == 0
//-    PEI_RAM_BOOT_FFS_GUID_LIST
//-    { 0 }
//-};

//----------------------------------------------------------------------------
// Function Definitions
/**
 *
 * @param PeiServices 
 * @param Address 
 * @retval TRUE True
 * @retval FALSE False
 *
**/
BOOLEAN
IsBootBlockFirmwareVolumes (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PHYSICAL_ADDRESS         Address
)
{
    UINT8           i;
#if (PI_SPECIFICATION_VERSION >= 0x0001000A)
    PEI_CORE_INSTANCE           *Private = NULL;
    Private = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);
    if (Private->Fv[0].FvHeader == (EFI_FIRMWARE_VOLUME_HEADER*)Address) 
        return TRUE;
#endif // #if (PI_SPECIFICATION_VERSION >= 0x0001000A)
    for (i = 0; FvBootBlocksList[i] != -1; i++)
        if (Address == (EFI_PHYSICAL_ADDRESS)FvBootBlocksList[i]) return TRUE;
    return FALSE;    
}
/**
 * This procedure allocate memroy buffer for copying rom to ram.
 *
 * @param PeiServices 
 * @param HobRomImage 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PrepareForCopyRomToRam (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage
)
{
    UINT8                   i;
    EFI_STATUS              Status = EFI_SUCCESS;
    UINTN                   NumOfPages;
    EFI_PHYSICAL_ADDRESS    Buffer = 0;
    UINT32                  FvAlignment = 0;
    UINT32                  Alignment = 0;

    for (i = 0; i < HobRomImage->NumOfFv; i++) {
        EFI_FIRMWARE_VOLUME_HEADER *FvHeader = NULL;
        NumOfPages = HobRomImage->FvInfo[i].UsedBytes;
        if (HobRomImage->FvInfo[i].FvLength != HobRomImage->FvInfo[i].UsedBytes) {
#if SAVE_ENTIRE_FV_IN_MEM == 0
            // 0x1200 = 1)4k aligned, 2)NULL bytes of FV
            NumOfPages = HobRomImage->FvInfo[i].UsedBytes + 0x1200;
#else
            NumOfPages = HobRomImage->FvInfo[i].FvLength;
#endif
        }
        //If firmware volume alignment is bigger than 4K, enlarge the memory.
        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)HobRomImage->FvInfo[i].FvAddress;
        FvAlignment = (FvHeader->Attributes) & EFI_FVB2_ALIGNMENT;
        if( FvAlignment > EFI_FVB2_ALIGNMENT_4K ) {
            Alignment = 1 << (FvAlignment >> 16);
            NumOfPages += Alignment;
        }
        Status = (*PeiServices)->AllocatePages (PeiServices, \
                             EfiBootServicesData, NumOfPages >> 12, &Buffer);
        if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
        // If HobValid is "TRUE", it means FV memories aren't destroyed, just
        // reserve the necessary memory space for FV then exit.
        if (HobRomImage->HobValid == TRUE) continue;
        HobRomImage->FvInfo[i].NumOfPages = NumOfPages >> 12;
        //Make memory address is match with alignment.
        if( FvAlignment > EFI_FVB2_ALIGNMENT_4K ) {
            Buffer += (Alignment - 1);
            Buffer &= (0xFFFFFFFF - (Alignment - 1));
        }
        HobRomImage->FvInfo[i].MemAddress = (UINT32)Buffer;
        HobRomImage->FvInfo[i].FvMemReady = FALSE;
    }
    return EFI_SUCCESS;
}
#if (PI_SPECIFICATION_VERSION >= 0x0001000A) && (OPTIMIZE_BOOT_FV_COPY == 1)
/**
 * This routine copy only undispatched PEIM and Preserved FFS Guid
 * to memory and FFS Header only if dispatched PEIM.
 *
 * @param PeiServices 
 * @param HobRomImage 
 * @param Index 
 * @retval VOID
**/
VOID
MinimumizeBootFv (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage,
    IN UINT8                    Index
)
{
    UINTN       k = 0, n = 0, FfsLength;
    UINT8       *p = (UINT8*)HobRomImage->FvInfo[Index].MemAddress;
    UINT8       *q = (UINT8*)HobRomImage->FvInfo[Index].FvAddress;
    EFI_FFS_FILE_HEADER     *FfsFile = NULL;
    EFI_FFS_FILE_HEADER     *PadFfsHeader = NULL;
    EFI_GUID                *pGuid = NULL;
    EFI_STATUS              Status = EFI_SUCCESS;
    EFI_PHYSICAL_ADDRESS    ExtFvHeader;
    UINT32                  ExtFvHeaderOffset;
    UINT32                  ExtFvHeaderLength;
    UINT32                  CopyLength;

    // 1. Copy Firmware Volume Header to Memory buffer.
    CopyLength = ((EFI_FIRMWARE_VOLUME_HEADER*)q)->HeaderLength;
    //If ExtHeader exist
    if( ((EFI_FIRMWARE_VOLUME_HEADER*)q)->ExtHeaderOffset )
    {
        ExtFvHeaderOffset = \
                    ((EFI_FIRMWARE_VOLUME_HEADER*)q)->ExtHeaderOffset;
        ExtFvHeader = (EFI_PHYSICAL_ADDRESS)(q + ExtFvHeaderOffset);
        ExtFvHeaderLength = \
                    ((EFI_FIRMWARE_VOLUME_EXT_HEADER*)ExtFvHeader)->ExtHeaderSize;
        CopyLength = ExtFvHeaderOffset + ExtFvHeaderLength;
    }
    (*PeiServices)->CopyMem ( p, q, CopyLength);
    p += CopyLength;
    //Align on 8 bytes
    (*PeiServices)->SetMem ((UINT8*)p, 0x8, 0xff);
    p = (UINT8*)(((UINT32)p + 7) & 0xfffffff8);
    do {
        Status = (*PeiServices)->FfsFindNextFile (PeiServices, \
              EFI_FV_FILETYPE_ALL, (EFI_FIRMWARE_VOLUME_HEADER*)q, &FfsFile );
        if (EFI_ERROR(Status)) break;
        // calculate file alignment (Align on 8 bytes).
        FfsLength = *(UINT32*)FfsFile->Size & 0xffffff;
        FfsLength = (FfsLength + 7) & 0xfffffff8;
        // Copy undispatched PEIMs and FFS File Header only if dispatched PEIM 
        if ((FfsFile->Type == EFI_FV_FILETYPE_PEI_CORE) || \
            ((FfsFile->Type == EFI_FV_FILETYPE_PEIM) && \
            (!IsPeimDispatched(PeiServices, FfsFile, k)))) n = FfsLength;
        else {
            n = sizeof(EFI_FFS_FILE_HEADER);
            // Copy RAW and FREEFORM FFS file..
            if ((FfsFile->Type == EFI_FV_FILETYPE_FREEFORM) || \
                (FfsFile->Type == EFI_FV_FILETYPE_RAW)) n = FfsLength;
        }
        // Increase PEIM index meeting PEI Core Private Data of PI 1.2  
        if (FfsFile->Type == EFI_FV_FILETYPE_PEIM) k++;    
        (*PeiServices)->CopyMem (p, (UINT8*)FfsFile, n);
        //If the CheckSum is enable, only modify the minimized FFS's attribute and file's checksum.
        if( ((EFI_FFS_FILE_HEADER*)p)->Attributes & FFS_ATTRIB_CHECKSUM ) {
            if( (FfsFile->Type == EFI_FV_FILETYPE_SECURITY_CORE) ||
                ( (FfsFile->Type == EFI_FV_FILETYPE_PEIM) &&
                  IsPeimDispatched(PeiServices, FfsFile, k) ) ) {
                ((EFI_FFS_FILE_HEADER*)p)->Attributes &= (~FFS_ATTRIB_CHECKSUM);
                ((EFI_FFS_FILE_HEADER*)p)->IntegrityCheck.Checksum.Header += FFS_ATTRIB_CHECKSUM;
                ((EFI_FFS_FILE_HEADER*)p)->IntegrityCheck.Checksum.File = 0xAA;
            }
        }
        p = p + FfsLength;
        if(FfsFile->Type == EFI_FV_FILETYPE_SECURITY_CORE) continue;
        // Save PAD_FFS_FILE Header for FvCheck of NotifyFwVolBlock procedure.
        PadFfsHeader = (EFI_FFS_FILE_HEADER*)((UINT8*)FfsFile + FfsLength);
        if( PadFfsHeader->Type == EFI_FV_FILETYPE_FFS_PAD )
        {
            FfsLength = *(UINT32*)(PadFfsHeader->Size) & 0xffffff;
            FfsLength = (FfsLength + 7) & 0xfffffff8;
            n = sizeof(EFI_FFS_FILE_HEADER);
            (*PeiServices)->CopyMem (p, (UINT8*)PadFfsHeader, n);
            p += FfsLength;
        }
    } while(!EFI_ERROR(Status));
}
#endif  // #if (PI_SPECIFICATION_VERSION >= 0x0001000A) && (OPTIMIZE_BOOT_FV_COPY == 1)
/**
 * This procedure copy in used Firmware Volume to memroy.
 *     
 * @param PeiServices 
 * @param HobRomImage 
 * @param IsEndOfPei 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
CopyFirmwareVolumesToRam (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage,
    IN BOOLEAN                  IsEndOfPei
)
{
    UINT8                   i;

    for (i = 0; i < HobRomImage->NumOfFv; i++) {
        if ((!IsEndOfPei) && (!HobRomImage->FvInfo[i].IsBootFv)) continue;
        else if (IsEndOfPei && HobRomImage->FvInfo[i].IsBootFv) continue;
        if (HobRomImage->FvInfo[i].FvMemReady == TRUE) continue;
        HobRomImage->FvInfo[i].FvMemReady = TRUE;
        if (HobRomImage->HobValid) continue;
#if (PI_SPECIFICATION_VERSION >= 0x0001000A) && (OPTIMIZE_BOOT_FV_COPY == 1)
        if (HobRomImage->FvInfo[i].IsBootFv == TRUE) {
            MinimumizeBootFv (PeiServices, HobRomImage, i);
        }
        else
#endif  // #if (OPTIMIZE_BOOT_FV_COPY == 1)
        {    
            (*PeiServices)->CopyMem ((UINT8*)HobRomImage->FvInfo[i].MemAddress, \
                                     (UINT8*)HobRomImage->FvInfo[i].FvAddress, \
                                     HobRomImage->FvInfo[i].UsedBytes);
        }
        if (HobRomImage->FvInfo[i].UsedBytes != HobRomImage->FvInfo[i].FvLength) 
        {
            UINT32          NumofNullBytes;
#if SAVE_ENTIRE_FV_IN_MEM == 1
            // Fill Null bytes after Used bytes
            NumofNullBytes = HobRomImage->FvInfo[i].FvLength - \
                                    HobRomImage->FvInfo[i].UsedBytes;
#else
            // Fill Max 512 Null bytes after Used bytes
            NumofNullBytes = (HobRomImage->FvInfo[i].NumOfPages << 12) - \
                                    HobRomImage->FvInfo[i].UsedBytes;
            if (NumofNullBytes > 512) NumofNullBytes = 512;
#endif  // #if SAVE_ENTIRE_FV_IN_MEM == 1
            (*PeiServices)->SetMem (
                (UINT8*)(HobRomImage->FvInfo[i].MemAddress + \
                                    HobRomImage->FvInfo[i].UsedBytes), \
                NumofNullBytes, FLASH_EMPTY_BYTE );
        }                         
    }
    return EFI_SUCCESS;
}
/**
 *     
 * @param PeiServices 
 * @param HobRomImage 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
CollectRomImageInfo (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage
)
{
    UINT32                  FileSize;
//-    UINT8                   i = 0, j = 0, n = 0;
    UINT8                   i = 0;
    EFI_STATUS              Status;
    EFI_FFS_FILE_HEADER     *FfsFile = NULL, *LastFfsFile = NULL;
    EFI_GUID                *pLastFileOvrdeGuid = NULL;
#if (PI_SPECIFICATION_VERSION < 0x0001000A)
    UINT64                  DispatchedPeimBitMap = GetDispatchedPeimBitMap(PeiServices);
#endif
#if (CORE_COMBINED_VERSION >= 0x5000a)
    AMI_ROM_AREA            *Area, *RomLayout = AmiGetFirstRomArea();
#else
    ROM_AREA                *Area, *RomLayout = NULL;
    if (EFI_ERROR(GetRomLayout(PeiServices, &RomLayout))) RomLayout = (ROM_AREA*)NULL;
#endif
    if (RomLayout == NULL) return EFI_UNSUPPORTED;
    // Get Last File Override Guid for skiping unnecessary files copied
    // for reducing POST time. 
    Status = (*PeiServices)->LocatePpi ( PeiServices, \
                                         &gLastFfsFileOverrideGuid, \
                                         0, \
                                         NULL, \
                                         &pLastFileOvrdeGuid);
    if (EFI_ERROR(Status)) pLastFileOvrdeGuid = NULL;

    // find last ffs file for calculating used rom space for each Firmware Volume.
#if (CORE_COMBINED_VERSION >= 0x5000a)
    for(Area = RomLayout; Area != NULL; \
            Area = AmiGetNextRomArea(Area), FfsFile = NULL, FileSize = 0)
#else
    for (Area = RomLayout; Area->Size != 0; Area++, FfsFile = NULL, FileSize = 0)
#endif
    {        
        if (Area->Type != RomAreaTypeFv) continue;
        if (!(Area->Attributes & (ROM_AREA_FV_PEI_ACCESS + ROM_AREA_FV_DXE)))
            continue;
        // find last ffs file for calculating used rom space.
        do {
//-            IsDispatched = TRUE;
            Status = (*PeiServices)->FfsFindNextFile (
                         PeiServices, \
                         EFI_FV_FILETYPE_ALL, \
                         (EFI_FIRMWARE_VOLUME_HEADER*)(Area->Address), \
                         &FfsFile );
            if (!EFI_ERROR(Status)) {
                // Terminate searching Last File if Last File Override Guid 
                // is defined and is identified.
                if ((pLastFileOvrdeGuid != NULL) && \
                    (!guidcmp(&FfsFile->Name, pLastFileOvrdeGuid))) {
                    FfsFile = LastFfsFile;
                    Status = EFI_NOT_FOUND;
                } else LastFfsFile = FfsFile;
            }
        } while (!EFI_ERROR(Status));
        FileSize = *(UINT32*)LastFfsFile->Size & 0xffffff;
        FileSize += ((UINT32)LastFfsFile - (UINT32)Area->Address);
        HobRomImage->FvInfo[i].FvAddress = (UINT32)Area->Address;
        HobRomImage->FvInfo[i].FvLength = Area->Size;
        HobRomImage->FvInfo[i].UsedBytes = FileSize;
        HobRomImage->FvInfo[i].MemAddress = 0;
        if (IsBootBlockFirmwareVolumes(PeiServices, Area->Address))
            HobRomImage->FvInfo[i].IsBootFv = TRUE;
        else HobRomImage->FvInfo[i].IsBootFv = FALSE;
        HobRomImage->NumOfFv = ++i;
    }
    if (HobRomImage->NumOfFv == 0) return EFI_UNSUPPORTED;
    return EFI_SUCCESS;
}
/**
 * This procedure locate the Index of FVInfo from RomImage HOB by 
 * Base Address and Length of Firmware Volume.
 *     
 * @param PeiServices 
 * @param HobRomImage 
 * @param BaseAddress 
 * @param Length 
 * @retval UINT8 Index of FvInfo
**/
UINT8
LocateFvInfoFromHobRomImage (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage,
    IN UINT32                   BaseAddress,
    IN UINT32                   Length
)
{
    UINT8               i;   
    for (i = 0; i < HobRomImage->NumOfFv; i++) {
        if ((BaseAddress == HobRomImage->FvInfo[i].FvAddress) && \
            (Length == HobRomImage->FvInfo[i].FvLength) && \
            (HobRomImage->FvInfo[i].MemAddress != 0) && \
            (HobRomImage->FvInfo[i].FvMemReady)) break;
    }       
    return i;         
}
/**
 * This procedure redirect the FV Base Address of FV HOB to RAM.
 *     
 * @param PeiServices 
 * @param HobRomImage 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
FvInfoSwitchToRam (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN HOB_ROM_IMAGE            *HobRomImage
)
{
    VOID                    *p;
    UINT8                   i = 0, Index = 0;
    EFI_BOOT_MODE           BootMode;
    EFI_STATUS              Status;
    EFI_PEI_FIRMWARE_VOLUME_INFO_PPI      *FvInfoPpi = NULL;
#if (PI_SPECIFICATION_VERSION >= 0x0001000A)
    PEI_CORE_INSTANCE       *PrivateData;
    PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS (PeiServices);
#endif

    // Get current Boot Mode.
    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;

    // Here locate the FV HOB then update its Base Address to be pointed to 
    // in-memory cache.
    for ((*PeiServices)->GetHobList(PeiServices,&p);
            !(FindNextHobByType(EFI_HOB_TYPE_FV,&p)); ) {
        EFI_HOB_FIRMWARE_VOLUME *FvHob = (EFI_HOB_FIRMWARE_VOLUME*)p;
        Index = LocateFvInfoFromHobRomImage(PeiServices, \
                    HobRomImage, (UINT32)FvHob->BaseAddress, (UINT32)FvHob->Length);
        if (Index >= HobRomImage->NumOfFv) continue;                        

#if (PI_SPECIFICATION_VERSION >= 0x0001000A)
            for (i = 0; i < PrivateData->FvCount; ++i) {
                if ((UINT32)FvHob->BaseAddress == (UINT32)PrivateData->Fv[i].FvHandle) {
                    PrivateData->Fv[i].FvHandle = (EFI_PEI_FV_HANDLE)HobRomImage->FvInfo[Index].MemAddress;
                }
            }
#endif
            FvHob->BaseAddress = HobRomImage->FvInfo[Index].MemAddress;
    }

    // Here locate the Firmware Volume PPI and Update its BaseAddress to be 
    // pointed to in-memory cache. 
    i = 0; // Initialize PPI Instance 
    do {
        Status = (*PeiServices)->LocatePpi ( PeiServices, \
                                             &gEfiPeiFirmwareVolumeInfoPpiGuid, \
                                             i++, \
                                             NULL, \
                                             &FvInfoPpi);
        if (!EFI_ERROR(Status)) {
            Index = LocateFvInfoFromHobRomImage (PeiServices, \
                        HobRomImage, (UINT32)FvInfoPpi->FvInfo, FvInfoPpi->FvInfoSize);
            if (Index >= HobRomImage->NumOfFv) continue;
            FvInfoPpi->FvInfo = (VOID*)HobRomImage->FvInfo[Index].MemAddress;    
        }
    } while(!EFI_ERROR(Status));
    
    return EFI_SUCCESS;
}

/**
 * This procedure redirect the FV Base Address of FV HOB to RAM in
 * End of PEI Phase PPI.
 *
 * @param PeiServices 
 * @param NotifyDescriptor 
 * @param NullPpi 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootEndOfPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *NullPpi
)
{
    VOID                    *p;
    INTN                    Result;
    EFI_BOOT_MODE           BootMode;
    EFI_STATUS              Status;
    // Update BaseAddress of FV HOB again if new FV is reported.
    for ((*PeiServices)->GetHobList(PeiServices,&p); \
            !(FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &p));  ) {
        Result = guidcmp(&((EFI_HOB_GUID_TYPE*)p)->Name, &gHobRomImageGuid);
        if (!Result) break;
    }
    if (Result) return EFI_UNSUPPORTED;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    if (BootMode != BOOT_ON_S3_RESUME) {
        CopyFirmwareVolumesToRam(PeiServices, (HOB_ROM_IMAGE*)p, TRUE);
    }
    SwitchPeiServiceDataToRam (PeiServices, (HOB_ROM_IMAGE*)p);
    FvInfoSwitchToRam (PeiServices, (HOB_ROM_IMAGE*)p);
    return EFI_SUCCESS;
}
/**
 * This procedure allocate a Memory buffer and redirect the FV from
 * ROM to RAM in PERMANENT MEMORY INSTALLED PPI;
 *     
 * @param PeiServices 
 * @param NotifyDescriptor 
 * @param NullPpi 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootMemoryReady (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *NullPpi
)
{
    EFI_STATUS                  Status;
    HOB_ROM_IMAGE               *HobRomImage = NULL;
    EFI_BOOT_MODE               BootMode;

    // Get current Boot Mode.
    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    // Check Boot mode again, do nothing if recovery mode.
    if ((BootMode == BOOT_IN_RECOVERY_MODE) || \
            (BootMode == BOOT_ON_FLASH_UPDATE)) return EFI_SUCCESS;

    Status = (*PeiServices)->CreateHob ( PeiServices, \
                                         EFI_HOB_TYPE_GUID_EXTENSION, \
                                         sizeof(HOB_ROM_IMAGE), \
                                         &HobRomImage   );
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    HobRomImage->EfiHobGuidType.Name = gHobRomImageGuid;
    HobRomImage->HobValid = FALSE;
    HobRomImage->NumOfFv = 0;
    Status = CollectRomImageInfo (PeiServices, HobRomImage);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    Status = PrepareForCopyRomToRam (PeiServices, HobRomImage);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    Status = CopyFirmwareVolumesToRam (PeiServices, HobRomImage, FALSE);
    if (EFI_ERROR(Status)) return EFI_UNSUPPORTED;
    SwitchPeiServiceDataToRam (PeiServices, HobRomImage);
    FvInfoSwitchToRam (PeiServices, HobRomImage);
    return EFI_SUCCESS;
}
/**
 * PEI Entry Point for PeiRamBoot Driver.
 *
 * @param FfsHeader 
 * @param PeiServices 
 * @retval EFI_SUCCESS Success
**/
EFI_STATUS
PeiRamBootEntry (
    IN EFI_FFS_FILE_HEADER  *FfsHeader,
    IN EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS          Status;
    EFI_BOOT_MODE       BootMode;

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (EFI_ERROR(Status)) return Status;
    if ((BootMode == BOOT_ON_FLASH_UPDATE) || (BootMode == BOOT_ON_S3_RESUME) || \
        (BootMode == BOOT_IN_RECOVERY_MODE)) return EFI_SUCCESS;
    (*PeiServices)->NotifyPpi (PeiServices, PeiRamBootMemoryReadyNotify);
    (*PeiServices)->NotifyPpi (PeiServices, PeiRamBootEndOfPeiNotify);
    return EFI_SUCCESS;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

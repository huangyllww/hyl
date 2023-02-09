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
/** @file PeiRamBootHook.c
    Hook file for PEI Ram Boot module.

**/
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
//#include <Tiano.h>
//#include <PeiCore.h>
#include <AmiLib.h>
// "CorePeiMain.h" is a Duplicated file from "MdeModulePkg/Core/pei/PeiMain.h" 
// to "Build" Directory when building process..
#include <CorePeiMain.h>    
#include <Token.h>
#include <PeiRamBoot.h>

//----------------------------------------------------------------------------
// Function Externs

//----------------------------------------------------------------------------
// Local prototypes

//----------------------------------------------------------------------------
// Local Variables

//----------------------------------------------------------------------------
// Function Definitions
/**
 * Worker function for debug print.
 *
 * @param PeiServices 
 * @param ErrorLevel 
 * @param Format 
 * @retval VOID
**/
#ifdef EFI_DEBUG
VOID
PeiDebugPrint (
#if ((PI_SPECIFICATION_VERSION >= 0x0001000A) || (CORE_COMBINED_VERSION > 262785))
#if (CORE_COMBINED_VERSION > 262785)
    IN CONST EFI_PEI_SERVICES   **PeiServices,
#else
    IN EFI_PEI_SERVICES   **PeiServices,
#endif
#else
    IN EFI_PEI_SERVICES   **PeiServices,
#endif
    IN UINTN              ErrorLevel,
    IN CHAR8              *Format,
    IN ...
)
{
    CHAR8  Buffer[256];
    va_list ArgList; 
    va_start (ArgList, Format);
    PrepareStatusCodeString( Buffer, sizeof(Buffer), Format, ArgList );
    (*PeiServices)->ReportStatusCode (
        (EFI_PEI_SERVICES**)PeiServices, EFI_DEBUG_CODE,
        EFI_SOFTWARE_UNSPECIFIED, 0, NULL,
        (EFI_STATUS_CODE_DATA *)Buffer
    );
    va_end(ArgList);
}
#endif
#if (PI_SPECIFICATION_VERSION >= 0x0001000A)
/**
 *       
 *   @param PeiServices 
 *   @param FfsFile 
 *   @param Index 
 *   @retval TRUE True
**/
BOOLEAN
IsPeimDispatched (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_FFS_FILE_HEADER          *FfsFile,
    IN UINTN                        Index
)
{
    UINT8                       Count, FvIndex;
    PEI_CORE_INSTANCE           *Private = NULL;
    EFI_FIRMWARE_VOLUME_HEADER  *FvHdr;
    Private = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);

   for (FvIndex = 0; (FvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) && \
               (Private->Fv[FvIndex].FvHeader != NULL); FvIndex++) {
        FvHdr = Private->Fv[FvIndex].FvHeader;                        
        if (((UINT8*)FfsFile >= (UINT8*)FvHdr) && \
            ((UINT8*)FfsFile < ((UINT8*)FvHdr + (FvHdr->FvLength - 1)))) break;
    }
    for( Count = 0; Count < FixedPcdGet32 (PcdPeiCoreMaxPeimPerFv); Count++)
    {
        if ((UINT8*)Private->Fv[FvIndex].FvFileHandles[Count] == (UINT8*)FfsFile)
        {    
            if (Private->Fv[FvIndex].PeimState[Count] == PEIM_STATE_DISPATCHED)
                return TRUE;
        }
        else if( Private->Fv[FvIndex].FvFileHandles[Count] == NULL ) break;
    }
    return FALSE;
}
#else
/**
 * This procedure gets current PEIM dispatched BitMap from
 * DispatchedData of Privatedata of PeiService.
 *
 * @param PeiServices 
 * @retval UINT64 BitMap
**/
UINT64
GetDispatchedPeimBitMap (
    IN EFI_PEI_SERVICES     **PeiServices
)
{
    PEI_CORE_INSTANCE           *PrivateData = NULL;
    PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);
    return PrivateData->DispatchData.DispatchedPeimBitMap;
}
#endif
/**
 * This procedure redirect the FV Bass Address of Private data of
 * PEI Service from ROM to RAM.
 *     
 * @param PeiServices 
 * @param HobRomImage 
 * @retval VOID
**/
VOID
SwitchPeiServiceDataToRam (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN HOB_ROM_IMAGE        *HobRomImage
)
{
    PEI_CORE_INSTANCE           *PrivateData = NULL;
    EFI_PHYSICAL_ADDRESS        FvAddress = 0, PeimAddress = 0, Buffer = 0;
    UINTN                       i = 0, Index;

    PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS(PeiServices);
#if defined(PI_SPECIFICATION_VERSION) && (PI_SPECIFICATION_VERSION >= 0x0001000A)
    for (i = 0; i < HobRomImage->NumOfFv; i++) {
        if ( !HobRomImage->FvInfo[i].FvMemReady ) continue;
        for( Index = 0; Index < PrivateData->FvCount; Index++ ) {
            UINTN               j;
            FvAddress = (EFI_PHYSICAL_ADDRESS)PrivateData->Fv[Index].FvHeader;
            if ((UINT32)FvAddress != HobRomImage->FvInfo[i].FvAddress) continue;
            Buffer = HobRomImage->FvInfo[i].MemAddress;
            PrivateData->Fv[Index].FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)Buffer;
            for (j = 0; (j < FixedPcdGet32 (PcdPeiCoreMaxPeimPerFv)) && \
                        (PrivateData->Fv[Index].FvFileHandles[j] != NULL); j++) {

                PrivateData->Fv[Index].FvFileHandles[j] = \
                    (EFI_PEI_FILE_HANDLE)(((UINTN)PrivateData->Fv[Index].FvFileHandles[j] - \
                                           (UINTN)FvAddress) + Buffer);
                if (Index == PrivateData->CurrentPeimFvCount) {
                    PrivateData->CurrentFvFileHandles[j] = \
                    (EFI_PEI_FILE_HANDLE)(((UINTN)PrivateData->CurrentFvFileHandles[j] - \
                                           (UINTN)FvAddress) + Buffer);
                }
            }
        }
    }
#else // // PI 0.9 & 1.0
    // Update PrivateData.DispatchData of PeiService.
    FvAddress = (EFI_PHYSICAL_ADDRESS)PrivateData->DispatchData.BootFvAddress;
    for (i = 0, Index = 0; i < HobRomImage->NumOfFv; i++) {
        if ((UINT32)FvAddress != HobRomImage->FvInfo[i].FvAddress) continue;
        Buffer = HobRomImage->FvInfo[i].MemAddress;
        PrivateData->DispatchData.BootFvAddress = \
                (EFI_FIRMWARE_VOLUME_HEADER*)(Buffer);
        PrivateData->DispatchData.CurrentFvAddress = \
                (EFI_FIRMWARE_VOLUME_HEADER*)(Buffer);
        PeimAddress = (EFI_PHYSICAL_ADDRESS)PrivateData->DispatchData.CurrentPeimAddress;
        PrivateData->DispatchData.CurrentPeimAddress = \
                (EFI_FFS_FILE_HEADER*)(Buffer + (PeimAddress - FvAddress));
        break;
    }
#endif  // PI 0.9 & 1.0        
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2005, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************


/** @file SecMain.c
    SEC phase

**/

#include    "SecMain.h"
#include    <Library/IoLib.h>
#include    <Token.h>
#include    <Library/ReportStatusCodeLib.h> //<hujf1001-20180906 Oem Debug module support on AMD/Higon project>


EFI_PEI_TEMPORARY_RAM_SUPPORT_PPI gSecTemporaryRamSupportPpi = {
  TemporaryRamMigration
};

EFI_PEI_PPI_DESCRIPTOR            mPeiSecPlatformInformationPpi[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiTemporaryRamSupportPpiGuid,
    &gSecTemporaryRamSupportPpi
  }
};

VOID
EFIAPI
FindAndReportEntryPoints (
  IN  EFI_FIRMWARE_VOLUME_HEADER       *BootFirmwareVolumePtr,
  OUT EFI_PEI_CORE_ENTRY_POINT         *PeiCoreEntryPoint
  );


void EnableMsrMmioCfg(void);


VOID
EFIAPI
SecCEntry (
  IN UINT32     SizeTmpRam,
  IN UINT32     TmpRamBase,
  IN VOID       *BFV
  )
{
    EFI_SEC_PEI_HAND_OFF        SecCoreData;

	InitializeFloatingPointUnits();
    
    SecCoreData.DataSize               = (UINT16) sizeof (EFI_SEC_PEI_HAND_OFF);
    SecCoreData.BootFirmwareVolumeBase = BFV;
    SecCoreData.BootFirmwareVolumeSize = (UINTN)FV_BB_SIZE;
    SecCoreData.TemporaryRamBase       = (VOID*)TmpRamBase; 
    SecCoreData.TemporaryRamSize       = SizeTmpRam;
    SecCoreData.PeiTemporaryRamBase    = (VOID*)TmpRamBase;
    SecCoreData.PeiTemporaryRamSize    = SizeTmpRam/2;
    SecCoreData.StackBase              = (VOID*)(TmpRamBase + SizeTmpRam/2);
    SecCoreData.StackSize              = SizeTmpRam/2;

    //
    // Initialize Debug Agent to support source level debug in SEC/PEI phases before memory ready.
    //
    
#if !FV_BB_IN_ROM == 1
    InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, &SecCoreData, (DEBUG_AGENT_CONTINUE)(((UINT64)SecCPhase2 & (FV_BB_SIZE - 1)) + HIGH_MEMORY_REGION_BB_BASE)); 
#else
    InitializeDebugAgent (DEBUG_AGENT_INIT_PREMEM_SEC, &SecCoreData, (DEBUG_AGENT_CONTINUE)((UINT64)SecCPhase2)); 
#endif

}

VOID
EFIAPI
SecCPhase2 (
  IN    VOID    *Context
)
{

    EFI_PEI_CORE_ENTRY_POINT    PeiCoreEntryPoint;
    EFI_SEC_PEI_HAND_OFF        *SecCoreData;
    EFI_PEI_PPI_DESCRIPTOR      *PpiList;
    
    
    SecCoreData = (EFI_SEC_PEI_HAND_OFF *) Context;
    //
    // Find Pei Core entry point. It will report SEC and Pei Core debug information if remote debug
    // is enabled.
    //
    FindAndReportEntryPoints ((EFI_FIRMWARE_VOLUME_HEADER *) SecCoreData->BootFirmwareVolumeBase, &PeiCoreEntryPoint);
    ASSERT (PeiCoreEntryPoint != NULL);
    if (PeiCoreEntryPoint == NULL)
    {
      CpuDeadLoop ();
    }
	
	  EnableMsrMmioCfg();

    //SGMTB#0144953:Set PEI/DXE Phase Error/Info/Warn Debug Level In Default+>
    #if 0
    //<hujf1001-20180906 Oem Debug module support on AMD/Higon project +>
    REPORT_STATUS_CODE (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_SEC | EFI_SW_SEC_PC_HANDOFF_TO_NEXT
      );
    //<hujf1001-20180906 Oem Debug module support on AMD/Higon project ->
    #endif
    //SGMTB#0144953:Set PEI/DXE Phase Error/Info/Warn Debug Level In Default->

    PpiList = &mPeiSecPlatformInformationPpi[0];

    // Call PEI Foundation entry point
    (*PeiCoreEntryPoint) (SecCoreData, PpiList);
    
    // Should not return here
}


/**
  Find core image base.
  
  @param   BootFirmwareVolumePtr    Point to the boot firmware volume.
  @param   SecCoreImageBase         The base address of the SEC core image.
  @param   PeiCoreImageBase         The base address of the PEI core image.

**/
EFI_STATUS
EFIAPI
FindImageBase (
  IN  EFI_FIRMWARE_VOLUME_HEADER       *BootFirmwareVolumePtr,
  OUT EFI_PHYSICAL_ADDRESS             *SecCoreImageBase,
  OUT EFI_PHYSICAL_ADDRESS             *PeiCoreImageBase
  )
{
  EFI_PHYSICAL_ADDRESS        CurrentAddress;
  EFI_PHYSICAL_ADDRESS        EndOfFirmwareVolume;
  EFI_FFS_FILE_HEADER         *File;
  UINT32                      Size;
  EFI_PHYSICAL_ADDRESS        EndOfFile;
  EFI_COMMON_SECTION_HEADER   *Section;
  EFI_PHYSICAL_ADDRESS        EndOfSection;

  *SecCoreImageBase = 0;
  *PeiCoreImageBase = 0;

  CurrentAddress = (EFI_PHYSICAL_ADDRESS)(UINTN) BootFirmwareVolumePtr;
  EndOfFirmwareVolume = CurrentAddress + BootFirmwareVolumePtr->FvLength;

  //
  // Loop through the FFS files in the Boot Firmware Volume
  //
  for (EndOfFile = CurrentAddress + BootFirmwareVolumePtr->HeaderLength; ; ) {

    CurrentAddress = (EndOfFile + 7) & 0xfffffffffffffff8ULL;
    if (CurrentAddress > EndOfFirmwareVolume) {
      return EFI_NOT_FOUND;
    }

    File = (EFI_FFS_FILE_HEADER*)(UINTN) CurrentAddress;
    if (IS_FFS_FILE2 (File)) {
      Size = FFS_FILE2_SIZE (File);
      if (Size <= 0x00FFFFFF) {
        return EFI_NOT_FOUND;
      }
    } else {
      Size = FFS_FILE_SIZE (File);
      if (Size < sizeof (EFI_FFS_FILE_HEADER)) {
        return EFI_NOT_FOUND;
      }
    }

    EndOfFile = CurrentAddress + Size;
    if (EndOfFile > EndOfFirmwareVolume) {
      return EFI_NOT_FOUND;
    }

    //
    // Look for SEC Core / PEI Core files
    //
    if (File->Type != EFI_FV_FILETYPE_SECURITY_CORE &&
        File->Type != EFI_FV_FILETYPE_PEI_CORE) {
      continue;
    }

    //
    // Loop through the FFS file sections within the FFS file
    //
    if (IS_FFS_FILE2 (File)) {
      EndOfSection = (EFI_PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) File + sizeof (EFI_FFS_FILE_HEADER2));
    } else {
      EndOfSection = (EFI_PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) File + sizeof (EFI_FFS_FILE_HEADER));
    }
    for (;;) {
      CurrentAddress = (EndOfSection + 3) & 0xfffffffffffffffcULL;
      Section = (EFI_COMMON_SECTION_HEADER*)(UINTN) CurrentAddress;

      if (IS_SECTION2 (Section)) {
        Size = SECTION2_SIZE (Section);
        if (Size <= 0x00FFFFFF) {
          return EFI_NOT_FOUND;
        }
      } else {
        Size = SECTION_SIZE (Section);
        if (Size < sizeof (EFI_COMMON_SECTION_HEADER)) {
          return EFI_NOT_FOUND;
        }
      }

      EndOfSection = CurrentAddress + Size;
      if (EndOfSection > EndOfFile) {
        return EFI_NOT_FOUND;
      }

      //
      // Look for executable sections
      //
      if (Section->Type == EFI_SECTION_PE32 || Section->Type == EFI_SECTION_TE) {
        if (File->Type == EFI_FV_FILETYPE_SECURITY_CORE) {
          if (IS_SECTION2 (Section)) {
            *SecCoreImageBase = (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER2));
          } else {
            *SecCoreImageBase = (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER));
          }
        } else {
          if (IS_SECTION2 (Section)) {
            *PeiCoreImageBase = (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER2));
          } else {
            *PeiCoreImageBase = (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER));
          }
        }
        break;
      }
    }

    //
    // Both SEC Core and PEI Core images found
    //
    if (*SecCoreImageBase != 0 && *PeiCoreImageBase != 0) {
      return EFI_SUCCESS;
    }
  }
}

/**
    Returns PEI Core Entry point. Used for debug.

        
    @param BootFirmwareVolumePtr 
    @param PeiCoreEntryPoint 

         
    @retval VOID

**/

VOID
EFIAPI
FindAndReportEntryPoints (
  IN  EFI_FIRMWARE_VOLUME_HEADER    *BootFirmwareVolumePtr,
  OUT EFI_PEI_CORE_ENTRY_POINT      *PeiCoreEntryPoint
  )
{
    EFI_STATUS                       Status;
    EFI_PHYSICAL_ADDRESS             SecCoreImageBase;
    EFI_PHYSICAL_ADDRESS             PeiCoreImageBase;
    PE_COFF_LOADER_IMAGE_CONTEXT     ImageContext;

    //
    // Find SEC Core and PEI Core image base
    //
    Status = FindImageBase (BootFirmwareVolumePtr, &SecCoreImageBase, &PeiCoreImageBase);
    ASSERT_EFI_ERROR (Status);

    ZeroMem ((VOID *) &ImageContext, sizeof (PE_COFF_LOADER_IMAGE_CONTEXT));
    //
    // Report SEC Core debug information when remote debug is enabled
    //
    ImageContext.ImageAddress = SecCoreImageBase;
    ImageContext.PdbPointer = PeCoffLoaderGetPdbPointer ((VOID*) (UINTN) ImageContext.ImageAddress);
    PeCoffLoaderRelocateImageExtraAction (&ImageContext);

    //
    // Report PEI Core debug information when remote debug is enabled
    //
    ImageContext.ImageAddress = PeiCoreImageBase;
    ImageContext.PdbPointer = PeCoffLoaderGetPdbPointer ((VOID*) (UINTN) ImageContext.ImageAddress);
    PeCoffLoaderRelocateImageExtraAction (&ImageContext);

    //
    // Find PEI Core entry point
    //
    Status = PeCoffLoaderGetEntryPoint ((VOID *) (UINTN) PeiCoreImageBase, (VOID**) PeiCoreEntryPoint);
    if (EFI_ERROR (Status)) {
      *PeiCoreEntryPoint = 0;
    }

    return;

}

/**
    Move memory content from Temporary Memory to Permanent Memory.

        
    @param PeiServices 
    @param TemporaryMemoryBase 
    @param PermanentMemoryBase 
    @param CopySize 

         
    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS TemporaryRamMigration(
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
)
{
    IA32_DESCRIPTOR   IdtDescriptor;
    VOID*             OldHeap;
    VOID*             NewHeap;
    VOID*             OldStack;
    VOID*             NewStack;
    DEBUG_AGENT_CONTEXT_POSTMEM_SEC  DebugAgentContext;
//--    UINTN             Offset;

    if (TemporaryMemoryBase > PermanentMemoryBase) {
        if ((PermanentMemoryBase + CopySize) > TemporaryMemoryBase)
            return EFI_INVALID_PARAMETER;
    }

//      MemCpy((VOID*) PermanentMemoryBase, (VOID*) TemporaryMemoryBase, CopySize); //remove original

 //  Temporary Memory
 // |-------------------|
 // |                   |
 // |                   |
 // |                   |    PeiStackSize
 // |     Stack         |
 // |                   |
 // |                   |
 // |-------------------|----> StackBase (TempRamBase + CopySize/2)
 // |                   |
 // |                   |       PeiTemporayRamSize
 // |      Heap         |
 // |                   |
 // |                   |
 // |-------------------|---->  TempRamBase




 //  Permanent Memory
 // |-------------------|---->  (PermanentMemoryBase + CopySize)
 // |                   |
 // |                   |
 // |                   |
 // |     Heap          |
 // |                   |
 // |                   |
 // |-------------------|----> NewHeap (PermanentMemoryBase + CopySize/2)
 // |                   |
 // |                   |
 // |       Stack       |
 // |                   |
 // |                   |
 // |                   |
 // |-------------------|---->  PermanentMemoryBase (NewStack)

    OldHeap = (VOID*)(UINTN)TemporaryMemoryBase;
    NewHeap = (VOID*)((UINTN)PermanentMemoryBase + CopySize/2 );

    OldStack = (VOID*)((UINTN)TemporaryMemoryBase + CopySize/2 );
    NewStack = (VOID*)((UINTN)PermanentMemoryBase );

    DebugAgentContext.HeapMigrateOffset = (UINTN)NewHeap - (UINTN)OldHeap;
    DebugAgentContext.StackMigrateOffset = (UINTN)NewStack - (UINTN)OldStack;     

    InitializeDebugAgent (DEBUG_AGENT_INIT_POSTMEM_SEC, (VOID *) &DebugAgentContext, NULL);
    
    //
    // Migrate Heap
    //
    CopyMem(NewHeap, OldHeap, CopySize/2);

    //
    // Migrate Stack
    //
    CopyMem(NewStack, OldStack, CopySize/2);

//##<AAV>##  //
//##<AAV>##  // Rebase IDT table in permanent memory
//##<AAV>##  //
//##<AAV>##  if (TemporaryMemoryBase < PermanentMemoryBase)
//##<AAV>##      Offset = (UINTN)(PermanentMemoryBase - TemporaryMemoryBase - CopySize/2);
//##<AAV>##  else
//##<AAV>##      Offset = (UINTN)(TemporaryMemoryBase - PermanentMemoryBase + CopySize/2);
//##<AAV>##  MigrateIdtTable(PeiServices, Offset, (TemporaryMemoryBase < PermanentMemoryBase));

    //                                                                          
    // Rebase IDT table in permanent memory                                     
    //                                                                            //
    AsmReadIdtr (&IdtDescriptor);                                                 // SecSwitchStack function must be invoked after the memory migration
    IdtDescriptor.Base = IdtDescriptor.Base - (UINTN)OldStack + (UINTN)NewStack;  // immediately, also we need fixup the stack change caused by new call into
                                                                                // permanent memory.
    AsmWriteIdtr (&IdtDescriptor);                                                //

    SecSwitchStack (
    (UINT32) (UINTN) OldStack,
    (UINT32) ((UINTN) NewStack)
    );

    return EFI_SUCCESS;
}

void EnableMsrMmioCfg(void)
{
#ifdef PCIEX_BASE_ADDRESS
#if    PCIEX_BASE_ADDRESS
    UINT64  MMIOCfgValue;
    // Enable MMIO Configuration space by setting MSRC001_0058[Enable] = 1
    MMIOCfgValue = PCIEX_BASE_ADDRESS & (UINT64)(0xfffffff00000); //Bit 47 to 20
    MMIOCfgValue = MMIOCfgValue | 1;

    // Specify number of buses in the MMIO configuration space range
    if(PCIEX_LENGTH==0x10000000)
        MMIOCfgValue = MMIOCfgValue | 0x20;     //256MB
    else if(PCIEX_LENGTH == 0x8000000)
        MMIOCfgValue = MMIOCfgValue | 0x1c;     //128MB
    else if(PCIEX_LENGTH == 0x4000000)
        MMIOCfgValue = MMIOCfgValue | 0x18;     //64MB
    else if(PCIEX_LENGTH == 0x2000000)
        MMIOCfgValue = MMIOCfgValue | 0x14;     //32MB
    WriteMsr(HYGON_MSR_MMIO_CFG_BASE, MMIOCfgValue);
#endif
#endif
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

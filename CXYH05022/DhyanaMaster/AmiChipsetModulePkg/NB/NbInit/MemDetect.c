
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file MemDetect.c
    This file forms like a wrapper for actual memory detection code.
    The project will have two portion of Memory detection related
    code - one in North bridge that provides NB specific interface 
    (PPI/functions) for the memory detection code and the another
    component which is the actual memory detection code. This file
    belong to the second component.  
    This component might be provided in binary format (FFS) by
    some chipset vendor.  In that case, modify the NB.MAK, 
    , MemDetect.SDL and MemDetect.CIF files to remove the files
    MemDetect.c & MemDetect.h and include the provided binary
    in FV_BB. Also make sure the NB side component publishes 
    required PPI for the binary provided by the chipset vendor.
    If this component is created from source (assembly or C) files
    then add those files into MemDetect.CIF. Modify this file 
    to get the data published by NB side component and convert
    into the way the actual memory detection code needs (like a
    wrapper).
    It is responsibility of this component to setup proper
    HOBs related to memory

**/

#include "Efi.h"
#include "Pei.h"
#include "token.h"
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <AmiCspLibInc.h>
#include "Nb.h"
#include <Library/HobLib.h>
#ifdef ACPI_SUPPORT
#include "AmiAcpiS3.h"
#include <Guid/AcpiS3Context.h>
#endif

// Produced PPIs
#include "ppi/CspLibPpi.h"
#include <CpuSetupData.h>
#include "ppi/ReadOnlyVariable2.h"
#include "Memdetect.h"
#include "Guid/AmdMemoryInfoHob.h"
#include <Ppi/Capsule.h>
#include <CPU.h>

// this is copied from amdprocessorinitpeim.c
#define MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR    19
#define MAX_SIZEOF_HYGON_MEMORY_INFO_HOB_BUFFER       (sizeof (AMD_MEMORY_INFO_HOB) +  \
          (MAX_NUMBER_OF_EXTENDED_MEMORY_DESCRIPTOR * sizeof (AMD_MEMORY_RANGE_DESCRIPTOR)))

#ifndef TSEG_SIZE
#define	TSEG_SIZE 			0
#endif

#define	MEM_DET_COMMON_MEM_ATTR     (EFI_RESOURCE_ATTRIBUTE_PRESENT | \
                                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                    EFI_RESOURCE_ATTRIBUTE_TESTED | \
                                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |\
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)

#define	MEM_DET_COMMON_MEM_ATTR_ABOVE4G     (EFI_RESOURCE_ATTRIBUTE_PRESENT | \
                                    EFI_RESOURCE_ATTRIBUTE_INITIALIZED | \
                                    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |\
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE | \
                                    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE)

typedef struct
{
  UINT32    Base;
  UINT32    Size;
} NVDIMM_INFO;

// Function prototypes
EFI_STATUS  InstallEfiMemory (EFI_PEI_SERVICES **PeiServices, 
                                UINT32 dTOM, UINT64 UMABase, UINT64 UMASize );
EFI_STATUS  InstallS3Memory (EFI_PEI_SERVICES **PeiServices,
                             EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable);

//
// <AAV>
// To avoid more compiler problem we copy the function prototype from "BaseMemoryLib.h" of MdePkg.
//
BOOLEAN
EFIAPI
CompareGuid (
  IN CONST GUID  *Guid1,
  IN CONST GUID  *Guid2
  );

BOOLEAN SbCheckAfterG3 (
    VOID
);
/**
    Find instance of a HOB type in a HOB list
               
    @param Type The HOB type to return.
    @param HobStart The first HOB in the HOB list.
         
    @retval VOID HobStart      Returns the first HOB with the matching type.
**/
VOID * GetHob (  IN UINT16  Type,  IN VOID    *HobStart  )
{
  EFI_PEI_HOB_POINTERS  Hob;

  Hob.Raw = HobStart;
  //
  // Return input if not found
  //
  if (HobStart == NULL) {
    return HobStart;
  }

  //
  // Parse the HOB list, stop if end of list or matching type found.
  //
  while (!END_OF_HOB_LIST (Hob)) {

    if (Hob.Header->HobType == Type) {
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  
  //
  // Return input if not found
  //
  if (END_OF_HOB_LIST (Hob)) {
    return HobStart;
  }

  return (VOID *) (Hob.Raw);
}

/**
    Read UINT32 value from PCI register

        IN UINT32 - Address :(UINT32) ( (((UINT32)bus) << 24) + \
        (((UINT32)dev) << 19) + \
        (((UINT32)func) << 16) + \
        ((UINT32)reg) )   
    @param ExtendReg : external access

    @retval UINT32 value from PCI register.

**/

UINT32 NbPeiReadPCI32(
    IN UINT32 Address,
    IN UINT8  ExtendReg
)
{
    UINT32 Value32;
    UINT64 Value64;
    UINT32 AddrCf8 = ((UINT32)1 << 31) + ((Address >> 8) & 0x0FFFF00) + (Address & 0xFC);

    Value64 = ReadMsr(0xC001001F);
    Value64 |= 0x400000000000;
    WriteMsr(0xC001001F, Value64);

    if(ExtendReg)
        AddrCf8 |= (1 << 24);

    IoWrite32(0xCf8, AddrCf8);
    Value32 = IoRead32(0xCfC);

    Value64 = ReadMsr(0xC001001F);
    Value64 &= 0xFFFFBFFFFFFFFFFF;
    WriteMsr(0xC001001F, Value64);

    return Value32;
}

/**
    Create MTRR Hob. Will not save Var MTRR 20E and 20F.

        
    @param PeiServices 

         
    @retval VOID

**/

VOID
CreateMTRRHob(IN EFI_PEI_SERVICES **PeiServices)
{
	MTRR_HOB	*MtrrHob;
    UINT32      VARMTRRMsr;
    UINT8       Index;
	EFI_STATUS	Status;

	// Create hob for storing MTRR Data
	Status = (**PeiServices).CreateHob(PeiServices,
		EFI_HOB_TYPE_GUID_EXTENSION,
		sizeof(MTRR_HOB),
  		&MtrrHob);
	ASSERT_PEI_ERROR(PeiServices, Status);

	MtrrHob->EfiHobGuidType.Name = gAmiMTRRHobGuid;

    for (VARMTRRMsr = MTRR_PHYS_BASE_0, Index = 0; VARMTRRMsr < MTRR_PHYS_BASE_7; VARMTRRMsr += 2, Index++)
    {

   	    MtrrHob->VarMTRRBase[Index] = ReadMsr(VARMTRRMsr);
   	    MtrrHob->VarMTRRMask[Index] = ReadMsr(VARMTRRMsr + 1);

        //Clear the 0x200~0x20D MTRR registers, leave ROM cache MTRR 0x20E-0x20F
        WriteMsr(VARMTRRMsr + 1, 0);      // Always clear mask first then Base
        WriteMsr(VARMTRRMsr, 0);
    }
}

/**
    This function saves TOM for =< 4GB region

        
    @param PeiServices Pointer to the PEI services table
    @param dTOM 32-bit address of TOM 

         
    @retval EFI_STATUS Return Status 

**/

EFI_STATUS
SaveDTom(
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  UINT32            dTOM )
{
    EFI_PEI_PPI_DESCRIPTOR *mPpiSaveDTOM;
    EFI_PEI_SAVE_DTOM_PPI  *mSaveDTOMPpi;
    EFI_STATUS              Status;

    // allocate memory for EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_PPI_DESCRIPTOR), &mPpiSaveDTOM);
    ASSERT_PEI_ERROR( PeiServices, Status );
    
    // allocate memory for EFI_PEI_SAVE_DTOM_PPI mSaveDTOMPpi
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_SAVE_DTOM_PPI), &mSaveDTOMPpi);
    ASSERT_PEI_ERROR( PeiServices, Status );
    
    // Save the value of dTOM in mPpiSaveDTOM to be used later in CpuPeiEntry
    mSaveDTOMPpi->Value = dTOM;
    
    // Initialize the EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM
    mPpiSaveDTOM->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mPpiSaveDTOM->Guid = &gAmiDTomGuid;
    mPpiSaveDTOM->Ppi = mSaveDTOMPpi;
    
    // Install the EFI_PEI_SAVE_DTOM_PPI to retrieve dTOM value later in CpuPeiEntry
    Status = (*PeiServices)->InstallPpi(PeiServices, mPpiSaveDTOM);
    ASSERT_PEI_ERROR( PeiServices, Status );
    return Status;
}

EFI_STATUS
SaveDTom2 (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  UINT64            Base,
  IN  UINT64            Size )
{
    EFI_PEI_PPI_DESCRIPTOR *mPpiSaveDTOM2;
    EFI_PEI_SAVE_DTOM2_PPI  *mSaveDTOM2Ppi;
    EFI_STATUS Status;

    // allocate memory for EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM2
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_PPI_DESCRIPTOR), &mPpiSaveDTOM2);
    ASSERT_PEI_ERROR( PeiServices, Status );

    // allocate memory for EFI_PEI_SAVE_DTOM2_PPI mSaveDTOM2Ppi
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_SAVE_DTOM2_PPI), &mSaveDTOM2Ppi);
    ASSERT_PEI_ERROR( PeiServices, Status );

    // Save the value of dTOM2 in mPpiSaveDTOM2 to be used later in CpuPeiEntry
    mSaveDTOM2Ppi->Base = Base;
    mSaveDTOM2Ppi->Size = Size;

    // Initialize the EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM2
    mPpiSaveDTOM2->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mPpiSaveDTOM2->Guid = &gAmiDTom2Guid;
    mPpiSaveDTOM2->Ppi = mSaveDTOM2Ppi;

    // Install the EFI_PEI_SAVE_DTOM2_PPI to retrieve dTOM2 value later in CpuPeiEntry
    Status = (*PeiServices)->InstallPpi(PeiServices, mPpiSaveDTOM2);
    ASSERT_PEI_ERROR( PeiServices, Status );
    
    return Status;
}

EFI_STATUS
SaveDTom3 (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  UINT64            Base,
  IN  UINT64            Size )
{
    EFI_PEI_PPI_DESCRIPTOR *mPpiSaveDTOM3;
    EFI_PEI_SAVE_DTOM2_PPI  *mSaveDTOM3Ppi;
    EFI_STATUS Status;

    // allocate memory for EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM3
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_PPI_DESCRIPTOR), &mPpiSaveDTOM3);
    ASSERT_PEI_ERROR( PeiServices, Status );

    // allocate memory for EFI_PEI_SAVE_DTOM2_PPI mSaveDTOM3Ppi
    Status = (*PeiServices)->AllocatePool(PeiServices, sizeof(EFI_PEI_SAVE_DTOM2_PPI), &mSaveDTOM3Ppi);
    ASSERT_PEI_ERROR( PeiServices, Status );

    // Save the value of dTOM3 in mPpiSaveDTOM3 to be used later in CpuPeiEntry
    mSaveDTOM3Ppi->Base = Base;
    mSaveDTOM3Ppi->Size = Size;

    // Initialize the EFI_PEI_PPI_DESCRIPTOR mPpiSaveDTOM3
    mPpiSaveDTOM3->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mPpiSaveDTOM3->Guid = &gAmiDTom3Guid;
    mPpiSaveDTOM3->Ppi = mSaveDTOM3Ppi;

    // Install the EFI_PEI_SAVE_DTOM2_PPI to retrieve dTOM3 value later in CpuPeiEntry
    Status = (*PeiServices)->InstallPpi(PeiServices, mPpiSaveDTOM3);
    ASSERT_PEI_ERROR( PeiServices, Status );
    
    return Status;
}

/**
    This function is the entry point for this PEI. This function
    auto detects the memory installed and configures it

    @param FileHandle Handle of the file being invoked
    @param PeiServices Pointer to the PEI services table

    @retval EFI_STATUS Return Status based on errors that occurred while waiting for
        time to expire.

**/

EFI_STATUS
EFIAPI
MemDetectSupportInit (
    IN EFI_PEI_FILE_HANDLE      FileHandle,
    IN  EFI_PEI_SERVICES            **PeiServices
)
{
    UINT32                          dTOM = 0;
    UINT64                          TOM2 = 0;
    
    EFI_STATUS                      Status;
    UINT64                          UMASize = 0;
    UINT64                          UMABase = 0;
    EFI_BOOT_MODE                   BootMode;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINT8                           *MemInfoHobBuffer;
    AMD_MEMORY_INFO_HOB             *MemInfoHob;
    AMD_MEMORY_RANGE_DESCRIPTOR     *MemRangDesc;
    VOID                            *HobStart;
    VOID                            *ResultHobStart;
    EFI_PEI_HOB_POINTERS            GuidHob;
    AMI_MEM_INFO                    AmimemInfo;
    UINTN                           VarSize = sizeof(UINT64);
    UINTN                           VarAttrib = 0;
    UINT32                          MemRangeIndex;
    EFI_PEI_SAVE_DTOM2_PPI          SaveDTOM2Ppi = {0};
    VOID                            *HobAddr;
    NVDIMM_INFO                     *NvdimmInfo;
    UINT64                          NvdimmBase;
    UINT64                          NvdimmSize;
    UINT64                          TempSize;
    UINT64                          Temp1Size = 0, RegionTop64 = 0;


    Status = (*PeiServices)->LocatePpi(
                 PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL,
                 &ReadOnlyVariable );
    ASSERT_PEI_ERROR(PeiServices, Status);

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    ASSERT_PEI_ERROR (PeiServices, Status);


    if (BootMode == BOOT_ON_S3_RESUME)
    {
        //Read dTOM Value from NVRAM and report it.
        VarSize = sizeof(AMI_MEM_INFO);
        Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,
                        L"AMIMemInfo",
                        &gAmiMemInfoGuid,
                        &VarAttrib,
                        &VarSize,
                        &AmimemInfo);   //This info is a must for S3resume.
        if(EFI_ERROR(Status))
        {
            PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "S3 memory failure....Cannot restore system" ));
            PEI_ERROR_CODE(PeiServices, PEI_MEMORY_S3_RESUME_FAILED, EFI_ERROR_MAJOR);
            ASSERT_PEI_ERROR( PeiServices, Status );
            EFI_DEADLOOP();
        }
        AmimemInfo.dLoMemSize = AmimemInfo.dLoMemSize & 0xffff0000;
        Status = SaveDTom(PeiServices, AmimemInfo.dLoMemSize);
        Status = InstallS3Memory(PeiServices, ReadOnlyVariable);
        return Status;
    }

    Status = (*PeiServices)->GetHobList (PeiServices, &HobStart);
    ASSERT_PEI_ERROR( PeiServices, Status );

    Status = (*PeiServices)->AllocatePool(PeiServices, 
                    MAX_SIZEOF_HYGON_MEMORY_INFO_HOB_BUFFER, &MemInfoHobBuffer);
    ASSERT_PEI_ERROR( PeiServices, Status );

    ResultHobStart = GetNextGuidHob(&gAmdMemoryInfoHobGuid, HobStart);
    // If we don't find this HOB, which is essential, 
    // we report PEI_MEMORY_NOT_DETECTED
    if(ResultHobStart == NULL)
    {
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NOT_DETECTED, EFI_ERROR_MAJOR);
        ASSERT_PEI_ERROR( PeiServices, Status );
    }
    GuidHob.Guid = (EFI_HOB_GUID_TYPE *)ResultHobStart;
    MemInfoHobBuffer = (UINT8 *)(&GuidHob.Guid->Name) +  sizeof (EFI_GUID);
   
    //Get dTOM from AMD HOB
    MemInfoHob = (AMD_MEMORY_INFO_HOB *) &MemInfoHobBuffer[0];
    NB_TRACE((TRACE_NB, "CreateHobResourceDescriptor from HYGON_MEMORY_INFO_HOB\n"));
    if(MemInfoHob != NULL)
    {
        NB_TRACE((TRACE_NB, "Number of Descriptors: %d\n", MemInfoHob->NumberOfDescriptor ));
        
        MemRangDesc = &MemInfoHob->Ranges[0];
        for(MemRangeIndex = 0; MemRangeIndex < MemInfoHob->NumberOfDescriptor; MemRangeIndex++)
        {
            if((MemRangDesc[MemRangeIndex].Attribute == AMD_MEMORY_ATTRIBUTE_AVAILABLE) && (MemRangDesc[MemRangeIndex].Base < 0x100000000))
                dTOM += (UINT32) MemRangDesc[MemRangeIndex].Size;
            if((MemRangDesc[MemRangeIndex].Base >= 0x100000000 && (MemRangDesc[MemRangeIndex].Attribute == AMD_MEMORY_ATTRIBUTE_AVAILABLE)))
            {
                TOM2 += MemRangDesc[MemRangeIndex].Size;
//                Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY, MEM_DET_COMMON_MEM_ATTR_ABOVE4G,
//                                                    MemRangDesc[MemRangeIndex].Base, MemRangDesc[MemRangeIndex].Size);
//                NB_TRACE((TRACE_NB, "MemDescriptor[%2d] -- Base: %16lx -- Length: %16lx -- Attribute: %x HobStatus: %r\n", 
//                                                    MemRangeIndex, MemRangDesc[MemRangeIndex].Base, MemRangDesc[MemRangeIndex].Size, MemRangDesc[MemRangeIndex].Attribute, Status));

                SaveDTOM2Ppi.Base = MemRangDesc[MemRangeIndex].Base;
                SaveDTOM2Ppi.Size = MemRangDesc[MemRangeIndex].Size;
                RegionTop64 = SaveDTOM2Ppi.Base + SaveDTOM2Ppi.Size;
                HobAddr = GetFirstGuidHob (&gAmdNvdimmInfoHobGuid);
                if (HobAddr != NULL) {
                  NvdimmInfo = (NVDIMM_INFO *) GET_GUID_HOB_DATA (HobAddr);
                  while (NvdimmInfo->Size != 0) {
                    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "NVDIMM range from APOB: NvdimmBase = %08x, NvdimmSize = %08x\n", NvdimmInfo->Base, NvdimmInfo->Size));
                    NvdimmBase = (UINT64) NvdimmInfo->Base; 
                    NvdimmSize = (UINT64) NvdimmInfo->Size;
                    NvdimmBase = Shl64 (NvdimmBase, 8);
                    NvdimmSize = Shl64 (NvdimmSize, 8);
                    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "NvdimmBase = %08x-%08x, NvdimmSize = %08x-%08x\n", 
                               (UINT32) (Shr64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff), (UINT32) (Shr64(NvdimmSize, 32)), (UINT32) (NvdimmSize & 0xffffffff)));

                    if (NvdimmBase == SaveDTOM2Ppi.Base) {
                      Temp1Size = NvdimmSize;
                      if ((NvdimmBase + NvdimmSize) > RegionTop64) {
                        Temp1Size =  RegionTop64 - NvdimmBase;
                      }

                      SaveDTOM2Ppi.Base += Temp1Size;
                      SaveDTOM2Ppi.Size -= Temp1Size;
                      Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                              (EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE),  NvdimmBase, Temp1Size);
                      if (Status != EFI_SUCCESS) {
                          PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "creation failed @\n"));
                      }
                      PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Descriptor base = %08x-%08x, size = %08x-%08x, *** reserved\n",
                                 (UINT32) (Shr64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff),  (UINT32) (Shr64(Temp1Size, 32)), (UINT32) (Temp1Size & 0xffffffff)));

                    } else if ((NvdimmBase > SaveDTOM2Ppi.Base) && (NvdimmBase < RegionTop64)) {
                      TempSize = NvdimmBase - SaveDTOM2Ppi.Base;
                      Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                              MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  SaveDTOM2Ppi.Base, TempSize);
                      if (Status != EFI_SUCCESS) {
                          PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "creation failed @\n"));
                      }
                      PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Descriptor base = %08x-%08x, size = %08x-%08x, *** memory\n",
                                 (UINT32) (Shr64(SaveDTOM2Ppi.Base, 32)), (UINT32) (SaveDTOM2Ppi.Base & 0xffffffff), (UINT32) (Shr64(TempSize, 32)), (UINT32) (TempSize & 0xffffffff)));

                      Temp1Size = NvdimmSize;
                      if ((NvdimmBase + NvdimmSize) > RegionTop64) {
                        Temp1Size =  RegionTop64 - NvdimmBase;
                      }
                      Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                              (EFI_RESOURCE_ATTRIBUTE_PERSISTENT | EFI_RESOURCE_ATTRIBUTE_PERSISTABLE),  NvdimmBase, Temp1Size);
                      if (Status != EFI_SUCCESS) {
                          PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "creation failed @\n"));
                      }
                      PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Descriptor base = %08x-%08x, size = %08x-%08x, *** reserved\n",
                                 (UINT32) (Shr64(NvdimmBase, 32)), (UINT32) (NvdimmBase & 0xffffffff), (UINT32) (Shr64(Temp1Size, 32)), (UINT32) (Temp1Size & 0xffffffff)));
                      SaveDTOM2Ppi.Base += TempSize + Temp1Size;
                      SaveDTOM2Ppi.Size -= (TempSize + Temp1Size);
                    }
                    NvdimmInfo++; 
                  }   // while (NvdimmInfo->Size != 0

                  if (SaveDTOM2Ppi.Size != 0) {   // The last system memory range
                    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                            MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  SaveDTOM2Ppi.Base, SaveDTOM2Ppi.Size);
                    if (Status != EFI_SUCCESS) {
                        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "creation failed @\n"));
                    }
                    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Descriptor base = %08x-%08x, size = %08x-%08x, *** memory\n",
                               (UINT32) (Shr64(SaveDTOM2Ppi.Base, 32)), (UINT32) (SaveDTOM2Ppi.Base & 0xffffffff), (UINT32) (Shr64(SaveDTOM2Ppi.Size, 32)), (UINT32) (SaveDTOM2Ppi.Size & 0xffffffff)));
                  }
                } else {
                  Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                          MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  SaveDTOM2Ppi.Base, SaveDTOM2Ppi.Size);
                  PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Descriptor base = %08x-%08x, size = %08x-%08x, *** memory\n",
                             (UINT32) (Shr64(SaveDTOM2Ppi.Base, 32)), (UINT32) (SaveDTOM2Ppi.Base & 0xffffffff), (UINT32) (Shr64(SaveDTOM2Ppi.Size, 32)) , (UINT32)(SaveDTOM2Ppi.Size & 0xffffffff)));
                }
                
            }
            if((MemRangDesc[MemRangeIndex].Base >= 0x100000000) && (MemRangDesc[MemRangeIndex].Attribute == AMD_MEMORY_ATTRIBUTE_RESERVED))
            {
                Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_RESERVED, MEM_DET_COMMON_MEM_ATTR,
                                                    MemRangDesc[MemRangeIndex].Base, MemRangDesc[MemRangeIndex].Size);
                NB_TRACE((TRACE_NB, "MemDescriptor[%2d] -- Base: %16lx -- Length: %16lx -- Attribute: %x HobStatus: %r\n", 
                                                    MemRangeIndex, MemRangDesc[MemRangeIndex].Base, MemRangDesc[MemRangeIndex].Size, MemRangDesc[MemRangeIndex].Attribute, Status));
            }
            if(MemRangDesc[MemRangeIndex].Attribute == AMD_MEMORY_ATTRIBUTE_UMA)
            {
                UMABase = MemRangDesc[MemRangeIndex].Base;
                UMASize = MemRangDesc[MemRangeIndex].Size;
            }
        }
    }

    ///add for SMM lock hang on 2GB DDR, align dTOM with TSEG_SIZE
	NB_TRACE((TRACE_NB, "MemDetectSupportInit dTOM = %x\n", dTOM));
	dTOM &= ~(TSEG_SIZE - 1);
	NB_TRACE((TRACE_NB, "MemDetectSupportInit aligned dTOM = %x\n", dTOM));
	
    NB_TRACE((TRACE_NB, "Tom = %x\n", dTOM));
    NB_TRACE((TRACE_NB, "Available MEM above 4G = %lx\n", TOM2));
    NB_TRACE((TRACE_NB, "Uma Base = %16lx\n", UMABase));
    NB_TRACE((TRACE_NB, "Uma Size = %16lx\n", UMASize));
    
    Status = SaveDTom(PeiServices, dTOM);

    // Reserve FV_BB area in DRAM
    BuildMemoryAllocationHob(HIGH_MEMORY_REGION_BB_BASE, FV_BB_SIZE, EfiReservedMemoryType);
    
//### EIP314779 There is no need to disable VAR MTRRs for Fam17
//###    CreateMTRRHob(PeiServices);

    Status = InstallEfiMemory(PeiServices, dTOM, UMABase, UMASize);


    return Status;
}


/**
    This function is used to report the installed memory size
    to the Core Services. This function also creates appropriate
    HOBs

    @param PeiServices Pointer to the PEI services table
    @param dTOM 0 - TOM1 (exclude UMA)
    @param UMABase The base address of UMA
    @param UMASize The size of UMA

    @retval EFI_STATUS Return Status

**/

EFI_STATUS
InstallEfiMemory(
    IN  EFI_PEI_SERVICES        **PeiServices,
    IN  UINT32                  dTOM,
    IN  UINT64                  UMABase,
    IN  UINT64                  UMASize
)
{
    UINT32                      MemBegin = MM_LEGACY_RESERVE_MEM;
    EFI_STATUS                  Status;
    PEI_CAPSULE_PPI             *Capsule;
    BOOLEAN                     CapsulePresent = FALSE;
    VOID                        *Buffer;
    UINTN                       CapsuleSize = 0;
    UINT8                       Value8 = 0;

    //Check if Bit7 is set for capsule
    IoWrite8(0x72, PcdGet8(PcdCmosAgesaSpecified));
    Value8 = IoRead8(0x73);
#if defined(CAPSULE_RESET_MODE) && CAPSULE_RESET_MODE == 1
    Value8 |= BIT6;
#endif

    Status = (*PeiServices)->LocatePpi (PeiServices, &gPeiCapsulePpiGuid, 0, NULL, &Capsule);
    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Locate gPeiCapsulePpiGuid = %x\n", Status));
    if (!EFI_ERROR(Status))
    {
        Status = EFI_NOT_FOUND;// We may not go into the Capsule->Coalesce.
        					   // A successful status results in CapsluePresent and an error action occurs.
        Buffer = (VOID *)(UINTN)MemBegin;
        CapsuleSize = (UINTN)(dTOM - (MemBegin + TSEG_SIZE));

        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Buffer = %x\n", Buffer));
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "CapsuleSize = %x\n", CapsuleSize));

        // we do capsule only when the following conditions are satisfied
        // 1. system is boot from S3 or warm reset
        // 2. capsule flag is set
        if ((!SbCheckAfterG3()) && (Value8 & BIT6))
        {
            IoWrite8(0x80, 0xC0);
            Status = Capsule->Coalesce(PeiServices, &Buffer, &CapsuleSize);
            IoWrite8(0x80, 0xC1);
            PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "After Coalesce Status = %x\n", Status));
            PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "After Coalesce Buffer = %x\n", Buffer));
            PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "After Coalesce Size = %x\n", CapsuleSize));
        }

        //clear the capsule flag anyway no matter the Coalesce is successful or not
        IoWrite8(0x72, PcdGet8(PcdCmosAgesaSpecified));
        Value8 &= ~BIT6;
        IoWrite8(0x73, Value8);

        if (!EFI_ERROR(Status))
        {
            CapsulePresent = TRUE;
        }
    }

    // Report the memory to EFI (MemoryBegin to TOM- TSEGSize - MemoryBegin)
    Status = (*PeiServices)->InstallPeiMemory(PeiServices, MemBegin, (dTOM - (MemBegin + TSEG_SIZE)));
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Memory reporting failed.\n"));
        return Status;
    }

    if (CapsulePresent) {
        Capsule->CreateState(PeiServices, Buffer, CapsuleSize);
    }

    // Report first 640KB of memory
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                                         MEM_DET_COMMON_MEM_ATTR, 0, (640 * 1024));
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "640KB Mem reporting HOB creation failed.\n"));
        return Status;
    }

    // Report first 0A0000h - 0FFFFFh as RESERVED memory
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_RESERVED,
                                         MEM_DET_COMMON_MEM_ATTR, 0xA0000, (384 * 1024));
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "SMRAM ASEG reporting HOB creation failed.\n"));
        return Status;
    }


    // Report UMA range as reserved memory
    if (UMASize)
    {
        Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_RESERVED,
                                         MEM_DET_COMMON_MEM_ATTR, UMABase, UMASize);
        if (Status != EFI_SUCCESS) {
            PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "UMA reporting HOB creation failed.\n"));
		    return Status;
	    }
    }

#if	SMM_SUPPORT

    // Report 1MB to dTOM-TSEG_SIZE as system memory
 //   Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_RESERVED,
       //                                  MEM_DET_COMMON_MEM_ATTR, 0x1e80000,
     //                              0x180000 );
    
    // Report 1MB to dTOM-TSEG_SIZE as system memory
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                                         MEM_DET_COMMON_MEM_ATTR, 0x100000,
                                         (dTOM - (TSEG_SIZE + 0x100000)) );
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "System memory reporting HOB creation failed.\n"));
        return Status;
    }

    // Report (TOM-TSEG_SIZE) to TSEG_SIZE as reserved memory (SMRAM TSEG)
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_RESERVED,
                                         MEM_DET_COMMON_MEM_ATTR, (dTOM - TSEG_SIZE), TSEG_SIZE);
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "SMRAM TSEG reporting HOB creation failed.\n"));
        return Status;
    }
    
    // Report Tom to 4GB
     /* Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_MEMORY_MAPPED_IO,
                                           0x8000000000000001, (dTOM - TSEG_SIZE), TSEG_SIZE);
      if (Status != EFI_SUCCESS) {
          PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "SMRAM TSEG reporting HOB creation failed.\n"));
          return Status;
      }
      */
    
#else			// SMM_SUPPORT

    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Creating resource (1M to size:%x)\n",
               (dTOM - 0x100000) ));
    // Report 1MB to dTOM as system memory
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
                                         MEM_DET_COMMON_MEM_ATTR, 0x100000, (dTOM - 0x100000) );
    if (Status != EFI_SUCCESS) {
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "System memory reporting HOB creation failed.\n"));
        return Status;
    }
#endif

    return Status;
//Above 4GB Memory will be reported in CPUPei.c file.
}

/**
    This function is used to report the installed memory size
    during the S3 resume. It reports the memory from the value
    stored in the NVRAM

    @param PeiServices Pointer to the PEI services table
    @param ReadOnlyVariable Pointer to the Read Only Variable PPI

    @retval EFI_STATUS Return Status

**/

EFI_STATUS
InstallS3Memory(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable
)
{
    EFI_STATUS			Status;
    UINTN				S3MemoryBase;
    UINTN				S3MemorySize;
    NB_MEMORY_LENGTH    NbMemoryLength;
    UINTN				VarSize = sizeof(UINTN);
    UINTN				VarAttrib = 0;
    ACPI_VARIABLE_SET   *AcpiVariableSet = NULL;
    CHAR16              NbMemoryLengthVar[] = NB_MEMORY_LENGTH_VARIABLE;

    // Get type of EfiACPIMemoryNVS memory (AcpiS3Save.c)
    Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,
                                            ACPI_GLOBAL_VARIABLE,
                                            &gEfiAcpiVariableGuid,
                                            &VarAttrib,
                                            &VarSize,
                                            &AcpiVariableSet);
    if (EFI_ERROR (Status) || (AcpiVariableSet == NULL))
        return EFI_OUT_OF_RESOURCES;

    S3MemoryBase = (UINTN)(AcpiVariableSet->AcpiReservedMemoryBase);
    S3MemorySize = (UINTN)(AcpiVariableSet->AcpiReservedMemorySize);

    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Installing S3 PEI Memory. SysMem : %dMB \n",
               ((UINT32)AcpiVariableSet->SystemMemoryLength / (1024 * 1024))));

    // Report Memory to EFI
    Status = (*PeiServices)->InstallPeiMemory (
                 PeiServices, S3MemoryBase, S3MemorySize);
    ASSERT_PEI_ERROR (PeiServices, Status);

    VarSize = sizeof(NbMemoryLength);
    VarAttrib = 0;

    Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                            NbMemoryLengthVar, \
                                            &gNbMemoryLengthGuid, \
                                            &VarAttrib, \
                                            &VarSize, \
                                            &NbMemoryLength );
    if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;

#if NB_DEBUG_MESSAGE
    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Create S3 Memory HOB. Below 4G : %dMB \n",
               ((UINT32)NbMemoryLength.MemoryLengthBelow4G / (1024 * 1024))));
#endif
    Status = CreateHobResourceDescriptor( PeiServices, NULL, \
                                          EFI_RESOURCE_SYSTEM_MEMORY, \
                                          MEM_DET_COMMON_MEM_ATTR, 0x100000, \
                                          NbMemoryLength.MemoryLengthBelow4G);
    ASSERT_PEI_ERROR (PeiServices, Status);

    if (NbMemoryLength.MemoryLengthAbove4G) {
#if NB_DEBUG_MESSAGE
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Create S3 Memory HOB. Above 4G : %dMB \n",
                   ((UINT32)NbMemoryLength.MemoryLengthAbove4G / (1024 * 1024))));
#endif
        Status = CreateHobResourceDescriptor( PeiServices, NULL, \
                                              EFI_RESOURCE_SYSTEM_MEMORY, \
                                              MEM_DET_COMMON_MEM_ATTR, \
                                              0x100000000, \
                                              NbMemoryLength.MemoryLengthAbove4G);

        ASSERT_PEI_ERROR (PeiServices, Status);
    }

    return EFI_SUCCESS;
}

BOOLEAN SbCheckAfterG3 (
    VOID
)
{
    return FALSE;
    
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

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

/** @file CpuPei.c
    This file is the main CPU PEI component file. This component
    utilizes CPU I/O & PCI CFG PPI to publish early CPU Init PPI
    which can be used by NB PEI to load itself.  Also this file
    contains a CPU init routine to be executed in permanent memory
    present environment.  This is handled by issuing a notifyPPI
    on permanent memory PPI

**/

#include <pei.h>
#include <Ppi/Stall.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <AmiPeiLib.h>
#include "cpupei.h"
#include <AmiSmm.h>
#include "CPUCspLib.h"
#include <hob.h>
#include "Nb.h"
#include <CpuSetupData.h>
#include <PPI/CPUPolicy.h>

// Setup GUID variables for installing, locating and notifying PPIs  
EFI_GUID gAmiCpuinfoHobGuid				= AMI_CPUINFO_HOB_GUID;  

CHAR16  gCpuS3ResumeVariable[]          = CPU_S3_RESUME_VARIABLE;

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

extern VOID InitMtrrFamily15(EFI_PEI_SERVICES **PeiServices);

EFI_STATUS CpuInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
);

static EFI_PEI_NOTIFY_DESCRIPTOR CpuMemInstalledNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiMemoryDiscoveredPpiGuid, CpuInitAfterMemInstalled },
};

static EFI_PEI_PPI_DESCRIPTOR  mPpiAfterCpuPeiPpiGuid = 
{
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gHygonAfterCpuPeiPpiGuid,
  NULL
};

/**
    This collects all CPU features from CPUID. It provides in *Features the
    common features supported from all CPUs and returns CPU with the least number of
    features.

        
    @param VOID

         
    @retval UINT8 bFlag

**/

UINT8
isBsp(VOID)
{
    UINT8	bFlag = 1;				// 1 = BSP, 0 = AP
    UINT64	Buffer64;

    Buffer64 = ReadMsr(MSR_XAPIC_BASE);
    if ((Buffer64 & 0x0100) == 0)
    {
        bFlag = 0;
    }
    return bFlag;
}

/**
    This is an initialization routine for BSP and all APs.
    BSP has CPU number 0, and APs > 1. After init, BSP number may change.

        
    @param Cpu - number associated with CPU.
    @param Context 

         
    @retval VOID

**/

VOID
InitializeCpu()
{
    UINT32		    Value;
    UINT32		    CpuSignature;
    UINT64		    CpuFeatureInfo;
    UINT32	        FeatureFlagsEdx;
    UINT32	        FeatureFlagsEcx;
    UINT32	        RegEbx;
    
     
    CPULib_CpuID(1, &CpuSignature, &RegEbx,
                 &FeatureFlagsEcx, &FeatureFlagsEdx);

    CpuFeatureInfo = Shl64(FeatureFlagsEcx, 32) + FeatureFlagsEdx;


        // Read SVR data from Local APIC
        Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_SPURIOUS_VECTOR_REGISTER);
        //Clear SVR, Use Vector 0Fh
        Value &= 0x0F;
        Value |= 1 << 8;	//Enable Apic
        *(UINT32*)(UINTN)(APIC_BASE + APIC_SPURIOUS_VECTOR_REGISTER) = Value;
  
        //Program LINT0 vector as ExtInt
        Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT0_REGISTER);
        Value &= 0xFFFE58FF;
        Value |= 0x700;
        *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT0_REGISTER) = Value;

        //Program the LINT1 vector entry as NMI
        Value = *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT1_REGISTER);
        Value &= 0xFFFE58FF;

        Value |= 0x400;

        *(UINT32*)(UINTN)(APIC_BASE + APIC_LVT_LINT1_REGISTER) = Value;

    WriteMsr (MSR_SYS_CFG,
              ReadMsr (MSR_SYS_CFG) & (~(UINT64)(1 << MTRRFixDRAMModEnBit)) );

	return;
}

/**
    Get memory size below 4GB.

        
    @param PeiServices 

         
    @retval UINT64 SystemMemoryLength

**/

UINT64
GetSystemMemoryLength(
    IN EFI_PEI_SERVICES **PeiServices)
{

    // Please refer to EIP 30689 and EIP 30223 to understand why the above
    // implementation was broken and commented out

    EFI_PEI_SAVE_DTOM_PPI  *mSaveDTOMPpi;
    UINT64  SystemMemoryLength = 0x100000;
    EFI_STATUS			Status;

    // Locate EFI_PEI_SAVE_DTOM_PPI published in memdetect.c
    Status = (*PeiServices)->LocatePpi(PeiServices, &gAmiDTomGuid, \
                                       0, NULL, &mSaveDTOMPpi);
    ASSERT_PEI_ERROR( PeiServices, Status );
    if (mSaveDTOMPpi->Value == 0x100000)
        ASSERT_PEI_ERROR(PeiServices,EFI_NOT_FOUND);
    // The following calculation is based on InstallEfiMemory in memdetect.c
    // May need some modifications per project
    SystemMemoryLength = (UINT64)(mSaveDTOMPpi->Value - TSEG_SIZE);
    return SystemMemoryLength;
}

/**
    Create CPU Hob and fill in default data.

        
    @param PeiServices 
    @param NumCpus 

         
    @retval CPUINFO_HOB CpuinfoHob

**/

CPUINFO_HOB *
CreateCpuHobWithDefaults(
    IN EFI_PEI_SERVICES **PeiServices,
    IN UINT8 NumCpus)
{
    CPUINFO_HOB	*CpuinfoHob;
    EFI_STATUS	Status;
    UINT8		i;

    //Create hob for storing Cpu Data
    Status = (**PeiServices).CreateHob(PeiServices,
                                       EFI_HOB_TYPE_GUID_EXTENSION,
                                       //NOTE: sizeof(CPUINFO_HOB) already includes size of one CPUINFO structure
                                       sizeof(CPUINFO_HOB) + (NumCpus - 1) * sizeof(CPUINFO),
                                       &CpuinfoHob
                                      );
    ASSERT_PEI_ERROR(PeiServices, Status);

    CpuinfoHob->EfiHobGuidType.Name = gAmiCpuinfoHobGuid;
    CpuinfoHob->CpuCount = NCPU;
    CpuinfoHob->BspNo = 0; //BSP is always 0
    CpuinfoHob->CacheLineSize = 64;
    CpuinfoHob->NodeCount = HygonNodeCount();
    CpuinfoHob->TsegAddress = GetSystemMemoryLength(PeiServices);
    CpuinfoHob->TsegSize = TSEG_SIZE;

    PEI_TRACE((-1, PeiServices,
               "CreateCpuHobWithDefaults: NodeCount = 0x%X\n",
               CpuinfoHob->NodeCount));

    for (i = 0; i < NumCpus; i++) {
        CpuinfoHob->Cpuinfo[i].Valid	= FALSE;
        CpuinfoHob->Cpuinfo[i].Disabled	= FALSE;
        CpuinfoHob->Cpuinfo[i].ApicId	= i;
        CpuinfoHob->Cpuinfo[i].ApicEId	= i;
    }
    return CpuinfoHob;
}


/**
    Returns pointer to S3 Resume data in memory.

        
    @param PeiServices 
    @param ReadOnlyVariable 

         
    @retval CPU_S3_RESUME CpuS3Resume

**/
CPU_S3_RESUME * GetS3ResumeVariable(
    IN EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable
)
{
    CPU_S3_RESUME	*CpuS3Resume = 0;
    EFI_STATUS		Status;
    UINTN           VariableSize = sizeof(CPU_S3_RESUME*);

    Status = ReadOnlyVariable->GetVariable(
                 ReadOnlyVariable,
                 gCpuS3ResumeVariable, &gCpuS3ResumeGuid,
                 NULL,
                 &VariableSize, &CpuS3Resume
             );
    if (EFI_ERROR(Status))
    {
        PEI_TRACE((-1, PeiServices, "Smm S3 Resume -- No CPU S3 Resume Variable\n"));
        PEI_ERROR_CODE(PeiServices, PEI_S3_RESUME_ERROR, EFI_ERROR_MAJOR);
    }

    return CpuS3Resume;
}

/**
    Get the TSEG base

    @param PeiApData 

    @retval UINT32 TSEG Base

**/

UINT32
GetTsegBase(IN PEI_AP_DATA *PeiApData)
{
    return (UINT32)PeiApData->TopOfMemory;
}

/**
    Change SMM base

        
    @param Cpu 
    @param Context 

         
    @retval VOID

**/

VOID
SmmCpuBaseChange(
    IN UINT8 Cpu,
    IN VOID *Context)
{
    SmmBaseChangeOnCpu(Context);    //Context = Smmbase
}


/**
    Creates HOB resource descriptor for memory above 4Gb.

        
    @param PeiServices pointer to PEI services structure

          
    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS
ReportTOM2(
    IN EFI_PEI_SERVICES **PeiServices)
{
    EFI_PEI_SAVE_DTOM2_PPI  *mSaveDTOM2Ppi;
    EFI_STATUS          Status;

    // Locate EFI_PEI_SAVE_DTOM_PPI published in memdetect.c
    Status = (*PeiServices)->LocatePpi(PeiServices, &gAmiDTom2Guid, \
                                       0, NULL, &mSaveDTOM2Ppi);

    if (EFI_ERROR(Status))    //If not found then no TOM2
        return Status;
    
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
            MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  mSaveDTOM2Ppi->Base, mSaveDTOM2Ppi->Size);

    if (Status != EFI_SUCCESS) 
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Above 4GB system memory reporting HOB creation failed.\n"));

    return Status;
}

EFI_STATUS
ReportTOM3(
    IN EFI_PEI_SERVICES **PeiServices)
{
    EFI_PEI_SAVE_DTOM2_PPI  *mSaveDTOM3Ppi;
    EFI_STATUS          Status;

    // Locate EFI_PEI_SAVE_DTOM_PPI published in memdetect.c
    Status = (*PeiServices)->LocatePpi(PeiServices, &gAmiDTom3Guid, \
                                       0, NULL, &mSaveDTOM3Ppi);

    if (EFI_ERROR(Status))    //If not found then no TOM3
        return Status;
    
    Status = CreateHobResourceDescriptor(PeiServices, NULL, EFI_RESOURCE_SYSTEM_MEMORY,
            MEM_DET_COMMON_MEM_ATTR_ABOVE4G,  mSaveDTOM3Ppi->Base, mSaveDTOM3Ppi->Size);

    if (Status != EFI_SUCCESS) 
        PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Above 4GB system memory reporting HOB creation failed.\n"));

    return Status;
}

/**
    Reads PSP enabled/disabled status

    @param None 

    @retval BOOLEAN TRUE - PSP enabled
                    FALSE - PSP disabled

**/

BOOLEAN
AmiIsPspEn(
VOID
)
{
    UINT32          RegEax;
    UINT32          RegEbx; // Ext Family Mask, 0x0FF00000ul
    UINT32          RegEcx; // Ext Model Mask,  0x000F0000ul
    UINT32          RegEdx; // Stepping Mask,   0x0000000Ful
    BOOLEAN         PSPstate;

    CPULib_CpuID(1, &RegEax, &RegEbx,
            &RegEcx, &RegEdx);
    
    RegEbx = RegEax & 0x0FF00000;
    RegEcx = RegEax & 0x000F0000;
    RegEdx = RegEax & 0x0000000F;
    
    //if ((RegEbx | RegEcx) == 0x00660000)
    //{
        //This is Family 15h Model 60h~6Fh
        if (RegEdx == 0)
            PSPstate = FALSE;  // A0 stepping, S3 is going legacy path
        else
            PSPstate = TRUE;   // A1 stepping, S3 is using PSP restore
    //}
    
    return  PSPstate;

}


VOID
DisableRomCachingAp(
    IN UINT8    Cpu, 
    IN VOID     *Context
)
{
    WriteMsr(MTRR_PHYS_MASK_7, 0);
    WriteMsr(MTRR_PHYS_BASE_7, 0);
}


/**
    This function can be used to program any CPU register after
    PEI permanent memory be installed.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
           notification event.
    @param InvokePpi Pointer to the PPI that was installed

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS CpuInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{

    EFI_PEI_READ_ONLY_VARIABLE2_PPI	*ReadOnlyVariable;
    EFI_PEI_STALL_PPI	*PeiStall;
    EFI_BOOT_MODE		BootMode;
    EFI_STATUS			Status;
    CPUINFO_HOB			*CpuinfoHob;
    CPU_SETUP_DATA      CpuSetupData;
	PEI_AP_DATA			PeiApData;	//This is used pass data to APs. Global Variables won't in ROM.
//	VOID				*MpData;
	UINT64				SystemMemoryLength;
    UINT8               NumCpus;
    BOOLEAN             IsPspEnabled = FALSE;
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    ASSERT_PEI_ERROR(PeiServices, Status);
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gEfiPeiReadOnlyVariable2PpiGuid,
                 0, NULL,
                 &ReadOnlyVariable
             );
    ASSERT_PEI_ERROR(PeiServices, Status);

    if (BootMode == BOOT_ON_S3_RESUME )
    {
        IsPspEnabled = AmiIsPspEn();    // Check PSP fuse
        PeiApData.CpuS3Resume = GetS3ResumeVariable(PeiServices, ReadOnlyVariable);
    } else
    {
        PeiApData.CpuS3Resume = 0;
    }
    //----------------------------------------------//
    // Execute this function ASAP for performance. //
    //            This enables BSP cache.           //
    //----------------------------------------------//

    SystemMemoryLength = GetSystemMemoryLength(PeiServices);
    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "SystemMemoryLength= %dMB\n",
               ((UINT32)SystemMemoryLength / (1024 * 1024)) ));
    PEI_PROGRESS_CODE(PeiServices, PEI_CPU_CACHE_INIT);
    if (PeiApData.CpuS3Resume) 
    {
        ProgramS3ResumeMtrrs(PeiApData.CpuS3Resume->MtrrValue, IsPspEnabled);
    }
	else
//	PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "\n!********BEFORE CACHE ON**********!\n"));
	    InitMtrrFamily15(PeiServices);
  //      PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "\n!********CACHE ON**********!\n"));
#if 0
    if (BootMode == BOOT_ON_S3_RESUME)
    {
        UINT32  MsrBase = 0;
        UINT64  Value64 = 0;

        MsrBase = MTRR_PHYS_BASE_7;
        Value64 = (0x0100000000 - FLASH_SIZE + 0x5);
        WriteMsr(MsrBase, Value64);
        Value64 = 0xffffffffffff & (~(FLASH_SIZE - 1) + (1 << 11));
        WriteMsr(MsrBase + 1, Value64);
    }
#endif
    Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gEfiPeiStallPpiGuid,
                 0, NULL,
                 &PeiStall
             );
    ASSERT_PEI_ERROR(PeiServices, Status);

    GetCpuSetupData(PeiServices, &CpuSetupData, TRUE);

    PeiApData.BootMode = BootMode;
    PeiApData.CpuSetupData = &CpuSetupData;
    PeiApData.IsSetupDataValid = EFI_SUCCESS;
    PeiApData.TopOfMemory = SystemMemoryLength;
    PeiApData.NumCpusDisabled = 0;

    NumCpus = NCPU;

    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "CPUPEI: GetNumCpus returned %d\n",NumCpus));

    //After initialized, APs are in holding loop until halted.
    CpuinfoHob = CreateCpuHobWithDefaults(PeiServices, NumCpus);
//    CpuinfoHob->CpuFeatures = Features;
    PeiApData.Cpuinfo = &CpuinfoHob->Cpuinfo[0];

    //All CPUs initialized including BSP.
    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "CPUPEI: Before ExecuteFunctionOnRunningCpus\n"));
    PeiApData.PeiServices = PeiServices;
    PeiApData.Semaphore = 0;
    // All CPUs must execute InitializeCpu
    PEI_PROGRESS_CODE(PeiServices, PEI_CPU_INIT);

    InitializeCpu();

    if (!ReadMsr(HYGON_MSR_MCODE_SIGNATURE))
    {
        PEI_PROGRESS_CODE (PeiServices, PEI_CPU_NO_MICROCODE);
    }

    Status = (*PeiServices)->InstallPpi (PeiServices, &mPpiAfterCpuPeiPpiGuid);

    return  Status;
}


/**
    This function is called after the permanent memory is installed
    in the system.  This function does the initializes CPU in the memory
    present environment.

    @param FileHandle Handle of the file being invoked
    @param PeiServices Pointer to the PEI services table

    @retval EFI_SUCCESS always success

**/
EFI_STATUS CpuPeiEntry(
  IN       EFI_PEI_FILE_HANDLE      FileHandle,  
  IN CONST EFI_PEI_SERVICES         **PeiServices)
{

	EFI_STATUS			Status;
    Status = (*PeiServices)->NotifyPpi( PeiServices, CpuMemInstalledNotifyList );
    ASSERT_PEI_ERROR ( PeiServices, Status );
    
	return Status;
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


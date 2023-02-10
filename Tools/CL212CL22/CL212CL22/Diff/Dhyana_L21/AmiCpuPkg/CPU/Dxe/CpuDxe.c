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

/** @file CpuDxe.c
    Installs CPU Architectural Protocol and initializes the
    processor interrupt vector table. The CPU Architectural
    Protocol enables/disables/get state of interrupts, set
    memory range cache type, and installs/uninstalls
    interrupt handlers.

**/

#include "CpuDxe.h"
#include <CpuBoard.h>
#include <Dxe.h>
#include <ffs.h>
#include <token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include "CPUCspLib.h"
#include <AmiHobs.h>
#include "CpuInterfaces.h"
#include <Protocol\AmiCpuInfo.h>
#include <Protocol\Variable.h>
#include <Protocol\MpService.h>
#include <Protocol\Datahub.h>
#include <Setup.h>
// Agesa ACPI table generation includes
#ifdef AMI_PEI_DEBUG_SUPPORT
  #include "PeiDbgDxeCpuLib.h"
#endif
#include "CPUPrepareToBoot.h"
#include <CpuSetupData.h>
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
#include <Protocol/LegacyBiosExt.h>
#endif
#include <Library/UefiCpuLib.h>
#include <../../../UefiCpuPkg/Library/CpuExceptionHandlerLib/CpuExceptionCommon.h>
extern MTRR_SETTINGS   *mMtrrTable;
EFI_GUID gAmiCpuinfoHobGuid			= AMI_CPUINFO_HOB_GUID;
EFI_GUID gEfiSetupGuid				= SETUP_GUID;

CHAR16  gCpuS3ResumeVariable[] = CPU_S3_RESUME_VARIABLE;
CHAR16  gSetupVariable[]       = L"Setup";

EFI_DATA_HUB_PROTOCOL   *gDataHub;
CPUINFO_HOB             *gCpuInfoHob;
VOID                    *FirstHob;
AMI_CPU_INFO            *gAmiCpuInfo;
CPU_S3_RESUME           *gCpuS3Resume;

VOID    *gDataHubRegist = NULL;
BOOLEAN gDataHubInstalled = FALSE;

volatile UINT8  gAmiCpuInfoSemaphore = 0;
UINT8           gNumCpus = 0;
UINT64          gTimerPeriod = 0;
UINT32          gNumBoostStates = 0;
UINT8           gCoreCnt = 0;   // this value will be update at the end of DxeInitializeCPU
SETUP_DATA      gSetupData;
BOOLEAN         gIsSetupDataValid;
BOOLEAN         gC1ESupport = TRUE;
UINT32          gC1EStackSave;
#ifdef EFIx64
UINT16          *gIrqRuntimeHandler;
#else
UINT8           *gIrqRuntimeHandler;
#endif
INTR_GATE_DESC  *gIdt;

VOID            *gMpData;

#pragma pack(1)
typedef struct {
    UINT8   PushOp;
    UINT32  Interrupt;
    UINT8   PushEaxOp;
#ifdef EFIx64
    UINT8   Ext64Op;
#endif
    UINT8   MovEaxOp;
    VOID    *pJmpAddr;
    UINT16  JmpOp;          //Jmp ebx/rbx
} INT_ENTRY_POINT;
#pragma pack()

INT_ENTRY_POINT     *gIntEntryPoints;

INT_ENTRY_POINT IntEntryTemplate = 
{
    0x68,           //push Int
    0,              //Int
    0x50,           //push eax/rax
#ifdef EFIx64
    0x48,           //Extended Opcode
#endif
    0xb8,           //Mov eax,
    0,              //pJmpAddr
    0xe0ff          //Jmp ebx/rbx
};

//This extern is for a table of offset of interrupt routines in CpuSupport.asm.
extern UINT32       MachineCheckHandlerSize;
extern VOID         *InterruptHandlerStart;
extern UINT32       InterruptHandlerSize;
extern UINT32       InterruptHandlerTblFixup;
// These externs define structures to initialize cache descriptors
extern CPU_CACHE_TABLE tCache[];
extern UINT8 tCacheSize;

EFI_STATUS      InitInterrupts();
VOID 	        InterruptHandlerHalt(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);
#if USE_AP_HLT
VOID            InterruptHandlerRet(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);
VOID            AsmIret(VOID);
#endif
VOID            SetCPUS3ResumeData();

VOID MachineCheckHandler(
    IN EFI_EXCEPTION_TYPE   InterruptType,
    IN EFI_SYSTEM_CONTEXT   SystemContext
);

VOID TempGPInterruptHandler(EFI_EXCEPTION_TYPE Exception,EFI_SYSTEM_CONTEXT Context);

CACHE_DESCRIPTOR_INFO gCacheDescTable[] = {
    0x2c, 1, 0,   32,  8,
    0x30, 1, 1,   32,  8,
    0x60, 1, 0,   16,  8,
    0x62, 1, 0,   32,  8,
    0x66, 1, 0,    8,  4, 
    0x67, 1, 0,   16,  4,
    0x68, 1, 0,   32,  4,
    0x69, 1, 0,   64,  4,
    0x39, 2, 3,  128,  4,
    0x3a, 2, 3,  192,  6,
    0x3b, 2, 3,  128,  2,
    0x3c, 2, 3,  256,  4,
    0x3d, 2, 3,  384,  6,
    0x3e, 2, 3,  512,  4,
    0x78, 2, 3, 1024,  4,
    0x79, 2, 3,  128,  8,
    0x7a, 2, 3,  256,  8,
    0x7b, 2, 3,  512,  8,
    0x7c, 2, 3, 1024,  8,
    0x7d, 2, 3, 2048,  8,
    0x7f, 2, 3,  512,  2,
    0x86, 2, 3,  512,  4,
    0x87, 2, 3, 1024,  8,
    0x40, 3, 0,    0,  0,   //No L2 cache, or if L2 cache, then no L3 cache.
    0x22, 3, 3,  512,  8,
    0x23, 3, 3, 1024,  8,
    0x25, 3, 3, 2048,  8,
    0x29, 3, 3, 4096,  8,
    0x46, 3, 3, 4096,  4,
    0x47, 3, 3, 8192,  8,
    0x4a, 3, 3, 4096,  8,
    0x4b, 3, 3, 6144, 12,
    0x4c, 3, 3, 8192, 16
};
#define CACHE_DESCRIPTOR_INFO_TABLE_SIZE sizeof(gCacheDescTable) / sizeof(CACHE_DESCRIPTOR_INFO)

CACHE_DESCRIPTOR_INFO gZeroCacheDesc = {0, 0, 0, 0, 0};

#define  PCI_ADDRESS(bus, dev, func, reg) \
            ((UINT64) ( (((UINT8)bus) << 16) + (((UINT8)dev) << 11) + \
            (((UINT8)func) << 8) + ((UINT8)reg) )) & 0x00000000ffffffff

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) \
        + (((UINT32)func) << 16) + ((UINT32)reg) )
    @param ExtAccess : TRUE for extended access, FALSE is for normal access

    @retval UINT32 return value from the PCI Register.

**/
UINT32 NbReadPCI32(
    IN UINT32 Address,
    IN BOOLEAN  ExtAccess
)
{
    UINT32  Value32;
    UINT32  AddrCf8 = (1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    if (ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        AddrCf8 = AddrCf8 | 0x1000000;

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Read a CMOS byte

        
    @param Index 

         
    @retval UINT8 Value

**/
UINT8 EFI_ReadCMOSByte(IN UINT8 Index)
{
    if(Index & 0x80)
    {
        //Upper CMOS Read
        IoWrite8(0x72,Index);
        IoRead8(0x61); //IoDelay
        return IoRead8(0x73);
    }
    else
    {
        //Lower CMOS Read
        IoWrite8(0x70,Index);
        IoRead8(0x61); //IoDelay
        return IoRead8(0x71);
    }
}

/**
    From entry in Cache Descriptor Table from Descriptor.

        
    @param Desc 

         
    @retval CACHE_DESCRIPTOR_INFO cache information

**/

CACHE_DESCRIPTOR_INFO * GetPtrCacheDescr(IN UINT8 Desc)
{
    UINT8 i;
    for(i = 0; i < CACHE_DESCRIPTOR_INFO_TABLE_SIZE; ++i)
    {
        if (gCacheDescTable[i].Desc == Desc) return &gCacheDescTable[i];
    }
    return 0;
}

DESCRIPTOR_TABLE pIDT, pGdt;

//This is a table of pointers to installed interrupt handlers.
//It will be initialized as NULL, meaning no interrupt handler installed.
EFI_CPU_INTERRUPT_HANDLER *InterruptPtrTable;

//This will be copied to runtime memory.
UINT64 GDT[] = {
    0,                  //NULL_SEL
    0x00cf93000000ffff, //DATA_SEL 8, Data 0-ffffffff 32 bit
    0x00cf9b000000ffff, //CODE_SEL 10h, CODE 0-ffffffff 32 bit

// We only need this because Intel DebugSupport driver
// (RegisterPeriodicCallback funciton) assumes that selector 0x20 is valid
// The funciton sets 0x20 as a code selector in IDT
    0x00cf93000000ffff,	// YET_ANOTHER_DATA_SEL, 0x18, Data 0-ffffffff 32 bit	
    0x00cf9b000000ffff,	//YET_ANOTHER_CODE_SEL, 0x20, CODE 0-ffffffff 32 bit
#ifdef EFIx64
    0,                  //Spare
    0x00cf93000000FFFF, //64 Data.
    0x00af9b000000FFFF, //64 Code
    0                   //Spare
#endif
};

/**
    Allocate memory in runtime and copy data.

        
    @param Src 
    @param Size 

         
    @retval VOID return the runtime address

**/

VOID * CpyToRunTime(
    IN VOID *Src,
    IN UINTN Size)
{
    EFI_STATUS  Status;
    VOID        *Dest;

    //align to 16.
    Status = pBS->AllocatePool(EfiACPIMemoryNVS, Size + 16, &Dest);
    ASSERT_EFI_ERROR(Status);
    Dest = (VOID*)(((UINTN)Dest + 0xf) & ~(UINTN)0xf);

    MemCpy(Dest, Src, Size);
    return Dest;
}

/**
    Handler executed for ReadyToBoot

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID ExecuteAtReadyToBoot(IN EFI_EVENT Event, IN VOID *Context)
{
    UINT8 Value8;
    UINT64  HWCR_Value;
    HWCR_Value = ReadMsr(HYGON_MSR_HWCR);
    HWCR_Value |= 0x1;                      // Set SMM lock
    WriteMsr(HYGON_MSR_HWCR, HWCR_Value);

    SynchronizeMTRRs(pBS);
    SetCPUS3ResumeData();

    IoWrite8(0x72, PcdGet8(PcdCmosAgesaSpecified));
    IoRead8(0x61);
    Value8 = IoRead8(0x73); // Read from CMOS
    Value8 |= BIT1; 
    IoWrite8(0x73, Value8); // Save to CMOS
    
    pBS->CloseEvent(Event);
}

/**
    Handler executed before OS.

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID CpuBootOnExitBootService(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
	UINT16 i;
    BOOLEAN UefiOS = (BOOLEAN)Context;
    

    SynchronizeMTRRs(pBS);
//    SetCPUS3ResumeData();

    if (!UefiOS) return;

    CPULib_DisableInterrupt();

    //The interrupts are programmed to a default runtime handler. This is because
    //earlier OSes that don't support multi-processors will not take control of APs.
    //If this happens, and the AP gets an exception, such as the machine check
    // exception, the system will hang or reset.
    for (i = 0; i < 256; ++i) {
        gIdt[i].LoOffset = (UINT16)gIrqRuntimeHandler;
        gIdt[i].HiOffset = (UINT16)((UINT32)gIrqRuntimeHandler >> 16);
    }

    HaltAllAps(gMpData, TRUE);  //If legacy, CSM halts them.
    pBS->CloseEvent(Event);
}

/**
    The last boot point on legacy boot path in CPU code.
    This will be the last time we sync the MSRs on all cores before
    booting to legacy OS.

        
    @param Event 
    @param Context 
         
    @retval VOID

**/

VOID CpuBootOnLegacyBoot(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
    SynchronizeMTRRs(pBS);

    pBS->CloseEvent(Event);
}

/**
    Calculate CPU time period.

        
    @param VOID

         
    @retval UINT64 Timer Period
**/

UINT64 CalculateTimerPeriod(VOID)
{
    UINT64  Timer1;
    UINT64  Timer2;

    EFI_TPL	OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);

    Timer1 = ReadRtdsc();

    pBS->Stall(DELAY_CALCULATE_CPU_PERIOD);

    Timer2 = ReadRtdsc();

    pBS->RestoreTPL(OldTpl);

    return Div64(DELAY_CALCULATE_CPU_PERIOD * (UINT64)1000000000, (UINT32)(Timer2-Timer1),NULL);	//50us * 10E15  femtosec;
}

/**
    Enable CPU Interrupts.

        
    @param This 

         
    @retval EFI_STATUS EFI_SUCCESS (Always)
**/

EFI_STATUS EfiCpuEnableInterrupt (
    IN EFI_CPU_ARCH_PROTOCOL    *This)
{
	CPULib_EnableInterrupt ();
    return EFI_SUCCESS;
}


/**
    Disable CPU Interrupts

        
    @param This 

         
    @retval EFI_STATUS EFI_SUCCESS (Always)

**/

EFI_STATUS EfiCpuDisableInterrupt (
    IN EFI_CPU_ARCH_PROTOCOL    *This)
{
    CPULib_DisableInterrupt ();
    return EFI_SUCCESS;
}


/**
    Return State (Disable/Enable) CPU Interrupts.

        
    @param This 
    @param State *State=FALSE if Disable.

         
    @retval EFI_STATUS EFI_SUCCESS (Always)
**/

EFI_STATUS EfiCpuGetInterruptState (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    OUT BOOLEAN                 *State)
{
    *State = CPULib_GetInterruptState ();
    return EFI_SUCCESS;
}

/**
    Send INIT to the CPU.

        
    @param This 
    @param InitType 

         
    @retval EFI_STATUS Never returns.

**/
EFI_STATUS EfiCpuINIT (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN EFI_CPU_INIT_TYPE        InitType)
{
	IoWrite8(0x64, 0xfe);	//reset system using keyboard controller.
    WaitForever();         	//wait until reset.
	return EFI_DEVICE_ERROR;
}

/**
    Install or Uninstall Interrupt Handler.

        
    @param This 
    @param InterruptType 
    @param InterruptHandler 

         
    @retval EFI_SUCCESS Interrupt handler was uninstalled/installed.
    @retval EFI_ALREADY_STARTED Attempt to install an Interrupt Handler
        when one is already installed.
    @retval EFI_INVALID_PARAMETER Attempt to uninstall an Interrupt Handler
        when one is not installed.
    @retval EFI_UNSUPPORTED Interrupt Type not supported.

**/
EFI_STATUS EfiCpuRegisterInterruptHandler (
    IN EFI_CPU_ARCH_PROTOCOL        *This,
    IN EFI_EXCEPTION_TYPE           InterruptType,
    IN EFI_CPU_INTERRUPT_HANDLER    InterruptHandler)
{
    if ((UINTN) InterruptType > 255) return EFI_UNSUPPORTED;
    if (InterruptHandler != NULL && InterruptPtrTable[InterruptType] != NULL)
        return EFI_ALREADY_STARTED;
    if (InterruptHandler == NULL && InterruptPtrTable[InterruptType] == NULL)
        return EFI_INVALID_PARAMETER;

    InterruptPtrTable[InterruptType] = InterruptHandler;

#ifdef	AMI_PEI_DEBUG_SUPPORT
    IsItPEIDebugIRQHandlerToHookup(InterruptType,InterruptHandler);
#endif

    return EFI_SUCCESS;
}

/**
    For a CPU timer, return its current value.

        
    @param This 
    @param TimerIndex 
    @param TimerValue 
    @param TimerPeriod OPTIONAL

         
    @retval EFI_SUCCESS The time value is returned.
    @retval EFI_INVALID_PARAMETER TimerIndex is greater than 0.
        Only timer exists.
    @retval EFI_INVALID_PARAMETER TimerValue is NULL.

**/

EFI_STATUS EfiCpuGetTimerValue (
    IN EFI_CPU_ARCH_PROTOCOL    *This,
    IN UINT32                   TimerIndex,
    OUT UINT64                  *TimerValue,
    OUT UINT64                  *TimerPeriod OPTIONAL)
{
    if (TimerIndex > 0) return EFI_INVALID_PARAMETER;
    if (TimerValue==NULL)  return EFI_INVALID_PARAMETER;
    if (TimerPeriod != NULL)
    {
        if (!gTimerPeriod) gTimerPeriod = CalculateTimerPeriod();
        *TimerPeriod = gTimerPeriod;
    }

    *TimerValue = ReadRtdsc();

    return EFI_SUCCESS;
}

/**
    Install Interrupt Handlers for 0 to 19, to the routine
    InterruptHalt. Initialize the interrupt descriptors.
    Execute assembly instruction LIDT.


    @param VOID

    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS InitInterrupts(VOID)
{
    UINT32          i;
    UINT16          Segment;
    VOID            *InterruptHandler;
    EFI_STATUS      Status;

#ifdef	AMI_PEI_DEBUG_SUPPORT
    // Porting to support PEI Debugger
    // Obtain info about Exception and INT handlers use by PEI Debugger
    SetupPEIDebuggerExceptionHandlers();
#endif

    //Allocate memory for addresses of interrupt Handlers.
    Status = pBS->AllocatePool(
        EfiACPIMemoryNVS,
        256 * sizeof(EFI_CPU_INTERRUPT_HANDLER), 
        (VOID*)&InterruptPtrTable
    );
    ASSERT_EFI_ERROR(Status);
    MemSet(InterruptPtrTable, 256 * sizeof(EFI_CPU_INTERRUPT_HANDLER), 0); //Clear addresses.

    //Allocate memory for addresses of interrupt Handlers.
    Status = pBS->AllocatePool(
        EfiACPIMemoryNVS,
        sizeof(*gIrqRuntimeHandler), 
        (VOID*)&gIrqRuntimeHandler
    );
    ASSERT_EFI_ERROR(Status);
#ifdef EFIx64
    *gIrqRuntimeHandler = 0xcf48;	//IRET
#else
    *gIrqRuntimeHandler = 0xcf;	//IRET
#endif
    //InterruptHandler must be set to runtime address before copying interrupt handler.
    InterruptHandler = CpyToRunTime(InterruptHandlerStart, InterruptHandlerSize);
    *(VOID**)((UINT8*)InterruptHandler + InterruptHandlerTblFixup) = InterruptPtrTable;
    IntEntryTemplate.pJmpAddr = InterruptHandler;

    //Allocate interrupt entry points and copy.
    //Alocate space for Interrupt Descriptor Table. 256 entries/8 bytes.
    Status = pBS->AllocatePool(EfiACPIMemoryNVS, 256 * sizeof(INT_ENTRY_POINT), &gIntEntryPoints);
    ASSERT_EFI_ERROR(Status);

    for(i = 0; i < 256; ++i) {
        IntEntryTemplate.Interrupt = i;
        gIntEntryPoints[i] = IntEntryTemplate;
    }

    //Alocate space for Interrupt Descriptor Table. 256 entries/8 bytes.
    Status = pBS->AllocatePool(EfiACPIMemoryNVS, 256 * sizeof(*gIdt), &gIdt);
    ASSERT_EFI_ERROR(Status);

    for(i=0;i<=19;++i) EfiCpuRegisterInterruptHandler(0,i,InterruptHandlerHalt);
#if USE_AP_HLT    
    // INT2 handler
    EfiCpuRegisterInterruptHandler(0,2,InterruptHandlerRet);
#endif     
    Segment = GetCsSegment();

    for (i = 0; i < 256; ++i)
    {
        UINTN IntHndlrAddr = (UINTN)&gIntEntryPoints[i];
        gIdt[i].LoOffset=*(UINT16*)&IntHndlrAddr;
        gIdt[i].HiOffset=*(((UINT16*)&IntHndlrAddr)+1);
#if USE_AP_HLT
        if(i == 2){
            UINTN AsmIretAddr = (UINTN)AsmIret;
            gIdt[i].LoOffset=*(UINT16*)&AsmIretAddr;
            gIdt[i].HiOffset=*(((UINT16*)&AsmIretAddr)+1);
        }
#endif        
        gIdt[i].Segment=Segment;
        gIdt[i].DescBits=0x8e00;  //Present=1, DPL = 0, D = 1 (32bit)
#ifdef EFIx64
        gIdt[i].Hi32Offset = 0;
        gIdt[i].Rsv = 0;
#endif
#ifdef  AMI_PEI_DEBUG_SUPPORT
        FixUpPEIDebuggerExceptionHandlers((IDTEntry_T*)gIdt,i);
#endif
    }

    pIDT.Base=(UINT32)(UINTN)gIdt;

#ifdef EFIx64
    pIDT.Limit=16*256-1;
#else
    pIDT.Limit=8*256-1;
#endif

    return EFI_SUCCESS;
}


/**
    CPU exception

        
    @param Exception 
    @param Context 

         
    @retval VOID

**/
VOID InterruptHandlerHalt(
    IN EFI_EXCEPTION_TYPE Exception,
    IN EFI_SYSTEM_CONTEXT Context)
{
    checkpoint((UINT8)Exception);
    TRACE((TRACE_DXE_CPU,"ERROR: CPU Exception %X\n",Exception));
    DumpCpuContent(Exception,Context);
    HltCpu();
}

#if USE_AP_HLT
VOID InterruptHandlerRet(
    IN EFI_EXCEPTION_TYPE Exception,
    IN EFI_SYSTEM_CONTEXT Context)
{
    AsmIret();
}
#endif
/**
    Protocol function to get Cpu Info.

        
    @param This 
    @param Cpu 
    @param Info 

         
    @retval EFI_STATUS return the EFI status

**/
EFI_STATUS GetCpuInfo(
    IN AMI_CPU_INFO_PROTOCOL	*This,
    IN UINTN                    Cpu,
    OUT AMI_CPU_INFO            **Info)
{
    if (Cpu >= gNumCpus) return EFI_INVALID_PARAMETER;
    *Info = &gAmiCpuInfo[Cpu];
    return EFI_SUCCESS;
}

/**
    This function executed on AP core and reads SMM Base MSR

        
    @param pSmmBase pointer to SmmBase value

         
    @retval EFI_STATUS return EFI_INVALID_PARAMETER if input pointer NULL, 
        EFI_SUCCESS in all other cases.

**/

EFI_STATUS
ApGetSmmBase (
  IN VOID* pSmmBase
)
{
    if(pSmmBase == NULL) return EFI_INVALID_PARAMETER;
    if(IsItBsp()) return EFI_SUCCESS;
    
    *((UINTN*)pSmmBase) = ReadMsr(HYGON_MSR_SMM_BASE);
    return  EFI_SUCCESS;
}


/**
    Store S3 resume data in memory, and store its pointer in NVRAM.
    Halt All Aps

        
    @param VOID

         
    @retval VOID

**/
VOID SetCPUS3ResumeData(VOID)
{
    EFI_STATUS          Status;
    UINT8               i;
    UINT64              MsrSysCfg64;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;
    UINTN                       NumCpus=0;
    UINTN                       NumEnCpus=0;

    Status = pBS->LocateProtocol (
                        &gEfiMpServiceProtocolGuid,
                        NULL,
                        &MpServices );
    if (Status == EFI_SUCCESS){
        Status = MpServices->GetNumberOfProcessors (
                MpServices,
                &NumCpus,
                &NumEnCpus
                );
        for(i = 1; i < NumEnCpus; i++){
        // Execute on all running APs
            Status = MpServices->StartupThisAP(
                MpServices,                              // EFI_MP_SERVICES_PROTOCOL*
                (EFI_AP_PROCEDURE)
                ApGetSmmBase,                            // EFI_AP_PROCEDURE
                i,                                       // Processor number
                NULL,                                    // EFI_EVENT WaitEvent
                0,                                       // UINTN Timeout
                (void*)&(gCpuS3Resume->SmmBase.SmmBaseVal[i]), // VOID *ProcArguments
                NULL);                                  // BOOLEAN Finished
        }
    }
    // Get SmmBase for BSP
    gCpuS3Resume->SmmBase.SmmBaseVal[0]=ReadMsr(HYGON_MSR_SMM_BASE);

    gCpuS3Resume->Idt = pIDT;
    gCpuS3Resume->EnableC1E = gC1ESupport;

    MsrSysCfg64 = ReadMsr(MSR_SYS_CFG);
    WriteMsr(MSR_SYS_CFG, MsrSysCfg64 | (3 << MTRRFixDRAMEnBit));
    ASSERT(sizeof(MtrrRegs)/sizeof(UINT32) == NUM_OF_MTRR_SAVED);
    for(i = 0; i < NUM_OF_MTRR_SAVED; i++) {
        gCpuS3Resume->MtrrValue[i].Msr = MtrrRegs[i];
        gCpuS3Resume->MtrrValue[i].Value = (MtrrRegs[i]==MSR_SYS_CFG) ? MsrSysCfg64 : ReadMsr(MtrrRegs[i]);
    }
    WriteMsr(MSR_SYS_CFG, MsrSysCfg64);

    Status = pRS->SetVariable(
        gCpuS3ResumeVariable,
        &gCpuS3ResumeGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(UINT32),
        &gCpuS3Resume
    );
}

/**
    Write a value into the PCI register indicated

        
    @param Address 
    @param Value 
    @param ExtAccess 

         
    @retval VOID

**/
VOID HygonPciConfigWrite(
    IN UINT32 Address,
    IN UINT32 Value,
    IN BOOLEAN ExtAccess)
{
	UINT32  PciAddress = (1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );
    PciAddress = PciAddress & 0xfffffffc;
    if(ExtAccess)       //If extended config Access requested then add the extended config address  bit (24 to 27)
        PciAddress = PciAddress | 0x1000000;
    IoWrite32(0xcf8, PciAddress);  //Select the register to write
    IoWrite8(0xed,00);
    IoWrite8(0xed,00);
    IoWrite32(0xcfc, Value); //Write the Value
}

/**
    Read a value from the PCI register indicated

        
    @param Address 
    @param ExtAccess 

         
    @retval UINT32 Value

**/
UINT32 HygonPciConfigRead(
    IN UINT32 Address,
    IN BOOLEAN ExtAccess)
{
    UINT32  PciAddress = (1 << 31) +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Bus      << 16 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Device   << 11 )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Function << 8  )  +
        (((EFI_PCI_CONFIGURATION_ADDRESS*) &Address)->Register       );

    PciAddress = PciAddress & 0xfffffffc; //Dword Align

    if(ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        PciAddress = PciAddress | 0x1000000;

    IoWrite32(0xcf8, PciAddress);
    return IoRead32(0xcfc);         //Return the read value
}

/**
    Get CPU voltage

        
    @param cpuVid 

         
    @retval UINT32 Value

**/
UINT32 getCPUVoltage(
    IN UINT8 cpuVid)
{
    UINT32  Address32;
    UINT32  Buffer32;
    UINT32  v_x10000;
    BOOLEAN PVIFlag = FALSE;

    Address32 = (UINT32)HYGON_PCI_CFG_ADDRESS_S3 (0, (0), 
                HYGON_K8_MISC_FUNC, HYGON_K8_nF3_PWR_CTRL_MISC);
    Buffer32 = HygonPciConfigRead(Address32, FALSE );
    /* Check pvi/svi mode */
    if (Buffer32 & 0x00000100)
    {
        // PVI Mode PVIFlag = TRUE;
        /* Set cpuVid value in case CPU is in PVI mode */
        if (cpuVid > 0x5D)
            cpuVid = 0x3F;
        else if (cpuVid > 0x3E)
            cpuVid = cpuVid - 0x1F;
        else
            cpuVid = (cpuVid >> 1);

        /* PVI Encoding */
        if (cpuVid >= 0x20)
        {
            v_x10000 = 7625L - (125L * (cpuVid - 0x20));
        }
        else
        {
            v_x10000 = 15500L - (250L * cpuVid);
        }
    }
    else
    {
        //SVI Mode
        if (cpuVid > 0x7C)
            v_x10000 = 0;
        else
            v_x10000 = 15500L - (125L * cpuVid);
    }

    return v_x10000;
}



/**
    This is an initialization routine for BSP and all APs.
    BSP has CPU number 0, and APs > 1. After init, BSP number may change.

        
    @param Cpu - number associated with CPU.
    @param Context 

         
    @retval VOID

**/
VOID InitializeCpu(
    IN UINT8 Cpu,
    IN volatile VOID *Context)
{
    AMI_CPU_INFO    *AmiCpuInfo = &gAmiCpuInfo[Cpu];
    UINT32          CpuSignature;
    UINT64          CpuFeatureInfo;
    //Add for smee feature
    //UINT32          FeatureFlagsEax;
    //UINT8           RawBaseModel;
    //UINT8           RawExtModel;
    
    UINT32          FeatureFlagsEdx;
    UINT32          FeatureFlagsEcx;
    UINT32          RegEbx;
    CHAR8       *BrandString;
    UINT8       i; 
    UINT8       j;
    UINT8       Associativity;
    UINT64      TimerPeriod;
    BOOLEAN     IsFIDSupported = FALSE;
    BOOLEAN     IsVIDSupported = FALSE;
    EFI_STATUS  Status;
    REGISTER_TABLE          Reg;
    CACHE_DESCRIPTOR_INFO   *CacheInfo;
    CPUINFO         *CpuInfo = (CPUINFO*)&gCpuInfoHob->Cpuinfo[Cpu];
    UINT32 RegValue, CurFID, CurDID, CurVID;
    //UINT64                  Msr64Data;
    
    CPULib_LoadIdt(&pIDT);      //load idt address for each cpu in memory

    // Need semaphore lock here for allocating memory
    WaitForSemaphore(&gAmiCpuInfoSemaphore);
    
    // Set SVM and SVM Lock
    //if (gIsSetupDataValid)
    //{
    //    Msr64Data = ReadMsr(HYGON_MSR_SVM);
    //    Msr64Data &= ~0x0010;       // default is enabled
    //    if (gSetupData.SVM_MODE == 0)
    //    {
    //        Msr64Data |= 0x0010;
    //    }
    //    Msr64Data |= 0x08;          // Set SVM lock
    //    WriteMsr(HYGON_MSR_SVM, Msr64Data);

     //   //Hygon Ax series CPUs do not support SMEE  
     //   CPULib_CpuID(0x80000001, &FeatureFlagsEax, NULL, NULL, NULL);
     //   RawBaseModel = (UINT8)((FeatureFlagsEax & 0x000000F0ul)  >> 4);
     //   RawExtModel  = (UINT8)((FeatureFlagsEax & 0x000F0000ul)   >> 16);
        
     //   if ((RawExtModel << 4) + RawBaseModel >= 1) //CPU reverison >= B0
     //   {
     //       Msr64Data = ReadMsr(HYGON_MSR_SYS_CFG);
     //       Msr64Data &= ~0x800000;
            
     //       if (gSetupData.SMEE == 1)
     //       {
     //           Msr64Data |= 0x800000;
     //       }
    
     //       WriteMsr(HYGON_MSR_SYS_CFG, Msr64Data);
     //   }
     //   else
     //   {
     //       gSetupData.SMEE = 0;
     //       gSetupData.SmeeCap = 0; //make SMEE item gray in setup to avoid being changed
            
     //       //update setup_data
     //       Status = pRS->SetVariable(
     //           gSetupVariable,
     //           &gEfiSetupGuid,
     //           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
     //           sizeof(SETUP_DATA),
     //           &gSetupData
     //       );
            
     //       ASSERT_EFI_ERROR(Status);
     //   }
    //}
    CPULib_CpuID(1, &CpuSignature, &RegEbx,
                 &FeatureFlagsEcx, &FeatureFlagsEdx);

    CpuFeatureInfo = Shl64(FeatureFlagsEcx, 32) + FeatureFlagsEdx;

    CpuInfo->Valid = TRUE;
    CpuInfo->CpuSignature = CpuSignature;
    CpuInfo->CpuFeatureInfo = CpuFeatureInfo;
    CpuInfo->BIST = 0;

    // Get the CPU revision information from the scratch pad register
    CpuInfo->CpuRevision = (CpuSignature & 0xFF);
    CpuInfo->ApicId = *(UINT32*) (UINTN)(APIC_BASE + APIC_ID_REGISTER) >> 24;
    CpuInfo->ApicEId = 0;
    CpuInfo->ApicVer = (UINT8)*(UINT32*)(UINTN)(APIC_BASE + APIC_VERSION_REGISTER);


    // Allocate cache descriptor info structures for this CPU
    Status = pBS->AllocatePool(
        EfiBootServicesData, 
        (tCacheSize+1)* sizeof(CACHE_DESCRIPTOR_INFO),
        &CacheInfo );
    ASSERT_EFI_ERROR(Status);

    AmiCpuInfo->CacheInfo = (VOID*)CacheInfo;

    // Initialize Cache Descriptors
    for (i = 0; i < tCacheSize; i++)
    {
        if (i == 0 || (tCache[i].FCpuID != tCache[i-1].FCpuID)) 
        {
            CPULib_CpuID( tCache[i].FCpuID,
                &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
        }
        AmiCpuInfo->CacheInfo[i].Desc = 0xFF;  // non-zero value
        AmiCpuInfo->CacheInfo[i].Level = tCache[i].Level;
        AmiCpuInfo->CacheInfo[i].Type = tCache[i].Type;

        if (tCache[i].Level == EFI_CACHE_L3)
        {
            AmiCpuInfo->CacheInfo[i].Size =
                ( Reg[tCache[i].Register] >> tCache[i].SShift )
                & tCache[i].SMask;
            AmiCpuInfo->CacheInfo[i].Size = AmiCpuInfo->CacheInfo[i].Size / 2;  // size in MB;
        }
        else
            AmiCpuInfo->CacheInfo[i].Size =
            ( Reg[tCache[i].Register] >> tCache[i].SShift ) 
            & tCache[i].SMask;                          // size in KB

        // determine cache associativity
        Associativity = ( Reg[tCache[i].Register] >> tCache[i].AShift ) 
            & tCache[i].AMask;
        for (j = 0; j < tCache[i].ATable.size; j++)
        {
            if ( Associativity == tCache[i].ATable.Map[j].FieldValue )
            {
                AmiCpuInfo->CacheInfo[i].Associativity = 
                    tCache[i].ATable.Map[j].SMBiosValue;
                break;
            }
        }
        // default Associativity is EfiCacheAssociativityUnknown.
        if (j == tCache[i].ATable.size) 
            AmiCpuInfo->CacheInfo[i].Associativity 
                = EfiCacheAssociativityUnknown;

    }
    AmiCpuInfo->CacheInfo[i].Desc = 0;  // Null descriptor at end of list

    // Allocate and initialize brand string
    Status = pBS->AllocatePool(EfiBootServicesData, 49, &BrandString);
    ASSERT_EFI_ERROR(Status);
    AmiCpuInfo->BrandString = BrandString;

    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING0); BrandString += 8;
    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING1); BrandString += 8;
    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING2); BrandString += 8;
    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING3); BrandString += 8;
    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING4); BrandString += 8;
    *(UINT64*)BrandString = ReadMsr(HYGON_MSR_CPUID_NAME_STRING5); BrandString += 8;
    *BrandString = '\0';

    //Remove leading spaces. After removing leading spaces, the Brand String can not be
    //freed. However, it should never be freed.
    while (*AmiCpuInfo->BrandString == ' ') ++AmiCpuInfo->BrandString;

    CPULib_CpuID(1, &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
    AmiCpuInfo->Version = Reg[EAX];
    AmiCpuInfo->Features = Shl64(Reg[ECX], 32) + Reg[EDX];
    AmiCpuInfo->NumCores = (Reg[EDX] & (1 << 28)) ? ((Reg[EBX] >> 16) & 0xff) : 0;
    if(AmiCpuInfo->NumCores == 0)
    {
        CPULib_CpuID(HYGON_EXT_CPUID_8, &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
        AmiCpuInfo->NumCores = (UINT8)((Reg[ECX] & 0xff) + 1);
    }
    AmiCpuInfo->NumHts      = IsHtEnabled() * 2;    //Either 2 or 0.
    AmiCpuInfo->FSBFreq     = 0; //MHZ -- FSB not supported since its HT Speed.
    AmiCpuInfo->MicroCodeVers   = (UINT32)ReadMsr(HYGON_MSR_MCODE_SIGNATURE);

    CPULib_CpuID(HYGON_EXT_CPUID_0, &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
    if (Reg[EAX] >= HYGON_EXT_CPUID_7)
    {
        CPULib_CpuID(HYGON_EXT_CPUID_7, &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
        if (Reg[EDX] & BIT07)
        {                            // HWPState
            IsFIDSupported = TRUE;
            IsVIDSupported = TRUE;
        }
    }
    TimerPeriod = CalculateTimerPeriod();   //10^-15 s.
    AmiCpuInfo->ActualFreq  = 1000000000 / (UINT32)TimerPeriod;

//Fill this field with current voltage
    if ((IsFIDSupported) || (IsVIDSupported))
    {
        RegValue = (UINT32)ReadMsr(0xC0010064 + gNumBoostStates);
    }
  
    if (IsFIDSupported) {
        //Find the current pstate msr
        //Extract the fid and DID.
        //Frequency = ((fid+10h) * 200)/(2^DID)
        CurFID = (UINT8)RegValue & 0x3f;
        CurDID = (UINT8)(RegValue >> 6) & 7;
        CurDID = (UINT8)Shl64(1, CurDID); //2^DID
        AmiCpuInfo->IntendedFreq = (CurFID + 0x10);
        AmiCpuInfo->IntendedFreq = (AmiCpuInfo->IntendedFreq * 100 ) / CurDID;
    }
    if (IsVIDSupported) {
        CurVID = (UINT8) ((RegValue & 0x0000fe00) >> 9 );
        AmiCpuInfo->Voltage = getCPUVoltage(CurVID) / 10;    
    }    
    // Semaphore unlock here
    gAmiCpuInfoSemaphore = 0;


}



CPU_PREPARE_TO_BOOT_PROTOCOL	gCpuPrepareToBootProtocol = {SetCPUS3ResumeData}; 

AMI_CPU_INFO_PROTOCOL gAmiCpuInfoProtocol = {GetCpuInfo};

EFI_CPU_ARCH_PROTOCOL gEfiCpuArchProtocol = {
    EfiCpuFlushDataCache,
    EfiCpuEnableInterrupt,
    EfiCpuDisableInterrupt,
    EfiCpuGetInterruptState,
    EfiCpuINIT,
    EfiCpuRegisterInterruptHandler,
    EfiCpuGetTimerValue,
    EfiCpuSetMemoryAttributes,
    1,
    32
};


/**
    A routine to set MSR for BTS.

        
    @param Cpu 
    @param Context 

         
    @retval VOID

**/
VOID
SetHygonMSRRegforBTS(
IN UINT8 Cpu,
IN volatile VOID *Context)
{
    UINT64  NbCfg;
    UINT64  HWCR_Value;
    
    //Disable the Extended Config Space.
    NbCfg = ReadMsr(HYGON_MSR_NB_CFG);
    NbCfg = NbCfg & 0xFFFFBFFFFFFFFFFF;
    WriteMsr(HYGON_MSR_NB_CFG, NbCfg);

    //Enable the time stamp counter
    HWCR_Value = ReadMsr(HYGON_MSR_HWCR);
    HWCR_Value = (HWCR_Value | (1 << 24));
    WriteMsr(HYGON_MSR_HWCR, HWCR_Value);

}


/**
    Install Interrupt Handlers for 0 to 19, to the routine

        
    @param ImageHandle 
    @param SystemTable 

         
    @retval EFI_STATUS return the EFI status

**/

EFI_STATUS DxeInitializeCpu(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS          Status;
    EFI_HANDLE          Handle = 0;
    UINTN               VariableSize;
    VOID                *RuntimeGdt;
#if 0 //removed by GR-
    SETUP_CPU_FEATURES  SetupCpuFeatures;
#endif
    CPU_FEATURES        *CpuFeatures;
    EFI_EVENT           ReadyToBootEvent;
    EFI_EVENT           ExitBootServiceEvent;
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    EFI_EVENT           LegacyBootEvent;
#endif
    UINT64              HWCR_Value;
    UINT32              Attributes = 0;
//#if CORE_LEVELING_SUPPORT
    UINT32              Address32 = 0;
    UINT32              Buffer32 = 0;
    UINTN               i;
//#endif
    
    InitAmiLib(ImageHandle, SystemTable);
//EFI_DEADLOOP();
    
    PROGRESS_CODE(DXE_CPU_INIT);
    Status = pBS->LocateProtocol(&gEfiDataHubProtocolGuid, NULL, &gDataHub);

    VariableSize = sizeof(SETUP_DATA);
    Status = pRS->GetVariable(
        gSetupVariable,
        &gEfiSetupGuid,
        NULL,
        &VariableSize,
        &gSetupData
    );
    gIsSetupDataValid = !EFI_ERROR(Status);

    FirstHob = GetEfiConfigurationTable(SystemTable, &gEfiHobListGuid);
    if (!FirstHob) return EFI_INVALID_PARAMETER;	

    HWCR_Value = ReadMsr(HYGON_MSR_HWCR);
    HWCR_Value = (HWCR_Value | (1 << 24));
    WriteMsr(HYGON_MSR_HWCR, HWCR_Value);

    gCpuInfoHob = (CPUINFO_HOB*)FirstHob;

    //Find CPUINFO_HOB Hob.
    while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &gCpuInfoHob)))
    {
        if (guidcmp(&gCpuInfoHob->EfiHobGuidType.Name, &gAmiCpuinfoHobGuid) == 0) break;
    }
    if (EFI_ERROR(Status)) return Status;

    CpuFeatures = &gCpuInfoHob->CpuFeatures;

  
    TRACE((TRACE_ALWAYS, "DxeInitializeCpu: Before InitInterrupts\n"));
    Status = InitInterrupts();
    ASSERT_EFI_ERROR(Status);

    RuntimeGdt = CpyToRunTime(GDT, sizeof(GDT));
    pGdt.Limit = sizeof(GDT) - 1;
    pGdt.Base = (UINTN)RuntimeGdt;

    TRACE((TRACE_ALWAYS, "DxeInitializeCpu: Before CPULib_LoadGdt\n"));
    CPULib_LoadGdt(&pGdt);		//load gdt address for each cpu in memory

 
    ASSERT_EFI_ERROR(Status);

    Status =  EfiCpuRegisterInterruptHandler(NULL, 13, NULL);
    ASSERT_EFI_ERROR(Status);

    Status = EfiCpuRegisterInterruptHandler(0, 13, TempGPInterruptHandler);
    ASSERT_EFI_ERROR(Status);

    gNumCpus = StartAllAps(&gMpData);

    gCpuInfoHob->CpuCount = gNumCpus;
    
    for (i = 0; i < gNumCpus; i++) {
        gCpuInfoHob->Cpuinfo[i].Valid    = TRUE;
    }
    //Allocate memory for AMI_CPU_INFO. This will be filled by CPU initialization.
     Status = pBS->AllocatePool(
         EfiBootServicesData,
         sizeof(AMI_CPU_INFO) * gNumCpus,
         &gAmiCpuInfo
     );
    ExecuteFunctionOnRunningCpus(gMpData, TRUE, TRUE, InitializeCpu, NULL);
	InitializeFloatingPointUnits();

    Status = EfiCpuRegisterInterruptHandler(NULL, 13, NULL);
    ASSERT_EFI_ERROR(Status);

    Status = EfiCpuRegisterInterruptHandler(0, 13, InterruptHandlerHalt);	//Normal Handler.
    ASSERT_EFI_ERROR(Status);

    BoardCpuInit();             //Initialize any Board specific Cpu things.

#if 0 //removed by GR
    //This may be needed to be moved, to only show as supported if all are supported.
    MemSet(&SetupCpuFeatures, sizeof(SetupCpuFeatures), 0); //Just in case someone forgets to fill part of the structure.
    SetupCpuFeatures.HTAvailable = IsHtEnabled();
    SetupCpuFeatures.LimitCpuidAvailable = 1;               //Always available...C0010015 bit 11...used to restric limitcpuid
    SetupCpuFeatures.NodeCount = gCpuInfoHob->NodeCount;
    SetupCpuFeatures.DualNode = IsDualNode();
    SetupCpuFeatures.PopScktCount = SetupCpuFeatures.DualNode ? SetupCpuFeatures.NodeCount / 2 : \
                                    SetupCpuFeatures.NodeCount;

    Status = pRS->SetVariable(
        L"SetupCpuFeatures",
        &gEfiSetupGuid,
        EFI_VARIABLE_NON_VOLATILE
        | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(SETUP_CPU_FEATURES),
        &SetupCpuFeatures
    );
	ASSERT_EFI_ERROR(Status);
#endif

    //This is used by the event.
    //####4G+    Status = pBS->AllocatePool(EfiACPIMemoryNVS, sizeof(CPU_S3_RESUME), &gCpuS3Resume);
    gCpuS3Resume = (CPU_S3_RESUME*)0xFFFFFFFF;
    Status = pBS->AllocatePages(AllocateMaxAddress, EfiACPIMemoryNVS, EFI_SIZE_TO_PAGES(sizeof(CPU_S3_RESUME)),
                        (EFI_PHYSICAL_ADDRESS*)&gCpuS3Resume);
    ASSERT_EFI_ERROR(Status);
    //here

    Status = pBS->CreateEvent(
        EVT_SIGNAL_EXIT_BOOT_SERVICES,
        TPL_CALLBACK,
        CpuBootOnExitBootService,
        (VOID *)TRUE,
        &ExitBootServiceEvent
    );
    ASSERT_EFI_ERROR(Status);
    //In cases where core does not support a call back before option roms are executed.
    //The calling outside of SMM will be disabled before shell or a OS is loaded.
//    EFI_DEADLOOP();
    Status = CreateReadyToBootEvent(
        TPL_CALLBACK,
        ExecuteAtReadyToBoot,
        NULL,
        &ReadyToBootEvent
    );
    ASSERT_EFI_ERROR(Status);

#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    Status = CreateLegacyBootEvent(
            TPL_CALLBACK,
            CpuBootOnLegacyBoot,
            NULL,
            &LegacyBootEvent
        );
#endif
    Status = pBS->AllocatePool(EfiACPIMemoryNVS, sizeof (MTRR_SETTINGS), &mMtrrTable);
    
    Status = pBS->InstallMultipleProtocolInterfaces(
        &Handle,
       &gEfiCpuArchProtocolGuid,	&gEfiCpuArchProtocol,
        &gEfiMpServiceProtocolGuid,	gEfiMpServicesProtocol,
        &gAmiCpuInfoProtocolGuid, 	&gAmiCpuInfoProtocol,
        NULL);
    ASSERT_EFI_ERROR(Status);

// Commented call to SetHygonMSRRegforBTS, it was for Family15.
//###    ExecuteFunctionOnRunningCpus(gMpData, TRUE, TRUE, SetHygonMSRRegforBTS, NULL);


//Set Tom to 4GB as MMIO....pass memceil to agesa
//  BU code
    /*
    EfiCpuSetMemoryAttributes (
      NULL,
      0xE0000000,
      0x20000000,
      EFI_MEMORY_UC
      );
    */
    //VGA Enable
    //Disable vga enable for BU
   /* Address32 = 0xe00c0080;
    Buffer32 = 0x41;
    *(UINT32*)Address32 = (UINT32)Buffer32;
    */

    return EFI_SUCCESS;

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

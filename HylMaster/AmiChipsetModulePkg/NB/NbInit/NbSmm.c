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

/** @file NbSmm.c
    This file contains code for SMM access and control (both the
    protocol defined by Framework

**/


// Module specific Includes
#include "Efi.h"
#include "token.h"
#include <HOB.h>
#include <DXE.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>

// Used Protocols
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\MpService.h>

// Produced Protocols
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
#include <Protocol\SmmAccess2.h>
#else
#include <Protocol\SmmAccess.h>
#endif

#include "AmiCspLib.h"

// GUID Definitions

// Global variable declarations
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;
CPUINFO_HOB                     CpuInfoHob;

EFI_SMRAM_DESCRIPTOR gSmramMap[] = {
    {
    0,                                  //PhysicalStart <--To be filled in.
    0,                                  //CputStart
    1024 * 1024,                        //PhysicalSize < - Will be filled in.
    EFI_SMRAM_CLOSED                    //RegionState
    }
};

#define SMRAM_MAP_NUM_DESCRIPTORS (sizeof(gSmramMap)/sizeof(EFI_SMRAM_DESCRIPTOR))

EFI_HOB_HANDOFF_INFO_TABLE  *pHIT;

// Portable Constants

// Function Prototypes

// Function Definition

/**
    This function programs the NB Tseg Base.  

    @param VOID

    @retval EFI_SUCCESS Always

**/
EFI_STATUS
NBSMM_ProgramTsegBase (VOID)
{
    UINT64  qTsegAddress;

    WriteMsr (HYGON_MSR_SMM_ADDR_HL, CpuInfoHob.TsegAddress); // TSEG base

    // Program the TSEG size by programming mask register
    qTsegAddress = ReadMsr (HYGON_MSR_SMM_MASK_HL);
    qTsegAddress &= 0x1FFFF;            // Mask off unwanted bits
    qTsegAddress |= (~(UINT64)(CpuInfoHob.TsegSize - 1)) & 0xFFFFFFFFFFFE0000;
    qTsegAddress |= 0x00;
    qTsegAddress |= TSEG_CACHING_TYPE;  // Set Tseg cache type as WB=0x6000 or WT=0x4000
    WriteMsr (HYGON_MSR_SMM_MASK_HL, qTsegAddress);

    return EFI_SUCCESS;
}


/**
    This function programs the NB chipset registers to enable
    appropriate SMRAM area.  

    @param VOID

    @retval EFI_SUCCESS Always

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/

EFI_STATUS
NBSMM_EnableSMMAddress (VOID)
{
    UINT64                      qTsegAddress;
    EFI_STATUS                  Status;
    EFI_CPU_ARCH_PROTOCOL       Cpu;
    EFI_CPU_ARCH_PROTOCOL       *pCpu = &Cpu;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;

    // Currently TSEG address is assumed to be TOM - TSEG_SIZE
    // THIS CODE SHOULD BE CHANGED TO GET THE TSEG LOCATION FROM CPU INFO HOB
    qTsegAddress = pHIT->EfiMemoryTop;
#ifdef NB_DEBUG_MESSAGE
    TRACE((TRACE_ALWAYS, "TSEG Address %x.\n", qTsegAddress));
#endif
    gSmramMap[0].PhysicalStart	= gSmramMap[0].CpuStart = qTsegAddress;
    gSmramMap[0].PhysicalSize	= TSEG_SIZE;

    // Set TSEG memory attributes to uncached
    Status = pBS->LocateProtocol(&gEfiCpuArchProtocolGuid, NULL, &pCpu);
    if (EFI_ERROR(Status)) return Status;
    //TSEG as cacheable
/*  pCpu->SetMemoryAttributes(pCpu, qTsegAddress, 
                                TSEG_SIZE, EFI_MEMORY_UC);  // Uncached
*/
    // It is assumed 0xA0000 to 128K is already enabled
    // Program for TSEG start address
    CpuInfoHob.TsegAddress = qTsegAddress;
    CpuInfoHob.TsegSize = TSEG_SIZE;

    NBSMM_ProgramTsegBase ();           // For BSP

    // Execute on running APs
    Status = pBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT_EFI_ERROR(Status);

    MpServices->StartupAllAPs(
        MpServices,                                 // EFI_MP_SERVICES_PROTOCOL*
        (EFI_AP_PROCEDURE)NBSMM_ProgramTsegBase,    // EFI_AP_PROCEDURE
        FALSE,                                      // BOOLEAN SingleThread
        NULL,                                       // EFI_EVENT WaitEvent
        (UINTN)NULL,                                // UINTN Timeout
        (VOID*)NULL,                                // VOID *ProcArguments
        NULL);                                      // UINTN *FailedCPUList

    return EFI_SUCCESS;

}

EFI_STATUS
ApSetSmram(
    IN  VOID*   pSmramState
)
{
    UINT64  MtrrVal;

    if (ReadMsr(HYGON_MSR_HWCR) & 0x01)
    return EFI_ACCESS_DENIED;   //If device is locked, return error.

        // Enable RDMem & WRMem bits visible in fixed MTRR
    MtrrVal = ReadMsr (MSR_SYS_CFG);
    MtrrVal |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MtrrVal);

    if(*(UINT64*)pSmramState == EFI_SMRAM_OPEN){
        // Enable 0xA0000 read/write
        WriteMsr (MTRR_FIX_16K_A0000, 0x1C1C1C1C1C1C1C1C);
    }else if(*(UINT64*)pSmramState == EFI_SMRAM_CLOSED){
        // Disable 0xA0000 read/write
        WriteMsr (MTRR_FIX_16K_A0000, 0x0404040404040404);
    }

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MtrrVal = ReadMsr (MSR_SYS_CFG);
    MtrrVal &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MtrrVal);

    // Disable/Enable ASEG & TSEG SMRAM for VGA access
    MtrrVal = ReadMsr (HYGON_MSR_SMM_MASK_HL);
    if(*(UINT64*)pSmramState == EFI_SMRAM_OPEN){
        MtrrVal &= ~((UINT64)3);
    }else if(*(UINT64*)pSmramState == EFI_SMRAM_LOCKED){  // DEBUG - Changed from EFI_SMRAM_CLOSED <AAV> EIP157631 Implement caching improvments in SMM
        MtrrVal |= 3;
    }
    WriteMsr (HYGON_MSR_SMM_MASK_HL, MtrrVal);

    return	EFI_SUCCESS;
}

VOID
AllApSetSmram(
    IN  UINT64  SmramState
)
{
    EFI_STATUS                  Status;
    EFI_MP_SERVICES_PROTOCOL    *MpServices;

    Status = pBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
    ASSERT_EFI_ERROR(Status);

    MpServices->StartupAllAPs(
        MpServices,                                 // EFI_MP_SERVICES_PROTOCOL*
        (EFI_AP_PROCEDURE)ApSetSmram,               // EFI_AP_PROCEDURE
        TRUE,                                       // BOOLEAN SingleThread
        NULL,                                       // EFI_EVENT WaitEvent
        (UINTN)NULL,                                // UINTN Timeout
        (VOID*)&SmramState,                         // VOID *ProcArguments
        NULL); 
}

/**
    This function programs the NB chipset registers to open
    the SMRAM area.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Opened the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_ACCESS_DENIED SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If locked, return EFI_ACCESS_DENIED
                    4. Set Value for register to Open SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set OpenState to TRUE.
                    7. Set the RegionState to EFI_SMRAM_OPEN.
                    8. Return EFI_SUCCESS.
**/

EFI_STATUS NBSMM_OpenSmram (
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    IN EFI_SMM_ACCESS2_PROTOCOL      *This )
#else
    IN EFI_SMM_ACCESS_PROTOCOL      *This,
    IN UINTN                        DescriptorIndex )
#endif
{
    // All regions must open and close at the same time.
    UINT64  MTRRValue;

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION<0x0001000A)
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;
#endif

    if (ReadMsr(HYGON_MSR_HWCR) & 0x01)
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    // Enable 0xA0000 read/write
    WriteMsr (MTRR_FIX_16K_A0000, 0x1C1C1C1C1C1C1C1C);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    // Disable ASEG & TSEG SMRAM for VGA access
    MTRRValue = ReadMsr (HYGON_MSR_SMM_MASK_HL);
    MTRRValue &= ~((UINT64)3);
    WriteMsr (HYGON_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_OPEN);

    // Update appropriate flags
    This->OpenState = TRUE;
    gSmramMap[0].RegionState = EFI_SMRAM_OPEN;

    return EFI_SUCCESS;
}

/**
    This function programs the NB chipset registers to close
    the SMRAM area.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Closed the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_ACCESS_DENIED SMM area locked

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If locked, return EFI_ACCESS_DENIED
                    4. Set Value for register to close SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set OpenState to FALSE.
                    7. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                    8. Return EFI_SUCCESS.
**/

EFI_STATUS NBSMM_CloseSmram (
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    IN EFI_SMM_ACCESS2_PROTOCOL      *This )
#else
    IN EFI_SMM_ACCESS_PROTOCOL      *This,
    IN UINTN                        DescriptorIndex )
#endif
{
    // All regions must open and close at the same time.
    UINT64  MTRRValue;

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION<0x0001000A)
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;
#endif

    if (ReadMsr(HYGON_MSR_HWCR) & 0x01)
        return EFI_ACCESS_DENIED;   //If device is locked, return error.

    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    // Disable 0xA0000 read/write
    WriteMsr (MTRR_FIX_16K_A0000, 0x0404040404040404);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    // Enable ASEG & TSEG SMRAM for VGA access
//  MTRRValue = ReadMsr (HYGON_MSR_SMM_MASK_HL);
//  MTRRValue |= 3;
//  WriteMsr (HYGON_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_CLOSED);

    // Update appropriate flags
    This->OpenState = FALSE;
    gSmramMap[0].RegionState = EFI_SMRAM_CLOSED | EFI_ALLOCATED;

    return EFI_SUCCESS;
}


/**
    This function programs the NB chipset registers to lock
    the SMRAM area from opening/closing.  Only system level reset
    can unlock the SMRAM lock.

    @param This Pointer to the SMM access protocol
    @param DescriptorIndex Index of SMM Descriptor

           
    @retval EFI_SUCCESS Locked the SMM area.
    @retval EFI_INVALID_PARAMETER Descriptor doesn't exist.
    @retval EFI_DEVICE_ERROR SMM area is opened, need to be
        closed first before locking

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If invalid Descriptor return EFI_INVALID_PARAMETER.
                    2. Read NB SMM register.
                    3. If opened, return EFI_ACCESS_DENIED
                    4. Set Value for register to lock SMM area (0a0000 - bffff)
                    5. Write Register.
                    6. Set the RegionState to EFI_SMRAM_CLOSED & EFI_ALLOCATED
                    7. Return EFI_SUCCESS.
**/

EFI_STATUS NBSMM_LockSmram (
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    IN EFI_SMM_ACCESS2_PROTOCOL      *This )
#else
    IN EFI_SMM_ACCESS_PROTOCOL      *This,
    IN UINTN                        DescriptorIndex )
#endif
{
    UINT64  MTRRValue;
    UINT64  AsegStat;

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION<0x0001000A)
    //Both regions must open and close at the same time.
    if (DescriptorIndex >= SMRAM_MAP_NUM_DESCRIPTORS) 
        return EFI_INVALID_PARAMETER;
#endif

    AsmWbinvd();
    // Enable ASEG & TSEG SMRAM for VGA access
    MTRRValue = ReadMsr (HYGON_MSR_SMM_MASK_HL);
    MTRRValue |= 3;
    WriteMsr (HYGON_MSR_SMM_MASK_HL, MTRRValue);

    AllApSetSmram(EFI_SMRAM_LOCKED);
    // Enable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue |= ((UINT32)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    // Read 0xA0000 status
    AsegStat = ReadMsr (MTRR_FIX_16K_A0000);

    // Disable RDMem & WRMem bits visible in fixed MTRR
    MTRRValue = ReadMsr (MSR_SYS_CFG);
    MTRRValue &= ~((UINT64)1 << MTRRFixDRAMModEnBit);
    WriteMsr (MSR_SYS_CFG, MTRRValue);

    if (!(AsegStat & 0x1818181818181818))
        return EFI_DEVICE_ERROR;        // Region must be closed before locking

    // Lock the region
    //Errata 342
    //WriteMsr(HYGON_MSR_HWCR, (ReadMsr(HYGON_MSR_HWCR) | 0x01));

    // Update appropriate flags
    This->LockState = TRUE;
    gSmramMap[0].RegionState = EFI_SMRAM_LOCKED;

    return EFI_SUCCESS;
}

/**
    This function returns the current SMRAM area map information
    such as number of regions and its start address and size

    @param This Pointer to the SMM access protocol
    @param SmramMapSize Size of the SMRAM map buffer provided
    @param SmramMap Buffer to copy the SMRAM map information

           
    @retval EFI_SUCCESS Smram Map copied into buffer
    @retval EFI_BUFFER_TOO_SMALL Indicates that provided buffer is
        not enough SmramMapSize filled with required/updated size

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
                Here is the control flow of this function:
                    1. If Smram Map Size less than the actual map size, set 
                        the map size and return EFI_BUFFER_TOO_SMALL.
                    2. Copy the Smram Map descriptors into the supplied buffer.
                    3. Set the map size in *SmramMapSize, just in case is larger 
                        than the actual buffer.
                    4. Return EFI_SUCCESS.
**/

EFI_STATUS NBSMM_GetCapabilities (
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
#else
    IN EFI_SMM_ACCESS_PROTOCOL      *This,
#endif
    IN OUT UINTN                    *SmramMapSize,
    OUT EFI_SMRAM_DESCRIPTOR        *SmramMap )
{
    UINTN   i;

    if (*SmramMapSize < sizeof(gSmramMap)) {    //If input map size to small report the correct map size
        *SmramMapSize = sizeof(gSmramMap);      //and return error.
        return EFI_BUFFER_TOO_SMALL;
    }

    for(i=0;i<SMRAM_MAP_NUM_DESCRIPTORS;++i) 
        SmramMap[i] = gSmramMap[i];

    *SmramMapSize = sizeof(gSmramMap);          //Set the correct map size in case too large.
    return EFI_SUCCESS;
}

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
EFI_SMM_ACCESS2_PROTOCOL gSmmAccess = {
#else
EFI_SMM_ACCESS_PROTOCOL gSmmAccess = {
#endif
    NBSMM_OpenSmram,
    NBSMM_CloseSmram,
    NBSMM_LockSmram,
    NBSMM_GetCapabilities,
    FALSE,
    FALSE
};

/**
    This function is invoked from NB DXE to initialize SMM
    related stuff in NorthBridge and install appropriate
    SMM protocols such as SMM Access & SMM Control

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS Return Status based on errors that occurred while waiting for
        time to expire.

**/

EFI_STATUS
NBSMM_Init (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    EFI_STATUS  Status = EFI_SUCCESS;

    pHIT = GetEfiConfigurationTable(SystemTable, &gEfiHobListGuid);

    // Enable SMM address range by programming appropriate chipset registers
    NBSMM_EnableSMMAddress ();
#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x0001000A)
    return pBS->InstallMultipleProtocolInterfaces(
                        &ImageHandle,
                        &gEfiSmmAccess2ProtocolGuid, &gSmmAccess,
                        NULL, NULL);
#else
    return pBS->InstallMultipleProtocolInterfaces(
                        &ImageHandle,
                        &gEfiSmmAccessProtocolGuid, &gSmmAccess,
                        NULL, NULL);
#endif
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

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

/** @file NbPei.c
    This file contains code for HYGON Northbridge initialization
    in the PEI stage

**/


// Module specific Includes
//----------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <StatusCodes.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <AmiCspLibInc.h>
#include "Nb.h"
#include <CPU.h>
#include <CpuCspLib.h>
#include "MemErrorReportPpi.h"
#include "Ppi/PCICfg2.h"
#include "Ppi/CpuIo.h"
#include "Ppi/NbPpi.h"
#include "Ppi/CspLibPpi.h"                                 
#include <AmiChipsetIoLib.h>
//----------------------------------------------------------------------------

// Produced PPIs


//----------------------------------------------------------------------------
static AMI_PEI_NBINIT_POLICY_PPI mAMIPEINBInitPolicyPpi =
    {
        TRUE
    };

// PPI that are installed
static EFI_PEI_PPI_DESCRIPTOR mPpiList[] =
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
       &gAmiPeiNbInitPolicyGuid,
       &mAMIPEINBInitPolicyPpi
    };

//Get memory error ppi
PEI_MEMORY_ERROR_REPORT_PPI  mPeiMemoryErrorReportPpi = {
            //PPI data object;
            MemoryErrorRead
            };

//
// Define the list of Memory PPI
//
static EFI_PEI_PPI_DESCRIPTOR mMemErrPpi[] = {
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
      &gAmiMemoryErrorReportPpiGuid,
      &mPeiMemoryErrorReportPpi
    }
  };

//----------------------------------------------------------------------------

/**
    This function is the entry point for this PEI. This function
    initializes the NB

    @param FileHandle Handle of the file being invoked
    @param PeiServices Pointer to the PEI services table

    @retval EFI_STATUS Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function should initialize North Bridge for memory detection.
              Install AMI_PEI_NBINIT_POLICY_PPI to indicate that NB Init PEIM
              is installed
              Following things can be done at this point:
                  - Enabling top of 4GB decode for full flash ROM
                  - Programming North Bridge ports to enable access to South
                      Bridge and other I/O bridge access

**/

EFI_STATUS
EFIAPI
NbPeiInit (
  IN EFI_PEI_FILE_HANDLE      FileHandle,
  IN EFI_PEI_SERVICES          **PeiServices
  )
{
    EFI_STATUS                  Status;
    EFI_HOB_CPU                 *pCpu = NULL;
    BOOLEAN                     InterruptState;
    //EFI_PEI_PCI_CFG2_PPI        *PciCfg = NULL;
    //EFI_PEI_CPU_IO_PPI          *CpuIo = NULL;
    UINT32                      RegEax = 0;
    UINT32                      RegEbx = 0;
    UINT32                      RegEcx = 0;
    UINT32                      RegEdx = 0;
    //UINT32                      Value32 = 0;
    //UINT8                       Value8 = 0;
    PEI_PROGRESS_CODE(PeiServices, PEI_CAR_NB_INIT);

    InterruptState = CPULib_GetInterruptState();
    CPULib_DisableInterrupt();

    // Create CPU HOB with appropriate memory space size and IO space size
    Status = (*PeiServices)->CreateHob(PeiServices, EFI_HOB_TYPE_CPU, sizeof(EFI_HOB_CPU), &pCpu);
    if (Status == EFI_SUCCESS) {
//CPUID HYGON_EXT_CPUID_8 EAX BIT[0:7]
		CPULib_CpuID(HYGON_EXT_CPUID_8, &RegEax, &RegEbx, &RegEcx, &RegEdx);


        pCpu->SizeOfMemorySpace = (RegEax & 0xFF);
        pCpu->SizeOfIoSpace = 16;
        pCpu->Reserved[0] = pCpu->Reserved[1] = pCpu->Reserved[2] = pCpu->Reserved[3] =
                            pCpu->Reserved[4] = pCpu->Reserved[5] = 0;
    }

    // Get pointer to the PCI config PPI
    //PciCfg = (*PeiServices)->PciCfg;
    //CpuIo = (*PeiServices)->CpuIo;

    
    // Program NB Devices' Subsystem Vendor ID & Subsystem ID

    // Install the NB Init Policy PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);
    PEI_TRACE((TRACE_PEI_CHIPSET, PeiServices, "Done NBPEI_Init\n"));

    Status = (*PeiServices)->InstallPpi(PeiServices, &mMemErrPpi[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);

    if (InterruptState) CPULib_EnableInterrupt();
    return EFI_SUCCESS;
}


/**
    This function reports the error status after memory training.

           
    @param PeiServices PEI Services table pointer
    @param This Pointer to the PPI structure
    @param MemErrData Pointer to error data buffer

         
    @retval EFI_STATUS return EFI status

**/

EFI_STATUS
MemoryErrorRead (
  IN      EFI_PEI_SERVICES      **PeiServices,
  IN      PEI_MEMORY_ERROR_REPORT_PPI      *This,
  IN OUT  UINT32                MemErrData
  )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    if (MemErrData & NO_DIMM_FOUND)
    {
        //If DIMM NotFound, check if there is any useful memory. If not report it.    
        if (MemErrData & MEMORY_NONE_USEFUL)
            PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NONE_USEFUL, EFI_ERROR_MAJOR);
        //Else, check if there is memory error and report it.    
        else if (MemErrData & MEMORY_ERROR)
            PEI_ERROR_CODE(PeiServices, PEI_MEMORY_ERROR, EFI_ERROR_MAJOR);
        else
            PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NOT_DETECTED, EFI_ERROR_MAJOR);
    }
    else if (MemErrData & MEMORY_NONE_USEFUL)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NONE_USEFUL, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_ERROR)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_ERROR, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_SPD_FAIL)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_SPD_FAIL, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_INVALID_TYPE)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_INVALID_TYPE, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_INVALID_SPEED)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_INVALID_SPEED, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_INVALID_SIZE)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_INVALID_SIZE, EFI_ERROR_MAJOR);
    else if (MemErrData & MEMORY_MISMATCH)
        PEI_ERROR_CODE(PeiServices, PEI_MEMORY_MISMATCH, EFI_ERROR_MAJOR);
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

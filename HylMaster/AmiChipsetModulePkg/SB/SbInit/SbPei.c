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
/** @file SbPei.c
    This file contains code for Template Southbridge initialization
    in the PEI stage

**/
//*************************************************************************

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include "SB.h"
#include "AmiCspLib.h"
#include <Hob.h>
#include <AmiChipsetIoLib.h>
// Produced/used PPI interfaces
#include <Ppi/PciCfg2.h>
#include <Ppi/SbPpi.h>
#include <Ppi/CpuIo.h>
#include <Ppi/CspLibPpi.h>
#include <Ppi/SmmControl.h>
#if CAPSULE_SUPPORT
#include "AmiCspLib.h"
#endif
#if ATAPI_RECOVERY_SUPPORT
#include <Ppi/AtaController.h>
#endif

#include <Ppi/HygonSbWrapper.h>
#include <Ppi/AmdFchInitPpi.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Pci.h> //##<AAV> use the definition of MdePkg later.
// Portable Constants
AMI_SB_PCI_SSID_TABLE_STRUCT gDefaultSIdTbl[] = {SB_PCI_DEVICES_SSID_TABLE};

// Produced PPIs

// GUID Definitions

// Portable Constants

//----------------------------------------------------------------------------
// Function Prototypes
//----------------------------------------------------------------------------
EFI_STATUS SBPEI_Board(EFI_FFS_FILE_HEADER*, EFI_PEI_SERVICES**);

VOID ProgramSBSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

EFI_STATUS
ImcFanInitCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
);

EFI_STATUS
sbEndOfFchCallBack(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

EFI_STATUS
SbEndOfPeiCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

EFI_STATUS
SbAgesaCallback(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );


#if defined (XHCI_RECOVERY_PATH_MODE) && (XHCI_RECOVERY_PATH_MODE)
EFI_GUID gXhciRecoveryFwGuid = XHCI_RECOVERY_FIRMWARE_GUID;
#endif


EFI_STATUS
UpdateBootMode(
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI  *PciCfg
    );

BOOLEAN
IsRecovery (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI  *PciCfg,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo
    );

#if ATAPI_RECOVERY_SUPPORT
EFI_STATUS
EFIAPI SBPEI_EnableAtaChannelForRecovery (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_ATA_CONTROLLER_PPI     *This,
  IN UINT8                      ChannelIndex
  );
#endif

#if     SMM_SUPPORT
EFI_STATUS
SBPEI_ActivateSMI (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_SMM_CONTROL_PPI *This,
    IN OUT INT8 *ArgumentBuffer OPTIONAL,
    IN OUT UINTN *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN Periodic OPTIONAL,
    IN UINTN ActivationInterval OPTIONAL
    );

EFI_STATUS
SBPEI_DeactivateSMI (
    IN EFI_PEI_SERVICES **PeiServices,
    IN PEI_SMM_CONTROL_PPI *This,
    IN BOOLEAN Periodic OPTIONAL
    );
#endif

EFI_STATUS SBPEI_Init_AfterMemInstalled (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

#ifdef XHCI_Firmware_Support
#if XHCI_Firmware_Support
EFI_STATUS
sbFchRecoveryCallBack(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );
#endif
#endif

BOOLEAN
SbGetCapsuleFlag (
  );

VOID
SbWakeupTypeLibConstructor (
  VOID
  );

//----------------------------------------------------------------------------
// Define everything related to RESET PPI here (function is defined at the end)
//----------------------------------------------------------------------------
#if SB_RESET_PPI_SUPPORT

#include <ppi\reset.h>
extern VOID SBLib_ResetSystem (IN EFI_RESET_TYPE ResetType);

EFI_STATUS SBPEI_ResetSystem (IN EFI_PEI_SERVICES **PeiServices);

EFI_GUID gPeiResetPpiGuid = EFI_PEI_RESET_PPI_GUID;

static  EFI_PEI_RESET_PPI mResetPpi =
    {
        SBPEI_ResetSystem
    };

#endif

#if SB_STALL_PPI_SUPPORT
#include "ppi\stall.h"

extern EFI_STATUS CountTime (IN UINTN DelayTime, IN UINT16 BaseAddr);

EFI_STATUS  SBPEI_Stall (IN EFI_PEI_SERVICES **PeiServices,
                        IN EFI_PEI_STALL_PPI *This, IN UINTN  Microseconds);

// Constants related to template Stall code
#define TIMER_RESOLUTION  1

static EFI_PEI_STALL_PPI mStallPpi =
    {
        TIMER_RESOLUTION,
        SBPEI_Stall
    };


#endif

// PPI interface definition

// PPI Definition
static  AMI_PEI_SBINIT_POLICY_PPI mAMIPEISBInitPolicyPpi =
    {
        TRUE
    };

static  AMI_PEI_SBINIT_POLICY_PPI mAtiPEISBInitPolicyPpi =
{
        TRUE
};

static EFI_PEI_PPI_DESCRIPTOR mBootModePpi[] =
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMasterBootModePpiGuid,
    NULL
  };

static EFI_PEI_PPI_DESCRIPTOR mRecoveryModePpi[] =
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiBootInRecoveryModePpiGuid,
    NULL
  };
#if SMM_SUPPORT
static PEI_SMM_CONTROL_PPI mSmmControlPpi = {
    SBPEI_ActivateSMI,
    SBPEI_DeactivateSMI
};
#endif

#if ATAPI_RECOVERY_SUPPORT
static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi =
{
    SBPEI_EnableAtaChannelForRecovery
};
#endif

// PPI that are installed
static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
    // PPIs will be install after HYGON SB init.
#if SMM_SUPPORT
    {EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiSmmControlPpiGuid,
    &mSmmControlPpi},
#endif

#if ATAPI_RECOVERY_SUPPORT
    {EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiAtaControllerPpiGuid,
    &mAtaControllerPpi},
#endif

    {(EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiSbInitPolicyGuid,
    &mAMIPEISBInitPolicyPpi}
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList2[] = {
    {(EFI_PEI_PPI_DESCRIPTOR_PPI
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiStallPpiGuid,
    &mStallPpi}
};

static EFI_PEI_NOTIFY_DESCRIPTOR  NotifyList[] =
{
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK),
    &gEfiEndOfPeiSignalPpiGuid,
    SbEndOfPeiCallback},
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
        | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdFchInitPpiGuid,
    sbEndOfFchCallBack}
};

static EFI_PEI_NOTIFY_DESCRIPTOR MemInstalledNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiMemoryDiscoveredPpiGuid, SBPEI_Init_AfterMemInstalled },
};

//=============================================================================
// Function Definition
//=============================================================================

/**
    This function is the entry point for this PEI. This function
    initializes the chipset SB

    @param FfsHeader Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  This function should initialize South Bridge for memory detection.
              Install AMI_PEI_SBINIT_POLICY_PPI to indicate that SB Init PEIM
              is installed
              Following things can be done at this point:
                  - Enabling top of 4GB decode for full flash ROM
                  - Programming South Bridge ports to enable access to South
                      Bridge and other I/O bridge access

**/
EFI_STATUS
EFIAPI
SbPeiInit (
  IN EFI_PEI_FILE_HANDLE   FileHandle,	
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
    EFI_STATUS          Status;

    PEI_TRACE((-1, PeiServices, "AMI SB PEIM start\n"));

    SbWakeupTypeLibConstructor();

    SBPEI_Board(FileHandle, PeiServices);

    Status = (**PeiServices).NotifyPpi (PeiServices, &NotifyList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);

    {
    UINT8 DebBuf;
    UINT32 TpmEnableReg;
    //  Enable FCH ACPI MMIO
    //  The reason why coding here is SMBUS PPI should be available before AmdInitEarly.
    //  But the SMBUS PEIM will read SMBUS base from FCH ACPI MMIO.
    //  We should remove if HYGON-FCH has generic solution.
    IoWrite8(PM_IO_INDEX,FCH_PMIOA_REG04);
    DebBuf = IoRead8(PM_IO_DATA) | BIT01;
    IoWrite8(PM_IO_DATA, DebBuf);

    //  Enable FCH Cf9IoEn. (EIP172271)
    SET_IO8_PMIO(FCH_PMIOA_REG00, BIT01);
    // Enable TPM related decode.
    TpmEnableReg = READ_PCI32_SB(FCH_LPC_REG7C);
    TpmEnableReg |= ( BIT0 + BIT2 );
    WRITE_PCI32_SB(FCH_LPC_REG7C, TpmEnableReg);
    
    }
//Program secondary and suboardinage bus numers for enabling SATA behind the bridge b0|d8|f1
//WRITE_PCI32(0,8,1, 0x19, 0x12);
//WRITE_PCI32(0,8,1, 0x1A, 0x12);



#if HYGON_SB_SIO_PME_BASE_WORKAROUND //1 //###defined(SIO_SUPPORT) && SIO_SUPPORT
    {
    EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyListForSio;
    UINTN                           SioPmeBase;

    SioPmeBase = (UINTN)READ_PCI32_SB(0x64);
    if (SioPmeBase != SB_SIO_PME_BASE_ADDRESS && SioPmeBase != 0) {
   
        Status = (*PeiServices)->AllocatePool (PeiServices, \
                                                sizeof(EFI_PEI_NOTIFY_DESCRIPTOR) * 2,
                                                &NotifyListForSio
                                                );
        ASSERT_PEI_ERROR (PeiServices, Status);
        NotifyListForSio[0].Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
        NotifyListForSio[0].Guid = &gHygonFchAmiCallOutPpiGuid;
        NotifyListForSio[0].Notify = SbAgesaCallback;
        NotifyListForSio[1].Flags = SioPmeBase;
        Status = (**PeiServices).NotifyPpi (PeiServices, &NotifyListForSio[0]);
        ASSERT_PEI_ERROR(PeiServices, Status);
    }
    }
#endif

#if SB_STALL_PPI_SUPPORT
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList2[0]); //Install Stall PPi
    ASSERT_PEI_ERROR(PeiServices, Status);
#endif

    // Enable ACPI MMIO space
	//Check this
  //  SET_IO8_PMIO(FCH_PMIOA_REG04, BIT1);
    
    // Due to the workaround of HYGON reference source (Disabling the RTC Daylight Saving Time Feature v2.0) (FchInitResetHwAcpi)
    // the setting about "DISABLE_DAYLIGHT_SAVINGS" move to (sbEndOfFchCallBack)

    PEI_TRACE((-1, PeiServices, "End of AMI SB PEIM  \n"));
    return EFI_SUCCESS;
}

/**
    This function programs SB PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices Pointer to the PEI services table
    @param PciCfg Pointer to the PCI Configuration PPI

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

    @note  1. This routine only programs the PCI device in SB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a SB PCI device or not.
              2. This routine is invoked by PEI phase.

**/

VOID ProgramSBSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI      *PciCfg )
{
    EFI_STATUS                  	Status;
    AMI_PEI_SB_CUSTOM_PPI       	*SBPeiOemPpi;
    AMI_SB_PCI_SSID_TABLE_STRUCT 	volatile *SsidTblPtr = gDefaultSIdTbl;

    Status = (*PeiServices)->LocatePpi( PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SBPeiOemPpi );

    if (Status == EFI_SUCCESS) {
        if (SBPeiOemPpi->SsidTable != NULL) {
            SsidTblPtr = SBPeiOemPpi->SsidTable;
		#if defined(DYNAMIC_PCIE_SSID_SUPPORT) && (DYNAMIC_PCIE_SSID_SUPPORT == 1)
            {
				UINT32		TblSize;
				UINT32		volatile i;
				for (i = 0; ((SsidTblPtr + i)->PciAddr != 0xFFFFFFFFFFFFFFFF); i++) {}
				TblSize = sizeof(AMI_SB_PCI_SSID_TABLE_STRUCT) * (i + 1);
				SsidTblPtr = (AMI_SB_PCI_SSID_TABLE_STRUCT *)AllocateZeroPool(TblSize);
				CopyMem(SsidTblPtr, SBPeiOemPpi->SsidTable, TblSize);
            }
		#endif
        }
    }

    // Due to the SSID of SB need porting in DXE phase(AGESA init mid),
    // Using the PCD to pass related information.
	PcdSet32(PcdSbSsidTablePtr, (UINT32)SsidTblPtr);
}

/**
    This function can be used to program any SB regisater after
    PEI permantent memory be installed.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS SBPEI_Init_AfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    //EFI_PEI_CPU_IO_PPI          *CpuIo;
    EFI_PEI_PCI_CFG2_PPI     	*PciCfg;
    EFI_BOOT_MODE           	BootMode;

    //CpuIo = (*PeiServices)->CpuIo;
    PciCfg = (*PeiServices)->PciCfg;

    PEI_PROGRESS_CODE (PeiServices, PEI_MEM_SB_INIT);

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (BootMode != BOOT_ON_S3_RESUME) {
        //
        // Program SB Devices' Subsystem Vendor ID & Subsystem ID
        //
        ProgramSBSubId( PeiServices, PciCfg );
    }

    return  Status;
}



/**
    This function determines if the previous shut down
    was caused by PwrButton.

    @param VOID

    @retval TRUE If it is Powerbutton Shutdown
    @retval FALSE if it is not a Powerbutton Shutdown

**/
BOOLEAN isPowerButtonShutdown(VOID)
{
    BOOLEAN     IsPwrButtonShutdown = FALSE;
    UINT8       PMxC4;

    // Select PMxC0 [S5/Reset Status] to be S5/Reset Status register.
    PMxC4 = READ_IO8_PMIO(FCH_PMIOA_REGC4);
    RW_IO8_PMIO(FCH_PMIOA_REGC4,0,BIT2);

    // BIT2: the previous shut down was caused by PwrButton.
    if (READ_IO8_PMIO(FCH_PMIOA_REGC0) & BIT2 ) IsPwrButtonShutdown = TRUE;

    // Restore value of PMxC4
    WRITE_IO8_PMIO(FCH_PMIOA_REGC4, PMxC4);

    return IsPwrButtonShutdown;
}

#if KBC_SUPPORT && Recovery_SUPPORT

#define KBC_IO_DATA             0x60    // Keyboard Controller Data Port
#define KBC_IO_STS              0x64    // Keyboard Controller Status Port

#define IO_DELAY_PORT           0xed    // Use for I/O delay

/**
    This function resets Keyboard controller for Ctrl-Home
    recovery function.

    @param PeiServices Pointer to the Pei Services function and
        data structure
    @param CpuIo Pointer to the CPU I/O PPI
    @param PciCfg Pointer to the PCI Configuration PPI

    @retval VOID

    @note  No porting required.
**/

VOID ResetKbc (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfg )

{
    volatile UINT8      KbcSts = 0;
    volatile UINT8      Buffer8;
    UINT32              TimeOut = 0x100;

    // Reset KBC
    if (CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_STS ) != 0xff) {
        // Clear KBC buffer
        do {
            Buffer8 = CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_DATA );
            KbcSts = CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_STS ); // 0x64
            TimeOut--;
        } while ((KbcSts & 3) && (TimeOut != 0));


        // Send BAT command
        CpuIo->IoWrite8( PeiServices, CpuIo, KBC_IO_STS, 0xaa ); // 0x64

        // IBFree
        for (TimeOut = 0; TimeOut < 0x1000; TimeOut++) {
            CpuIo->IoWrite8( PeiServices, CpuIo, IO_DELAY_PORT, KbcSts );
            KbcSts = CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_STS ); // 0x64
            if ((KbcSts & 2) == 0) break;
        }

        // OBFree
        for (TimeOut = 0; TimeOut < 0x500; TimeOut++) {
            CpuIo->IoWrite8( PeiServices, CpuIo, IO_DELAY_PORT, KbcSts );
            KbcSts = CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_STS ); // 0x64
            if (KbcSts & 1) break;
        }

        // Get result if needed
        if (KbcSts & 1)
            Buffer8 = CpuIo->IoRead8( PeiServices, CpuIo, KBC_IO_DATA );
    }

    // Clear KBC status buffer.
    KbcSts = CpuIo->IoRead8 ( PeiServices, CpuIo, KBC_IO_STS ); // 0x64
}
#endif

/**

    Check to see if CMOS is bad

    @param None

    @retval TRUE  - CMOS is bad
    @retval FALSE - CMOS is good

**/
#if CLEAR_NVRAM_IF_CMOS_BAD
BOOLEAN
IsCmosBad(
     VOID
)
{		
	IoWrite8(CMOS_BANK1_INDEX, CMOS_BAD_CHECK_ADDRESS);
	if (IoRead8(CMOS_BANK1_DATA) ==0x55)  return TRUE;
	
	return FALSE;
}
#endif

/**
    This function determines the boot mode of the system.  After
    the correct boot mode has been determined, the PEI Service
    function SetBootMode is called and then the
    MasterBootModePpi is installed

    @param PeiServices Pointer to the Pei Services data structure
    @param CpuIo Pointer to the CPU I/O PPI
    @param PciCfg Pointer to the PCI Config PPI

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
UpdateBootMode(
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI  *PciCfg
    )
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_BOOT_MODE       HygonBootMode, SbBootMode;

    // EFI_PEI_MASTER_BOOT_MODE_PEIM_PPI will be handle by HYGON reference code
    //   (BOOT_ON_S4_RESUME/BOOT_ON_S3_RESUME/BOOT_WITH_FULL_CONFIGURATION).
    // We handle recovery mode and other exception.
    Status = (*PeiServices)->GetBootMode (PeiServices, &HygonBootMode);
    if (EFI_ERROR(Status)) SbBootMode = BOOT_WITH_FULL_CONFIGURATION;

    SbBootMode = HygonBootMode;
    if (SbBootMode == BOOT_ON_S4_RESUME || SbBootMode == BOOT_ON_S3_RESUME)
    {
        // the previous shut down was caused by PwrButton not sleep.
        if (isPowerButtonShutdown()) SbBootMode = BOOT_WITH_FULL_CONFIGURATION;
    }

    // Returns 0 if no S3 resume detected returns -1 if this is an S3 boot
    if (SbBootMode != BOOT_ON_S3_RESUME)
    {
        #if KBC_SUPPORT && Recovery_SUPPORT
        ResetKbc(PeiServices,  CpuIo, PciCfg);
        #endif
    }
 
#if CLEAR_NVRAM_IF_CMOS_BAD
    if (IsCmosBad()){
        SbBootMode = BOOT_WITH_DEFAULT_SETTINGS;
    }
#endif

#if defined (FLASH_UPDATE_BOOT_SUPPORTED) && (FLASH_UPDATE_BOOT_SUPPORTED)
    if (SbGetCapsuleFlag()) {
#if defined(CAPSULE_RESET_MODE) && CAPSULE_RESET_MODE == 0
 if (SbBootMode == BOOT_ON_S3_RESUME)
#endif
    	SbBootMode = BOOT_ON_FLASH_UPDATE;
    }
#endif

#if CAPSULE_SUPPORT
    if(!EFI_ERROR(CheckIfCapsuleAvailable()))
        SbBootMode = BOOT_ON_FLASH_UPDATE;
#endif

#if FORCE_RECOVERY
    SbBootMode = BOOT_IN_RECOVERY_MODE;
#endif //FORCE_RECOVERY

    if(SbBootMode != BOOT_ON_S4_RESUME && SbBootMode != BOOT_ON_S3_RESUME){
        RESET_IO16_PM(ACPI_IOREG_PM1_CNTL, 0x1C01); // Clear the SLP_TYP and SCI_EN of PmControl
    }

    // Set the system BootMode
    if (HygonBootMode != SbBootMode) {
        Status = (*PeiServices)->SetBootMode(PeiServices, SbBootMode);
        ASSERT_PEI_ERROR (PeiServices, Status);
    }

/*    DEBUG_CODE (
        switch (SbBootMode) {
            case BOOT_ON_FLASH_UPDATE:
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_ON_FLASH_UPDATE \n"));
                break;
            case BOOT_IN_RECOVERY_MODE:
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_IN_RECOVERY_MODE \n"));
                break;
            case BOOT_ON_S3_RESUME:
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_ON_S3_RESUME \n"));
                break;
            case BOOT_ON_S4_RESUME:
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_ON_S4_RESUME \n"));
                break;
            case BOOT_ON_S5_RESUME:
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_ON_S5_RESUME \n"));
                break;
            default :
                PEI_TRACE((-1, PeiServices, "Boot mode is BOOT_WITH_FULL_CONFIGURATION \n"));
                break;
        }
    )
*/
    // Let everyone know that boot mode has been determined by installing the
    //  MasterBootMode PPI
    (*PeiServices)->InstallPpi(PeiServices, mBootModePpi);

    // Recovery Boot Mode PPI
    if (SbBootMode == BOOT_IN_RECOVERY_MODE)
        Status = (*PeiServices)->InstallPpi( PeiServices, mRecoveryModePpi );

    return Status;
}

//----------------------------------------------------------------------------


#if SB_RESET_PPI_SUPPORT

/**
    This function is the reset call interface function published
    by the reset PPI

    @param PeiServices Pointer to the PEI services table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SBPEI_ResetSystem (
    IN EFI_PEI_SERVICES **PeiServices
)
{

    SBLib_ResetSystem(SB_COLD_RESET);

    // We should never get this far
    return EFI_SUCCESS;
}

#endif


#if SB_STALL_PPI_SUPPORT

/**
    This function is used to stall the boot process (provides delay)
    for specified number of microseconds

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the Stall PPI
    @param MicroSeconds Time to wait for (in Micro seconds)

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SBPEI_Stall (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_STALL_PPI    *This,
    IN UINTN                MicroSeconds)
{
    EFI_STATUS              Status = EFI_UNSUPPORTED;

    // #### <AAV> to be check further ###
    // CPM will call Stall-PPI before AmdProccessInitPeim
    //	so the PM-IO didn't decode. we will get error.
    //  the PM-IO decoded will be handle by HYGON reference.
    //  we didn't decode it here because of we don't want to change the register behavior.
    //  instead we use the IO delay when the PM-timer didn't get ready.
    if (IoRead16(PM_BASE_ADDRESS) == 0xFFFF) {
		UINTN                   uSec = 0;
		uSec = MicroSeconds / 4;
		while (uSec != 0) {
		    IoRead8(0x80);
			uSec--;
		}

		Status = EFI_SUCCESS;
    }

    if (EFI_ERROR(Status)) {
		// At this time no manipulation needed.  The value passed in is in
		//  MicroSeconds(us) and that is what the library function uses

		// Call Library function that is shared with Metronome
		//  Architecture Protocol

		Status = CountTime(MicroSeconds, PM_BASE_ADDRESS);
    }

    return Status;
}

#endif

#if ATAPI_RECOVERY_SUPPORT

#define     TEMP_BASE       0xF000
#define     TEMP_BASE2      0xE000

EFI_GUID gIdeRecoveryNativeModePpiGuid = PEI_IDE_RECOVERY_NATIVE_MODE_PPI_GUID;

PEI_IDE_RECOVERY_NATIVE_MODE_PPI IdeRecoveryNativeModePpi = {
   TEMP_BASE + 0x100,
   TEMP_BASE + 0x202,
   TEMP_BASE + 0x300,
   TEMP_BASE + 0x402
};

EFI_PEI_PPI_DESCRIPTOR IdeRecoveryNativeModePpiDescriptor = {
   (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
   &gIdeRecoveryNativeModePpiGuid,
   &IdeRecoveryNativeModePpi
};

/**
    This function is used to initialize the IDE ATA channel
    for BIOS recovery from ATA devices

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the PEI ATA Controller PPI
    @param ChannelMask Bit flag indicating which channel has to be
        enabled. The following is the bit definition:
        Bit0    IDE Primary
        Bit1    IDE Secondary
        Bit2    No SATA
        Bit3    SATA as Primary
        Bit4    SATA as Secondary

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS SBPEI_EnableAtaChannelForRecovery (
    IN EFI_PEI_SERVICES               **PeiServices,
    IN PEI_ATA_CONTROLLER_PPI         *This,
    IN UINT8                          ChannelMask
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32     TempSataBar5Add = 0xFF770000;
    
    // Mem and IO space
    //
    SET_PCI8_SATA(FCH_SATA_REG04, BIT0+BIT1+BIT3);

    WRITE_PCI32_SATA(FCH_SATA_REG10, IdeRecoveryNativeModePpi.PCMDBarAddress);
    WRITE_PCI32_SATA(FCH_SATA_REG14, IdeRecoveryNativeModePpi.PCNLBarAddress &~ 2);
    WRITE_PCI32_SATA(FCH_SATA_REG18, IdeRecoveryNativeModePpi.SCMDBarAddress);
    WRITE_PCI32_SATA(FCH_SATA_REG1C, IdeRecoveryNativeModePpi.SCNLBarAddress &~ 2);

    // Below unused IO base, But some base register default is 0
    // that will cause legacy DMA controller fail.
    WRITE_PCI32_SATA(FCH_SATA_REG20, TEMP_BASE2 + 0x500); // fill temproary base

    WRITE_PCI32_SATA(FCH_SATA_REG24, TempSataBar5Add); // fill temproary  BAR5 base

    // Clear AHCI Enable during POST time. DE is investigating when/how to restore this bit before goes to OS boot.
    RW_MEM32 (TempSataBar5Add + FCH_SATA_BAR5_REG04,0,BIT31); 
      
    //-----------------------
    // Native IDE mode (SATA)
    //-----------------------
    RW_PCI32_SATA(FCH_SATA_REG40,BIT0,0); //SubclassCodeWriteEnable: 
    // BIT00 == SATA enable (HW : Default)
    
    WRITE_PCI32_SATA(FCH_SATA_REG08, 0x01018F40); //Program IDE Mode
     
    WRITE_PCI16_SATA(FCH_SATA_REG02, FCH_SATA_DID); // Program IDE mode DID

    RW_PCI32_SATA(FCH_SATA_REG40,0,BIT0); //SubclassCodeWriteDisable
    
    // Install PPI for native mode
    Status = (**PeiServices).InstallPpi (
                                        PeiServices,
                                        &IdeRecoveryNativeModePpiDescriptor);
    if (EFI_ERROR (Status)) return Status;

    return EFI_SUCCESS;
}

#endif //  ATAPI_RECOVERY_SUPPORT


#if     SMM_SUPPORT

/**
    This function is used to generate S/W SMI in the system. This
    call is mainly used during S3 resume to restore SMMBase

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the SMM Control PPI
    @param ArgumentBuffer Argument to be used to generate S/W SMI
    @param ArgumentBufferSize Size of the argument buffer
    @param Periodic Indicates the type of SMI invocation
    @param ActivationInterval If it is periodic invocation, this field
        indicates the period at which the SMI is generated

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SBPEI_ActivateSMI (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN OUT INT8             *ArgumentBuffer OPTIONAL,
    IN OUT UINTN            *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN              Periodic OPTIONAL,
    IN UINTN                ActivationInterval OPTIONAL
)
{
    UINT8       SmiCmd8;
    UINT16      SmiCmd16, Value16;

    if (Periodic || ((NULL != ArgumentBuffer) && (NULL == ArgumentBufferSize))) {
      return EFI_INVALID_PARAMETER;
    }
    if (NULL == ArgumentBuffer) {
        SmiCmd8 = 0xff;
        SmiCmd16 = 0xffff;
    } else {
        SmiCmd8 = ArgumentBuffer[0];
        SmiCmd16 = (ArgumentBuffer[1] << 8) + ArgumentBuffer[0];
    }

    // Enable ACPI MMIO space
    SET_IO8_PMIO(FCH_PMIOA_REG04, BIT1);

    // Making sure SW SMI port is SW_SMI_IO_ADDRESS
    Value16 = READ_MEM16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
    while(Value16 != SW_SMI_IO_ADDRESS) {
        WRITE_MEM16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A, SW_SMI_IO_ADDRESS);
        Value16 = READ_MEM16(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
    }

    // Enable CmdPort SMI
	RW_MMIO32_SMI(FCH_SMI_REGB0, BIT23 + BIT22, BIT22);

	//
	// Clear SmiEnB to enable SMI function
	//
	RESET_MMIO32_SMI(FCH_SMI_REG98, BIT31);

	//
	// Set the EOS Bit
	//
	SET_MMIO32_SMI(FCH_SMI_REG98, BIT28);

    // Trigger command port SMI
    if (*ArgumentBufferSize == 2) {
        IoWrite16 (SW_SMI_IO_ADDRESS, SmiCmd16);
    } else {
        IoWrite8 (SW_SMI_IO_ADDRESS, SmiCmd8);
    }

    return EFI_SUCCESS;
}

/**
    This function is used to clear the SMI and issue End-of-SMI

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the SMM Control PPI
    @param Periodic Indicates the type of SMI invocation to stop

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SBPEI_DeactivateSMI (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN BOOLEAN              Periodic OPTIONAL
)
{
    if (Periodic) {
      return EFI_INVALID_PARAMETER;
    }

    // Clear SmiCmdPort Status Bit
    WRITE_MMIO32_SMI(FCH_SMI_REG88, BIT11);

    // Set the EOS Bit
    SET_MMIO32_SMI(FCH_SMI_REG98, BIT28);

    return EFI_SUCCESS;
}
#endif

/**
    This function is used to call back after HYGON SB init.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDesc 
    @param InvokePpi 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
sbEndOfFchCallBack(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_PEI_PCI_CFG2_PPI *PciCfg = (*PeiServices)->PciCfg;
    EFI_PEI_CPU_IO_PPI      *CpuIo = (*PeiServices)->CpuIo;
    EFI_BOOT_MODE           BootMode;

    //
    // update boot mode
    //
    PEI_TRACE((-1, PeiServices, "updating boot mode....\n"));
    Status = UpdateBootMode(PeiServices, CpuIo, PciCfg);
    ASSERT_PEI_ERROR (PeiServices, Status);

    Status = (*PeiServices)->NotifyPpi( PeiServices, MemInstalledNotifyList );
    ASSERT_PEI_ERROR ( PeiServices, Status );

    Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    ASSERT_PEI_ERROR (PeiServices, Status);

#if defined (USBPEI_SUPPORT) && (USBPEI_SUPPORT)
#if defined (XHCI_Firmware_Support) && (XHCI_Firmware_Support)
    if (BootMode != BOOT_ON_S3_RESUME){
    // When Usb Pei was enabled, we should always initial XHCI in early stage.
    // Notify Fch to initialize XHCI and so on...
	Status = sbFchRecoveryCallBack(PeiServices, NotifyDesc, InvokePpi);
	ASSERT_PEI_ERROR (PeiServices, Status);
    }
#endif
#endif

    if (BootMode != BOOT_ON_S3_RESUME && BootMode != BOOT_ON_FLASH_UPDATE) {
        if (IsRecovery(PeiServices, PciCfg, CpuIo)) {
            BootMode = BOOT_IN_RECOVERY_MODE;

            Status = (*PeiServices)->SetBootMode(PeiServices, BootMode);

#if defined (UsbRecov_SUPPORT) && (UsbRecov_SUPPORT)
#ifdef XHCI_Firmware_Support
#if XHCI_Firmware_Support
            // Notify Fch to initialize XHCI and so on...
            Status = sbFchRecoveryCallBack(PeiServices, NotifyDesc, InvokePpi);
            ASSERT_PEI_ERROR (PeiServices, Status);
#endif
#endif
#endif

            // Let everyone know that boot mode has been determined by
            // installing the MasterBootMode PPI
            (*PeiServices)->InstallPpi(PeiServices, mBootModePpi );
            (*PeiServices)->InstallPpi( PeiServices, mRecoveryModePpi );
        }
    }

#if defined DISABLE_DAYLIGHT_SAVINGS && DISABLE_DAYLIGHT_SAVINGS == 1
    // PMx5F_x00 RTCEXT DltSavEnable
    WRITE_IO8_PMIO(FCH_PMIOA_REG5E, 0);
    SET_IO8_PMIO(FCH_PMIOA_REG5F, BIT0);
    // Enable The RTC register 72/73 0xB[0].
    WRITE_IO8_HYGON_RTC(0xB, READ_IO8_HYGON_RTC(0xB) | BIT0);
#endif

    //
    // Install the SB Init Policy PPI
    //
    PEI_TRACE((-1, PeiServices, "Installing SB PEI services\n"));
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
    ASSERT_PEI_ERROR (PeiServices, Status);

    return Status;
}

#if SUPPORT_RAID_DRIVER
/**
    This function provides SATA Port Information

    @param PeiServices Pointer to the PEI services table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

VOID DetectSataPortInfo (
    IN EFI_PEI_SERVICES     **PeiServices)
{
    EFI_STATUS          Status;
    UINT16              HobSize = sizeof(SATA_PRESENT_HOB);
    EFI_GUID            SataPresentHobGuid = AMI_SATA_PRESENT_HOB_GUID;
    SATA_PRESENT_HOB    *SataPresentHob;
    UINT8               SataPortStatus;
    UINT8               i;
	UINT8   			Value8;
	SB_SETUP_DATA       SbSetupData;

    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobSize,
                                        &SataPresentHob);
    if(EFI_ERROR(Status)) return;

    SataPresentHob->EfiHobGuidType.Name = SataPresentHobGuid;

    for (i = 0; i < 4; i++) {
		SataPresentHob->SataInfo[i].ClassCode = 0;
		SataPresentHob->SataInfo[i].PresentPortBitMap = 0;
    }

	(*PeiServices)->SetMem((VOID *)&SbSetupData, sizeof (SB_SETUP_DATA), 0);
	GetSbSetupData( PeiServices, &SbSetupData, TRUE );
	
	if ((SbSetupData.Sata.SataClass == SataAhci) || (SbSetupData.Sata.SataClass == SataRaid)) { // AHCI or Raid mode
		if (SbSetupData.Sata.SataClass == SataAhci)
			SataPresentHob->SataInfo[0].ClassCode = AhciClassCode;
		else
			SataPresentHob->SataInfo[0].ClassCode = RaidClassCode;
		Value8 = READ_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0 + 0x03);
		Value8 |= 0x01;	//01: Select "det" for Port 0 to 5 
		WRITE_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0 + 0x03, Value8);
		SataPortStatus = READ_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0);
		SataPresentHob->SataInfo[0].PresentPortBitMap = (SATA_BUS_DEV_FUN << 16) | (SataPortStatus & 0x3F);
		SataPresentHob->ControllerCount = 1;
	} else {	//IDE mode
		SataPresentHob->SataInfo[0].ClassCode = IdeClassCode;
		Value8 = READ_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0 + 0x03);
		Value8 |= 0x01;	//01: Select "det" for Port 0 to 5 
		WRITE_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0 + 0x03, Value8);
		SataPortStatus = READ_MEM8(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REGF0);
		SataPresentHob->SataInfo[0].PresentPortBitMap = (SATA_BUS_DEV_FUN << 16) | (SataPortStatus & 0xF); // Port 0~3
		SataPresentHob->SataInfo[1].ClassCode = IdeClassCode;
		SataPresentHob->SataInfo[1].PresentPortBitMap = (IDE_BUS_DEV_FUN << 16) | (SataPortStatus & 0x3); // Port 4~5
		SataPresentHob->ControllerCount = 2;
	}
}
#endif

/**
    This function is used to call back after in latest PEI.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDesc 
    @param InvokePpi 

              
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SbEndOfPeiCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    EFI_BOOT_MODE   SbBootMode;

    Status = (*PeiServices)->GetBootMode (PeiServices, &SbBootMode);
    ASSERT_PEI_ERROR (PeiServices, Status);

    if (SbBootMode == BOOT_ON_S3_RESUME) {

    }
#if CMOS_MANAGER_SUPPORT
    // PM_Reg 59: VRT_T2 set to default value(0xff)
    WRITE_MEM8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG59, 0xFF);
#endif

    // Setting 8254
    // program timer 1 as refresh timer
    IoWrite8(TIMER_CTRL,0x54);
    IoWrite8(TIMER_1_COUNT,0x12);
#if SUPPORT_RAID_DRIVER
    DetectSataPortInfo(PeiServices);
#endif

    return  EFI_SUCCESS;
}

/**
    This function is used to call back before/after as follows AGESA-routine.
    AmdInitReset/AmdInitEarly/AmdInitPost

    @param PeiServices Pointer to the PEI services table
    @param NotifyDesc 
    @param InvokePpi 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
SbAgesaCallback(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
    EFI_STATUS                      Status = EFI_SUCCESS;

#if HYGON_SB_SIO_PME_BASE_WORKAROUND //1 //###defined(SIO_SUPPORT) && SIO_SUPPORT
    HYGON_FCH_AMI_CALLOUT_PPI         *FchCallOut = InvokePpi;
    //
    // The reason why the callback here is....
    //   HYGON AGESA-FCH reference code hard coding "CfgSioPmeBaseAddress".
    // We don't have chance to change the base unless we synchronize the SB_SIO_PME_BASE_ADDRESS
    //   and the PME-base token of SIO component.
    //  : This method maybe change later if HYGON AGESA-FCH support it.
    // 

    if(FchCallOut->CallOutType == AfacAfterInitReset) {
        EFI_PEI_NOTIFY_DESCRIPTOR       *theNotifyDesc = NotifyDesc;
        // restore back previous value which assigned by SIO component.
        WRITE_PCI32_SB(0x64, theNotifyDesc[1].Flags);
    }
#endif
    return Status;
}



#if defined (XHCI_RECOVERY_PATH_MODE) && (XHCI_RECOVERY_PATH_MODE)
/**
    This function is used to get the XHCI firmware start address.

    @param PeiServices Pointer to the PEI services table

        UINT32
    @retval SectionData XHCI firmware address

**/
UINT32
SbFchGetXhciImage (IN  EFI_PEI_SERVICES **PeiServices)
{
    EFI_STATUS                    	Status = EFI_NOT_FOUND;
    EFI_PEI_FV_HANDLE    			VolumeHandle;
    UINTN                         	Instance = 0;
    EFI_PEI_FILE_HANDLE           	FileHandle;
    EFI_FV_FILE_INFO				FileInfo;
    VOID							*SectionData;

    Status = PeiServicesFfsFindNextVolume(Instance, &VolumeHandle);
    // The XHCI firmware can't be load into memory otherwise XHCI recovery is invalid.
    // if PeiRamBoot on it might be loaded into memory. Find it in ROM directly.
    //		VolumeHandle = (EFI_PEI_FV_HANDLE)FV_BB_BASE;
    if (!EFI_ERROR(Status)) {
    	Status = PeiServicesFfsFindFileByName(&gXhciRecoveryFwGuid, VolumeHandle, &FileHandle);
    	//##if (!EFI_ERROR(Status)) {
    	//##	Status = PeiServicesFfsFindSectionData(EFI_SECTION_RAW, FileHandle, &SectionData);
    	//##}
    	if (!EFI_ERROR(Status)) {
    		Status = PeiServicesFfsGetFileInfo(FileHandle, &FileInfo);
    		if (!EFI_ERROR(Status)) {
    			SectionData = FileInfo.Buffer;
    		}
    	}
    }

	if (EFI_ERROR(Status)) SectionData = 0;

	PEI_TRACE(((UINTN)TRACE_ALWAYS, PeiServices, "  XHCI firmware Status=%r, Addr=%X\n", Status, (UINT32)SectionData));

    return (UINT32)SectionData;
}
#endif

#ifdef XHCI_Firmware_Support
#if XHCI_Firmware_Support
/**
    This function is used to call back recovery MODE init.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDesc 
    @param InvokePpi 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
sbFchRecoveryCallBack(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
    EFI_STATUS         Status = EFI_SUCCESS;
    AMD_FCH_INIT_PPI    *fchInitPpi = InvokePpi;
    
    if (fchInitPpi->Revision >= 0x1){
#if defined (PEI_XHCI_SUPPORT) && (PEI_XHCI_SUPPORT)
    	UINT32		XhciRomAddress = XHCI_FIRMWARE_BASE;
#if defined (XHCI_RECOVERY_PATH_MODE) && (XHCI_RECOVERY_PATH_MODE)
    	XhciRomAddress = SbFchGetXhciImage(PeiServices);
#endif
        if (XhciRomAddress != 0) {
        	fchInitPpi->FpFchXhciRecovery(fchInitPpi, XhciRomAddress);
        	Status = CountTime(1000*1000, PM_BASE_ADDRESS);
        }
#endif  //defined (PEI_XHCI_SUPPORT) && (PEI_XHCI_SUPPORT)
    }

    return Status;
}
#endif
#endif
/**
    Setup the wake type in PCD.

    @param

    @retval VOID

    @note
**/
VOID
SbWakeupTypeLibConstructor (
  VOID
  )
{
	SB_WAKEUP_TYPE	SbWakeType;
	UINT16	Pm1Status = READ_IO16_PM(SB_ACPI_PM1_STS);
	UINT16	Pm1En = READ_IO16_PM(SB_ACPI_PM1_EN);
	UINT32	Gpe0Status = READ_MMIO32_SMI(0);
	UINT32	Gpe0En = READ_MMIO32_SMI(0x4);


	if (SbCheckAfterG3()) SbWakeType = SbWakeupTypeAcPowerRestored;
	else if (Pm1Status & BIT15) {
		// Power button
		if (Pm1Status & Pm1En & BIT8) {
			SbWakeType = SbWakeupTypePowerSwitch;
		} else if (Pm1Status & Pm1En & BIT10) {
			SbWakeType = SbWakeupTypeApmTimer;
		} else if (Gpe0Status & Gpe0En) {
			SbWakeType = SbWakeupTypePciPme;
		} else SbWakeType = SbWakeupTypeOther; // don't know what the type is. Let's assign "Other".
	} else SbWakeType = SbWakeupTypeUnknown; // not sure what's is.

	// HYGON No on chip lan. The SbWakeupTypeLanRemote is not support
	// SbWakeupTypeModemRing.....

	PcdSet32(PcdSbWakeType, SbWakeType);
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

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

//**********************************************************************
/** @file SbInterfacePei.c
    SB CIMX interface in the PEI Phase ,it is main change the parameters
    for the SB CIMX code .

**/
//**********************************************************************

// Module specific Includes

#include <PiPei.h>

#include "token.h"
#include <Setup.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <Library/SbSetupData.h>

#include <Ppi/HygonSbWrapper.h>
#include <Ppi/ReadOnlyVariable2.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

VOID
HookInitReset (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookInitEarly (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookInitPost (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookS3LateRestore (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookAfterInitReset (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookAfterInitEarly (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookAfterInitPost (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookAfterS3LateRestore (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookAgesaFchOemCalloutPei (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  );

VOID
HookApplyGpioTable (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN OUT HYGON_FCH_AMI_POLICY_PPI *This,
  IN VOID                       *OverwriteTable
  );

EFI_STATUS
SbInterfaceAfterSbCustomPpi(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
    );

BOOLEAN
SbGetCapsuleFlag (
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

static HYGON_FCH_AMI_CALLOUT_PPI CallOutInitReset = {AfacInitReset};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutInitEarly = {AfacInitEarly};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutInitPost = {AfacInitPost};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutS3LateRestore = {AfacS3LateRestore};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutAfterInitReset = {AfacAfterInitReset};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutAfterInitEarly = {AfacAfterInitEarly};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutAfterInitPost = {AfacAfterInitPost};
static HYGON_FCH_AMI_CALLOUT_PPI CallOutAfterS3LateRestore = {AfacAfterS3LateRestore};

// PPI that are installed
static EFI_PEI_PPI_DESCRIPTOR gCallOutPpiList[] = {
    // PPIs will be install after HYGON SB init.
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutInitReset},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutInitEarly},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutInitPost},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutS3LateRestore},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutAfterInitReset},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutAfterInitEarly},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutAfterInitPost},
    {(EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gHygonFchAmiCallOutPpiGuid,
    &CallOutAfterS3LateRestore},
};

static EFI_PEI_NOTIFY_DESCRIPTOR  NotifyList[] =
{
    {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmiPeiSbCustomPpiGuid,
    SbInterfaceAfterSbCustomPpi}
};

// This is for backward compatible old PXP module.
EFI_PEI_PPI_DESCRIPTOR  STATIC mPpiListHygonSbInitDonePolicyGuid = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonPeiSbInitDonePolicyGuid,
  NULL
};

AMI_GPIO_INIT_TABLE_STRUCT gAmiGpioInitTable[] =
{
    //  { GpioNo, GpioCfg}
    #include "GPIO.h"
};

// External Declaration(s)

extern UINT8 IoMuxSelect[];

// Function Definition(s)

/**
    This function is the entry point for this PEI. This function
    initializes the Chipset interface PPI

    @param FfsHeader Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
HygonSbInterfaceInit (
  IN       EFI_PEI_FILE_HANDLE   FileHandle,
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    SB_SETUP_DATA               SbSetupData;
    FCH_RESET_DATA_BLOCK        *FchData;
    HYGON_FCH_AMI_POLICY_PPI      *SbPolicy;
    EFI_PEI_PPI_DESCRIPTOR      *PpiList;

    Status = (*PeiServices)->AllocatePool (PeiServices, \
                        sizeof(EFI_PEI_PPI_DESCRIPTOR),
                        &PpiList
                        );
    ASSERT_PEI_ERROR (PeiServices, Status);

    Status = (*PeiServices)->AllocatePool (PeiServices, \
                        sizeof(HYGON_FCH_AMI_POLICY_PPI),
                        &SbPolicy
                        );
    ASSERT_PEI_ERROR (PeiServices, Status);

    PpiList->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI \
                                | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    PpiList->Guid = &gHygonFchAmiPolicyPpiGuid;
    PpiList->Ppi = SbPolicy;

    //
    // Apply AGESA-FCH wrapper call out
    //
    SbPolicy->HookInitReset = HookInitReset;
    SbPolicy->HookInitEarly = HookInitEarly;
    SbPolicy->HookInitPost = HookInitPost;
    SbPolicy->HookS3LateRestore = HookS3LateRestore;
    SbPolicy->HookAfterInitReset = HookAfterInitReset;
    SbPolicy->HookAfterInitEarly = HookAfterInitEarly;
    SbPolicy->HookAfterInitPost = HookAfterInitPost;
    SbPolicy->HookAfterS3LateRestore = HookAfterS3LateRestore;
    SbPolicy->HookAgesaFchOemCalloutPei = HookAgesaFchOemCalloutPei;
    SbPolicy->HookApplyGpioTable = HookApplyGpioTable;

    //
    // Apply default gpio table
    //
    SbPolicy->AmiPeiSbCustomPpi = NULL;
    SbPolicy->DefaultGpioTbl = gAmiGpioInitTable;
    SbPolicy->DefaultGpioTblEntries = \
        sizeof(gAmiGpioInitTable) / sizeof(AMI_GPIO_INIT_TABLE_STRUCT) - 1;

    FchData = &SbPolicy->HygonParameters;

    //
    // Get SETUP DATA parameters
    //
    (*PeiServices)->SetMem((VOID *)&SbSetupData, sizeof (SB_SETUP_DATA), 0);
    GetSbSetupData( PeiServices, &SbSetupData, TRUE );

    // clear
    (*PeiServices)->SetMem((VOID *)FchData, sizeof (FCH_RESET_DATA_BLOCK), 0);
    SbPolicy->HygonSafeDefault = SbSetupData.AmdSafeDefault;
    if (!SbPolicy->HygonSafeDefault) {
        //======================================================================
        // assign FCH parameters here.
        if (!SbSetupData.CbsPresent){
            PcdSetBool (PcdSataEnable,SbSetupData.Sata.SataMode.SataEnable);
            PcdSetBool (PcdXhci0Enable,SbSetupData.Usb.Xhci0Enable);
        }

        FchData->AutoMode = SbSetupData.Spi.AutoMode;
        if(FchData->AutoMode == 0){        
            FchData->Mode = SbSetupData.Spi.SpiMode;
    
            FchData->SPI100_Enable = SbSetupData.Spi.SPI100_Enable;
            FchData->SpiSpeed = SbSetupData.Spi.SpiSpeed;
            FchData->FastSpeed = SbSetupData.Spi.SpiFastSpeed;
            FchData->BurstWrite = SbSetupData.Spi.SpiBurstWrite;
        }

        PcdSetBool (PcdLegacyFree,SbSetupData.LegacyFree);
        FchData->FchOscout1ClkContinous = FCH_SERIAL_IRQ_MODE;     
        PcdSet8 (PcdLpcClockDriveStrength,FCH_LPC_CLK_DRVSTH);
        
        PcdSetBool (PcdSataSetMaxGen2,SbSetupData.Sata.SataMode.SataSetMaxGen2);
    }

    // Publish the SB interface PPI
    Status = (*PeiServices)->InstallPpi (PeiServices, PpiList);
    ASSERT_PEI_ERROR (PeiServices, Status);

    // Notify PPI after user gAmiPeiSbCustomPpiGuid.
    Status = (**PeiServices).NotifyPpi (PeiServices, &NotifyList[0]);

  return Status;
}


/**
    Before AGESA FchInitResetTaskTable.
    This is CSP ELINK after AgesaFchOemCallout.

    @param FchData 

    @retval VOID

**/

VOID
HookAgesaFchOemCalloutPei (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    FCH_RESET_DATA_BLOCK        *FchData = InitAgesaHook, *HygonParameters = NULL;
    EFI_PEI_PPI_DESCRIPTOR      *CallOutPpiDesc = NULL;
    HYGON_FCH_AMI_CALLOUT_PPI     *CallOutPpi = NULL;

    //
    // create call out PPI
    //
    Status = (*PeiServices)->AllocatePool (PeiServices, \
                        sizeof(EFI_PEI_PPI_DESCRIPTOR),
                        &CallOutPpiDesc
                        );
    ASSERT_PEI_ERROR (PeiServices, Status);
    Status = (*PeiServices)->AllocatePool (PeiServices, \
                        sizeof(HYGON_FCH_AMI_POLICY_PPI),
                        &CallOutPpi
                        );
    ASSERT_PEI_ERROR (PeiServices, Status);

    CallOutPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI \
                                | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    CallOutPpiDesc->Guid = &gHygonFchAmiCallOutPpiGuid;
    CallOutPpiDesc->Ppi = CallOutPpi;

    //
    // Assign AGESA-FCH parameters
    //
    if (!This->HygonSafeDefault) {
        HygonParameters = &This->HygonParameters;
        //####(*PeiServices)->CopyMem(FchData, &HygonParameters, sizeof (FCH_RESET_DATA_BLOCK));
        //======================================================================
        // assign FCH parameters here.
        FchData->FchReset.Xhci1Enable = FALSE;
        
        FchData->AutoMode = HygonParameters->AutoMode;
        if(FchData->AutoMode == 0){        
            FchData->Mode = HygonParameters->Mode;
            FchData->SPI100_Enable = HygonParameters->SPI100_Enable;
            FchData->SpiSpeed = HygonParameters->SpiSpeed;
            FchData->FastSpeed = HygonParameters->FastSpeed;
            FchData->BurstWrite = HygonParameters->BurstWrite;
        }
        FchData->FchOscout1ClkContinous = HygonParameters->FchOscout1ClkContinous;  
    }

    {
    EFI_BOOT_MODE	BootMode;
    Status = (*PeiServices)->GetBootMode(PeiServices, &BootMode);
    FchData->Gpp.IsCapsuleMode = FALSE;
    if (BootMode == BOOT_ON_FLASH_UPDATE || SbGetCapsuleFlag()) FchData->Gpp.IsCapsuleMode = TRUE;
    }

    //
    // Install call out PPI to notify user to change FCH parameters.
    //
    CallOutPpi->CallOutType = AfacAgesaFchOemCalloutPei;
    CallOutPpi->CallOutData = FchData;
    Status = (*PeiServices)->InstallPpi (
                                PeiServices,
                                CallOutPpiDesc
                                );
    ASSERT_PEI_ERROR (PeiServices, Status);
}



/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitReset (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacInitReset]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}


/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitEarly (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacInitEarly]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

               
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookInitPost (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacInitPost]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}


/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

               
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookS3LateRestore (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacS3LateRestore]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

               
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitReset (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacAfterInitReset]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

               
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitEarly (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;

    // This is for backward compatible old PXP module.
    Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mPpiListHygonSbInitDonePolicyGuid
                             );
    ASSERT_PEI_ERROR (PeiServices, Status);

    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacAfterInitEarly]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

               
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterInitPost (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacAfterInitPost]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**
    This is the stub function call out from AGESA. User can through
    the Notify installation.

              
    @param PeiServices 
    @param This 
    @param InitAgesaHook 

    @retval VOID

**/

VOID
HookAfterS3LateRestore (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN HYGON_FCH_AMI_POLICY_PPI     *This,
  IN VOID                       *InitAgesaHook
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    Status = (*PeiServices)->InstallPpi(PeiServices, &gCallOutPpiList[AfacAfterS3LateRestore]);
    ASSERT_PEI_ERROR (PeiServices, Status);
}

/**

        
    @param AcpiRwTbl 
    @param GpioTb 
    @param Entries 

        ACPI_REG_WRITE*  
    @retval AcpiRwTbl Return value

**/
ACPI_REG_WRITE*
TranslateToHygonAcpiTbl (
  IN OUT ACPI_REG_WRITE         *AcpiRwTbl,
  IN AMI_GPIO_INIT_TABLE_STRUCT *GpioTb,
  IN UINT32                     Entries
  )
{
    UINTN   i, Entry;

    i = 0;
    for (Entry = 0; Entry < Entries; Entry++) {
        AcpiRwTbl[i].MmioBase       = IOMUX_BASE >> 8;
        AcpiRwTbl[i].MmioReg        = (UINT8)GpioTb[Entry].GpioNo;
        AcpiRwTbl[i].DataAndMask    = 0;
        AcpiRwTbl[i].DataOrMask     = (UINT8)(GpioTb[Entry].GpioCfg >> 8) & ~(IS_GPIO >> 8);

        if((GpioTb[Entry].GpioCfg & IS_GPIO) == IS_GPIO) {
            AcpiRwTbl[i].DataOrMask = IoMuxSelect[GpioTb[Entry].GpioNo];
            i++;
            AcpiRwTbl[i].MmioBase    = GPIO_BASE >> 8;
            AcpiRwTbl[i].MmioReg     = (UINT8)GpioTb[Entry].GpioNo;
            AcpiRwTbl[i].DataAndMask = 0;
            AcpiRwTbl[i].DataOrMask  = (UINT8)GpioTb[Entry].GpioCfg;
        }
        i++;
    }

    return &AcpiRwTbl[i];
}

/**
    This function configur paremeters in the PEI phase for SB CIMX

               
    @param PeiServices 
    @param This 
    @param OverwriteTable 

    @retval VOID

**/
VOID
HookApplyGpioTable (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN OUT HYGON_FCH_AMI_POLICY_PPI *This,
  IN VOID                       *OverwriteTable
  )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    AMI_GPIO_INIT_TABLE_STRUCT      *DefaultGpioTbl, *CustomizeTbl;
    ACPI_REG_WRITE                  *HygonTbl, *TmpTbl;
    UINT32                          TblEntries, DefEntries, TotalEntries, TotalSize;

    if (OverwriteTable != NULL) {
//###        This->HygonParameters.OemResetProgrammingTablePtr = OverwriteTable;
        return;
    }

    TotalSize = 0;
    TblEntries = 0;
    TotalEntries = 0;
    //TmpTbl = NULL;
    CustomizeTbl = NULL;
    DefaultGpioTbl = This->DefaultGpioTbl;
    // entries for default table
    DefEntries = This->DefaultGpioTblEntries;
    TotalEntries = DefEntries;

    if (This->AmiPeiSbCustomPpi != NULL) {
        CustomizeTbl = This->AmiPeiSbCustomPpi->GpioInit->GpioTable;

        // how many entries in the table?
        while (CustomizeTbl[TblEntries].GpioNo != 0xFFFF
                &&  CustomizeTbl[TblEntries].GpioCfg != 0xFFFF)
        {
            TblEntries++;
        }

        // if ignore the default table.
        if (This->AmiPeiSbCustomPpi->GpioInit->InitDefaultGpioSetting == FALSE) {
            // ignore the default table.
            TotalEntries = TblEntries;
            DefEntries = 0;
        } else {
            TotalEntries += TblEntries;
        }
    }

    // one entry need 2 contentor for GPIO and IoMUX.
    TotalSize = TotalEntries * sizeof(ACPI_REG_WRITE) * 2;
    // append one Signature and one EndOfTable in Size.
    TotalSize = TotalSize + (1 + 1) * sizeof(ACPI_REG_WRITE);
    Status = (*PeiServices)->AllocatePool (
                                PeiServices,
                                TotalSize,
                                &HygonTbl
                                );
    ASSERT_PEI_ERROR (PeiServices, Status);

    // fill HYGON Signature
    HygonTbl[0].MmioBase       = 0;
    HygonTbl[0].MmioReg        = 0;
    HygonTbl[0].DataAndMask    = 0xB0;
    HygonTbl[0].DataOrMask     = 0xAC;
    //
    // fill Default gpio table.
    //
    TmpTbl = TranslateToHygonAcpiTbl(&HygonTbl[1], DefaultGpioTbl, DefEntries);
    if (TmpTbl == NULL) {
//###        This->HygonParameters.OemResetProgrammingTablePtr = NULL;
        return;
    }
    //
    // fill the customization table.
    //
    TmpTbl = TranslateToHygonAcpiTbl(TmpTbl, CustomizeTbl, TblEntries);
    if (TmpTbl == NULL) {
//###        This->HygonParameters.OemResetProgrammingTablePtr = NULL;
        return;
    }
    //
    // fill the End Of Table
    //
    TmpTbl->MmioBase       = 0xFF;
    TmpTbl->MmioReg        = 0xFF;
    TmpTbl->DataAndMask    = 0xFF;
    TmpTbl->DataOrMask     = 0xFF;

//###    This->HygonParameters.OemResetProgrammingTablePtr = HygonTbl;
}


/**
    To notify us the user custom PPI has been update.

                   
    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SbInterfaceAfterSbCustomPpi(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
    )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    HYGON_FCH_AMI_POLICY_PPI          *This;
    //
    // Apply user GPIO table
    //
    Status = (*PeiServices)->LocatePpi(
                    PeiServices,
                    &gHygonFchAmiPolicyPpiGuid,
                    0,
                    NULL,
                    &This);
    if (EFI_ERROR(Status)) return Status;

    This->AmiPeiSbCustomPpi = InvokePpi;
    This->HookApplyGpioTable(PeiServices, This, NULL);

    return Status;
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

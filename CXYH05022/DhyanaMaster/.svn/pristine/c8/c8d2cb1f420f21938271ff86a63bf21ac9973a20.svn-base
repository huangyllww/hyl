//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file AmiRasPei.c
    This file contains code for RAS related functionality

**/

#include <Library/PeiServicesLib.h>
#include <Setup.h>
#include <AmiPeiLib.h>
#include <Ppi/ReadOnlyVariable2.h>

#include "Guid/AmdCbsConfig.h"
#include "Library/AmdCbsVariable.h"
/**
    This function is the entry point for AmiRasPei PEIM.

    @param FileHandle Pointer to the FFS file header.
    @param PeiServices Pointer to the PEI services table.

    @retval EFI_STATUS EFI_SUCCESS

**/

EFI_STATUS
EFIAPI
AmiRasPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    
    SETUP_DATA                          SetupData;
    UINTN                               VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    EFI_GUID                            SetupGuid= SETUP_GUID;

    CBS_CONFIG                          CbsConfig;


    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status);
    
    VariableSize = sizeof(SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                           NULL, &VariableSize, &SetupData);
    
    if(EFI_ERROR(Status)){
        // Set default value from RasWrapperSetup.sd
        PcdSetBool (PcdAmdCcxCfgPFEHEnable, FALSE);
        PcdSetBool(PcdMcaErrThreshEn, FALSE);
       
        //<Chendl001-20180626 Memory Leaky Bucket Design +>
#if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
        PcdSet16(PcdMcaMemErrThreshCount, 0);
		PcdSet16(PcdMcaProErrThreshCount, 0);
#else
        PcdSet16(PcdMcaErrThreshCount, 0);
#endif
        //<Chendl001-20180626 Memory Leaky Bucket Design ->
	 
        return  EFI_SUCCESS;
    }
    
    VariableSize = sizeof(CBS_CONFIG);
    // ZeroMem(&CbsConfig, VarSize);

    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, CBS_SYSTEM_CONFIGURATION_NAME, &gCbsSystemConfigurationGuid,
                                           NULL, &VariableSize, &CbsConfig);
    if(EFI_ERROR (Status))
       return(Status);
       
	//<Chendl001-20180626 Memory Leaky Bucket Design +>
#if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
    if((SetupData.McaMemErrThreshCount == 0) && (SetupData.McaProErrThreshCount == 0)){
#else
    if(SetupData.McaErrThreshCount == 0){
#endif
    //<Chendl001-20180626 Memory Leaky Bucket Design ->
        PcdSetBool(PcdMcaErrThreshEn, FALSE);
    }
#if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
    PcdSet16(PcdMcaMemErrThreshCount, CbsConfig.McaMemErrThreshCount);//<Lvshh001-20210507 Optimized Memory Leaky Bucket_3>
	PcdSet16(PcdMcaProErrThreshCount, (4095 - CbsConfig.McaProErrThreshCount));
    //<Chendl001-20181103 Optimized Memory Leaky Bucket_2 +>
    PcdSet32(PcdMcaMemErrInterval, CbsConfig.MemLeakyBucketInterval);
    PcdSet16(PcdMcaMemErrDropOut, CbsConfig.MemLeakyBucketDropOutCnt);
    //<Chendl001-20181103 Optimized Memory Leaky Bucket_2 ->
#else
    PcdSet16(PcdMcaErrThreshCount, CbsConfig.McaErrThreshCount);
#endif

    PcdSetBool (PcdAmdCcxCfgPFEHEnable, CbsConfig.CbsCmnCpuPfeh);

    //SGEZT#23035:PCIe Leaky Buckey +>
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
    PcdSet16S(PcdMcaPcieErrThreshCount, CbsConfig.McaPcieErrThreshCount);
    PcdSet32S(PcdMcaPcieErrInterval, CbsConfig.PcieLeakyBucketInterval);
    PcdSet16S(PcdMcaPcieErrDropOut, CbsConfig.PcieLeakyBucketDropOutCnt);
    PcdSet8S(PcdSeparateAERLog, CbsConfig.SeparateAERLog);
#endif
    //SGEZT#23035:PCIe Leaky Buckey ->       
  //SGEZT#26230:Mask ECC Error For OS +>
  #if defined(DEFAULT_SEPARATE_DIMM_ECC_LOG)  
    PcdSet8S(PcdSeparateDIMMECCLog, CbsConfig.SeparateDIMMECCLog);
   #endif
   //SGEZT#26230:Mask ECC Error For OS ->
    return  Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

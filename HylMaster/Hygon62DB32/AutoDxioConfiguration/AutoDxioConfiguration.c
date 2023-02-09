//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define FchSmnGpio                      0x02D02500

#define J10_START_LANE   40
#define J7_START_LANE    80
#define J8A_START_LANE   80
#define J9_START_LANE    112
//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

EFI_STATUS PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
    );
//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gAmdCpmTablePpiGuid, PeiConfigDxioSettings}
};

//-----------------------------------------------------------------------------
EFI_STATUS
DxioDisableTable (
  EFI_PEI_SERVICES **PeiServices, 
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr, 
  UINT8 StartLane,
  UINT8 EndLane
  )
{
  UINT8                               Index;
  DEBUG ((DEBUG_ERROR, "AutoDxioConfiguration Start: StartLane=%d EndLane=%d \n",StartLane,EndLane));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane && DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane ) {
      DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;            
      return EFI_SUCCESS;
    }        
  }
  DEBUG ((DEBUG_ERROR, "AutoDxioConfiguration end\n"));              
  return EFI_NOT_FOUND;
}

EFI_STATUS
DxioDisableTableForSwitch( 
  EFI_PEI_SERVICES **PeiServices,
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr, 
  UINT8 EngineType,
  UINT8 StartLane )
{    
  UINT8                               Index;
  DEBUG ((DEBUG_ERROR, "AutoDxioConfigurationForSwitch Start: StartLane=%d, EngineType=%d\n",StartLane,EngineType));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.EngineType == EngineType && DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane ) {
      DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;            
      return EFI_SUCCESS;
    }        
  }
  DEBUG ((DEBUG_ERROR, "AutoDxioConfigurationForSwitch end\n"));              
  return EFI_NOT_FOUND;
}

EFI_STATUS
DxioEnableConfig(
EFI_PEI_SERVICES **PeiServices,
AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr,
UINT8 EngineType,
UINT8 StartLane,
UINT8 EndLane
)
{
	UINT8                               Index;
	DEBUG((DEBUG_ERROR, "AutoDxioConfiguration Start: StartLane=%d EndLane=%d, EngineType=%d: ", StartLane, EndLane, EngineType));
	for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {

		if (DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane &&
			DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane)
		{
			DxioTopologyTablePtr->Port[Index].EngineData.EngineType = EngineType;
      DEBUG((DEBUG_ERROR, "SUCCESS\n"));
			return EFI_SUCCESS;
		}
	}
	DEBUG((DEBUG_ERROR, "Configuration Not Found\n"));
	return EFI_NOT_FOUND;
}
/**
 * Get Multi Socket/Die FCH GPIO Pin
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 * @param[in] Socket  The Socket Number.
 * @param[in] Die     The Die Number.
 * @param[in] Pin     The GPIO Pin Number.
 *
 * @retval            The State of GPIO Pin.
 *
 */
UINT8
CpmSmnGetGpio (
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT16                      Die,
  IN       UINT16                      Pin
  )
{
  AMD_CPM_COMMON_FUNCTION *CommonFunctionPtr;
  UINT8                   Value;

  CommonFunctionPtr   = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;
  Value = 0;

  Value = CommonFunctionPtr->SmnRead8 (This, Socket, Die, (UINT32) FchSmnGpio, (UINT32) ((Pin << 2) + 2)) & 0x01;
  return Value;
}


/**
 * Read Riser ID GPIO and config DXIO setting per riser id.
 */
EFI_STATUS  
PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  EFI_STATUS                          Status;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS0Ptr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS1Ptr;
  BOOLEAN                            // SATA_CPU0_DETECT0_N,
                                    //  SATA_CPU0_DETECT1_N,
                                      RAID_SLOT_NVME_DET_N,
                                      OCP_PCIEX16_DET_N,
                                      RISERA_TYPE0_N,
                                      RISERA_TYPE1_N,
                                    //  SATA_CPU1_DETECT0_N,
                                      OCP_PCIEX8_DET_N,
                                      RISERB_TYPE0_N,
                                      RISERB_TYPE1_N,
                                      RISERC_TYPE0_N,
                                      RISERC_NVME_DETECT_N;
  EFI_GUID                            SetupGuid= SETUP_GUID;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
  UINTN                               VariableSize;
  SETUP_DATA                          SetupData;
 
  
  Status = (*PeiServices)->LocatePpi (
            (CPM_PEI_SERVICES**)PeiServices,
            &gAmdCpmTablePpiGuid,
            0,
            NULL,
            (VOID**)&CpmTablePpiPtr
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  DxioTopologyTableS1Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);

  //SATA_CPU0_DETECT0_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 136);
 // SATA_CPU0_DETECT1_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 138);
  RAID_SLOT_NVME_DET_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 152);
  OCP_PCIEX16_DET_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 140);
  RISERA_TYPE0_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 3, 10);
  RISERA_TYPE1_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 3, 40);
 // SATA_CPU1_DETECT0_N = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 138);
  OCP_PCIEX8_DET_N = CpmSmnGetGpio(CpmTablePpiPtr, 1, 0, 4);
  RISERB_TYPE0_N = CpmSmnGetGpio(CpmTablePpiPtr, 1, 3, 10);
  RISERB_TYPE1_N = CpmSmnGetGpio(CpmTablePpiPtr, 1, 3, 40);
  RISERC_TYPE0_N = CpmSmnGetGpio(CpmTablePpiPtr, 1, 0, 3);
  RISERC_NVME_DETECT_N = CpmSmnGetGpio(CpmTablePpiPtr, 1, 0, 6);

  //
  //Socket 0: 

      // CPU0 P0 Lane4~7     2: PCIE X4      See sata pcie switch            
      
      // CPU0 P0 Lane8-15    2: PCIE X8                   
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
    
      // CPU0 P1     Lane0-7    1: SATA X8 + 2*PCIE X4         See below sata pcie switch 
  
      // CPU0 P1     Lane8-15    2: PCIEX8         
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 40, 47);
   

    if((FALSE == RISERA_TYPE0_N) && (FALSE == RISERA_TYPE1_N)) {
      // CPU0 P2     Lane0-15    1：PCIEX8+PCIEX8             RISERA_TYPE0_N=0, RISERA_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 80, 87);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 88, 95);
    }else if ((TRUE == RISERA_TYPE0_N) && (FALSE == RISERA_TYPE1_N)){
      // CPU0 P2     Lane0-15    2: PCIEX16                   RISERA_TYPE0_N=1, RISERA_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 80, 95);
    }else if ((FALSE == RISERA_TYPE0_N) && (TRUE == RISERA_TYPE1_N)) {
      // CPU0 P2     Lane0-15    3: 4*PCIE X4                 RISERA_TYPE0_N=0, RISERA_TYPE1_N=1
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 80, 83);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 84, 87);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 88, 91);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 92, 95);
    }else if ((TRUE == RISERA_TYPE0_N) && (TRUE == RISERA_TYPE1_N)) {
      // CPU0 P2     Lane0-15    4: PCIEX8+2*PCIEX4           RISERA_Type0_N=1, RISERA_TYPE1_N=1
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 80, 87);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 88, 91);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 92, 95);
    }

    if(RISERA_TYPE1_N) {
      // CPU0 P3     Lane8-15    1: 2*PCIEX4                  RISERA_TYPE1_N=1
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 120, 123);
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 124, 127);
    }else{
      // CPU0 P3     Lane8-15    2: PCIE X8                   RISERA_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 120, 127);
    }

  //
  //Socket 1:  
      // CPU1 P0     Lane0-7      1：SATA X8+ 2: 2*PCIEX4             See below sata pcie switch

      // CPU1 P0     Lane8-15     2: 2*PCIE X4           
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 8, 11);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 12, 15);
    

    if((FALSE == RISERB_TYPE0_N) && (FALSE == RISERB_TYPE1_N)) {
      // CPU1 P1     Lane0-15     1: PCIEX8+PCIEX8        RISERB_TYPE0_N=0, RISERB_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 32, 39);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 40, 47);
    }else if((TRUE == RISERB_TYPE0_N) && (FALSE == RISERB_TYPE1_N)) {
      // CPU1 P1     Lane0-15     2: PCIE X16             RISERB_TYPE0_N=1, RISERB_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 32, 47);
    }else if(TRUE == RISERB_TYPE1_N) {
      // CPU1 P1     Lane0-15     3: 4*PCIEX4             RISERB_TYPE1_N=1, 
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 32, 35);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 36, 39);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 40, 43);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 44, 47);
    }

    if(FALSE == RISERB_TYPE1_N) {
      // CPU1 P2     Lane0-15     1: PCIE X16             RISERB_TYPE1_N=0
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 80, 95);
    }else{
      // CPU1 P2     Lane0-15     2: 4*PCIEX4             RISERB_TYPE1_N=1, 
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 80, 83);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 84, 87);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 88, 91);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 92, 95);
    }

    if((TRUE == RISERC_TYPE0_N)&&(TRUE == RISERC_NVME_DETECT_N)) {
      // CPU1 P3     Lane0-15     1: PCIEX16              RISERC_TYPE0_N=1, RISERC_NVME_DETECT_N=1
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 112, 127);
    }else if((FALSE == RISERC_TYPE0_N)&&(TRUE == RISERC_NVME_DETECT_N)) {
      // CPU1 P3     Lane0-15     2: 2*PCIEX8             RISERC_TYPE0_N=0, RISERC_NVME_DETECT_N=1
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 112, 119);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 120, 127);
    }else if(FALSE == RISERC_NVME_DETECT_N) {
      // CPU1 P3     Lane0-15     3: 4*PCIEX4             RISERC_NVME_DETECT_N =0
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 112, 115);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 116, 119);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 120, 123);
      DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 124, 127);
    }

    //sata pcie switch
     
     Status = (*PeiServices)->LocatePpi(
                 PeiServices,
                 &gEfiPeiReadOnlyVariable2PpiGuid,
                 0, 
                 NULL,
                 &ReadOnlyVariable
                 );
           
     VariableSize = sizeof(SETUP_DATA);
     Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                     NULL, &VariableSize, &SetupData);
     if(EFI_ERROR(Status)) {
         DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings: GetVariable error %r, use default value!\n", Status));
         
         SetupData.CPU1SataPcieSel[0] = 0;
         SetupData.CPU1SataPcieSel[1] = 0;
         SetupData.CPU2SataPcieSel[0] = 0;
         

     }
                                                                     
     DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[0]:%d\n",SetupData.CPU1SataPcieSel[0])); 
     DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[1]:%d\n",SetupData.CPU1SataPcieSel[1])); 
     DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU2SataPcieSel[0]:%d\n",SetupData.CPU2SataPcieSel[0])); 
    
     if (!SetupData.CPU1SataPcieSel[0]) {
         // CPU0 P0 Lane4~7     2: SATA X4
       DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4,7);
     }else{
         // CPU0 P0 Lane4~7     2: PCIE X4
       DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4,7);            
     }
       
     if ( !SetupData.CPU1SataPcieSel[1]) {
         // CPU0 P1     Lane0-7     SATA X8 
       DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 32,39);
     }else{
         // CPU0 P1     Lane0-7    2*PCIE X4
       DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 32,35);
       DxioEnableConfig(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 36,39);
     }

     if (!SetupData.CPU2SataPcieSel[0]) {
         // CPU1 P0     Lane0-7      SATA X8 
       DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioSATAEngine, 0, 7);
     }else{
         // CPU1 P0     Lane0-7      2*PCIE X4 
       DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 0, 3);
       DxioEnableConfig(PeiServices, DxioTopologyTableS1Ptr, DxioPcieEngine, 4, 7);            
     }



  return EFI_SUCCESS;
}



//----------------------------------------------------------------------------
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ProjectPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the Project PEI phase.
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
// Notes:       This function could initialize Project for anything.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AutoDxioConfigurationInit (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  (*PeiServices)->NotifyPpi( PeiServices, PeiNotifyList );

  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

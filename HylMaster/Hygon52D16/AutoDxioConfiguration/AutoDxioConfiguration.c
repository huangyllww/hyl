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

#define J30_START_LANE   56
#define J34_START_LANE   48
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
  SETUP_DATA                          SetupData;
  UINTN                               VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
  EFI_GUID                            SetupGuid= SETUP_GUID;
  
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
      
      SetupData.CPU1SataPcieSel = 0;

      SetupData.CPU1PcieSlotSel = 0;
      SetupData.CPU2PcieSlotSel = 0;
  }  
  
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel:%d\n",SetupData.CPU1SataPcieSel)); 
                                  
  if (SetupData.CPU1SataPcieSel) {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4 );            
  }
  
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel:%d\n",SetupData.CPU1PcieSlotSel));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU2PcieSlotSel:%d\n",SetupData.CPU2PcieSlotSel));
  
  if (SetupData.CPU1PcieSlotSel == 0 ) {  
  //J30 select x8
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J30_START_LANE, J30_START_LANE+3 );   //disable x4
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J30_START_LANE+4, J30_START_LANE+7 ); //disable x4
  } else if (SetupData.CPU1PcieSlotSel == 1 ) {
  //J30 select x4x4
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J30_START_LANE, J30_START_LANE+7 );  //disable x8
  }
  
  if (SetupData.CPU2PcieSlotSel == 0 ) {   
    //J34 select x16
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+8, J34_START_LANE+15 );  //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+4, J34_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+8, J34_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+12, J34_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU2PcieSlotSel == 1 ) {  
    //J34 select x8x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+4, J34_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+8, J34_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+12, J34_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU2PcieSlotSel == 2 ) {
    //J34 select x4x4x4x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE, J34_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, J34_START_LANE+8, J34_START_LANE+15 );  //disable x8
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

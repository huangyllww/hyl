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

#define J24_START_LANE   48
#define J25_START_LANE   80
#define J26_START_LANE   112
#define J27_START_LANE   104
#define J28_START_LANE   16
#define J301_START_LANE  40

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
      SetupData.CPU1SataPcieSel[2] = 0;
      SetupData.CPU1SataPcieSel[3] = 0;
      SetupData.CPU1SataPcieSel[4] = 0;
      SetupData.CPU1SataPcieSel[5] = 0;
      SetupData.CPU1SataPcieSel[6] = 0;
      
      SetupData.CPU1PcieSlotSel[0] = 0;
      SetupData.CPU1PcieSlotSel[1] = 0;
      SetupData.CPU1PcieSlotSel[2] = 0;
      SetupData.CPU1PcieSlotSel[3] = 0;
      SetupData.CPU1PcieSlotSel[4] = 0;
      SetupData.CPU1PcieSlotSel[5] = 0;
  }           
                                  
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[0]:%d\n",SetupData.CPU1SataPcieSel[0])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[1]:%d\n",SetupData.CPU1SataPcieSel[1])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[2]:%d\n",SetupData.CPU1SataPcieSel[2])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[3]:%d\n",SetupData.CPU1SataPcieSel[3]));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[4]:%d\n",SetupData.CPU1SataPcieSel[4]));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[5]:%d\n",SetupData.CPU1SataPcieSel[5]));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1SataPcieSel[6]:%d\n",SetupData.CPU1SataPcieSel[6]));

                                   
  if (SetupData.CPU1SataPcieSel[0]) {   //J12
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 96 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 96 );            
  }
  
  if (SetupData.CPU1SataPcieSel[1]) {   //J13
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 100 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 100 );            
  }
  
  if (SetupData.CPU1SataPcieSel[2]) {   //J16
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 64 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 64 );            
  }
  
  if (SetupData.CPU1SataPcieSel[3]) {   //J17
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 68 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 68 );            
  }
  
  if (SetupData.CPU1SataPcieSel[4]) {   //J18
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 32 );
    } else {
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 32 );            
    }
    
  if (SetupData.CPU1SataPcieSel[5]) {   //J19
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 36 );
    } else {
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 36 );            
    }
  
  if (SetupData.CPU1SataPcieSel[6]) {   //J110005
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4 );
    } else {
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4 );            
  }
  
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[0]:%d\n",SetupData.CPU1PcieSlotSel[0])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[1]:%d\n",SetupData.CPU1PcieSlotSel[1])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[2]:%d\n",SetupData.CPU1PcieSlotSel[2])); 
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[3]:%d\n",SetupData.CPU1PcieSlotSel[3]));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[4]:%d\n",SetupData.CPU1PcieSlotSel[4]));
  DEBUG ((DEBUG_ERROR, "PeiConfigDxioSettings:CPU1PcieSlotSel[5]:%d\n",SetupData.CPU1PcieSlotSel[5]));

  if (SetupData.CPU1PcieSlotSel[0] == 0 ) {   
      //J24 select x16
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+7 );   //disable x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+15 );  //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+4, J24_START_LANE+7 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+12, J24_START_LANE+15 ); //disable x4
  } else if (SetupData.CPU1PcieSlotSel[0] == 1 ) {  
      //J24 select x8x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+4, J24_START_LANE+7 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+12, J24_START_LANE+15 ); //disable x4
  } else if (SetupData.CPU1PcieSlotSel[0] == 2 ) {
      //J24 select x4x4x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+7 );   //disable x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+15 );  //disable x8
  } else if (SetupData.CPU1PcieSlotSel[0] == 3 ) {
      //J24 select x8x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+15 );  //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+4, J24_START_LANE+7 ); //disable x4    
  } else if (SetupData.CPU1PcieSlotSel[0] == 4 ) {
      //J24 select x4x4x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE, J24_START_LANE+7 );   //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+8, J24_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J24_START_LANE+12, J24_START_LANE+15 ); //disable x4
  }
  
  if (SetupData.CPU1PcieSlotSel[1] == 0 ) {   
        //J25 select x16
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+15 );  //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+4, J25_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+12, J25_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU1PcieSlotSel[1] == 1 ) {  
        //J25 select x8x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+4, J25_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+12, J25_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU1PcieSlotSel[1] == 2 ) {
        //J25 select x4x4x4x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+15 );  //disable x8
    } else if (SetupData.CPU1PcieSlotSel[1] == 3 ) {
        //J25 select x8x4x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+15 );  //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+4, J25_START_LANE+7 ); //disable x4    
    } else if (SetupData.CPU1PcieSlotSel[1] == 4 ) {
        //J25 select x4x4x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE, J25_START_LANE+7 );   //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+8, J25_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J25_START_LANE+12, J25_START_LANE+15 ); //disable x4
    }

  if (SetupData.CPU1PcieSlotSel[2] == 0 ) {   
        //J26 select x16
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+15 );  //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+4, J26_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+12, J26_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU1PcieSlotSel[2] == 1 ) {  
        //J26 select x8x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+4, J26_START_LANE+7 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+12, J26_START_LANE+15 ); //disable x4
    } else if (SetupData.CPU1PcieSlotSel[2] == 2 ) {
        //J26 select x4x4x4x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+7 );   //disable x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+15 );  //disable x8
    } else if (SetupData.CPU1PcieSlotSel[2] == 3 ) {
        //J26 select x8x4x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+15 );  //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+3 );   //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+4, J26_START_LANE+7 ); //disable x4    
    } else if (SetupData.CPU1PcieSlotSel[2] == 4 ) {
        //J26 select x4x4x8
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+15 ); //disable x16
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE, J26_START_LANE+7 );   //disable x8
        
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+8, J26_START_LANE+11 ); //disable x4
        Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J26_START_LANE+12, J26_START_LANE+15 ); //disable x4
    }
  
  if (SetupData.CPU1PcieSlotSel[3] == 0 ) {   
      //J27 select x8     
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J27_START_LANE, J27_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J27_START_LANE+4, J27_START_LANE+7 ); //disable x4

  } else if (SetupData.CPU1PcieSlotSel[3] == 1 ) {  
      //J27 select x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J27_START_LANE, J27_START_LANE+7 ); //disable x8
  }
  
  if (SetupData.CPU1PcieSlotSel[4] == 0 ) {   
      //J28 select x16
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+7 );   //disable x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+15 );  //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+4, J28_START_LANE+7 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+12, J28_START_LANE+15 ); //disable x4
  } else if (SetupData.CPU1PcieSlotSel[4] == 1 ) {  
      //J28 select x8x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+4, J28_START_LANE+7 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+12, J28_START_LANE+15 ); //disable x4
  } else if (SetupData.CPU1PcieSlotSel[4] == 2 ) {
      //J28 select x4x4x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+7 );   //disable x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+15 );  //disable x8
  } else if (SetupData.CPU1PcieSlotSel[4] == 3 ) {
      //J28 select x8x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+15 );  //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+4, J28_START_LANE+7 ); //disable x4    
  } else if (SetupData.CPU1PcieSlotSel[4] == 4 ) {
      //J28 select x4x4x8
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+15 ); //disable x16
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE, J28_START_LANE+7 );   //disable x8
      
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+8, J28_START_LANE+11 ); //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J28_START_LANE+12, J28_START_LANE+15 ); //disable x4
  }
  
  if (SetupData.CPU1PcieSlotSel[5] == 0 ) {   
      //J30-1 select x8     
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J301_START_LANE, J301_START_LANE+3 );   //disable x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J301_START_LANE+4, J301_START_LANE+7 ); //disable x4

  } else if (SetupData.CPU1PcieSlotSel[5] == 1 ) {  
      //J30-1 select x4x4
      Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, J301_START_LANE, J301_START_LANE+7 ); //disable x8
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

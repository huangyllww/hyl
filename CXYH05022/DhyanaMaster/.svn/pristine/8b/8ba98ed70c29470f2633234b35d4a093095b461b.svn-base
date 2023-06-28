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
//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane +>
#include <AmiCspLib.h>
#include <AmiPpi.h>
#include <ppi\ReadOnlyVariable.h>
#include <Ppi\Reset.h>
#include <Hob.h>
#include <Setup.h>
#include <Library/HobLib.h>
#include <Library\PciLib.h>
#include <AmdPcieComplex.h>
#include <Ppi\IpmiTransportPpi.h>
//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane ->

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define FchSmnGpio                      0x02D02500

//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane +>
typedef struct _AMD_CPM_OEM_TABLE_PPI {
  UINTN               Revision;               ///< Revision Number
  UINT16              PlatformId;             ///< Current Platform Id
  VOID                *TableList;             ///< The Point of CPM Definition Table List
} AMD_CPM_OEM_TABLE_PPI;
//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane ->

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
  DEBUG ((DEBUG_ERROR, "[OemDbg Dxio] AutoDxioConfigurationForSwitch Start: StartLane=%d, EngineType=%d\n",StartLane,EngineType));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.EngineType == EngineType && DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane ) {
      DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;            
      return EFI_SUCCESS;
    }        
  }
  DEBUG ((DEBUG_ERROR, "AutoDxioConfigurationForSwitch end\n"));              
  return EFI_NOT_FOUND;
}

//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane +>
EFI_STATUS  
DxioSetHotplug( 
  EFI_PEI_SERVICES **PeiServices, 
  AMD_CPM_DXIO_TOPOLOGY_TABLE *pAmdCpmDxioTopologyTable, 
  UINT8 EngineType, 
  UINT8 StartLane,
  UINT8 EndLane,
  UINT8 Hotplug
  )
{
    DXIO_PORT_DESCRIPTOR            *pDxioPortDescriptor = NULL;
    
    if (pAmdCpmDxioTopologyTable == NULL) {
        return EFI_INVALID_PARAMETER;
    }
    
    DEBUG ((DEBUG_ERROR, "[OemDbg Dxio] DxioSetHotplug: Socket=%d, StartLane=%d, EndLane=%d, EngineType=%d\n",pAmdCpmDxioTopologyTable->SocketId,StartLane,EndLane,EngineType));     
    pDxioPortDescriptor = &pAmdCpmDxioTopologyTable->Port[0];

    while( !(pDxioPortDescriptor->Flags & DESCRIPTOR_TERMINATE_LIST) )
    {
        if((pDxioPortDescriptor->EngineData.EngineType == EngineType) && (pDxioPortDescriptor->EngineData.StartLane == StartLane) && (pDxioPortDescriptor->EngineData.EndLane == EndLane))
        {
            DEBUG ((EFI_D_INFO, "[OemDbg Dxio] Set Hotplug = %d\n", Hotplug));
            pDxioPortDescriptor->Port.LinkHotplug = Hotplug;
            return EFI_SUCCESS;
        }
        
        pDxioPortDescriptor++;
    }
            
    return EFI_NOT_FOUND;
}
//<LiuYB037-20180124 Set Hotplug Attributes if detect 1U4 NVMe Backplane ->

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
  EFI_STATUS                          Status = EFI_SUCCESS;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
//<huangjin001-20190703 Update cpmtable for B01 board + >    
//  UINT8                               CPU0SataPcieSel;
//<huangjin001-20190703 Update cpmtable for B01 board - >    
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS0Ptr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS1Ptr;
  UINT8                               Cpu0P2Type;
  UINT8                               Cpu1P3PCIE0;
  UINT8                               Cpu1P3PCIE1;
   
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
  
  Cpu0P2Type  = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,3 ,23 ); //CPU0 AGPIO23_3 PRSNT_HDDBP_N J178
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio] Cpu0 RB 2 - Cpu0P2Type:%d, ", Cpu0P2Type));
  if (Cpu0P2Type == 0 ) { // 
    DEBUG ((EFI_D_INFO, "x4x4\n"));
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, 88, 95 );
  } else {
    DEBUG ((EFI_D_INFO, "x8\n"));
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, 88, 91 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS0Ptr, 92, 95 );
  }
  
  Cpu1P3PCIE0  = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,2 ,10 ); //CPU0 AGPIO10_2 PRSNT_REAR_PCIE_0_N
  Cpu1P3PCIE1  = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,2 ,40 ); //CPU0 AGPIO40_2 PRSNT_REAR_PCIE_1_N
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio] Cpu1 RB 3 - Cpu2P3PCIE0:%d, Cpu2P3PCIE1:%d, ", Cpu1P3PCIE0, Cpu1P3PCIE1));
  if (Cpu1P3PCIE0 == 0 && Cpu1P3PCIE1 == 0) { // 
    DEBUG ((EFI_D_INFO, "x8x8\n"));
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 112, 127 );
  } else {
    DEBUG ((EFI_D_INFO, "x16\n"));
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 112, 119 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 120, 127 );
  }
//<huangjin001-20190703 Update cpmtable for B01 board + >    
/*  
  CPU0SataPcieSel  = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,0 ,88 ); //CPU0 AGPIO88 PEDET_M2_1_N
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio] CPU0SataPcieSel(J53,0=SATA) %d", CPU0SataPcieSel));
  
  if (CPU0SataPcieSel) {  // J53
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0 );
  } else {
	Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0 );
  }*/
//<huangjin001-20190703 Update cpmtable for B01 board - >    
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

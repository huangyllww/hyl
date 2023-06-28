#include "AutoDxioConfiguration.h"

EFI_STATUS
DxioDumopTable (
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr
  )
{
  UINT8                               Index;

  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if(DxioTopologyTablePtr->Port[Index].EngineData.EngineType != DxioUnusedEngine)
    {
      
        DEBUG((EFI_D_INFO, "[Cancon.%d.%a]Socket:%x EngineType:%x Start Lane:%03d -- %03d EngineData HotPlug:%x Port HotPlug:%x\n",__LINE__,__FUNCTION__,\
                                                                                          DxioTopologyTablePtr->SocketId,\
                                                                                          DxioTopologyTablePtr->Port[Index].EngineData.EngineType,\
                                                                                          DxioTopologyTablePtr->Port[Index].EngineData.StartLane,\
                                                                                          DxioTopologyTablePtr->Port[Index].EngineData.EndLane,\
                                                                                          DxioTopologyTablePtr->Port[Index].EngineData.HotPluggable,\
                                                                                          DxioTopologyTablePtr->Port[Index].Port.LinkHotplug));
    }
  }        
  return EFI_SUCCESS;
}

/**
GC_TODO: DxioDump
@param  CpmTablePpiPtr - GC_TODO: DXIO PPI Pointer

@retval NULL   - GC_TODO: add retval description
**/
void DxioDump(
  IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
)
{
  
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable_Config;

  //Mizl-20200219- Improve the DXIO Part Logic->
  pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);

  //Mizl-20200825-Add Graphic Switch Support+>
  DxioDumopTable(pDxioTopologyTable_Config);
  //Mizl-20200825-Add Graphic Switch Support->
  pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);
  DxioDumopTable(pDxioTopologyTable_Config);
  
  return;
}

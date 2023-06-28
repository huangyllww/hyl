#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>
#include "AmdServerHotplug.h"
#include "AutoDxioConfig.h"

/**
GC_TODO: DxioDump
@param  CpmTablePpiPtr - GC_TODO: DXIO PPI Pointer

@retval NULL   - GC_TODO: add retval description
**/
void DXIOUpdateForGraphicSwitch(
  IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
)
{
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable=NULL;
  UINT8                                iDxioTable;
  //UINT8                               DisableOnboardSwitch;

  if(PcdGet8(PcdOnBoardVideoEn) != 0)
    return;
  
  pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  
  for ( iDxioTable= 0; pDxioTopologyTable->Port[iDxioTable].Flags != DESCRIPTOR_TERMINATE_LIST; iDxioTable++)
  {
    /* code */
    if (pDxioTopologyTable->Port[iDxioTable].EngineData.StartLane != 0)
      continue;

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Disable Onboard VGA -AST2500\n\n",__FUNCTION__,__LINE__));
    pDxioTopologyTable->Port[iDxioTable].EngineData.EngineType=DxioUnusedEngine;

    break;
  }
  
  return;
}
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>
#include "AutoDxioConfig.h"
#include "OemLib.h"

#include "Library/BaseCoreLogicalIdLib.h"

void UpgradeRaidSpeedToGEN4ForHygonDxCPU(
    IN AMD_CPM_TABLE_PPI *CpmTablePpiPtr)
{
  UINT8 iDXIOTable;
  SOC_LOGICAL_ID            LogicalId;
  
  AMD_CPM_DXIO_TOPOLOGY_TABLE *pDxioTopologyTable = CpmTablePpiPtr->CommonFunction.GetTablePtr2(CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  
  MODULE_START

  //Get Logical Id
  BaseGetLogicalIdOnExecutingCore (&LogicalId); 
  if(!(LogicalId.Revision & (HYGON_REV_F18_DN_Dx)))
    MODULE_VOID_END
  
  for (iDXIOTable = 0; iDXIOTable < AMD_DXIO_PORT_DESCRIPTOR_SIZE; iDXIOTable++)
  {

    if (pDxioTopologyTable->Port[iDXIOTable].EngineData.EngineType != DxioUnusedEngine &&
        pDxioTopologyTable->Port[iDXIOTable].EngineData.StartLane == 8 &&
        pDxioTopologyTable->Port[iDXIOTable].EngineData.EndLane == 15)
    {
      pDxioTopologyTable->Port[iDXIOTable].Port.LinkSpeedCapability = DxioGenMaxSupported;
      pDxioTopologyTable->Port[iDXIOTable].Port.MiscControls.LinkSafeMode = DxioGenMaxSupported;
      break;
    }
  }

  MODULE_VOID_END
}
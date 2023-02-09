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

//Include OEM NVMe Topy Porting
//Hugc-20211222-Add Riser/NVMe control function >>>
//#include "NVMeTopy.c"
//#include "AutoNVMePortingList.h"

// NVME_CONFIG   *NvmeConfigList[] = {AUTO_NVME_PORTING_LIST NULL};
// CHAR8         *NvmeConfigNameList[] = {AUTO_NVME_PORTING_LIST_NAMES NULL};
extern NVME_CONFIG   *NvmeConfigList[] ;
extern *NvmeConfigNameList[];


//Hugc-20211222-Add Riser/NVMe control function <<<

#define FchSmnGpio                      0x02D02500
extern UINT8 CpmSmnGetGpio (\
  IN       VOID                        *This,\
  IN       UINT16                      Socket,\
  IN       UINT16                      Die,\
  IN       UINT16                      Pin\
  );
#if 0
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
#endif
/**
GC_TODO: CheckGPIOSettingForNVMe
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
EFI_STATUS
CheckGPIOSettingForNVMe(
  IN    VOID                  *This,
  IN    UINT8                 TableIndex,
  IN    NVME_CONFIG           *NvmeConfig
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumberNVMe];

  GPIOCONFIG      *GPIOConfig;


  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumberNVMe; iGpioIndex++)
  {
    /* code */
    GPIOConfig = &NvmeConfig[TableIndex].GpioMap[iGpioIndex];

    if(GPIOConfig->GpioNumber != INVALID_SETTING) 
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig->SocketNumber,GPIOConfig->DieNumber,GPIOConfig->GpioNumber);
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
  }
  
  //Compare All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumberNVMe; iGpioIndex++)
  { 
    GPIOConfig = &NvmeConfig[TableIndex].GpioMap[iGpioIndex];
    if(( GPIOSetting[iGpioIndex] != INVALID_SETTING) && (GPIOSetting[iGpioIndex]  != GPIOConfig->Validvalue))
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
/**
GC_TODO: DxioDump
@param  CpmTablePpiPtr - GC_TODO: DXIO PPI Pointer

@retval NULL   - GC_TODO: add retval description
**/
void DXIOUpdateForNVMe(
  IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
)
{
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable;
  UINT8                               iDieNumber=0;
  UINT8                               iHotPlugPortList=0,DieLanBase=0;
  UINT8                               DieLanNumber[8]={0,32,80,112,0,32,80,112};
  UINT8                               PortStartLane,PortEndLane;
  UINT8                               iDXIOTable;
  //UINT32                              NVMeValidPortMap=0;
  NVME_CONFIG                         *NvmeConfig=NvmeConfigList[0];
  UINT8                               iNVMeCount=0;
 UINT8                               *pValidNvmeInstalledList=(UINT8*)PcdGetPtr(ValidNvmePortMap);

  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
  DEBUG((EFI_D_ERROR, "==================================================================================\n"));

  if((PcdGet8(PcdInstalledCPUS) < 5) && (NVME_CONFIG_MAP_COUNT == 2))
  {
    NvmeConfig = NvmeConfigList[1];
    DEBUG((EFI_D_ERROR, "Table:%a(%x) will be installed\n\n",NvmeConfigNameList[1],NvmeConfig));
  }

  for (iHotPlugPortList = 0; NvmeConfig[iHotPlugPortList].DieNumber!=INVALID_SETTING; iHotPlugPortList++)
  {
    /* code */
    if(EFI_ERROR(CheckGPIOSettingForNVMe(CpmTablePpiPtr,iHotPlugPortList,NvmeConfig)))
      continue;

    if(NvmeConfig[iHotPlugPortList].DieNumber < DIE4)
      pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
    else 
      pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);

    DEBUG((EFI_D_ERROR, "DieNumber: %x  StartLane:%03d ~ %03d ",NvmeConfig[iHotPlugPortList].DieNumber,NvmeConfig[iHotPlugPortList].StartLane,NvmeConfig[iHotPlugPortList].EndLane));
    DieLanBase = DieLanNumber[NvmeConfig[iHotPlugPortList].DieNumber];
    PortStartLane = DieLanBase + NvmeConfig[iHotPlugPortList].StartLane;
    PortEndLane = DieLanBase + NvmeConfig[iHotPlugPortList].EndLane;

    DEBUG((EFI_D_ERROR, "Base:%03d StartLane_T:%03d ~ %03d ,Bytemap = %d,SlaveAddress:0x%02x",DieLanBase,PortStartLane,PortEndLane,NvmeConfig[iHotPlugPortList].ByteMap ,NvmeConfig[iHotPlugPortList].SlaveAddress));
    for (iDXIOTable = 0; iDXIOTable < AMD_DXIO_PORT_DESCRIPTOR_SIZE; iDXIOTable++)
    {
    
      if(pDxioTopologyTable->Port[iDXIOTable].EngineData.EngineType != DxioUnusedEngine && \
        pDxioTopologyTable->Port[iDXIOTable].EngineData.StartLane == PortStartLane && \
        pDxioTopologyTable->Port[iDXIOTable].EngineData.EndLane == PortEndLane)
        {
          //Config this Port As Hotplug
          DEBUG((EFI_D_ERROR, " - Hot Plug Enable"));
          pDxioTopologyTable->Port[iDXIOTable].EngineData.HotPluggable = DxioHotplugServerExpress;
          pDxioTopologyTable->Port[iDXIOTable].Port.LinkHotplug = DxioHotplugServerExpress;
          //NVMeValidPortMap = NVMeValidPortMap | (1 << iHotPlugPortList);
          pValidNvmeInstalledList[iNVMeCount]=iHotPlugPortList;
          DEBUG((EFI_D_ERROR, " iNVMeCount = %x,iHotPlugPortList = %x ",iNVMeCount,iHotPlugPortList));
          iNVMeCount++;
          break;
        }
        
    }
    DEBUG((EFI_D_ERROR, "\n"));
  }

  //PcdSet32(ValidNvmePortMap,NVMeValidPortMap);
  PcdSet8(NVMeInstalledCount,iNVMeCount);
  //DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]NVMeValidPortMap:%x\n\n",__FUNCTION__,__LINE__,NVMeValidPortMap));
  DEBUG((EFI_D_ERROR, "==================================================================================\n"));
  return;
}
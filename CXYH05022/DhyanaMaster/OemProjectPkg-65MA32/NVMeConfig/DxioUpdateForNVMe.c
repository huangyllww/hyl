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
#include "NVMeTopy.c"
#include "AutoNVMePortingList.h"

BOARD_NVME_TOPY_CONFIG   *NvmeConfigList[] = {AUTO_NVME_PORTING_LIST NULL};
CHAR8         *NvmeConfigNameList[] = {AUTO_NVME_PORTING_LIST_NAMES NULL};

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
//<xuyj1001-20200401 Support new pcie topo +> 
BOOLEAN
CheckGPIOSettingForNVMe(
        IN    VOID      *This,
        IN    NVME_CONFIG           *NvmeConfig,
        IN    UINT8     Index
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxPortGpioNumber];

  GPIOCONFIG      *GPIOConfig;

  GPIOConfig = NvmeConfig[Index].PortTypeGpioMap;
  //Get All Gpio's Setting
  DEBUG((EFI_D_ERROR,"\n"));
  for (iGpioIndex = 0; iGpioIndex < MaxPortGpioNumber; iGpioIndex++)
  {
    /* code */
    //GPIOConfig = &(BoardPCIeTopyTemp->BoardGpioMap[iGpioIndex]);

    if(GPIOConfig[iGpioIndex].GpioNumber != INVALID_GPIO_SETTING) 
      {
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig[iGpioIndex].SocketNumber,GPIOConfig[iGpioIndex].DieNumber,GPIOConfig[iGpioIndex].GpioNumber);
      }
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
    if(GPIOSetting[iGpioIndex]!=INVALID_SETTING) 
        DEBUG((EFI_D_ERROR,"Cancon CheckGPIOSettingForNVMe GpioIndex =%0x ;GPIOConfig->GpioNumber =%0x ; Value=%0x ; Validvalue=%0x\n",iGpioIndex,GPIOConfig[iGpioIndex].GpioNumber, GPIOSetting[iGpioIndex],GPIOConfig[iGpioIndex].Validvalue));
  }
  //Compare All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxPortGpioNumber; iGpioIndex++)
  { 
    //GPIOConfig = &(BoardPCIeTopyTemp->BoardGpioMap[iGpioIndex]);
    if(( GPIOSetting[iGpioIndex] != INVALID_SETTING) && (GPIOSetting[iGpioIndex]  != GPIOConfig[iGpioIndex].Validvalue))
      return FALSE;
  }

  return TRUE;
}
//<xuyj1001-20200401 Support new pcie topo -> 
/**
GC_TODO: CheckBoardIDGPIOSettingForNVME
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
EFI_STATUS
CheckBoardIDGPIOSettingForNVME(
  IN    VOID      *This,
  IN    BOARD_NVME_TOPY_CONFIG     *BoardNVMETopyTemp
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumber];

  GPIOCONFIG      *GPIOConfig;

  GPIOConfig = BoardNVMETopyTemp->BoardGpioMap;
  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  {
    /* code */
    //GPIOConfig = &(BoardPCIeTopyTemp->BoardGpioMap[iGpioIndex]);

    if(GPIOConfig[iGpioIndex].GpioNumber  != INVALID_GPIO_SETTING) 
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig[iGpioIndex].SocketNumber,GPIOConfig[iGpioIndex].DieNumber,GPIOConfig[iGpioIndex].GpioNumber);
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
   
    DEBUG((EFI_D_ERROR,"Cancon CheckBoardIDGPIOSettingForNVME GpioIndex =%0x ;GPIOConfig->GpioNumber =%0x ; Value=%0x\n",iGpioIndex,GPIOConfig[iGpioIndex].GpioNumber, GPIOSetting[iGpioIndex]));//<xuyj1001-20200401 Support new pcie topo +> 
  }
  
  //Compare All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  { 
    //GPIOConfig = &(BoardPCIeTopyTemp->BoardGpioMap[iGpioIndex]);
    if(( GPIOSetting[iGpioIndex] != INVALID_SETTING) && (GPIOSetting[iGpioIndex]  != GPIOConfig[iGpioIndex].Validvalue))
      return FALSE;
  }

  return TRUE;
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
  UINT32                              NVMeValidPortMap=0;
  NVME_CONFIG                         *NvmeConfig=NULL;//NvmeConfigList[0];
  UINT8                               iNVMeCount=0;
  BOARD_NVME_TOPY_CONFIG              *BoardNVMETopyTemp=NULL;
  UINTN                               CountNumber;
 // UINTN                               NumberOfInstalledProcessors;
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
  DEBUG((EFI_D_ERROR, "==================================================================================\n"));

/*  NumberOfInstalledProcessors = PcdGet32(PcdNumberOfInstalledProcessors);
  if ((NumberOfInstalledProcessors == 1)&& (NVME_CONFIG_MAP_COUNT == 2))
  {
    NvmeConfig = NvmeConfigList[1];
    DEBUG((EFI_D_ERROR, "Table:%a(%x) will be installed\n\n",NvmeConfigNameList[1],NvmeConfig));
  }*/

  for(CountNumber=0; NvmeConfigList[CountNumber]!=NULL; CountNumber++){
      BoardNVMETopyTemp=NvmeConfigList[CountNumber];
      if(BoardNVMETopyTemp!=NULL){
      if(CheckBoardIDGPIOSettingForNVME(CpmTablePpiPtr,BoardNVMETopyTemp))
      { 
          NvmeConfig=BoardNVMETopyTemp->NVMETopy;
          PcdSet8(PcdBoardIdSwitchNumber,(UINT8)CountNumber);
          break;
      } 
      }
  }
  if(NvmeConfig==NULL)
  {
      DEBUG((EFI_D_ERROR,"SetNbioHotplugDescriptorProtocol Can't find this correct board id\n"));
      PcdSet8(PcdBoardIdSwitchNumber,0xFF);
      return ;
  }
  for (iHotPlugPortList = 0; NvmeConfig[iHotPlugPortList].DieNumber!=INVALID_SETTING; iHotPlugPortList++)
  {
    /* code */
   // if(EFI_ERROR(CheckGPIOSettingForNVMe(CpmTablePpiPtr,iHotPlugPortList,NvmeConfig)))
   //   continue;
   //<xuyj1001-20200401 Support new pcie topo +> 
    if(!CheckGPIOSettingForNVMe(CpmTablePpiPtr,NvmeConfig,iHotPlugPortList))
          continue;
   //<xuyj1001-20200401 Support new pcie topo -> 
    if(NvmeConfig[iHotPlugPortList].DieNumber < DIE4)
      pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
    else 
      pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);

    DEBUG((EFI_D_ERROR, "DieNumber: %x  StartLane:%03d ~ %03d ",NvmeConfig[iHotPlugPortList].DieNumber,NvmeConfig[iHotPlugPortList].StartLane,NvmeConfig[iHotPlugPortList].EndLane));
    DieLanBase = DieLanNumber[NvmeConfig[iHotPlugPortList].DieNumber];
    PortStartLane = DieLanBase + NvmeConfig[iHotPlugPortList].StartLane;
    PortEndLane = DieLanBase + NvmeConfig[iHotPlugPortList].EndLane;

    DEBUG((EFI_D_ERROR, "Base:%03d StartLane_T:%03d ~ %03d SlaveAddress:0x%02x",DieLanBase,PortStartLane,PortEndLane,NvmeConfig[iHotPlugPortList].SlaveAddress));
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
          NVMeValidPortMap = NVMeValidPortMap | (1 << iHotPlugPortList);
          iNVMeCount++;
          break;
        }
        
    }
    DEBUG((EFI_D_ERROR, "\n"));
  }

  PcdSet32(ValidNvmePortMap,NVMeValidPortMap);
  PcdSet8(NVMeInstalledCount,iNVMeCount);
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]NVMeValidPortMap:%x\n\n",__FUNCTION__,__LINE__,NVMeValidPortMap));
  DEBUG((EFI_D_ERROR, "==================================================================================\n"));
  return;
}

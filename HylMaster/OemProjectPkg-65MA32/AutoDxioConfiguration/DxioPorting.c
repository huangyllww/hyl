#include "AutoDxioConfiguration.h"
#include "PCIeTopy.c"
#include "AutoDxioFunctionList.h"

BOARD_PCIE_TOPY_CONFIG    *BoardPcieTopy[] = {AUTO_PCIE_PORTING_LIST NULL};
CHAR8                   *BoardPcieTopyNameList[] = {AUTO_PCIE_PORTING_LIST_NAMES NULL};

UINT8       DieLanNumber[8]={0,32,80,112,0,32,80,112};
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define FchSmnGpio                      0x02D02500
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
GC_TODO: CheckGPIOSetting
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
/*EFI_STATUS
CheckGPIOSetting(
  IN    VOID      *This,
  IN    UINT8     TableIndex,
  IN    PCIE_TOPY_CONFIG *PcieTopy
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumber];

  GPIOCONFIG      *GPIOConfig;

  GPIOConfig = PcieTopy[TableIndex].GpioMap;
  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  {
    // code 
   // GPIOConfig = &(PcieTopy[TableIndex].GpioMap[iGpioIndex]);
      
    if(GPIOConfig->GpioNumber != INVALID_GPIO_SETTING) 
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig[iGpioIndex].SocketNumber,GPIOConfig[iGpioIndex].DieNumber,GPIOConfig[iGpioIndex].GpioNumber);
    else
      GPIOSetting[iGpioIndex] = INVALID_GPIO_SETTING;
    DEBUG((EFI_D_INFO,"Cancon PcieTopy GpioIndex =%0x ;GPIOConfig->GpioNumber =%0x ; Value=%0x\n",iGpioIndex,GPIOConfig[iGpioIndex].GpioNumber, GPIOSetting[iGpioIndex]));
  }
  
  //Compare All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  { 
    //GPIOConfig = &(PcieTopy[TableIndex].GpioMap[iGpioIndex]);
    if(( GPIOSetting[iGpioIndex] != INVALID_GPIO_SETTING) && (GPIOSetting[iGpioIndex]  != GPIOConfig[iGpioIndex].Validvalue))
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}*/

/**
GC_TODO: CheckBoardIDGPIOSetting
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
//<xuyj1001-20200401 Support new pcie topo +> 
BOOLEAN
CheckPortGPIOSetting(
        IN    VOID      *This,
        IN    PCIE_TOPY_CONFIG     *BoardPCIeTopyTemp,
        IN    UINT8     Index
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxPortGpioNumber];

  GPIOCONFIG      *GPIOConfig;

  GPIOConfig = BoardPCIeTopyTemp[Index].PortTypeGpioMap;
  //Get All Gpio's Setting
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
    DEBUG((EFI_D_INFO,"Cancon CheckPortGPIOSetting GpioIndex =%0x ;GPIOConfig->GpioNumber =%0x ; Value=%0x ; Validvalue=%0x\n",iGpioIndex,GPIOConfig[iGpioIndex].GpioNumber, GPIOSetting[iGpioIndex],GPIOConfig[iGpioIndex].Validvalue));
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

/**
GC_TODO: CheckBoardIDGPIOSetting
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
BOOLEAN
CheckBoardIDGPIOSetting(
  IN    VOID      *This,
  IN    BOARD_PCIE_TOPY_CONFIG     *BoardPCIeTopyTemp,
  UINT8 *BoardGPIOValue
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumber];
  UINT8           GPIOValue=0;

  GPIOCONFIG      *GPIOConfig;

  GPIOConfig = BoardPCIeTopyTemp->BoardGpioMap;
  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  {
    /* code */
    //GPIOConfig = &(BoardPCIeTopyTemp->BoardGpioMap[iGpioIndex]);

    if(GPIOConfig[iGpioIndex].GpioNumber != INVALID_GPIO_SETTING) 
      {
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig[iGpioIndex].SocketNumber,GPIOConfig[iGpioIndex].DieNumber,GPIOConfig[iGpioIndex].GpioNumber);
      GPIOValue =(GPIOValue | ((GPIOSetting[iGpioIndex])<<(MaxGpioNumber-iGpioIndex-1)));
      }
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
    
    DEBUG((EFI_D_INFO,"Cancon CheckBoardIDGPIOSetting GpioIndex =%0x ;GPIOConfig->GpioNumber =%0x ; Value=%0x\n",iGpioIndex,GPIOConfig[iGpioIndex].GpioNumber, GPIOSetting[iGpioIndex]));
  }
  *BoardGPIOValue=GPIOValue;
  DEBUG((EFI_D_INFO,"Cancon CheckBoardIDGPIOSetting BoardGPIOValue Value=%0x;GPIOValue=%0x\n",*BoardGPIOValue,GPIOValue));
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
GC_TODO: add routine description
@param  DxioTopologyTablePtr  - GC_TODO: PPI Pointer
@param  StartLane             - GC_TODO: Start Lane Number Of The Port
@param  EndLane               - GC_TODO: End Lane Number Of The Port
@param  LaneType              - GC_TODO: Sata/PCIE

@retval EFI_SUCCESS   - GC_TODO: Config Success
@retval EFI_NOT_FOUND - GC_TODO: Config Fail
**/
EFI_STATUS
DxioEnableTable (
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr, 
  UINT8 StartLane,
  UINT8 EndLane,
  UINT8 LaneType,
  UINT8 HotplugSupport
  )
{
  UINT8                               Index;
  UINT8                               DevNumber=0x00;//,FunctionNumber=0x00;
  UINT8                               EngType=0;

  //DEBUG ((EFI_D_INFO, __FUNCTION__ "Start: StartLane=%d EndLane=%d \n",StartLane,EndLane));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane && DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane ) {
      DevNumber = DxioTopologyTablePtr->Port[Index].Port.DeviceNumber;
      //FunctionNumber = DxioTopologyTablePtr->Port[Index].Port.FunctionNumber;
      if(DevNumber == 0)
        EngType = DxioSATAEngine;
      else
        EngType = DxioPcieEngine;
      
      if((LaneType & EngType) == DxioSATAEngine)
        DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioSATAEngine; 
      else if((LaneType & EngType) == DxioPcieEngine)
        DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioPcieEngine; 
      else
        return EFI_NOT_FOUND;
      //<xuyj1202010406 Add ocp hotplug support +>
      if(HotplugSupport==1){
      DEBUG((EFI_D_INFO, " - Hot Plug Enable"));
      DxioTopologyTablePtr->Port[Index].EngineData.HotPluggable = DxioHotplugServerExpress;
      DxioTopologyTablePtr->Port[Index].Port.LinkHotplug = DxioHotplugServerExpress;
      }
      //<xuyj1202010406 Add ocp hotplug support ->
      //DEBUG((EFI_D_INFO, "DxioEnableTable [Cancon.%d.%a]StartLane:%d EndLane:%d EngType:%x\n",__LINE__,__FUNCTION__,StartLane,EndLane,EngType));       
      return EFI_SUCCESS;
    }        
  }        
  DEBUG((EFI_D_INFO, "DxioEnableTable [Cancon.%d.%a]StartLane:%d EndLane:%d Fail!!!!!\n",__LINE__,__FUNCTION__,StartLane,EndLane));   
  DEBUG((EFI_D_INFO, "DxioEnableTable [OemDbg][%a][%d]Cab Not Found The Match Setting,Please Double Check The CPM Table!!!!!\n\n",__FUNCTION__,__LINE__));
  return EFI_NOT_FOUND;
}

/**
GC_TODO: Update DXIO Table
@param AMD_CPM_TABLE_PPI  *CpmTablePpiPtr

@retval VOID
**/
void UpdateDxioSetting(
  IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
)
{
  UINT8                               iPTL=0,DieLanBase=0;
  EFI_STATUS                          Status = EFI_SUCCESS;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable_Config;
  UINTN                               NumberOfInstalledProcessors;
  UINTN                               i;
  BOARD_PCIE_TOPY_CONFIG              *BoardPCIeTopyTemp=NULL;
  PCIE_TOPY_CONFIG                    *PcieTopy=NULL;
  UINT8                                 BoardGPIOValue=0;
  UINT8                                 OcpDetectValue=0xff;//<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 >
  
  //Get the installed CPU Number
  //Try to read D18F1X204  System Component Count
 
  NumberOfInstalledProcessors = CpmTablePpiPtr->CommonFunction.SmnRead32(CpmTablePpiPtr,0,0,0x1c400,0x204);
  NumberOfInstalledProcessors = NumberOfInstalledProcessors & 0x0F; // Now, we got the DieNumber
  NumberOfInstalledProcessors = (NumberOfInstalledProcessors > 4)?2:1;
  PcdSet32(PcdNumberOfInstalledProcessors,NumberOfInstalledProcessors); //#mizl-20201014-Update For Signle CPU NVMe Config
  //DEBUG((EFI_D_INFO, "[Cancon.%d.%a]Installed CPU Number:%x\n",__LINE__,__FUNCTION__,InstalledCPUs));
  //Now we Try to Loop DxioTopo Table
  //Now we need get CPU Number installed

   
  DEBUG((EFI_D_INFO,"UpdateDxioSetting Entry\n"));
  for(i=0; BoardPcieTopy[i]!=NULL; i++){
      BoardPCIeTopyTemp=BoardPcieTopy[i];
      if(BoardPCIeTopyTemp!=NULL){
      if(CheckBoardIDGPIOSetting(CpmTablePpiPtr,BoardPCIeTopyTemp,&BoardGPIOValue))
      { 
          PcdSet8(PcdBoardGpioVaule,BoardGPIOValue);
          PcieTopy=BoardPCIeTopyTemp->PcieTopy;
          break;
      } 
      }
  }
  if(PcieTopy==NULL)
  {
      PcdSet8(PcdBoardGpioVaule,0xff);
      DEBUG((EFI_D_INFO,"UpdateDxioSetting Can't find this correct board id\n"));
      return ;
  }
  DEBUG((EFI_D_INFO,"USED PCIe List is %a ;PCIe BIF GPIO number is %0x;BIF GPIO PCD Value is %0x\n",BoardPcieTopyNameList[i],BoardGPIOValue,PcdGet8(PcdBoardGpioVaule)));
  
  NumberOfInstalledProcessors = PcdGet32(PcdNumberOfInstalledProcessors);
  for(iPTL=0;PcieTopy[iPTL].StartLane != INVALID_SETTING && PcieTopy[iPTL].DieNumber < (NumberOfInstalledProcessors*4);iPTL++)
  {
    DEBUG((EFI_D_INFO, "====================================================================================================\n"));
    DEBUG((EFI_D_INFO, "UpdateDxioSetting [Cancon.%d.%a]iPTL:%d PcieTopy[iPTL].DieNumber:%d StartLane:%d  EndLane:%d\n",__LINE__,__FUNCTION__,iPTL,PcieTopy[iPTL].DieNumber,PcieTopy[iPTL].StartLane,PcieTopy[iPTL].EndLane));
   //<xuyj1001-20200401 Support new pcie topo +> 
    if(!CheckPortGPIOSetting(CpmTablePpiPtr,PcieTopy,iPTL))
        continue;
   //<xuyj1001-20200401 Support new pcie topo -> 
    //Try to get DXIO Table
    if(PcieTopy[iPTL].DieNumber < DIE4) pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
    else pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);

    DieLanBase = DieLanNumber[PcieTopy[iPTL].DieNumber];

    //Status = CheckGPIOSetting(CpmTablePpiPtr,iPTL,PcieTopy);
   // if(EFI_ERROR(Status))
    //  continue;
    
    DxioEnableTable(pDxioTopologyTable_Config,DieLanBase + PcieTopy[iPTL].StartLane,DieLanBase + PcieTopy[iPTL].EndLane,PcieTopy[iPTL].LaneType,PcieTopy[iPTL].OCPHotplugSupport);
  }
  //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 +>
  OcpDetectValue=CpmSmnGetGpio((VOID*)CpmTablePpiPtr,0,0,136);//OCP detect GPIO number 
  DEBUG((EFI_D_INFO,"[OemDbg][%a][%d] OCPDetectValue=%0x\n",__FUNCTION__,__LINE__,OcpDetectValue));
  PcdSet8(PcdOcpDetectValue,OcpDetectValue);
  //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 ->
  return;
}

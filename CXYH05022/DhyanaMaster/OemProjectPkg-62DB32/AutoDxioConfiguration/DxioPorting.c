#include "AutoDxioConfiguration.h"
#include "PCIeTopy.c"
#include "AutoDxioFunctionList.h"

PCIE_TOPY_CONFIG    *PcieTopy = AUTO_PCIE_PORTING_LIST;

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
GC_TODO: DumpPCIeConfig
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
DumpPCIeConfig(
  IN    VOID      *This,
  IN    UINT8     TableIndex
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumber];

  GPIOCONFIG      *GPIOConfig;
  DEBUG((EFI_D_ERROR, "------------------------------------------------------------------\n"));
  DEBUG((EFI_D_ERROR, "Die:%x Lane:%d~%d Enabled\n",PcieTopy[TableIndex].DieNumber,PcieTopy[TableIndex].StartLane,PcieTopy[TableIndex].EndLane));
  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  {
    /* code */

    GPIOConfig = &PcieTopy[TableIndex].GpioMap[iGpioIndex];

    if(GPIOConfig->GpioNumber != INVALID_SETTING) 
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig->SocketNumber,GPIOConfig->DieNumber,GPIOConfig->GpioNumber);
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
    
    DEBUG((EFI_D_ERROR, "GPIO%03d - %a = %d\n",GPIOConfig->GpioNumber,GPIOConfig->GpioDescription,GPIOSetting[iGpioIndex]));
  }

  return EFI_SUCCESS;
}
/**
GC_TODO: CheckGPIOSetting
@This         - CpmTablePpiPtr:  PPI Pointer
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
EFI_STATUS
CheckGPIOSetting(
  IN    VOID      *This,
  IN    UINT8     TableIndex
)
{
  UINT8           iGpioIndex;

  UINT8           GPIOSetting[MaxGpioNumber];

  GPIOCONFIG      *GPIOConfig;


  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  {
    /* code */
    GPIOConfig = &PcieTopy[TableIndex].GpioMap[iGpioIndex];

    if(GPIOConfig->GpioNumber != INVALID_SETTING) 
      GPIOSetting[iGpioIndex] = CpmSmnGetGpio(This,GPIOConfig->SocketNumber,GPIOConfig->DieNumber,GPIOConfig->GpioNumber);
    else
      GPIOSetting[iGpioIndex] = INVALID_SETTING;
  }
  
  //Compare All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioNumber; iGpioIndex++)
  { 
    GPIOConfig = &PcieTopy[TableIndex].GpioMap[iGpioIndex];
    if(( GPIOSetting[iGpioIndex] != INVALID_SETTING) && (GPIOSetting[iGpioIndex]  != GPIOConfig->Validvalue))
      return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
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
  UINT8 PreFixedDev,
  UINT8 PreFixedFun
  )
{
  UINT8                               Index;
  UINT8                               DevNumber=0x00;//,FunctionNumber=0x00;
  UINT8                               EngType=0;

  //DEBUG ((DEBUG_ERROR, __FUNCTION__ "Start: StartLane=%d EndLane=%d \n",StartLane,EndLane));     
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
              { 
        DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioPcieEngine; 
        DxioTopologyTablePtr->Port[Index].Port.DeviceNumber= PreFixedDev;
        DxioTopologyTablePtr->Port[Index].Port.FunctionNumber = PreFixedFun;
      }
      else
        return EFI_NOT_FOUND;
      
      //DEBUG((EFI_D_ERROR, "[Cancon.%d.%a]StartLane:%d EndLane:%d EngType:%x\n",__LINE__,__FUNCTION__,StartLane,EndLane,EngType));       
      return EFI_SUCCESS;
    }        
  }        
  DEBUG((EFI_D_ERROR, "[Cancon.%d.%a]StartLane:%d EndLane:%d Fail!!!!!\n",__LINE__,__FUNCTION__,StartLane,EndLane));   
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Cab Not Found The Match Setting,Please Double Check The CPM Table!!!!!\n\n",__FUNCTION__,__LINE__));
  return EFI_NOT_FOUND;
}

#define GetCfgData(DieName,CfgData)  {\
    switch(DieName){\
      case 0:\
        CfgData=PcdGet64(PcdDie0PCIeCfgIndex);\
        break;\
      case 1:\
        CfgData=PcdGet64(PcdDie1PCIeCfgIndex);\
        break;\
      case 2:\
        CfgData=PcdGet64(PcdDie2PCIeCfgIndex);\
        break;\
      case 3:\
        CfgData=PcdGet64(PcdDie3PCIeCfgIndex);\
        break;\
      case 4:\
        CfgData=PcdGet64(PcdDie4PCIeCfgIndex);\
        break;\
      case 5:\
        CfgData=PcdGet64(PcdDie5PCIeCfgIndex);\
        break;\
      case 6:\
        CfgData=PcdGet64(PcdDie6PCIeCfgIndex);\
        break;\
      case 7:\
        CfgData=PcdGet64(PcdDie7PCIeCfgIndex);\
        break;\
      default:\
        CfgData=0xFFFFFFFFFFFFFFFF;\
    }\
}

#define SetCfgData(DieName,CfgData)  {\
    switch(DieName){\
      case 0:\
        PcdSet64(PcdDie0PCIeCfgIndex,CfgData);\
        break;\
      case 1:\
        PcdSet64(PcdDie1PCIeCfgIndex,CfgData);\
        break;\
      case 2:\
        PcdSet64(PcdDie2PCIeCfgIndex,CfgData);\
        break;\
      case 3:\
        PcdSet64(PcdDie3PCIeCfgIndex,CfgData);\
        break;\
      case 4:\
        PcdSet64(PcdDie4PCIeCfgIndex,CfgData);\
        break;\
      case 5:\
        PcdSet64(PcdDie5PCIeCfgIndex,CfgData);\
        break;\
      case 6:\
        PcdSet64(PcdDie6PCIeCfgIndex,CfgData);\
        break;\
      case 7:\
        PcdSet64(PcdDie7PCIeCfgIndex,CfgData);\
        break;\
      default:\
        break;\
    }\
}

VOID
SyncDiexPCIeCfgIndex(
  IN  UINT8   DieIndex,
  IN  UINT8   TableOffset
)
{
  UINT64    CfgIndex=0xFFFFFFFFFFFFFFFF;
  UINT8     iOffset=0,TempData,LeftShift;
  
  GetCfgData(DieIndex,CfgIndex);

  DEBUG ((DEBUG_ERROR, "%a %d  Org Cfg=%lx  Current Offset=%x\n", __FUNCTION__, __LINE__,CfgIndex,TableOffset));
  for(iOffset=0;iOffset<8;iOffset++)
  {
      LeftShift=iOffset*8;
      TempData= (UINT8)(CfgIndex >> LeftShift);
      if(TempData != 0xFF)
        continue;
      
      //Clear First
      CfgIndex&=~((UINT64)(0xFF << LeftShift));

      //Set Offset
      CfgIndex|=(UINT64)(TableOffset<<(iOffset*8));

      //Update PCD
      SetCfgData(DieIndex,CfgIndex);
      break;
        
  }
  GetCfgData(DieIndex,CfgIndex);

  DEBUG ((DEBUG_ERROR, "%a %d Die%d PCIe Config=%lx \n", __FUNCTION__, __LINE__,DieIndex,CfgIndex));
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
  UINT32                              InstalledCPUs;
  UINT8                               iPTL=0,DieLanBase=0;
  EFI_STATUS                          Status = EFI_SUCCESS;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable_Config;
  //Get the installed CPU Number
  //Try to read D18F1X204  System Component Count
  InstalledCPUs = CpmTablePpiPtr->CommonFunction.SmnRead32(CpmTablePpiPtr,0,0,0x1c400,0x204);
  InstalledCPUs = InstalledCPUs & 0x0F; // Now, we got the DieNumber
  //InstalledCPUs = (InstalledCPUs > 4)?2:1;
  PcdSet8(PcdInstalledCPUS,InstalledCPUs); //#mizl-20201014-Update For Signle CPU NVMe Config
  //DEBUG((EFI_D_ERROR, "[Cancon.%d.%a]Installed CPU Number:%x\n",__LINE__,__FUNCTION__,InstalledCPUs));
  //Now we Try to Loop DxioTopo Table
  //Now we need get CPU Number installed
  for(iPTL=0;PcieTopy[iPTL].StartLane != INVALID_SETTING && PcieTopy[iPTL].DieNumber < InstalledCPUs;iPTL++)
  {
    //DEBUG((EFI_D_ERROR, "====================================================================================================\n"));
    //DEBUG((EFI_D_ERROR, "[Cancon.%d.%a]iPTL:%d StartLane:%d  EndLane:%d\n",__LINE__,__FUNCTION__,iPTL,PcieTopy[iPTL].StartLane,PcieTopy[iPTL].EndLane));

    //Try to get DXIO Table
    if(PcieTopy[iPTL].DieNumber < DIE4) pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
    else pDxioTopologyTable_Config= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);

    DieLanBase = DieLanNumber[PcieTopy[iPTL].DieNumber];

    Status = CheckGPIOSetting(CpmTablePpiPtr,iPTL);
    if(EFI_ERROR(Status))
      continue;
    DumpPCIeConfig(CpmTablePpiPtr,iPTL);
        DxioEnableTable(pDxioTopologyTable_Config,DieLanBase + PcieTopy[iPTL].StartLane,DieLanBase + PcieTopy[iPTL].EndLane,PcieTopy[iPTL].LaneType,\
                    PcieTopy[iPTL].PreFixedDev,PcieTopy[iPTL].PreFixedFun);
        //Sync PCD Variable DiexPCIeCfgIndex
        SyncDiexPCIeCfgIndex(PcieTopy[iPTL].DieNumber,iPTL);

  }
  return;
}
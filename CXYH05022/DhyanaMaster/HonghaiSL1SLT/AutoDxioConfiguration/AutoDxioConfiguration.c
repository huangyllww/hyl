#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Ppi/Pca9545aPpi.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>
#include "CustomerCpmTable.h"
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

EFI_STATUS OverideCpmTable (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
    );
EFI_STATUS  SetupPca9546A(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi);
//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gAmdCpmTablePpiGuid, OverideCpmTable},
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gPca9545aPpiGuid, SetupPca9546A}
};

/**
 * clear DXIO port data to 0 in a CPM table
 */
VOID
CpmTableClearPortData(AMD_CPM_DXIO_TOPOLOGY_TABLE* CpmTable)
{
    DXIO_PORT_DESCRIPTOR* PortDsc;
    
    PortDsc = &CpmTable->Port[0];
    ZeroMem(PortDsc,sizeof(DXIO_PORT_DESCRIPTOR)*AMD_DXIO_PORT_DESCRIPTOR_SIZE);    
}

/**
 * Update DXIO port data in a CPM table
 */
VOID
CpmTableUpdatePortData(AMD_CPM_DXIO_TOPOLOGY_TABLE* CpmTable, DXIO_PORT_DESCRIPTOR* SourcePortDsc, UINT8 PortDataCount)
{
    DXIO_PORT_DESCRIPTOR* TargetPortDsc;
    
    TargetPortDsc = &CpmTable->Port[0];
    CopyMem(TargetPortDsc,SourcePortDsc,PortDataCount*sizeof(DXIO_PORT_DESCRIPTOR));
    //set table end flag
    CpmTable->Port[PortDataCount-1].Flags = DESCRIPTOR_TERMINATE_LIST;
}

/**
 * Attach Customer Data to Port data descriptor
 * @param PortDsc : target PortDsc
 * @param CustomerPortData : CoustomerPortData
 * @param StartLane : DXIO start lane number
 * @return -1: Attach Customer Data error
 *          N: Attach port data count
 */
INT8
PortDataDscAttachCustomerData(DXIO_PORT_DESCRIPTOR* PortDsc,CUSTOMER_DXIO_PLAN* CustomerPortData,UINT8 StartLane)
{
    static INT8            PortCount=0;
    UINT8*                 PortDscEnd;
    DXIO_PORT_DESCRIPTOR*  pPortDsc;
    UINT8                  i;
    UINT8                  DevNumber=1,FunNumber=1;
    
    if((StartLane%16!=0)||PortCount>AMD_DXIO_PORT_DESCRIPTOR_SIZE){
        return -1;
    }
    PortDscEnd = ((UINT8*)PortDsc)+PortCount*sizeof(DXIO_PORT_DESCRIPTOR);
    pPortDsc   = (DXIO_PORT_DESCRIPTOR*)PortDscEnd;
    CopyMem(PortDscEnd,CustomerPortData->DxioPortConfig,CustomerPortData->DxioPortCount*sizeof(DXIO_PORT_DESCRIPTOR));
    PortCount+=CustomerPortData->DxioPortCount;

    //update start lane/end lane number
    for(i=0;i<CustomerPortData->DxioPortCount;i++){
        pPortDsc->EngineData.StartLane+=StartLane;
        pPortDsc->EngineData.EndLane+=StartLane;
        if(pPortDsc->EngineData.EngineType==DxioPcieEngine){
            //Update RB Device/Function number
            if(StartLane%32==0){
                pPortDsc->Port.DeviceNumber = DevNumber;
                pPortDsc->Port.FunctionNumber = FunNumber;
                FunNumber++;
                if(FunNumber==8){
                    FunNumber=0;
                    DevNumber++;
                }
            } else {
                pPortDsc->Port.DeviceNumber = DevNumber+2;
                pPortDsc->Port.FunctionNumber = FunNumber;
                FunNumber++;
                if(FunNumber==8){
                    FunNumber=0;
                    DevNumber++;
                }
            }
        }
        pPortDsc++;
    }
    
    return PortCount;
}

/**
 * Build customer port data descriptor
 */
INT8
BuildCustomerPortDataDsc(IN EFI_PEI_SERVICES **PeiServices,DXIO_PORT_DESCRIPTOR* PortDsc)
{
    EFI_STATUS                       Status;
    UINT8                            PortDataCount;
    SETUP_DATA                       SetupData;
    UINTN                            VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    EFI_GUID                         SetupGuid= SETUP_GUID;
    DXIO_PORT_DESCRIPTOR*            pPortDsc;
#if 0
    UINT8                            i;
    UINT8                            FieldCardOnDie=0;
    EFI_PEI_PCA9545A_PPI             *Pca9545aPpi;
#endif    
    Status = (*PeiServices)->LocatePpi(
                PeiServices,
                &gEfiPeiReadOnlyVariable2PpiGuid,
                0, 
                NULL,
                &ReadOnlyVariable
                );
    VariableSize = sizeof(SETUP_DATA);
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,NULL, &VariableSize, &SetupData);
    if(EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "BuildCustomerPortDataDsc: GetVariable error %r, use default value!\n", Status));      
        return -1;
    }
    if(SetupData.EnableDxioCfg==0){
        return -1;
    }
    DEBUG ((DEBUG_ERROR, " SetupData.DieDxioType0Conf[0]:%d\n",SetupData.DieDxioType0Conf[0])); 
    DEBUG ((DEBUG_ERROR, " SetupData.DieDxioType0Conf[1]:%d\n",SetupData.DieDxioType0Conf[1]));
    DEBUG ((DEBUG_ERROR, " SetupData.DieDxioType0Conf[2]:%d\n",SetupData.DieDxioType0Conf[2]));
    DEBUG ((DEBUG_ERROR, " SetupData.DieDxioType0Conf[3]:%d\n",SetupData.DieDxioType0Conf[3]));
#if 0
    Status = (*PeiServices)->LocatePpi (
       PeiServices,
       &gPca9545aPpiGuid,
       0,
       NULL,
       &Pca9545aPpi
       );
    if(!EFI_ERROR(Status)){
        for(i=0;i<4;i++)
        {
            if(SetupData.DieDxioType0Conf[i]==1){
                FieldCardOnDie=i;
                break;
            }
        }
        //Config PCA9546
        Pca9545aPpi->Set(PeiServices, 0, 0xE0, 1<<FieldCardOnDie);
    } 
#endif
    //Die 0 Type A
    DEBUG ((DEBUG_ERROR, "Die 0 DXIO Type A AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die0Type0DxioConfigPlan[SetupData.DieDxioType0Conf[0]],0);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 0 Type B
    DEBUG ((DEBUG_ERROR, "Die 0 DXIO Type B AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die0Type1DxioConfigPlan[SetupData.DieDxioType1Conf[0]],16);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 1 Type A
    DEBUG ((DEBUG_ERROR, "Die 1 DXIO Type A AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die1Type0DxioConfigPlan[SetupData.DieDxioType0Conf[1]],32);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 1 Type B
    DEBUG ((DEBUG_ERROR, "Die 1 DXIO Type B AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die1Type1DxioConfigPlan[SetupData.DieDxioType1Conf[1]],48);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 2 Type A
    DEBUG ((DEBUG_ERROR, "Die 2 DXIO Type A AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die2Type0DxioConfigPlan[SetupData.DieDxioType0Conf[2]],64);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 2 Type B
    DEBUG ((DEBUG_ERROR, "Die 2 DXIO Type B AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die2Type1DxioConfigPlan[SetupData.DieDxioType1Conf[2]],80);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 3 Type A
    DEBUG ((DEBUG_ERROR, "Die 3 DXIO Type A AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die3Type0DxioConfigPlan[SetupData.DieDxioType0Conf[3]],96);
    if(PortDataCount<0){
        return PortDataCount;
    }
    //Die 3 Type B
    DEBUG ((DEBUG_ERROR, "Die 3 DXIO Type B AttachCoustomerData\n"));
    PortDataCount = PortDataDscAttachCustomerData(PortDsc,&Die3Type1DxioConfigPlan[SetupData.DieDxioType1Conf[3]],112);
    
    //update start lane/end lane number
    pPortDsc=PortDsc+PortDataCount;
    pPortDsc->Flags = DESCRIPTOR_TERMINATE_LIST;
    return PortDataCount;
}
/**
 * Dump Customer configuration
 */
VOID
DumpCustomerCpmTable(AMD_CPM_DXIO_TOPOLOGY_TABLE* CustomerCpmTable)
{
    DXIO_PORT_DESCRIPTOR*        PortDsc;
    UINT8                        i;
    
    PortDsc =  &CustomerCpmTable->Port[0];
    DEBUG ((DEBUG_ERROR, "<==================DumpCustomerCpmTable=====================>\n"));
    for(i=0;i<AMD_DXIO_PORT_DESCRIPTOR_SIZE;i++) {
        DEBUG ((DEBUG_ERROR, "Engine     = %d\n",PortDsc->EngineData.EngineType));
        DEBUG ((DEBUG_ERROR, "Start Lane = %d\n",PortDsc->EngineData.StartLane));
        DEBUG ((DEBUG_ERROR, "End   Lane = %d\n",PortDsc->EngineData.EndLane));
        if(PortDsc->EngineData.EngineType==DxioPcieEngine){
            DEBUG ((DEBUG_ERROR, "PCIE Dev = %d\n",PortDsc->Port.DeviceNumber));
            DEBUG ((DEBUG_ERROR, "PCIE Fun = %d\n",PortDsc->Port.FunctionNumber));
        }
        if(PortDsc->Flags==DESCRIPTOR_TERMINATE_LIST){
            break;
        }
        PortDsc++;
    }
    DEBUG ((DEBUG_ERROR, "<==================DumpCustomerCpmTable=====================>\n"));
}
/**
 * Override CPM table by customer configuration
 */
EFI_STATUS  
OverideCpmTable (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  EFI_STATUS                   Status;
  AMD_CPM_TABLE_PPI            *CpmTablePpiPtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE  *DxioTopologyTableS0Ptr;
  DXIO_PORT_DESCRIPTOR*        PortDsc=NULL;
  INT8                         PortDataCount;
  
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
  //1.prepare customer DXIO port data by user set
  PortDsc=AllocatePool(sizeof(DXIO_PORT_DESCRIPTOR)*AMD_DXIO_PORT_DESCRIPTOR_SIZE);
  if(PortDsc==NULL){
      return EFI_OUT_OF_RESOURCES;
  }
  DEBUG ((DEBUG_ERROR, "1.prepare customer DXIO port data by user set\n"));
  PortDataCount=BuildCustomerPortDataDsc(PeiServices,PortDsc);
  if(PortDataCount<0){
      return EFI_OUT_OF_RESOURCES;
  }
  //2.clear CPM table port data
  DEBUG ((DEBUG_ERROR, "2.clear CPM table port data"));
  CpmTableClearPortData(DxioTopologyTableS0Ptr);
  //3.update CPM table port data
  DEBUG ((DEBUG_ERROR, "update CPM table port data, Port data count = %d\n",PortDataCount));
  CpmTableUpdatePortData(DxioTopologyTableS0Ptr,PortDsc,PortDataCount);
  DumpCustomerCpmTable(DxioTopologyTableS0Ptr);
  return EFI_SUCCESS;
}
/**
 * 
 */
EFI_STATUS  
SetupPca9546A(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi)
{
    EFI_STATUS                       Status;
    SETUP_DATA                       SetupData;
    UINTN                            VariableSize;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;
    EFI_GUID                         SetupGuid= SETUP_GUID;
    UINT8                            i;
    UINT8                            FieldCardOnDie=0;
    EFI_PEI_PCA9545A_PPI             *Pca9545aPpi;
    
    Status = (*PeiServices)->LocatePpi (
       PeiServices,
       &gPca9545aPpiGuid,
       0,
       NULL,
       &Pca9545aPpi
       );
    if(!EFI_ERROR(Status)){
        Status = (*PeiServices)->LocatePpi(
                    PeiServices,
                    &gEfiPeiReadOnlyVariable2PpiGuid,
                    0, 
                    NULL,
                    &ReadOnlyVariable
                    );
        VariableSize = sizeof(SETUP_DATA);
        Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,NULL, &VariableSize, &SetupData);
        if(EFI_ERROR(Status)) {
            Pca9545aPpi->Set(PeiServices, 0, 0xE0>>1, 1);   
            return Status;
        }
        for(i=0;i<4;i++)
        {
            if(SetupData.DieDxioType0Conf[i]==1){
                FieldCardOnDie=i;
                break;
            }
        }
        //setup PCA9546
        Pca9545aPpi->Set(PeiServices, 0, 0xE0>>1, 1<<FieldCardOnDie);
    }
    
    return EFI_SUCCESS;
}
/**
 * Module entry point
 */
EFI_STATUS
AutoDxioConfigurationInit (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  (*PeiServices)->NotifyPpi( PeiServices, PeiNotifyList );

  return Status;
}

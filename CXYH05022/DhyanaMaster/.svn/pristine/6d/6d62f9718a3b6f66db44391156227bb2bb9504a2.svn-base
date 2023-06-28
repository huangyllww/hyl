//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  ScanNvmeBackPanel.c
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Library/PeiServicesLib.h>
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include <Library/DebugLib.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/Pca9555aPpi.h>
#include <Ppi/Pca9545aPpi.h>
#include "OemLib.h"

#define PCA9555A_INPUT0      0
#define PCA9555A_INPUT1      1
#define PCA9555A_OUTPUT0     2
#define PCA9555A_OUTPUT1     3
#define PCA9555A_CONFIG0     6
#define PCA9555A_CONFIG1     7

#define PCA9555A_PRESENT       BIT4
#define PCA9555A_PWRFAULT      BIT5
#define PCA9555A_ATTENTION     BIT3
#define PCA9555A_EMILS         BIT6
#define PCA9555A_POWER_EN      BIT2
#define PCA9555A_ATTENTION_LED BIT0
#define PCA9555A_POWER_LED     BIT1
#define PCA9555A_EMIL          BIT7


EFI_STATUS EFIAPI ScanNvmeBackPanel_Entry (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS              Status;
    UINT8                   SlaveAddress = 0x20,iI2CBus=0;
    EFI_PEI_PCA9555A_PPI    *Pca9555APpi;
    UINT16                  Data=0;
    UINT8                    Command,DefaultConfig=PCA9555A_PRESENT | PCA9555A_PWRFAULT | PCA9555A_ATTENTION | PCA9555A_EMILS;
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)
    EFI_PEI_PCA9545A_PPI   *Pca9545aPpi;
    UINT8                   iI2CChannel=0;
    UINT8                   ControlByte,ControlByteReserved;
#endif

    MODULE_START
    if (PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE)
        MODULE_END(Status=EFI_UNSUPPORTED)

    Status = (*PeiServices)->LocatePpi (
                                    PeiServices,
                                    &gPca9555aPpiGuid,
                                    0,
                                    NULL,
                                    &Pca9555APpi);
    if (EFI_ERROR(Status))
        MODULE_END(Status)
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)    
    Status = (*PeiServices)->LocatePpi (
                                  PeiServices,
                                  &gPca9545aPpiGuid,
                                  0,
                                  NULL,
                                  &Pca9545aPpi);
    if (EFI_ERROR(Status))
        MODULE_END(Status)
    
    Status = Pca9545aPpi->Get(PeiServices, 0, 0x70, &ControlByteReserved);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Get 0x70 Status:%r ControlByteReserved:%x\n\n",__FUNCTION__,__LINE__,Status,ControlByteReserved));
    if (EFI_ERROR(Status))
        MODULE_END(Status)

    for(iI2CChannel=0; iI2CChannel < 4; iI2CChannel++)
	{
        ControlByte = ControlByteReserved & 0xF0 | (UINT8)(BIT0 << iI2CChannel);
		Status = Pca9545aPpi->Set(PeiServices, 0, 0x70, ControlByte);
        DEBUG((EFI_D_ERROR, "[%a][%d]Set 0x70 Status:%r \n\n",__FUNCTION__,__LINE__,Status));
		if (EFI_ERROR(Status))
            continue;
#endif
        for(iI2CBus=0;(iI2CBus+SlaveAddress) < 0x28;iI2CBus++)
        {
            Data = 0;
            Data |= DefaultConfig|(DefaultConfig<<8); 
            Command = PCA9555A_CONFIG0;
            Status = Pca9555APpi->Set(PeiServices, 0, iI2CBus+SlaveAddress, Command, Data);
            if(EFI_ERROR(Status)) continue;
            
            Command = PCA9555A_INPUT0;
            Status = Pca9555APpi->Get(PeiServices, 0, iI2CBus+SlaveAddress, Command, &Data);
            DEBUG((DEBUG_ERROR, "Get PCA9555A Slave:%x Register:%d Data:%02x Status:%r\n", iI2CBus+SlaveAddress, Command, Data, Status));
            if(EFI_ERROR(Status)) continue;


            //if 9555 PCIE slot not empty, then set PCIE power_enable to low
            if(((Data & PCA9555A_PRESENT)==0) && ((Data & PCA9555A_POWER_EN)==PCA9555A_POWER_EN))
            {
                Data &= (~(PCA9555A_POWER_EN|(PCA9555A_POWER_EN<<8)));
                Command = PCA9555A_OUTPUT0;
                Status = Pca9555APpi->Set(PeiServices, 0, iI2CBus+SlaveAddress, Command, Data);
                DEBUG((DEBUG_ERROR, "Set PCA9555A Slave:%x Register:%d Data:%02x Status:%r\n", iI2CBus+SlaveAddress, Command, Data, Status));
            }
        }
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)    

    }

    Status = Pca9545aPpi->Set(PeiServices, 0, 0x70, ControlByteReserved);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Set 0x70 Status:%r\n\n",__FUNCTION__,__LINE__,Status));
    if (EFI_ERROR(Status))
        MODULE_END(Status)
#endif
    MODULE_END(Status=EFI_SUCCESS)
}

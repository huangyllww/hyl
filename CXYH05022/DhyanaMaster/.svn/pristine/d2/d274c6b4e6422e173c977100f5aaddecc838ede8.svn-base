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

EFI_STATUS EFIAPI ScanNvmeBackPanel_Entry (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS              Status;
    UINT8                   SlaveAddress = 0x20,iI2CBus=0;
    EFI_PEI_PCA9555A_PPI    *Pca9555APpi;
    UINT16                  Data=0;
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)
    EFI_PEI_PCA9545A_PPI   *Pca9545aPpi;
    UINT8                   iI2CChannel=0;
    UINT8                   ControlByte,ControlByteReserved;
#endif
    Status = (*PeiServices)->LocatePpi (
                                    PeiServices,
                                    &gPca9555aPpiGuid,
                                    0,
                                    NULL,
                                    &Pca9555APpi);
    if (EFI_ERROR(Status))
        return Status;
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)    
    Status = (*PeiServices)->LocatePpi (
                                  PeiServices,
                                  &gPca9545aPpiGuid,
                                  0,
                                  NULL,
                                  &Pca9545aPpi);
    if (EFI_ERROR(Status))
        return Status;
    
    Status = Pca9545aPpi->Get(PeiServices, 0, 0x70, &ControlByteReserved);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Get 0x70 Status:%r ControlByteReserved:%x\n\n",__FUNCTION__,__LINE__,Status,ControlByteReserved));
    if (EFI_ERROR(Status))
        return Status;

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
            //Try to Read Input0
            Status = Pca9555APpi->Get(PeiServices, 0, (iI2CBus + SlaveAddress), 0, &Data);
            if(!EFI_ERROR(Status))
                DEBUG((EFI_D_ERROR, "[%a][%d]0x%02x  Input0:%x\n\n",__FUNCTION__,__LINE__,((iI2CBus + SlaveAddress)<<1),Data));
        }
#if defined(PCA9545_SUPPORT) && (PCA9545_SUPPORT == 1)    

    }

    Status = Pca9545aPpi->Set(PeiServices, 0, 0x70, ControlByteReserved);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Set 0x70 Status:%r\n\n",__FUNCTION__,__LINE__,Status));
    if (EFI_ERROR(Status))
        return Status;
#endif
    return EFI_SUCCESS;
}

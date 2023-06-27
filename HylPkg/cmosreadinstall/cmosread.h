//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.h 6     1/13/10 2:13p Felixp $
//READ
#ifndef _CMOSREAD_H_
#define _CMOSREAD_H_
#include <Pi/PiPeiCis.h>
#include "Token.h"
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg2.h>
#include <Library/IoLib.h>
#include<Uefi/UefiBaseType.h>                                                                                                                                       
#include <Library/DebugLib.h>
typedef void (EFIAPI *WRITE_CMOS)(UINT8 OFFSET,UINT8 DATA);
typedef UINT8 (EFIAPI *READ_CMOS)(UINT8 OFFSET_READ); 
typedef struct _CMOS_READ_WRITE_PPI{
    WRITE_CMOS  peiwritecmos8;
    READ_CMOS   peireadcmos8;
}CMOS_READ_WRITE_PPI;
// GUID Define
// function code define
void peiwritecmos8(UINT8 OFFSET,UINT8 DATA);
UINT8 peireadcmos8( UINT8 OFFSET_READ);
  
#endif
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.h $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description: 
//
//<AMI_FHDR_END>
//**********************************************************************

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

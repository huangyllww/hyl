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
//
// $Revision: 6 $
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
#ifndef __Dxepciread__H__
#define __Dxepciread__H__

/****** DO NOT WRITE UPON THIS LINE *******/

#include <Library/IoLib.h>
#include "Token.h"
#include<Uefi/UefiBaseType.h>
#include <Library/NbCspLib.h>
#include <Protocol/AcpiTable.h>
#include  "AmiDxeLib.h"
#include <Library/DebugLib.h>

 UINT32 DXEPCIRead (
    IN UINT32        bus_num,
    IN UINT32        device_num,
    IN UINT32        function_num,
    IN UINT32        offset);
 
void DXEPCIWrite (
    IN UINT32        bus_num,
    IN UINT32        device_num,
    IN UINT32       function_num,
    IN UINT32        offset,
    IN UINT32       data);

typedef struct _EFI_PCI_ROOT_DXE_RW_PROTOCOL { 
    UINT32  ( *DXEPCIRead)(   IN UINT32        bus_num,
         IN UINT32        device_num,
         IN UINT32        function_num,
         IN UINT32        offset); 
    void   (*DXEPCIWrite )(   IN UINT32    bus_num,
       IN UINT32        device_num,
       IN UINT32       function_num,
       IN UINT32        offset,
       IN UINT32       data);
} EFI_PCI_ROOT_DXE_RW_PROTOCOL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#endif
/****** DO NOT WRITE BELOW THIS LINE *******/

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

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:$
//
// $Revision:$
//
// $Date:$
//**********************************************************************
// Revision History
// ----------------
// $Log:$
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OemPlatLib.h
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __OEM_PLAT_LIB_H__
#define __OEM_PLAT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE UPON THIS LINE *******/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Efi.h>
#include <token.h>

//----------------------------------------------------------------------
// Macro/Type/Structure Definition
//----------------------------------------------------------------------
#pragma pack (1)

#pragma pack()

//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------
VOID memcpy_CHAR16(CHAR16 * dest,CHAR16 * src, UINTN len);
VOID memcpy_UINT8(UINT8 * dest,UINT8 * src, UINTN len);
VOID memset_CHAR16(CHAR16 * dest, UINTN len,CHAR16 val);
VOID memset_UINT8(UINT8 * dest, UINTN len,UINT8 val);

UINTN EfiValueToString ( IN OUT CHAR16  *,IN INT64,IN UINTN,IN UINTN);

//SGMTB#0144069-ImproveHintMesssageForOOB +>
typedef enum
{
  OOBStatus_DeviceCheck,
  OOBStatus_BMCSelfTest,
  OOBStatus_IpmiTransferProtocol,
  OOBStatus_Terminate
}OOB_STATUS_CHECK;
//SGMTB#0144069-ImproveHintMesssageForOOB ->
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

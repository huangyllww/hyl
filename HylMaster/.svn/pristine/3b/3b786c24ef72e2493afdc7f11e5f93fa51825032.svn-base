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
// Name:  OememLib.h
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef __OEMEM_LIB_H__
#define __OEMEM_LIB_H__
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

#define MODULE_START DEBUG((EFI_D_INFO, "[OemDbg][%a][%d] START\r\n-----------------------------------------------------\r\n", __FUNCTION__, __LINE__));
#define MODULE_END(Status)                                                                                                             \
  {                                                                                                                                    \
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d] Status:%r\r\n", __FUNCTION__, __LINE__, Status));                                            \
    DEBUG((EFI_D_INFO, "\r\n-----------------------------------------------------\r\n[OemDbg][%a][%d] END\r\n", __FUNCTION__, __LINE__)); \
    return Status;                                                                                                                     \
  }

#define MODULE_VOID_END DEBUG((EFI_D_INFO, "[OemDbg][%a][%d] END\r\n-----------------------------------------------------\r\n", __FUNCTION__, __LINE__));
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

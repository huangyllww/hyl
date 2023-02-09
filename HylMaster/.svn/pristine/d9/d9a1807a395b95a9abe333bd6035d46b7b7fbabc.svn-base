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
//<Oem_FHDR_START>
//
// Name:  OemRtcResetHook.h
//
// Description:	RTCRST# OEM hook header
//
//<Oem_FHDR_END>
//**********************************************************************
#ifndef __OEM_RTC_RESET_HOOK_H__
#define __OEM_RTC_RESET_HOOK_H__
#ifdef __cplusplus
extern "C" {
#endif
/****** DO NOT WRITE UPON THIS LINE *******/

//----------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------
#include <Efi.h>
#include <token.h>

#include <Ppi/ReadOnlyVariable2.h>	
#include <AMIVfr.h>
#include <Library\HobLib.h>
#include <Library\MemoryAllocationLib.h>

//----------------------------------------------------------------------
// Macro/Type/Structure Definition
//----------------------------------------------------------------------
#define OEM_INFO_DATA_HOB_GUID \
        {0x2930A592, 0xA48B, 0x41E5, 0x91, 0xE7, 0x29, 0x93, 0x12, 0x7A, 0x98, 0x34}

#pragma pack (push, 1)
//<lvych00120161014-> Lizhq 20140821 Reserve Password after clear CMOS.}
//<lvych001-20161130 Add password clear pin function+>
typedef struct {
  EFI_HOB_GUID_TYPE   Header;
  BOOLEAN             IsPWDClear;
} OEM_INFO_DATA_HOB;
//<lvych001-20161130 Add password clear pin function->

typedef struct {
  EFI_HOB_GUID_TYPE   EfiHobGuidType;
  AMITSESETUP         AmiTseData;
  BOOLEAN               RtcResetFlag;
} AMI_TSE_HOB;
#pragma pack(pop)
//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------


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

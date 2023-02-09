#ifndef __SATA_CONTROLLER_INFO_LIB_H__
#define __SATA_CONTROLLER_INFO_LIB_H__

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
EFI_STATUS GetSataControllerIndex(VOID *Dev, VOID **rBrg, UINTN *RPIndex);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
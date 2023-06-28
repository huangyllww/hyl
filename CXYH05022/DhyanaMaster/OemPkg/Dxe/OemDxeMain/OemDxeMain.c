#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>

/**
Note:Auto Generate By Make Files
**/
#include "OemDxeHook.h"

extern OEM_DXE_HOOK OEM_DXE_HOOK_LIST EndOemDxeHookList;

OEM_DXE_HOOK *OemDxeHooks[] = {
    OEM_DXE_HOOK_LIST NULL
};

CHAR8 *OemDxeHooksame[] = {
    OEM_DXE_HOOK_LIST_NAMES NULL
};

/**
GC_TODO: Oem Dxe Hook For Dxe Phase Function
@param  ImageHandle
@param  SystemTable

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS OemDxeHook_Entry (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
){
  EFI_STATUS                          Status = EFI_SUCCESS;
  UINT8                               iDxeHook;
  
  InitAmiLib(ImageHandle, SystemTable);

  for(iDxeHook=0; OemDxeHooks[iDxeHook]!=NULL; iDxeHook++)
  {
    DEBUG((EFI_D_INFO, "OemDxeHook.%a",OemDxeHooksame[iDxeHook]));
    Status = OemDxeHooks[iDxeHook](SystemTable);
    DEBUG((EFI_D_INFO, " %r\n",Status));
    DEBUG((EFI_D_INFO, "====================================================================================================\n"));
  }

  return EFI_SUCCESS;
}
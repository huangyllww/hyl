/** @file OemTSEHook.h
    Header file for string functionalities.

**/

#ifndef _OEM_TSE_HOOK_H_
#define	_OEM_TSE_HOOK_H_

#include "token.h"
#include "AMIVfr.h"


#define SETUP_LOAD_DEFAULT_GUID \
  {0xb0875b12, 0x16ce, 0x4d19, 0x92, 0x77, 0x75, 0x67, 0xe0, 0xf8, 0x9b, 0xf4};

#if defined(SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT)&& (SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT == 1) //<lvych00120160314+>VOID HandleOemAdvanceModeKey(VOID *app, VOID *hotkey, VOID *cookie );
VOID HandleOemAdvanceModeKey(VOID *app, VOID *hotkey, VOID *cookie);
VOID OemCheckForKey (AMI_EFI_KEY_DATA AmiKey); //<lvych001-20161129 Add press hot key tips>
#endif

//<lvych006-20160310 Administrator and User Password Policy ->
#endif /* _OEM_AMI_TSE_HOOK_H_ */

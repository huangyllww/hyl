#ifndef __HARDWARE_MONITOR_H__
#define __HARDWARE_MONITOR_H__

#include <AmiDxelib.h>

#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Setup.h>
#include "token.h"
#include "AUTOID.h"
#if APTIO_4
#include "Protocol\GenericSensor\GenericSensor.h"
#include <SetupStrTokens.h>
#endif
#include <Library\UefiBootServicesTableLib.h>

typedef struct {
#if EFI_SPECIFICATION_VERSION>0x20000
    EFI_HII_CONFIG_ACCESS_PROTOCOL Callback;
#else
    EFI_FORM_CALLBACK_PROTOCOL Callback;
#endif
    UINT16 Class, SubClass;
} SETUP_CALLBACK;

#endif

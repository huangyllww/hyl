//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
    Definitions used by PlatformPeiSystemInformation.

**/

#include <AmiLib.h>
#include <timestamp.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/EndOfPeiPhase.h>

#include <PPI/AmiPeiTextOut.h>

#include "Token.h"

typedef struct {
  UINT16    Value;
  CHAR8     *String;
} VALUE_TO_STRING_MAPPING;

static VALUE_TO_STRING_MAPPING gDimmMfgIdNameMapping[] = {
    {0x5705, "AENEON"},
    {0x5105, "Qimonda"},
    {0x1000, "NEC"},
    {0xB300, "IDT"},
    {0x9700, "Texas Instruments"},
    {0xAD00, "SK Hynix"},
    {0x2C00, "Micron Technology"},
    {0xC100, "Infineon (Siemens)"},
    {0xCE00, "Samsung"},
    {0x3D00, "Tektronix"},
    {0x9801, "Kingston"},
    {0xFE02, "Elpida"},
    {0x9401, "Smart Modular"},
    {0xC802, "Agilent Technologies"},
    {0x0B03, "Nanya Technology"},
    {0xB304, "Inphi Corporation"},
    {0x3206, "Montage Technology Group"}
};

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#ifndef __ODA_SETUP__H__
#define __ODA_SETUP__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Token.h>
#include "OemDebugAlways_DebugLevel.h"

#define OEM_COMMON_VAR_NAME       L"OEM_VAR"
#define OEM_COMMON_VAR_GUID       {0x9D4D817A, 0x0808, 0x4CC6, 0xA6, 0xD1, 0xF2, 0x69, 0x2B, 0xE9, 0x32, 0x93}
#define OEM_COMMON_VAR_KEY        0xF045

#pragma pack(push, 1)
typedef struct _OEM_COMMON_SETUP_DATA {
    UINT8   DebugModeMask;  ///
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#if defined(VGA_UART_DIS16BIT_PORT80_SUPPORT) && (VGA_UART_DIS16BIT_PORT80_SUPPORT == 1)
    UINT8   PostExEn;
    UINT8   Revised[14];
#else
    UINT8   Revised[15];
#endif
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
} OEM_COMMON_SETUP_DATA;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif
#endif

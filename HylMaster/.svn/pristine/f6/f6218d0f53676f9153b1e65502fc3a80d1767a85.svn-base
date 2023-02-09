#pragma once

//#include<stdlib.h>
//#include<memory.h>
#include "Efi.h"
#define _EFI 1

#ifdef _WIN32
//#include "EDKStructures.h"
#endif
#if defined(_LNX) || defined(_BSD)
//#include <stdint.h>
#include "os_hook.h"
#endif
//Basic definitions from Aptio4 EFI.h
/*
typedef unsigned char BOOLEAN;
#define TRUE 1
#define FALSE 0
#if !defined (_MSC_VER) || (_MSC_VER < 1300)
#define __w64
#endif*/
#if defined(EFI64) || defined(EFIx64)
typedef __int64 INTN;
typedef unsigned __int64 UINTN;
#endif
#ifdef _WIN32
//typedef int __w64 INTN;
//typedef unsigned __w64 UINTN;
#endif

#ifdef _EFI
#include "os_hook.h"
#endif

#if defined(_BSD64)
typedef __int64_t INTN;
typedef __uint64_t UINTN;
#else
#if defined(_LNX) || defined(_BSD)
typedef int INTN;
typedef unsigned int UINTN;
#endif
#endif
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef OPTIONAL
#define OPTIONAL
#endif

/*typedef signed char INT8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned UINT32;
typedef __int64 INT64;
typedef unsigned __int64 UINT64;
typedef unsigned char CHAR8;
typedef unsigned short CHAR16;
#define VOID void*/

//Redeclaration
/*typedef struct{
	UINT32  Data1;
	UINT16  Data2;
	UINT16  Data3;
	UINT8   Data4[8];
} EFI_GUID; */

#ifndef _EFI
typedef INTN EFI_STATUS;
typedef int	 DLL_STATUS;
#endif

//Error Codes and Warning Codes
//#define EFI_ERROR_BIT 	((INTN)1<<(sizeof(UINTN)*8-1))
//#define PI_ERROR_BIT (EFI_ERROR_BIT|(EFI_ERROR_BIT>>1))
//#define EFI_SUCCESS	0
#define FV_SUCCESS 0

/*#define EFI_ERROR(r) ( (r) & EFI_ERROR_BIT )

#define EFI_LOAD_ERROR                 ( 1 | EFI_ERROR_BIT)
#define EFI_INVALID_PARAMETER          ( 2 | EFI_ERROR_BIT)
#define EFI_UNSUPPORTED                ( 3 | EFI_ERROR_BIT)
#define EFI_BAD_BUFFER_SIZE            ( 4 | EFI_ERROR_BIT)*/
#ifndef _EFI
#define EFI_BUFFER_TOO_SMALL           ( 5 | EFI_ERROR_BIT)
#endif
/*#define EFI_NOT_READY                  ( 6 | EFI_ERROR_BIT)
#define EFI_DEVICE_ERROR               ( 7 | EFI_ERROR_BIT)
#define EFI_WRITE_PROTECTED            ( 8 | EFI_ERROR_BIT)
#define EFI_OUT_OF_RESOURCES           ( 9 | EFI_ERROR_BIT)
#define EFI_VOLUME_CORRUPTED           (10 | EFI_ERROR_BIT) // 0xa
#define EFI_VOLUME_FULL                (11 | EFI_ERROR_BIT) // 0xb
#define EFI_NO_MEDIA                   (12 | EFI_ERROR_BIT) // 0xc
#define EFI_MEDIA_CHANGED              (13 | EFI_ERROR_BIT) // 0xd
#define EFI_NOT_FOUND                  (14 | EFI_ERROR_BIT) // 0xe
#define EFI_ACCESS_DENIED              (15 | EFI_ERROR_BIT) // 0xf
#define EFI_NO_RESPONSE                (16 | EFI_ERROR_BIT) // 0x10
#define EFI_NO_MAPPING                 (17 | EFI_ERROR_BIT) // 0x11
#define EFI_TIMEOUT                    (18 | EFI_ERROR_BIT) // 0x12
#define EFI_NOT_STARTED                (19 | EFI_ERROR_BIT) // 0x13
#define EFI_ALREADY_STARTED            (20 | EFI_ERROR_BIT) // 0x14
#define EFI_ABORTED                    (21 | EFI_ERROR_BIT) // 0x15
#define EFI_ICMP_ERROR                 (22 | EFI_ERROR_BIT) // 0x16
#define EFI_TFTP_ERROR                 (23 | EFI_ERROR_BIT) // 0x17
#define EFI_PROTOCOL_ERROR             (24 | EFI_ERROR_BIT) // 0x18
#define EFI_INCOMPATIBLE_VERSION       (25 | EFI_ERROR_BIT) // 0x19
#define EFI_SECURITY_VIOLATION         (26 | EFI_ERROR_BIT) // 0x1a
#define EFI_CRC_ERROR                  (27 | EFI_ERROR_BIT) // 0x1b

//Modifiers (Modifiers for Common EFI Data Types)
#define IN
#define OUT
#define OPTIONAL
#define EFIAPI
#define CONST const

//*******************************************************
// Variable Attributes
//*******************************************************
#define EFI_VARIABLE_NON_VOLATILE       0x0000000000000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x0000000000000002
#define EFI_VARIABLE_RUNTIME_ACCESS     0x0000000000000004*/

//NVRAM Routine Prototypes
#ifdef __cplusplus
//extern "C" {
#endif

VOID NvramLibInit(char* Buffer, UINTN Size, UINT8 FlashErasePolarity);
EFI_STATUS SceGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data
);
EFI_STATUS GetNextVariableName(
	IN OUT UINTN *VariableNameSize,
	IN OUT wchar_t *VariableName, IN OUT EFI_GUID *VendorGuid
);
EFI_STATUS SetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
);

//Additional output parameter NvramDataPtr
//On exit it will contain addrress within NVRAM buffer at which
//variable data has been placed.
EFI_STATUS SetVariableEx(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data, OUT VOID **NvramDataPtr
);

//Cleans up NVRAM and by performing garbage collection
VOID NvramCompact();
#ifdef __cplusplus
//}
#endif

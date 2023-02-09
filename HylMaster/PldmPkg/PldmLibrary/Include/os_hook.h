#include <wchar.h>
#include "types.h"
#pragma once

#define _Wstrlen SCE_StrLen
//#define _wcsicmp wcscmp     // TEMPORARY!!!
#define _wcsicmp wcsicmp
extern int wcsicmp(const wchar_t *s1, const wchar_t *s2);
//#define _Wstrlen wcslen
#define BBS_TYPE_FLOPPY           0x01
#define BBS_TYPE_HARDDRIVE        0x02
#define BBS_TYPE_CDROM            0x03
#define BBS_TYPE_PCMCIA           0x04
#define BBS_TYPE_USB              0x05
#define BBS_TYPE_EMBEDDED_NETWORK 0x06
#define BBS_TYPE_BEV              0x80
#define BBS_TYPE_UNKNOWN          0xFF


#define ERROR_SUCCESS                    0L
#define ERROR_NOT_ENOUGH_MEMORY          8L
#define ERROR_HANDLE_EOF                 38L
#define ERROR_INVALID_DATA               13L
#define ERROR_INVALID_PARAMETER          87L
#define ERROR_CRC                        23L
#define ERROR_OUTOFMEMORY                14L
#define ERROR_NOT_SUPPORTED              50L
#define ERROR_GEN_FAILURE                31L
#define ERROR_PROC_NOT_FOUND             127L

//For printing wide char strings
#ifndef _S
// collides with EFI ctype/wctype
#define _S L"ls"
#endif
/// TEMP ///
#if 0
#define swprintf _swprintf
extern int _swprintf (wchar_t *, int, const wchar_t *, ...);
#else
//#define swprintf(s, n, ...) swprintf(s, __VA_ARGS__)
#endif

#if 1
// efi lacks the "re-entrant" version
// this is used in AMISCE.cpp:_GetVariableData()
// and appears to not actually use the capability to switch among buffers
#define wcstok_s(s, del, lasts) wcstok(s, del, lasts)
#else
extern wchar_t *wcstok_s(wchar_t *Source, const wchar_t *Delimiters, wchar_t **Lasts);
#endif

//#define NULL 0

#ifndef VOID
#define VOID void
typedef void *PVOID;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif


// should some of these be in types.h?

typedef char TCHAR;
typedef wchar_t _TCHAR;
typedef unsigned char BYTE;
typedef unsigned char BOOLEAN;
typedef unsigned int UINT32;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef unsigned char UINT8;
typedef unsigned long long UINT64; // NEED TO VERIFY
typedef unsigned short USHORT;

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int BOOL;
typedef BYTE * LPBYTE;
typedef char *LPSTR;
typedef char *PCHAR;

/*typedef struct{
	UINT32  Data1;
	UINT16  Data2;
	UINT16  Data3;
	UINT8   Data4[8];
} GUID;*/

// elimate throw on malloc failure
extern void MallocFailure(wchar_t *);
#define throw(x) MallocFailure(x);


int SCE_GetVariable(wchar_t *VariableName, GUID *Guid, UINT32 *Attributes, UINT64 *VariableDataSize, void **VariableData);
int SCE_SetVariable( wchar_t *VariableName, GUID *VendorGUID, UINT32 Attributes, UINT64 VariableDataSize, UINT8 *VariableData );
int SCE_GetNextVariableName( UINT32 *VariableNameSize, wchar_t **VariableName, GUID *VendorGUID );

// debugging
//extern "C" void sleep(int);

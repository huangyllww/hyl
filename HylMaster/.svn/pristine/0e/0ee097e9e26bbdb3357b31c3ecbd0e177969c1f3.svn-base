//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file SetupDataCreationLib.h
    Contains structure declarations for Jsondata types and has function declarations

**/

#ifndef _SETUPDATA_CREATIONLIB_H_
#define	_SETUPDATA_CREATIONLIB_H_

#include "Token.h"
#include "Efi.h"
#include "setupdata.h"
#include <Include/AMIVfr.h>
#include "variable.h"
#include "boot.h"
#include "bbs.h"
#include "TseUefiHii.h"
#include "Include/AmiLib.h"
#include "CtrlCond.h"
#include "AmiDxeLib.h"
#include "Protocol/DevicePath.h"
#include "EDKhelper.h"
#include "Protocol/UnicodeCollation.h"

#define STRUCT_OFFSET(type, field)  (UINTN)&(((type *)0)->field);
#define memset                      MemSet
#define AMI_FLAG_MANUFACTURING      EFI_IFR_OPTION_DEFAULT_MFG
#define AMI_FLAG_DEFAULT            EFI_IFR_OPTION_DEFAULT
#define INVALID_HANDLE              ((VOID*)(UINTN)-1)
#define INVALID_TOKEN               ((UINT16)-1)
#define NG_SIZE                     19

#define CONSTRUCT_DEFAULTS_FIRST_BOOT_GUID  \
{ 0xc5912ed9, 0x83c2, 0x4bff, 0x99, 0x36, 0x23, 0x1f, 0xeb, 0x85, 0xf3, 0xe8 }

#define EFI_DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID  \
  { 0x3c4ead08, 0x45ae, 0x4315, 0x8d, 0x15, 0xa6, 0x0e, 0xaa, 0x8c, 0xaf, 0x69 }

#define EFI_DEFAULT_BOOT_ORDER_VARIABLE_GUID  \
  { 0x45cf35f6, 0x0d6e, 0x4d04, 0x85, 0x6a, 0x03, 0x70, 0xa5, 0xb1, 0x6f, 0x53 }

#define EFI_DEFAULT_DRIVER_ORDER_VARIABLE_GUID  \
  { 0xde788bed, 0xb6e2, 0x4290, 0xbd, 0xc8, 0x2a, 0xbb, 0x65, 0xd6, 0x21, 0x78 }

#define AMI_BASE_BIN        2
#define AMI_BASE_OCT        8
#define AMI_BASE_DEC        10
#define AMI_BASE_HEX        16
#define AMI_BASE_INT_DEC    32

#define CHARACTER_NUMBER_FOR_VALUE  30

extern UINT8                *gApplicationData;
extern VARIABLE_LIST        *gVariables;
extern EFI_BOOT_SERVICES    *gBS;
extern EFI_SYSTEM_TABLE     *gST;
extern EFI_RUNTIME_SERVICES *gRT;
extern SETUP_PKG            *gSetupPkg;
extern SCREEN_INFO          *gScreen;
extern PAGE_LIST            *gPages;
extern PAGE_LIST            *gToolPages;
extern CONTROL_INFO         *gToolControlInfo;
extern CONTROL_INFO         *gControlInfo;
extern VARIABLE_LIST        *gToolVariables;
extern VARIABLE_LIST        *gVariables;
extern PAGE_ID_LIST         *gPageIdList;
extern PAGE_ID_LIST         *gToolPageIdList;
extern PAGE_ID_INFO         *gPageIdInfo;
extern PAGE_ID_INFO         *gToolPageIdInfo;

extern NVRAM_VARIABLE       *gVariableList;
extern NVRAM_VARIABLE       *gFailsafeDefaults;
extern NVRAM_VARIABLE       *gOptimalDefaults;
extern HPK_INFO             *gHpkInfo;
extern GUID_LIST            *gGuidList;

/*
typedef struct RefreshIdInfo{
    UINT8 *pEvent;
    UINT8 *pNotifyContext;
} REFRESH_ID_INFO;
*/

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  ///
  /// Firmware file name
  ///
  EFI_GUID                        FvFileName;
} AMITSE_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;

EFI_STATUS  VarBuildDefaults (void);
EFI_STATUS  HiiFixupData ();
void        *EfiLibAllocateZeroPool (IN  UINTN   AllocationSize);
VOID        MemCopy (VOID *dest, VOID *src, UINTN size);
VOID        EfiStrCpy (IN CHAR16   *Destination, IN CHAR16   *Source);
VOID        MemFreePointer( VOID **ptr );
CHAR16      *GetVariableNameByID( UINT32 VariableID );
CHAR16      *GetGUIDNameByID( UINT32 VariableID );
VOID        MemSet( VOID *buffer, UINTN size, UINT8 value );
EFI_STATUS  HiiLoadDefaults( VOID **varList, UINT32 Mask );
EFI_STATUS  TseVarBuildAMIDefaults(VOID);
UINTN       HiiGetManufactuingMask(VOID);
UINTN       HiiGetDefaultMask(VOID);
VOID        LoadDefaultLegDevOrder(VOID);
VOID        *BBSReOrderDefultLegacyDevOrder(BBS_ORDER_TABLE *DefaultLegDevOrder,UINTN DefaultLegDevOrderSize);
VARIABLE_INFO *VarGetVariableInfoIndex( UINT32 index );
UINTN       EfiStrLen (IN CHAR16   *String);
BOOLEAN     UefiIsEfiVariable(UINT32 variable, VARIABLE_INFO *varInfo);
EFI_STATUS  UefiVarSetNvram(VARIABLE_INFO *VariableInfo, VOID *Buffer, UINTN Offset, UINTN Size);
EFI_STATUS  UefiVarGetNvram(VARIABLE_INFO *VariableInfo, VOID **Buffer, UINTN Offset, UINTN Size);
INTN        EfiCompareMem (IN VOID     *MemOne, IN VOID     *MemTwo, IN UINTN    Length);
UINTN       StrLen8(CHAR8 *string);
EFI_STATUS  InitApplicationData ();
VOID        *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );
BOOLEAN     EfiCompareGuid (IN EFI_GUID *Guid1, IN EFI_GUID *Guid2);
EFI_STATUS  FormBrowserLocateSetupHandles(VOID*  **handleBuffer,UINT16 *count);
BOOLEAN     FormBrowserHandleValid(VOID);
EFI_STATUS  Uefi21HiiInitializeProtocol(VOID);
CHAR16      *HiiGetString( VOID* handle, UINT16 token );
INTN        EfiStrCmp (IN CHAR16 *String, IN CHAR16 *String2);
INTN        MemCmp( UINT8 *dest, UINT8 *src, UINTN size );
EFI_STATUS GetSetDefaultsFromControlPtr(NVRAM_VARIABLE *optimalDefaultlist, NVRAM_VARIABLE *failSafeDefaultlist);
VOID        EfiStrCat (IN CHAR16   *Destination, IN CHAR16   *Source);
EFI_STATUS  _VerifyResponseString(EFI_STRING Configuration, BOOLEAN NameValue);
EFI_STATUS  BufToHexString (IN OUT CHAR16 *Str, IN OUT UINTN *HexStringBufferLength, IN UINT8 *Buf, IN UINTN Len);
EFI_STATUS  HiiExtendedInitializeProtocol();
CHAR8       *StrDup16to8(CHAR16 *String);
EFI_STATUS  HiiLibGetStringEx(IN EFI_HII_HANDLE HiiHandle, IN STRING_REF StringId, IN CHAR8 *Language, IN OUT UINTN *StringSize, OUT EFI_STRING String);
CHAR8       *StrDup8( CHAR8 *string );
UINT64      AmiTseDivU64x32 (IN  UINT64  Dividend, IN  UINTN   Divisor, OUT UINTN   *Remainder OPTIONAL);
EFI_STATUS  HexStringToBuf (IN OUT UINT8 *Buf, IN OUT UINTN *Len, IN CHAR16 *Str, OUT UINTN *ConvertedStrLen  OPTIONAL);
UINTN       EfiDevicePathSize (IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath);
BOOLEAN     IsHexDigit (OUT UINT8 *Digit, IN CHAR16 Char);
UINTN       DPLength (EFI_DEVICE_PATH_PROTOCOL *pDp);
UINTN       SPrint (OUT CHAR16 *Buffer, IN UINTN BufferSize, IN CONST CHAR16  *Format,  ...);
CHAR16      *StrDup( CHAR16 *string );
UINTN       Rbatoi_base (CHAR8 *string, UINT8 base);
UINTN       atoi_base(CHAR16 *string, UINT8 base);
EFI_STATUS  UefiFormCallbackNVRead(CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size, VOID **buffer);
EFI_STATUS  UefiFormCallbackNVWrite(CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size);
UINT16      HiiAddString( VOID* handle, CHAR16 *string );
CHAR16*     EfiStrStr (IN  CHAR16  *String, IN  CHAR16  *StrCharSet);
UINTN       EfiStrSize (IN CHAR16   *String);
EFI_STATUS  _GetPageInfo(UINT32 PgIndex, PAGE_INFO **PageInfo);
EFI_STATUS  BufferToHexString (IN OUT CHAR16 *Str, IN UINT8 *Buffer, IN UINTN BufferSize);
EFI_STATUS  UnicodeToConfigString (IN OUT CHAR16 *ConfigString, IN OUT UINTN *StrBufferLen, IN CHAR16 *UnicodeString);
INTN        EfiStrnCmp (IN CHAR16 *String, IN CHAR16 *String2, IN UINTN Length);
INTN        EfiAsciiStrCmp (IN CHAR8   *String, IN CHAR8   *String2);
VOID        UnRegFormNotification (VOID);
VOID        RTIfrProcessRunTimeForms(EFI_IFR_REF **ref);
VOID        RTIfrProcessAddVarListAndPageIDList(VOID);
BOOLEAN     RTIfrProcessFormIfUpdated(UINT16 link);
VOID        RTIfrProcessExit(VOID);
BOOLEAN     RTIfrUpdateVariableInfoWrapper(UINT32 ControlVariable, EFI_HANDLE Handle);
VOID        RTIfrUpdateVariableInfo( UINT32 ControlVariable, EFI_HANDLE Handle );
CHAR16      *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang);
UINT16      Uefi21HiiChangeStringLanguage(VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string);
VOID        SetCallBackControlInfo(EFI_HANDLE VarHandle, UINT32 VarIndex);
size_t      strlen(const char *s);
char        *strcpy(char * __restrict s1, const char * __restrict s2);
#endif /* _SETUPDATA_CREATIONLIB_H_ */

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

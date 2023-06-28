#include "types.h"

#define UEFI21_LANGUAGE_SIZE 6

#define VARIABLE_NAME_LENGTH 40

typedef struct _VARIABLE VARIABLE;
#ifndef _EFI
typedef GUID	EFI_GUID;
#endif

struct _VARIABLE
{
	EFI_GUID	VariableGuid;		//Varies	AMI_GUID for the NVRAM variable store for this data
	wchar_t 		VariableName[VARIABLE_NAME_LENGTH];	//Varies	Variable store name
	UINT32		VariableAttributes;	//Varies	Attributes of the variable
	UINT32		VariableID;			//Varies	unique variable number, 0x0FFFF for our own variables.
	UINT16		VariableSize;		//Varies	The size of Buffer Varstore}VARIABLE;
	UINT16    VariableIndex;
	UINT8     *VariableBuffer;
	VARIABLE  *Next;
	UINT8      Referenced;          // Indicates referenced by a control
	// additional fields from tse
#define	VARIABLE_ATTRIBUTE_VARSTORE		0x0000	// Buffer varstore accessed via Config. Access protocol exposed by the driver.
#define	VARIABLE_ATTRIBUTE_EFI_VARSTORE	0x1000	// EFI Variable, GetVeriable and SetVariable Can be used.
#define	VARIABLE_ATTRIBUTE_NAMEVALUE	0x2000	// Namevalue varstore accessed via Config. Access protocol exposed by the driver.
#define AMI_SPECIAL_NO_DEFAULTS			0x00000001
#define AMI_SPECIAL_VARIABLE_NO_SET		0x00000002
#define AMI_SPECIAL_VARIABLE_NO_GET		0x00000004
	UINT32		ExtendedAttibutes;	//Varies	Extended Attributes to identify type of Variable.
	UINT16		VariableNameId;		//Varies	Id for the Variable Name
	VOID       *VariableHandle;     // HII handle of formset
};

extern VOID *GetNVRAMVariable( wchar_t *Name, UINT8 *VendorGuid, UINT32 *Attributes,
							   UINT64 *VarSize );
extern BOOLEAN IsVerbose();
extern VOID MemAllocFailure();
int AddUefiVariable( UINT8 *IFRData );

VOID *VarGetNVRAMVariable( VARIABLE *varInfo, UINT64 *VarSize );
EFI_STATUS VarSetNVRAMVariable( VARIABLE *varInfo, VOID *buffer, UINTN size );
BOOLEAN UefiIsEfiVariable(VARIABLE *varInfo);
VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
EFI_STATUS UefiVarGetNvram(VARIABLE *VariableInfo, VOID **Buffer, UINTN Offset, UINTN Size);
EFI_STATUS UefiVarSetNvram(VARIABLE *VariableInfo, VOID *Buffer, UINTN Offset, UINTN Size);
EFI_STATUS VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size );
EFI_STATUS UefiFormCallbackNVRead(CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size, VOID **buffer);
EFI_STATUS UefiFormCallbackNVWrite(CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size);
EFI_STATUS GetConfigHeader(VARIABLE *VariableInfo, EFI_STRING *Configuration, UINTN *Length);
EFI_STATUS GetBlockName(EFI_STRING *Configuration, UINTN *Length, UINTN Offset, UINTN Size);
EFI_STATUS _VerifyResponseString(EFI_STRING Configuration, BOOLEAN NameValue);
EFI_STATUS GetHexStringAsBuffer(UINT8 **Buffer, CHAR16 *HexStringBuffer);
EFI_STATUS HiiInitializeProtocol(VOID);
EFI_STATUS HiiExtendedInitializeProtocol();
EFI_STATUS _ConstructConfigHdr (CHAR16  *ConfigHdr, UINTN *StrBufferLen, EFI_GUID *Guid, CHAR16 *Name, EFI_HANDLE *DriverHandle);
EFI_STATUS HexStringToBuf ( UINT8 *Buf, UINTN *Len, CHAR16 *Str, UINTN *ConvertedStrLen);
EFI_STATUS NameValueHexStringToBuf ( UINT8 *Buf, UINTN  *Len, CHAR16 *Str, UINTN *ConvertedStrLen);
UINTN EfiDevicePathSize ( EFI_DEVICE_PATH_PROTOCOL  *DevicePath );
EFI_STATUS BufferToHexString (CHAR16 *Str, UINT8 *Buffer, UINTN BufferSize);
VOID SwapBuffer (UINT8 *Buffer, UINTN BufferSize );
EFI_STATUS BufToHexString (CHAR16  *Str, UINTN  *HexStringBufferLength, UINT8 *Buf, UINTN Len);
EFI_STATUS UnicodeToConfigString ( CHAR16 *ConfigString, UINTN *StrBufferLen, CHAR16 *UnicodeString);
BOOLEAN IsHexDigit ( UINT8 *Digit, CHAR16 Char);
VOID ToLower (CHAR16    *Str);
CHAR16 NibbleToHexChar (UINT8 Nibble );
UINT8 HexToNibbleChar (UINT8 Nibble);
VOID MemFreePointer( VOID **ptr );
VOID *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );

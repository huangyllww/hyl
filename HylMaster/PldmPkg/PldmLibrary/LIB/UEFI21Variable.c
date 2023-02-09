//extern "C"
//{
#include "string.h"
#include "stdlib.h"
#include "wchar.h"
#include "AmiSceCommon.h"
#include "Wrapper.h"
//}
#define _EFI 1
#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#endif
#include "UEFI21Variable.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/HiiConfigRouting.h"
#include "Protocol/HiiDatabase.h"
#include "AmiDxeLib.h"
#include "Library/UefiLib.h"

//start here
#include "EDKStructures21.h"

#if defined(_LNX) || defined(_EFI)
#define _S L"ls"
#endif
//#ifdef WIN32
//#define _S L"s"
//#endif
extern VARIABLE *gVariable ;   //TEMP:: removed extern here
extern BOOLEAN g_QuietMode;
extern EFI_HANDLE gCurrentHiiHandle;

extern EFI_BOOT_SERVICES 		*gBS;
extern EFI_RUNTIME_SERVICES    *gRT;
extern EFI_HII_DATABASE_PROTOCOL *gHiiDatabaseProtocol;
extern EFI_HII_CONFIG_ROUTING_PROTOCOL *gHiiConfigRouting;

int AddUefiVariable(UINT8 *IFRData)
{
	int Status = EFI_SUCCESS;
	EFI_IFR_OP_HEADER *OpHeader = ( EFI_IFR_OP_HEADER * )IFRData;
	EFI_IFR_FORM_SET *FormSet = ( EFI_IFR_FORM_SET * )NULL;
	EFI_GUID *Guid = ( EFI_GUID * )NULL;
	VARIABLE *Var = ( VARIABLE * )NULL;
	VARIABLE *PrevVar = ( VARIABLE * )NULL;
	EFI_IFR_VARSTORE *Varstore = ( EFI_IFR_VARSTORE * )NULL;
	EFI_IFR_VARSTORE_EFI *VarstoreEFI = ( EFI_IFR_VARSTORE_EFI * )NULL;
	EFI_IFR_VARSTORE_NAME_VALUE *VarstoreName = ( EFI_IFR_VARSTORE_NAME_VALUE * )NULL;

	char *Str = ( char * )NULL;
	wchar_t *VarNamePtr = (wchar_t *)NULL;
	UINT16 Index = 0;
	UINT16 VarID = 0;
	wchar_t VarName[VARIABLE_NAME_LENGTH] = {0};
	UINT16 Size = 0;
	UINT16 Found = 0;
	UINT64 BufferSize;
	UINT32 ExtendedAttr;

	memset(&BufferSize, 0, sizeof(UINT64));

	switch (OpHeader->OpCode)
	{
		case EFI_IFR_FORM_SET_OP:
			FormSet = ( EFI_IFR_FORM_SET * )IFRData;
			Guid = &( FormSet->Guid );
			wcscpy( VarName, L"Setup" );
			break;

		case EFI_IFR_VARSTORE_OP:
			Varstore = ( EFI_IFR_VARSTORE * )IFRData;
			Guid = &( Varstore->Guid );
			VarID = Varstore->VarStoreId;
			ExtendedAttr = VARIABLE_ATTRIBUTE_VARSTORE;
			Size = Varstore->Size;
			//if(Size<=0)
			//	return Status;
			Str = ( char* )( Varstore->Name );

			//Convert to unicode string
			for (VarNamePtr =  VarName; *Str; *VarNamePtr = *Str, Str++, VarNamePtr++)
				if (VarNamePtr - VarName >= VARIABLE_NAME_LENGTH - 1)
				{
					// truncate name if too long (unlikely!)
					*VarNamePtr = 0;
					break;
				};

			break;

		//not needed for the tset version
		case EFI_IFR_VARSTORE_EFI_OP:
			VarstoreEFI = ( EFI_IFR_VARSTORE_EFI*  )IFRData;
			Guid = &( VarstoreEFI->Guid );
			VarID = VarstoreEFI->VarStoreId;
			Size = VarstoreEFI->Size;
			ExtendedAttr = VARIABLE_ATTRIBUTE_EFI_VARSTORE;
            Str = ( char* )( VarstoreEFI->Name );

            //Convert to unicode string
            for (VarNamePtr =  VarName; *Str; *VarNamePtr = *Str, Str++, VarNamePtr++)
                if (VarNamePtr - VarName >= VARIABLE_NAME_LENGTH - 1)
                {
                    // truncate name if too long (unlikely!)
                    *VarNamePtr = 0;
                    break;
                };
			break;

		case EFI_IFR_VARSTORE_NAME_VALUE_OP:
			VarstoreName = ( EFI_IFR_VARSTORE_NAME_VALUE * )IFRData;
			Guid = &( VarstoreName->Guid );
			VarID = VarstoreName->VarStoreId;
			Size = sizeof( EFI_IFR_VARSTORE_NAME_VALUE );
			ExtendedAttr = VARIABLE_ATTRIBUTE_NAMEVALUE;
			break;

		default:
			break;
	}

	if (Size <= 0)
	{
		return Status;
	}

	// check that the Var is not already in the list
	for ( Var = gVariable, Index = 0; Var; Var = Var->Next, Index++ )
	{
		if ( CompareGuid(Guid, &(Var->VariableGuid)) == 1 ) //Compare Guid
		{
			if ( wcscmp(VarName, Var->VariableName) == 0 )
			{
				Found = 1;
				break;
			}
		}

		PrevVar = Var;
	}


	if (!Found )
	{
		//if (Size <=0 )
		//	return Status;
		Var = ( VARIABLE * )AllocateZeroPool ( ( sizeof( VARIABLE ) ) * ( sizeof( UINT8 ) ) );

		if (NULL == Var)
		{
			throw (L"Memory Allocation Failure"); //MemAllocFailure();
		}

		if ( gVariable == NULL )
		{
			gVariable = Var;
		}
		else
		{
			PrevVar->Next = Var;
		}

		Var->VariableIndex = Index;
		Var->VariableID = VarID;
		memcpy(( &Var->VariableGuid), Guid, sizeof( EFI_GUID ));
		Var->VariableAttributes = 0;
		wcscpy( Var->VariableName, VarName );
		Var->VariableSize = Size;
		Var->Referenced = 0;
		// hii handle, if available, 0 otherwise
		Var->VariableHandle = gCurrentHiiHandle;
		Var->ExtendedAttibutes = ExtendedAttr;
		Var->VariableNameId = 0;    // for name/value, not implimented yet
	}
	else
	{
		Var->VariableID = VarID;
		Var->VariableSize = Size;
	}

	if ( Var->VariableBuffer == NULL )
	{

		//Var->VariableBuffer = ( UINT8 * )GetNVRAMVariable( Var->VariableName, ( UINT8 * )&Var->VariableGuid, &Var->VariableAttributes, &BufferSize );
		Var->VariableBuffer = ( UINT8 * )VarGetNVRAMVariable( Var, &BufferSize );

		if ( ( UINT16 )BufferSize != Var->VariableSize )
		{
			//CHAR16 *GuidStr = GetGuidAsUnicodeString(( UINT8 * )&Var->VariableGuid );

			Var->VariableSize = ( UINT16 )BufferSize;
			Status = -1;
			//if(GuidStr)
			//free( GuidStr );
		}
	}

	return Status;
}

UINT16 GetUEFI21LangDataWidth()
{

	return UEFI21_LANGUAGE_SIZE;
}

VOID *VarGetNVRAMVariable( VARIABLE *varInfo, UINT64 *VarSize )
{
	VOID *buffer = NULL;
//	VARIABLE *varInfo;
	EFI_STATUS status = EFI_SUCCESS;

	*VarSize = 0;

	//varInfo = VarGetVariableInfoIndex( variable );
	if ( varInfo == NULL )
	{
		return buffer;
	}

	if (UefiIsEfiVariable(varInfo))
	{
		*VarSize = 0;

		if (varInfo->VariableAttributes == 0)
		{
			varInfo->VariableAttributes = 0x07;
		}

		buffer = VarGetNvramName( varInfo->VariableName, &varInfo->VariableGuid, &varInfo->VariableAttributes, (UINTN*)VarSize );
	}
	else
	{
		{
			if ((varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_VARSTORE) == VARIABLE_ATTRIBUTE_VARSTORE)
			{
				*VarSize = varInfo->VariableSize;
			}

			status = UefiVarGetNvram(varInfo, &buffer, 0, (UINTN)(*VarSize));

			if (EFI_ERROR(status))
			{
				*VarSize = 0;
			}
		}
	}

	return buffer;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    UefiIsEfiVariable
//
// Description:
//
// Parameter:	    UINT32 variable, VARIABLE_INFO *varInfo
//
// Return value:  	BOOLEAN
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN UefiIsEfiVariable(VARIABLE *varInfo)
{
	if (varInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_EFI_VARSTORE ||
			(!wcscmp(varInfo->VariableName, L"PlatformLang")) ||
			(!wcscmp(varInfo->VariableName, L"DelBootOption")) ||
			(!wcscmp(varInfo->VariableName, L"AddBootOption")) ||
			(!wcscmp(varInfo->VariableName, L"BootManager")) ||
			(!wcscmp(varInfo->VariableName, L"BootNowCount")) ||
			(!wcscmp(varInfo->VariableName, L"LegacyDev")) ||
			(!wcscmp(varInfo->VariableName, L"LegacyGroup")) ||
			(!wcscmp(varInfo->VariableName, L"Timeout")) ||
			(!wcscmp(varInfo->VariableName, L"BootOrder")) ||
			(!wcscmp(varInfo->VariableName, L"AMICallback")) ||
			(!wcscmp(varInfo->VariableName, L"AMITSESetup")) ||
			(!wcscmp(varInfo->VariableName, L"ErrorManager")) ||
			(!wcscmp(varInfo->VariableName, L"UserDefaults")) ||
			(!wcscmp(varInfo->VariableName, L"BBSOrder")) ||
		    (!wcscmp(varInfo->VariableName, L"LegacyDevOrder")) ||
			(!wcscmp(varInfo->VariableName, L"DriverManager")) ||
			(!wcscmp(varInfo->VariableName, L"DriverOrder")) ||
			(!wcscmp(varInfo->VariableName, L"AddDriverOption")) ||
			(!wcscmp(varInfo->VariableName, L"DelDriverOption")) ||
			(!wcscmp(varInfo->VariableName, L"IDESecDev")) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	VarGetNvramName
//
// Description:	function to get the NvRam Variable Name
//
// Input:		CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size
//
// Output:		Name String
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size )
{
	VOID *buffer = NULL;
	UINT32 attrib = 0;
	EFI_STATUS Status = EFI_UNSUPPORTED;

	// NO GET Functionality is taken care by VarGetNvram()

	if ( attributes != NULL )
	{
		attrib = *attributes;
	}

	Status = UefiFormCallbackNVRead(name, guid, &attrib, size, &buffer);

	if (EFI_ERROR(Status))
	{
		Status = pRS->GetVariable(
					 name,
					 guid,
					 &attrib,
					 size,
					 buffer
				 );

		if ( Status != EFI_BUFFER_TOO_SMALL )
		{
			return buffer;
		}

		buffer = AllocateZeroPool ( *size + 1);

		if ( buffer == NULL )
		{
			return buffer;
		}

		Status = pRS->GetVariable(
					 name,
					 guid,
					 &attrib,
					 size,
					 buffer
				 );
	}

	if ( EFI_ERROR( Status ) )
	{
		MemFreePointer( (VOID *)&buffer );
	}

	if ( attributes != NULL )
	{
		*attributes = attrib;
	}

	return buffer;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    UefiFormCallbackNVRead
//
// Description:
//
// Parameter:	    CHAR16 *name, EFI_GUID *guid, UINT32 *attributes,
//                UINTN *size, VOID **buffer
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS UefiFormCallbackNVRead(CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size, VOID **buffer)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    UefiFormCallbackNVWrite
//
// Description:
//
// Parameter:	    CHAR16 *name, EFI_GUID *guid, UINT32 attributes,
//                VOID *buffer, UINTN size
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS UefiFormCallbackNVWrite(CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size)
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    UefiVarGetNvram
//
// Description:
//
// Parameter:	    VARIABLE_INFO *VariableInfo, VOID **Buffer, UINTN Offset,
//					UINTN Size
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NameValueGetHexStringAsBuffer (UINT8 **Buffer, CHAR16 *HexStringBuffer, UINTN Size);
EFI_STATUS UefiVarGetNvram(VARIABLE *VariableInfo, VOID **Buffer, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING configuration = (EFI_STRING)NULL;
	EFI_STRING result = (EFI_STRING)NULL;
	EFI_STRING stringPtr = (EFI_STRING)NULL;
	UINTN length = 1;
	UINT8 *buffer = NULL;
	EFI_GUID EfiHiiConfigAccessProtocolGuid = EFI_HII_CONFIG_ACCESS_PROTOCOL_GUID;
	EFI_HANDLE DriverHandle;
	EFI_HII_CONFIG_ACCESS_PROTOCOL	*ConfigAccess;
//	CHAR16 tmpString = (CHAR16)NULL; cast from pointer to integer of different size
	CHAR16 tmpString = 0;
	EFI_STRING progress = &tmpString;
	BOOLEAN PreservegEnableDrvNotification = FALSE;

	configuration = AllocateZeroPool(length);

	if (configuration == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	//
	// Construct the Configuration Header String
	//
	status = GetConfigHeader(VariableInfo, &configuration, &length);

	if (EFI_ERROR(status) || (configuration == NULL)) //EIP-75384 Static Analysis
	{
		goto DONE;
	}

	if (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		//Allocate Memory for "&<Name>"
		UINTN len = length + StrLen(L"&") + StrLen(VariableInfo->VariableName) + sizeof(CHAR16);

		configuration = (EFI_STRING)MemReallocateZeroPool(configuration, length * sizeof(CHAR16), len * sizeof(CHAR16));

		if (configuration == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}

		StrCat(configuration, L"&");
		StrCat(configuration, VariableInfo->VariableName);
		length = len;
	}
	else
	{
		status = GetBlockName(&configuration, &length, Offset, Size);

		if (EFI_ERROR(status))
		{
			goto DONE;
		}
	}

//	SetCallBackControlInfo(VariableInfo->VariableHandle, 0); //Set this varaible handle to allow BrowserCallback

//	PreservegEnableDrvNotification = gEnableDrvNotification;
//	gEnableDrvNotification = TRUE; //EIP75588 Set to enable notification processing

	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))		// Invoke Config Routing Route Config
	{
		status = gHiiConfigRouting->ExtractConfig (gHiiConfigRouting, configuration, &progress, &result);
	}
	else
	{
		status = gHiiDatabaseProtocol->GetPackageListHandle (gHiiDatabaseProtocol, VariableInfo->VariableHandle, (VOID**)&DriverHandle); //Calling the driver which publishes the name value

		if (!EFI_ERROR(status))
		{
			status = gBS->HandleProtocol (DriverHandle, &EfiHiiConfigAccessProtocolGuid , (VOID**)&ConfigAccess);

			if (!EFI_ERROR(status))
			{
				status = ConfigAccess->ExtractConfig (ConfigAccess, configuration, &progress, &result);
			}
		}
	}

//  if (!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
//		gEnableDrvNotification = FALSE; //EIP75588 Reset to disable notification processing
//	SetCallBackControlInfo(NULL, 0); //Disable update
	if (EFI_ERROR(status) || (progress == (EFI_STRING)NULL))
	{
		goto DONE;
	}

	if (EFI_ERROR(status = _VerifyResponseString(result, (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE) ? TRUE : FALSE)))
	{
		goto DONE;
	}

	stringPtr = result;

	if (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		for (stringPtr = result; *stringPtr != 0 && StrnCmp(stringPtr, VariableInfo->VariableName, StrLen(VariableInfo->VariableName)) != 0; stringPtr++)
			;

		stringPtr += StrLen(VariableInfo->VariableName);
		stringPtr += StrLen(L"=");
		status = NameValueGetHexStringAsBuffer (&buffer, stringPtr, Size);
	}
	else
	{
		for (stringPtr = result; *stringPtr != 0 && StrnCmp(stringPtr, L"&VALUE=", StrLen(L"&VALUE=")) != 0; stringPtr++)
			;

		stringPtr += StrLen(L"&VALUE=");
		status = GetHexStringAsBuffer (&buffer, stringPtr);
	}


	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	*Buffer = buffer;

DONE:

	if (configuration)
	{
		MemFreePointer((VOID**)&configuration);
	}

	if (result)
	{
		MemFreePointer((VOID**)&result);
	}

	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    GetConfigHeader
//
// Description:
//
// Parameter:	  	VARIABLE *VariableInfo, EFI_STRING *Configuration, UINTN *Length
//
// Return value:  	EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetConfigHeader(VARIABLE *VariableInfo, EFI_STRING *Configuration, UINTN *Length)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING configuration = NULL;
	EFI_HANDLE driverHandle;
	UINTN length = 50;

	//
	// Initialize Protocols
	//
	status = HiiInitializeProtocol();

	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	//
	// Retrieve the Device Path from Handle
	//

#if !TSE_GNUC_BUILD_SUPPORT
	status = gHiiDatabaseProtocol->GetPackageListHandle(gHiiDatabaseProtocol, VariableInfo->VariableHandle, &driverHandle);
#else
	status = gHiiDatabaseProtocol->GetPackageListHandle(gHiiDatabaseProtocol, (EFI_HII_HANDLE)ReadUnaligned64((const UINT64 *) & ( VariableInfo->VariableHandle)), &driverHandle);
#endif

	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	// Allocate static memory of 50 Bytes, which will be freed if not used
	configuration = (UINT16*)AllocateZeroPool(50);

	// Construct the Configuration Header String
	//
	status = _ConstructConfigHdr(configuration, &length, &VariableInfo->VariableGuid, VariableInfo->VariableName, driverHandle);

	if (EFI_ERROR(status))
	{
		if (status == EFI_BUFFER_TOO_SMALL)
		{
			MemFreePointer((VOID**)&configuration);
			configuration = (UINT16*)AllocateZeroPool(length);

			if (configuration == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}

			status = _ConstructConfigHdr(configuration, &length, &VariableInfo->VariableGuid, VariableInfo->VariableName, driverHandle);
		}
	}

	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	if (*Configuration)
	{
		MemFreePointer((VOID**)Configuration);
	}

	*Configuration = configuration;
	*Length = length;

DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    GetBlockName
//
// Description:
//
// Parameter:	    EFI_STRING *Configuration, UINTN *Length, UINTN Size
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetBlockName(EFI_STRING *Configuration, UINTN *Length, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN length = 0;
	UINTN finalLen = 0;
	CHAR16 *tempBuffer = NULL;

	length = (StrLen(L"&OFFSET=") + sizeof(Offset) + StrLen(L"&WIDTH=") + sizeof(Size) + 1) * sizeof(CHAR16);
	tempBuffer = (CHAR16*)AllocateZeroPool(length);

	if (tempBuffer == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	swprintf(tempBuffer, length, L"&OFFSET=%llx&WIDTH=%llx", Offset, Size);

	length = StrLen(*Configuration) * sizeof(CHAR16);
	finalLen = (StrLen(*Configuration) + StrLen(tempBuffer) + 1) * sizeof(CHAR16);
	*Configuration = (EFI_STRING)MemReallocateZeroPool(*Configuration, length, finalLen);

	if (*Configuration == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	StrCat(*Configuration, tempBuffer);

	MemFreePointer((VOID**)&tempBuffer);
	*Length = finalLen;

DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    _VerifyResponseString
//
// Description:   Verifies Correctness of Response String. This is to prevent
//				  crashing if the result pointer is pointing to an invalid location.
//
// Parameter:	    EFI_STRING *Configuration
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS _VerifyResponseString(EFI_STRING Configuration, BOOLEAN NameValue)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING config = Configuration;
	CHAR16 *keyWord[] = {L"GUID=", L"&NAME=", L"&PATH=", L"&OFFSET=", L"&WIDTH=", L"&VALUE="};
	UINT32 i = 0;
	UINT32 count = NameValue ? 3 : 6; // If Name Value Check only upto &Path else check until &Value.

	if (StrnCmp(config, keyWord[i], StrLen(keyWord[i])) != 0)
	{
		status = EFI_NOT_FOUND;
		goto DONE;
	}

	for (i = 1; i < count; i++)
	{
		for (; StrnCmp(config, keyWord[i], StrLen(keyWord[i])) != 0 && (config != (Configuration + StrLen(Configuration))); config++)
			;

		if (StrnCmp(config, keyWord[i], StrLen(keyWord[i])) != 0)
		{
			status = EFI_NOT_FOUND;
			goto DONE;
		}
	}

DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:		GetHexStringAsBuffer
//
// Description:
//
// Parameter:		CHAR8 *Buffer
//					UINT16 *HexStringBuffer
//					UINTN Size
//
// Return value:	EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetHexStringAsBuffer(UINT8 **Buffer, CHAR16 *HexStringBuffer)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN bufLength = 0;
	UINTN convertedStrLen = 0;

	status = HexStringToBuf(*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);

	if (EFI_ERROR(status))
	{
		if (status == EFI_BUFFER_TOO_SMALL)
		{
			if (*Buffer)
			{
				MemFreePointer((VOID**)Buffer);
			}

			*Buffer = AllocateZeroPool(bufLength);

			if (*Buffer == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}
		}
		else
		{
			goto DONE;
		}

		status = HexStringToBuf(*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
	}

DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:		NameValueGetHexStringAsBuffer
//
// Description:
//
// Parameter:		CHAR8 *Buffer
//					UINT16 *HexStringBuffer
//					UINTN Size
//
// Return value:	EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NameValueHexStringToBuf (IN OUT UINT8 *, IN OUT UINTN *Len, IN CHAR16 *, OUT UINTN *);
EFI_STATUS NameValueGetHexStringAsBuffer (UINT8 **Buffer, CHAR16 *HexStringBuffer, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	UINTN bufLength = 0;
	UINTN convertedStrLen = 0;

	if (Size <= sizeof  (UINT64))
	{
		GetHexStringAsBuffer (Buffer, HexStringBuffer);
	}
	else
	{
		status = NameValueHexStringToBuf (*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);

		if (EFI_ERROR(status))
		{
			if (status == EFI_BUFFER_TOO_SMALL)
			{
				if (*Buffer)
				{
					MemFreePointer(Buffer);
				}

				bufLength = (Size > bufLength) ? Size : bufLength;
				*Buffer = AllocateZeroPool(bufLength);

				if (*Buffer == NULL)
				{
					status = EFI_OUT_OF_RESOURCES;
					goto DONE;
				}
			}
			else
			{
				goto DONE;
			}

			status = NameValueHexStringToBuf (*Buffer, &bufLength, HexStringBuffer, &convertedStrLen);
		}
	}

DONE:
	return status;
}

static BOOLEAN gHiiInitialized = FALSE;
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    HiiInitializeProtocol
//
// Description:
//
// Parameter:
//
// Return value: EFI_STATUS
//					EFI_SUCCESS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HiiInitializeProtocol(VOID)
{
	EFI_STATUS Status = EFI_SUCCESS;

	/*	Status = gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, (VOID**)&gHiiFont);
		if(EFI_ERROR(Status))
		{
			goto DONE;
		}*/
	Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabaseProtocol);

	if (EFI_ERROR(Status))
	{
		goto DONE;
	}

	/*	Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL,(VOID**) &gHiiString);
		if(EFI_ERROR(Status))
		{
			goto DONE;
		}*/
	Status = HiiExtendedInitializeProtocol();

DONE:
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    HiiExtendedInitializeProtocol
//
// Description:
//
// Parameter:
//
// Return value: EFI_STATUS
//					EFI_SUCCESS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HiiExtendedInitializeProtocol()
{
	EFI_STATUS status = EFI_SUCCESS;

	if (gHiiInitialized)
	{
		goto DONE;
	}

	status = gBS->LocateProtocol(&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&gHiiConfigRouting);

	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	gHiiInitialized = TRUE;

DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    _ConstructConfigHdr
//
// Description:		Construct <ConfigHdr> using routing information GUID/NAME/PATH.
//
// Parameter:	  	IN OUT CHAR16 *ConfigHdr,
//                	IN OUT UINTN  *StrBufferLen,
//                	IN EFI_GUID   *Guid,
//                	IN CHAR16     *Name, OPTIONAL
//                	IN EFI_HANDLE *DriverHandle
//
// Return value:  	EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS _ConstructConfigHdr (
	IN OUT CHAR16                *ConfigHdr,
	IN OUT UINTN                 *StrBufferLen,
	IN EFI_GUID                  *Guid,
	IN CHAR16                    *Name, OPTIONAL
	IN EFI_HANDLE                *DriverHandle
)
{
	EFI_STATUS                Status;
	UINTN                     NameStrLen;
	UINTN                     DevicePathSize;
	UINTN                     BufferSize;
	CHAR16                    *StrPtr;
	EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

	if (Name == NULL)
	{
		//
		// There will be no "NAME" in <ConfigHdr> for  Name/Value storage
		//
		NameStrLen = 0;
	}
	else
	{
		//
		// For buffer storage
		//
		NameStrLen = StrLen (Name);
	}

	//
	// Retrieve DevicePath Protocol associated with this HiiPackageList
	//
	Status = gBS->HandleProtocol (
				 DriverHandle,
				 &gEfiDevicePathProtocolGuid,
				 (VOID**)&DevicePath
			 );

	if (EFI_ERROR (Status))
	{
		return Status;
	}

	DevicePathSize = EfiDevicePathSize (DevicePath);

	//
	// GUID=<HexCh>32&NAME=<Char>NameStrLen&PATH=<HexChar>DevicePathStrLen <NULL>
	// | 5  |   32   |  6  |  NameStrLen*4 |  6  |    DevicePathStrLen    | 1 |
	//
	BufferSize = (5 + 32 + 6 + NameStrLen * 4 + 6 + DevicePathSize * 2 + 1) * sizeof (CHAR16);

	if (*StrBufferLen < BufferSize)
	{
		*StrBufferLen = BufferSize;
		return EFI_BUFFER_TOO_SMALL;
	}

	*StrBufferLen = BufferSize;

	StrPtr = ConfigHdr;

	StrCpy (StrPtr, L"GUID=");
	StrPtr += 5;
	BufferToHexString (StrPtr, (UINT8 *) Guid, sizeof (EFI_GUID));
	StrPtr += 32;

	//
	// Convert name string, e.g. name "ABCD" => "&NAME=0041004200430044"
	//
	StrCpy (StrPtr, L"&NAME=");
	StrPtr += 6;

	if (Name != NULL)
	{
		BufferSize = (NameStrLen * 4 + 1) * sizeof (CHAR16);
		UnicodeToConfigString (StrPtr, &BufferSize, Name);
		StrPtr += (NameStrLen * 4);
	}

	StrCpy (StrPtr, L"&PATH=");
	StrPtr += 6;
	BufferToHexString (StrPtr, (UINT8 *) DevicePath, DevicePathSize);

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	HexStringToBuf
//
// Description:	To avoid including EDK libs.
//
// Input:
//
// Output:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
HexStringToBuf (
	IN OUT UINT8                     *Buf,
	IN OUT UINTN                    *Len,
	IN     CHAR16                    *Str,
	OUT    UINTN                     *ConvertedStrLen  OPTIONAL
)
/*++

  Routine Description:
    Converts Unicode string to binary buffer.
    The conversion may be partial.
    The first character in the string that is not hex digit stops the conversion.
    At a minimum, any blob of data could be represented as a hex string.

  Arguments:
    Buf    - Pointer to buffer that receives the data.
    Len    - Length in bytes of the buffer to hold converted data.
                If routine return with EFI_SUCCESS, containing length of converted data.
                If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
    Str    - String to be converted from.
    ConvertedStrLen - Length of the Hex String consumed.

  Returns:
    EFI_SUCCESS: Routine Success.
    EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.
    EFI_

--*/
{
	UINTN       HexCnt;
	UINTN       Idx;
	UINTN       BufferLength;
	UINT8       Digit;
	UINT8       Byte;

	//
	// Find out how many hex characters the string has.
	//
	for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

	if (HexCnt == 0)
	{
		*Len = 0;
		return EFI_SUCCESS;
	}

	//
	// Two Unicode characters make up 1 buffer byte. Round up.
	//
	BufferLength = (HexCnt + 1) / 2;

	//
	// Test if  buffer is passed enough.
	//
	if (BufferLength > (*Len))
	{
		*Len = BufferLength;
		return EFI_BUFFER_TOO_SMALL;
	}

	*Len = BufferLength;

	for (Idx = 0; Idx < HexCnt; Idx++)
	{

		IsHexDigit (&Digit, Str[HexCnt - 1 - Idx]);

		//
		// For odd charaters, write the lower nibble for each buffer byte,
		// and for even characters, the upper nibble.
		//
		if ((Idx & 1) == 0)
		{
			Byte = Digit;
		}
		else
		{
			Byte = Buf[Idx / 2];
			Byte &= 0x0F;
			Byte |= Digit << 4;
		}

		Buf[Idx / 2] = Byte;
	}

	if (ConvertedStrLen != NULL)
	{
		*ConvertedStrLen = HexCnt;
	}

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------------------------------------------------------------------------------
// Procedure:	NameValueHexStringToBuf
//
// Description:	Converts name value string to buffer
//
// Input:	: 	Buf    - Pointer to buffer that receives the data.
//			Len    - Length in bytes of the buffer to hold converted data.
//            		If routine return with EFI_SUCCESS, containing length of converted data.
//            		If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
//			Str    - String to be converted from.
//			ConvertedStrLen - Length of the Hex String consumed.
//
// Output:	EFI_SUCCESS: Routine Success.
//			EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 HexToNibbleChar (IN UINT8 Nibble);
EFI_STATUS NameValueHexStringToBuf (
	IN OUT UINT8                     *Buf,
	IN OUT UINTN                    *Len,
	IN     CHAR16                    *Str,
	OUT    UINTN                     *ConvertedStrLen  OPTIONAL
)
{
	UINTN       HexCnt;
	UINTN       Idx, ByteIndex = 0;
	UINTN       BufferLength;
	UINT8       Digit;
	UINT8       Byte;
	UINT64	  Ext8byte = 0;
	UINT32	  Ext4byte = 0;
	UINT16      Word;

	//
	// Find out how many hex characters the string has.
	//
	for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

	if (HexCnt == 0)
	{
		*Len = 0;
		return EFI_SUCCESS;
	}

	//
	// Two Unicode characters make up 1 buffer byte. Round up.
	//
	BufferLength = (HexCnt + 1) / 2;

	//
	// Test if  buffer is passed enough.
	//
	if (BufferLength > (*Len))
	{
		*Len = BufferLength;
		return EFI_BUFFER_TOO_SMALL;
	}

	*Len = BufferLength;

	for (Idx = 0; Idx < HexCnt; Idx += 4)
	{
		//00 31 00 36 00 30 00 00
		Ext4byte = *((UINT32 *)&Str[Idx + 2]);			//00 31 00 36

		Word = ((UINT16)(Ext4byte & 0x0000FFFF));		//00 36
		Byte = ((UINT8)(Word & 0x00FF));				//36
		Buf [ByteIndex] = HexToNibbleChar (Byte);				//6
		Buf [ByteIndex] <<= 0x4;						//60

		Word = ((UINT16)(Ext4byte >> 16));			//00 31
		Byte = ((UINT8)(Word & 0x00FF));				//31
		Buf [ByteIndex] |= HexToNibbleChar (Byte);				//1

		ByteIndex ++;

		Ext4byte = *((UINT32 *)&Str[Idx]);	//00 30 00 30
		Word = ((UINT16)(Ext4byte & 0x0000FFFF));	//00 30
		Byte = ((UINT8)(Word & 0x00FF));				//30
		Buf [ByteIndex] = HexToNibbleChar (Byte);			//0
		Buf [ByteIndex] <<= 0x4;						//60

		Word = ((UINT16)(Ext4byte >> 16));	//00 30
		Byte = ((UINT8)(Word & 0x00FF));			//30
		Buf [ByteIndex] |= HexToNibbleChar (Byte);			//0

		ByteIndex ++;
	}

	if (ConvertedStrLen != NULL)
	{
		*ConvertedStrLen = HexCnt;
	}

	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//------------------------------------------------------------------------------------
// Procedure:	HexToNibbleChar
//
// Description:	Converts the hex value to character.
//
// Input:	Nibble - lower nibble of a byte.
//
// Output:	Hex unicode character.
//
//--------------------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
HexToNibbleChar (
	IN UINT8      Nibble
)
{
	if (Nibble - L'0' <= 0x9)
	{
		return (CHAR16)(Nibble - L'0');
	}

	return (CHAR16)(Nibble + 0xA - L'A');
}

UINTN
EfiDevicePathSize (
	IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
)
{
	return DPLength(DevicePath);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	BufferToHexString
//
// Description:	Converts binary buffer to Unicode string in reversed byte order from BufToHexString().
//
// Input:	Str        -  String for output
//					Buffer     -  Binary buffer.
//					BufferSize -  Size of the buffer in bytes.
//
// Output:	EFI_SUCCESS  -  The function completed successfully.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
BufferToHexString (
	IN OUT CHAR16    *Str,
	IN UINT8         *Buffer,
	IN UINTN         BufferSize
)
{
	EFI_STATUS  Status;
	UINT8       *NewBuffer;
	UINTN       StrBufferLen;

	NewBuffer = AllocateCopyPool (BufferSize, Buffer);
	SwapBuffer (NewBuffer, BufferSize);

	StrBufferLen = BufferSize * 2 + 1;
	Status = BufToHexString (Str, &StrBufferLen, NewBuffer, BufferSize);

	FreePool (NewBuffer);
	//
	// Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
	//
	ToLower (Str);

	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	SwapBuffer
//
// Description:	Swap bytes in the buffer.
//
// Input:	Buffer     -  Binary buffer.
//					BufferSize -  Size of the buffer in bytes.
//
// Output:	None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
SwapBuffer (
	IN OUT UINT8     *Buffer,
	IN UINTN         BufferSize
)
{
	UINTN  Index;
	UINT8  Temp;
	UINTN  SwapCount;

	SwapCount = BufferSize / 2;

	for (Index = 0; Index < SwapCount; Index++)
	{
		Temp = Buffer[Index];
		Buffer[Index] = Buffer[BufferSize - 1 - Index];
		Buffer[BufferSize - 1 - Index] = Temp;
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	BufToHexString
//
// Description:	Converts binary buffer to Unicode string.
//					At a minimum, any blob of data could be represented as a hex string.
//
// Input:	Str - Pointer to the string.
//				    HexStringBufferLength - Length in bytes of buffer to hold the hex string. Includes tailing '\0' character.
//                                        If routine return with EFI_SUCCESS, containing length of hex string buffer.
//                                        If routine return with EFI_BUFFER_TOO_SMALL, containg length of hex string buffer desired.
//				    Buf - Buffer to be converted from.
//				    Len - Length in bytes of the buffer to be converted.
//
// Output:	EFI_SUCCESS: Routine success.
//				    EFI_BUFFER_TOO_SMALL: The hex string buffer is too small.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
BufToHexString (
	IN OUT CHAR16                    *Str,
	IN OUT UINTN                     *HexStringBufferLength,
	IN     UINT8                     *Buf,
	IN     UINTN                      Len
)
{
	UINTN       Idx;
	UINT8       Byte;
	UINTN       StrLen;

	//
	// Make sure string is either passed or allocate enough.
	// It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
	// Plus the Unicode termination character.
	//
	StrLen = Len * 2;

	if (StrLen > ((*HexStringBufferLength) - 1))
	{
		*HexStringBufferLength = StrLen + 1;
		return EFI_BUFFER_TOO_SMALL;
	}

	*HexStringBufferLength = StrLen + 1;
	//
	// Ends the string.
	//
	Str[StrLen] = L'\0';

	for (Idx = 0; Idx < Len; Idx++)
	{

		Byte = Buf[Idx];
		Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
		Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
	}

	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	UnicodeToConfigString
//
// Description:	Convert Unicode string to binary representation Config string, e.g.
//					"ABCD" => "0041004200430044". Config string appears in <ConfigHdr> (i.e.
//					"&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").
//
// Input:	ConfigString  - Binary representation of Unicode String, <string> := (<HexCh>4)+
//					StrBufferLen  - On input: Length in bytes of buffer to hold the Unicode string.
//       		    Includes tailing '\0' character.
//					On output:
//						If return EFI_SUCCESS, containing length of Unicode string buffer.
//						If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
//					UnicodeString - Original Unicode string.
//
// Output:	EFI_SUCCESS          - Routine success.
//					EFI_BUFFER_TOO_SMALL - The string buffer is too small.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
UnicodeToConfigString (
	IN OUT CHAR16                *ConfigString,
	IN OUT UINTN                 *StrBufferLen,
	IN CHAR16                    *UnicodeString
)
{
	UINTN       Index;
	UINTN       Len;
	UINTN       BufferSize;
	CHAR16      *String;

	Len = StrLen (UnicodeString);
	BufferSize = (Len * 4 + 1) * sizeof (CHAR16);

	if (*StrBufferLen < BufferSize)
	{
		*StrBufferLen = BufferSize;
		return EFI_BUFFER_TOO_SMALL;
	}

	*StrBufferLen = BufferSize;
	String        = ConfigString;

	for (Index = 0; Index < Len; Index++)
	{
		BufToHexString (ConfigString, &BufferSize, (UINT8 *) UnicodeString, 2);

		ConfigString += 4;
		UnicodeString += 1;
	}

	//
	// Add tailing '\0' character
	//
	*ConfigString = L'\0';

	//
	// Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
	//
	ToLower (String);
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	IsHexDigit
//
// Description:	To avoid including EDK libs.
//
// Input:
//
// Output:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN
IsHexDigit (
	OUT UINT8      *Digit,
	IN  CHAR16      Char
)
/*++

  Routine Description:
    Determines if a Unicode character is a hexadecimal digit.
    The test is case insensitive.

  Arguments:
    Digit - Pointer to byte that receives the value of the hex character.
    Char  - Unicode character to test.

  Returns:
    TRUE  - If the character is a hexadecimal digit.
    FALSE - Otherwise.

--*/
{
	if ((Char >= L'0') && (Char <= L'9'))
	{
		*Digit = (UINT8) (Char - L'0');
		return TRUE;
	}

	if ((Char >= L'A') && (Char <= L'F'))
	{
		*Digit = (UINT8) (Char - L'A' + 0x0A);
		return TRUE;
	}

	if ((Char >= L'a') && (Char <= L'f'))
	{
		*Digit = (UINT8) (Char - L'a' + 0x0A);
		return TRUE;
	}

	return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	ToLower
//
// Description:	Converts the unicode character of the string from uppercase to lowercase.
//
// Input:	Str        -  String to be converted
//
// Output:	None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
ToLower (
	IN OUT CHAR16    *Str
)
{
	CHAR16      *Ptr;

	for (Ptr = Str; *Ptr != L'\0'; Ptr++)
	{
		if (*Ptr >= L'A' && *Ptr <= L'Z')
		{
			*Ptr = (CHAR16) (*Ptr - L'A' + L'a');
		}
	}
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	NibbleToHexChar
//
// Description:	Converts the low nibble of a byte  to hex unicode character.
//
// Input:	Nibble - lower nibble of a byte.
//
// Output:	Hex unicode character.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR16
NibbleToHexChar (
	IN UINT8      Nibble
)
{
	Nibble &= 0x0F;

	if (Nibble <= 0x9)
	{
		return (CHAR16)(Nibble + L'0');
	}

	return (CHAR16)(Nibble - 0xA + L'A');
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	MemReallocateZeroPool
//
// Description:	function to reuse a allocated buffer
//
// Input:		VOID *oldBuffer, UINTN oldSize, UINTN newSize
//
// Output:		void
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize )
{
	VOID *buffer;

	buffer = AllocateZeroPool( newSize );

	if ( buffer == NULL )
	{
		return buffer;
	}

	if ( newSize < oldSize )
	{
		oldSize = newSize;
	}

	if ( oldBuffer != NULL )
	{
		gBS->CopyMem( buffer, oldBuffer, oldSize );
	}

	MemFreePointer( (VOID **)&oldBuffer );

	return buffer;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	MemFreePointer
//
// Description:	function to free the pointers
//
// Input:		VOID **ptr
//
// Output:		void
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID MemFreePointer( VOID **ptr )
{
	if ( ( ptr == NULL ) || ( *ptr == NULL ) )
	{
		return;
	}

	FreePool( *ptr );
	*ptr = NULL;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	VarSetNvram
//
// Description:	function to set the NvRam varible names
//
// Input:	UINT32 variable, VOID *buffer, UINTN *size
//
// Output:	status
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS VarSetNVRAMVariable( VARIABLE *varInfo, VOID *buffer, UINTN size )
{
	EFI_STATUS Status = EFI_INVALID_PARAMETER;

	if (buffer == NULL)
	{
		goto DONE;
	}

	if (varInfo == NULL)
	{
		goto DONE;
	}

	if ((varInfo->ExtendedAttibutes & AMI_SPECIAL_VARIABLE_NO_SET) == AMI_SPECIAL_VARIABLE_NO_SET)
	{
		// Don't save the variable
		Status = EFI_SUCCESS;
	}
	else
	{
		if (UefiIsEfiVariable(varInfo))
		{
			Status = VarSetNvramName( varInfo->VariableName, &varInfo->VariableGuid, varInfo->VariableAttributes, buffer, size );
		}
		else
		{
			Status = UefiVarSetNvram(varInfo, buffer, 0, size);
		}
	}

DONE:
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	VarSetNvramName
//
// Description:	function to Set the NvRam Variable Name
//
// Input:		CHAR16 *name, EFI_GUID *guid, UINT32 *attributes,
//					VOID *buffer, UINTN *size
//
// Output:		status
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size )
{
	EFI_STATUS Status = EFI_UNSUPPORTED;

	// NO SET Functionality is taken care by VarSetNvram()

	Status = UefiFormCallbackNVWrite(name, guid, attributes, buffer, size);

	if (EFI_ERROR(Status ) )
		Status = gRT->SetVariable(
					 name,
					 guid,
					 attributes,
					 size,
					 buffer
				 );

	return (Status);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	    UefiVarSetNvram
//
// Description:
//
// Parameter:	    VARIABLE_INFO *VariableInfo, VOID *Buffer, UINTN Offset,
//					UINTN Size
//
// Return value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NameValueBufToHexString (IN OUT CHAR16 **, IN OUT UINTN *,  IN UINT8 *,  IN UINTN );
EFI_STATUS UefiVarSetNvram(VARIABLE *VariableInfo, VOID *Buffer, UINTN Offset, UINTN Size)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_STRING configuration = NULL;
	EFI_STRING progress = NULL;
	CHAR16 *tempBuffer = NULL;
	UINTN length = 0;
	UINTN finalLen = 0;
	UINTN bufLen = 1;
	BOOLEAN   PreservegEnableDrvNotification = FALSE;

	//
	//Construct the Configuration Header String
	//
	status = GetConfigHeader(VariableInfo, &configuration, &length);

	if (EFI_ERROR(status) || (configuration == NULL)) //EIP-75384 Static Analysis
	{
		goto DONE;
	}

	if (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		UINTN len = length + StrLen(L"&") + StrLen(VariableInfo->VariableName) + StrLen(L"=") + sizeof(CHAR16);
		configuration = MemReallocateZeroPool (configuration, length * sizeof(CHAR16), len * sizeof(CHAR16));

		if (configuration == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}

		StrCat(configuration, L"&");
		StrCat(configuration, VariableInfo->VariableName);
		StrCat(configuration, L"=");
		length = StrLen (configuration) * sizeof (CHAR16);
	}
	else
	{
		status = GetBlockName(&configuration, &length, Offset, Size);

		if (EFI_ERROR(status))
		{
			goto DONE;
		}
	}

	//
	// Get the Buffer as Hex String
	//
	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
	{
		tempBuffer = (UINT16*)AllocateZeroPool(bufLen * sizeof(UINT16));

		if (tempBuffer == NULL)
		{
			status = EFI_OUT_OF_RESOURCES;
			goto DONE;
		}

		status = BufToHexString(tempBuffer, &bufLen, Buffer, Size);

		if (EFI_ERROR(status))
		{
			if (status != EFI_BUFFER_TOO_SMALL)
			{
				goto DONE;
			}

			if (tempBuffer)
			{
				MemFreePointer((VOID**)&tempBuffer);
			}

			tempBuffer = (UINT16*)AllocateZeroPool(bufLen * 2);

			if (tempBuffer == NULL)
			{
				status = EFI_OUT_OF_RESOURCES;
				goto DONE;
			}
		}

		status = BufToHexString(tempBuffer, &bufLen, Buffer, Size);
	}
	else
	{
		bufLen = 0;
		status = NameValueBufToHexString (&tempBuffer, &bufLen, Buffer, Size);
	}

	if (EFI_ERROR(status))
	{
		goto DONE;
	}

	//
	// Calculate the total Configuration string length
	// <Config Hdr>&OFFSET=<HexChar>0&WIDTH=<HexChar>&VALUE=<HexChar><NULL>
	// |                       length                |  7   | bufLen |  1  |
	//
	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
	{
		finalLen = length + (bufLen * 2) + (StrLen(L"&VALUE=") * sizeof(CHAR16));
	}
	else
	{
		//<exislen> + <Var val> + <NULL>
		finalLen = length + bufLen + (2 * sizeof (CHAR16));			//Last 2 bytes for appending & and NULL
	}

	configuration = MemReallocateZeroPool(configuration, length, finalLen);

	if (configuration == NULL)
	{
		status = EFI_OUT_OF_RESOURCES;
		goto DONE;
	}

	if (!(VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE))
	{
		StrCat(configuration, L"&VALUE=");
	}

	StrCat(configuration, tempBuffer);

	if (VariableInfo->ExtendedAttibutes & VARIABLE_ATTRIBUTE_NAMEVALUE)
	{
		StrCat (configuration, L"&");
	}

	MemFreePointer((VOID**)&tempBuffer);
//      PreservegEnableDrvNotification =  gEnableDrvNotification;
//  	 gEnableDrvNotification = TRUE; //EIP75588 Set to enable notification processing
	status = gHiiConfigRouting->RouteConfig (gHiiConfigRouting, configuration, &progress);
//  	 if(!PreservegEnableDrvNotification) // If gEnableDrvNotification is already True Don't touch it
//  		gEnableDrvNotification = FALSE; //Reset to disable notification processing
	MemFreePointer ((VOID**)&configuration);
DONE:
	return status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	NameValueBufToHexString
//
// Description:	Converts binary buffer to Unicode string.
//					At a minimum, any blob of data could be represented as a hex string.
//					Used only for name value implementaion
//
// Input:	Str - Pointer to the string.
//				    HexStringBufferLength - Length in bytes of buffer to hold the hex string. Includes tailing '\0' character.
//                                        If routine return with EFI_SUCCESS, containing length of hex string buffer.
//                                        If routine return with EFI_BUFFER_TOO_SMALL, containg length of hex string buffer desired.
//				    Buf - Buffer to be converted from.
//				    Len - Length in bytes of the buffer to be converted.
//
// Output:	EFI_SUCCESS: Routine success.
//				    EFI_BUFFER_TOO_SMALL: The hex string buffer is too small.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
NameValueBufToHexString (
	IN OUT CHAR16          **Str,
	IN OUT UINTN             *HexStringBufferLength,
	IN     UINT8                     *Buf,
	IN     UINTN                    Len
)
{
	UINT8       Byte;

	//
	// Make sure string is either passed or allocate enough.
	// It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
	// Plus the Unicode termination character.
	//

	if (Len <= sizeof (UINT64))			//For upto 8 bytes we have to take care of endian allignment
	{
		UINTN       Idx;
		UINTN       StrLen;
		CHAR16 	*tempStr = (CHAR16 *)NULL;

		//
		// Make sure string is either passed or allocate enough.
		// It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
		// Plus the Unicode termination character.
		//
		StrLen = Len * 2;
		*Str = (CHAR16*)AllocateZeroPool ((StrLen + 1) * sizeof (UINT16) );

		if (NULL == *Str)
		{
			return EFI_OUT_OF_RESOURCES;
		}

		*HexStringBufferLength = (StrLen + 1) * sizeof (UINT16);			//UefiVarSetNvram needs full size
		//
		// Ends the string.
		//
		tempStr = *Str;
		tempStr[StrLen] = L'\0';

		for (Idx = 0; Idx < Len; Idx++)
		{
			Byte = Buf[Idx];
			tempStr[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
			tempStr[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
		}
	}
	else
	{
		CHAR16 *tempBuf = (CHAR16 *)NULL, *tempStr = (CHAR16 *)NULL;
		UINT32 iIndex = 0, StringHexLength = 0;
		CHAR16 Word;

		tempBuf = (CHAR16*)AllocateZeroPool (Len + sizeof (CHAR16));		//If full string is given then NULL wont be there so crashing so added one NULL

		if (NULL == tempBuf)											//character at end
		{
			return EFI_OUT_OF_RESOURCES;
		}

		MemCpy (tempBuf, Buf, Len);

		//Finding length to allocate
		while (tempBuf [iIndex])
		{
			StringHexLength ++;
			iIndex ++;
		}

		iIndex = 0;
		*Str = (CHAR16*)AllocateZeroPool ((StringHexLength  *  sizeof (CHAR16) * 4) + sizeof (CHAR16));

		if (NULL == *Str )
		{
			FreePool(tempBuf);
			return EFI_OUT_OF_RESOURCES;
		}

		tempStr = *Str;

		while (tempBuf [iIndex])
		{
			Word = tempBuf [iIndex];
			Byte =  ((UINT8)(Word >> 8));
			tempStr [iIndex * 4] = NibbleToHexChar ((UINT8)(Byte >> 4));
			tempStr [iIndex * 4 + 1] = NibbleToHexChar (Byte);
			Byte = Word & 0x00FF;
			tempStr [iIndex * 4 + 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
			tempStr [iIndex * 4 + 3] = NibbleToHexChar (Byte);
			iIndex ++;
		}

		*HexStringBufferLength =  (StringHexLength  *  sizeof (CHAR16) * 4) + sizeof (CHAR16);
		tempStr [StringHexLength * 4] = L'\0';
		MemFreePointer ((VOID **)&tempBuf);
	}

	return EFI_SUCCESS;
}


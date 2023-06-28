#ifdef _WIN32
//#include "tchar.h"
#endif
//extern "C"
//{
#include "string.h"
#include "wchar.h"
#include "stdlib.h"
#include "AmiSceCommon.h"
#include "Library/UefiLib.h"
#include "Wrapper.h"
#include <AmiLib.h>
#include <Token.h>
#include "Library/MemoryAllocationLib.h"
//}
#define _EFI 1
#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#endif
#include "UEFI21StringManagement.h"
#include "UEFI21HIIParser.h"

extern int wnstrcmp(wchar_t *wide, unsigned short *narrow);
VOID _Wmemcpy( wchar_t *VariableName, UINT16 *VarName,int Size);
#if defined(_LNX) || defined(_BSD)
extern size_t _Wstrlen(UINT16 *str);
#endif
#ifdef _WIN32
//#define _Wstrlen wcslen
#endif

//extern "C"
//{
//#include "wchar.h"
//}
//----------------------------------------------------------------------------

#define APTIO_HII_PROTOCOL_GUID  \
	{ 0xea816d2c, 0xcee5, 0x4f02, 0x99, 0xb5, 0xd3, 0x90, 0x5c, 0xbb, 0xd0, 0x77 }
//----------------------------------------------------------------------------


extern wchar_t *GetCurrentSystemLanguage();

//extern VOID MemAllocFailure();

extern long g_pkglen;

//extern long *g_PkgFilePos;

//extern UINT16 g_PkgCount;

extern FILE *g_HIIDBFile;
extern BOOLEAN gMapLang ;


BOOLEAN IsUefi21UQISupported()
{
  return FALSE;
}

//UINT8 * GetNextPackagelist(FILE *fp, int Index, UINT8* Buffer);

UINT32 GetUefi21HiiLanguages( UINT8 *PackageList, VOID* HiiPackage, LANGUAGE_DATA **Language)
{
//  EFI_HII_PACKAGE_LIST_HEADER *RefPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )PackageList;
  EFI_HII_PACKAGE_LIST_HEADER *CurPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )HiiPackage;
  EFI_HII_PACKAGE_HEADER *PackageHdr = ( EFI_HII_PACKAGE_HEADER * )NULL;
  LANGUAGE_DATA *Lang = ( LANGUAGE_DATA * )*Language;

  UINT32 Count = 0;
  UINT32 Index = 0;
  UINT32 Offset = 0;
  BOOLEAN Found = FALSE;
  UINT16 PackageCount = 0;
  EFI_HII_STRING_PACKAGE_HDR *StringPack = (EFI_HII_STRING_PACKAGE_HDR *)NULL;
  char *Local8 = ( char *)NULL;
  UINT16 *Text = ( UINT16 * )NULL;
  wchar_t *Local = ( wchar_t * )NULL;
  UINT16 *Local16 = (UINT16*)NULL;
  UINT16 Zero = 0;


  //UINT8*	Buffer=0;
  //PHY_ADDR_T IntrBufPhyAddr = 0;
  //LPBYTE IntrBuf = 0;
  /*AllocPhyMem( g_pkglen, &IntrBufPhyAddr, ( LPBYTE * )&IntrBuf );
  if( IntrBufPhyAddr == 0 )
  {
     MemAllocFailure();
  }
  Buffer= (UINT8*)IntrBuf;
  Buffer = (UINT8*) AllocMem(g_pkglen);*/

//  PackageCount = g_PkgCount;
PackageCount = GetTotalPackageList(( UINT8* ) HiiPackage );
  // Get the Unique Languages
  for( Index = 0; Index < PackageCount && Found == FALSE; Index++ )
  {

	  Offset = sizeof( EFI_HII_PACKAGE_LIST_HEADER );
	  do
		{
		  StringPack = (EFI_HII_STRING_PACKAGE_HDR *)NULL;

		  PackageHdr = ( EFI_HII_PACKAGE_HEADER * )( ( UINT8 * )CurPackageList + Offset );
		  if( PackageHdr->Type == EFI_HII_PACKAGE_STRINGS )
		  {
			StringPack = ( EFI_HII_STRING_PACKAGE_HDR * )PackageHdr;
			//
			// We have the right string package - size it, and copy it to the StringBuffer
			//
			Local8 = ( char *)NULL;
			Text = ( UINT16 * )NULL;
			Local = ( wchar_t * )NULL;
			Local16 = (UINT16*)NULL;
			Zero = 0;

			// Copy the Lang Code
			Local8 = ( char * ) (( UINT8 * )StringPack + LANG_OFFSET_UEFI21 );
			Local = (wchar_t *)calloc( strlen( Local8 ) + 1, sizeof( wchar_t));
			if( Local == NULL )
			{
				  throw(L"Memory Allocation Failure");//MemAllocFailure();
			}
			mbstowcs( Local, Local8, strlen( Local8 ));

			// Search the gLang To check if the Language Already Exits
			for(Count = 0; Count < gLangCount; Count++)
			{
			  if( wcscmp( Lang[Count].LangCode, Local ) == 0 )
			  {
				Found = TRUE;
				break;
			  }
			}

			if( Found == FALSE )
			{
			  Lang = ( LANGUAGE_DATA * )realloc( Lang, ( size_t )( gLangCount + 1 ) * sizeof( LANGUAGE_DATA ));
			  if( Lang == NULL )
			  {
				   throw(L"Memory Allocation Failure");//MemAllocFailure();
			  }
			  Lang[gLangCount].LangCode = (wchar_t *)NULL;
			  Lang[gLangCount].LangString = (wchar_t *)NULL;
			  // Since StringPack is a packed structure, we need to determine the string's
			  // size safely, thus byte-wise.  Post-increment the size to include the null-terminator
			  //
			  for( Count = 0; CompareMem( &Local[Count], &Zero, 2 ); Count++ );
			  Count++;

			  // Allocate memory and copy string
			  Lang[gLangCount].LangCode = ( wchar_t * )calloc( Count+1, sizeof( wchar_t ));
			  if(Lang[gLangCount].LangCode)
           	  {
				memcpy( Lang[gLangCount].LangCode, Local, Count * sizeof(wchar_t) );
        	  }
			  else
			  {
				   throw(L"Memory Allocation Failure");//MemAllocFailure();
			  }

			  // Copy the Printable Language String
			  //used 0x01 as the token, since the first string in a string package will point to the
			  //language code
			  Local16 = ( UINT16 * )GetUefi21StringOffset(( UINT8** )( &StringPack ), 0x01 );
			  if( Local16 )
			  {
				for( Count = 0; CompareMem( &Local16[Count], &Zero, 2 ); Count++ );
				Count++;
				// Allocate memory and copy string
				Lang[gLangCount].LangString = ( wchar_t * )calloc( Count, sizeof( wchar_t ));
				if(Lang[gLangCount].LangString)
				{
					#if defined(_LNX) || defined(_BSD)
					_Wmemcpy( Lang[gLangCount].LangString, Local16, Count * 2 );
					#else
					memcpy( Lang[gLangCount].LangString, Local16, ( size_t )Count * sizeof(wchar_t) );
					#endif
				}
				else
				{
				    throw(L"Memory Allocation Failure");//MemAllocFailure();
				}
			  }
			  gLangCount++;
			}
		  }
		  Offset += PackageHdr->Length;
		}while( Offset < CurPackageList->PackageLength );


      // Move To the Next package list
      CurPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )(( UINT8 * )CurPackageList + CurPackageList->PackageLength );

    //Re-Initialize Offset
    Offset = 0;
  }
  *Language = Lang;
  //FreePhyMem( IntrBufPhyAddr, g_pkglen );
 // FreeMem(Buffer);
  return gLangCount;
}


wchar_t *_UEFI21HiiGetString( UINT8 *PackageList, VOID* HiiPackage, UINT16 Token , UINT8 *Lang8, BOOLEAN MapLangFlag)
{
  EFI_HII_PACKAGE_LIST_HEADER *RefPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )PackageList;
  EFI_HII_PACKAGE_LIST_HEADER *CurPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )HiiPackage;
  EFI_HII_PACKAGE_HEADER *PackageHdr = ( EFI_HII_PACKAGE_HEADER * )NULL;

  UINT32 Count = 0;
  UINT32 Index = 0;
  UINT32 Offset = 0;
  wchar_t *Text = ( wchar_t * )NULL;
  UINT16 *Local = ( UINT16 * )NULL;
//  char  *Text8 = (char *)NULL;
  BOOLEAN Found = FALSE;
  UINT16 PackageCount = 0;
  static wchar_t *Lang = (wchar_t *)NULL;
  EFI_HII_STRING_PACKAGE_HDR *StringPack = ( EFI_HII_STRING_PACKAGE_HDR * )PackageHdr;
  UINT16 Zero = 0;
  UINTN j;
  CHAR8 **MappingLangArr = (CHAR8 **)NULL; 
  UINTN MappingLangCount = 0;
    
  /*
  PHY_ADDR_T IntrBufPhyAddrText = 0, IntrBufPhyAddr = 0;
  LPBYTE IntrBufText = 0, IntrBuf = 0;
  UINT8*	Buffer=0;

  //Buffer = (UINT8*) calloc( (size_t) g_pkglen, sizeof( UINT8 ));

  AllocPhyMem( g_pkglen, &IntrBufPhyAddr, ( LPBYTE * )&IntrBuf );
  if( IntrBufPhyAddr == 0 )
  {
  	MemAllocFailure();
  }
  Buffer= (UINT8*)IntrBuf;

  */

  MappingLangCount = GetPLDMMappingCount();
  MappingLangArr = GetPLDMMappingLanguageID();
 
//  PackageCount = g_PkgCount;
  PackageCount = GetTotalPackageList( ( UINT8* )HiiPackage );
  for( Index = 0; Index < PackageCount && Found == FALSE; Index++ )
  {

    //Compare Data Table GUID with IFR DataTable
    if( CompareGuid( &RefPackageList->PackageListGuid, &CurPackageList->PackageListGuid) == 1 )
    {
	  Offset = sizeof( EFI_HII_PACKAGE_LIST_HEADER );
      // Matching Guid The String may be in this package list
	  do
	  {
		PackageHdr = ( EFI_HII_PACKAGE_HEADER * )( ( UINT8 * )CurPackageList + Offset );
		if( PackageHdr->Type == EFI_HII_PACKAGE_STRINGS )
		{
		  StringPack = ( EFI_HII_STRING_PACKAGE_HDR * )PackageHdr;

		
		  for (j = 0; j < MappingLangCount; j++) {
		           
		      if( (MapLangFlag && (strcmp( MappingLangArr[j], ( char* )(( UINT8 * )StringPack +  LANG_OFFSET_UEFI21 )) == 0)) ||
		              (!MapLangFlag && (strcmp( Lang8, ( char* )(( UINT8 * )StringPack +  LANG_OFFSET_UEFI21 )) == 0)) )
              {     // found string for Lang8
                    Zero = 0;
        
                    Local = ( UINT16 * )GetUefi21StringOffset(( UINT8** )( &StringPack ), Token );
                    // GetUefi21StringOffset() will return null, if the string corresponding to the token
                    // was not found.
                    if( Local )
                    {
                        for( Count = 0; CompareMem( &Local[Count], &Zero, 2 ); Count++ );
                        Count++;
                        break;
                    }
              }
              if( FALSE == MapLangFlag)
                  break;
		  }
		  if( Count > 0 )
		      break;
		}
		Offset += PackageHdr->Length;
	  }while( Offset < CurPackageList->PackageLength );

	  if(Count !=0){
	  	// Allocate memory and copy string
	   //AllocPhyMem( ( size_t )Count * sizeof( UINT16 ), &IntrBufPhyAddrText, ( LPBYTE * )&IntrBufText );
	   Text = (wchar_t *) calloc( Count, sizeof(wchar_t));
	   //Text = (CHAR16 *) AllocMem(( size_t )Count * sizeof( UINT16 ));

	    /*if( IntrBufPhyAddrText == 0 )
	    {
	       MemAllocFailure();
    	}
	    Text  =  ( CHAR16 * )IntrBufText;*/

	  	if(Text)
	  	{
			#if defined(_LNX) || defined(_BSD)
				_Wmemcpy(Text, Local, Count * 2 );
			#else
				memcpy( Text, Local, ( size_t )Count * sizeof(wchar_t) );
			#endif
			Found = TRUE;
	  	}
	  	else
	  	{
			 throw(L"Memory Allocation Failure");//MemAllocFailure();
		}
	  }
    }else
    {
      // Move To the Next package list
     CurPackageList = ( EFI_HII_PACKAGE_LIST_HEADER * )(( UINT8 * )CurPackageList + CurPackageList->PackageLength );

    }

  }

  //FreePhyMem( IntrBufPhyAddr, g_pkglen );
  //free(Buffer);

  if(Text)
  {
	//Text8 = (char*)calloc(wcslen(Text) + 1, sizeof(char));
 //   if(NULL == Text8)
 //   {
	//  MemAllocFailure();
 //   }
	//wcstombs(Text8, Text, wcslen(Text));
	////FreePhyMem( IntrBufPhyAddrText,( size_t )Count * sizeof( UINT16 )  );
	////FreeMem((LPBYTE) Text);
	//free(Text);
	return Text;
  }
  else
  	return (wchar_t *)NULL;
}

// get string for current language
wchar_t *HiiGetCurrentLangString( UINT8 *PackageList, VOID* HiiPackage, UINT16 Token)
{
    wchar_t *Lang = (wchar_t *)NULL;
    char Lang8[0x10];
    Lang8[0] = 0;
    Lang = GetCurrentSystemLanguage();
    if (Lang)
    {
        wcstombs( Lang8, Lang, sizeof(Lang8) - 1);
        free(Lang);
    }
    else
        // failed to get current language
        strcpy(Lang8, "en-US");
    return _UEFI21HiiGetString(PackageList, HiiPackage, Token, Lang8,FALSE);
}

wchar_t *UEFI21HiiGetString( UINT8 *PackageList, VOID* HiiPackage, UINT16 Token , BOOLEAN MapLangFlag)
{
    char Lang8[0x10] = {0};
    if ( FALSE == MapLangFlag)
		strcpy(Lang8, "en-US");
	return _UEFI21HiiGetString(PackageList, HiiPackage, Token, Lang8,MapLangFlag);
}


UINT8 *GetUefi21StringOffset( UINT8** pStringPack, UINT16 Token )
{
	UINT8* StringPack = *pStringPack;
	UINT32 Offset = 0;
//	UINT16 Zero = 0;
	UINT8  BlockType = 0;
	UINT16  StringCount = 0;
	UINT16  StringSize = 0;

	Offset = (( EFI_HII_STRING_PACKAGE_HDR* )StringPack)->StringInfoOffset;
	BlockType = *(StringPack + Offset);

	do
	{
		switch (BlockType)
		{
			case EFI_HII_SIBT_STRING_UCS2:
				++Offset;
#if defined(_LNX) || defined(_BSD)
				StringSize = _Wstrlen( (UINT16*) (StringPack + Offset));
#else
				StringSize = (UINT16)_Wstrlen( (wchar_t*) (StringPack + Offset));
#endif

				++StringCount;
				if( StringCount == Token )
					return ( StringPack + Offset );
				Offset += sizeof( UINT16 ) * ( StringSize + 1 );
				break;

			case EFI_HII_SIBT_STRINGS_UCS2:
			{
				UINT16 k = 0;
				EFI_HII_SIBT_STRINGS_UCS2_BLOCK StringBlock;
				memcpy( &StringBlock, StringPack + Offset,
					sizeof( EFI_HII_SIBT_STRINGS_UCS2_BLOCK ));
				Offset += ( sizeof( EFI_HII_STRING_BLOCK ) + sizeof( UINT16 ));

				for (k = 0; k < StringBlock.StringCount; ++k)
				{
#if defined(_LNX) || defined(_BSD)
				StringSize = _Wstrlen( (UINT16*) (StringPack + Offset));
#else
				StringSize = (UINT16)_Wstrlen( (wchar_t*) (StringPack + Offset));
#endif
					++StringCount;
					if( StringCount == Token )
						return ( StringPack + Offset );
					Offset += sizeof( UINT16 ) * ( StringSize + 1 );
				}
				break;
			}

			case EFI_HII_SIBT_SKIP2:
			{
				EFI_HII_SIBT_SKIP2_BLOCK StringBlock;
				memcpy( &StringBlock, StringPack + Offset,
					sizeof( EFI_HII_SIBT_SKIP2_BLOCK ));
				if( Token <= ( StringCount + StringBlock.SkipCount ))
					return (UINT8 *)NULL;
				Offset += sizeof( EFI_HII_SIBT_SKIP2_BLOCK );
				StringCount += StringBlock.SkipCount;
				break;
			}
			case EFI_HII_SIBT_SKIP1:
			{
				EFI_HII_SIBT_SKIP1_BLOCK StringBlock;
				memcpy( &StringBlock, StringPack + Offset,
					sizeof( EFI_HII_SIBT_SKIP1_BLOCK ));
				if( Token <= ( StringCount + StringBlock.SkipCount ))
					return (UINT8 *)NULL;
				Offset += sizeof( EFI_HII_SIBT_SKIP1_BLOCK );
				StringCount += StringBlock.SkipCount;
				break;
			}
			case EFI_HII_SIBT_END:
				return (UINT8 *)NULL; //End of string package
			default:
			{
				//fwprintf(stderr, L"The BIOS has unsupported string blocks.\n");
				exit(1);
			}
		}
		BlockType = *( StringPack + Offset );
	} while (( BlockType != EFI_HII_SIBT_END ) && ( Offset < (( EFI_HII_STRING_PACKAGE_HDR* )StringPack)->Header.Length ));
	return (UINT8 *)NULL;
}

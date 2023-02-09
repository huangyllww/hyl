//#include "wchar.h"
//#include "memory.h"
//#include "stdlib.h"
#define _EFI 1

#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "os_hook.h"
#include "EDKStructures.h"
//#include "NvramLib.h"
#endif

#include "EfiCommon.h"
//Added for EDK
//#include "EDKStructures.h"
#include "NvramLib.h"
//Removed for EDK
/*#ifdef _WIN32
#include EFI_PROTOCOL_DEFINITION(Hii)
#endif*/
#include "UEFI20StringManagement.h"
#include "UEFI20Variable.h"

//----------------------------------------------------------------------------

#define APTIO_HII_PROTOCOL_GUID  \
	{ 0xea816d2c, 0xcee5, 0x4f02, 0x99, 0xb5, 0xd3, 0x90, 0x5c, 0xbb, 0xd0, 0x77 }
//----------------------------------------------------------------------------

extern wchar_t *GetCurrentSystemLanguage();
extern int wnstrcmp(wchar_t *wide, unsigned short *narrow);
VOID _Wmemcpy( wchar_t *VariableName, UINT16 *VarName,int Size);

BOOLEAN IsUQISupported()
{
  return FALSE;
}

#if 0
UINT32 GetUefi20HiiLanguages( UINT8 *IFRDataTable, VOID* HiiPackage, LANGUAGE_DATA **Language )
{
  EFI_HII_EXPORT_TABLE *ExportTable = ( EFI_HII_EXPORT_TABLE * )HiiPackage;
  EFI_HII_DATA_TABLE *RefDataTable = ( EFI_HII_DATA_TABLE * )IFRDataTable;
  EFI_HII_DATA_TABLE *DTable = NULL;
  EFI_HII_PACK_HEADER *PackageHdr = ( EFI_HII_PACK_HEADER * )NULL;
  LANGUAGE_DATA *Lang = ( LANGUAGE_DATA * )*Language;

  UINTN Count = 0;
  UINTN StringPackCount = 0;
  UINTN Index = 0;
  UINT32 Offset = 0;
  BOOLEAN Found = FALSE;

  DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )ExportTable + sizeof( EFI_HII_EXPORT_TABLE ));
  //Count Number of Language Packs in HII DB
  for( Index = 0; Index < ExportTable->NumberOfHiiDataTables; Index++ )
  {
    Count += DTable->NumberOfLanguages;
    DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )DTable + DTable->DataTableSize );
  }

  Lang = ( LANGUAGE_DATA * )calloc( Count, sizeof( LANGUAGE_DATA ));
  if( Lang == NULL )
	throw(L"Memory Allocation Failure");

  // Get the Unique Languages
  DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )ExportTable + sizeof( EFI_HII_EXPORT_TABLE ));
  for( Index = 0; Index < ExportTable->NumberOfHiiDataTables && Found == FALSE; Index++ )
  {
    Offset = DTable->StringDataOffset;
    if( Offset )
    {
      do
        {
          EFI_HII_STRING_PACK *StringPack = NULL;

          PackageHdr = ( EFI_HII_PACK_HEADER * )( ( UINT8 * )DTable + Offset );
          if( PackageHdr->Type == EFI_HII_STRING )
          {
            UINT32 LangOffset = 0;
            UINT32 LangStrOffset = 0;
            UINT16 Zero = 0;
            UINT16 *Text = ( UINT16 * )NULL;
            UINT16 *Local = ( UINT16 * )NULL;
            BOOLEAN Found = FALSE;

            StringPack = ( EFI_HII_STRING_PACK * )PackageHdr;

            //
            // We have the right string package - size it, and copy it to the StringBuffer
            //
            memcpy( &LangOffset, &StringPack->LanguageNameString, sizeof ( RELOFST ) );
            memcpy( &LangStrOffset, &StringPack->PrintableLanguageName, sizeof ( RELOFST ) );

            // Copy the Lang Code
            Local = ( UINT16 * ) (( UINT8 * )StringPack + LangOffset );

            // Search the gLang To check if the Language Already Exits
            for(Count = 0; Count < gLangCount; Count++)
            {
              //if( wcscmp( Lang[Count].LangCode, Local ) == 0 )
				if(wnstrcmp(Lang[Count].LangCode,Local) == 0)
              {
                Found = TRUE;
                break;
              }
            }

            if( Found == FALSE )
            {
              for( Count = 0; CompareMem( &Local[Count], &Zero, 2 ); Count++ );
              Count++;

              // Allocate memory and copy string
              Lang[gLangCount].LangCode = ( wchar_t * )calloc( Count, sizeof( wchar_t ));

			 if(Lang[gLangCount].LangCode)
				#if defined(_LNX) || defined(_BSD)
				_Wmemcpy( Lang[gLangCount].LangCode, Local, Count * 2 );
				#else
				 memcpy(Lang[gLangCount].LangCode,Local,( size_t )Count * sizeof(wchar_t));
				#endif
			  else
			    throw(L"Memory Allocation Failure");

              // Copy the Printable Language String
              Local = ( UINT16 * ) (( UINT8 * )StringPack + LangStrOffset );
              for( Count = 0; CompareMem( &Local[Count], &Zero, 2 ); Count++ );
              Count++;
              // Allocate memory and copy string
              Lang[gLangCount].LangString = ( wchar_t * )calloc( Count, sizeof( wchar_t ));
			  if(Lang[gLangCount].LangString)
				#if defined(_LNX) || defined(_BSD)
				_Wmemcpy( Lang[gLangCount].LangString, Local, Count * 2 );
				#else
				memcpy(Lang[gLangCount].LangString,Local,( size_t )Count * sizeof(wchar_t));
				#endif
			  else
			    throw(L"Memory Allocation Failure");
              gLangCount++;
            }
          }
          StringPackCount++;
          if( StringPackCount < DTable->NumberOfLanguages )
          {
            Offset += PackageHdr->Length;
          }else
          {
            break;
          }
        }while( PackageHdr->Type == EFI_HII_STRING );
    }

    // Move To the Next DataTable
    DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )DTable + DTable->DataTableSize );
  }

  *Language = Lang;

  return gLangCount;
}

wchar_t *UEFI20HiiGetString( UINT8 *IFRDataTable, VOID* HiiPackage, UINT16 Token )
{
  EFI_HII_EXPORT_TABLE *ExportTable = ( EFI_HII_EXPORT_TABLE * )HiiPackage;
  EFI_HII_DATA_TABLE *RefDataTable = ( EFI_HII_DATA_TABLE * )IFRDataTable;
  EFI_HII_DATA_TABLE *DTable = NULL;
  EFI_HII_PACK_HEADER *PackageHdr = ( EFI_HII_PACK_HEADER * )NULL;

  UINTN Count = 0;
  UINTN Index = 0;
  UINTN HIndex = 0;
  UINT32 Offset = 0;
  wchar_t *Text = ( wchar_t * )NULL;
  UINT16 *Local = ( UINT16 * )NULL;
  BOOLEAN Found = FALSE;

  //static CHAR16 *Lang = NULL;
  static wchar_t *Lang = NULL;

  if( Lang == NULL )
  {
    Lang = GetCurrentSystemLanguage();
  }

  DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )ExportTable + sizeof( EFI_HII_EXPORT_TABLE ));
 // first check GUID in package with same handle
  for( HIndex = 0; Index < ExportTable->NumberOfHiiDataTables; HIndex++ )
  {
        if (DTable->HiiHandle == RefDataTable->HiiHandle
                && CompareMem( &RefDataTable->PackageGuid, &DTable->PackageGuid, sizeof( EFI_GUID )) == 0 )
        {
            Index = HIndex;
            break;
        }
        // Move To the Next DataTable
        DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )DTable + DTable->DataTableSize );
  }

  if (Index == 0)
		  DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )ExportTable + sizeof( EFI_HII_EXPORT_TABLE ));
  for(; Index < ExportTable->NumberOfHiiDataTables && Found == FALSE; Index++ )
  {
    //Compare Data Table GUID with IFR DataTable
    if( CompareMem( &RefDataTable->PackageGuid, &DTable->PackageGuid, sizeof( EFI_GUID )) == 0 )
    {
      // Matching Guid The String may be in this Data Table
      Offset = DTable->StringDataOffset;
      if( Offset )
      {
        do
          {
            PackageHdr = ( EFI_HII_PACK_HEADER * )( ( UINT8 * )DTable + Offset );
            if( PackageHdr->Type == EFI_HII_STRING )
            {
              EFI_HII_STRING_PACK *StringPack = ( EFI_HII_STRING_PACK * )PackageHdr;

              if( wnstrcmp( Lang, ( UINT16 * )(( UINT8 * )StringPack + StringPack->LanguageNameString )) == 0 )
			  {
                //
                // We have the right string package - size it, and copy it to the StringBuffer
                //
                UINT32 *StringPointer = ( UINT32 * )(( UINT8 * )StringPack + sizeof( EFI_HII_STRING_PACK ) );
                UINT32 Offset = 0;
                UINT16 Zero = 0;

                if ( Token >= StringPack->NumStringPointers )
                {
                  goto DONE;
                } else
                {
                  memcpy( &Offset, &StringPointer[Token], sizeof ( RELOFST ) );
                }
                //
                // Since StringPack is a packed structure, we need to determine the string's
                // size safely, thus byte-wise.  Post-increment the size to include the null-terminator
                //
                Local = ( UINT16 * ) (( UINT8 * )StringPack + Offset );
                for( Count = 0; CompareMem( &Local[Count], &Zero, 2 ); Count++ );
                Count++;
                break;
              }
            }
            Offset += PackageHdr->Length;
          }while( PackageHdr->Type == EFI_HII_STRING );

          // Allocate memory and copy string
          Text = ( wchar_t * )calloc( Count, sizeof( wchar_t ));
		  if(Text)
		  {
	        #if defined(_LNX) || defined(_BSD)
			_Wmemcpy( Text, Local, Count * 2 );
			#else
				memcpy(Text,Local,Count*sizeof(wchar_t));
			#endif
		    Found = TRUE;
		  }
		  else
		    throw(L"Memory Allocation Failure");
      }
    }else
    {
      // Move To the Next DataTable
      DTable = ( EFI_HII_DATA_TABLE * )(( UINT8 * )DTable + DTable->DataTableSize );
    }

  }

DONE:
  return Text;
}
#endif

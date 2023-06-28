//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/AdvScript.h 2     6/26/13 11:15a Arund $
//
// $Revision: 2 $
//
// $Date: 6/26/13 11:15a $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/AdvScript.h $
// 
// 2     6/26/13 11:15a Arund
// Updated code with PLDM integration and Mapping language
//
// 6     9/27/10 5:21p Timothym
// Correct typo.
//
// 5     9/25/10 10:45p Timothym
// EFI shell port.
//
//********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          AdvScript.h
//
// Description:   Contain definitions for advanced script processing.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef _ADV_SCRIPT_H_
#define _ADV_SCRIPT_H_

#define _EFI 1

#if defined(_LNX) || defined(_BSD) || defined(_EFI)
#include "../LIB/NvramLib.h"
#include "../LIB/UEFI20Variable.h"
#endif

//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name: HII_DATA
//
// Description:
// This structure is a single point access that contains all the information
// regarding the data required for HII_DATA
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// HIIDBCRC            UINT32                 // CRC32 for the HiiDb
// HIIDBData            UINT8*                  Buffer containing the HII DB
//
// Notes:
//
// Referrals:
//
//----------------------------------------------------------------------------
//<AMI_SHDR_END>
typedef struct _HII_DATA
{
  UINT32 HIIDBCRC;  // CRC32 for the HiiDb
  UINT8 *HIIDBData; // Buffer containing the HII Database
}HII_DATA;

//----------------------------------------------------------------------------

typedef struct _CONTROL_RESET_FLAGINFO CONTROL_RESET_FLAGINFO;

//----------------------------------------------------------------------------
//<AMI_SHDR_START>
//----------------------------------------------------------------------------
// Name: CONTROL_RESET_FLAGINFO
//
// Description:
// This structure stores the information of controls having RESET_REQUIRED flag
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// VariableIndex         UINT16                  stores Control'sVariableStorage value
// DataOffset   UINT16                  stores Control's DataOffset
// DataWidth         UINT16                stores Control's DataWidth
//----------------------------------------------------------------------------
//<AMI_SHDR_END>

struct _CONTROL_RESET_FLAGINFO
{
	UINT16	VariableIndex;
	UINT16 DataOffset;
	UINT16	DataWidth;
	CONTROL_RESET_FLAGINFO  *Next;
};

wchar_t *PldmGetHiiString( UINT8 *DataTable, UINT8 *HiiData, UINT16 Token ,BOOLEAN MapLang);
VOID FindUefiVersion( UINT8 *AmisceData );
void	CleanUpPldmLibrary(INTN ExitVal);
VOID InitializePldmLibrary(VOID);
#endif /* _ADV_SCRIPT_H_ */
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//

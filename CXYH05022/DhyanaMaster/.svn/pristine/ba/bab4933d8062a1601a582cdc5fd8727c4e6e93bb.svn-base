#include "UEFI20HIIParser.h" // to re use structures like CONTROL INFO, VALUE INFO etc..
//#include "stdio.h"
//#include "../App/Amisce.h"
#include "AdvScript.h"
UINT16 GetTotalPackageList( UINT8 *HiiData );
UINT8 *GetNextPackageList( UINT8 * HiiData, UINT8 **CurrentPackageList );
UINT8 *GetNextUefiFormSet( UINT8 *CurrentPackageList, UINT8 **CurrentFormSet, UINT8 *Guid,
                      UINT16 *FormSetTitle, UINT16 *FormSetHelp, UINT16 *Class, UINT16 *SubClass );
UINT8 *GetNextUefiForm( UINT8 *FormSet, UINT8 **CurrentForm, UINT16 *FormID, UINT16 *FormTitle );
CONTROL_INFO *GetNextUefiControl( UINT8 *CurrentForm, UINT8 **CurrentControl, HII_DATA *AmisceData, UINT8 *CurrentPackageList, CONTROL_INFO *Control );
BOOLEAN IsValidUefi21Pack( UINT8* HiiData );
UINT8 * GetNextPackagelist(FILE *fp, int Index, UINT8* Buffer, long *lPackgeFilePos);
UINT32 GetMaximumPackageSize21(FILE *hiiDBFile);
UINT8 _GetUefiMaxEntries(UINT8 * HiiData);

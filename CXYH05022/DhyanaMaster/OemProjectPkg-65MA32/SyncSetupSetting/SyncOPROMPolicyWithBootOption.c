//#include "Porting.h"
#include "AmiDxeLib.h"
#include "BootOptions.h"
#include "EfiOsNamesFilePathMaps.h"
#include "Setup.h"
#include<Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>


typedef struct
{
	CHAR16 *FilePath;            ///< Path to open the OS image.
	CHAR16 *BootOptionName;      ///< Or, OS Name.
}NAME_MAP;

//extern DLIST *BootOptionList;
NAME_MAP UefiFileNameMaps[] = { EfiOsFilePathMaps {NULL,L"UEFI:"},{NULL,NULL} };

EFI_STATUS
BootOptionStringCmp(
    IN    CHAR16    *BootOptionString,
    IN    CHAR16    *UefiFileNameMapsString,
    IN    UINTN     CmpStringSize
)
{
    UINT8     iString;

    for(iString=0;iString < CmpStringSize;iString++)
    {
      if(*(BootOptionString+iString) != *(UefiFileNameMapsString+iString))
        return (iString+1);
    }

    return EFI_SUCCESS;
}

VOID SyncOPROMPolicyWithBootOption(){
  UINT8                   iFileNameMaps;
  EFI_STATUS              Status=EFI_SUCCESS;
  EFI_LOAD_OPTION         *Option=NULL;
  UINT16                  *BootOrder=NULL;
  UINTN                   BootOrderSize,OptionSize;
  CHAR16                  BootVarName[15],*pBootOptionString=NULL; //Boot0000
  UINTN                   DescriptionSize,UefiFileNameSize;
  UINTN                   VariableSize= sizeof(SETUP_DATA);
  SETUP_DATA              SetupData;
  EFI_GUID                gSetupGuid = SETUP_GUID;
  UINT32                  Attributes;

  DEBUG((EFI_D_ERROR, "[%a][%d]\n\n",__FUNCTION__,__LINE__));

  Status = gRT->GetVariable (L"Setup", &gSetupGuid, &Attributes, &VariableSize,&SetupData);
  if (EFI_ERROR(Status)) return;

  Status = GetEfiVariable(L"BootOrder", &EfiVariableGuid, NULL, &BootOrderSize, (VOID**)&BootOrder);
  if (EFI_ERROR(Status)) return;
  
  Swprintf(BootVarName,L"Boot%04X",BootOrder[0]);
  Status = GetEfiVariable(BootVarName, &EfiVariableGuid, NULL, &OptionSize, (VOID**)&Option);
  if (EFI_ERROR(Status)){
    // Workaround for non-UEFI specification complaint OS.
    // Some OS create BootXXXX variables using lower case letters A-F.
    // Search for lower case boot option variable if no upper case variable found.
    Swprintf(BootVarName,L"Boot%04x",BootOrder[0]);
    Status=GetEfiVariable(BootVarName, &EfiVariableGuid, NULL, &OptionSize,(VOID**)&Option);
    if (EFI_ERROR(Status)) return;
  }
  pBootOptionString = (CHAR16*)(Option+1);

  //DEBUG((EFI_D_ERROR, "[%a][%d] Option Description:%s\n\n",__FUNCTION__,__LINE__,pBootOptionString));
  DescriptionSize = (Wcslen(pBootOptionString)+1)*sizeof(CHAR16);

  
  Status=EFI_UNSUPPORTED; //Mizl-20200710-If no match boot option found, Option Will Not Sync >
  for(iFileNameMaps=0;UefiFileNameMaps[iFileNameMaps].BootOptionName != NULL;iFileNameMaps++)
  {
    //DEBUG((EFI_D_ERROR, "[%a][%d]Boot Option Name:%s\n\n",__FUNCTION__,__LINE__,UefiFileNameMaps[iFileNameMaps].BootOptionName));
    UefiFileNameSize = (Wcslen(UefiFileNameMaps[iFileNameMaps].BootOptionName))*sizeof(CHAR16);
    if(UefiFileNameSize > DescriptionSize)
      continue;
    
    Status=EFI_UNSUPPORTED;
    Status = BootOptionStringCmp(pBootOptionString,UefiFileNameMaps[iFileNameMaps].BootOptionName,(UefiFileNameSize/sizeof(CHAR16)));
    //DEBUG((EFI_D_ERROR,"[%a][%d]Return Status:%x\n\n",__FUNCTION__,__LINE__,Status));
    if(Status == EFI_SUCCESS)
      break;
  }

  if(Status == EFI_SUCCESS)
  {
    //Match UEFI Boot Option
    if(SetupData.OpROMEN[VIDEOINDX] == CSMSETUP_UEFI_ONLY_OPROMS)
      return;
//<Yanshp-20210805-Sync Other PCI devices value from video +>
    else
    {
      DEBUG((EFI_D_ERROR, "[%a][%d]We Need Sync Option Setting,Set UEFI MODE\n\n",__FUNCTION__,__LINE__));
      SetupData.OpROMEN[VIDEOINDX] = CSMSETUP_UEFI_ONLY_OPROMS;
      //SetupData.OpROMEN[OLDINDX] = CSMSETUP_UEFI_ONLY_OPROMS;
    }
  }
  else
  {
    //Match Legacy Boot Option
    if(SetupData.OpROMEN[VIDEOINDX] == CSMSETUP_LEGACY_ONLY_OPROMS)
      return;
    else
    {
      DEBUG((EFI_D_ERROR, "[%a][%d]We Need Sync Option Setting,Set LEGACY MODE\n\n",__FUNCTION__,__LINE__));
      SetupData.OpROMEN[VIDEOINDX] = CSMSETUP_LEGACY_ONLY_OPROMS;
      //SetupData.OpROMEN[OLDINDX] = CSMSETUP_LEGACY_ONLY_OPROMS;
    }
  }
  SetupData.OpROMEN[OLDINDX] = SetupData.OpROMEN[VIDEOINDX];
//<Yanshp-20210805-Sync Other PCI devices value from video ->
  Status = gRT->SetVariable(L"Setup",&gSetupGuid, Attributes, VariableSize,(VOID**)&SetupData);
  if(EFI_ERROR (Status))
  {
    DEBUG((EFI_D_ERROR, "[%a][%d]Sync BootOption Fail!!!\n\n",__FUNCTION__,__LINE__));
    return;
  }

  DEBUG((EFI_D_ERROR, "[%a][%d]We Need Reset Sytem to Wake the OPROM!!!\n\n",__FUNCTION__,__LINE__));
  gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);

  return;
}

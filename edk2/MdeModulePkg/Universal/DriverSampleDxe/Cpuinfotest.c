/** @file
This is an example of how a driver might export data to the HII protocol to be
later utilized by the Setup Protocol

Copyright (c) 2004 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Universal\DriverSampleDxe\DriverSample.h>
#include <Library/HiiLib.h>
//#include <Library/DeviceManagerUiLib/DeviceManager.h>
#define LABEL_1_VALUE               0x1111
#define LABEL_2_VALUE               0x1112
VOID InitString(EFI_HII_HANDLE HiiHandel ,EFI_STRING_ID StrRef,CHAR16 *sFormat, ...)
{
  CHAR16 s[1024];
  VA_LIST Arglist;
  VA_START(Arglist,sFormat);
  UnicodeVSPrint(s,sizeof(s),sFormat,Arglist);
  VA_END(Arglist);
  HiiSetString(HiiHandel,StrRef,s,NULL);
}


UINT16  CPUinfo[]={
   STRING_TOKEN (MYCPUINFO),   // Prompt text
   STRING_TOKEN (MYCPUINFO1),   // 
   STRING_TOKEN (MYCPUINFO2),   // Prompt text
   STRING_TOKEN (MYCPUINFO3),   // 
   STRING_TOKEN (MYCPUINFO4),   // Prompt text
   STRING_TOKEN (MYCPUINFO5), // 
   STRING_TOKEN (MYCPUINFO6),   // Prompt text
   STRING_TOKEN (MYCPUINFO7),   // 
   STRING_TOKEN (MYCPUINFO8),   // Prompt text
   STRING_TOKEN (MYCPUINFO9),   // 
   STRING_TOKEN (MYCPUINFO10),   // Prompt text
   STRING_TOKEN (MYCPUINFO11)  // 
};
UINT16  CPUinfoHelp[]={
   STRING_TOKEN (MYCPUINFOVALUE),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE1),   // 
   STRING_TOKEN (MYCPUINFOVALUE2),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE3),   // 
   STRING_TOKEN (MYCPUINFOVALUE4),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE5), // 
   STRING_TOKEN (MYCPUINFOVALUE6),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE7),   // 
   STRING_TOKEN (MYCPUINFOVALUE8),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE9),   // 
   STRING_TOKEN (MYCPUINFOVALUE10),   // Prompt text
   STRING_TOKEN (MYCPUINFOVALUE11)  // 

};


EFI_GUID  myvfrguid = DRIVER_SAMPLE_INVENTORY_GUID;

  EFI_STATUS
  HylUpdatCpuInfo (
    IN UINT16               QuestionIdBase,
    EFI_HII_HANDLE          Handle
)
{
  EFI_STATUS  status=EFI_SUCCESS;
  VOID                          *StartOpCodeHandle;
  VOID                          *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL            *StartLabel;
  EFI_IFR_GUID_LABEL            *EndLabel;
  UINT16  Index=0;
  {
    /* data */
  };
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);
  StartLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_1_VALUE;
  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;
   
 
   HiiCreateSubTitleOpCode (StartOpCodeHandle, STRING_TOKEN (MYCPUINFOA), 0, 0, 0);

   HiiCreateActionOpCode (
            StartOpCodeHandle,              // Container for dynamic created opcodes
            QuestionIdBase,                         // Question ID
            STRING_TOKEN (MYCPUINFO),   // Prompt text
            STRING_TOKEN (MYCPUINFO),   // Help text
            EFI_IFR_FLAG_CALLBACK,          // Question flag
            0                               // Action String ID
            );
  for(Index=0;Index<10;Index++)
  {
     InitString(Handle,CPUinfoHelp[Index],L"InitString--CPU%d",Index);
     InitString(Handle, CPUinfo [Index],L"InitString--CPU%d",Index);
     HiiCreateTextOpCode (
            StartOpCodeHandle,
            CPUinfoHelp[Index],
            CPUinfoHelp[Index],
            CPUinfo[Index]
            );

  }

status= HiiUpdateForm (
    Handle,
    &gDriverSampleFormSetGuid,  
    0x1,
    StartOpCodeHandle, // LABEL_START
    EndOpCodeHandle    // LABEL_END
    );
 HiiFreeOpCodeHandle (StartOpCodeHandle);
 HiiFreeOpCodeHandle (EndOpCodeHandle);
          // Refresh the Question value
return   status;
}

/** @file
 * OemBds.h

  $Revision:0A
  $Date:  lvych 2018/09/21

  Copyright (C) 2018 Dawning Information Industry Co.,LTD.All rights reserved.<BR>

**/
#ifndef _OEM_BDS_H_
#define _OEM_BDS_H_

#include <IndustryStandard/Pci30.h>
#include <IndustryStandard/PciCodeId.h>
#include "BootOptions.h"
#include "Protocol\AmiAhciBus.h"
#include <Library\PciLib.h>

//<Kangmm-20200811-Fix mantis 0042028 Harddisk default boot order doesn't not follow BBS Priorities +>
#ifndef DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID
/// GUID Definition for the Legacy Device Order Variable
#define DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID  \
  { 0x3c4ead08, 0x45ae, 0x4315, 0x8d, 0x15, 0xa6, 0x0e, 0xaa, 0x8c, 0xaf, 0x69 }
#endif

#ifndef DEFAULT_BOOT_ORDER_VARIABLE_GUID
/// GUID Definition for the Boot Order Variable
#define DEFAULT_BOOT_ORDER_VARIABLE_GUID  \
  { 0x45cf35f6, 0x0d6e, 0x4d04, 0x85, 0x6a, 0x03, 0x70, 0xa5, 0xb1, 0x6f, 0x53 }
#endif
//<Kangmm-20200811-Fix mantis 0042028 Harddisk default boot order doesn't not follow BBS Priorities ->
//----------------------------------------------------------------------
// Structures Definitions
//----------------------------------------------------------------------
typedef enum{
  BoTagsSata, //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support>
  BoTagSata,
  BoTagNvme,  //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support>
  BoTagNvmeM2, //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support>
  BoTagSAS,
  BoTagRAID,//<hanzhr001-20181107-Add RAID to Change Boot Option>
  BoTagASM1061M2, //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support>
  BoTagOther
} BOOT_OPTION_OEM_TAG;

typedef struct _PRIORITY_ATTRIBUTE {
  UINT32 Tag;
  UINT32 BusNum; //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support>
  UINT32 HighPriority;
  UINT32 MidPriority;
  UINT32 LowPriority;
} PRIORITY_ATTRIBUTE;

//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
typedef struct _PCI_BRIDGE_LIST
{
  UINT8   SocNum;
  UINT8   NbioNum;
  UINT8   DevNum;
  UINT8   FunNum;
}PCI_BRIDGE_LIST;
//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->

//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern VOID SortList(DLIST *List,  COMPARE_FUNCTION Compare);
extern VOID CreateGroupHeaders(DLIST *BootOptionList);

//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------

#endif

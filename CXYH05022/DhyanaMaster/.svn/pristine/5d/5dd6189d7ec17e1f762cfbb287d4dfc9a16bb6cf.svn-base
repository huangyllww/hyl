//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:$
//
// $Revision:$
//
// $Date:$
//**********************************************************************
// Revision History
// ----------------
// $Log:$
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: pro_dxe.c
//
// Description: This file contains code for Chipset Reference Board Template
//              initialization in the DXE stage
//
//<AMI_FHDR_END>
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol/Variable.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "OemPCIeDev.h"
#include "TseSetupTransfer.h"
#include <Library\OemPCIeDevPlatLib.h>
//#include <PciBus.h>  //<lvych001-20171205 Support Network port efi oprom per-control function.>

#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
#include <Protocol/AmiPostMgr.h> //<Yaosr002-20200712 Display PCIe error info in the Screen.>
#endif

//<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --BEGIN>
#include "OemPCIeDevSetup.h"
#include "OemPCIeDevCodeId.h"
#include "PCIEXPRESS.h"
#include "OemPCIeData.h" 
//<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --END>
#include <FchPlatform.h> //<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module>

//<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module +>
#define DEVFUNC(d, f) ((((UINT8) d) << 3) | ((UINT8) f))

UINT8 DefaultPortDevMapZP [] = {
  DEVFUNC (1, 1),
  DEVFUNC (1, 2),
  DEVFUNC (1, 3),
  DEVFUNC (1, 4),
  DEVFUNC (1, 5),
  DEVFUNC (1, 6),
  DEVFUNC (1, 7),
  DEVFUNC (2, 1),
  DEVFUNC (3, 1),
  DEVFUNC (3, 2),
  DEVFUNC (3, 3),
  DEVFUNC (3, 4),
  DEVFUNC (3, 5),
  DEVFUNC (3, 6),
  DEVFUNC (3, 7),
  DEVFUNC (4, 1)
};
//<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN by PCIe Devices List module ->


//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern  PCI_DEV_CLASS_INFO  PCIDevClassDB[];
extern  PCI_DEV_VID_INFO    PCIDevVidDB[];
extern  PCI_DEV_NINFO       PCIDevDB[];

#define TSTR_SIZE      0x200      //temp string length
#define SETUP_DATA_ID  0xFFF1     // setup data Var Store Id
#define OEMPCIEVAR   L"OEMPCIE_DEV"
#define EFI_BDS_EVENT_BEFORE_SETUP_GUID  { 0xdbc9fd21, 0xfad8, 0x45b0, { 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93 } }

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------
PCI_DEV_SETUP_FORM_DATA       *gPciDevListData = NULL;
Tse_Setup_transfer            *gTseSetuptransferInterface = NULL;
OEM_PCIE_DEV_DATA_PROTOCOL  gOemPcieDevProtocol;

//<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue +>
OEM_PCIE_DEV_DATA_PROTOCOL    *gpOemPcieDevProtocol = NULL;
EFI_HII_HANDLE                gOemPcieDevHiiHandle = NULL;
static UINT8                  sMapNum = 0;//Spiral
PCIE_POLICY_DATA              *gPciePolicyData;
PCI_DEV_NINFO                 gPciDevInfo={0};
CHAR16                        gPciDevName[TSTR_SIZE];
//<Lvshh001-20200721 Define Independent variable and for Dynamic PCIE Dev to solve oob default and current issue ->

EFI_GUID OemPcieDevFormsetGuid =  {0x97cdc6be, 0x7792, 0x4782, 0xba, 0x6d, 0xe6, 0xe3, 0xbb, 0x56, 0x66, 0x66};
EFI_STATUS EFIAPI OemPcieDevBrowserCallback(
    CONST EFI_HII_CONFIG_ACCESS_PROTOCOL*, EFI_BROWSER_ACTION,EFI_QUESTION_ID,UINT8,EFI_IFR_TYPE_VALUE*, EFI_BROWSER_ACTION_REQUEST*
    );
EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = { NULL, NULL,OemPcieDevBrowserCallback };

CALLBACK_INFO SetupCallBack[] =
{
    // Last field in every structure will be filled by the Setup
    { &OemPcieDevFormsetGuid, &CallBack, OEMPCIEDEV_FORM_SET_CLASS, 0, 0},
};
EFI_STATUS 
EFIAPI 
OemPcieDevBrowserCallback(
    IN  CONST   EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
    IN  EFI_BROWSER_ACTION                      Action,
    IN  EFI_QUESTION_ID                         KeyValue,
    IN  UINT8                                   Type,
    IN  EFI_IFR_TYPE_VALUE                      *Value,
    OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest
)
{
/*
    DEBUG((EFI_D_ERROR,"PcieSetup: BrowserCallback(Action %x, KeyValue %x, Type %x).\n", Action, KeyValue, Type));
    if (ActionRequest) 
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    if( (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD ||
         Action == EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING || 
         Action == EFI_BROWSER_ACTION_DEFAULT_SAFE)){
          if((KeyValue & PCI_DEV_DYNAMIC_ITEM_BASE_KEY)!= 0) {
                return EFI_SUCCESS;
          } else {
                return EFI_UNSUPPORTED;
          }
        
     }   
        
     if(Action == EFI_BROWSER_ACTION_CHANGING){
            return EFI_SUCCESS;
     }   
     return EFI_UNSUPPORTED;*/
   return EFI_SUCCESS;

}

VOID
AsciiToUnicodeSize (
  OUT CHAR16            *UStr,
  IN UINTN              Size,
  IN CHAR8              *AStr
  );
//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------

//<Yaosr002-20200712 Display PCIe error info in the Screen. +>
#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
#define NULL_LINE L"                                                                                "//lenth=80
extern EFI_RUNTIME_SERVICES *gRT;
UINT8 ErrorPCIeInfoRows = 0;
EFI_STATUS PCIeErrorInfoDisplayPostMessage( CHAR16 *message)
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Foreground = POST_FONT_FG;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  Background = POST_FONT_BG;
  AMI_POST_MANAGER_PROTOCOL      *gPostManager = NULL;
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_GUID                       AmiTseSetupguid = AMITSESETUP_GUID;
  AMITSESETUP                    TSEVar;
  UINTN                          Size;

  Size = sizeof (AMITSESETUP);
  Status = gRT->GetVariable(L"AMITSESetup", \
                              &AmiTseSetupguid, \
                              NULL, \
                              &Size, \
                              &TSEVar );

  if(gPostManager == NULL){
    Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,NULL,&gPostManager);
    if (EFI_ERROR(Status)) {
      gPostManager=NULL;
      return Status;
    }
  }

  if (1 == TSEVar.AMISilentBoot) {
    if (ErrorPCIeInfoRows >= 10) {
      pBS->Stall(1000000);//delay 1000ms
      for (; ErrorPCIeInfoRows > 0; ErrorPCIeInfoRows--)
        gPostManager->DisplayQuietBootMessage(NULL_LINE, 0, (ErrorPCIeInfoRows - 1) * 17, CA_AttributeLeftTop, Foreground, Background);
    }
    Status = gPostManager->DisplayQuietBootMessage(message, 0, (ErrorPCIeInfoRows++) * 17, CA_AttributeLeftTop, Foreground, Background);
  } else {
    Status = gPostManager->DisplayPostMessage(message);
  }

  return Status;
}
#endif
//<Yaosr002-20200712 Display PCIe error info in the Screen. ->

VOID Deleteblock(IN OUT CHAR16 *str){
    CHAR16 *dest = str;
    int i,j=0;
    for(i=0;str[i]!='\0';i++)
    {
        if(str[i]!=' ')
            dest[j++]=str[i];
    }
    dest[j]='\0';
    str = dest;
}

VOID
TrimWSpace (
  IN  CHAR16  *Data,
  IN  UINT16 Size
  )
/*++
Routine Description:
  Trim space from CHAR8 data header and tail.

Arguments:
  Data  -  The data to swap.
  Size  -  Size of data to swap.

Returns:
  None.
--*/
{
  UINT16  Index;
  CHAR16   *TempBuffer = NULL;
  CHAR16   *TempBufferPtr = NULL;
  

  if (Data == NULL)
    return;

  //Remove the SPACE CHARACTER from the string header
  if (Data[0] == ' ') {
    TempBufferPtr = TempBuffer = AllocateZeroPool(Size*sizeof(CHAR16));
    CopyMem(TempBuffer, Data, Size*sizeof(CHAR16));
    Index = 0;
    while ((*TempBuffer == ' ') && (Index<Size)){
      Index++;
      TempBuffer++;
    }
    ZeroMem(Data, Size*sizeof(CHAR16));
    CopyMem(Data, TempBuffer, (Size-Index)*sizeof(CHAR16));

    if (TempBuffer != NULL) {
      FreePool(TempBufferPtr);
      TempBufferPtr = TempBuffer = NULL;
    }
  }

  //Remove the SPACE CHARACHER from the string tail end
  for (Index = Size-1; Index > 0; Index--) {
    if ( (Data[Index] != ' ')&&(Data[Index] != '\0') )
      break;

    if (Data[Index] == ' ')
      Data[Index] = '\0';
  }

  return ;
}

PCI_DEV_NINFO *GetDevStdName(PCI_DEV_INFO *Dev,UINT16 VenId, UINT16 DevId, UINT16 SubSID ,UINT8 *dFound)
{
  EFI_STATUS                Status;
  ACTIVE_NAMESPACE_DATA             *ActiveNameSpace;
  UINT8       Data;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  IHANDLE                              *Handle;
  LIST_ENTRY                           *Link;
  LIST_ENTRY                           *ProtLink;
  OPEN_PROTOCOL_DATA                   *OpenData;
  PROTOCOL_INTERFACE                   *Prot;
  UINTN                                ChildHandleCount;
  EFI_HANDLE                           ChildHandle;
  
  
  if(dFound == NULL) dFound =&Data;

  gPciDevInfo.DeviceId = DevId;
  gPciDevInfo.VendorId = VenId;
  gPciDevInfo.SubsystemID = SubSID;
  gPciDevInfo.Speed = Speed_Undefined;
  gPciDevInfo.BrandName = gPciDevName;
  ZeroMem(gPciDevName, TSTR_SIZE*sizeof(CHAR16));
  *dFound = 0;
  
  if(NULL == Dev)
    return NULL;
  
  Handle = Dev->Handle;
  // Check for MASS Storage controller, Non-Volatile and NVMHCI interface
  if((Dev->Class.BaseClassCode == 0x01)
      &&(Dev->Class.SubClassCode == 0x08)
      &&(Dev->Class.ProgInterface == 0x02))
  {
    for (Link = Handle->Protocols.ForwardLink, ChildHandleCount = 0; Link != &Handle->Protocols; Link = Link->ForwardLink) {
      Prot = CR(Link, PROTOCOL_INTERFACE, Link, PROTOCOL_INTERFACE_SIGNATURE);
      for (ProtLink = Prot->OpenList.ForwardLink;
          ProtLink != &Prot->OpenList;
          ProtLink = ProtLink->ForwardLink) {
        OpenData = CR (ProtLink, OPEN_PROTOCOL_DATA, Link, OPEN_PROTOCOL_DATA_SIGNATURE);
        if ((OpenData->Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
          ChildHandle = OpenData->ControllerHandle;
          
          Status = pBS->HandleProtocol(ChildHandle, &gEfiDiskInfoProtocolGuid, &DiskInfo);
          if(!EFI_ERROR(Status)){
            ActiveNameSpace = BASE_CR(DiskInfo, ACTIVE_NAMESPACE_DATA, NvmeDiskInfo);
            if(ActiveNameSpace->Signature == ACTIVE_NAME_SPACE_SIG){
              ZeroMem(gPciDevName, TSTR_SIZE*sizeof(CHAR16));
              AsciiToUnicodeSize(gPciDevInfo.BrandName,40,(CHAR8 *)&(ActiveNameSpace->NvmeController->IdentifyControllerData->ModelNumber[0]));
              TrimWSpace (gPciDevInfo.BrandName, 40);
              *dFound = 1;
              break;
            }
          }
          ChildHandleCount++;
        }
      }
      if(*dFound == 1)
        break;
    }
  }
  
  return &gPciDevInfo;

}

PCI_DEV_NINFO* AutoGenDevInfo(
    UINT16 VenId, 
    UINT16 DevId, 
    UINT16 SubSID,
    UINT8  BaseClass,
    UINT8  SubClass,
    UINT8  ProgInterface,
    UINT8  *dFound
)
{
  PCI_DEV_VID_INFO    *PCIDevVidEntry;
  PCI_DEV_CLASS_INFO  *PCIDevClassEntry;
  UINT8               FoundvName=0;
  UINT8               Data;
  
  if(dFound == NULL) dFound =&Data;
  
  PCIDevVidEntry = PCIDevVidDB;
  PCIDevClassEntry = PCIDevClassDB;
  gPciDevInfo.DeviceId = DevId;
  gPciDevInfo.VendorId = VenId;
  gPciDevInfo.SubsystemID = SubSID;
  gPciDevInfo.Speed = Speed_Undefined;
  gPciDevInfo.BrandName = gPciDevName;
  *dFound = 0;
  do {
    
    if( (PCIDevVidEntry->VendorId == VenId)){
      FoundvName = 1;
      break;
    }
    
    if(PCIDevVidEntry->VendorId == 0xFFFF){
              FoundvName = 0;
              break;
    }
    PCIDevVidEntry++;
  }while(TRUE);
  
  if(FoundvName){
    do {
      
      if((PCIDevClassEntry->BaseClass == BaseClass)&&(PCIDevClassEntry->SubClass == SubClass)){
        if((PCIDevClassEntry->ProgInterface == ProgInterface)||(PCIDevClassEntry->ProgInterface == 0xFF)){
            *dFound = 1;
            break;
        }
      }
      
      if((PCIDevClassEntry->BaseClass == 0xFF)&&(PCIDevClassEntry->SubClass == 0xFF)){
        *dFound = 0;
        break;
      }
      PCIDevClassEntry++;
      
    }while(TRUE);
  }
  
  if(*dFound)
  {
    Swprintf(gPciDevName, L"%s %s",PCIDevVidEntry->VName,PCIDevClassEntry->DeviceName);
  }else{
    Swprintf(gPciDevName, L"New Device");
  }

  return &gPciDevInfo;
  
}

PCI_DEV_NINFO *GetDevBaseInfo(UINT16 VenId, UINT16 DevId, UINT16 SubSID ,UINT8 *dFound)
{
  PCI_DEV_NINFO  *PCIDevEntry;
    UINT8       Data;
    
    if(dFound == NULL) dFound =&Data;
    
    PCIDevEntry = &PCIDevDB[0];   
    do {
      if( (PCIDevEntry->VendorId == VenId) && (PCIDevEntry->DeviceId == DevId) ){
        if((PCIDevEntry->SubsystemID == SubSID)||(PCIDevEntry->SubsystemID == 0xFFFF)){
            *dFound = 1;
            break;
        }
      }
      if((PCIDevEntry->VendorId == 0xFFFF) && (PCIDevEntry->DeviceId == 0xFFFF)){
          *dFound = 0;
          break;
      }
        PCIDevEntry++;
    } while(TRUE);

    return PCIDevEntry;
}

//<Chendl001-20190903 Pcie device list option can show device link Status +>
// Get Pcie device attribute
// ReturnAttribute[0] = Max Speed
// ReturnAttribute[1] = Max Width
// ReturnAttribute[2] = Current Speed
// ReturnAttribute[3] = Current Speed
// Return : Attribute register
PCI_ATTRIBUTE_PARAM* GetPciAttribute(PCI_DEV_INFO *dev)
{
    UINT8                         CapIdOffset;
    UINT16                        Data16 = 0; 
    UINT16                        ReturnAttribute[4];
    PCI_ATTRIBUTE_PARAM          *tDevAttribute=NULL;
    //<Yaosr002-20200712 Display PCIe error info in the Screen. +>
#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
    CHAR16                         TmpStr[0x40];
#endif
    //<Yaosr002-20200712 Display PCIe error info in the Screen. ->

    tDevAttribute = AllocateZeroPool(sizeof(PCI_ATTRIBUTE_PARAM));
    if ((dev == NULL) || (tDevAttribute == NULL)){
      return NULL;
    }

    CapIdOffset = PcieFindCapId(&dev->PciIo, 0x10);
    if(CapIdOffset != 0){
      dev->RbIo->Pci.Read(dev->RbIo,EfiPciIoWidthUint16,(dev->Address.ADDR + CapIdOffset + PCIE_LINK_CAPABILITY_OFFSET),1,&Data16);
      ReturnAttribute[0] = Data16 & 0x0f; // Max speed
      ReturnAttribute[1] = (Data16 >> 4) & 0x3f;// Max width

      Data16 = 0;
      dev->RbIo->Pci.Read(dev->RbIo,EfiPciIoWidthUint16,(dev->Address.ADDR + CapIdOffset + PCIE_LINK_STATUS_OFFSET),1,&Data16);
      ReturnAttribute[2] = Data16 & 0x0f; // Current speed
      ReturnAttribute[3] = (Data16 >> 4) & 0x3f;// Current width

      TRACE((-1, "ReturnAttribute[0]=%02x, ReturnAttribute[1]=%02x,ReturnAttribute[2]=%02x,ReturnAttribute[3]=%02x\n", ReturnAttribute[0],ReturnAttribute[1],ReturnAttribute[2],ReturnAttribute[3]));

      //<Yaosr002-20200712 Display PCIe error info in the Screen. +>
#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
      if (DevItem->PciLnkSpeed == 0) { //<Yaosr-20201201 Set PCIe link speed by device.>
#endif
        if (ReturnAttribute[0] > ReturnAttribute[2]) {
          Swprintf(TmpStr, L"B%02x:D%02x:F%02x: is speed reduction", dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function);
          PCIeErrorInfoDisplayPostMessage(TmpStr);
        }
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
      }
#endif
      if (ReturnAttribute[1] > ReturnAttribute[3]) {
        Swprintf(TmpStr, L"B%02x:D%02x:F%02x: is width reduction", dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function);
        PCIeErrorInfoDisplayPostMessage(TmpStr);
      }
#endif
      //<Yaosr002-20200712 Display PCIe error info in the Screen. ->
      // Max Speed
      if(ReturnAttribute[0] == 0x01){
        Swprintf(tDevAttribute->MaxSpeed, L"2.5 GT/s");
      } else if(ReturnAttribute[0] == 0x02){
        Swprintf(tDevAttribute->MaxSpeed, L"5.0 GT/s");
      } else if(ReturnAttribute[0] == 0x03){
        Swprintf(tDevAttribute->MaxSpeed, L"8.0 GT/s");
      } else {
        Swprintf(tDevAttribute->MaxSpeed, L"16.0 GT/s");
      }

      TRACE((-1, "Max Speed = %S\n", tDevAttribute->MaxSpeed));

      // Max Width
      if(ReturnAttribute[1] == 0x01){
        Swprintf(tDevAttribute->MaxWidth, L"x1");
      } else if(ReturnAttribute[1] == 0x02){
        Swprintf(tDevAttribute->MaxWidth, L"x2");
      } else if(ReturnAttribute[1] == 0x04){
        Swprintf(tDevAttribute->MaxWidth, L"x4");
      } else if(ReturnAttribute[1] == 0x08){
        Swprintf(tDevAttribute->MaxWidth, L"x8");
      } else if(ReturnAttribute[1] == 0x0c){
        Swprintf(tDevAttribute->MaxWidth, L"x12");
      } else if(ReturnAttribute[1] == 0x10){
        Swprintf(tDevAttribute->MaxWidth, L"x16");
      } else if(ReturnAttribute[1] == 0x20){
        Swprintf(tDevAttribute->MaxWidth, L"x32");
      } 

      TRACE((-1, "Max Width = %S\n", tDevAttribute->MaxWidth));

      // Current Speed
      if(ReturnAttribute[2] == 0x01){
        Swprintf(tDevAttribute->CurrentSpeed, L"2.5 GT/s");
      } else if(ReturnAttribute[2] == 0x02){
        Swprintf(tDevAttribute->CurrentSpeed, L"5.0 GT/s");
      } else if(ReturnAttribute[2] == 0x03){
        Swprintf(tDevAttribute->CurrentSpeed, L"8.0 GT/s");
      } else {
        Swprintf(tDevAttribute->CurrentSpeed, L"16.0 GT/s");
      }

      TRACE((-1, "Current Speed = %S\n", tDevAttribute->CurrentSpeed));

      // Current Width
      if(ReturnAttribute[3] == 0x01){
        Swprintf(tDevAttribute->CurrentWidth, L"x1");
      } else if(ReturnAttribute[3] == 0x02){
        Swprintf(tDevAttribute->CurrentWidth, L"x2");
      } else if(ReturnAttribute[3] == 0x04){
        Swprintf(tDevAttribute->CurrentWidth, L"x4");
      } else if(ReturnAttribute[3] == 0x08){
        Swprintf(tDevAttribute->CurrentWidth, L"x8");
      } else if(ReturnAttribute[3] == 0x0c){
        Swprintf(tDevAttribute->CurrentWidth, L"x12");
      } else if(ReturnAttribute[3] == 0x10){
        Swprintf(tDevAttribute->CurrentWidth, L"x16");
      } else if(ReturnAttribute[3] == 0x20){
        Swprintf(tDevAttribute->CurrentWidth, L"x32");
      } 

      TRACE((-1, "Current Width = %S\n", tDevAttribute->CurrentWidth));

      return tDevAttribute;
    }
    
    return NULL;
}
//<Chendl001-20190903 Pcie device list option can show device link Status ->

UINT8 GetPciSlotNum(PCI_DEV_INFO    *RootPort)
{
    PCI_DEV_INFO                  *Prt;
    UINT8                         SlotNum;    
    UINT32                        Data32;
    UINT8                         CapIdOffset;
    PCIE_SLOT_CAP                 SlotData;
    
    SlotNum = 0xFF;
    Prt = RootPort;
    if( Prt  !=  NULL )
    {
      CapIdOffset = PcieFindCapId(&Prt->PciIo,0x10);
      
      if(CapIdOffset !=0){

        Prt->RbIo->Pci.Read (Prt->RbIo,
                    EfiPciIoWidthUint32,
                    (Prt->Address.ADDR + CapIdOffset + PCIE_SLOT_CAPABILITY_OFFSET),
                    1,
                    &SlotData.Dword
                    );
        Data32 = SlotData.Bits.PhysicalSlotNum;
        if(Data32) SlotNum = (UINT8)Data32;
      }

    } 
    TRACE((-1,"RootPort is B%02x|D%02x|F%02x SlotNum:0x%2X\n",Prt->Address.Addr.Bus,Prt->Address.Addr.Device,Prt->Address.Addr.Function,SlotNum));
    
    return SlotNum;
}

VOID DevRootPortDisable(PCI_DEV_IDX_DATA *IdxDev){
  
  PCI_DEV_INFO        *Prt;
  UINT16              Data16 = 0;
  UINT8               CapIdOffset;
  UINT8               DevNum, FuncNum, DevFunc, Index; //<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module>
    
  
  if( NULL == IdxDev ) return;
  //Link disable
  
  if(IdxDev->PortData[0].PciDevEN == 0){
    Prt = (PCI_DEV_INFO *)IdxDev->PortData[0].PciRBInfo;
    //<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module +>
    DevNum = Prt->Address.Addr.Device;
    FuncNum = Prt->Address.Addr.Function;
    DevFunc = (DevNum << 3) | FuncNum;
    for (Index = 0; Index < sizeof (DefaultPortDevMapZP); ++Index) {
      if (DefaultPortDevMapZP[Index] == DevFunc) {
        break;
      }
    }
    if (Index == sizeof (DefaultPortDevMapZP))
      return;
    //<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module ->
    
    CapIdOffset = PcieFindCapId(&Prt->PciIo,0x10);
    
    if(CapIdOffset !=0){

      Prt->RbIo->Pci.Read (Prt->RbIo,
                  EfiPciIoWidthUint16,
                  (Prt->Address.ADDR + CapIdOffset + PCIE_LINK_CONTROL_OFFSET),
                  1,
                  &Data16
                  );
      Data16 |= BIT4;
      Prt->RbIo->Pci.Write (Prt->RbIo,
            EfiPciIoWidthUint16,
            (Prt->Address.ADDR + CapIdOffset + PCIE_LINK_CONTROL_OFFSET),
            1,
            &Data16
            );
    }
    FchSmnRW (Prt->Address.Addr.Bus, (0x13B31004 + ((UINT32)Index << 10)), ~(BIT0 | BIT2 | BIT18), (BIT0 | BIT2 | BIT18), NULL); //<hujf1001-20190527 Fix system hang when boot into Windows after disable LAN under PCIe Devices List module +>
    TRACE((-1,"RootPort disable is B%02x|D%02x|F%02x Data:0x%2X\n",Prt->Address.Addr.Bus,Prt->Address.Addr.Device,Prt->Address.Addr.Function,Data16));
  } 
}

VOID PciDevReadyToBootProcess(IN EFI_EVENT Event, IN VOID *Context)
{
  
  PCI_DEV_SETUP_FORM_DATA   *PciDevFormData;
  PCI_DEV_IDX_DATA          *IdxDev;
  UINTN                     i;

  TRACE ((-1, "PciDevReadyToBootProcess.\n"));
  if (Event) {
        pBS->CloseEvent(Event);
  }

  PciDevFormData = (PCI_DEV_SETUP_FORM_DATA*)Context;
  
  for(i=0;i<PciDevFormData->PciDevCount;i++){
    IdxDev = PciDevFormData->PciDevIdxData[i];
    DevRootPortDisable(IdxDev);
  }
  
  return;

}

//-------------------------------------------------------------------------
// CheckDevOpExtend:check dev option space can be extend
//-------------------------------------------------------------------------
BOOLEAN CheckDevOpExtend(PCI_DEV_SETUP_FORM_DATA  *PciDevFormData,PCI_DEV_IDX_DATA  *item)
{
  UINTN             Offset,i;
  PCI_DEV_IDX_DATA  *IdxDev;
    
  if(PciDevFormData->PciDevCount == 0) return TRUE;
  
  for (i=0; i<PciDevFormData->PciDevCount; i++) {
    IdxDev = PciDevFormData->PciDevIdxData[i];
    if(IdxDev->PciDevIdx == item->PciDevIdx){
      if(i < PciDevFormData->PciDevCount){
        if(i == PciDevFormData->PciDevCount - 1){
          Offset = (MAX_DEV_COUNT -1) - item->PciDevIdx;
          if(Offset > MAX_EXTERN_PORT_COUNTS) Offset = MAX_EXTERN_PORT_COUNTS;
        }else{
          IdxDev = PciDevFormData->PciDevIdxData[i+1];
          Offset = IdxDev->PciDevIdx - item->PciDevIdx;
        }
        Offset *= MAX_DEV_PORT;
        if(Offset < item->PortCount)
          return FALSE;
        else
          return TRUE;
      }else{
        return TRUE;
      }
    }
  }
  return TRUE;
  
}

//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
EFI_STATUS CheckSriovSupport(  EFI_PCI_IO_PROTOCOL *PciIo, PCI_DEV_SETUP_DATA *DevItem)
{
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    ExtendedRegister=0x100;
  PCIE_EXT_CAP_HDR          extcap;
  
  while(ExtendedRegister){

  Status = PciIo->Pci.Read(
          PciIo,
          EfiPciIoWidthUint32,
          ExtendedRegister,
          1,
          &extcap.EXT_CAP_HDR
          );
  
  if(EFI_ERROR(Status)||extcap.ExtCapId==0xFFFF) break;
  
  if (extcap.ExtCapId==0x10){//SRIOV
    DevItem->SriovCap = TRUE;
    break;
  }
  ExtendedRegister = extcap.NextItemPtr;
      }
  return Status;
}
#endif
//<bieyb-20180727 add function enable SRIOV by port->

//<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
VOID SetDeviceLinkPortSpeed(PCI_DEV_SETUP_DATA        *DevItem)
{
  PCI_DEV_INFO        *ParentPort;
  UINT8               CapIdOffset;
  UINT16              Data16 = 0;
  
  if(DevItem == NULL)
    return;

  ParentPort=((PCI_DEV_INFO *)DevItem->PciDevInfo)->ParentBrg;

  CapIdOffset = PcieFindCapId(&ParentPort->PciIo,0x10);


  if(CapIdOffset ==0) 
    return;

  ParentPort->PciIo.Pci.Read (&ParentPort->PciIo,EfiPciIoWidthUint16,\
                              (CapIdOffset + 0x30),//Link Control 2
                              1,
                              &Data16);

  switch(DevItem->PciLnkSpeed){
    case 1:
      Data16 = Data16 & 0xFFF0 | 1;
      break;
    case 2:
      Data16 = Data16 & 0xFFF0 | 2;
      break;
    case 3:
      Data16 = Data16 & 0xFFF0 | 3;
      break;
  }

  ParentPort->PciIo.Pci.Write (&ParentPort->PciIo,EfiPciIoWidthUint16,\
                                (CapIdOffset + 0x30),
                                1,
                                &Data16);
}
#endif
//<Yaosr-20201201 Set PCIe link speed by device. ->
//*************************************************************************
EFI_STATUS InitPCIeDevSetting(EFI_HII_HANDLE HiiHandle,PCI_DEV_SETUP_FORM_DATA  *PciDevFormData)
{
  EFI_STATUS                Status;
  EFI_GUID                  gEfiSetupGuid = OEMPCIEDEV_VARSTORE_GUID;
  OEMPCIE_SETUP_DATA      SetupData;
  UINTN                     SetupDataSize;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  PCI_DEV_NINFO             *ptrdninfo;
  PCI_DEV_SETUP_DATA        *DevItem;
  PCI_DEV_IDX_DATA          *IdxDev;
  PCI_DEV_INFO              *dev;
  EFI_STRING                tString=NULL;
  UINTN                     i,j;
  UINTN                     DevIndex,PortVarIndex;
  UINT16                    VenId,DevId,SubSID;
  UINT16                    Bus,Device,Function;
  BOOLEAN                   OpExtend;
  UINT8                     dfound;
  UINT8                     PortIndex;
  PCI_ATTRIBUTE_PARAM       *DevAttribute; //<Chendl001-20190903 Pcie device list option can show device link Status>
  EFI_STRING                tID = NULL;
  
  tString = MallocZ(TSTR_SIZE);
  tID = MallocZ(TSTR_SIZE);
  SetupDataSize = sizeof(OEMPCIE_SETUP_DATA);
  Status = pRS->GetVariable ( OEMPCIEVAR, &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  ASSERT_EFI_ERROR (Status);

  //<Yaosr002-20200712 Display PCIe error info in the Screen. +>
#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
  ErrorPCIeInfoRows = PcdGet8(PcdShowInfoCount);
#endif
  //<Yaosr002-20200712 Display PCIe error info in the Screen. ->

  TRACE ((-1, __FUNCTION__" Start\n"));
  for(i=0;i<PciDevFormData->PciDevCount;i++){
    
    IdxDev = PciDevFormData->PciDevIdxData[i];
    OpExtend = CheckDevOpExtend(PciDevFormData,IdxDev);
    for(j=0;j<IdxDev->PortCount;j++){
      
      DevItem = &IdxDev->PortData[j];
      dev = (PCI_DEV_INFO *)DevItem->PciDevInfo;
      DevIndex = DevItem->PciDevIdx;
      PortIndex = DevItem->PortIdx;
      PortVarIndex = DevIndex*MAX_DEV_PORT + PortIndex;
      
      if(OpExtend)
      {
        DevItem->OpExtend = 1;
      }else if(PortIndex >= MAX_DEV_PORT)
      {
        PortVarIndex = DevIndex*MAX_DEV_PORT + MAX_DEV_PORT - 1;
      }
      
      DevItem->PciSetupOpCodeHandle = HiiAllocateOpCodeHandle ();
      DevItem->PciOpRomOpCodeHandle = HiiAllocateOpCodeHandle ();
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      DevItem->PcieSriovOpCodeHandle = HiiAllocateOpCodeHandle ();
#endif
//<bieyb-20180727 add function enable SRIOV by port->
      //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
      DevItem->PcieLnkSpeedOpCodeHandle = HiiAllocateOpCodeHandle ();
      DevItem->PciLnkSpeed = SetupData.PciLnkSpeed[DevIndex];
#endif
      //<Yaosr-20201201 Set PCIe link speed by device. ->
      DevItem->PciDevEN = SetupData.PciDevEN[DevIndex];
      DevItem->OpROMEN = SetupData.OpROMEN[PortVarIndex];
      DevItem->ShowItem = TRUE;
      DevItem->VarStoreId = OEMPCIE_DATA_ID;
      DevItem->DevEnableQid = (UINT16)(PCI_DEV_DYNAMIC_ITEM_BASE_KEY+i);
      DevItem->SlotNum = GetPciSlotNum((PCI_DEV_INFO *)DevItem->PciRBInfo);
      
      PciIo = &dev->PciIo;
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      Status = CheckSriovSupport(PciIo,DevItem);
#endif
//<bieyb-20180727 add function enable SRIOV by port->
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
      // SetDevRootPortSpeed(DevItem);//<Yaosr-20201201 Set PCIe link speed by device.>
      SetDeviceLinkPortSpeed(DevItem);
#endif

      Status = PciIo->Pci.Read(
              PciIo,
              EfiPciIoWidthUint8,
              0x2e,
              2,
              &SubSID
              );
      ASSERT_EFI_ERROR (Status); 
      
      VenId = dev->DevVenId.VenId;
      DevId = dev->DevVenId.DevId;
      Bus = dev->Address.Addr.Bus;
      Device = dev->Address.Addr.Device;
      Function = dev->Address.Addr.Function;
      ptrdninfo = GetDevStdName(dev, VenId, DevId, SubSID,&dfound);
      if(!dfound) ptrdninfo = GetDevBaseInfo(VenId, DevId, SubSID, &dfound);
      if(!dfound)ptrdninfo = AutoGenDevInfo(VenId, DevId, SubSID,dev->Class.BaseClassCode,dev->Class.SubClassCode,dev->Class.ProgInterface,&dfound);
      
      TRACE ((-1, "GetBrandName:vid:%X,did:%X,sid:%X--%S\n", VenId,DevId,SubSID,ptrdninfo->BrandName));
      
      StrCpyS(DevItem->BrandName,64,ptrdninfo->BrandName);
      DevItem->Speed = ptrdninfo->Speed;
      if(dfound == 0){
            Swprintf(tString, L"%s V:%04X D:%04X S:%04X B:%02X D:%02X F:%02X",DevItem->BrandName,VenId,DevId,SubSID,Bus,Device,Function);
      }else{
            Swprintf(tString, L"%s B:%02X D:%02X F:%02X",DevItem->BrandName,Bus,Device,Function);
      }
      DevItem->DevStringId=HiiSetString(HiiHandle, 0,tString, NULL );
      TRACE ((-1, "[SK] tString--%S\n", tString)); 
      Deleteblock(tString);     
      //Spiral >
      Swprintf(tID, L"OEM4%01X%01X",i,j);
      TRACE ((-1, "[SK] tID--%S\n", tID));
      TRACE ((-1, "[SK] tString--%S\n", tString));
      HiiSetString (HiiHandle, DevItem->DevStringId, tString, "x-UEFI-AMI");
      //HiiSetString (HiiHandle, DevItem->DevStringId, tID, "x-UEFI-AMI");
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
      Status = gpOemPcieDevProtocol->UpdateDevicePortCfg(DevItem);
      if(!EFI_ERROR(Status)) {
        if(DevItem->PciDevEN!=SetupData.PciDevEN[DevIndex]) {
          SetupData.PciDevEN[DevIndex] = DevItem->PciDevEN;
          pRS->SetVariable ( OEMPCIEVAR, &gEfiSetupGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE, SetupDataSize, &SetupData);
        }
      }
#else
    gpOemPcieDevProtocol->UpdateDevicePortCfg(DevItem);
#endif
//<licm-20210531 Optimization PCIE Device List module ->
      //Last Update
      //<Chendl001-20190903 Pcie device list option can show device link Status +>
      DevAttribute = GetPciAttribute(dev);
      if (DevAttribute != NULL){
        //<Fancf-20200810 No longer display pcie device slot num +>
        //if(DevItem->SlotNum !=0xFF)
//<huangjin002-20200103 Add VID and DID into help message of pcie device list +>  
//        Swprintf(tString, L"Enable/Disable PCIe Device\nLocation:\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\n%s\nMax Speed:%s\nMax Width:%s\nCurrent Speed:%s\nCurrent Width:%s\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,DevItem->Prefix,DevAttribute->MaxSpeed,DevAttribute->MaxWidth,DevAttribute->CurrentSpeed,DevAttribute->CurrentWidth);
          //Swprintf(tString, L"Enable/Disable PCIe Device\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\n%s\nVendor ID:%02X\nDevice ID:%02X\nMax Speed:%s\nMax Width:%s\nCurrent Speed:%s\nCurrent Width:%s\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,DevItem->Prefix,dev->DevVenId.VenId,dev->DevVenId.DevId,DevAttribute->MaxSpeed,DevAttribute->MaxWidth,DevAttribute->CurrentSpeed,DevAttribute->CurrentWidth);
        //else
//        Swprintf(tString, L"Enable/Disable PCIe Device\nLocation:\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\nMax Speed:%s\nMax Width:%s\nCurrent Speed:%s\nCurrent Width:%s\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,DevAttribute->MaxSpeed,DevAttribute->MaxWidth,DevAttribute->CurrentSpeed,DevAttribute->CurrentWidth);
          Swprintf(tString, L"Enable/Disable PCIe Device\nSeg/Bus/Dev/Func: %02X/%02X/%02X/%02X\nVendor ID    : %02X\nDevice ID    : %04X\nMaximum Speed: %s\nMaximum Width: %s\nCurrent Speed: %s\nCurrent Width: %s\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,dev->DevVenId.VenId,dev->DevVenId.DevId,DevAttribute->MaxSpeed,DevAttribute->MaxWidth,DevAttribute->CurrentSpeed,DevAttribute->CurrentWidth);
//<huangjin002-20200103 Add VID and DID into help message of pcie device list ->
        pBS->FreePool(DevAttribute);
        //<Fancf-20200810 No longer display pcie device slot num ->
      } else {
        if(DevItem->SlotNum !=0xFF)
          Swprintf(tString, L"Enable/Disable PCIe Device\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\n%s\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function,DevItem->Prefix);
        else
          Swprintf(tString, L"Enable/Disable PCIe Device\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\n",dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function);
      }
      //<Chendl001-20190903 Pcie device list option can show device link Status ->
      DevItem->DevHelpStringId = HiiSetString(HiiHandle, 0,tString, NULL );
    }
  }

#if defined(ShowPCIeErrorInfo) && (ShowPCIeErrorInfo == 1)
  PcdSet8(PcdShowInfoCount, ErrorPCIeInfoRows);//<Yaosr002-20200712 Display PCIe error info in the Screen.>
#endif

  TRACE ((-1, __FUNCTION__" Exit\n"));
  pBS->FreePool(tString);
  return Status;
}

/**
    This will sort having setup devices items in Following order:
    First go slots in Ascending order than on board devices...

    @param IfrData Pointer to the private Pci Setup data structure

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

    @retval EFI_SUCCESS

**/
VOID OemSortDataByDevPortIdx(PCI_DEV_SETUP_FORM_DATA     *PciIfrData)
{
  EFI_STATUS            Status;
  T_ITEM_LIST           slt={0,0,NULL};
  PCI_DEV_IDX_DATA      *cb,*tmpcb;
  UINTN                 i;
  BOOLEAN               ap;
//------------------------------------  
  while(PciIfrData->PciDevCount){
    cb=PciIfrData->PciDevIdxData[0];
    
    //Remove item from unsorted array;
    DeleteItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, 0, FALSE);
    
      //Sort device to the separate list by dev index
      for(i=0, ap=TRUE; i<slt.ItemCount; i++){
        tmpcb=(PCI_DEV_IDX_DATA*)slt.Items[i];

        if(tmpcb->PciDevIdx > cb->PciDevIdx){
            Status=InsertItemLst(&slt, cb, i);
            ap=FALSE;
            break;
        }
      }//for
      // if we apPEND Flag is set Slot # == the biggest one yet.
      if(ap) Status = AppendItemLst(&slt, cb);

    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return;
  } //keep going until unsorted list empty.
  
  //now fill same list we just emptied PciIfrData->PciDevSetupData[] with sorted data.
  //With Sorted Slot Data....
  for(i=0;i<slt.ItemCount; i++) {
    Status=AppendItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, slt.Items[i]);
    ASSERT_EFI_ERROR(Status);
  }

  ClearItemLst(&slt,FALSE);
}


/**
    This will sort having setup devices items in Following order:
    First go slots in Ascending order than on board devices...

    @param IfrData Pointer to the private Pci Setup data structure

    @retval EFI_SUCCESS if everything good
    @retval EFI_ERROR value if something wrong happends

    @retval EFI_SUCCESS

**/
VOID OemSortData(PCI_DEV_SETUP_FORM_DATA     *PciIfrData)
{
  EFI_STATUS            Status;
  T_ITEM_LIST           slt={0,0,NULL};
  T_ITEM_LIST           obd={0,0,NULL};
  PCI_DEV_IDX_DATA      *cb,*tmpcb;
  UINTN                 i;
  BOOLEAN               ap;
//------------------------------------  
  while(PciIfrData->PciDevCount){
    cb=PciIfrData->PciDevIdxData[0];
    
    //Remove item from unsorted array;
    DeleteItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, 0, FALSE);
    
    if(cb->PortData[0].SlotNum == 0xFF){
      //Add OnBoard device to the separate list, 
      Status = AppendItemLst(&obd, cb);
    } else {
    //----------------- 
      for(i=0, ap=TRUE; i<slt.ItemCount; i++){
        tmpcb=(PCI_DEV_IDX_DATA*)slt.Items[i];
        if(tmpcb->PortData[0].SlotNum > cb->PortData[0].SlotNum){
            Status=InsertItemLst(&slt, cb, i);
            ap=FALSE;
            break;
        }
      }//for
      // if we apPEND Flag is set Slot # == the biggest one yet.
      if(ap) Status = AppendItemLst(&slt, cb);
    }
    ASSERT_EFI_ERROR(Status);
    if(EFI_ERROR(Status)) return;
  } //keep going until unsorted list empty.
  
  //now fill same list we just emptied PciIfrData->PciDevSetupData[] with sorted data.
  //With Sorted Slot Data....
#if (PCI_SETUP_SHOW_SLOTS_FIRST == 1) 
  for(i=0;i<slt.ItemCount; i++) {
    Status=AppendItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, slt.Items[i]);
    ASSERT_EFI_ERROR(Status);
  }
#endif
  //with On Board Devices Data...
  for(i=0;i<obd.ItemCount; i++) {
    Status=AppendItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, obd.Items[i]);
    ASSERT_EFI_ERROR(Status);
  }
  
#if (PCI_SETUP_SHOW_SLOTS_FIRST == 0) 
  for(i=0;i<slt.ItemCount; i++) {
    Status=AppendItemLst((T_ITEM_LIST*)&PciIfrData->PciDevInitCnt, slt.Items[i]);
    ASSERT_EFI_ERROR(Status);
  }
#endif  
  ClearItemLst(&obd,FALSE);
  ClearItemLst(&slt,FALSE);
}

EFI_STATUS InitPciDevData(EFI_HII_HANDLE HiiHandle,PCI_DEV_SETUP_FORM_DATA  *PciDevFormData)
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_IFR_GUID_LABEL  *StartLabel;
  EFI_IFR_GUID_LABEL  *EndLabel;

  // Create new OpCode Handle
  PciDevFormData->StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  PciDevFormData->EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  
  // Create Hii Extend Label OpCode as the start opcode 
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (PciDevFormData->StartOpCodeHandle, 
          &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number = PCI_DEV_DYNAMIC_ITEM_START;
  PciDevFormData->StartLabel = StartLabel;
  
    // Create Hii Extend Label OpCode as the end opcode
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (PciDevFormData->EndOpCodeHandle, 
          &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number = PCI_DEV_DYNAMIC_ITEM_END;  
  PciDevFormData->EndLabel = EndLabel;
  
  OemSortDataByDevPortIdx(PciDevFormData);
  InitPCIeDevSetting(HiiHandle,PciDevFormData);
  OemSortData(PciDevFormData);
  
  return Status;

}

//-------------------------------------------------------------------------
// Library Functions Implementation
//-------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////
UINT8 *
EFIAPI
AmiHiiCreateSuppresGrayIdVal(
  IN VOID               *OpCodeHandle,
  IN EFI_QUESTION_ID    QuestionId,
  UINT16                Value,
  BOOLEAN               Suppress    //if TRUE Suppress; False Gray out.
  )
{
    EFI_IFR_EQ_ID_VAL   *Condition;
    EFI_IFR_OP_HEADER   *Header;
    UINT8               *Buffer=NULL;
    UINTN               sz;
//-------------------------------------
    //Allocate buffer for SUPPRESS_IF/GRAY_OUT_IF opcode + EFI_IFR_EQ_ID_VAL for suppress;
    sz=sizeof(EFI_IFR_OP_HEADER)+sizeof(EFI_IFR_EQ_ID_VAL);
    Buffer=AllocateZeroPool(sz);
    if(Buffer==NULL) {
        ASSERT_EFI_ERROR(EFI_OUT_OF_RESOURCES);
        return Buffer;
    }
    
    //Init Pointers;
    Header=(EFI_IFR_OP_HEADER*)Buffer;
    Condition=(EFI_IFR_EQ_ID_VAL*)(Header+1);
    
    //Update OpCodes...
    //EFI_IFR_SUPPRESS_IF_OP/EFI_IFR_GRAY_OUT_IF_OP first;
    if(Suppress) Header->OpCode=EFI_IFR_SUPPRESS_IF_OP;
    else Header->OpCode=EFI_IFR_GRAY_OUT_IF_OP;
    Header->Length=sizeof(EFI_IFR_OP_HEADER);
    Header->Scope=1;
    
    //then goes conditions.
    //First update OpCode Header
    Condition->Header.OpCode=EFI_IFR_EQ_ID_VAL_OP;
    Condition->Header.Scope=0;
    Condition->Header.Length=sizeof(EFI_IFR_EQ_ID_VAL);
    //Then goes Opcode Data..
    Condition->QuestionId=QuestionId;
    Condition->Value=Value;
    
    //Here we go...
    return HiiCreateRawOpCodes(OpCodeHandle,Buffer,sz);
}


//*************************************************************************
UINT8 *
EFIAPI
AmiHiiTerminateScope(IN VOID *OpCodeHandle)
{
    EFI_IFR_END       *IfrEnd;
  UINT8             *Buffer;
  UINTN             sz;
//----------------------------------------- 
  sz=sizeof(EFI_IFR_END);
  Buffer=AllocateZeroPool(sz);
  IfrEnd=(EFI_IFR_END*)Buffer;

  //Update Header...
  IfrEnd->Header.OpCode = EFI_IFR_END_OP;
  IfrEnd->Header.Scope  = 0;
  IfrEnd->Header.Length = sizeof (EFI_IFR_END);

  //Submit it...    
  return HiiCreateRawOpCodes(OpCodeHandle,Buffer, sz);
}


//*************************************************************************
VOID CreatePciSetupOptions(VOID *OpcodeHandle,UINT8 DefaultValue){
    static EFI_STRING_ID    opstr[]={
            STRING_TOKEN(STR_DISABLE),STRING_TOKEN(STR_ENABLE), 
        //STRING_TOKEN(STR_AUTO)
    };
  UINTN         i;
  UINT8         val;
  UINT8     flags;
  
    for (i=0, val=0; i<(sizeof(opstr)/sizeof(EFI_STRING_ID)); i++, val++){
        
    if(i==DefaultValue) flags=(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    else flags = 0;

    HiiCreateOneOfOptionOpCode (
            OpcodeHandle,
            opstr[i],
            flags,
            EFI_IFR_NUMERIC_SIZE_1,
            val );
    }
}

//*************************************************************************
VOID CreatePciSetupItems(
        IN EFI_HII_HANDLE           HiiHandle,
        IN PCI_DEV_SETUP_FORM_DATA  *PciDevFormData,
        IN PCI_DEV_SETUP_DATA       *item
)
{
    PCI_DEV_SETUP_DATA  *Devitem;
  VOID                *StartOpCodeHandle;
  VOID                *OptionsOpCodeHandle; 
  EFI_QUESTION_ID     QuestionId;
  EFI_VARSTORE_ID     VarStoreId;
  EFI_STRING_ID       VarPromptStringId;
  EFI_STRING_ID       VarHelpStringId;
  UINTN                       index;
  UINT16              VarOffset;

    Devitem = item;
    
    if(NULL == PciDevFormData || NULL == Devitem) 
        return;
    
    //1. Create Pci Setup OneOf items
    StartOpCodeHandle = PciDevFormData->StartOpCodeHandle;
    OptionsOpCodeHandle = Devitem->PciSetupOpCodeHandle;
    QuestionId = Devitem->DevEnableQid;
    VarStoreId = Devitem->VarStoreId;
    index = Devitem->PciDevIdx;
    VarOffset = (UINT16)EFI_FIELD_OFFSET(OEMPCIE_SETUP_DATA, PciDevEN[index]);
    VarPromptStringId = Devitem->DevStringId;
    VarHelpStringId = Devitem->DevHelpStringId;

    CreatePciSetupOptions(OptionsOpCodeHandle,Devitem->DevDefValue);

    //Now create OneOf OpCode...
    HiiCreateOneOfOpCode (
        StartOpCodeHandle,                  //*OpCodeHandle
        QuestionId,                         //QuestionId
        VarStoreId,                         //VarStore ID
        VarOffset,                          //Offset
        VarPromptStringId,                  //Prompt
        VarHelpStringId,                    //Help 
        EFI_IFR_FLAG_RESET_REQUIRED,        //QuestionFlags
        EFI_IFR_NUMERIC_SIZE_1,             //OneOfFlags
        OptionsOpCodeHandle,                //OptionsOpCodeHandle
        NULL                                //defopt //*DefaultsOpCodeHandle  OPTIONAL
    );
}

//*************************************************************************
VOID CreatePciOpRomOptions(VOID *OpcodeHandle,UINT8 DefaultValue){
    static OPTION_ITEM_VALUE    opstr[]={
            {STRING_TOKEN(STR_OPROM_POLICY_DISABLE),0},
            {STRING_TOKEN(STR_OPROM_POLICY_UEFI_ONLY),1},
            {STRING_TOKEN(STR_OPROM_POLICY_LEGACY_ONLY),2},
            {STRING_TOKEN(STR_OPROM_POLICY_AUTO),4},
    };
  UINTN         i;
  UINT8         val;
  UINT8     flags;
  
    for (i=0; i<(sizeof(opstr)/sizeof(OPTION_ITEM_VALUE)); i++){
      
    
    val=opstr[i].Value;
    if(val==DefaultValue) flags=(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    else flags = 0;
            
    HiiCreateOneOfOptionOpCode (
            OpcodeHandle,
            opstr[i].StrId,
            flags,
            EFI_IFR_NUMERIC_SIZE_1,
            val );
    
    }
}

//*************************************************************************
VOID CreatePciOpRomOptionsEX(VOID *OpcodeHandle,UINT8 DefaultValue){
  static OPTION_ITEM_VALUE    opstr[]={
          {STRING_TOKEN(STR_OPROM_POLICY_DISABLE),0},
          {STRING_TOKEN(STR_OPROM_POLICY_UEFI_ONLY),1},
          {STRING_TOKEN(STR_OPROM_POLICY_LEGACY_ONLY),2},
          {STRING_TOKEN(STR_OPROM_POLICY_AUTO),4},
  };
  UINTN         i;
  UINT8         val;
  UINT8         Data;
  UINT8     flags;

    for (i=0; i<(sizeof(opstr)/sizeof(OPTION_ITEM_VALUE)); i++){
        
    val = opstr[i].Value;
    if(val == PCI_OPROM_DEFAULT_VALUE) {
        Data = val;
        flags=(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    }else{
        Data =  (PCI_DEV_EXT|val);
        flags = 0;
    }
            
    HiiCreateOneOfOptionOpCode (
            OpcodeHandle,
            opstr[i].StrId,
            flags,
            EFI_IFR_NUMERIC_SIZE_1,
            Data );
    }
}



//*************************************************************************
VOID CreatePciOpRomItems(
        IN EFI_HII_HANDLE             HiiHandle,
        IN PCI_DEV_SETUP_FORM_DATA    *PciDevFormData,
        IN PCI_DEV_SETUP_DATA         *item
){
  PCI_DEV_SETUP_DATA    *Devitem;
  VOID                *StartOpCodeHandle;
  VOID                *OptionsOpCodeHandle; 
  EFI_QUESTION_ID     QuestionId;
  EFI_VARSTORE_ID     VarStoreId;
  EFI_STRING_ID       VarPromptStringId;
  EFI_STRING_ID       VarHelpStringId;
  EFI_STRING                tString=NULL;
  UINTN                       index;
  UINT16              VarOffset;
  UINT8                     PortCount;
  BOOLEAN                     Tcap;
  EFI_STRING          tID=NULL;//Spiral

  static UINT8        NicPxeNum=0;//<Lvshh001-20200211 Optimized Redfish-OOB function to modify the optionrom of a single network card>
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
  UINT8                     NicMac[6];
  EFI_GUID                  NicGuid = {0};
  EFI_STATUS                Status;
  PCIE_DEV_INFO             PcieDevInfo;
  PCIE_NIC_INFO             *PcieNicInfo;
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->
  PCI_DEV_INFO              *dev;//<Lvshh001-20190605 Add per port PCIe BDF of device >
  
  Devitem = item;
  dev = (PCI_DEV_INFO *)Devitem->PciDevInfo;//<Lvshh001-20190605 Add per port PCIe BDF of device >
  
  if(NULL == PciDevFormData || NULL == Devitem || NULL == HiiHandle) 
    return;
  
  //2. Create Pci OptionRom OneOf items
  StartOpCodeHandle = PciDevFormData->StartOpCodeHandle;
  OptionsOpCodeHandle = Devitem->PciOpRomOpCodeHandle;
  QuestionId = 0;
  VarStoreId = Devitem->VarStoreId;
  index = (Devitem->PciDevIdx*MAX_DEV_PORT)+Devitem->PortIdx;
  VarOffset = (UINT16)EFI_FIELD_OFFSET(OEMPCIE_SETUP_DATA, OpROMEN[index]);
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
  tString = MallocZ(TSTR_SIZE);
  
  Status = gpOemPcieDevProtocol->GetPcieDevInfo(PciDevFormData,Devitem->PciDevIdx,Devitem->PortIdx,&PcieDevInfo);
  if(PcieDevInfo.BaseClassCode == PCI_CL_NETWORK)
  {   
      PcieNicInfo = (PCIE_NIC_INFO *)(PcieDevInfo.DevInfo);
      if(Status != EFI_SUCCESS)
          TRACE ((-1, "Get NIC MAC/GUID fail.\n"));
      TRACE ((-1, "Get NIC MAC/GUID Pass.\n"));
      if(PcieNicInfo->NicMacGuidSelect == 1)  //MAC
      {
        CopyMem(NicMac,PcieNicInfo->NicMac, 6);
        Swprintf(tString, L"Select the execution of UEFI and Legacy OpROM\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\nSeg:Bus:Dev:Func %02X:%02X:%02X:%02X\n",NicMac[0],NicMac[1],NicMac[2],NicMac[3],NicMac[4],NicMac[5],dev->RbIo->SegmentNumber,dev->Address.Addr.Bus,dev->Address.Addr.Device,dev->Address.Addr.Function);//<Lvshh001-20190605 Add per port PCIe BDF of device >
      }else if(PcieNicInfo->NicMacGuidSelect == 2)  //GUID
      {
        CopyMem(&NicGuid,PcieNicInfo->NicGuid, 8);
        Swprintf(tString, L"Select the execution of UEFI and Legacy OpROM\nGUID %G", NicGuid);
      }

      
      VarHelpStringId = HiiSetString(HiiHandle, 0, tString, NULL );
  }
  else
  {
      VarHelpStringId = STRING_TOKEN(STR_PCI_DEV_OPROM_STRING_HELP);
  }
  pBS->FreePool(tString);//Cancon-liutz001-20191220 need improve
  PortCount = *(Devitem->pPortCount);
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->
  tString = MallocZ(TSTR_SIZE);//Cancon-liutz001-20191220 need improve
  tID = MallocZ(TSTR_SIZE);
  Swprintf(tString, L"  Option ROM P%d", Devitem->PortIdx);
  Deleteblock(tString); 
  TRACE ((-1, "[SK] tString--%S\n", tString));//Spiral
  Tcap = Devitem->OpExtend;
  
 //<Lvshh001-20200211 Optimized Redfish-OOB function to modify the optionrom of a single network card +>  
 if(PcieDevInfo.BaseClassCode == PCI_CL_NETWORK)
 {
     Swprintf(tID, L"PXE%01X", ++NicPxeNum);//Spiral
     TRACE ((-1, "[SK] 2tID--%S\n", tID));//Spiral
 }
 else
 {
     Swprintf(tID, L"OEM5%01X%01X", ++sMapNum, Devitem->PortIdx);//Spiral
     TRACE ((-1, "[SK] 2tID--%S\n", tID));//Spiral
 }
 //<Lvshh001-20200211 Optimized Redfish-OOB function to modify the optionrom of a single network card -> 
  if((PortCount > MAX_DEV_PORT) && (Devitem->PortIdx + 1 >= MAX_DEV_PORT)){
  //set MAX_DEV_PORT
    if(Devitem->PortIdx + 1 == MAX_DEV_PORT){
      if(!Tcap){ 
        Swprintf(tString, L"  Option ROM PX");
        Deleteblock(tString); 
        TRACE ((-1, "[SK] tString--%S\n", tString));//Spiral
        VarPromptStringId = HiiSetString(HiiHandle, 0, tString, NULL );
       HiiSetString (HiiHandle, VarPromptStringId, tString, "x-UEFI-AMI");//Spiral
       TRACE ((-1, "[SK] 1\n"));//Spiral
        CreatePciOpRomOptionsEX(OptionsOpCodeHandle,Devitem->PortDefValue);
      }else{
        VarPromptStringId = HiiSetString(HiiHandle, 0, tString, NULL );
       HiiSetString (HiiHandle, VarPromptStringId, tString, "x-UEFI-AMI");//Spiral
       TRACE ((-1, "[SK] 2\n"));//Spiral
        CreatePciOpRomOptions(OptionsOpCodeHandle,Devitem->PortDefValue);           
      }             
    }else{
    
      if(Tcap){
        VarPromptStringId = HiiSetString(HiiHandle, 0, tString, NULL );
       HiiSetString (HiiHandle, VarPromptStringId, tString, "x-UEFI-AMI");//Spiral
       TRACE ((-1, "[SK] 3\n"));//Spiral
        CreatePciOpRomOptionsEX(OptionsOpCodeHandle,Devitem->PortDefValue);
      }else{
        pBS->FreePool(tString);
        return;
      }
    }
  }else{
    VarPromptStringId = HiiSetString(HiiHandle, 0, tString, NULL );
   HiiSetString (HiiHandle, VarPromptStringId, tString, "x-UEFI-AMI");//Spiral
   TRACE ((-1, "[SK] 4\n"));//Spiral
    CreatePciOpRomOptions(OptionsOpCodeHandle,Devitem->PortDefValue);
  }
  //Now create OpRom OneOf OpCode...
  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    QuestionId,
    VarStoreId,
    VarOffset,
    VarPromptStringId,
    VarHelpStringId,
    EFI_IFR_FLAG_RESET_REQUIRED,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
  );
  
  pBS->FreePool(tString);
  pBS->FreePool(tID);
  
}

//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
//*************************************************************************
VOID CreatePcieSriovOptions(VOID *OpcodeHandle,UINT8 DefaultValue){
    static EFI_STRING_ID    opstr[]={
            STRING_TOKEN(STR_DISABLE),
            STRING_TOKEN(STR_ENABLE), 
    };
  UINTN         i;
  UINT8         val;
  UINT8     flags;
  
    for (i=0, val=0; i<(sizeof(opstr)/sizeof(EFI_STRING_ID)); i++, val++){
        
    if(i==DefaultValue) flags=(EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    else flags = 0;

    HiiCreateOneOfOptionOpCode (
            OpcodeHandle,
            opstr[i],
            flags,
            EFI_IFR_NUMERIC_SIZE_1,
            val );
    }
}

//*************************************************************************
VOID CreatePcieSriovItems(
        IN EFI_HII_HANDLE             HiiHandle,
        IN PCI_DEV_SETUP_FORM_DATA    *PciDevFormData,
        IN PCI_DEV_SETUP_DATA         *item
){
  PCI_DEV_SETUP_DATA    *Devitem;
  VOID                *StartOpCodeHandle;
  VOID                *OptionsOpCodeHandle; 
  EFI_QUESTION_ID     QuestionId;
  EFI_VARSTORE_ID     VarStoreId;
  EFI_STRING_ID       VarPromptStringId;
  EFI_STRING_ID       VarHelpStringId;
  UINTN               index;
  UINT16              VarOffset;

  Devitem = item;
  if(NULL == PciDevFormData || NULL == Devitem || NULL == HiiHandle)
    return;

  //2. Create Pci OptionRom OneOf items
  StartOpCodeHandle = PciDevFormData->StartOpCodeHandle;
  OptionsOpCodeHandle = Devitem->PcieSriovOpCodeHandle;
  QuestionId = Devitem->DevEnableQid;
  VarStoreId = Devitem->VarStoreId;
  index = Devitem->PciDevIdx;
  VarOffset = (UINT16)EFI_FIELD_OFFSET(OEMPCIE_SETUP_DATA, SriovEN[index]);
  VarPromptStringId = STRING_TOKEN(STR_PCI_DEV_SRIOV_STRING);
  VarHelpStringId = STRING_TOKEN(STR_PCI_DEV_SRIOV_STRING_HELP);
  
  CreatePcieSriovOptions(OptionsOpCodeHandle,1);
   
  //Now create OpRom OneOf OpCode...
  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    QuestionId,
    VarStoreId,
    VarOffset,
    VarPromptStringId,
    VarHelpStringId,
    EFI_IFR_FLAG_RESET_REQUIRED,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
  );

}
#endif
//<bieyb-20180727 add function enable SRIOV by port->

//<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
VOID CreatePcieLnkSpeedOptions(VOID *OpcodeHandle,UINT8 DefaultValue){
  static EFI_STRING_ID    opstr[]={
            STRING_TOKEN(STR_OPROM_POLICY_AUTO), //SGMTB#163841:Add Chinese Support
            STRING_TOKEN(STR_2_5G),
            STRING_TOKEN(STR_5_0G),
            STRING_TOKEN(STR_8_0G),
  };

  UINTN     i;
  UINT8     val;
  UINT8     flags;

  for (i=0, val=0; i<(sizeof(opstr)/sizeof(EFI_STRING_ID)); i++, val++) {

    if(i == DefaultValue)
      flags = (EFI_IFR_OPTION_DEFAULT | EFI_IFR_OPTION_DEFAULT_MFG);
    else
      flags = 0;

    HiiCreateOneOfOptionOpCode (OpcodeHandle, opstr[i], flags, EFI_IFR_NUMERIC_SIZE_1, val);
  }
}

//*************************************************************************
VOID CreatePciLnkSpeedItems(
        IN EFI_HII_HANDLE           HiiHandle,
        IN PCI_DEV_SETUP_FORM_DATA  *PciDevFormData,
        IN PCI_DEV_SETUP_DATA       *item
)
{
  PCI_DEV_SETUP_DATA  *Devitem;
  VOID                *StartOpCodeHandle;
  VOID                *OptionsOpCodeHandle; 
  EFI_QUESTION_ID     QuestionId;
  EFI_VARSTORE_ID     VarStoreId;
  EFI_STRING_ID       VarPromptStringId;
  EFI_STRING_ID       VarHelpStringId;
  UINTN               index;
  UINT16              VarOffset;

  Devitem = item;
  if(NULL == PciDevFormData || NULL == Devitem || NULL == HiiHandle)
    return;

  //1. Create Pci LnkSpeed OneOf items
  StartOpCodeHandle = PciDevFormData->StartOpCodeHandle;
  OptionsOpCodeHandle = Devitem->PcieLnkSpeedOpCodeHandle;
  QuestionId = Devitem->DevEnableQid;
  VarStoreId = Devitem->VarStoreId;
  index = Devitem->PciDevIdx;
  VarOffset = (UINT16)EFI_FIELD_OFFSET(OEMPCIE_SETUP_DATA, PciLnkSpeed[index]);
  VarPromptStringId = STRING_TOKEN(STR_PCI_DEV_LnkSpeed_STRING);
  VarHelpStringId = STRING_TOKEN(STR_PCI_DEV_LnkSpeed_STRING_HELP);;

  CreatePcieLnkSpeedOptions(OptionsOpCodeHandle,Devitem->LnkSpeedDefValue);

  //Now create OneOf OpCode...
  HiiCreateOneOfOpCode (
        StartOpCodeHandle,                  //*OpCodeHandle
        QuestionId,                         //QuestionId
        VarStoreId,                         //VarStore ID
        VarOffset,                          //Offset
        VarPromptStringId,                  //Prompt
        VarHelpStringId,                    //Help 
        EFI_IFR_FLAG_RESET_REQUIRED,        //QuestionFlags
        EFI_IFR_NUMERIC_SIZE_1,             //OneOfFlags
        OptionsOpCodeHandle,                //OptionsOpCodeHandle
        NULL                                //defopt //*DefaultsOpCodeHandle  OPTIONAL
      );
}
#endif
//<Yaosr-20201201 Set PCIe link speed by device. ->

//*************************************************************************
VOID CreatePciDeviceItems(IN EFI_EVENT Event, IN VOID *Context)
{
  EFI_GUID                          gEfiSetupFormGuid = OEMPCIEDEV_FORM_SET_GUID; 
  EFI_HII_HANDLE                    HiiHandle;
  PCI_DEV_SETUP_FORM_DATA       *PciDevFormData;
  PCI_DEV_IDX_DATA          *IdxDev;
  PCI_DEV_SETUP_DATA              *Devitem; 
  VOID                          *StartOpCodeHandle;
  VOID                          *EndOpCodeHandle;
  UINTN                                 i,j;


  TRACE ((-1, "CreatePciDeviceItems.\n"));
  
    if (Event) {
      pBS->CloseEvent(Event);
  }
    
    HiiHandle = gOemPcieDevHiiHandle;
    PciDevFormData = (PCI_DEV_SETUP_FORM_DATA*)gPciDevListData;
    
    if(NULL == PciDevFormData || NULL == HiiHandle ) return;
    
    if(PciDevFormData->PciDevCount == 0) return;
    
    StartOpCodeHandle = PciDevFormData->StartOpCodeHandle;
    EndOpCodeHandle = PciDevFormData->EndOpCodeHandle;
    
  for (i=0; i<PciDevFormData->PciDevCount; i++) {
    
    IdxDev = PciDevFormData->PciDevIdxData[i];
    
    if(IdxDev->PortData[0].ShowItem != TRUE)
      continue;
    
    //1.Create Pci Setup OneOf items
    CreatePciSetupItems(HiiHandle,PciDevFormData,&IdxDev->PortData[0]);
    //Create Suppress if opcode based on Global Settings OEMPCIE_SETUP_DATA.PciDevEN
    AmiHiiCreateSuppresGrayIdVal(StartOpCodeHandle, IdxDev->PortData[0].DevEnableQid, 0,TRUE);
    
    for(j=0;j<IdxDev->PortCount;j++){
      Devitem = &IdxDev->PortData[j];
      //2. Create Pci OptionRom OneOf items
      if(Devitem->ShowItem == TRUE) CreatePciOpRomItems(HiiHandle,PciDevFormData,Devitem);
      
    }
    
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      //3. Create Pcie SRIOV OneOf items
      if(IdxDev->PortData[0].SriovCap == TRUE)  CreatePcieSriovItems(HiiHandle,PciDevFormData,&IdxDev->PortData[0]);
#endif
//<bieyb-20180727 add function enable SRIOV by port->

    //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
    //4. Create Pcie LinkSpeed OneOf items
    CreatePciLnkSpeedItems(HiiHandle, PciDevFormData, &IdxDev->PortData[0]);
#endif
    //<Yaosr-20201201 Set PCIe link speed by device. ->

    //close scope
    AmiHiiTerminateScope(StartOpCodeHandle);
  }
    
  HiiUpdateForm (
    HiiHandle,
    &gEfiSetupFormGuid,
    PCI_DEV_LIST_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
  );
  
  //Free memory resource
  for (i=0; i<PciDevFormData->PciDevCount; i++) {
    
    IdxDev = PciDevFormData->PciDevIdxData[i];
    
    for(j=0;j<IdxDev->PortCount;j++){
      
      Devitem = &IdxDev->PortData[j];

      if (Devitem->PciSetupOpCodeHandle != NULL) HiiFreeOpCodeHandle (Devitem->PciSetupOpCodeHandle);
      if (Devitem->PciOpRomOpCodeHandle != NULL) HiiFreeOpCodeHandle (Devitem->PciOpRomOpCodeHandle);
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      if (Devitem->PcieSriovOpCodeHandle != NULL) HiiFreeOpCodeHandle (Devitem->PcieSriovOpCodeHandle);
#endif
//<bieyb-20180727 add function enable SRIOV by port->
      //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
      if (Devitem->PcieLnkSpeedOpCodeHandle != NULL) HiiFreeOpCodeHandle (Devitem->PcieLnkSpeedOpCodeHandle);
#endif
      //<Yaosr-20201201 Set PCIe link speed by device. ->
    }
  }
  if (StartOpCodeHandle != NULL) HiiFreeOpCodeHandle (StartOpCodeHandle);
  if (EndOpCodeHandle != NULL) HiiFreeOpCodeHandle (EndOpCodeHandle);
}

EFI_STATUS InitPcieIfr(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS                        Status;
    PCI_DEV_SETUP_FORM_DATA       *DevListData;

    TRACE ((-1, "InitPcieDevData\n"));
//<licm-20210531 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
    UpdateTableListInVariable();
#endif
//<licm-20210531 Optimization PCIE Device List module ->
	if (Event) {
		pBS->CloseEvent(Event);
	}

    Status = pBS->LocateProtocol (
             &gOemPcieDevProtocolGuid,
             NULL,
             &gpOemPcieDevProtocol
             );
    if ( EFI_ERROR (Status) ) 
    {
         return Status;
    }
  
  Status = gpOemPcieDevProtocol->GetPcieDevList(&DevListData);
  
  Status= InitPciDevData (gOemPcieDevHiiHandle,DevListData);
  if(EFI_ERROR(Status)) return Status;
  
  gpOemPcieDevProtocol->DeviceListInitDone();
    return Status;
}

EFI_STATUS SetPciDevPolicyDefaults()
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_GUID      gOemSetupGuid = OEMPCIEDEV_VARSTORE_GUID; 
  UINTN         SetupDataSize;
  UINTN         index;
  UINT32        Attributes = 0;
  UINT8         DefaultValue;
  UINT8         BoardId,SkuID;
  OEMPCIE_SETUP_DATA  PcieSetupData;    
  TRACE((-1, "SetPCIeDevPolicyDefaults.\n"));
  
  Status = GetBoardSkuId(&BoardId,&SkuID);
  if(EFI_ERROR(Status)) return Status;
  
  //SetupDataSize = sizeof(SETUP_DATA);
  //Status = pRS->GetVariable ( L"Setup", &gEfiSetupGuid, &Attributes, &SetupDataSize, &SetupData);
  SetupDataSize = sizeof(OEMPCIE_SETUP_DATA);
  Status = pRS->GetVariable (OEMPCIEVAR, &gOemSetupGuid, &Attributes, &SetupDataSize, &PcieSetupData);
  if(!EFI_ERROR(Status)){
    for(index=0;index < DYNAMIC_PCIDEV_ENTRY_NUMBER;index++){
      if(index<MAX_DEV_COUNT)
      {
        //override PciDevEN default value
        DefaultValue = GetDeviceDefValueByVarIdx(BoardId,SkuID,index);
        PcieSetupData.PciDevEN[index] = DefaultValue;
        //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
        DefaultValue = GetDeviceLnkSpeedDefValueByVarIdx(BoardId,SkuID,index);
        PcieSetupData.PciLnkSpeed[index] = DefaultValue;
#endif
        //<Yaosr-20201201 Set PCIe link speed by device. ->
      }
  
      //override OpROMEN default value
      DefaultValue = GetDevicePortDefValueByVarIdx(BoardId,SkuID,index);
      PcieSetupData.OpROMEN[index] = DefaultValue;
      
//<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      //override SRIOVEN default value
      if(index<MAX_DEV_COUNT)
      {
      DefaultValue = GetDeviceSriovDefValueByVarIdx(BoardId,SkuID,index);
      PcieSetupData.SriovEN[index] = DefaultValue;
      }
#endif
//<bieyb-20180727 add function enable SRIOV by port->
    }
    Status=pRS->SetVariable(OEMPCIEVAR,&gOemSetupGuid, Attributes, sizeof(OEMPCIE_SETUP_DATA), &PcieSetupData);
  }
  return Status;
}

VOID LoadPciDevPolicyDefaults(    
    IN EFI_EVENT Event, 
    IN VOID *Context
    )
{
  EFI_STATUS            Status;
  EFI_GUID              gEfiSetupGuid = OEMPCIEDEV_VARSTORE_GUID;
  EFI_GUID              gTseTransferGuid = TSE_TRANSFER_GUID;
  OEMPCIE_SETUP_DATA  *SetupDataPtr;
  UINTN                 SetupDataSize = sizeof(OEMPCIE_SETUP_DATA);
  UINTN                 index;
  UINT8                 DefaultValue;
  UINT8                 BoardId,SkuID;
  
  Status = GetBoardSkuId(&BoardId,&SkuID);
  if(EFI_ERROR(Status)) return;
  
  if(gTseSetuptransferInterface == NULL){
     Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &gTseSetuptransferInterface);
     if(EFI_ERROR(Status)) return;
  }
  
  Status = gTseSetuptransferInterface->VarGetBuffer( OEMPCIEVAR,gEfiSetupGuid,(UINT8**)&SetupDataPtr,&SetupDataSize);
  if(!EFI_ERROR(Status)) {
    for(index=0;index < DYNAMIC_PCIDEV_ENTRY_NUMBER;index++){
      if(index < MAX_DEV_COUNT)
      { 
        //override PciDevEN default value
        DefaultValue = GetDeviceDefValueByVarIdx(BoardId,SkuID,index);
        SetupDataPtr->PciDevEN[index] = DefaultValue;
        //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
        DefaultValue = GetDeviceLnkSpeedDefValueByVarIdx(BoardId,SkuID,index);
        SetupDataPtr->PciLnkSpeed[index] = DefaultValue;
#endif
        //<Yaosr-20201201 Set PCIe link speed by device. ->
      }
      //override OpROMEN default value
      DefaultValue = GetDevicePortDefValueByVarIdx(BoardId,SkuID,index);
      SetupDataPtr->OpROMEN[index] = DefaultValue;
      
      //<bieyb-20180727 add function enable SRIOV by port+>
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
      //override SRIOVEN default value
      if(index < MAX_DEV_COUNT)
      { 
      DefaultValue = GetDeviceSriovDefValueByVarIdx(BoardId,SkuID,index);
      SetupDataPtr->SriovEN[index] = DefaultValue;
      }
#endif
      //<bieyb-20180727 add function enable SRIOV by port->
    }
  }
    return;
}

EFI_STATUS  GetPcieDevListBuffer(
        IN  PCI_DEV_SETUP_FORM_DATA **PcieData
)
{
    if(PcieData==NULL || gPciDevListData == NULL) 
    	return EFI_INVALID_PARAMETER;
    
    *PcieData = gPciDevListData;
    
    return EFI_SUCCESS;
}

EFI_STATUS GetDevicePortIndex(
    VOID *Dev,
    UINTN DevIdx,
    UINT8 *PortIndex)
{
  return GetDevicePortIndexLib(gPciDevListData,Dev,DevIdx,PortIndex);
}

EFI_STATUS GetDevicePortVarIndex(
    VOID *Dev,
    UINTN DevIdx,
    UINT8 *PortIndex,
    UINTN *PortVarIndex
    )
{
  return GetDevicePortVarIndexLib(gPciDevListData,Dev,DevIdx,PortIndex,PortVarIndex);
}

VOID DeviceListInitDone()
{
  EFI_HANDLE DummyHandle = NULL;
  
  if(gPciDevListData->InitStatus != 0xAA){
    
    //EFI_GUID OemNvramUpdateGuid = OEM_NVRAM_UPDATE_GUID;
    pBS->InstallProtocolInterface(
        &DummyHandle, &gOemPcieDevInitDoneGuid, EFI_NATIVE_INTERFACE, NULL
    );
    gPciDevListData->InitStatus = 0xAA;
  }
}

EFI_STATUS GetSetPcieDevPolicy(
        IN OUT VOID* PolicyData,
        IN PCIEDATA_TYPE  Type,
        IN BOOLEAN   Get
)
{
  UINTN         SetupDataSize;
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_GUID      OemSetupGuid = OEMPCIEDEV_VARSTORE_GUID;
  UINT32        Attributes=(EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE); 
  if(PolicyData==NULL){
    return EFI_INVALID_PARAMETER;
  }
  if(Get){
    if(!gPciePolicyData->IsNew){
        SetupDataSize = sizeof(OEMPCIE_SETUP_DATA);
        Status = pRS->GetVariable (OEMPCIEVAR, &OemSetupGuid, &Attributes, &SetupDataSize, &(gPciePolicyData->OemPcieData));
        if(EFI_ERROR(Status)){
            return EFI_NOT_READY;
        }
        gPciePolicyData->IsNew = TRUE;
    }
    switch(Type){
    case PCIDEVEN:
        pBS->CopyMem(PolicyData,gPciePolicyData->OemPcieData.PciDevEN, MAX_DEV_COUNT);
        break;
    case OPROMEN:
         pBS->CopyMem(PolicyData,gPciePolicyData->OemPcieData.OpROMEN, PCIDEV_ENTRY_NUMBER);
        break;
    #if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
    case SRIOVEN:
         pBS->CopyMem(PolicyData,gPciePolicyData->OemPcieData.SriovEN, MAX_DEV_COUNT);
        break;
    #endif 
    //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
    case PCILNKSPEED:
        pBS->CopyMem(PolicyData,gPciePolicyData->OemPcieData.PciLnkSpeed, MAX_DEV_COUNT);
        break;
#endif
    //<Yaosr-20201201 Set PCIe link speed by device. -
    default:
        return EFI_INVALID_PARAMETER;
   }
    
  }else{
    switch(Type){
    case PCIDEVEN:
        pBS->CopyMem(gPciePolicyData->OemPcieData.PciDevEN,PolicyData,MAX_DEV_COUNT);
        break;
    case OPROMEN:
         pBS->CopyMem(gPciePolicyData->OemPcieData.OpROMEN,PolicyData, PCIDEV_ENTRY_NUMBER);
        break;
    #if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
    case SRIOVEN:
         pBS->CopyMem(gPciePolicyData->OemPcieData.SriovEN,PolicyData, MAX_DEV_COUNT);
        break;
    #endif
    //<Yaosr-20201201 Set PCIe link speed by device. +>
#if defined(PCIE_LINK_SPEED_BY_DEVICE_SUPPORT) && (PCIE_LINK_SPEED_BY_DEVICE_SUPPORT == 1)
    case PCILNKSPEED:
         pBS->CopyMem(gPciePolicyData->OemPcieData.PciLnkSpeed, PolicyData, MAX_DEV_COUNT);
         break;
#endif
    //<Yaosr-20201201 Set PCIe link speed by device. ->
    default:
        return EFI_INVALID_PARAMETER;
   }
    Status=pRS->SetVariable(OEMPCIEVAR,&OemSetupGuid, Attributes, sizeof(OEMPCIE_SETUP_DATA), &(gPciePolicyData->OemPcieData)); 
    if(EFI_ERROR(Status)){
        return Status;
    }
    
    gPciePolicyData->IsNew = FALSE;
  }
  return EFI_SUCCESS;
}

//<lvych001-20171205 Support Network port efi oprom per-control function.+>
EFI_STATUS GetDevPortOpRomPolicy(
    IN OEMPCIE_SETUP_DATA   *pSetupData,
    IN UINT8        PciClass,
    UINTN           DevIndex,
    UINTN           PortIndex,
    OUT UINT8       *pPolicy
)
{
    UINT8 Policy,Value8;
    UINTN SetupDataIndex,tIndex,dIdx;

    if((pSetupData == NULL) || (pPolicy == NULL))
      return EFI_INVALID_PARAMETER;


    if(pSetupData->PciDevEN[DevIndex] == 0) {
      *pPolicy = POLICY_DISABLE;
      return EFI_SUCCESS;
    }

    switch(PciClass) {
        case PCI_CL_NETWORK:
            Policy = pSetupData->OpROMEN[PXEINDX];
            break;

        case PCI_CL_MASS_STOR:
            Policy = pSetupData->OpROMEN[MASSINDX];
            break;

        case PCI_CL_DISPLAY:
            Policy = pSetupData->OpROMEN[VIDEOINDX];
            break;

        default:
/*
    For PCI devices with PCI class different from Network, Mass Storage or Video
    behavior will be as in previous versions of PciBus driver - UEFI OpROM will
    be executed first or not executed at all
*/
            Policy = pSetupData->OpROMEN[OLDINDX];
            break;
    }

    SetupDataIndex = DevIndex*MAX_DEV_PORT + PortIndex;

    dIdx = SetupDataIndex;
    if(PortIndex >= MAX_DEV_PORT){
      //check dev max port value
      tIndex = SetupDataIndex - PortIndex + MAX_DEV_PORT -1;
      if(pSetupData->OpROMEN[tIndex] == PCI_OPROM_DEFAULT_VALUE){
        dIdx = tIndex;
          if((pSetupData->OpROMEN[SetupDataIndex] & PCI_DEV_EXT)== PCI_DEV_EXT) {
        dIdx = SetupDataIndex;
      }
      } else if((pSetupData->OpROMEN[tIndex] & PCI_DEV_EXT)== PCI_DEV_EXT){//not default value conflict check
         dIdx = tIndex;
      }
    }

    Value8 = pSetupData->OpROMEN[dIdx];
    Value8 &= ~PCI_DEV_EXT;

    //Auto option return
    if(Value8 != OPTION_AUTO_POLICY)
      Policy = Value8;

    *pPolicy = Policy;

    return EFI_SUCCESS;
}

VOID ProcessPcieDevDriver(
    IN EFI_EVENT  Event,
    IN VOID       *Context
    )
{
  EFI_STATUS                      Status;
  EFI_GUID                        gEfiSetupGuid = SETUP_GUID;
  SETUP_DATA                      SetupData;
  UINTN                           SetupDataSize;
  PCI_DEV_SETUP_FORM_DATA         *PcieDevData;
  PCI_DEV_IDX_DATA                *IdxDev;
  PCI_DEV_SETUP_DATA              *DevItem;
  PCI_DEV_INFO                    *pdev;
  UINTN                           i,j;
  UINT8                           OpromPly;
  
#if defined(OemPCIeDev_SUPPORT) && (OemPCIeDev_SUPPORT == 1) 
  EFI_GUID                        OemSetupGuid = OEMPCIEDEV_VARSTORE_GUID;
  UINT32                          Attributes=(EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE);
#endif

  TRACE((-1, "ProcessPcieDevDriver\n"));
  SetupDataSize = sizeof(SETUP_DATA);
  Status = pRS->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  if(EFI_ERROR(Status)) return;
  
  SetupDataSize = sizeof(OEMPCIE_SETUP_DATA);
  Status = pRS->GetVariable (OEMPCIEVAR, &OemSetupGuid, &Attributes, &SetupDataSize, &(gPciePolicyData->OemPcieData));
  if(EFI_ERROR(Status)){
    return ;
  }
  
  PcieDevData = NULL;
  GetPcieDevListBuffer(&PcieDevData);

  for(i=0;i<PcieDevData->PciDevCount;i++)
  {
    IdxDev = PcieDevData->PciDevIdxData[i];
    for(j=0;j<IdxDev->PortCount;j++)
    {
      DevItem = &IdxDev->PortData[j];
      pdev = (PCI_DEV_INFO *)DevItem->PciDevInfo;
      Status = GetDevPortOpRomPolicy(&(gPciePolicyData->OemPcieData),pdev->Class.BaseClassCode,DevItem->PciDevIdx,DevItem->PortIdx,&OpromPly);
      if(!EFI_ERROR(Status)){
//<huangjin001-20190425-Fix can't load Uefi network driver with CSM disabled +>
//        if((OpromPly == POLICY_LEGACY) || (OpromPly == POLICY_DISABLE)){
        if(((OpromPly == POLICY_LEGACY)&&(SetupData.CsmSupport != 0)) || (OpromPly == POLICY_DISABLE)){
//<huangjin001-20190425-Fix can't load Uefi network driver with CSM disabled ->         
          //Support Network disconnect by port
          if(pdev->Class.BaseClassCode == PCI_CL_NETWORK)
            pBS->DisconnectController(pdev->Handle, NULL, NULL);
        }
      }
    }
  }
}
//<lvych001-20171205 Support Network port efi oprom per-control function.->

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   PCIeDevPolicyEntryPoint
//
// Description: This function is the entry point for this DXE. This function
//              initializes the Project
//
// Parameters:  ImageHandle Image handle
//              SystemTable Pointer to the system table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PCIeDevPolicyEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    ThisHandle = NULL;
  EFI_EVENT     LoadDevDefEvent;
  VOID          *LoadDevDefRegistration;
//<lvych001-20171205 Support Network port efi oprom per-control function.+>
  EFI_EVENT     pDEvent;
  VOID          *pDRegistration;
//<lvych001-20171205 Support Network port efi oprom per-control function.->
  UINTN         VariableSize =0;
  UINT32        Attributes = 0;
  UINT32        BootMode = BOOT_WITH_FULL_CONFIGURATION;
  UINT8		    Data8;
  EFI_GUID      SetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
  EFI_GUID      guidBdsBeforeSetup = EFI_BDS_EVENT_BEFORE_SETUP_GUID;
  VOID          *SetupRegistration;
  VOID          *BdsEventBeforeSetupRegistration;
  EFI_EVENT     SetupEnterEvent;
  EFI_EVENT     BdsEventBeforeSetup;
  EFI_EVENT     PciDevEvent;  
  
  InitAmiLib(ImageHandle, SystemTable);
  
  TRACE((-1, "PCIeDevPolicyEntryPoint !!\n"));
  
  gPciDevListData = MallocZ(sizeof(PCI_DEV_SETUP_FORM_DATA));
  if(NULL == gPciDevListData)
    return EFI_OUT_OF_RESOURCES;
  
  gPciePolicyData=MallocZ(sizeof(PCIE_POLICY_DATA));
  if(gPciePolicyData==NULL) 
    return EFI_OUT_OF_RESOURCES;
  
  gOemPcieDevProtocol.GetPcieDevList = GetPcieDevListBuffer;
  gOemPcieDevProtocol.GetBoardSkuId =  GetBoardSkuId;
  gOemPcieDevProtocol.GetDeviceDefaultPolicy = GetDeviceDefValueByVarIdx;
  gOemPcieDevProtocol.GetDevicePortDefaultPolicy = GetDevicePortDefValueByVarIdx;
  gOemPcieDevProtocol.UpdateDevicePortCfg = UpdateDevicePortCfg;
  gOemPcieDevProtocol.GetDeviceIndex = GetDeviceIndexLib;
  gOemPcieDevProtocol.GetDevicePortIndex = GetDevicePortIndex;
  gOemPcieDevProtocol.GetDevicePortVarIndex = GetDevicePortVarIndex;
  gOemPcieDevProtocol.DeviceListInitDone = DeviceListInitDone;
  gOemPcieDevProtocol.GetSetPcieDevPolicy = GetSetPcieDevPolicy;
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
  gOemPcieDevProtocol.GetPcieDevInfo = GetPcieDevInfo;
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->
//<licm-20210513 Optimization PCIE Device List module +>
#if defined(Optimization_PCIE_Device_List_SUPPORT) && (Optimization_PCIE_Device_List_SUPPORT == 1)
  gOemPcieDevProtocol.DynamicGenerationRbtable = DynamicGenerationRbtable;
  gOemPcieDevProtocol.GetTableListInVariable = GetTableListInVariable;
#endif
//<licm-20210513 Optimization PCIE Device List module ->  
  Status = pBS->InstallProtocolInterface (
          &ThisHandle, &gOemPcieDevProtocolGuid,
          EFI_NATIVE_INTERFACE, &gOemPcieDevProtocol
  );
  
  VariableSize = sizeof(UINT8);
  Status=pRS->GetVariable(L"PCIE_DEVE",&gAmiTseOemPortingVar1Guid, &Attributes, &VariableSize, &Data8); 
  if(EFI_ERROR(Status)){
      if(Status==EFI_NOT_FOUND) {
        Attributes=(EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE); 
        Data8 = 1;
        Status=pRS->SetVariable(L"PCIE_DEVE",&gAmiTseOemPortingVar1Guid, Attributes, sizeof(UINT8), &Data8);
        ASSERT_EFI_ERROR(Status);
        Status=SetPciDevPolicyDefaults();
      }
  }else{
    BootMode = GetBootModeHob ();
    if(BootMode == BOOT_WITH_DEFAULT_SETTINGS)
        Status=SetPciDevPolicyDefaults();
  }
  
  //Load setup page and update strings
  Status = LoadResources(ImageHandle, sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), SetupCallBack, NULL);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR,"LoadResources (OemPCIeDev.c) Error\n"));
  }
  gOemPcieDevHiiHandle = SetupCallBack[0].HiiHandle;
  
  Status =  RegisterProtocolCallback(
             &guidBdsBeforeSetup,
             InitPcieIfr,
             NULL,
             &BdsEventBeforeSetup,
             &BdsEventBeforeSetupRegistration );
  
  Status = RegisterProtocolCallback(
      &SetupEnterGuid, CreatePciDeviceItems,
      NULL, &SetupEnterEvent, &SetupRegistration
  );
  Status = CreateReadyToBootEvent(TPL_CALLBACK,
      PciDevReadyToBootProcess,
      (VOID*)gPciDevListData,
      &PciDevEvent);
  
  Status = RegisterProtocolCallback(
        &gOemSetupLoadDefaultGuid,
        LoadPciDevPolicyDefaults,
        NULL, &LoadDevDefEvent, &LoadDevDefRegistration
        );
//<lvych001-20171205 Support Network port efi oprom per-control function.+>
  Status = RegisterProtocolCallback(
            &gBdsAllDriversConnectedProtocolGuid,
            ProcessPcieDevDriver,
            NULL, &pDEvent, &pDRegistration
            );
//<lvych001-20171205 Support Network port efi oprom per-control function.->
  
  return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **

/** @file NbSetup.c
    North Bridge Setup Rountines

**/

#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol/AmiCpuInfo.h>
#include <AMIHobs.h>
#include <Protocol/AmiSmbios.h>
#include <Protocol/Smbios.h>
#include <Library/DebugLib.h>
#include "Protocol/FabricTopologyServices.h"
#include <Library/ApcbLib.h>
#include <Library/PciLib.h>
#include "Library/MemChanXLat.h"
#include "Setup.h"
//<ChengXW001-20171104 Record a SEL if memory training failed while post +>
#include <Protocol/IPMITransportProtocol.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
//<ChengXW001-20171104 Record a SEL if memory training failed while post ->
//dengyh-20220403-show dimm error info on logo +
#define DIMM_ERROR_MESSAGE_COLUMN 0
#define DIMM_ERROR_MESSAGE_ROW 7
//dengyh-20220403-show dimm error info on logo -
// CPU
#define MSR_SYS_CFG     0x0C0010010
#define MSR_TOP_MEM     0x0C001001A
#define MSR_TOP_MEM2    0x0C001001D

//AMICSPLib Cpu
UINT64 ReadMsr(UINT32 Msr);
VOID WriteMsr(UINT32 Msr, UINT64 Value);

//MemClk


#define DDR1333_FREQ 1     ///< DDR 1333
#define DDR1600_FREQ 2     ///< DDR 1600
#define DDR1866_FREQ 3     ///< DDR 1866
#define DDR2133_FREQ 4    ///< DDR 2133
#define DDR2400_FREQ 5    ///< DDR 2400
#define DDR2666_FREQ 6    ///< DDR 2666 <Yaosr001-20181009 add memory clock 2666MHz>
#define DDR2933_FREQ 7    ///< DDR 2933 
#define DDR3200_FREQ 8    ///< DDR 3200 //<libj-20220310 add memory 3200MHz option>

#define DIE_PER_SCKT 4
// External data definitions
EFI_GUID gEfiSetupGuid = SETUP_GUID;


//----------------------------------------------------------------------------
EFI_PHYSICAL_ADDRESS  TotalMemorySize = 0;
EFI_PHYSICAL_ADDRESS  SystemRam = 0;

#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8


VOID
NbSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32          *Value
  )
{
    UINT32 read_value=0;
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    read_value=PciRead32(NB_SMN_DATA_3_BIOS);
    *Value=read_value;
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
}
/**
    Return TOM2 MSR value or zero.

    @param VOID

    @retval UINT64 TOM2 value

**/

UINT64
GetTom2(
  IN    VOID
)
{
    return (ReadMsr(MSR_SYS_CFG)&BIT21) ? ReadMsr(MSR_TOP_MEM2) : 0;
}

/**
    Return TOM MSR value.

    @param VOID

    @retval UINT64 TOM value

**/

UINT64
GetTom(
  IN    VOID
)
{
    return ReadMsr(MSR_TOP_MEM);
}

/**

    This performs word-by-word copy of a Unicode string.

    @param Destination - Pointer to a CHAR16 string buffer to receive the 
                         contents
    @param Source - Pointer to a null-terminated CHAR16 string to be copied.

    @return VOID

    @note
    Assumes the destination string is large enough (error checking must
    be made by caller).

**/
VOID
StrCpy16 (
  IN CHAR16   *Destination,
  IN CHAR16   *Source )
{
  while (*Source) {
    *(Destination++) = *(Source++);
  }
  *Destination = 0;
}
/**

    This is a worker function to extract the SMBIOS string from the
    specified SMBIOS structure data.

    @param StructureHandle - SMBIOS structure data pointer.
    @param StringNumber - The string number to get.
    @param StringData - The string is copied here.

    @return EFI_STATUS
    @retval EFI_SUCCESS or valid error code.

    @note
    This function is called by GetSmbiosDeviceString.
    Assumes the string is large enough (error checking must be made by caller).

**/

EFI_STATUS
GetSmbiosStringByNumber (
    IN  UINT8    *StructureHandle,
    IN  UINT8    StringNumber,
    OUT UINT8    **StringData )
{
   *StringData = StructureHandle +
                ((SMBIOS_STRUCTURE_HEADER*)StructureHandle)->Length;

    while (--StringNumber) {
        while(**StringData != 0) {
            (*StringData)++;
        }
        (*StringData)++;
    }

    return EFI_SUCCESS;
}
/**

    Simple utility function to convert an ASCII string to Unicode.

    @param UnicodeStr - Converted string.
    @param AsciiStr - String to be converted

    @return CHAR16 * Same as UnicodeStr to allow use in an assignment.

    @note  N/A

**/

CHAR16 *
Ascii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  CHAR8          *AsciiStr )
{
  CHAR16  *Str;

  Str = UnicodeStr;

  while (TRUE) {

    *(UnicodeStr++) = (CHAR16) *AsciiStr;

    if (*(AsciiStr++) == '\0') {
      return Str;
    }
  }
}

VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID
  )
{
  UINT8 *Buffer;

  Buffer = PlatformMemoryConfiguration;

  while ((Buffer[0] != PSO_END) && (Buffer[0] != PSO_CPU_FAMILY_TO_OVERRIDE)) {
    if (Buffer[0] == EntryType) {
      if ((Buffer[2] & ((UINT8) 1 << SocketID)) != 0 ) {
        if ((Buffer[3] & ((UINT8) 1 << ChannelID)) != 0 ) {
          if ((Buffer[4] & ((UINT8) 1 << DimmID)) != 0 ) {
            return &Buffer[5];
          }
        }
      }
    }
    Buffer += Buffer[1] + 2;
  }

  return NULL;
}

UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  )
{
  UINT8  *DimmsPerChPtr;
  UINT8  MaxDimmPerCH;

  DimmsPerChPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, ChannelID, 0);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = MAX_DIMMS_PER_CHANNEL;
  }

  // Maximum number of dimms per channel cannot be larger than its default value.
  ASSERT (MaxDimmPerCH <= MAX_DIMMS_PER_CHANNEL);

  return MaxDimmPerCH;
}

/**
    This function initializes the NB related setup option values

    @param HiiHandle Handle to HII database
    @param Class Indicates the setup class

    @retval VOID

**/
VOID
InitNbStrings(
  IN    EFI_HII_HANDLE  HiiHandle,
  IN    UINT16          Class
)
{
    static BOOLEAN                      Enumerate = FALSE;
    UINT8                               CurDimm;
    UINT8                               CurSocket;
    UINT8                               CurChannel;
    HOST_TO_APCB_CHANNEL_XLAT           *ApcbToBoardChanXLatTab;
    UINT8                               Index;
    UINT8                               HostChannel;
    BOOLEAN                             ValidChannel = FALSE;
    EFI_STATUS                          Status = EFI_DEVICE_ERROR;
    EFI_PHYSICAL_ADDRESS                Tom1;
    EFI_PHYSICAL_ADDRESS                Tom2;
    EFI_SMBIOS_PROTOCOL                 *EfiSmbios;
    EFI_SMBIOS_HANDLE                   SmbiosHandle;
    EFI_SMBIOS_TYPE                     SmbiosType;
    EFI_SMBIOS_TABLE_HEADER             *SmbiosTable;
    SMBIOS_MEMORY_DEVICE_INFO           *Type17;
    UINT32                              Type17Speed;
    UINT32                              Type17Size;
    
    UINT32                              DieNum;
    UINT32                              Address;
    UINT32                              IsTrained;
    
    static AMI_SMBIOS_PROTOCOL          *SmbiosProtocol = NULL;
    CHAR8                               *StrAsciiTmpPtr;
    CHAR8                               *DevLocStrAsciiTmpPtr;
    CHAR16                              BankLocatorString[13];
    CHAR16                              DeviceLocatorString[11];
    UINT32                              TypeDataSize;
    UINT8                               *PlatformMemoryConfiguration;
    UINT8                               DimmStringCurNum;
    UINTN                               StringNumber;
    CHAR8                               *UnknownString="Unknown";    
    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup +>   
    UINT8                               EccCapbility;
    UINT8                               ModuleType;
    UINT8                               RankNum;
    UINT8                               DramWidth;
    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup ->
//<Kangmm-20190517 Modify OemData Follow Oem SEL spec+>
    UINT8                               DimmNum;
    UINT8                               UMCNum;
//<Kangmm-20190517 Modify OemData Follow Oem SEL spec->  
//dengyh-20220403-show dimm error info on logo +
CHAR16      TempStr[500] = L"\0";
UINT8        DimmErrorInfoCol=DIMM_ERROR_MESSAGE_COLUMN;
UINT8        DimmErrorInfoRow=DIMM_ERROR_MESSAGE_ROW;
//dengyh-20220403-show dimm error info on logo +  
    UINT16  DimmString[] = {
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT0_VALUE),    // Array0 Dimm0
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT1_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT2_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT3_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT4_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT5_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT6_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT7_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT8_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT9_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT10_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT11_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT12_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT13_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT14_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT0_SLOT15_VALUE),       
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT0_VALUE),    
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT1_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT2_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT3_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT4_VALUE),    
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT5_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT6_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT7_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT8_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT9_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT10_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT11_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT12_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT13_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT14_VALUE),
        STRING_TOKEN(STR_MEMORY_SIZE_SCKT1_SLOT15_VALUE)
    };

    //<LiuYB019-20170601 Add memory vendor and PN in setup +>
    CHAR8 *StrMemManuAscii = NULL;
    CHAR8 *StrMemPnAscii   = NULL;
    CHAR16 StrMemManuUnicode[0x80];
    CHAR16 *StrMemManuTemp = NULL;
    CHAR16 StrMemPnUnicode[0x80];
    //<LiuYB019-20170601 Add memory vendor and PN in setup ->
    
    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup +>
    CHAR8 *StrMemSnAscii = NULL;
    CHAR16 StrMemSnUnicode[0x80];
    CHAR8 *StrMemDramWidthAscii = NULL;
    CHAR16 StrMemDramWidthUnicode[0x40];
    CHAR8 *StrMemEccCapbilityAscii = NULL;
    CHAR16 StrMemEccCapbilityUnicode[0x40];
    CHAR8 *StrMemModuleTypeAscii = NULL;
    CHAR16 StrMemModuleTypeUnicode[0x40];
    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup ->
    UINTN                       NumberOfInstalledProcessors;
    UINTN                       NumberOfDie;
    UINT32                      DieNumberOfPerSocket;
    UINTN                       NumberOfChannel;
    UINTN                       IgnoredRootBridges;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    
    //<ChengXW001-20171104 Record a SEL if memory training failed while post +>
    EFI_IPMI_TRANSPORT          *gIpmiTransport = NULL;
    IPMI_SEL_EVENT_RECORD_DATA  SelRecord;
    UINT8                       ResponseData[3];
    UINT8                       ResponseDataSize;
    
    // Locate EfiDxeIpmiTransportProtocol
    Status = pBS->LocateProtocol ( &gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&gIpmiTransport );
    //<ChengXW001-20171104 Record a SEL if memory training failed while post ->
    
    // Locate FabricTopologyServicesProtocol
    Status = pBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);

    if (!Enumerate)
    {
        Tom2 = Shr64 (GetTom2(), 20);
        Tom1 = Shr64 (GetTom(), 20);
        SystemRam = (Tom2) ? (Tom2 - 0x1000) : 0;
        SystemRam += Tom1;
        Enumerate = TRUE;
    }


    // Get DIMM information from Smbios protocol
    Status = pBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &EfiSmbios);
    if (EFI_ERROR (Status))
    {
        DEBUG ((DEBUG_ERROR , "InitNBStrings: pBS->LocateProtocol(EfiSmbiosProtocol... ERROR\n"));
        return;
    }

    SmbiosHandle = 0xFFFE;
    SmbiosType = 17;
    TypeDataSize = 0;
    Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_PSO_DATA, 0, NULL, &TypeDataSize);
      
    Status = pBS->AllocatePool(EfiBootServicesData, TypeDataSize, &PlatformMemoryConfiguration);   
    if (EFI_ERROR(Status)) return;

    pBS->SetMem(PlatformMemoryConfiguration, TypeDataSize, 0);
    Status = ApcbGetType (APCB_GROUP_MEMORY , APCB_MEM_TYPE_PSO_DATA, 0, PlatformMemoryConfiguration, &TypeDataSize);

    // Get ApcbToBoard channel mapping table
    ApcbToBoardChanXLatTab = (HOST_TO_APCB_CHANNEL_XLAT*)PcdGetPtr(PcdApcbToBoardChanXLatTab);
    
    // Calculate die number and channel number 
    DieNumberOfPerSocket = (UINT32)(NumberOfDie / NumberOfInstalledProcessors);
    NumberOfChannel = MAX_CHANNELS_PER_DIE * 2;
    
    for (CurSocket = 0; CurSocket < NumberOfInstalledProcessors; CurSocket++)
    {   
        DimmStringCurNum = 0;
        for (CurChannel = 0; CurChannel < NumberOfChannel; CurChannel++) {
            DEBUG ((DEBUG_INFO, "CurSocket= %lx CurChannel= %lx\n", CurSocket, CurChannel));
            Index = 0;
            for (CurDimm = 0; CurDimm < DIMM_SLOTS_PER_CHNL; CurDimm++)
            {
                DEBUG ((DEBUG_INFO, "InitNBStrings: Physical channel = %d, CurDimm = %lx \n", CurChannel, CurDimm));
                Status = EfiSmbios->GetNext(EfiSmbios, &SmbiosHandle, &SmbiosType, &SmbiosTable, NULL);
                if (Status == EFI_SUCCESS)
                {
                    Type17 = (SMBIOS_MEMORY_DEVICE_INFO*)SmbiosTable;
                    DEBUG ((DEBUG_INFO, "InitNBStrings: Type17 size = %lx \n", Type17->Size));
                    Type17Speed = (UINT32)(Type17->Speed);

                    Type17Size = (UINT32)(Type17->Size);
                    if (Type17Size==0x7FFF)
                    {
                        Type17Size = Type17->ExtendedSize;
                    }
                    //<LiuYB019-20170601 Add memory vendor and PN in setup +>
                    if (Type17->Manufacturer != 0) 
                    {
                        GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->Manufacturer, (UINT8 **)&StrMemManuAscii);
                        Ascii2Unicode(StrMemManuUnicode, StrMemManuAscii);

                        // only reserved first vendor word, such as "Micro Technology -> Micro"
                        StrMemManuTemp = StrMemManuUnicode;

                        while (TRUE) 
                        {
                            if (*StrMemManuTemp == L' ') {
                                *StrMemManuTemp = L'\0';
                            }
                            if (*StrMemManuTemp == L'\0') {
                                break;
                            }
                            StrMemManuTemp++;
                        }
                    } else {
                        StrMemManuUnicode[0] = L'\0';
                    }

                    if (Type17->PartNumber != 0) {
                        GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->PartNumber, (UINT8 **)&StrMemPnAscii);
                        Ascii2Unicode(StrMemPnUnicode, StrMemPnAscii);
                    } else {
                        StrMemPnUnicode[0] = L'\0';
                    }
                    //<LiuYB019-20170601 Add memory vendor and PN in setup ->
                    GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->BankLocator,
                                             (UINT8 **)&StrAsciiTmpPtr);
                    Ascii2Unicode(BankLocatorString, StrAsciiTmpPtr); 
                    
                    //***  Calculate which Die the channel is on. start ***// 
                    while (ApcbToBoardChanXLatTab[Index].TranslatedChannelId != 0xFF) { 
                        if ((BankLocatorString[StrLen(BankLocatorString)-1] - 'A') == ApcbToBoardChanXLatTab[Index].TranslatedChannelId) {
                            HostChannel = Index;
                            DieNum = CurSocket * DieNumberOfPerSocket + HostChannel / MAX_CHANNELS_PER_DIE;
                            Address = ((HostChannel % 2) == 0) ? 0x50104 : 0x150104;
                            UMCNum = ((HostChannel % 2) == 0) ? 0 : 1; //<Kangmm-20190517 Modify OemData Follow Oem SEL spec->  
                            ValidChannel = TRUE;
                            break;
                        }
                        Index++;
                        ValidChannel = FALSE;
                    }
                    if (!ValidChannel) {
                        DEBUG ((DEBUG_INFO, "Error Channel %c\n", BankLocatorString[StrLen(BankLocatorString)-1]));
                        ASSERT(FALSE);
                    }
                    //***  Calculate which Die the channel is on. end ***//
                    
                    //*** Get Channel training status start ***//
                    GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->DeviceLocator,
                                                              (UINT8 **)&DevLocStrAsciiTmpPtr);                 
                    Ascii2Unicode(DeviceLocatorString, DevLocStrAsciiTmpPtr); 
                    
                    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup +>
                    if (Type17->SerialNumber != 0) {
                      GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->SerialNumber, (UINT8 **)&StrMemSnAscii);
                      Ascii2Unicode(StrMemSnUnicode, StrMemSnAscii);
                    } else {
                      StrMemSnUnicode[0] = L'\0';
                    }
                    //<Kangmm001-20190516 Regression issue caused by Mantis0028709 +>
                    EccCapbility = ((Type17->TotalWidth - Type17->DataWidth) & BIT3 )? 2 : 1;
                    //<Kangmm001-20190516 Regression issue caused by Mantis0028709 ->
                    ModuleType = PcdGet8(PcdSystemModuleType);
                    RankNum = Type17->Attributes;
                    DramWidth = (PcdGet8(PcdSystemDramWidth)+1); //For spd byte12  Bit [2, 1, 0] :
                                                                 //000 = 4 bits
                                                                 //001 = 8 bits
                                                                 //010 = 16 bits
                                                                 //011 = 32 bits
                                                                 //But Oem define:
                                                                 //  UINT8 DramWidth;
                                                                 //  1: x4
                                                                 //  2: x8
                                                                 //  3: x16
                                                                 //  4: x32
                    if ((EccCapbility == 1) || (EccCapbility == 2)){
                      switch (EccCapbility){
                        case 0x01:
                          StrMemEccCapbilityAscii = "Non-ECC";
                          break;
                        case 0x02:
                          StrMemEccCapbilityAscii = "ECC";
                          break;
                        default:
                          break;
                      }
                      Ascii2Unicode(StrMemEccCapbilityUnicode, StrMemEccCapbilityAscii);
                    } else {
                      StrMemEccCapbilityUnicode[0] = L'\0';
                    }
                    if (ModuleType != 0){
                      switch (ModuleType) {
                        case 0x01:
                          StrMemModuleTypeAscii = "RDIMM";
                          break;
                        case 0x02:
                          StrMemModuleTypeAscii = "UDIMM";
                          break;
                        case 0x03:
                          StrMemModuleTypeAscii = "SO-DIMM";
                          break;
                        case 0x04:
                          StrMemModuleTypeAscii = "LRDIMM";
                          break;
                        case 0x05:
                          StrMemModuleTypeAscii = "Mini-RDIMM";
                          break;
                        case 0x06:
                          StrMemModuleTypeAscii = "Mini-UDIMM";
                          break;
                        case 0x07:
                          StrMemModuleTypeAscii = "Reserved";
                          break;
                        case 0x08:
                          StrMemModuleTypeAscii = "72b-SO-RDIMM";
                          break;
                        case 0x09:
                          StrMemModuleTypeAscii = "72b-SO-UDIMM";
                          break;
                        case 0x0A:
                          StrMemModuleTypeAscii = "Reserved";
                          break;
                        case 0x0B:
                          StrMemModuleTypeAscii = "Reserved";
                          break;
                        case 0x0C:
                          StrMemModuleTypeAscii = "16b-SO-DIMM";
                          break;
                        case 0x0D:
                          StrMemModuleTypeAscii = "32b-SO-DIMM";
                          break;
                        case 0x0E:
                          StrMemModuleTypeAscii = "Reserved";
                          break;
                        case 0x0F:
                          StrMemModuleTypeAscii = "Reserved";
                          break;
                        default:
                          break;
                      }
                      Ascii2Unicode(StrMemModuleTypeUnicode, StrMemModuleTypeAscii);   
                    } else {
                      StrMemModuleTypeUnicode[0] = L'\0';
                    }
                    if (DramWidth != 0){
                      switch (DramWidth) {
                        case 0x01:
                          StrMemDramWidthAscii = "x4";
                          break;
                        case 0x02:
                          StrMemDramWidthAscii = "x8";
                          break;
                        case 0x03:
                          StrMemDramWidthAscii = "x16";
                          break;
                        case 0x04:
                          StrMemDramWidthAscii = "x32";
                          break;
                        default:
                          break;
                      }
                      Ascii2Unicode(StrMemDramWidthUnicode, StrMemDramWidthAscii);    
                    } else {
                      StrMemDramWidthUnicode[0] = L'\0'; 
                    }
                    //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup ->
                    NbSmnRead(DieNum, Address, &IsTrained);
                    IsTrained = IsTrained & 0x80000000;
                    //*** Get Channel training status end ***//
                    if(IsTrained){
                        GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->Manufacturer,
                                                 (UINT8 **)&StrAsciiTmpPtr);              
                        if(AsciiStrCmp(StrAsciiTmpPtr,UnknownString)){ // Check to see if any DIMM is installed
                            //InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                            //      L"%s: Size %d MB, Speed %d MT/s", 
                            //      DeviceLocatorString , Type17Size, Type17->ConfMemClkSpeed);
                            //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup +>
                            InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                                             L"%s: %s  %dGB  %dMT/s  %s  SN-%s  %dR%s  %s  %s", 
                                             DeviceLocatorString,
                                             StrMemManuUnicode, Type17Size >> 10, Type17->ConfMemClkSpeed, StrMemPnUnicode,StrMemSnUnicode,RankNum,StrMemDramWidthUnicode,StrMemEccCapbilityUnicode,StrMemModuleTypeUnicode);
                            //<Lvshh001-20181114 Add memory SN,Rank,DramWidth,EccCapbility,ModuleType in setup ->
                            DimmStringCurNum = DimmStringCurNum + 1;                            
                        } else if(Type17Size && Type17->ConfMemClkSpeed) {
                            //<hujf1001-20181030 Add more memory info under setup +>
                            //InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                            //          L"%s: (Unknown RAM)Size %d MB, Speed %d MT/s", 
                            //          DeviceLocatorString , Type17Size, Type17->ConfMemClkSpeed);        
                            InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                                        L"%s: (Unknown RAM) %dGB  %d MT/s", 
                                        DeviceLocatorString , Type17Size >> 10, Type17->ConfMemClkSpeed);   
                            //<hujf1001-20181030 Add more memory info under setup ->
                            DimmStringCurNum = DimmStringCurNum + 1;
                        } else {
                            InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                                        L"%s: Not Present",
                                        DeviceLocatorString);         
                                        DimmStringCurNum = DimmStringCurNum + 1;
                        }
                    }                       
                    else{
                        GetSmbiosStringByNumber((UINT8 *)SmbiosTable, Type17->Manufacturer,
                                                 (UINT8 **)&StrAsciiTmpPtr);              
                        if(AsciiStrCmp(StrAsciiTmpPtr,UnknownString)){ // Check to see if any DIMM is installed, but not trained
                            InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                                        L"%s: Present but not trained",
                                        DeviceLocatorString);
            //dengyh-20220403-show dimm error info on logo +
            Swprintf(TempStr,L"%s: Present but not trained",DeviceLocatorString);
            //pST->ConOut->Reset(pST->ConOut, TRUE);
            pST->ConOut->SetCursorPosition(pST->ConOut, DimmErrorInfoCol, DimmErrorInfoRow);
            // Get Image Device Path Str
            pST->ConOut->OutputString(pST->ConOut, TempStr);
            DimmErrorInfoRow++;
            //dengyh-20220403-show dimm error info on logo +
                            DimmStringCurNum = DimmStringCurNum + 1;
                            StringNumber = 3; //String for Manufacturer to set to "Unknown" if not trained                        
                            Status = EfiSmbios->UpdateString(EfiSmbios, &SmbiosHandle, &StringNumber, UnknownString ) ;
                            //<ChengXW001-20171104 Record a SEL if memory training failed while post +>
                            if (gIpmiTransport != NULL) 
                            {
                                SelRecord.TimeStamp = 0;
                                SelRecord.RecordType = IPMI_SEL_SYSTEM_RECORD;
                                SelRecord.GeneratorId = (UINT16) EFI_GENERATOR_ID (SMI_HANDLER_SOFTWARE_ID);
                                SelRecord.EvMRevision = IPMI_EVM_REVISION;
                                SelRecord.EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
                        
                                SelRecord.SensorType = 0x0C; // Memory
                                SelRecord.SensorNumber = 0x08; //<huangjin002-20190426 Change memory SensorNumber to 0x08 + > 
                        
                                SelRecord.OEMEvData1 = 0xA0 | 0x0B; //offset_MEM_MRC_training_warning

                                DimmNum = CurDimm;
                                if(DIMM_SLOTS_PER_CHNL == 2)
                                    DimmNum = (DimmNum == 0)?1:0;
                                SelRecord.OEMEvData2 = (DieNum << 0x03)|(UMCNum << 0x1);
                                SelRecord.OEMEvData3 = (CurSocket <<0x5) | (CurChannel <<0x2) | (DimmNum <<0x0);                            
                            
                                ResponseDataSize = sizeof (ResponseData);
                                Status = gIpmiTransport-> SendIpmiCommand (
                                         gIpmiTransport,
                                         IPMI_NETFN_STORAGE,
                                         BMC_LUN,
                                         IPMI_STORAGE_ADD_SEL_ENTRY,
                                         (UINT8 *) &SelRecord,
                                         sizeof (IPMI_SEL_EVENT_RECORD_DATA),
                                         (UINT8 *) &ResponseData,
                                         (UINT8 *) &ResponseDataSize 
                                         );
                            
                                if (EFI_ERROR (Status)) 
                                {
                                    DEBUG ((DEBUG_ERROR , "[OemDbg] IPMI_STORAGE_ADD_SEL_ENTRY Status:%r\n", Status));
                                }
                            }else{
                                DEBUG ((DEBUG_ERROR , "[OemDbg] LocateProtocol(EfiDxeIpmiTransportProtocol...) ERROR\n"));
                            }
                            //<ChengXW001-20171104 Record a SEL if memory training failed while post ->
                        } else {
                            InitString(HiiHandle, DimmString[DimmStringCurNum + (CurSocket * 16)],
                                                                L"%s: Not Present",
                                                                DeviceLocatorString);         
                            DimmStringCurNum = DimmStringCurNum + 1;
                        }
                    }
                    
                }
            }
        }
        
    }
    //<Wangyia001-20170413 make memory location information same as silk screen +>  
    /*if (Class == MAIN_FORM_SET_CLASS)
    {
        InitString (HiiHandle, STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
         L" %4d MB (DDR4)", SystemRam);
    }

    if (Class == CHIPSET_FORM_SET_CLASS)
    {
        InitString (HiiHandle, STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
         L"Total Memory: %4d MB (DDR4)", SystemRam);
    }*/

    if (Class == CHIPSET_FORM_SET_CLASS)
    {
        InitString (HiiHandle, STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
         L"Total Memory:%d GB (DDR4)", SystemRam >> 10);
    }
    //<Wangyia001-20170413 make memory location information same as silk screen -> 
    pBS->FreePool(PlatformMemoryConfiguration);
}

/**
    This function sets the Memory Clock speed
               
         

**/
EFI_STATUS SetMemClockCallBack(   
        IN EFI_HII_HANDLE HiiHandle, 
        IN UINT16 Class, 
        IN UINT16 SubClass, 
        IN UINT16 Key)
{
    
    EFI_STATUS                      Status = EFI_SUCCESS;
    SETUP_DATA                      SetupData;
    UINT64                          ConfigMemClockSpeed;
    UINTN                           VariableSize = sizeof(SETUP_DATA);
    UINT32                          TokenSize = sizeof(UINT32);
    UINT64                          UserTimingMode = TIMING_MODE_SPECIFIC;
    CALLBACK_PARAMETERS             *CallbackParameters;
    UINT64                          aDDR_Freq[]={DDR3200_FREQUENCY,DDR1333_FREQUENCY,DDR1600_FREQUENCY,DDR1866_FREQUENCY,DDR2133_FREQUENCY,DDR2400_FREQUENCY,DDR2666_FREQUENCY,DDR2933_FREQUENCY,DDR3200_FREQUENCY};
    
    UINT8                           Max_Supported_Freq=sizeof(aDDR_Freq)/sizeof(UINT64);

    if ( Key != MEM_CLOCK_KEY ) {
        return EFI_UNSUPPORTED;
    }
    
    CallbackParameters = GetCallbackParameters ();
    DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d CallbackParameters->Action:%x\n", __FUNCTION__, __LINE__,CallbackParameters->Action));
    
    if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
    {      
      return EFI_UNSUPPORTED;
    }
    
    /*if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED)
      return EFI_SUCCESS;*/
  
    Status = pRS->GetVariable( L"Setup", \
                                   &gEfiSetupGuid, \
                                   NULL, \
                                   &VariableSize, \
                                   &SetupData );
    if (EFI_ERROR(Status))
        return EFI_UNSUPPORTED;

   
    if (SetupData.MemClk == 0)
      UserTimingMode=TIMING_MODE_AUTO;
    
    if(SetupData.MemClk > (Max_Supported_Freq-1))
      ConfigMemClockSpeed=aDDR_Freq[0];
    ConfigMemClockSpeed=aDDR_Freq[SetupData.MemClk];
  
      
    DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d \n", __FUNCTION__, __LINE__));    
    AmdPspApcbSetCfgParameter(APCB_TOKEN_CONFIG_USERTIMINGMODE, &TokenSize, &UserTimingMode);
    AmdPspApcbSetCfgParameter(APCB_TOKEN_CONFIG_MEMCLOCKVALUE, &TokenSize, &ConfigMemClockSpeed);
    AmdPspWriteBackApcbShadowCopy();
    return  EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

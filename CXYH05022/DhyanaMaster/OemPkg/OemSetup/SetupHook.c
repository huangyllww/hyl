//**********************************************************************
// Name: SetupHook.c
//
// Description: OEM Information support in BIOS SETUP..
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <Library/PrintLib.h> //Machong 2016/2/1 Include HiiResource Relative Issue

#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Protocol/AmiCpuInfo.h>
//<lvych00120160912+>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/BaseLib.h>
//<lvych00120160912->
//<lvych00120160810+>
#include "PwdLib.h"
#include <TseSetupTransfer.h>
#include <Protocol/IPMISelfTestLogProtocol.h>

//<duanjun 20200514 mantis 0039358 bios password chinese language string show error+>
EFI_STRING_ID
EFIAPI
HiiSetString (
  IN EFI_HII_HANDLE    HiiHandle,
  IN EFI_STRING_ID     StringId,            OPTIONAL
  IN CONST EFI_STRING  String,
  IN CONST CHAR8       *SupportedLanguages  OPTIONAL
  )
;

EFI_STRING
EFIAPI
HiiGetString (
  IN EFI_HII_HANDLE  HiiHandle,
  IN EFI_STRING_ID   StringId,
  IN CONST CHAR8     *Language  OPTIONAL
  )
;
//<duanjun 20200514 mantis0039358 bios password chinese language string show error->
//<Chendl001-20180503 Benchmark Recommendation Setting in AMD Platform +>
#include "Library/Family/0x17/ZP/External/Resource/AmdCbsVariable.h"
//<Chendl001-20180503 Benchmark Recommendation Setting in AMD Platform ->

Tse_Setup_transfer *gTseSetuptransferInterface = NULL;
static UINT32 PasswordStatus = 0;
//<lvych00120160810->
//<lvych00120160912+>
#define PCIE_CFG_ADDR(bus,dev,func,reg) \
  ((VOID*) (UINTN)(0x80000000 + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

#define MAX_NEWSTRING_SIZE           0x200

//<Wangyia002-20170314 Setup Info display +>
// CPU
#define MSR_SYS_CFG     0x0C0010010
#define MSR_TOP_MEM     0x0C001001A
#define MSR_TOP_MEM2    0x0C001001D
//<Wangyia002-20170314 Setup info display ->

//<Wangyia002-20170412 Setup info display +>
#define AMD_MSR_MCODE_SIGNATURE     0x8b
#define AMD_CPUID_APICID_LPC_BID    0x00000001ul  // Local APIC ID, Logical Processor Count, Brand ID

/// CPUID data received registers format
typedef struct {
  OUT UINT32          EAX_Reg;                    ///< CPUID instruction result in EAX
  OUT UINT32          EBX_Reg;                    ///< CPUID instruction result in EBX
  OUT UINT32          ECX_Reg;                    ///< CPUID instruction result in ECX
  OUT UINT32          EDX_Reg;                    ///< CPUID instruction result in EDX
} CPUID_DATA;
//<Wangyia002-20170412 Setup info display +>

typedef struct {
  UINT32  Id;
  UINT32  Family;
  UINT32  Model;
  UINT32  Stepping;
  UINT32  Freq;
  UINT32  Ucode;
  UINT32  L1ICache;
  UINT32  L1DCache;
  UINT32  L2Cache;
  UINT32  L3Cache;
  CHAR16  *Version;
} CPU_SETUP_INFO;

typedef struct {
  UINT32   memStatus;
  UINT64   memSize;      //<Wangyia002-20170314 Setup info display ->
  UINT16   memFreq;                                              // in MHz
  UINT16   memRASMode;                                              
} MEM_SETUP_INFO;

typedef struct {
  CPU_SETUP_INFO        CpuSetupInfo[NSOCKETS];
  MEM_SETUP_INFO        MemSetupInfo;
} PLATFORM_SETUP_INFO;

typedef struct {
  UINT8   Type;
  UINT8   Length;
  UINT16  Handle;
} SMBIOS_HEADER;

static PLATFORM_SETUP_INFO               gPlatSetupInfo={0};

//<Wangyia002-20170314 Setup info display +>
UINT64 ReadMsr(UINT32 Msr);
VOID WriteMsr(UINT32 Msr, UINT64 Value);
UINT64 GetTom2(VOID);
UINT64 GetTom(VOID);
//<Wangyia002-20170314 Setup info display ->

//<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY + > 
#if defined(CPU_TDP_XGMI_GMI_FREQUENCY) && (CPU_TDP_XGMI_GMI_FREQUENCY == 1)
#include <Library/PciLib.h>
#define DIE_PER_SCKT 4
#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8
VOID
OemNbSmnRead (
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
#endif
//<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY - > 

EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart); // size includes null terminator
  // SMBIOS strings are NULL terminated, and end of all strings is indicated with NULL
  // loop until at end of all SMBIOS strings (found NULL terminator at string index past string's NULL), and Index != 0
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    return EFI_INVALID_PARAMETER;
  } else {
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SmbiosGetStructure (
  IN      UINT16  Handle,
  IN      EFI_SMBIOS_TYPE         Type,
  OUT     EFI_SMBIOS_TABLE_HEADER **Record
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_HEADER                     *SmbiosHeader;
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = 0;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &Type, Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    SmbiosHeader = (SMBIOS_HEADER *) *Record;
    if (SmbiosHeader->Handle == Handle) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


VOID
InitFruStrings(EFI_HII_HANDLE HiiHandle)
{
    CHAR16          *PcdDefaultString = L"To be filled by O.E.M.To be filled by O.E.M.To be filled by O.E.M.";
    CHAR16          *SdrDefaultString = L"Unknown";
    EFI_GUID        DefaultSystemUuid = {0x00};
    CHAR16          *UpdateStr;
    EFI_GUID        *Uuid;
    
  
    //
    // SystemManufacturer
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemManufacturer);
    if ( StrCmp (UpdateStr,PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrMfg);
    }
    DEBUG((EFI_D_ERROR, "SystemManufacturer: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_SYS_MANUFACTURER_VAL),
        L"%s",
        UpdateStr );

    //
    // SystemProductName
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemProductName);
    if ( StrCmp (UpdateStr, PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrProduct);
    }
    DEBUG((EFI_D_ERROR, "SystemProductName: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_SYS_PROD_NAME_VAL),
        L"%s",
        UpdateStr );


    //<ChengXW001-20170927 Update Smbios Type1 SKU number +>
    //
    // SystemProductPartNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemProductPartNumber);
    if ( StrCmp (UpdateStr, PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrProduct);
    }
    DEBUG((EFI_D_ERROR, "SystemProductPartNumber: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_SYS_PROD_NUM_VAL),
        L"%s",
        UpdateStr );
    //<ChengXW001-20170927 Update Smbios Type1 SKU number ->
    //
    // SystemSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSystemSerialNumber);
    DEBUG((EFI_D_ERROR, "SystemSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SYS_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardManufacturer
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardManufacturer);
    DEBUG((EFI_D_ERROR, "BaseBoardManufacturer: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_MANUFACTURER_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardProductName
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardProductName);
    DEBUG((EFI_D_ERROR, "BaseBoardProductName: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_PROD_NAME_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardVersion
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardVersion);
    DEBUG((EFI_D_ERROR, "BaseBoardVersion: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // BaseBoardSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdBaseBoardSerialNumber);
    DEBUG((EFI_D_ERROR, "BaseBoardSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_BRD_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // ChassisManufacturer
    //
    /*UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisManufacturer);
    if ( StrCmp (UpdateStr, PcdDefaultString) == 0x00 ) {
        UpdateStr = (CHAR16 *) PcdGetPtr (PcdStrMfg);
    }
    DEBUG((EFI_D_ERROR, "ChassisManufacturer: UpdateStr: %S\n", UpdateStr));
    InitString (
        HiiHandle,
        STRING_TOKEN(STR_CHA_MANUFACTURER_VAL),
        L"%s",
        UpdateStr );

    //
    // ChassisVersion
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisVersion);
    DEBUG((EFI_D_ERROR, "ChassisVersion: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_CHA_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // ChassisSerialNumber
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdChassisSerialNumber);
    DEBUG((EFI_D_ERROR, "ChassisSerialNumber: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, PcdDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_CHA_SERIAL_NUM_VAL),
            L"%s",
            UpdateStr );
    }*/

    //
    // SdrVersionNo
    //
    UpdateStr = (CHAR16 *) PcdGetPtr (PcdSdrVersionNo);
    DEBUG((EFI_D_ERROR, "SdrVersionNo: UpdateStr: %S\n", UpdateStr));
    if ( StrCmp (UpdateStr, SdrDefaultString) != 0x00 ) {
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SDR_VERSION_VAL),
            L"%s",
            UpdateStr );
    }

    //
    // System Unique ID
    //
    Uuid = (EFI_GUID *) PcdGetPtr (PcdSystemUuid);
    //<YinQiang001-20170421 Get System UUID +>
    //if (guidcmp ((CONST EFI_GUID*)Uuid, (CONST EFI_GUID*)&DefaultSystemUuid) == FALSE) {
        DEBUG((EFI_D_ERROR, "UUID:%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n", Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]));
        InitString (
            HiiHandle,
            STRING_TOKEN(STR_SYSTEM_UUID_VAL),
            L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            Uuid->Data1, Uuid->Data2, Uuid->Data3, Uuid->Data4[0], Uuid->Data4[1], Uuid->Data4[2], Uuid->Data4[3], Uuid->Data4[4], Uuid->Data4[5], Uuid->Data4[6], Uuid->Data4[7]);
    //}
    //<YinQiang001-20170421 Get System UUID ->
        
    DEBUG((EFI_D_ERROR, "InitFruStrings Exiting.....\n"));
    return ;
}

//<Wangyia002-20170412 Setup info display +>
VOID
UpdateCpuSetupInformation (
    IN EFI_HII_HANDLE       HiiHandle,
    IN PLATFORM_SETUP_INFO   *PlatSetupInfo
)
{
    EFI_STATUS                            Status;
    UINTN                                 StrSize;
    EFI_STRING_ID                         TokenToUpdate;
    CHAR16                                *NewString;
    CPUID_DATA                            CpuId;
    UINT8                                 ExtFamily;
    UINT8                                 BaseFamily;
    UINT8                                 ExtModel;
    UINT8                                 BaseModel;
    CPU_SETUP_INFO                        *CpuSetupInfo;
    AMI_CPU_INFO                          *GetCpuInfo = NULL;
    AMI_CPU_INFO_PROTOCOL                 *Cpu;
    INT8                                  i;
    EFI_GUID                              gAmiCpuInfoProtocolGuid = AMI_CPU_INFO_PROTOCOL_GUID;
    UINT8                                 NumPhysicalCores; //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup>

    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY + >    
    #if defined(CPU_TDP_XGMI_GMI_FREQUENCY) && (CPU_TDP_XGMI_GMI_FREQUENCY == 1)
    UINT32  Value1 = 0;
    UINT32  Value2 = 0;
    UINT32  TDP = 0;
    #endif
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY - > 
    
    CpuSetupInfo = &PlatSetupInfo->CpuSetupInfo[0];
    StrSize      = MAX_NEWSTRING_SIZE;
    NewString    = MallocZ (StrSize);
    gBS->SetMem(CpuSetupInfo, sizeof (CPU_SETUP_INFO) * NSOCKETS, 0);

    CpuSetupInfo[0].Ucode = (UINT32)ReadMsr(AMD_MSR_MCODE_SIGNATURE);

    AsmCpuid (
      AMD_CPUID_APICID_LPC_BID,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
    );
    
    ExtFamily = CpuId.EAX_Reg >> 20 & 0xFF;
    BaseFamily = CpuId.EAX_Reg >> 8 & 0xF;
    ExtModel = (UINT8)(CpuId.EAX_Reg >> 16 & 0xF);
    BaseModel = (UINT8)(CpuId.EAX_Reg >> 4 & 0xF);

    CpuSetupInfo[0].Model = CpuId.EAX_Reg >> 4 & 0xF;
    CpuSetupInfo[0].Family = BaseFamily + ExtFamily;
    CpuSetupInfo[0].Freq = CpuSetupInfo[0].Freq / 100;
    
    //<Chendl001-20170315 Correted CPU Stepping Info +>
    CpuSetupInfo[0].Stepping = CpuId.EAX_Reg &0x0f;
    //<Chendl001-20170315 Correted CPU Stepping Info ->
    
    Status = pBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
    if (EFI_ERROR(Status)) return;
    Cpu->GetCpuInfo(Cpu, 0, &GetCpuInfo);

    //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup +>
    if(GetCpuInfo->NumHts == 0)
      NumPhysicalCores = GetCpuInfo->NumCores;
    else
      NumPhysicalCores = GetCpuInfo->NumCores / 2;
    //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup ->

    Status = pBS->AllocatePool(EfiBootServicesData, 100, &CpuSetupInfo[0].Version);
    if (*GetCpuInfo->BrandString != 0) {
        AsciiStrToUnicodeStr(GetCpuInfo->BrandString,CpuSetupInfo[0].Version);
    }
    
    CpuSetupInfo[0].Version[39] = 0;
    // Remove the spaces from the end
    for (i = 39; i > 0; i-- ) {
      if (CpuSetupInfo[0].Version[i] != 0x20) { 
        break;
      }
      CpuSetupInfo[0].Version[i] = 0;
    }
    
    CpuSetupInfo[0].Freq = GetCpuInfo->ActualFreq;
    CpuSetupInfo[0].Freq = CpuSetupInfo[0].Freq / 100;
    
    for (i = 0; i < NUMBER_OF_CACHES; i++) {
      switch (GetCpuInfo->CacheInfo[i].Level) {   
        case EFI_CACHE_L1:
          if (GetCpuInfo->CacheInfo[i].Type == EfiCacheTypeData) {
            // L1 data cache is per core
            //CpuSetupInfo[0].L1DCache = GetCpuInfo->CacheInfo[i].Size;
            CpuSetupInfo[0].L1DCache = GetCpuInfo->CacheInfo[i].Size * NumPhysicalCores; //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup>
          } else {
            // L1 instruction cache is per core
            //CpuSetupInfo[0].L1ICache = GetCpuInfo->CacheInfo[i].Size;
            CpuSetupInfo[0].L1ICache = GetCpuInfo->CacheInfo[i].Size * NumPhysicalCores; //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup>
          }
          break;

        case EFI_CACHE_L2:
          // L2 cache is per core
          //CpuSetupInfo[0].L2Cache = GetCpuInfo->CacheInfo[i].Size;
          CpuSetupInfo[0].L2Cache = GetCpuInfo->CacheInfo[i].Size * NumPhysicalCores; //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup>
          break;
        case EFI_CACHE_L3:
          // L3 cache is shared by all cores      
            CpuSetupInfo[0].L3Cache = GetCpuInfo->CacheInfo[i].Size; //<YinQiang001-20170803 EIP350189:Correction CPU Cache Algorithm>        
      }

    }
    
    if (CpuSetupInfo[0].Version) {
      TokenToUpdate = STR_OEM_PROCESSOR_NAME_VALUE2;
      StrCpyS (NewString, MAX_NEWSTRING_SIZE/sizeof(CHAR16), CpuSetupInfo[0].Version);
      InitString(HiiHandle, TokenToUpdate, NewString + 23, NULL);
      TokenToUpdate = STR_OEM_PROCESSOR_NAME_VALUE;
      NewString[23] = L'\0';
      InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    }
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_FAMILY_VALUE;
    //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info +>
    //UnicodeSPrint (NewString,StrSize,L"%xh",(UINTN)CpuSetupInfo[0].Family);
    UnicodeSPrint (NewString, StrSize, L"%xh (Ext. %xh)", BaseFamily, ExtFamily);
    //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info ->
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_MODEL_VALUE;
    //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info +>
    //InitString(HiiHandle, TokenToUpdate, L"%x0h-%xFh", ExtModel, ExtModel); 
    UnicodeSPrint (NewString, StrSize, L"%xh (Ext. %xh)", BaseModel, ExtModel);  
    InitString(HiiHandle, TokenToUpdate, NewString, NULL); 
    //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info ->
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_STEPPING_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%x",(UINTN)CpuSetupInfo[0].Stepping);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);

    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_FREQUENCY_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%d.%dGHz",(UINTN) (CpuSetupInfo[0].Freq / 10),(UINTN) (CpuSetupInfo[0].Freq % 10));
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);

    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_MICROCODE_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%08xh",(UINTN)CpuSetupInfo[0].Ucode);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_L1_ICACHE_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%dKB",(UINTN)CpuSetupInfo[0].L1ICache);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_L1_DCACHE_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%dKB",(UINTN)CpuSetupInfo[0].L1DCache);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_L2_CACHE_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%dKB",(UINTN)CpuSetupInfo[0].L2Cache);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
    
    TokenToUpdate = (STRING_REF) STR_OEM_PROCESSOR_L3_CACHE_VALUE;
    //UnicodeSPrint (NewString,StrSize,L"%dMB",(UINTN)CpuSetupInfo[0].L3Cache);
    UnicodeSPrint (NewString,StrSize,L"%dKB",(UINTN)CpuSetupInfo[0].L3Cache * 1024); //<Yaosr001-20190420 revised CPU Cache info display in the BIOS Setup>
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
   
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY + >    
    #if defined(CPU_TDP_XGMI_GMI_FREQUENCY) && (CPU_TDP_XGMI_GMI_FREQUENCY == 1)    
    OemNbSmnRead(0, 0x5d374, &Value1);
    OemNbSmnRead(0, 0x5d378, &Value2);
    TDP = ((Value2&0x00000001)<<7)|((Value1&0xFE000000)>>25);
        
    TokenToUpdate = (STRING_REF) STR_PROCESSOR_TDP_VALUE;
    UnicodeSPrint (NewString,StrSize,L"%dW",TDP);
    InitString(HiiHandle, TokenToUpdate, NewString, NULL);
        
    TRACE((-1, "0x5d378 =0x%x,0x5d374 =0x%x,TDP = %d\n", Value2,Value1,TDP));
    #endif
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY - > 
    
    if (NewString) {
      gBS->FreePool (NewString);
    }
    if (CpuSetupInfo[0].Version) {
      gBS->FreePool (CpuSetupInfo[0].Version);
    }
    return;
}
//<Wangyia002-20170412 Setup info display ->

VOID
UpdateMEMSetupInformation (
    IN EFI_HII_HANDLE       HiiHandle,
    IN PLATFORM_SETUP_INFO   *PlatSetupInfo
)
{
    //<Wangyia002-20170314 Setup info display +>
    EFI_STATUS                               Status;
    MEM_SETUP_INFO                           *MemSetupInfo;
    CHAR16                                   *NewString;    
    UINTN                                    StrSize;
    EFI_PHYSICAL_ADDRESS                     Tom1;
    EFI_PHYSICAL_ADDRESS                     Tom2;
    EFI_PHYSICAL_ADDRESS                     SystemRam = 0;
    UINT32                                   MemoryFreq = 0;
    UINT32                                   MemSpeed = 0;
    UINT32                                   i;
    SMBIOS_TABLE_TYPE17                      *SmbiosType17Record;
    EFI_SMBIOS_PROTOCOL                      *Smbios;
    EFI_SMBIOS_HANDLE                        SmbiosHandle;
    EFI_SMBIOS_TYPE                          SmbiosType;
    EFI_SMBIOS_TABLE_HEADER                  *SmbiosRecord;
  
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY + >    
    #if defined(CPU_TDP_XGMI_GMI_FREQUENCY) && (CPU_TDP_XGMI_GMI_FREQUENCY == 1)
    UINT32  XGMI_GMI_Freguency = 0;
    #endif
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY - > 
    
    StrSize      = MAX_NEWSTRING_SIZE;
    NewString    = MallocZ (StrSize);
    
    MemSetupInfo = &PlatSetupInfo->MemSetupInfo;
    
    MemSetupInfo->memStatus = 0;
  
  
    Tom2 = Shr64 (GetTom2(), 20);
    Tom1 = Shr64 (GetTom(), 20);
    SystemRam = (Tom2) ? (Tom2 - 0x1000) : 0;
    SystemRam += Tom1;
    
    MemSetupInfo->memSize = SystemRam >> 10; //GB
    
    Status = pBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
    TRACE((-1, "GetMemoryInfo gEfiSmbiosProtocolGuid Status: %r\n", Status));
  
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  
    for (i = 0; ; ++i) {
      Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
      if (EFI_ERROR(Status)) {
        break;
      }
      TRACE((-1, "SmbiosRecord Status: %r\n", Status));
      SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosRecord;
      
      //<Wangyia001-20170330 revice Post info and some info in Main Screen +>
      //MemSpeed = SmbiosType17Record->Speed;
      MemSpeed = SmbiosType17Record->ConfiguredMemoryClockSpeed;
      //<Wangyia001-20170330 revice Post info and some info in Main Screen ->

      if (MemSpeed != 0) {
      	MemoryFreq = MemSpeed;
        TRACE((-1, "Freq = %d\n", MemoryFreq));
      }
    }
  
    MemSetupInfo->memFreq = MemoryFreq;
    
    InitString(HiiHandle, STRING_TOKEN(STR_MEMORY_SIZE_STRING_VALUE),L"%dGB",MemSetupInfo->memSize);
    InitString(HiiHandle, STRING_TOKEN(STR_MEMORY_FREQUENCY_STRING_VALUE),L"%dMT/s",MemSetupInfo->memFreq);

    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY + >
    #if defined(CPU_TDP_XGMI_GMI_FREQUENCY) && (CPU_TDP_XGMI_GMI_FREQUENCY == 1)
        XGMI_GMI_Freguency = MemSetupInfo->memFreq *4;
        #if defined(NSOCKETS) && (NSOCKETS > 1)
        InitString(HiiHandle, STRING_TOKEN(STR_PROCESSOR_XGMI_GMI_FREQUENCY_VALUE),L"%d.%dGT/s",(UINTN) (XGMI_GMI_Freguency/1000),(UINTN) ((XGMI_GMI_Freguency%1000/10+5)/10));
        #else
        InitString(HiiHandle, STRING_TOKEN(STR_PROCESSOR_GMI_FREQUENCY_VALUE),L"%d.%dGT/s",(UINTN) (XGMI_GMI_Freguency/1000),(UINTN) ((XGMI_GMI_Freguency%1000/10+5)/10));
        #endif
    #endif 
    //<zhaoyf-20190617-Add CPU  TDP and XGMI/GMI_FREQUENCY - >
        
    if(NewString)gBS->FreePool (NewString);
    //<Wangyia002-20170314 Setup info display ->

    return;
}

VOID
UpdateSystemSetupInformation (
    IN EFI_HII_HANDLE       HiiHandle,
    IN PLATFORM_SETUP_INFO   *PlatSetupInfo
)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    UINTN                           StrSize;
    EFI_BMC_SELF_TEST_LOG_PROTOCOL  *BmcSelfTestProtocol = NULL;
    CHAR16                          *StrBuf = NULL;
    //<hujf1001-20200613 Use PcdAmdPackageString to update AGESA PI version +>
    UINTN                           SigStringSize;
    CONST CHAR8*                    AmdVersionStringSig;
    CHAR16                          AmdVersionString[30] = {0};
    //<hujf1001-20200613 Use PcdAmdPackageString to update AGESA PI version ->
    StrSize = MAX_NEWSTRING_SIZE;


  #if defined(BASEBOARD_TYPE)&& (BASEBOARD_TYPE == 1) //<Lvych20160310001+>
    InitString(HiiHandle, STRING_TOKEN(STR_BOARD_NAME_EX_VALUE),L"%s",CONVERT_TO_WSTRING(BASE_BOARD_PRODUCT_NAME));
  #else
    InitString(HiiHandle, STRING_TOKEN(STR_BOARD_NAME_VALUE),L"%s",CONVERT_TO_WSTRING(BASE_BOARD_PRODUCT_NAME));
  #endif
    StrBuf = CONVERT_TO_WSTRING(BASE_BOARD_MANUFACTURER);
    //update system Platform info 
    InitString(HiiHandle, STRING_TOKEN(STR_PLATFORM_STRING_VALUE),L"%s",StrBuf);
    //<hujf1001-20200613 Use PcdAmdPackageString to update AGESA PI version +>
    AmdVersionStringSig = PcdGetPtr (PcdAmdPackageString);
    SigStringSize = AsciiStrSize (AmdVersionStringSig);
    AsciiStrToUnicodeStr ((AmdVersionStringSig + SigStringSize), AmdVersionString);
    InitString(HiiHandle, STRING_TOKEN(STR_PLATFORM_AGESA_STRING_VALUE),L"%s",AmdVersionString);
    //<hujf1001-20200613 Use PcdAmdPackageString to update AGESA PI version ->

    //
    // Locate the BMC self test protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiBmcSelfTestLogProtocolGuid,
                    NULL,
                    (VOID **)&BmcSelfTestProtocol );
    DEBUG((EFI_D_ERROR, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));    
    if ( (!EFI_ERROR (Status)) && BmcSelfTestProtocol->IsGetDeviceIdCommandPassed ) {
        //
        // BMC Firmware Revision
        //
        InitString (
                HiiHandle,
                STRING_TOKEN(STR_BMC_FW_VERSION_VALUE),
                L"%d.%02x", //liutz001-20191216 for ipmispec major need 10decode//<Kangmm001-20200428 Fix BMC minor version show incorrectly>
                BmcSelfTestProtocol->GetDeviceIdResponse.MajorFirmwareRev, BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev
                );
    }
    InitFruStrings(HiiHandle);
}


VOID
InitOEMTSEStrings(
    IN EFI_HII_HANDLE      HiiHandle,
    IN UINT16              Class
)
{

    DEBUG((EFI_D_INFO, "InitOEMTSEStrings.\n"));
//Init
    
    UpdateMEMSetupInformation(HiiHandle,&gPlatSetupInfo);
    UpdateCpuSetupInformation(HiiHandle,&gPlatSetupInfo);
    UpdateSystemSetupInformation(HiiHandle,&gPlatSetupInfo);

    return;
}
//<lvych00120160912->
//<AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure:    BenchmarkCallback
//
// Description:
//  Form Callback Function.Will be called every 100m Polling for data Updates.
//
// Input:
//  IN EFI_HII_HANDLE HiiHandle
//  IN UINT16 Class
//  IN UINT16 SubClass
//  IN UINT16 Key
//
// Output:    VOID
//
// Modified:  Nothing
//
// Referrals: None
//
// Note:
//-------------------------------------------------------------------------
//<AMI_PHDR_END>
//<Chendl001-20180503 Benchmark Recommendation Setting in AMD Platform +>
EFI_STATUS BenchmarkCallback(
IN EFI_HII_HANDLE               HiiHandle,
IN UINT16                       Class,
IN UINT16                       SubClass,
IN UINT16                       Key
)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_GUID                    SetupGuid = SETUP_GUID;
  UINTN                       sz;
  UINT32                      VarAttr;
  SETUP_DATA                  SetupData;
  CBS_CONFIG                  NvDataAmdSetup;
  UINTN                       VariableSize;
  UINT32                      VariableAttribute;

  DEBUG((EFI_D_INFO, "[OemDbg]: Benchmark Callback -- START... \n"));
  
  // Get "Setup" Variable 
  sz=sizeof(SETUP_DATA);
  Status = gST->RuntimeServices->GetVariable(
  	         L"Setup",
  	         &SetupGuid, 
  	         &VarAttr, 
  	         &sz, 
  	         &SetupData
  	         ); 
  if (EFI_ERROR(Status)) {
	DEBUG((EFI_D_INFO, "[OemDbg]: Get Setup Variable Fail ...\n"));
	DEBUG((EFI_D_INFO, "[OemDbg]: Benchmark Callback -- END... \n"));
    return Status;
  }
  
  // Get "AmdSetup" Variable
  Status = gST->RuntimeServices->GetVariable(
             L"AmdSetup",
             &gCbsSystemConfigurationGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataAmdSetup
             );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "[OemDbg]: Get CBS Setup Variable Fail ...\n"));
	DEBUG((EFI_D_INFO, "[OemDbg]: Benchmark Callback -- END... \n"));
    return Status;
  }

  // Bench Mark Setting
  if (SetupData.Benchmark == 0){ // Default
  	
    NvDataAmdSetup.CbsCmncTDPCtl = 0; //Auto
    
  } else if (SetupData.Benchmark == 1){ // Computing Throughput Mode

    NvDataAmdSetup.CbsCmncTDPCtl = 1;   //Manual
    NvDataAmdSetup.CbsCmncTDPLimit = 200;

  } else if (SetupData.Benchmark == 2){ //Computing Latency Mode

    NvDataAmdSetup.CbsCmncTDPCtl = 0;   //Auto

  } else if (SetupData.Benchmark == 3){ //Memory Bandwidth Mode
  
    NvDataAmdSetup.CbsCmncTDPCtl = 0;   //Auto

  } else if (SetupData.Benchmark == 4){ // Power Efficiency Mode
  
    NvDataAmdSetup.CbsCmncTDPCtl = 0;   //Auto

  } else if (SetupData.Benchmark == 5){  // Java Application Mode
  
    NvDataAmdSetup.CbsCmncTDPCtl = 1;    //Manual
	NvDataAmdSetup.CbsCmncTDPLimit = 200; // 200
  }

  // Save CBS Setup Variable
  Status = gST->RuntimeServices->SetVariable(
			     L"AmdSetup",
			     &gCbsSystemConfigurationGuid,
			     VariableAttribute,
			     sizeof(CBS_CONFIG),
			     &NvDataAmdSetup
			     );
  
  if (EFI_ERROR(Status)) {
    TRACE((-1, "[OemDbg]: Save CBS Setup Variable Fail ...\n"));
	DEBUG((EFI_D_INFO, "[OemDbg]: Benchmark Callback -- END... \n"));
    return Status;
  }

  DEBUG((EFI_D_INFO, "[OemDbg]: Benchmark Callback -- END... \n"));
  
  return EFI_SUCCESS;
}
//<Chendl001-20180503 Benchmark Recommendation Setting in AMD Platform ->
//<lvych00120160912->

//<lvych00120160810+>
EFI_STATUS	PasswordSetupCallback(
	IN	EFI_HII_HANDLE HiiHandle, 
	IN	UINT16	Class, 
	IN	UINT16	SubClass, 
	IN	UINT16	Key
)
{
	EFI_STATUS 	    Status;
	EFI_GUID gTseTransferGuid = TSE_TRANSFER_GUID;
	EFI_GUID SetupGuid = AMITSESETUP_GUID;
	AMITSESETUP	*AMITSEData;
	UINT32	i,PasswordInstalled = 	0;
	UINTN		AMITSEDataSize;
	//<xiaozhch-20220330 Hide password complexity menu when password is installed+>
    EFI_GUID	gSetupGuid = SETUP_GUID;
    SETUP_DATA	pSetup;
    SETUP_DATA	*PasswordStautus = &pSetup;
    UINTN       VariableSize = sizeof(SETUP_DATA);
    //<xiaozhch-20220330 Hide password complexity menu when password is installed->
	//<duanjun 20200514 mantis0039358 bios password chinese language string show error+>
	CHAR16      *StrBuff = NULL;
//<duanjun 20200514 mantis0039358 bios password chinese languare string show error->	
	
	if(gTseSetuptransferInterface == NULL){
	       Status = gBS->LocateProtocol(&gTseTransferGuid, NULL, &gTseSetuptransferInterface);
	       //<zhanghy2001-20200423 if error,return nonzero value +>
	       if(EFI_ERROR(Status)) return Status;
	       //<zhanghy2001-20200423 if error,return nonzero value ->
	 }
    //DEBUG((EFI_D_INFO, "Password Setup Callback.\n"));

	gTseSetuptransferInterface->VarGetBuffer(L"AMITSESetup",SetupGuid,(UINT8**)&AMITSEData,&AMITSEDataSize);
//<xiaozhch-20220330 Hide password complexity menu when password is installed+>
	Status = HiiLibGetBrowserData(&VariableSize, PasswordStautus, &gSetupGuid, L"Setup");
//<xiaozhch-20220330 Hide password complexity menu when password is installed->
	for(i=0;i<SETUP_PASSWORD_LENGTH;i++){
		if(AMITSEData->AdminPassword[i]  !=  L'\0'){
			PasswordInstalled |= AMI_PASSWORD_ADMIN;
			break;
		}
	}
	for(i=0;i<SETUP_PASSWORD_LENGTH;i++){
		if(AMITSEData->UserPassword[i]  !=  L'\0'){
			PasswordInstalled |= AMI_PASSWORD_USER;
			break;
		}
	}
	//<zhanghy2002-20200423 if password no change,return nonzero value,that means no need update +>
	if(PasswordStatus == PasswordInstalled) return EFI_NOT_FOUND;
	//<zhanghy2002-20200423 if password no change,return nonzero value,that means no need update ->
	PasswordStatus	=	PasswordInstalled;
//<duanjun 20200514 mantis0039358 bios password chinese language string show error+>
	DEBUG((EFI_D_INFO, "Password Setup Callback PasswordStatus:%x.\n",PasswordStatus));
    if( PasswordInstalled & AMI_PASSWORD_ADMIN ){
	    PasswordStautus->Adminpasswordinstall=1;//<xiaozhch-20220330 Hide password complexity menu when password is installed>
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_INSTALLED), "en-US");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_VALUE), StrBuff, "en-US");
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_INSTALLED), "zh-chs");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_VALUE), StrBuff, "zh-chs");
    }else{
	    PasswordStautus->Adminpasswordinstall=0;//<xiaozhch-20220330 Hide password complexity menu when password is installed>
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_NOT_INSTALLED), "en-US");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_VALUE), StrBuff, "en-US");
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_NOT_INSTALLED), "zh-chs");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_VALUE), StrBuff, "zh-chs");
    }

    if( PasswordInstalled & AMI_PASSWORD_USER ){        
	    PasswordStautus->Userpasswordinstall=1;//<xiaozhch-20220330 Hide password complexity menu when password is installed>       
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_INSTALLED), "en-US");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_VALUE), StrBuff, "en-US");
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_INSTALLED), "zh-chs");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_VALUE), StrBuff, "zh-chs");
    }else{     
	    PasswordStautus->Userpasswordinstall=0;//<xiaozhch-20220330 Hide password complexity menu when password is installed>   
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_NOT_INSTALLED), "en-US");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_VALUE), StrBuff, "en-US");
        StrBuff = HiiGetString (HiiHandle, STRING_TOKEN(STR_PASSWORD_VALUE_NOT_INSTALLED), "zh-chs");
        HiiSetString(HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_VALUE), StrBuff, "zh-chs");
    }   
//<duanjun 20200514 mantis0039358 bios password chinese language string show error->
//<xiaozhch-20220330 Hide password complexity menu when password is installed+>
	Status = HiiLibSetBrowserData(VariableSize, PasswordStautus, &gSetupGuid, L"Setup");
//<xiaozhch-20220330 Hide password complexity menu when password is installed->

	return EFI_SUCCESS;

}
//<lvych00120160810->

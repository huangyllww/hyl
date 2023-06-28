#include <Efi.h>
#include <UefiHii.h>
#include <AmiDxeLib.h>
#include <Setup.h> //AMI Variable
#include <Token.h> //Token

//<Wangyia001-20170314 Post info display  +>
#include <Uefi/UefiBaseType.h>
#include <CPU.h>
#include <AmiCpuPkg/CPU/Include/CpuCspLib.h>
//<Wangyia001-20170314 Post info display  ->

#include <Library/HobLib.h>
#include <Protocol/Smbios.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/AmiUsbController.h>
#include "IndustryStandard/SmBios.h" 
#include <Protocol/OemStorageInfoProtocol.h>

#include <Protocol/HiiString.h>
#include <Protocol/AMIPostMgr.h>
#include <Library/AmiPciBusLib.h>
//#include <Guid/MdeModuleHii.h>

//<Yaosr001-20180724 display BMC info +>
#include <Protocol/IpmiSelfTestLogProtocol.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Include/IpmiNetFnTransportDefinitions.h>
//<Yaosr001-20180724 display BMC info ->

//static EFI_GUID gEfiSetupVariableGuid = {0xec87d643, 0xeba4, 0x4bb5, {0xa1, 0xe5, 0x3f, 0x3e, 0x36, 0xb2, 0xd, 0xa9 }};

//<lvych00120160816+>
#define TSTR_SIZE 	0x200

#define MAX_DEVS_LINE_LENGTH 80
#define MAX_DEVICE_NUMBER_LENGTH 10
#define MAX_DEVICE_AMOUNT 127
#define MSR_TOP_MEM     0x0C001001A  //<Wangyia001-20170314 Post info display>


//#pragma pack(push, 1)
typedef struct {
  UINT8   Present;
  UINT8   SocketID;
  UINT32  Id;
  UINT32  Freq;
  CHAR16 *VersionStr;
  UINT16  CurrentCore;//<Yaosr001-20200511 Display CPU cores during the post.>
} CPU_POST_INFO;

typedef struct {
  UINT64   memSize;
  UINT32   memFreq;                                              // in MHz
  //UINT8    memMode;                                              // 0 - Independent, 1 - Lockstep
  UINT8    MemCount;//<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post.>
} MEM_POST_INFO;
//<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. +>
typedef struct {
  CHAR16   *MemManufacturer;
  CHAR16   *MemLocator;
  UINT16    Size;
  UINT8     EccCapbility;
} MEM_POST_EXTEND_INFO;
//<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. ->

typedef struct {
  CHAR16                DevName[MAX_DEVS_LINE_LENGTH];
} POST_DEV_DATA;

typedef struct {
      CHAR16            UsbDevNumStr[MAX_DEVS_LINE_LENGTH];
      CONNECTED_USB_DEVICES_NUM       DevNum;
  //Like T_ITEM_LIST
      UINTN             UsbDevInitCnt;
      UINTN             UsbDevCount;    //total devices port count
      POST_DEV_DATA     **UsbDevData;
} USB_POST_INFO;

typedef struct {
  //Like T_ITEM_LIST
      UINTN             StorageDevInitCnt;
      UINTN             StorageDevCount;    //total devices port count
      POST_DEV_DATA     **StorageDevData;
} STORAGE_POST_INFO;

//<Chendl001-20170615 Display Network Card Info +>
typedef struct {
  CHAR16                      *NetworkCardStr;
  UINT8                       OnBoardDev;
  //<YinQiang001-20170923 bios post network devices message +>
  UINT32                      Slot;
  UINT8                       Bus;            
  UINT8                       Device;
  //<YinQiang001-20170923 bios post network devices message ->
} NETWORK_POST_INFO;
//<Chendl001-20170615 Display Network Card Info ->

typedef struct {
  CPU_POST_INFO		      CpuPostInfo[NSOCKETS];
  MEM_POST_INFO		      MemPostInfo;                                              // in MHz
  MEM_POST_EXTEND_INFO        MemPostExtendInfo[32]; //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post.>
  USB_POST_INFO               UsbPostInfo;
  STORAGE_POST_INFO           StoragePostInfo;
  //<Chendl001-20170615 Display Network Card Info +>
  NETWORK_POST_INFO           NetWorkPostInfo[MaxNetworkNum]; 
  //<Chendl001-20170615 Display Network Card Info ->
} POST_INFO;
//#pragma pack(pop)

//<Wangyia001-20170314 Post info display +>
UINT64 ReadMsr(UINT32 Msr);
VOID WriteMsr(UINT32 Msr, UINT64 Value);
UINT64 GetTom2(VOID);
UINT64 GetTom(VOID);
//<Wangyia001-20170314 Post info display ->


VOID *
EfiLibAllocateZeroPool (
  IN  UINTN   AllocationSize
  );
/**

  Acquire the string associated with the Index from SMBIOS structure and return it.
  The caller is responsible for freeing the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.  

**/
EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  );

EFI_STATUS GetCpuInfo(VOID);

EFI_STATUS GetMemorySize(VOID);

//<Chendl001-20170615 Display Network Card Info +>
typedef struct _PCIDevInfo {
    UINT16                  VendorId;
    UINT16                  DeviceId;
    UINT16                  SubsystemID;
    CHAR16                  *BrandName;
} PCIDevInfo;
//<Chendl001-20170615 Display Network Card Info ->


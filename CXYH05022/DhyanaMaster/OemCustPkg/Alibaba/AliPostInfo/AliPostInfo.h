#include <Efi.h>
#include <UefiHii.h>
#include <AmiDxeLib.h>
#include <Setup.h> //AMI Variable
#include <Token.h> //Token

#include <Uefi/UefiBaseType.h>
#include <CPU.h>
#include <AmiCpuPkg/CPU/Include/CpuCspLib.h>


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

#include <Protocol/IpmiSelfTestLogProtocol.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Include/IpmiNetFnTransportDefinitions.h>


#define TSTR_SIZE 	0x200

#define MAX_DEVS_LINE_LENGTH 80
#define MAX_DEVICE_NUMBER_LENGTH 10
#define MAX_DEVICE_AMOUNT 127
#define MSR_TOP_MEM     0x0C001001A 


//#pragma pack(push, 1)
typedef struct {
  UINT8   Present;
  UINT8   SocketID;
  UINT32  Id;
  UINT32  Freq;
  CHAR16 *VersionStr;
  UINT16  CurrentCore;
} CPU_POST_INFO;

typedef struct {
  UINT64   memSize;
  UINT32   memFreq;                                              // in MHz
  //UINT8    memMode;                                              // 0 - Independent, 1 - Lockstep
  UINT8    MemCount;
} MEM_POST_INFO;

typedef struct {
  CHAR16   *MemManufacturer;
  CHAR16   *MemLocator;
  UINT16    Size;
  UINT8     EccCapbility;
} MEM_POST_EXTEND_INFO;


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

typedef struct {
  CHAR16                      *NetworkCardStr;
  UINT8                       OnBoardDev;
  UINT32                      Slot;
  UINT8                       Bus;            
  UINT8                       Device;
} NETWORK_POST_INFO;

typedef struct {
  CPU_POST_INFO		      CpuPostInfo[NSOCKETS];
  MEM_POST_INFO		      MemPostInfo;                                              // in MHz
  MEM_POST_EXTEND_INFO        MemPostExtendInfo[32]; 
  USB_POST_INFO               UsbPostInfo;
  STORAGE_POST_INFO           StoragePostInfo;
  NETWORK_POST_INFO           NetWorkPostInfo[MaxNetworkNum]; 
} POST_INFO;
//#pragma pack(pop)

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



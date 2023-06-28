#include "OemPCIeDev.h"
#include "Library/OemDXECommonLib.h"
#include "Library/DevicePathLib.h"
#include "Protocol/DevicePath.h"
#include "Efi.h"
#include "AmiProtocol.h"
#include "Library/DebugLib.h"

EFI_STATUS
GetNicMacByUefi(
    IN EFI_BOOT_SERVICES *BootService,
    IN EFI_HANDLE PciDevHandle,
    IN OUT UINT8 *NicMac)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN HandleCount, Index, Index1;
  EFI_HANDLE *HandleBuffer = NULL, NewHandle;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath = NULL, *TmpDp = NULL, *Dp = NULL, *DpPciNode = NULL;
  MAC_ADDR_DEVICE_PATH *MacDp = NULL;

  //
  // Find all MAC device path handle
  //
  Status = BootService->LocateHandleBuffer(
      ByProtocol,
      &gEfiDevicePathProtocolGuid,
      NULL,
      &HandleCount,
      &HandleBuffer);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++)
  {

    //Open the DevicePath
    Status = BootService->HandleProtocol(
        HandleBuffer[Index],
        &gEfiDevicePathProtocolGuid,
        (VOID *)&DevicePath);
    if (EFI_ERROR(Status))
      continue;

    //MAC_ADDR_DEVICE_PATH filter
    for (Dp = DevicePath; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp))
    {
      if (Dp->Type == MESSAGING_DEVICE_PATH && Dp->SubType == MSG_MAC_ADDR_DP)
      {
        break;
      }
    }
    if (isEndNode(Dp))
      continue;
    MacDp = (MAC_ADDR_DEVICE_PATH *)Dp;

    //Create the parent PCI DevicePath
    for (Dp = DevicePath; !(isEndNode(Dp)); Dp = NEXT_NODE(Dp))
    {
      if (Dp->Type == HARDWARE_DEVICE_PATH && Dp->SubType == HW_PCI_DP)
      {
        DpPciNode = Dp;
      }
    }
    if (DpPciNode == NULL)
      continue;
    TmpDp = DuplicateDevicePath(DevicePath);
    Dp = (EFI_DEVICE_PATH_PROTOCOL *)((UINTN)TmpDp + ((UINTN)NEXT_NODE(DpPciNode) - (UINTN)DevicePath));
    Dp->Type = END_DEVICE_PATH;
    Dp->SubType = END_ENTIRE_SUBTYPE;
    SET_NODE_LENGTH(Dp, sizeof(*Dp));
    Dp = TmpDp;

    Status = BootService->LocateDevicePath(&gEfiPciIoProtocolGuid,&Dp,&NewHandle);
    if (TmpDp)
    {
      BootService->FreePool(TmpDp);
      TmpDp = NULL;
    }
    if (EFI_ERROR(Status))
      continue;

    //Found the parent PCI device handle
    if (NewHandle == PciDevHandle)
    {

      for (Index1 = 0; Index1 < 6; Index1++)
      {
        NicMac[Index1] = MacDp->MacAddress.Addr[Index1];
      }
      DEBUG((EFI_D_INFO, " ***UEFI MAC is %x %x %x %x %x %x *** \n", NicMac[0], NicMac[1], NicMac[2], NicMac[3], NicMac[4], NicMac[5]));
      break;
    }
  }

  if (HandleBuffer != NULL)
  {
    BootService->FreePool(TmpDp);
  }

  if (Index == HandleCount)
  {
    return EFI_NOT_FOUND;
  }
  return Status;
}
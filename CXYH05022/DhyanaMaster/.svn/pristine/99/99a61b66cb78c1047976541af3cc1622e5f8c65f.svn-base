#include "Token.h"

#include "OemIpmiNetFnDefinitions.h"
#include "Library/PciLib.h"

#include "Library/PCIeRegAccessLib.h"
#include "Library/DebugLib.h"
/*----------------------------------------------------------------------------------------*/
/*
 * Check if device present
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @retval    TRUE            Device is present
 * @retval    FALSE           Device is not present
 */

BOOLEAN
OemPciIsDevicePresent(
    IN UINT32 Address)
{
  UINT32 DeviceId;
  DeviceId = PciRead32(Address);
  if (DeviceId == 0xffffffff)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Find PCI capability pointer
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @param[in] CapabilityId    PCI capability ID
 * @retval                    Register address of capability pointer
 *
 */

UINT8
OemFindPciCapability(
    IN UINT32 Address,
    IN UINT8 CapabilityId)
{
  UINT8 CapabilityPtr;
  UINT8 CurrentCapabilityId;
  CapabilityPtr = 0x34;
  if (!OemPciIsDevicePresent(Address))
  {
    return 0;
  }
  while (CapabilityPtr != 0)
  {
    CapabilityPtr = PciRead8(Address | CapabilityPtr);
    if (CapabilityPtr != 0)
    {
      CurrentCapabilityId = PciRead8(Address | CapabilityPtr);
      if (CurrentCapabilityId == CapabilityId)
      {
        break;
      }
      CapabilityPtr++;
    }
  }
  return CapabilityPtr;
}
/*----------------------------------------------------------------------------------------*/
/*
 * Check if device is PCIe device
 *
 *
 *
 * @param[in] Address         PCI address (as described in PCI_ADDR)
 * @retval    TRUE            Device is a PCIe device
 * @retval    FALSE           Device is not a PCIe device
 *
 */

BOOLEAN
OemPciIsPcieDevice(
    IN UINT32 Address)
{
  if (OemFindPciCapability(Address, PCIE_CAP_ID) != 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*----------------------------------------------------------------------------------------*/
/*
 * Find PCIe extended capability pointer
 *
 *
 *
 * @param[in] Address               PCI address (as described in PCI_ADDR)
 * @param[in] ExtendedCapabilityId  Extended PCIe capability ID
 * @retval                          Register address of extended capability pointer
 *
 */

UINT16
OemFindPcieExtendedCapability(
    IN UINT32 Address,
    IN UINT16 ExtendedCapabilityId)
{
  UINT16 CapabilityPtr;
  UINT32 ExtendedCapabilityIdBlock;
  if (OemPciIsPcieDevice(Address))
  {
    CapabilityPtr = 0x100;
    ExtendedCapabilityIdBlock = PciRead32(Address | CapabilityPtr);
    if ((ExtendedCapabilityIdBlock != 0) && ((UINT16)ExtendedCapabilityIdBlock != 0xffff))
    {
      do
      {
        ExtendedCapabilityIdBlock = PciRead32(Address | CapabilityPtr);
        DEBUG((EFI_D_ERROR, "   - Capability at 0x%x with type 0x%x\n", CapabilityPtr, (UINT16)ExtendedCapabilityIdBlock));
        if ((UINT16)ExtendedCapabilityIdBlock == ExtendedCapabilityId)
        {
          return CapabilityPtr;
        }
        CapabilityPtr = (UINT16)((ExtendedCapabilityIdBlock >> 20) & 0xfff);
      } while (CapabilityPtr != 0);
    }
  }
  return 0;
}

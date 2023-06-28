#ifndef __PCIE_REG_ACCESS_LIB_H__
#define __PCIE_REG_ACCESS_LIB_H__

#ifndef PCIE_EXT_AER_CAP_ID
#define PCIE_EXT_AER_CAP_ID 0x01
#endif

#ifndef PCIE_UNCORR_STATUS_PTR
#define PCIE_UNCORR_STATUS_PTR (0x04)
#endif

#ifndef PCIE_CORR_STATUS_PTR
#define PCIE_CORR_STATUS_PTR (0x10)
#endif

#ifndef PCIE_CAP_ID
#define PCIE_CAP_ID                 0x10
#endif

UINT16
OemFindPcieExtendedCapability(
    IN UINT32 Address,
    IN UINT16 ExtendedCapabilityId);

#endif
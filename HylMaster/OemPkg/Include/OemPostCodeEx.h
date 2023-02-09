//**********************************************************************
// $Header:OEM POST CODE EX DEFINE
//
// $Revision: 0.01
//
// $Date:  <licm-20200622>
//**********************************************************************
#ifndef _OEM_POST_CODE_EX_H_
#define _OEM_POST_CODE_EX_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>
#include <Efi.h>
#include <StatusCodes.h>

//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#define PROGRESS_CODE16_EX(Code,AddPoint)\
    LibReportStatusCode(EFI_PROGRESS_CODE|((AddPoint<<8)&0xff00), Code, 0, NULL, NULL)
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT -> 

#define PORT81_MASK_BIT                                   0xFF00

#define POST_EX_DXE_SEL_STARTED                           0x1060
#define POST_EX_DXE_PCI_ENUM_STARTED                      0x1292
#define POST_EX_DXE_PCI_REG_NOTIFY                        0x1392
#define POST_EX_DXE_PCI_FREE_RES                          0x1492
#define POST_EX_DXE_PCI_ALLOC_SETUPBUF                    0x1592
#define POST_EX_DXE_PCI_SCAN_ROOTBRIDGE                   0x1692
#define POST_EX_DXE_PCI_INIT_HP                           0x1792
#define POST_EX_DXE_PCI_INIT_EXPRESS                      0x1892
#define POST_EX_DXE_PCI_INIT_BAR                          0x1992
#define POST_EX_DXE_PCI_GET_OPROM                         0x1A92
#define POST_EX_DXE_PCI_REQUEST_RES_STARTED               0x1B92
#define POST_EX_DXE_PCI_REQUEST_RES_END                   0x1C92
#define POST_EX_DXE_PCI_ASSIGN_RES_STARTED                0x1D92
#define POST_EX_DXE_PCI_ASSIGN_RES_END                    0x1E92
#define POST_EX_DXE_PCI_UPDATE_DEVICES_LIST               0x1F92
#define POST_EX_DXE_EFI_OPROM_STARTED                     0x2092
#define POST_EX_DXE_EFI_OPROM_END                         0x2192
#define POST_EX_DXE_LEGACY_OPROM_STARTED                  0x2292
#define POST_EX_DXE_LEGACY_OPROM_END                      0x2392
#define POST_EX_DXE_CONTROLLER_DRIVER_STARTED             0x1098
#define POST_EX_DXE_CONTROLLER_DRIVER_END                 0x1198
#define POST_EX_DXE_USB_HOST_CON_INIT                     0x109A
#define POST_EX_DXE_USB_BUS_DRV_INIT                      0x119A
#define POST_EX_DXE_USB_DEV_DRV_INIT                      0x129A
#define POST_EX_DXE_REDFISH_INIT_SARTED                   0x3092
#define POST_EX_DXE_REDFISH_INIT_END                      0x3192
#define POST_EX_DXE_REDFISH_CALLBACK_SARTED               0x3292
#define POST_EX_DXE_REDFISH_CALLBACK_END                  0x3392
#define POST_EX_DXE_REDFISH_NET_STACK_INIT                0x3492
#define POST_EX_DXE_REDFISH_JSON_LIB_INIT                 0x3592
#define POST_EX_DXE_REDFISH_REG_DATA_CALLBACK             0x3692
#define POST_EX_DXE_REDFISH_REQUEST_BIOS_CONFIG           0x3792
#define POST_EX_DXEREDFISH_POST_BIOS_CONFIG               0x3892
#define POST_EX_DXESEL_POST_OK                            0xF0AD

//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT +>
#define EFI_IOB_EC_OEM_CPLD                                 0x0000000A
#define EFI_IOB_EC_OEM_DEBUG                                0x0000000B

#define PORT80_OEM_CPLD                                   (EFI_IO_BUS_PCI | EFI_IOB_EC_OEM_CPLD)
#define PORT80_OEM_DEBUG                                  (EFI_IO_BUS_PCI | EFI_IOB_EC_OEM_DEBUG)
//<licm-20210226 VGA and UART Display 16bit Port80 SUPPORT ->

#define PORT81_EX_DXE_SEL_STARTED                           ((POST_EX_DXE_SEL_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_ENUM_STARTED                      ((POST_EX_DXE_PCI_ENUM_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_REG_NOTIFY                        ((POST_EX_DXE_PCI_REG_NOTIFY&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_FREE_RES                          ((POST_EX_DXE_PCI_FREE_RES&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_ALLOC_SETUPBUF                    ((POST_EX_DXE_PCI_ALLOC_SETUPBUF&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_SCAN_ROOTBRIDGE                   ((POST_EX_DXE_PCI_SCAN_ROOTBRIDGE&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_INIT_HP                           ((POST_EX_DXE_PCI_INIT_HP&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_INIT_EXPRESS                      ((POST_EX_DXE_PCI_INIT_EXPRESS&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_INIT_BAR                          ((POST_EX_DXE_PCI_INIT_BAR&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_GET_OPROM                         ((POST_EX_DXE_PCI_GET_OPROM&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_REQUEST_RES_STARTED               ((POST_EX_DXE_PCI_REQUEST_RES_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_REQUEST_RES_END                   ((POST_EX_DXE_PCI_REQUEST_RES_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_ASSIGN_RES_STARTED                ((POST_EX_DXE_PCI_ASSIGN_RES_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_ASSIGN_RES_END                    ((POST_EX_DXE_PCI_ASSIGN_RES_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_PCI_UPDATE_DEVICES_LIST               ((POST_EX_DXE_PCI_UPDATE_DEVICES_LIST&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_EFI_OPROM_STARTED                     ((POST_EX_DXE_EFI_OPROM_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_EFI_OPROM_END                         ((POST_EX_DXE_EFI_OPROM_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_LEGACY_OPROM_STARTED                  ((POST_EX_DXE_LEGACY_OPROM_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_LEGACY_OPROM_END                      ((POST_EX_DXE_LEGACY_OPROM_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_CONTROLLER_DRIVER_STARTED             ((POST_EX_DXE_CONTROLLER_DRIVER_STARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_CONTROLLER_DRIVER_END                 ((POST_EX_DXE_CONTROLLER_DRIVER_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_USB_HOST_CON_INIT                     ((POST_EX_DXE_USB_HOST_CON_INIT&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_USB_BUS_DRV_INIT                      ((POST_EX_DXE_USB_BUS_DRV_INIT&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_USB_DEV_DRV_INIT                      ((POST_EX_DXE_USB_DEV_DRV_INIT&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_INIT_SARTED                   ((POST_EX_DXE_REDFISH_INIT_SARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_INIT_END                      ((POST_EX_DXE_REDFISH_INIT_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_CALLBACK_SARTED               ((POST_EX_DXE_REDFISH_CALLBACK_SARTED&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_CALLBACK_END                  ((POST_EX_DXE_REDFISH_CALLBACK_END&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_NET_STACK_INIT                ((POST_EX_DXE_REDFISH_NET_STACK_INIT&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_JSON_LIB_INIT                 ((POST_EX_DXE_REDFISH_JSON_LIB_INIT&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_REG_DATA_CALLBACK             ((POST_EX_DXE_REDFISH_REG_DATA_CALLBACK&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_REQUEST_BIOS_CONFIG           ((POST_EX_DXE_REDFISH_REQUEST_BIOS_CONFIG&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_REDFISH_POST_BIOS_CONFIG              ((POST_EX_DXEREDFISH_POST_BIOS_CONFIG&PORT81_MASK_BIT)>>8)
#define PORT81_EX_DXE_SEL_POST_OK                           ((POST_EX_DXESEL_POST_OK&PORT81_MASK_BIT)>>8)


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif


/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Pei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <AmdServerHotplug.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
//<duanjun 20200808 j27 support hotplug +>
#include <Setup.h>
//<duanjun 20200808 j27 support hotplug ->
/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//<duanjun-20200427-add function to query current cpu +>
BOOLEAN Is2diecpu()
{
    BOOLEAN Is2die = TRUE;
    UINT32  VidDid = 0xffffffff;
    VidDid = PciRead32(PCI_LIB_ADDRESS(0, 0x1a, 0, 0x00));
    
    if(VidDid != 0xffffffff) Is2die = FALSE;
    
    DEBUG((EFI_D_ERROR, "Is2diecpu is %d\n",Is2die));
    
    return Is2die;
}
//<duanjun-20200427-add function to query current cpu ->
//Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME >>> 
HOTPLUG_USERCONFIG_DESCRIPTOR HotplugUserDescriptor_2DIE_J34[] = {
     
// 2die -J34
        //J34 x4.1
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                48,                   // Start Lane
                51,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //   
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.2
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                52,                    // Start Lane
                55,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.3
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                56,                   // Start Lane
                59,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                60,                    // Start Lane
                63,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },
};

HOTPLUG_USERCONFIG_DESCRIPTOR HotplugUserDescriptor_4DIE_J34[] = {

// 4die -J34
        //J34 x4.1
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                80,                   // Start Lane
                83,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //   
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.2
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                84,                    // Start Lane
                87,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.3
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                88,                   // Start Lane
                91,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                92,                    // Start Lane
                95,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },
};
HOTPLUG_USERCONFIG_DESCRIPTOR HotplugUserDescriptor_2DIE[] = {
        //J27 x4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                4,                   // Start Lane
                7,                   // End Lane
                0,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

// 2die -J34
        //J34 x4.1
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                48,                   // Start Lane
                51,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //   
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.2
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                52,                    // Start Lane
                55,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.3
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                56,                   // Start Lane
                59,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                60,                    // Start Lane
                63,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },
};

HOTPLUG_USERCONFIG_DESCRIPTOR HotplugUserDescriptor_4DIE[] = {
        //J27 x4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                4,                   // Start Lane
                7,                   // End Lane
                0,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

// 4die -J34
        //J34 x4.1
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                80,                   // Start Lane
                83,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //   
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.2
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                84,                    // Start Lane
                87,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x20,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.3
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                88,                   // Start Lane
                91,                   // End Lane
                1,                    // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                Low8BitGpio,           // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },

        //J34 x4.4
        {
                //
                //StartLane, EndLane, SocketNumber
                //
                92,                    // Start Lane
                95,                    // End Lane
                1,                     // Socket Number

                //
                //HotplugType
                //
                HotplugExpressModuleB,

                //
                //Pca9545BusSegment, PcaChipSelect, I2CSlaveAddress, I2CGpioByteMapping, FunctionMask;
                //
                Pca9545BusSegmentNone, // Pca9545BusSegment :BusSegmentNone, BusSegment0/1/2/3...
                Pca9555,               // PcaChipSelect: Pca9539, Pca9555, Pca9506
                0x21,                  // I2CSlaveAddress: 0xXX
                High8BitGpio,          // I2CGpioByteMapping: Low8BitGpio / High8BitGpio
                0xFF                   // Function Mask
        },
};

//Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME <<<
STATIC NBIO_HOTPLUG_DESC_PROTOCOL mHotplugDescriptorProtocol = {
        AMD_NBIO_HOTPLUG_DESC_VERSION,  ///< revision
        NULL
};

void DumpHotplugDescriptor(HOTPLUG_DESCRIPTOR *HotplugDescriptor, UINTN Count)
{
    UINTN       i;

    for(i=0; i < Count; i++)
    {
        DEBUG((DEBUG_INFO, "[%d].Flags                            = %X\n", i, HotplugDescriptor[i].Flags));

        DEBUG((DEBUG_INFO, "[%d].Engine.StartLane                 = %X\n", i, HotplugDescriptor[i].Engine.StartLane));
        DEBUG((DEBUG_INFO, "[%d].Engine.EndLane                   = %X\n", i, HotplugDescriptor[i].Engine.EndLane));
        DEBUG((DEBUG_INFO, "[%d].Engine.SocketNumber              = %X\n", i, HotplugDescriptor[i].Engine.SocketNumber));

        DEBUG((DEBUG_INFO, "[%d].Mapping.HotPlugFormat            = %X\n", i, HotplugDescriptor[i].Mapping.HotPlugFormat));
        DEBUG((DEBUG_INFO, "[%d].Mapping.Revision                 = %X\n", i, HotplugDescriptor[i].Mapping.Revision));
        DEBUG((DEBUG_INFO, "[%d].Mapping.GpioDescriptorValid      = %X\n", i, HotplugDescriptor[i].Mapping.GpioDescriptorValid));
        DEBUG((DEBUG_INFO, "[%d].Mapping.ResetDescriptorValid     = %X\n", i, HotplugDescriptor[i].Mapping.ResetDescriptorValid));
        DEBUG((DEBUG_INFO, "[%d].Mapping.PortActive               = %X\n", i, HotplugDescriptor[i].Mapping.PortActive));
        DEBUG((DEBUG_INFO, "[%d].Mapping.MasterSlaveAPU           = %X\n", i, HotplugDescriptor[i].Mapping.MasterSlaveAPU));
        DEBUG((DEBUG_INFO, "[%d].Mapping.DieNumber                = %X\n", i, HotplugDescriptor[i].Mapping.DieNumber));
        DEBUG((DEBUG_INFO, "[%d].Mapping.ApertureID               = %X\n", i, HotplugDescriptor[i].Mapping.ApertureID));
        DEBUG((DEBUG_INFO, "[%d].Mapping.AlternateSlotNumber      = %X\n", i, HotplugDescriptor[i].Mapping.AlternateSlotNumber));
        DEBUG((DEBUG_INFO, "[%d].Mapping.PrimarySecondaryLink     = %X\n", i, HotplugDescriptor[i].Mapping.PrimarySecondaryLink));
        DEBUG((DEBUG_INFO, "[%d].Mapping.Reserved1                = %X\n", i, HotplugDescriptor[i].Mapping.Reserved1));

        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioBitSelector      = %X\n", i, HotplugDescriptor[i].Function.I2CGpioBitSelector));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioByteMapping      = %X\n", i, HotplugDescriptor[i].Function.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioDeviceMappingExt = %X\n", i, HotplugDescriptor[i].Function.I2CGpioDeviceMappingExt));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CGpioDeviceMapping    = %X\n", i, HotplugDescriptor[i].Function.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CDeviceType           = %X\n", i, HotplugDescriptor[i].Function.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CBusSegment           = %X\n", i, HotplugDescriptor[i].Function.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Function.FunctionMask            = %X\n", i, HotplugDescriptor[i].Function.FunctionMask));
        DEBUG((DEBUG_INFO, "[%d].Function.PortMapping             = %X\n", i, HotplugDescriptor[i].Function.PortMapping));
        DEBUG((DEBUG_INFO, "[%d].Function.Reserved1               = %X\n", i, HotplugDescriptor[i].Function.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Function.I2CBusSegmentExt        = %X\n", i, HotplugDescriptor[i].Function.I2CBusSegmentExt));

        DEBUG((DEBUG_INFO, "[%d].Reset.Reserved0                  = %X\n", i, HotplugDescriptor[i].Reset.Reserved0));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CGpioByteMapping         = %X\n", i, HotplugDescriptor[i].Reset.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Reset.Reserved1                  = %X\n", i, HotplugDescriptor[i].Reset.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CGpioDeviceMapping       = %X\n", i, HotplugDescriptor[i].Reset.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CDeviceType              = %X\n", i, HotplugDescriptor[i].Reset.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Reset.I2CBusSegment              = %X\n", i, HotplugDescriptor[i].Reset.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Reset.ResetSelect                = %X\n", i, HotplugDescriptor[i].Reset.ResetSelect));

        DEBUG((DEBUG_INFO, "[%d].Gpio.Reserved0                   = %X\n", i, HotplugDescriptor[i].Gpio.Reserved0));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CGpioByteMapping          = %X\n", i, HotplugDescriptor[i].Gpio.I2CGpioByteMapping));
        DEBUG((DEBUG_INFO, "[%d].Gpio.Reserved1                   = %X\n", i, HotplugDescriptor[i].Gpio.Reserved1));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CGpioDeviceMapping        = %X\n", i, HotplugDescriptor[i].Gpio.I2CGpioDeviceMapping));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CDeviceType               = %X\n", i, HotplugDescriptor[i].Gpio.I2CDeviceType));
        DEBUG((DEBUG_INFO, "[%d].Gpio.I2CBusSegment               = %X\n", i, HotplugDescriptor[i].Gpio.I2CBusSegment));
        DEBUG((DEBUG_INFO, "[%d].Gpio.GpioSelect                  = %X\n", i, HotplugDescriptor[i].Gpio.GpioSelect));
    }
}

EFI_STATUS SetNbioHotplugDescriptorProtocol(VOID)
{
    EFI_STATUS                       Status;
    HOTPLUG_DESCRIPTOR               *HotplugDescriptor;
    UINTN                            HotplugUserDescriptorCount=0;
    UINTN                            i;
    HOTPLUG_USERCONFIG_DESCRIPTOR    *pHotplugUserDescriptor = NULL;
    
    SETUP_DATA              SetupDataBuffer;
    UINTN                   SetupVariableSize = sizeof(SETUP_DATA);
    UINT32                  SetupVariableAttributes;
    EFI_GUID                gSetupGuid = SETUP_GUID;


    Status = gST->RuntimeServices->GetVariable (L"Setup",
                               &gSetupGuid,
                               &SetupVariableAttributes,
                               &SetupVariableSize,
                               &SetupDataBuffer);
//Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME >>> 
    if ( SetupDataBuffer.CPU0SataPcieSel ) { //J27 = nvme
        
        if (Is2diecpu()){      //2Die CPU
            HotplugUserDescriptorCount = sizeof(HotplugUserDescriptor_2DIE) / sizeof(HOTPLUG_USERCONFIG_DESCRIPTOR);
            pHotplugUserDescriptor = HotplugUserDescriptor_2DIE;
    }
        else                  //4die CPU
                        {   
            HotplugUserDescriptorCount = sizeof(HotplugUserDescriptor_4DIE) / sizeof(HOTPLUG_USERCONFIG_DESCRIPTOR);
            pHotplugUserDescriptor = HotplugUserDescriptor_4DIE;
    }
  }
    else{
        if (Is2diecpu()){      //2Die CPU
            HotplugUserDescriptorCount = sizeof(HotplugUserDescriptor_2DIE_J34) / sizeof(HOTPLUG_USERCONFIG_DESCRIPTOR);
            pHotplugUserDescriptor = HotplugUserDescriptor_2DIE_J34;
    }
        else                  //4die CPU
                        {   
            HotplugUserDescriptorCount = sizeof(HotplugUserDescriptor_4DIE_J34) / sizeof(HOTPLUG_USERCONFIG_DESCRIPTOR);
            pHotplugUserDescriptor = HotplugUserDescriptor_4DIE_J34;
    }
        
    }
  //Zhouyj1-20220521-Add AGPIO23 to decide whether it's PCIE or NVME <<<    

    Status = gBS->AllocatePool (
            EfiBootServicesData,
            HotplugUserDescriptorCount * sizeof(HOTPLUG_DESCRIPTOR),
            (VOID **) &HotplugDescriptor
    );
    if(EFI_ERROR(Status) || HotplugDescriptor == NULL) {
        ASSERT(FALSE);
        return Status;
    }
    for(i=0; i<HotplugUserDescriptorCount; i++)
      {
          HotplugDescriptor[i].Flags = 0;

          HotplugDescriptor[i].Engine.StartLane                 = pHotplugUserDescriptor[i].StartLane;
          HotplugDescriptor[i].Engine.EndLane                   = pHotplugUserDescriptor[i].EndLane;
          HotplugDescriptor[i].Engine.SocketNumber              = pHotplugUserDescriptor[i].SocketNumber;

          HotplugDescriptor[i].Mapping.HotPlugFormat            = pHotplugUserDescriptor[i].HotplugType & 0x7;
          HotplugDescriptor[i].Mapping.Revision                 = 1;
          HotplugDescriptor[i].Mapping.GpioDescriptorValid      = 0;
          HotplugDescriptor[i].Mapping.ResetDescriptorValid     = 0;
          HotplugDescriptor[i].Mapping.PortActive               = 1;
          HotplugDescriptor[i].Mapping.MasterSlaveAPU           = 0;
          HotplugDescriptor[i].Mapping.DieNumber                = 0;
          
          if ( SetupDataBuffer.CPU0SataPcieSel == 0 ) { //sata
              HotplugDescriptor[i].Mapping.DieNumber                = 1;
          }
          
          HotplugDescriptor[i].Mapping.ApertureID               = 0;
          HotplugDescriptor[i].Mapping.AlternateSlotNumber      = 0;
          HotplugDescriptor[i].Mapping.PrimarySecondaryLink     = 0;
          HotplugDescriptor[i].Mapping.Reserved1                = 0;

          HotplugDescriptor[i].Function.I2CGpioBitSelector      = 0;
          HotplugDescriptor[i].Function.I2CGpioByteMapping      = (pHotplugUserDescriptor[i].I2CGpioByteMapping == Low8BitGpio) ? 0 : 1;
          HotplugDescriptor[i].Function.I2CGpioDeviceMappingExt = 0;
          HotplugDescriptor[i].Function.I2CGpioDeviceMapping    = pHotplugUserDescriptor[i].I2CSlaveAddress & 0x7;
          HotplugDescriptor[i].Function.I2CDeviceType           = pHotplugUserDescriptor[i].PcaChipSelect & 0x3;
          HotplugDescriptor[i].Function.I2CBusSegment           = pHotplugUserDescriptor[i].Pca9545BusSegment & 7;
          HotplugDescriptor[i].Function.FunctionMask            = pHotplugUserDescriptor[i].FunctionMask;
          HotplugDescriptor[i].Function.PortMapping             = 0;
          HotplugDescriptor[i].Function.Reserved1               = 0;
          HotplugDescriptor[i].Function.I2CBusSegmentExt        = pHotplugUserDescriptor[i].Pca9545BusSegment >> 3;

          HotplugDescriptor[i].Reset.Reserved0                  = 0;
          HotplugDescriptor[i].Reset.I2CGpioByteMapping         = 0;
          HotplugDescriptor[i].Reset.Reserved1                  = 0;
          HotplugDescriptor[i].Reset.I2CGpioDeviceMapping       = 0;
          HotplugDescriptor[i].Reset.I2CDeviceType              = 0;
          HotplugDescriptor[i].Reset.I2CBusSegment              = 0;
          HotplugDescriptor[i].Reset.ResetSelect                = 0;

          HotplugDescriptor[i].Gpio.Reserved0                   = 0;
          HotplugDescriptor[i].Gpio.I2CGpioByteMapping          = 0;
          HotplugDescriptor[i].Gpio.Reserved1                   = 0;
          HotplugDescriptor[i].Gpio.I2CGpioDeviceMapping        = 0;
          HotplugDescriptor[i].Gpio.I2CDeviceType               = 0;
          HotplugDescriptor[i].Gpio.I2CBusSegment               = 0;
          HotplugDescriptor[i].Gpio.GpioSelect                  = 0;
      }
      HotplugDescriptor[HotplugUserDescriptorCount - 1].Flags = DESCRIPTOR_TERMINATE_LIST;

      DumpHotplugDescriptor(HotplugDescriptor, HotplugUserDescriptorCount);

      mHotplugDescriptorProtocol.NbioHotplugDesc = HotplugDescriptor;
      return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HotplugDescEntry (
        IN       EFI_HANDLE         ImageHandle,
        IN       EFI_SYSTEM_TABLE   *SystemTable
)
{
    EFI_HANDLE                       Handle;
    EFI_STATUS                       Status;

    Handle = NULL;

    Status = SetNbioHotplugDescriptorProtocol();
    if(EFI_ERROR(Status) || (mHotplugDescriptorProtocol.NbioHotplugDesc == NULL))
    {
        DEBUG((DEBUG_ERROR, "SetNbioHotplugDescriptorProtocol Status=%r, NbioHotplugDesc=%p\n", Status, mHotplugDescriptorProtocol.NbioHotplugDesc));
        return Status;
    }

    // Fixup HOTPLUG_DESCRIPTOR here
    Status = gBS->InstallProtocolInterface (
            &Handle,
            &gAmdHotplugDescProtocolGuid,
            EFI_NATIVE_INTERFACE,
            &mHotplugDescriptorProtocol
    );

    return Status;
}
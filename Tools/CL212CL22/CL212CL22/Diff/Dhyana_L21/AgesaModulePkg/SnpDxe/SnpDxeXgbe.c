/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxeXgbe.c
 *
 * Contains Ethernet Phy Driver for the DXE phase.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 300489 $ @e date: $Date: 2014-08-08 14:36:46 -0700 (Fri, 08 Aug 2014) $
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
#include "SnpDxe.h"
#include "SnpDxeXgbe.h"
#include <AMD.h>
#include <Library/AmdBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/Pca9535aProtocol.h>
#include <Protocol/Pca9545aProtocol.h>
#include <Protocol/Sff8472Protocol.h>
#include <Protocol/xGbEI2cMaster.h>
//#include "Library/GnbCZLib/Module/GnpCommonLib/GnbLibPciAcc.h"
#include <Protocol/PciIo.h>
#include <AmdBoardId.h>

//extern EFI_GUID gEfiPciIoProtocolGuid; // = EFI_PCI_IO_PROTOCOL_GUID;//PS-67++add for multi xgbe ports
EFI_GUID EfiPciIoProtocolGuid          = { 0x4CF5B200, 0x68B8, 0x4CA5, { 0x9E, 0xEC, 0xB2, 0x3E, 0x3F, 0x50, 0x02, 0x9A }};

#define XGBE_PHY_PORT_SPEED_100      BIT0
#define XGBE_PHY_PORT_SPEED_1000     BIT1
#define XGBE_PHY_PORT_SPEED_2500     BIT2
#define XGBE_PHY_PORT_SPEED_10000    BIT3

STATIC UINT8 mCurrLanSpeed = 1;
STATIC UINT8 mLan0Speed = 0;
STATIC UINT8 mLan1Speed = 0;
STATIC UINT8 mLan2Speed = 0;
STATIC UINT8 mLan3Speed = 0;
UINTN OldIntVal = 0;
EFI_EVENT LinkStsTimerEvent = NULL;
/* SFP EEPROM Base ID values relative to an offset of 0 */
#define XGBE_SFP_BASE_ID              0
#define XGBE_SFP_ID_SFP               0x03

#define XGBE_SFP_BASE_EXT_ID          1
#define XGBE_SFP_EXT_ID_SFP           0x04

#define XGBE_SFP_BASE_10GBE_CC          3
#define XGBE_SFP_BASE_10GBE_CC_SR       BIT4
#define XGBE_SFP_BASE_10GBE_CC_LR       BIT5
#define XGBE_SFP_BASE_10GBE_CC_LRM      BIT6
#define XGBE_SFP_BASE_10GBE_CC_ER       BIT7

#define XGBE_SFP_BASE_1GBE_CC           6
#define XGBE_SFP_BASE_1GBE_CC_SX        BIT0
#define XGBE_SFP_BASE_1GBE_CC_LX        BIT1
#define XGBE_SFP_BASE_1GBE_CC_CX        BIT2
#define XGBE_SFP_BASE_1GBE_CC_T         BIT3

#define XGBE_SFP_BASE_CABLE             8
#define XGBE_SFP_BASE_CABLE_PASSIVE     BIT2
#define XGBE_SFP_BASE_CABLE_ACTIVE      BIT3
#define XGBE_SFP_BASE_BR                12
#define XGBE_SFP_BASE_BR_1GBE_MIN       0x0a
#define XGBE_SFP_BASE_BR_1GBE_MAX       0x0d
#define XGBE_SFP_BASE_BR_10GBE_MIN      0x64
#define XGBE_SFP_BASE_BR_10GBE_MAX      0x68

#define XGBE_SFP_BASE_CC                63
/* SFP EEPROM Extended ID values relative to an offset of 64 */
#define XGBE_SFP_EXTD_CC                31
/* Rate-change complete wait/retry count */
#define XGBE_RATECHANGE_COUNT           500

#define XGBE_MUTEX_RELEASE              0x80000000ul
#define XGBE_SFP_DIRECT                 7

#define XGBE_PHY_REDRV_MODE_REG         0x12b0
#define XGBE_ABORT_COUNT                500
#define XGBE_DISABLE_COUNT              1000
#define XGBE_INTR_RX_FULL               BIT2 //BIT(IC_RAW_INTR_STAT_RX_FULL_INDEX)
#define XGBE_INTR_TX_EMPTY              BIT4 //BIT(IC_RAW_INTR_STAT_TX_EMPTY_INDEX)
#define XGBE_INTR_TX_ABRT               BIT6 //BIT(IC_RAW_INTR_STAT_TX_ABRT_INDEX)
#define XGBE_INTR_STOP_DET              BIT9 //BIT(IC_RAW_INTR_STAT_STOP_DET_INDEX)
#define XGBE_DEFAULT_INT_MASK    (XGBE_INTR_RX_FULL  | \
                        XGBE_INTR_TX_EMPTY |  \
                        XGBE_INTR_TX_ABRT  |  \
                        XGBE_INTR_STOP_DET)
#define XGBE_SFP_BASE_CU_CABLE_LEN     18
#define DUPLE X_HALF                   0x00
#define DUPLEX_FULL                    0x01
#define DUPLEX_UNKNOWN                 0xff
#define MDIO_AN_ADVERTISE              16


enum xgbe_port_mode
{
  XGBE_PORT_MODE_RSVD = 0,
  XGBE_PORT_MODE_BACKPLANE,
  XGBE_PORT_MODE_BACKPLANE_2500,
  XGBE_PORT_MODE_1000BASE_T,
  XGBE_PORT_MODE_1000BASE_X,
  XGBE_PORT_MODE_NBASE_T,
  XGBE_PORT_MODE_10GBASE_T,
  XGBE_PORT_MODE_10GBASE_R,
  XGBE_PORT_MODE_SFP,
  XGBE_PORT_MODE_MAX,
};

enum xgbe_conn_type
{
  XGBE_CONN_TYPE_NONE = 0,
  XGBE_CONN_TYPE_SFP,
  XGBE_CONN_TYPE_MDIO,
  XGBE_CONN_TYPE_BACKPLANE,
  XGBE_CONN_TYPE_MAX,
};

typedef enum {
  XGBE_PHY_MODE_RSVD = 0,
  XGBE_PHY_MODE_BACKPLANE,
  XGBE_PHY_MODE_BACKPLANE_2500,
  XGBE_PHY_MODE_1000BASE_T,
  XGBE_PHY_MODE_1000BASE_X,
  XGBE_PHY_MODE_NBASE_T,
  XGBE_PHY_MODE_10GBASE_T,
  XGBE_PHY_MODE_10GBASE_R,
  XGBE_PHY_MODE_SFP,
  XGBE_PHY_MODE_SFP_KR,
} xgbe_phy_mode;

/* SFP/SFP+ related definitions */
typedef enum {
  XGBE_SFP_COMM_DIRECT = 0,
  XGBE_SFP_COMM_PCA9545,
} xgbe_sfp_comm;

typedef enum {
  XGBE_SFP_CABLE_UNKNOWN = 0,
  XGBE_SFP_CABLE_ACTIVE,
  XGBE_SFP_CABLE_PASSIVE,
} xgbe_sfp_cable;

typedef enum {
  XGBE_SFP_BASE_UNKNOWN = 0,
  XGBE_SFP_BASE_1000_T,
  XGBE_SFP_BASE_1000_SX,
  XGBE_SFP_BASE_1000_LX,
  XGBE_SFP_BASE_1000_CX,
  XGBE_SFP_BASE_10000_SR,
  XGBE_SFP_BASE_10000_LR,
  XGBE_SFP_BASE_10000_LRM,
  XGBE_SFP_BASE_10000_ER,
  XGBE_SFP_BASE_10000_CR,
} xgbe_sfp_base;

typedef enum {
  XGBE_SFP_SPEED_UNKNOWN = 0,
  XGBE_SFP_SPEED_100_1000,
  XGBE_SFP_SPEED_1000,
  XGBE_SFP_SPEED_10000,
} xgbe_sfp_speed;


/* MDIO PHY reset types */
enum xgbe_mdio_reset
{
  XGBE_MDIO_RESET_NONE = 0,
  XGBE_MDIO_RESET_I2C_GPIO,
  XGBE_MDIO_RESET_INT_GPIO,
  XGBE_MDIO_RESET_MAX,
};

/* Re-driver related definitions */
enum xgbe_phy_redrv_if
{
  XGBE_PHY_REDRV_IF_MDIO = 0,
  XGBE_PHY_REDRV_IF_I2C,
  XGBE_PHY_REDRV_IF_MAX,
};

enum xgbe_phy_redrv_model
{
  XGBE_PHY_REDRV_MODEL_4223 = 0,
  XGBE_PHY_REDRV_MODEL_4227,
  XGBE_PHY_REDRV_MODEL_MAX,
};

enum xgbe_phy_redrv_mode
{
  XGBE_PHY_REDRV_MODE_CX = 5,
  XGBE_PHY_REDRV_MODE_SR = 9,
};

enum xgbe_speed
{
  XGBE_SPEED_1000 = 0,
  XGBE_SPEED_2500,
  XGBE_SPEED_10000,
  XGBE_SPEEDS,
};

enum xgbe_an_mode
{
  XGBE_AN_MODE_CL73 = 0,
  XGBE_AN_MODE_CL73_REDRV,
  XGBE_AN_MODE_CL37,
  XGBE_AN_MODE_CL37_SGMII,
  XGBE_AN_MODE_NONE,
};

enum xgbe_an
{
  XGBE_AN_READY = 0,
  XGBE_AN_PAGE_RECEIVED,
  XGBE_AN_INCOMPAT_LINK,
  XGBE_AN_COMPLETE,
  XGBE_AN_NO_LINK,
  XGBE_AN_ERROR,
};

enum xgbe_rx
{
  XGBE_RX_BPA = 0,
  XGBE_RX_XNP,
  XGBE_RX_COMPLETE,
  XGBE_RX_ERROR,
};

enum xgbe_mode
{
  XGBE_MODE_KX_1000 = 0,
  XGBE_MODE_KX_2500,
  XGBE_MODE_KR,
  XGBE_MODE_X,
  XGBE_MODE_SGMII_100,
  XGBE_MODE_SGMII_1000,
  XGBE_MODE_SFI,
  XGBE_MODE_UNKNOWN,
};

enum xgbe_speedset
{
  XGBE_SPEEDSET_1000_10000 = 0,
  XGBE_SPEEDSET_2500_10000,
};

enum xgbe_mdio_mode
{
  XGBE_MDIO_MODE_NONE = 0,
  XGBE_MDIO_MODE_CL22,
  XGBE_MDIO_MODE_CL45,
};

typedef struct {
  UINT8 base[64];
  UINT8 extd[32];
  UINT8 vendor[32];
} xgbe_sfp_eeprom;

typedef struct {
  xgbe_phy_mode port_mode;
  UINT8 port_id;
  UINTN port_speeds;
  UINTN conn_type;
  enum xgbe_mode cur_mode;
  enum xgbe_mode start_mode;
  UINTN rrc_count;
  UINTN mdio_addr;
  UINTN comm_owned;
// SFP Support
  xgbe_sfp_comm sfp_comm;
  UINTN sfp_mux_address;
  UINTN sfp_mux_channel;
  UINTN sfp_gpio_address;
  UINTN sfp_gpio_mask;
  UINTN sfp_gpio_rx_los;
  UINTN sfp_gpio_tx_fault;
  UINTN sfp_gpio_mod_absent;
  UINTN sfp_gpio_rate_select;
  UINTN sfp_rx_los;
  UINTN sfp_tx_fault;
  UINTN sfp_mod_absent;
  UINTN sfp_diags;
  UINTN sfp_changed;
  UINTN sfp_phy_avail;
  UINTN sfp_cable_len;
  xgbe_sfp_base sfp_base;
  xgbe_sfp_cable sfp_cable;
  xgbe_sfp_speed sfp_speed;
  xgbe_sfp_eeprom sfp_eeprom;
  // External PHY support
  enum xgbe_mdio_mode phydev_mode;
  struct mii_bus  *mii;
  struct phy_device  *phydev;
  enum xgbe_mdio_reset  mdio_reset;
  UINTN mdio_reset_addr;
  UINTN mdio_reset_gpio;
  // UINT8 EEPROM[128];
  //Re-driver support
  UINTN redrv;
  UINTN redrv_if;
  UINTN redrv_addr;
  UINTN redrv_lane;
  UINTN redrv_model;
} XGBE_PHY_DATA;

VOID STATIC xgbe_phy_set_mode (ENET_DXE_PRIVATE_DATA *pdata, enum xgbe_mode mode);
VOID STATIC xgbe_an73_disable (ENET_DXE_PRIVATE_DATA *pdata);

VOID
STATIC
write_mmd_regs (
  IN       UINTN XpcsBase,
  IN       UINTN mmd_reg,
  IN       UINTN mmd_data
  );

EFI_STATUS
XgbeI2cCall (
 ENET_DXE_PRIVATE_DATA *Private
 );


UINT32
STATIC
clearBitfield (
  IN UINT32 Value,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 ClearMask;
  UINTN i;

  ClearMask = 0;
  for (i = 0; i < Width; i++) {
    ClearMask |= 1 << i;
  }
  return Value & ~(ClearMask << Lsb);
}

UINT32
STATIC
getBitfield (
  IN UINT32 Value,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 Mask;
  UINT32 i;

  Mask = 0;
  for (i = 0; i < Width; i++) {
    Mask |= 1 << i;
  }
  Value &= Mask << Lsb;
  return (Value >> Lsb);
}

VOID
STATIC
setBitfieldReg (
  IN UINT32 Base,
  IN UINT32 Reg,
  IN UINT32 Lsb,
  IN UINT32 Width,
  IN UINT32 FieldValue
  )
{
  UINT32 Value;
  UINT32 RetryCount;
  UINT32 Index;
  UINT32 VerifyValue;
  UINT32 Addr;

  Addr = Base + ( Reg * 4 );
  RetryCount = 1000;
  Value = MmioRead16 (Addr);
  Value = clearBitfield (Value, Lsb, Width);
  Value |= FieldValue << Lsb;
  MmioWrite16 (Addr, (UINT16)Value);

  for (Index = 0; Index < RetryCount; Index++) {
    VerifyValue = MmioRead16 (Addr);
    if (VerifyValue == Value) {
      break;
    }

    MmioWrite16 (Addr, (UINT16)Value);
    gBS->Stall (1000);
  }
}

VOID
STATIC
setBitfieldReg32 (
  IN UINT32 Addr,
  IN UINT32 Lsb,
  IN UINT32 Width,
  IN UINT32 FieldValue
  )
{
  UINT32 Value;
  UINT32 RetryCount;
  UINT32 Index;
  UINT32 VerifyValue;

  RetryCount = 1000;
  Value = MmioRead32 (Addr);
  Value = clearBitfield (Value, Lsb, Width);
  Value |= FieldValue << Lsb;
  MmioWrite32 (Addr, Value);

  for (Index = 0; Index < RetryCount; Index++) {
    VerifyValue = MmioRead32 (Addr);
    if (VerifyValue == Value) {
      break;
    }

    MmioWrite32 (Addr, Value);
    gBS->Stall (1000);
  }
}

UINT32
STATIC
getBitfieldReg (
  IN UINT32 Base,
  IN UINT32 Reg,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 Value;
  UINT32 Addr;
  Addr = Base + Reg;
  Value = MmioRead16 (Addr);
  return getBitfield (Value, Lsb, Width);
}

UINT32
STATIC
getBitfieldReg32 (
  IN UINT32 Addr,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 Value;

  Value = MmioRead32 (Addr);
  return getBitfield (Value, Lsb, Width);
}

VOID
xgbe_an37_clear_interrupts (
  UINTN XpcsBase
  )
{
  UINTN reg;

  reg = XMDIO_READ (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_STAT);
  reg &= ~XGBE_AN_CL37_INT_MASK;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_STAT, reg);
}

VOID
xgbe_an37_disable_interrupts (
  UINTN XpcsBase
  )
{
  UINTN reg;
  DEBUG ((EFI_D_ERROR, "xgbe_an37_disable_interrupts\n" ));
  gBS->Stall (10);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_CTRL);
  reg &= ~XGBE_AN_CL37_INT_MASK;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_CTRL, reg);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_PCS_DIG_CTRL);
  reg &= ~XGBE_PCS_CL37_BP;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PCS, MDIO_PCS_DIG_CTRL, reg);
}

VOID
xgbe_an37_enable_interrupts (
  UINTN XpcsBase
  )
{
  UINTN reg;

  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_PCS_DIG_CTRL);
  reg |= XGBE_PCS_CL37_BP;
  XMDIO_WRITE ( XpcsBase, MDIO_MMD_PCS, MDIO_PCS_DIG_CTRL, reg);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_CTRL);
  reg |= XGBE_AN_CL37_INT_MASK;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_VEND2, MDIO_VEND2_AN_CTRL, reg);
}

VOID
xgbe_an73_clear_interrupts (
  UINTN XpcsBase
  )
{
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
}

VOID
xgbe_an73_disable_interrupts (
  UINTN XpcsBase
  )
{
  DEBUG ((EFI_D_ERROR, "xgbe_an73_disable_interrupts\n" ));
  gBS->Stall (10);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INTMASK, 0);
}

VOID
xgbe_an73_enable_interrupts (
  UINTN XpcsBase
  )
{
  DEBUG ((EFI_D_ERROR, "xgbe_an73_enable_interrupts\n" ));
  gBS->Stall (10);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INTMASK, XGBE_AN_CL73_INT_MASK);
}

VOID
STATIC
xgbe_an_clear_interrupts_all (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN XpcsBase;
  XpcsBase = pdata->xpcs_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_an_clear_interrupts_all\n"));
  gBS->Stall (10);
  xgbe_an73_clear_interrupts (XpcsBase);
  xgbe_an37_clear_interrupts (XpcsBase);
}

EFI_STATUS
XgbePlatinit (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINT32 XgmacBase;
  XgmacBase = Private->xgmac_pdata.xgmac_regs;
  setBitfieldReg32 (XgmacBase + INT_EN, 0, 20, 0x1FFFF);
  return EFI_SUCCESS;
}

VOID
SetLanSpeed (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  //xgbe_sfp_eeprom *sfp_eeprom = &phy_data->sfp_eeprom;
  DEBUG ((EFI_D_ERROR, "SetLanSpeed: phy_data->sfp_speed =%x  \n", phy_data->sfp_speed));
  gBS->Stall (10);

  switch (phy_data->sfp_speed) {
  case XGBE_SFP_SPEED_100_1000:
  case XGBE_SFP_SPEED_1000:
    mCurrLanSpeed = 1;
    break;
  case XGBE_SFP_SPEED_10000:
    mCurrLanSpeed = 10;
    break;
  default:
    break;
  }
}

BOOLEAN
CheckPortEnabled (
  IN       UINT32 XgmacBase
  )
{
  UINT32 ConnType;
  ConnType = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 28, 3 ); // Check Connection Type
  if (ConnType == 0) {
    return FALSE;
  }
  ConnType = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 23, 4 ); // Check Speed Type
  if (ConnType == 0) {
    return FALSE;
  }
  return TRUE;
}

BOOLEAN
XgbePhyModeMismatch (
  XGBE_PHY_DATA *phy_data
  )
{
  switch (phy_data->port_mode) {
  case XGBE_PHY_MODE_BACKPLANE:
    if ((phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000)) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_BACKPLANE_2500:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_2500) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_1000BASE_T:
    if ((phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000)) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_1000BASE_X:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_NBASE_T:
    if ((phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_2500)) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_10GBASE_T:
    if ((phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000)) {
      return FALSE;
    }
    break;
  case XGBE_PHY_MODE_10GBASE_R:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000) {
      return FALSE;
    }
    break;

  case XGBE_PHY_MODE_SFP:
  case XGBE_PHY_MODE_SFP_KR:
    if ((phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) ||
        (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000)) {
        DEBUG ((EFI_D_ERROR, "XGBE_PHY_MODE_SFP returning FALSE \n"));
		    gBS->Stall (10);
      return FALSE;
    }

    break;
  default:
    break;
  }
  return TRUE;
}

EFI_STATUS
xgbe_phy_mdio_reset_setup (
  XGBE_PHY_DATA *phy_data,
  IN       UINT32 XgmacBase
  )
{

  if (phy_data->conn_type != XGBE_CONN_TYPE_MDIO) {
    DEBUG ((EFI_D_ERROR, "Not XGBE_CONN_TYPE_MDIO so get out of here....\n"));
	gBS->Stall (10);
    return 0;
  }
  phy_data->mdio_reset = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 0, 2 );
  switch (phy_data->mdio_reset) {
  case XGBE_MDIO_RESET_NONE:
  case XGBE_MDIO_RESET_I2C_GPIO:
  case XGBE_MDIO_RESET_INT_GPIO:
    break;
  default:
    DEBUG ((EFI_D_ERROR, "unsupported MDIO reset\n"));
	gBS->Stall (10);
    return 1;
  }

  if (phy_data->mdio_reset == XGBE_MDIO_RESET_I2C_GPIO) {
    phy_data->mdio_reset_addr = XGBE_GPIO_ADDRESS_PCA9555 + (getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 8, 3 ));
    phy_data->mdio_reset_gpio = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 12, 4 );
  } else if (phy_data->mdio_reset == XGBE_MDIO_RESET_INT_GPIO) {
    phy_data->mdio_reset_gpio = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 4, 2 );
  }
  return EFI_SUCCESS;
}
BOOLEAN
xgbe_phy_redrv_error (
  XGBE_PHY_DATA *phy_data,
  IN       UINT32 XgmacBase
  )
{
  if (!phy_data->redrv) {
    return FALSE;
  }
  if (phy_data->redrv_if >= XGBE_PHY_REDRV_IF_MAX) {
    return TRUE;
  }

  switch (phy_data->redrv_model) {
  case XGBE_PHY_REDRV_MODEL_4223:
    if (phy_data->redrv_lane > 3) {
      return TRUE;
    }
    break;
  case XGBE_PHY_REDRV_MODEL_4227:
    if (phy_data->redrv_lane > 1) {
      return TRUE;
    }
  break;
  default:
    return TRUE;
  }
  return FALSE;
}

VOID
xgbe_phy_sfp_setup (
  ENET_DXE_PRIVATE_DATA *Private,
  XGBE_PHY_DATA *phy_data
  )
{
  UINT32 MuxAddrHi;
  UINT32 MuxAddrLo;
  UINT32 XgmacBase;
  XgmacBase = Private->xgmac_pdata.xgmac_regs;

  MuxAddrLo = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 0, 3);  //PCA9545 Low Address
  MuxAddrHi = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 8, 5);  //PCA9545 Upper Address
  if ((MuxAddrLo & 0x7) == FALSE) {
    phy_data->sfp_comm = XGBE_SFP_COMM_PCA9545;
    phy_data->sfp_mux_address = (MuxAddrHi << 2) + MuxAddrLo;
    phy_data->sfp_mux_channel = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 4, 3);
    DEBUG ((EFI_D_ERROR, "SFP: mux_address 2nd = %x.\n", phy_data->sfp_mux_address));
    DEBUG ((EFI_D_ERROR, "SFP: mux_channel 2nd = %u\n", phy_data->sfp_mux_channel));
	gBS->Stall (10);
  }
  //SET GPIOS
  phy_data->sfp_gpio_address = PCA9555_ADDRESS + getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 8, 3);  //GPIO_ADDR
  phy_data->sfp_gpio_tx_fault = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 12, 4);    //GPIO_TX_FAULT
  phy_data->sfp_gpio_rate_select = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 16, 4); //GPIO_RATE_SELECT
  phy_data->sfp_gpio_mod_absent = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 20, 4);  //GPIO_MOD_ABS
  phy_data->sfp_gpio_rx_los = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 24, 4);      //GPIO_RX_LOS
  phy_data->sfp_gpio_mask = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_3, 28, 4);        //GPIO_MASK

  DEBUG ((EFI_D_ERROR, "sfp_gpio_address = %0x\n", phy_data->sfp_gpio_address));
  DEBUG ((EFI_D_ERROR, "gpio_mask = %0x\n", phy_data->sfp_gpio_mask));
  DEBUG ((EFI_D_ERROR, "gpio_rx_los = %0x\n", phy_data->sfp_gpio_rx_los));
  DEBUG ((EFI_D_ERROR, "gpio_mod_absent = %0x\n", phy_data->sfp_gpio_mod_absent));
  DEBUG ((EFI_D_ERROR, "gpio_rate_select = %0x\n", phy_data->sfp_gpio_rate_select));
  DEBUG ((EFI_D_ERROR, "gpio_tx_fault = %0x\n", phy_data->sfp_gpio_tx_fault));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_an_sfp_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  switch (phy_data->sfp_base) {
  case XGBE_SFP_BASE_1000_T:
    pdata->an_mode = XGBE_AN_MODE_CL37_SGMII;
  case XGBE_SFP_BASE_1000_SX:
  case XGBE_SFP_BASE_1000_LX:
  case XGBE_SFP_BASE_1000_CX:
    pdata->an_mode = XGBE_AN_MODE_CL37;
  default:
    pdata->an_mode = XGBE_AN_MODE_NONE;
  }
  
  DEBUG ((EFI_D_ERROR, "PS-67: pdata->an_mode = %d\n", pdata->an_mode));
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->sfp_base = %d\n", phy_data->sfp_base));
}

VOID
STATIC
xgbe_phy_an_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  // A KR re-driver will always require CL73 AN
  if (phy_data->redrv) {
    pdata->an_mode = XGBE_AN_MODE_CL73_REDRV;
  }
  switch (phy_data->port_mode) {
  case XGBE_PORT_MODE_BACKPLANE:
    pdata->an_mode = XGBE_AN_MODE_CL73;
  case XGBE_PORT_MODE_BACKPLANE_2500:
    pdata->an_mode = XGBE_AN_MODE_NONE;
  case XGBE_PORT_MODE_1000BASE_T:
    pdata->an_mode = XGBE_AN_MODE_CL37_SGMII;
  case XGBE_PORT_MODE_1000BASE_X:
    pdata->an_mode = XGBE_AN_MODE_CL37;
  case XGBE_PORT_MODE_NBASE_T:
    pdata->an_mode = XGBE_AN_MODE_CL37_SGMII;
  case XGBE_PORT_MODE_10GBASE_T:
    pdata->an_mode = XGBE_AN_MODE_CL73;
  case XGBE_PORT_MODE_10GBASE_R:
    pdata->an_mode = XGBE_AN_MODE_NONE;
  case XGBE_PORT_MODE_SFP:
    xgbe_phy_an_sfp_mode (pdata);
  default:
    pdata->an_mode = XGBE_AN_MODE_NONE;
  }
}

EFI_STATUS
XgbePhySetup (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data;
  UINT32 XgmacBase;

  XgmacBase = Private->xgmac_pdata.xgmac_regs;
  phy_data = AllocateZeroPool (sizeof (XGBE_PHY_DATA));
  if (phy_data == NULL) {
    DEBUG ((EFI_D_ERROR, "Could not allocate xgbe_phy_data structures.\n"));
	gBS->Stall (10);
    return EFI_OUT_OF_RESOURCES;
  }
  if (CheckPortEnabled (XgmacBase) == FALSE) {
    DEBUG ((EFI_D_ERROR, "XMAC: Port is disabled \n", XgmacBase));
	gBS->Stall (10);
    return EFI_UNSUPPORTED;
  }
  phy_data->port_id = (UINT8)getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 0, 8);      //Port Number
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->port_id = 0x%x \n", phy_data->port_id));
  phy_data->port_mode = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 8, 4);    //Platform Config
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->port_mode = 0x%x \n", phy_data->port_mode));
  phy_data->mdio_addr = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 16, 5);     //MDIO_ID);
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->mdio_addr = 0x%x \n", phy_data->mdio_addr));
  phy_data->port_speeds = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 23, 4); // Port Speed
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->port_speeds = 0x%x \n", phy_data->port_speeds));
  phy_data->conn_type = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_0, 28, 3);   //Connection Type
  DEBUG ((EFI_D_ERROR, "PS-67: phy_data->conn_type = 0x%x \n", phy_data->conn_type));

#if 0  //PS-67----
  if (phy_data->conn_type == 0) {
    //setBitfieldReg32 (XgmacBase + PORT_CLK_GATE_EN, 1, 1, 0x1);                  //Disabled port, Gate clock  //PS-67 ----
      setBitfieldReg32 (XgmacBase + PORT_CLK_GATE_EN, 0, 1, 0x1);                  ////PS-67 +++++
    DEBUG ((EFI_D_ERROR, "XgmacBase disabled with address %0x \n", XgmacBase));
	gBS->Stall (10);
    return EFI_NOT_FOUND;
  }
#endif
  
  phy_data->redrv = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 31, 1);
  phy_data->redrv_if = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 23, 1);
  phy_data->redrv_addr = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 16, 7);
  phy_data->redrv_lane = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 24, 3);
  phy_data->redrv_model = getBitfieldReg32 (XgmacBase + ENET_PORT_PROPERTY_4, 28, 3);

  Private->xprop_regs = Private->xgmac_pdata.xgmac_regs + XGBE_MAC_CTRL_OFFSET;
  Private->xi2c_regs = Private->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  gBS->Stall (10);
  DEBUG ((EFI_D_ERROR, "Private->xprop_regs = %x \n", Private->xprop_regs));
  DEBUG ((EFI_D_ERROR, "***PhySetup port_id = %0x \n",  phy_data->port_id ));
  DEBUG ((EFI_D_ERROR, "***PhySetup port_mode = %0x \n",  phy_data->port_mode ));
  DEBUG ((EFI_D_ERROR, "***PhySetup mdio_addr = %0x \n",  phy_data->mdio_addr ));
  DEBUG ((EFI_D_ERROR, "***PhySetup port_speeds = %0x \n",  phy_data->port_speeds ));
  DEBUG ((EFI_D_ERROR, "***PhySetup conn_type = %0x \n",  phy_data->conn_type ));
  DEBUG ((EFI_D_ERROR, "***PhySetup  phy_data->redrv Present = %0x \n",   phy_data->redrv ));
  DEBUG ((EFI_D_ERROR, "***PhySetup  redrv_if = %0x \n",   phy_data->redrv_if ));
  DEBUG ((EFI_D_ERROR, "***PhySetup  redrv_addr = %0x \n",   phy_data->redrv_addr ));
  DEBUG ((EFI_D_ERROR, "***PhySetup  redrv_lane = %0x \n",   phy_data->redrv_lane ));
  DEBUG ((EFI_D_ERROR, "***PhySetup  redrv_model = %0x \n",   phy_data->redrv_model ));

  if (XgbePhyModeMismatch (phy_data)) {
    DEBUG ((EFI_D_ERROR, "phy mode / speed mismatch.\n"));
	gBS->Stall (10);
    return EFI_DEVICE_ERROR;
  }
  xgbe_phy_mdio_reset_setup (phy_data, XgmacBase);
  if (xgbe_phy_redrv_error (phy_data, XgmacBase)) {
    DEBUG ((EFI_D_ERROR, "XGBE: phy re-driver settings error\n"));
	gBS->Stall (10);
    return EFI_DEVICE_ERROR;
  }
    DEBUG ((EFI_D_ERROR, "xgbe_phy_redrv_error Complete\n"));
    Private->kr_redrv = phy_data->redrv;
    //   Indicate current mode is unknown
    phy_data->cur_mode = XGBE_MODE_UNKNOWN;
   //   Initialize supported features
    Private->phy.supported = 0;

  DEBUG ((EFI_D_ERROR, "Checking Mode port_mode = %0x \n", phy_data->port_mode));
  gBS->Stall (10);
  switch (phy_data->port_mode) {
  // Backplane support
  case XGBE_PORT_MODE_BACKPLANE:
      if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
        phy_data->start_mode = XGBE_MODE_KX_1000;
      }
      if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000) {
        phy_data->start_mode = XGBE_MODE_KR;
      }
      phy_data->phydev_mode = XGBE_MDIO_MODE_NONE;
      break;
  case XGBE_PORT_MODE_BACKPLANE_2500:
    phy_data->start_mode = XGBE_MODE_KX_2500;
    phy_data->phydev_mode = XGBE_MDIO_MODE_NONE;
    break;

   // MDIO 1GBase-T support
  case XGBE_PORT_MODE_1000BASE_T:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) {
        phy_data->start_mode = XGBE_MODE_SGMII_100;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
        phy_data->start_mode = XGBE_MODE_SGMII_1000;
    }
    phy_data->phydev_mode = XGBE_MDIO_MODE_CL22;
    break;

  // MDIO Base-X support
  case XGBE_PORT_MODE_1000BASE_X:
    phy_data->start_mode = XGBE_MODE_X;
    phy_data->phydev_mode = XGBE_MDIO_MODE_CL22;
    break;

  // MDIO NBase-T support
  case XGBE_PORT_MODE_NBASE_T:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) {
      phy_data->start_mode = XGBE_MODE_SGMII_100;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
      phy_data->start_mode = XGBE_MODE_SGMII_1000;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_2500) {
      phy_data->start_mode = XGBE_MODE_KX_2500;
    }
    phy_data->phydev_mode = XGBE_MDIO_MODE_CL45;
    break;

  // 10GBase-T support
  case XGBE_PORT_MODE_10GBASE_T:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) {
      phy_data->start_mode = XGBE_MODE_SGMII_100;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
      phy_data->start_mode = XGBE_MODE_SGMII_1000;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000) {
      phy_data->start_mode = XGBE_MODE_KR;
    }
    phy_data->phydev_mode = XGBE_MDIO_MODE_NONE;
    break;

  // 10GBase-R support
  case XGBE_PORT_MODE_10GBASE_R:
    phy_data->start_mode = XGBE_MODE_SFI;
    phy_data->phydev_mode = XGBE_MDIO_MODE_NONE;
    break;

  // SFP support
  case XGBE_PORT_MODE_SFP:
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_100) {
      DEBUG ((EFI_D_ERROR, "XGBE_PHY_PORT_SPEED_100\n"));
	    gBS->Stall (10);
      phy_data->start_mode = XGBE_MODE_SGMII_100;
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_1000) {
      phy_data->start_mode = XGBE_MODE_SGMII_1000;
      DEBUG ((EFI_D_ERROR, "XGBE_MODE_SGMII_1000\n"));
              gBS->Stall (10);
    }
    if (phy_data->port_speeds & XGBE_PHY_PORT_SPEED_10000) {
      phy_data->start_mode = XGBE_MODE_SFI;
      DEBUG ((EFI_D_ERROR, "XGBE_MODE_SFI\n"));
              gBS->Stall (10);
//  if (Private->fec_ability & MDIO_PMA_10GBR_FECABLE_ABLE)
//    pdata->phy.supported |=  SUPPORTED_10000baseR_FEC;
    }
    phy_data->phydev_mode = XGBE_MDIO_MODE_CL22;
    xgbe_phy_sfp_setup (Private, phy_data);
    break;
  default:
    return EFI_UNSUPPORTED;
  }

  if ((phy_data->conn_type & XGBE_CONN_TYPE_MDIO) && (phy_data->phydev_mode != XGBE_MDIO_MODE_NONE)) {
    DEBUG ((EFI_D_ERROR, "*****ERRRRR Port for Gearbox\n"));
	gBS->Stall (10);
//  ret = pdata->hw_if.set_ext_mii_mode(pdata, phy_data->mdio_addr,
//     phy_data->phydev_mode);
  }

  if (phy_data->redrv && !phy_data->redrv_if) {
    DEBUG ((EFI_D_ERROR, "*****ERRRRR Port for External PHY\n"));
	gBS->Stall (10);
//  ret = pdata->hw_if.set_ext_mii_mode(pdata, phy_data->redrv_addr,
//  XGBE_MDIO_MODE_CL22);
  }

  // Start in highest supported mode
  Private->phy_data = phy_data;
  xgbe_phy_set_mode (Private, phy_data->start_mode);
  xgbe_phy_an_mode (Private);
  return EFI_SUCCESS;
}

VOID
GetI2cOwnership (
  IN       UINT32 XgmacBase,
  IN       UINT8 MutexId
  )
{
  UINT32 MutexVal;
  UINT32 MutexSet1;
  MutexVal |= MutexId;
  MutexVal <<= 29;
  MutexVal |= 1;
  MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, 0x80000000);
  do {
    MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, MutexVal);
    gBS->Stall (10);
    MutexSet1 = MmioRead32 (XgmacBase + PORT_I2C_MUTEX);
  } while (MutexSet1 != MutexVal);
}

VOID
PutI2cOwnership (
  IN       UINT32 XgmacBase,
  IN       UINT8 MutexId
  )
{
  if (getBitfieldReg32 (XgmacBase + PORT_I2C_MUTEX, 29, 2) == MutexId) {
    MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, 0x80000000);
  }
}

EFI_STATUS
XgbeSfpSignal (                      //    Pca9555
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
  UINT8                     Offset;
  UINT8                     Data[2];
  UINT16                    Pca9555Data;
  UINT32                    XgmacBaseI2c;
  UINTN                     SlaveAdd;
  Status =                  EFI_SUCCESS;
  Pca9555Data =             0;
  Data[0] =                 0;
  Data[1] =                 0;
  Offset = 0;
  SlaveAdd = phy_data->sfp_gpio_address; //0x23;
  phy_data->sfp_rx_los = 0;
  phy_data->sfp_tx_fault = 0;
  phy_data->sfp_mod_absent  = 1;
  XgmacBaseI2c = Private->xgmac_pdata.xgmac_regs + 0x1E000;

  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC XgbeSfpSignal: Could not Locate Protocol.\n"));
	gBS->Stall (10);
    return Status;
  }

  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof (UINT16);
  Operation[1].Buffer           = Data;

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest.\n"));
    gBS->Stall (100);
    return Status;
  }
  Pca9555Data = (Data[1] << 8) | Data[0];
  
  DEBUG ((EFI_D_ERROR, "\nphy_data->sfp_gpio_rx_los = 0x%x\n", phy_data->sfp_gpio_rx_los));
  DEBUG ((EFI_D_ERROR, "\nphy_data->sfp_gpio_rx_los = 0x%x\n", phy_data->sfp_gpio_tx_fault));
  DEBUG ((EFI_D_ERROR, "\nphy_data->sfp_gpio_rx_los = 0x%x\n", phy_data->sfp_gpio_mod_absent));
  
//    return Pca9555Data;

  if (Pca9555Data & (1 << phy_data->sfp_gpio_rx_los)) {
    phy_data->sfp_rx_los = 1;
  }
  if (Pca9555Data & (1 << phy_data->sfp_gpio_tx_fault)) {
    phy_data->sfp_tx_fault = 1;
  }
  if (!(Pca9555Data & (1 << phy_data->sfp_gpio_mod_absent))) {
    phy_data->sfp_mod_absent = 0;
  }
  DEBUG ((EFI_D_ERROR, "PCA9535 Data received = %0x \n", Pca9555Data));
  gBS->Stall (100);
  return Status;
}

EFI_STATUS
XgbeSfpPutMux (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  UINT8                     Mux_Channel;
  UINTN                     SlaveAdd;
  UINT32                    XgmacBaseI2c;
  EFI_STATUS                Status;

  Status = EFI_SUCCESS;
  Mux_Channel = 0;
  SlaveAdd = phy_data->sfp_mux_address;
  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Could not Locate gxGbEI2cMasterProtocolGuid\n"));
    gBS->Stall (100);
    return Status;
  }
  XgmacBaseI2c = Private->xgmac_pdata.xgmac_regs + 0x1E000;
  //SET Base address
  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 1;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Mux_Channel;

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest for MUX.\n"));
	gBS->Stall (10);
    return Status;
  }
  DEBUG ((EFI_D_ERROR, "\nXGMAC: Success to StartRequest for MUX turned OFF.\n"));
  gBS->Stall (10);
  return Status;
}

EFI_STATUS
XgbeSfpGetMux (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  UINT8                     Mux_Channel;
  UINTN                     SlaveAdd;
  UINT32                    XgmacBaseI2c;
  EFI_STATUS                Status;

  Status = EFI_SUCCESS;
  Mux_Channel = 1 << phy_data->sfp_mux_channel;
  SlaveAdd = phy_data->sfp_mux_address;
  DEBUG ((EFI_D_ERROR, "XGMAC: Mux SlaveAdd = %0x , Mux_Channel = %0x\n", SlaveAdd, Mux_Channel));

  gBS->Stall (10);
  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Could not Locate gxGbEI2cMasterProtocolGuid\n"));
    gBS->Stall (100);
    return Status;
  }
  XgmacBaseI2c = Private->xgmac_pdata.xgmac_regs + 0x1E000;
  //SET Base address
  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 1;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Mux_Channel;

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest for MUX.\n"));
	gBS->Stall (10);
    return Status;
  }
  return Status;
}

EFI_STATUS
XgbeSfpReadEeprom (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  xgbe_sfp_eeprom *sfp_eeprom = &phy_data->sfp_eeprom;
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
  UINT8                     Offset;
  UINT8                     Data[16];
  UINT8                     *DestDataPtr;
  UINT8                     i;
  UINT32                    XgmacBaseI2c;
  UINTN SlaveAdd;

  Status = EFI_SUCCESS;
  Offset = 0;
  for (i = 0; i < 16; i++) {
    Data[i] = 0;
  }

  Status = XgbeSfpGetMux (Private);
  ASSERT (Status == EFI_SUCCESS);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "Prepare to Start Reading Eeprom XgbeSfpReadEeprom\n"));
  gBS->Stall (10);
  SlaveAdd = 0x50;
  XgmacBaseI2c = Private->xgmac_pdata.xgmac_regs + 0x1E000;

  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC Workaround: Could not Locate gxGbEI2cMasterProtocolGuid.\n"));
    gBS->Stall (100);
  }
  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  //  Operation[0].Buffer       = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof (Data);
  Operation[1].Buffer           = &Data[0];

  DestDataPtr = (UINT8 *) &sfp_eeprom;
  for (i = 0; i < 8; i++) {
    Operation[0].Buffer = &Offset;
    Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
    CopyMem (DestDataPtr, Data, 16);
    DEBUG ((EFI_D_ERROR, "PS-67: XgbeSfpReadEeprom() Data =0x%x\n", Data));
    DestDataPtr += 16;
    Offset += 16;
  }
  CopyMem (&phy_data->sfp_eeprom, &sfp_eeprom, sizeof (sfp_eeprom));

  XgbeSfpPutMux (Private);
  return Status;
//  Private->phy_data = phy_data;
}
BOOLEAN
xgbe_phy_sfp_verify_eeprom (
  UINT8 cc_in,
  UINT8 *buf,
  UINTN len
  )
{
  UINT8 cc;
  for (cc = 0; len; buf++, len--) {
    cc += *buf;
  }
  return (cc == cc_in) ? TRUE : FALSE;
}

BOOLEAN
XgbeSfpBitRate (
  xgbe_sfp_eeprom *sfp_eeprom,
  xgbe_sfp_speed sfp_speed
  )
{
  UINT8 *sfp_base;
  UINT8  min;
  UINT8  max;

  sfp_base = sfp_eeprom->base;

  switch (sfp_speed) {
  case XGBE_SFP_SPEED_1000:
    min = XGBE_SFP_BASE_BR_1GBE_MIN;
    max = XGBE_SFP_BASE_BR_1GBE_MAX;
    break;
  case XGBE_SFP_SPEED_10000:
    min = XGBE_SFP_BASE_BR_10GBE_MIN;
    max = XGBE_SFP_BASE_BR_10GBE_MAX;
    break;
  default:
    return FALSE;
  }
return ((sfp_base[XGBE_SFP_BASE_BR] >= min) &&
    (sfp_base[XGBE_SFP_BASE_BR] <= max));
}

EFI_STATUS
XgbeSfpParseEeprom (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  xgbe_sfp_eeprom *sfp_eeprom = &phy_data->sfp_eeprom;
  UINT8 *sfp_base;

  phy_data->sfp_base = XGBE_SFP_BASE_UNKNOWN;
  phy_data->sfp_cable = XGBE_SFP_CABLE_UNKNOWN;
  phy_data->sfp_speed = XGBE_SFP_SPEED_UNKNOWN;

  sfp_base = sfp_eeprom->base;
    DEBUG ((EFI_D_ERROR, "sfp_base[XGBE_SFP_BASE_CABLE] =%x \n", sfp_base[XGBE_SFP_BASE_CABLE]));
    DEBUG ((EFI_D_ERROR, "sfp_base[XGBE_SFP_BASE_CU_CABLE_LEN]; =%x \n", sfp_base[XGBE_SFP_BASE_CU_CABLE_LEN]));
    DEBUG ((EFI_D_ERROR, "sfp_base[XGBE_SFP_BASE_10GBE_CC]; =%x \n", sfp_base[XGBE_SFP_BASE_10GBE_CC]));
    DEBUG ((EFI_D_ERROR, "sfp_base[XGBE_SFP_BASE_1GBE_CC]; =%x \n", sfp_base[XGBE_SFP_BASE_1GBE_CC]));
	gBS->Stall (10);
	
  DEBUG ((EFI_D_ERROR, "PS-67-SFP: sfp_base[XGBE_SFP_BASE_ID] =%x \n", sfp_base[XGBE_SFP_BASE_ID]));
  DEBUG ((EFI_D_ERROR, "PS-67-SFP: sfp_base[XGBE_SFP_BASE_EXT_ID]] =%x \n", sfp_base[XGBE_SFP_BASE_EXT_ID]));

  if (sfp_base[XGBE_SFP_BASE_ID] != XGBE_SFP_ID_SFP) {
    return EFI_UNSUPPORTED;
  }
  if (sfp_base[XGBE_SFP_BASE_EXT_ID] != XGBE_SFP_EXT_ID_SFP) {
    return EFI_UNSUPPORTED;
  }
  if (sfp_base[XGBE_SFP_BASE_CABLE] & XGBE_SFP_BASE_CABLE_PASSIVE) {
    phy_data->sfp_cable = XGBE_SFP_CABLE_PASSIVE;
    phy_data->sfp_cable_len = sfp_base[XGBE_SFP_BASE_CU_CABLE_LEN];
  }
//  else if (sfp_base[XGBE_SFP_BASE_CABLE] & XGBE_SFP_BASE_CABLE_ACTIVE)
  else
    phy_data->sfp_cable = XGBE_SFP_CABLE_ACTIVE;

  if (sfp_base[XGBE_SFP_BASE_10GBE_CC] & XGBE_SFP_BASE_10GBE_CC_SR) {
    phy_data->sfp_base = XGBE_SFP_BASE_10000_SR;
  } else if (sfp_base[XGBE_SFP_BASE_10GBE_CC] & XGBE_SFP_BASE_10GBE_CC_LR) {
    phy_data->sfp_base = XGBE_SFP_BASE_10000_LR;
  } else if (sfp_base[XGBE_SFP_BASE_10GBE_CC] & XGBE_SFP_BASE_10GBE_CC_LRM) {
    phy_data->sfp_base = XGBE_SFP_BASE_10000_LRM;
  } else if (sfp_base[XGBE_SFP_BASE_10GBE_CC] & XGBE_SFP_BASE_10GBE_CC_ER) {
    phy_data->sfp_base = XGBE_SFP_BASE_10000_ER;
  } else if (sfp_base[XGBE_SFP_BASE_1GBE_CC] & XGBE_SFP_BASE_1GBE_CC_SX) {
    phy_data->sfp_base = XGBE_SFP_BASE_1000_SX;
  } else if (sfp_base[XGBE_SFP_BASE_1GBE_CC] & XGBE_SFP_BASE_1GBE_CC_LX) {
    phy_data->sfp_base = XGBE_SFP_BASE_1000_LX;
  } else if (sfp_base[XGBE_SFP_BASE_1GBE_CC] & XGBE_SFP_BASE_1GBE_CC_T) {
    phy_data->sfp_base = XGBE_SFP_BASE_1000_T;
  } else if (sfp_base[XGBE_SFP_BASE_1GBE_CC] & XGBE_SFP_BASE_1GBE_CC_CX) {
    phy_data->sfp_base = XGBE_SFP_BASE_1000_CX;
  } else if ((phy_data->sfp_cable == XGBE_SFP_CABLE_PASSIVE) &&
   XgbeSfpBitRate (sfp_eeprom, XGBE_SFP_SPEED_10000)) {
    phy_data->sfp_base = XGBE_SFP_BASE_10000_CR;
  }

  switch (phy_data->sfp_base) {
  case XGBE_SFP_BASE_1000_T:
    phy_data->sfp_speed = XGBE_SFP_SPEED_100_1000;
    break;
  case XGBE_SFP_BASE_1000_SX:
  case XGBE_SFP_BASE_1000_LX:
  case XGBE_SFP_BASE_1000_CX:
    phy_data->sfp_speed = XGBE_SFP_SPEED_1000;
    break;
  case XGBE_SFP_BASE_10000_SR:
  case XGBE_SFP_BASE_10000_LR:
  case XGBE_SFP_BASE_10000_LRM:
  case XGBE_SFP_BASE_10000_ER:
  case XGBE_SFP_BASE_10000_CR:
    phy_data->sfp_speed = XGBE_SFP_SPEED_10000;
    break;
  default:
    break;
  }
  return EFI_SUCCESS;
}

VOID
STATIC
xgbe_phy_sfp_phy_settings (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;

  if (phy_data->sfp_mod_absent) {
    pdata->phy.speed = (UINTN)SPEED_UNKNOWN;
    pdata->phy.duplex = DUPLEX_UNKNOWN;
    pdata->phy.autoneg = AUTONEG_ENABLE;
    pdata->phy.advertising = pdata->phy.supported;
  }
  switch (phy_data->sfp_base) {
  case XGBE_SFP_BASE_1000_T:
  case XGBE_SFP_BASE_1000_SX:
  case XGBE_SFP_BASE_1000_LX:
  case XGBE_SFP_BASE_1000_CX:
    pdata->phy.speed = (UINTN)SPEED_UNKNOWN;
    pdata->phy.duplex = DUPLEX_UNKNOWN;
    pdata->phy.autoneg = AUTONEG_ENABLE;
//  pdata->phy.advertising |= ADVERTISED_Autoneg;
    break;
  case XGBE_SFP_BASE_10000_SR:
  case XGBE_SFP_BASE_10000_LR:
  case XGBE_SFP_BASE_10000_LRM:
  case XGBE_SFP_BASE_10000_ER:
  case XGBE_SFP_BASE_10000_CR:
  default:
    pdata->phy.speed = SPEED_10000;
    pdata->phy.duplex = DUPLEX_FULL;
    pdata->phy.autoneg = AUTONEG_DISABLE;
    break;
  }

  switch (phy_data->sfp_base) {
  case XGBE_SFP_BASE_1000_T:
  case XGBE_SFP_BASE_1000_CX:
  case XGBE_SFP_BASE_10000_CR:
  //pdata->phy.advertising |= ADVERTISED_TP;
    break;
  default:
    break;
//  pdata->phy.advertising |= ADVERTISED_FIBRE;
  }
}

VOID
STATIC
xgbe_phy_sfp_external_phy (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  if (!phy_data->sfp_changed) {
    return;
  }

  phy_data->sfp_phy_avail = 0;
  if (phy_data->sfp_base != XGBE_SFP_BASE_1000_T) {
    return;
  }
  phy_data->sfp_phy_avail = 1;
}

EFI_STATUS
XgbeDetectSfp (
  IN  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  UINT32 XgmacBase;
  UINT8 PortNum;
  EFI_STATUS Status;
  Status = EFI_SUCCESS;
  PortNum = phy_data->port_id;
  phy_data->sfp_rx_los = 0;
  phy_data->sfp_tx_fault = 0;
  phy_data->sfp_mod_absent = 1;
  phy_data->sfp_diags = 0;
  phy_data->sfp_base = XGBE_SFP_BASE_UNKNOWN;
  phy_data->sfp_cable = XGBE_SFP_CABLE_UNKNOWN;
  phy_data->sfp_speed = XGBE_SFP_SPEED_UNKNOWN;
  SetMem (&phy_data->sfp_eeprom, sizeof (phy_data->sfp_eeprom), 0);

  XgmacBase = Private->xgmac_pdata.xgmac_regs;
  GetI2cOwnership (XgmacBase, PortNum);
  Status = XgbeSfpSignal (Private);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed SFP.\n"));
	gBS->Stall (10);
  }
  if (phy_data->sfp_mod_absent) {
      DEBUG ((EFI_D_ERROR, "\nXGMAC: SFP Module Absent, phy_data->sfp_mod_absent = %0x\n", phy_data->sfp_mod_absent));
	    gBS->Stall (10);
      PutI2cOwnership (XgmacBase, PortNum);
      return EFI_NOT_FOUND;
  }
  Status = XgbeSfpReadEeprom (Private);
  XgbeSfpParseEeprom (Private);
  xgbe_phy_sfp_external_phy (Private);
  xgbe_phy_sfp_phy_settings (Private);
  PutI2cOwnership (XgmacBase, PortNum);
  return Status;
}

EFI_STATUS
XgbeConfigPhy (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  EFI_STATUS                Status;
  DEBUG ((EFI_D_ERROR, "XgbeConfigPhy phy_data->port_mode: %x \n", phy_data->port_mode));
  gBS->Stall (10);
  switch (phy_data->port_mode) {
  case XGBE_PHY_MODE_BACKPLANE:
  case XGBE_PHY_MODE_BACKPLANE_2500:
    break;
  case XGBE_PHY_MODE_1000BASE_T:
  case XGBE_PHY_MODE_1000BASE_X:
  case XGBE_PHY_MODE_NBASE_T:
  case XGBE_PHY_MODE_10GBASE_T:
  case XGBE_PHY_MODE_10GBASE_R:
    return EFI_UNSUPPORTED;
  case XGBE_PHY_MODE_SFP:
  case XGBE_PHY_MODE_SFP_KR:
    DEBUG ((EFI_D_ERROR, "Case for XgbeDetectSfp \n"));
	gBS->Stall (10);
    Status = XgbeDetectSfp (Private);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "XgbeDetectSfp SFP NOT FOUND \n"));
	    gBS->Stall (10);
      return EFI_NOT_FOUND;
  }
    break;
  default:
    return EFI_UNSUPPORTED;
  }
  return EFI_SUCCESS;
}


EFI_STATUS
GetCurrMode (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
}

/*********************************************************************************
 * Name: XgbeReadEeprom
 *
 * Description
 *   Call to Read XGBE I2C devices
 *
 * Input
 *   XgbeBaseAdd:   XGBE Base address.\
 *   SlaveAdd:      Slave Address of IC
 *   Offset:        Offset to write to
 *   DataLength:    Size of Data to be Read from device.
 *
 * Output
 *   Data read from device
 *
 *********************************************************************************/
UINT16
XgbeReadEeprom (
  UINT32 XgbeBaseAdd,
  UINT8 SlaveAdd,
  UINT8 Offset,
  UINT16 DataLength
  )
{
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
//  UINT8                     Offset;
  UINT8                     Data[2];
  UINT16                    Pca9555Data;
  UINT32                    XgmacBaseI2c;

  Status =                  EFI_SUCCESS;
  Pca9555Data =             0;
  Data[0] =                 0;
  Data[1] =                 0;

  XgmacBaseI2c = XgbeBaseAdd + 0x1E000;
  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC Workaround: Could not Locate IC Protocol.\n"));
    gBS->Stall (100);
  }

  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = DataLength;
  Operation[1].Buffer           = Data;

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest.\n"));
	gBS->Stall (10);
  }
  Pca9555Data = (Data[1] << 8) | Data[0];
  return Pca9555Data;
}

/*********************************************************************************
 * Name: XgbeWriteEeprom
 *
 * Description
 *   Call to write to XGBE IC devices

 *
 * Input
 *   XgbeBaseAdd : XGBE Base address.\
 *   SlaveAdd:     Slave Address of IC
 *   Offset:       Offset to write to
 *   Data:         Data to be written on device.
 *
 * Output
 *
 *********************************************************************************/

VOID
XgbeWriteEeprom (
  UINT32 XgbeBaseAdd,
  UINT8 SlaveAdd,
  UINT8 Offset,
  UINT16 Data
  )
{
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
  UINT8                     Buffer[3];
  UINT32                    XgmacBaseI2c;
  Status =                  EFI_SUCCESS;
  XgmacBaseI2c = XgbeBaseAdd + 0x1E000;
  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC Workaround: Could not Locate IC Protocol.\n"));
    gBS->Stall (100);
  }
  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);

  //Prepare write data buffer
  Buffer[0] = Offset;
  Buffer[1] = (UINT8)(Data & 0x00FF); //Low byte
  Buffer[2] = (UINT8)(Data >> 8);     //High byte

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 1;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = 3;
  Operation[0].Buffer           = &Buffer[0];

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest.\n"));
	gBS->Stall (10);
  }
}

EFI_STATUS
XgbeBaseAddr (
        OUT ENET_DXE_PRIVATE_DATA *Private
  )
{
  PCI_ADDR  PciAddress;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  EFI_STATUS   Status;
  UINTN     BusNumberBase;
  UINTN     SocketNum;
  UINTN     DieNum;
  UINT16    BusMasterEn;
  UINT8     SecBusDie0;
  UINT8     SecBusDie1;
  UINT32    Xgmac0_Base;
  UINT32    Xgmac1_Base;
  UINT32    Xgmac2_Base;
  UINT32    Xgmac3_Base;
  UINT32    Xgmac4_Base;
  UINT32    Xgmac5_Base;
  UINT32    Xgmac6_Base;
  UINT32    Xgmac7_Base;
  UINT32    Xpcs0_Base;
  UINT32    Xpcs1_Base;
  UINT32    Xpcs2_Base;
  UINT32    Xpcs3_Base;
  UINT32    Xpcs4_Base;
  UINT32    Xpcs5_Base;
  UINT32    Xpcs6_Base;
  UINT32    Xpcs7_Base;
  UINT32    XgmacBase;
  UINT8     i;
  //UINT8     port; //PS-67---
  UINT32    Value;
  UINT16    DataSize;
  UINT16    Data;
  UINT8     SlaveAdd;
  UINT32    XgbeBaseAdd;
  UINT8     Offset;
  EFI_PCI_IO_PROTOCOL *PciIo;//PS-67++for multi xgbe ports
  UINT64     Seg, Bus, Dev, Func; //PS-67++for multi xgbe ports
  UINT32    XpcsBase; //PS-67+++
  
  UINT8 PortNum;//PS-67+++

  XgmacBase = 0;
  Xgmac0_Base = 0;
  Xgmac1_Base = 0;
  Xgmac2_Base = 0;
  Xgmac3_Base = 0;
  Xgmac4_Base = 0;
  Xgmac5_Base = 0;
  Xgmac6_Base = 0;
  Xgmac7_Base = 0;
  Xpcs0_Base = 0;
  Xpcs1_Base = 0;
  Xpcs2_Base = 0;
  Xpcs3_Base = 0;
  Xpcs4_Base = 0;
  Xpcs5_Base = 0;
  Xpcs6_Base = 0;
  Xpcs7_Base = 0;
  BusNumberBase = 0;
  SocketNum = 0;
  DieNum = 0;
  BusMasterEn = 0;
  SecBusDie0 = 0;
  SecBusDie1 = 0;
  Status = EFI_SUCCESS;

  //PS-67++++
  if (Private->DeviceHandle == NULL) {
    DEBUG((DEBUG_INFO, "PS-67: xgbe controller device handle can not be NULL\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  Status=gBS->HandleProtocol(Private->DeviceHandle,&EfiPciIoProtocolGuid,(VOID**)&PciIo);
  if (EFI_ERROR(Status)) 
  {
      DEBUG((DEBUG_INFO, "PS-67:can not open gEfiPciIoProtocol for multi xgbe function\n"));
      return Status;
  }
  
  Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
  
  if  ( EFI_ERROR(Status) ) {
      DEBUG((DEBUG_INFO, "PS-67:can not get location for multi xgbe function\n"));
      return  EFI_UNSUPPORTED;
  }
  
  if (PcdGet8(PcdPlatformSelect) == HYGON_DIESEL) {
      SocketNum = 0; //For SLT, only one socket
      switch(Bus)
      {
      case 0x02: //xgbe bus number on fieldcard 0 is 0x02
          DieNum = 0;
          DEBUG((DEBUG_INFO, "PS-67:Fieldcard 0\n"));
          break;
      default:
          //add configuration for xgbe on other fieldcard
          break;
      }
  }
  
  //Get Base address for XGMAC and  XPCS
  // Port 0, 1, 2, & 3 are on Socket 0, Die0
  //Get the Base BUS# first.
  Status = gBS->LocateProtocol (
                &gAmdFabricTopologyServicesProtocolGuid,
                NULL,
                &FabricTopology
                );

  if (FabricTopology->GetRootBridgeInfo (FabricTopology, SocketNum, DieNum, 0, NULL, &BusNumberBase, NULL) != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }
  DEBUG ((EFI_D_ERROR, "BusNumberBase: %x \n", BusNumberBase));
  gBS->Stall (10);

  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_1, 0x19));
  DEBUG ((EFI_D_ERROR, "PciAddress.AddressValue: %x \n", PciAddress.AddressValue));
  gBS->Stall (10);
 // LibAmdPciRead (AccessWidth8, PciAddress, &SecBusDie0, NULL);
  SecBusDie0 = PciRead8 (PciAddress.AddressValue);


  //Port 0
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_4, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_4, 0x10));
  Xgmac0_Base = PciRead32 (PciAddress.AddressValue);

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_4, 0x14));
  Xpcs0_Base = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "Xgmac0_Base: %x \n", Xgmac0_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs0_Base: %x \n", Xpcs0_Base));
  if ((Xgmac0_Base == 0xFFFFFFFF) || (Xpcs0_Base == 0xFFFFFFFF) ||
     (Xgmac2_Base == 0xFFFFFFFF) || (Xpcs2_Base == 0xFFFFFFFF)) {
    Status = EFI_DEVICE_ERROR;
    return Status;
  }
#if 0
  //port 1
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_5, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_5, 0x10));
  Xgmac1_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_5, 0x14));
  Xpcs1_Base = PciRead32 (PciAddress.AddressValue);

  DEBUG ((EFI_D_ERROR, "Xgmac1_Base: %x \n", Xgmac1_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs1_Base: %x \n", Xpcs1_Base));

  //port 2
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_6, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_6, 0x10));
  Xgmac2_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_6, 0x14));
  Xpcs2_Base = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "Xgmac2_Base: %x \n", Xgmac2_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs2_Base: %x \n", Xpcs2_Base));
  //port 3
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_7, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_7, 0x10));
  Xgmac3_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_7, 0x14));
  Xpcs3_Base = PciRead32 (PciAddress.AddressValue);

  DEBUG ((EFI_D_ERROR, "Xgmac3_Base: %x \n", Xgmac3_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs3_Base: %x \n", Xpcs3_Base));

  // Port 4, 5, 6, & 7 are on Socket 0, Die1
  DieNum++;
  if (FabricTopology->GetRootBridgeInfo (FabricTopology, SocketNum, DieNum, 0, NULL, &BusNumberBase, NULL) != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }
  DEBUG ((EFI_D_ERROR, "BusNumberBase for DIE 1: %x \n", BusNumberBase));

  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_1, 0x19));
  SecBusDie1 = PciRead8 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "SecBusDie1: %x \n", SecBusDie1));

  //Port 4
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_4, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_4, 0x10));
  Xgmac4_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_4, 0x14));
  Xpcs4_Base = PciRead32 (PciAddress.AddressValue);

  DEBUG ((EFI_D_ERROR, "Xgmac4_Base: %x \n", Xgmac4_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs4_Base: %x \n", Xpcs4_Base));

  //port 5
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_5, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_5, 0x10));
  Xgmac5_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_5, 0x14));
  Xpcs5_Base = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "Xgmac5_Base: %x \n", Xgmac5_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs5_Base: %x \n", Xpcs5_Base));
  //port 6
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_6, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_6, 0x10));
  Xgmac6_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_6, 0x14));
  Xpcs6_Base = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "Xgmac6_Base: %x \n", Xgmac6_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs6_Base: %x \n", Xpcs6_Base));
  //port 7
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_7, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_7, 0x10));
  Xgmac7_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie1, 0x0, FUNC_7, 0x14));
  Xpcs7_Base = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "Xgmac7_Base: %x \n", Xgmac7_Base));
  DEBUG ((EFI_D_ERROR, "Xpcs7_Base: %x \n", Xpcs7_Base));
  
  //assign addresses for each port. Change for port 0 only
  Private->xgmac_pdata.xgmac_regs = Xgmac2_Base;
  Private->xpcs_regs = Xpcs2_Base;
  Private->XgmacBus = SecBusDie0;
  DEBUG ((EFI_D_ERROR, "XGmac = %x \n Xpcs = %x \n Bus = %x \n", Private->xgmac_pdata.xgmac_regs, Private->xpcs_regs, Private->XgmacBus ));
  Private->i2c_regs = Private->XgmacBus + XGBE_I2C_CTRL_OFFSET;

  
  
   //Program for XGBE ECC Interrupt.
  for (port = 0; port <= 7; port++) {
    if (port == 0)    XgmacBase = Xgmac0_Base;
    if (port == 1)    XgmacBase = Xgmac1_Base;
    if (port == 2)    XgmacBase = Xgmac2_Base;
    if (port == 3)    XgmacBase = Xgmac3_Base;
    if (port == 4)    XgmacBase = Xgmac4_Base;
    if (port == 5)    XgmacBase = Xgmac5_Base;
    if (port == 6)    XgmacBase = Xgmac6_Base;
    if (port == 7)    XgmacBase = Xgmac7_Base;
      DEBUG ((EFI_D_ERROR, "XGmac Start ECC programming  \n" ));
    MmioWrite32 (XgmacBase + 0x1008, 0x103);
    while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    MmioWrite32 (XgmacBase + 0x1010, 0x0);
    //write rx_fifo
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x380f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x80f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x180f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x340f);
      do {
          Value = MmioRead32 (XgmacBase + 0x100C);
          Value = getBitfieldReg32 (XgmacBase + 0x100C, 0, 1);
      } while (Value != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x40f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x140f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }
    MmioWrite32 (XgmacBase + 0x1d030, 0x3f);
  }
#else
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, Func, 0x4));
  BusMasterEn = (UINT16)PciRead32 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite32 (PciAddress.AddressValue, *((UINT32 *) &BusMasterEn));

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, Func, 0x10));
  XgbeBaseAdd = PciRead32 (PciAddress.AddressValue);

  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, Func, 0x14));
  XpcsBase = PciRead32 (PciAddress.AddressValue);
  DEBUG ((EFI_D_ERROR, "PS-67: XgbeBaseAdd: %x \n", XgbeBaseAdd));
  DEBUG ((EFI_D_ERROR, "PS-67: XpcsBase: %x \n", XpcsBase));
  if ((XgbeBaseAdd == 0xFFFFFFFF) || (XpcsBase == 0xFFFFFFFF)){
    Status = EFI_DEVICE_ERROR;
    return Status;
  }
  
  Private->xgmac_pdata.xgmac_regs = XgbeBaseAdd;
  Private->xpcs_regs = XpcsBase;
  Private->XgmacBus = SecBusDie0;
  DEBUG ((EFI_D_ERROR, "XGmac = %x \n Xpcs = %x \n Bus = %x \n", Private->xgmac_pdata.xgmac_regs, Private->xpcs_regs, Private->XgmacBus ));
  Private->i2c_regs = Private->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  
  PortNum = (UINT8)getBitfieldReg32 (XgbeBaseAdd + ENET_PORT_PROPERTY_0, 0, 8);      //Port Number
  DEBUG ((EFI_D_ERROR, "PS-67: PortNum = 0x%x \n", PortNum));
  
  //Program for XGBE ECC Interrupt.
     DEBUG ((EFI_D_ERROR, "XGmac Start ECC programming  \n" ));
     MmioWrite32 (XgbeBaseAdd + 0x1008, 0x103);
     while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
     MmioWrite32 (XgbeBaseAdd + 0x1010, 0x0);
     //write rx_fifo
     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x380f);
       while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
     }

     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x80f);
       while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
     }
     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x180f);
       while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
     }

     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x340f);
       do {
           Value = MmioRead32 (XgbeBaseAdd + 0x100C);
           Value = getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1);
       } while (Value != 0);
       Value = MmioRead32 (XgbeBaseAdd + 0x1010);
     }

     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x40f);
       while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
       Value = MmioRead32 (XgbeBaseAdd + 0x1010);
     }
     for (i = 0; i <= 3; i++) {
       MmioWrite32 (XgbeBaseAdd + 0x1008, 0x140f);
       while (getBitfieldReg32 (XgbeBaseAdd + 0x100C, 0, 1) != 0);
       Value = MmioRead32 (XgbeBaseAdd + 0x1010);
     }
     MmioWrite32 (XgbeBaseAdd + 0x1d030, 0x3f);

#endif

   // SlaveAdd = PCA9555_ADDRESS + getBitfieldReg32 (XgbeData->Xgmac0BaseAdd+ENET_PORT_PROPERTY_3, 8, 3);  //GPIO_ADDR
  DEBUG ((EFI_D_ERROR, "PS-67: Get I2C mutex \n"));
  //GetI2cOwnership (XgmacBase, PortNum);   
  
  SlaveAdd = 0x23;  //Slave address for Field Card = 0x23, 2nd Slot Field card = 0x24, GearBox = 0x26
  DataSize = sizeof (UINT16);
  XgbeBaseAdd = Xgmac0_Base; //PS-67---set value forward
  Offset = 6;
  Data = XgbeReadEeprom (XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  DEBUG ((EFI_D_ERROR, "Device23 Data Offset = 6 before = %x \n", Data ));
  Data &= 0x7777;
  XgbeWriteEeprom (XgbeBaseAdd, SlaveAdd, Offset, Data);
  Data = 0;
  Data = XgbeReadEeprom (XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  DEBUG ((EFI_D_ERROR, "Device23 Data Offset =6 after = %x \n", Data ));

  Offset = 2;
  Data = XgbeReadEeprom (XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  DEBUG ((EFI_D_ERROR, "Device23 Offset = 2, Data before = %x \n", Data )); //PS-67+++
  Data &= 0x7777;
  XgbeWriteEeprom (XgbeBaseAdd, SlaveAdd, Offset, Data);

  Data = 0;
  Data = XgbeReadEeprom (XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  DEBUG ((EFI_D_ERROR, "Device23 Data after 2 = %x \n", Data ));

  //PutI2cOwnership (XgmacBase, PortNum);
  DEBUG ((EFI_D_ERROR, "PS-67: Put I2C mutex \n"));
#if 0
  //2ND Field Card SLOT:
  SlaveAdd = 0x24;  //Slave address for Field Card = 0x23, 2nd Slot Field card = 0x24, GearBox = 0x26
  DataSize = sizeof (UINT16);
  Offset = 6;
  Data = XgbeReadEeprom ( XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  Data &= 0x7777;
  XgbeWriteEeprom (XgbeBaseAdd, SlaveAdd, Offset, Data);
  Data = 0;  //PS-67+++
  Data = XgbeReadEeprom ( XgbeBaseAdd, SlaveAdd, Offset, DataSize); //PS-67+++
  DEBUG ((EFI_D_ERROR, "Device24 Offset = 6, Data after = %x \n", Data )); //PS-67+++
  
  Offset = 2;
  Data = XgbeReadEeprom (XgbeBaseAdd, SlaveAdd, Offset, DataSize);
  DEBUG ((EFI_D_ERROR, "Device24 Offset = 2, Data before = %x \n", Data )); //PS-67+++
  Data &= 0x7777;
  XgbeWriteEeprom (XgbeBaseAdd, SlaveAdd, Offset, Data);
  Data = 0;  //PS-67+++
  Data = XgbeReadEeprom ( XgbeBaseAdd, SlaveAdd, Offset, DataSize); //PS-67+++
  DEBUG ((EFI_D_ERROR, "Device24 Offset = 2, Data after = %x \n", Data )); //PS-67+++
#endif
  return Status;
}

VOID
STATIC
xgbe_phy_start_ratechange (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;

  if (!getBitfieldReg32 (XgmacBase + 0x1d064, 0, 1)) {
    DEBUG ((EFI_D_ERROR, "firmware mailbox GOOD for ratechange command \n"));
	gBS->Stall (10);
    return;
  }
  DEBUG ((EFI_D_ERROR, "firmware mailbox not ready for command \n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_complete_ratechange (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN wait;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;

  //Wait for command to complete
  wait = XGBE_RATECHANGE_COUNT;
  while (wait--) {
    if (!getBitfieldReg32 (XgmacBase + 0x1d064, 0, 1)) {
      DEBUG ((EFI_D_ERROR, "rate Change Successfull \n"));
      return;
    }
    gBS->Stall (100);
  }
  DEBUG ((EFI_D_ERROR, "firmware mailbox command did not complete \n"));
}

VOID
STATIC
xgbe_phy_power_off (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  pdata->xprop_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_MAC_CTRL_OFFSET;
  pdata->xi2c_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_power_off start \n"));
  gBS->Stall (10);
  xgbe_phy_start_ratechange (pdata);

  // Call FW to make the change

  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_0, 0);
  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_1, 0);
  XP_IOWRITE_BITS (pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_UNKNOWN;
  DEBUG ((EFI_D_ERROR, "phy powered off\n"));
  gBS->Stall (10);
}

UINTN
STATIC
xgbe_phy_get_comm_ownership (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN  mutex_id;
  UINTN MutexMdio;
  UINTN MutexI2c;
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_get_comm_ownership\n"));
  gBS->Stall (10);
  if (phy_data->comm_owned) {
    return 0;
  }
  // Clear the mutexes
  XP_IOWRITE (pdata, XP_I2C_MUTEX, XGBE_MUTEX_RELEASE);
  XP_IOWRITE (pdata, XP_MDIO_MUTEX, XGBE_MUTEX_RELEASE);

  mutex_id = 0;
  XP_SET_BITS (mutex_id, XP_I2C_MUTEX, ID, phy_data->port_id);
  XP_SET_BITS (mutex_id, XP_I2C_MUTEX, ACTIVE, 1);

  do {
    MutexI2c = XP_IOREAD (pdata, XP_I2C_MUTEX);
  MutexMdio = XP_IOREAD (pdata, XP_MDIO_MUTEX);
  gBS->Stall (100);

  } while (MutexMdio || MutexI2c);

  if ((MutexMdio == 0) && (MutexI2c == 0)) {
    // Obtain the mutex
    DEBUG ((EFI_D_ERROR, "Recieved the hardware mutexes\n"));
	gBS->Stall (10);
    XP_IOWRITE (pdata, XP_I2C_MUTEX, mutex_id);
    XP_IOWRITE (pdata, XP_MDIO_MUTEX, mutex_id);
    phy_data->comm_owned = 1;
    return 0;
  }
  DEBUG ((EFI_D_ERROR, "unable to obtain hardware mutexes\n"));
  gBS->Stall (10);
  return 1;
}

VOID
STATIC
xgbe_phy_put_comm_ownership (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  phy_data->comm_owned = 0;
//  mutex_unlock (&xgbe_phy_comm_lock);
}

UINTN
STATIC
xgbe_i2c_abort (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN wait;
  wait = XGBE_ABORT_COUNT;
  // Must be enabled to recognize the abort request
  XI2C_IOWRITE_BITS (pdata, IC_ENABLE, EN, 1);
  // Issue the abort
  XI2C_IOWRITE_BITS (pdata, IC_ENABLE, ABORT, 1);
  while (wait--) {
    if (!XI2C_IOREAD_BITS (pdata, IC_ENABLE, ABORT)) {
      return 0;
    }
    gBS->Stall (100);
  }
  return 1;
}

UINTN
STATIC
xgbe_i2c_set_enable (
  ENET_DXE_PRIVATE_DATA *pdata,
  BOOLEAN enable
  )
{

  UINTN    wait;
  UINTN    mode;
  wait = XGBE_DISABLE_COUNT;
  mode = enable ? 1 : 0;

  while (wait--) {
    XI2C_IOWRITE_BITS (pdata, IC_ENABLE, EN, mode);
    if (XI2C_IOREAD_BITS (pdata, IC_ENABLE_STATUS, EN) == mode) {
      return 0;
    }
    gBS->Stall (100);
  }

  return 1;
}

UINTN
STATIC
xgbe_i2c_disable (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN ret;
  ret = xgbe_i2c_set_enable (pdata, FALSE);
  if (ret) {
    // Disable failed, try an abort
    ret = xgbe_i2c_abort (pdata);
    if (ret) {
      return ret;
    }
       // Abort succeeded, try to disable again */
    ret = xgbe_i2c_set_enable (pdata, FALSE);
  }
  return ret;
}

VOID
STATIC
xgbe_i2c_set_target (
  ENET_DXE_PRIVATE_DATA *pdata, UINTN addr
  )
{
  XI2C_IOWRITE (pdata, IC_TAR, addr);
}

VOID
STATIC
xgbe_i2c_clear_all_interrupts (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XI2C_IOREAD (pdata, IC_CLR_INTR);
}

VOID
STATIC
xgbe_i2c_disable_interrupts (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XI2C_IOWRITE (pdata, IC_INTR_MASK, 0);
}

VOID
STATIC
xgbe_i2c_enable_interrupts (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XI2C_IOWRITE (pdata, IC_INTR_MASK, XGBE_DEFAULT_INT_MASK);
}

UINTN
STATIC
xgbe_i2c_enable (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  return xgbe_i2c_set_enable (pdata, TRUE);
}

EFI_STATUS
xgbe_phy_redrv_write (
  ENET_DXE_PRIVATE_DATA *pdata,
  UINTN reg,
  UINTN val
  )
{
  //XGBE_PHY_DATA *phy_data = pdata->phy_data;
  XGBE_I2C_MASTER_PROTOCOL  *XgbEI2cMaster;
  UINT8                     PacketBuffer[sizeof (EFI_I2C_REQUEST_PACKET) + sizeof (EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
  UINTN                     SlaveAdd;
  UINT16                    redrv_val;
  UINT8                     redrv_data[5];
  UINT8                     csum;
  UINT8                     i;
  UINT32                    XgmacBaseI2c;
  Status = EFI_SUCCESS;
  SlaveAdd = 0x40; //phy_data->redrv_addr;  //0x40

  for (i = 0; i < 5; i++) {
    redrv_data[i] = 0;
  }
  //XgmacBaseI2c = pdata->xgmac_pdata.xgmac_regs + 0x1E000;  //PS-67---mark here
  XgmacBaseI2c = 0xEFBE0000 + 0x1E000;//PS-67++
  Status = gBS->LocateProtocol (&gxGbEI2cMasterProtocolGuid, NULL, &XgbEI2cMaster);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC Workaround: Could not Locate gxGbEI2cMasterProtocolGuid\n"));
    gBS->Stall (100);
  }
  XgbEI2cMaster->BaseAddress = XgmacBaseI2c;
  XgbEI2cMaster->I2cInit (XgbEI2cMaster);

  // High byte of register contains read/write indicator
  redrv_data[0] = ((reg >> 8) & 0xff) << 1;
  redrv_data[1] = reg & 0xff;
  redrv_val = *(UINT16 *)&redrv_data[2];
  redrv_data[3] = (UINT8) val;
//  redrv_data[3] = 0x9;

  //Calculate 1 byte checksum
  csum = 0;
  for (i = 0; i < 4; i++) {
    csum += redrv_data[i];
    if (redrv_data[i] > csum) {
      csum++;
    }
  }
  redrv_data[4] = ~csum;
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;

  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof (redrv_data);
  Operation[0].Buffer           = redrv_data;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof (UINT8);
  Operation[1].Buffer           = redrv_data;

  Status = XgbEI2cMaster->StartRequest (XgbEI2cMaster, SlaveAdd, RequestPacket, NULL, NULL);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to StartRequest.\n"));
	gBS->Stall (10);
  }
  if (redrv_data[0] != 0xff) {
    DEBUG ((EFI_D_ERROR, "XGMAC: Redriver write checksum error\n"));
	gBS->Stall (10);
  }
  DEBUG ((EFI_D_ERROR, "XGMAC: redrv_data  = %0x \n", redrv_data[0]));
  gBS->Stall (10);
  return Status;
}

VOID
xgbe_phy_set_redrv_mode_i2c (
  ENET_DXE_PRIVATE_DATA *pdata,
  enum xgbe_phy_redrv_mode mode
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINTN redrv_reg;
  EFI_STATUS                Status;
  Status;

  // Calculate the register to write
  redrv_reg = XGBE_PHY_REDRV_MODE_REG + (phy_data->redrv_lane * 0x1000);
  Status = xgbe_phy_redrv_write (pdata, redrv_reg, mode);
}

VOID
STATIC
xgbe_phy_set_redrv_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  enum xgbe_phy_redrv_mode mode;
  UINTN ret;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_set_redrv_mode \n"));
  gBS->Stall (10);
  if (!phy_data->redrv) {
    return;
  }
  mode = XGBE_PHY_REDRV_MODE_CX;
  if ((phy_data->port_mode == XGBE_PORT_MODE_SFP) &&
        (phy_data->sfp_base != XGBE_SFP_BASE_1000_CX) &&
        (phy_data->sfp_base != XGBE_SFP_BASE_10000_CR)) {
    mode = XGBE_PHY_REDRV_MODE_SR;
    DEBUG ((EFI_D_ERROR, "Setting Redrv Mode set to XGBE_PHY_REDRV_MODE_SR\n"));
	gBS->Stall (10);
  }
  ret = xgbe_phy_get_comm_ownership (pdata);
  if (ret) {
    DEBUG ((EFI_D_ERROR, "Setting Redrv Mode ERROR1 Returning back\n"));
	gBS->Stall (10);
    return;
  }

  if (phy_data->redrv_if) {
    xgbe_phy_set_redrv_mode_i2c (pdata, mode);
  }
// Gearbox  else
// Gearbox  xgbe_phy_set_redrv_mode_mdio(pdata, mode);

//Gearbox xgbe_phy_put_comm_ownership(pdata);
  phy_data->comm_owned = 0;
}

UINTN
STATIC
xgbe_phy_mdio_reset (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
//XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINTN ret;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_mdio_reset  \n"));
  gBS->Stall (10);

//dolater  if (phy_data->conn_type != XGBE_CONN_TYPE_MDIO)
//dolater return 0;
  ret = xgbe_phy_get_comm_ownership (pdata);
  if (ret) {
    return ret;
  }

#if 0   //SKIP GEAR BOX for now.
//gearbox stuff
  if (phy_data->mdio_reset == XGBE_MDIO_RESET_I2C_GPIO) {
    ret = xgbe_phy_i2c_mdio_reset (pdata);
  } else if (phy_data->mdio_reset == XGBE_MDIO_RESET_INT_GPIO) {
    ret = xgbe_phy_int_mdio_reset (pdata);
  }
#endif
  xgbe_phy_put_comm_ownership (pdata);
  return ret;
}
VOID
STATIC
xgbe_phy_sfi_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 s0;
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_x_mode\n"));
  gBS->Stall (10);
  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

  // 10G/SFI
  s0 = 0;
  DEBUG ((EFI_D_ERROR, "PS-67: sfp_cable = %d\n", phy_data->sfp_cable ));
  DEBUG ((EFI_D_ERROR, "PS-67: sfp_cable_len = %d\n", phy_data->sfp_cable_len ));
  XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, COMMAND, 3);
  if (phy_data->sfp_cable != XGBE_SFP_CABLE_PASSIVE) {
    XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 0);
  } else {
    if (phy_data->sfp_cable_len <= 1) {
      XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 1);
    } else if (phy_data->sfp_cable_len <= 3) {
        XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 2);
    } else if (phy_data->sfp_cable_len <= 5) {
        XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 3);
    } else
      XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 3);
    }

  // Call FW to make the change. XP_IOWRITE does not work. Later.
  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_0, s0);
  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_1, 0);
  XP_IOWRITE_BITS (pdata, XP_DRIVER_INT_REQ, REQUEST, 1);
  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_SFI;
  DEBUG ((EFI_D_ERROR, "10GbE SFI mode set\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_x_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT16 s0;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_x_mode\n"));
  gBS->Stall (10);
  xgbe_phy_set_redrv_mode (pdata);

  xgbe_phy_start_ratechange (pdata);
  s0 = 0x0301;
//  s0 = 0x0004; //For LiangJiansheng -PS-67
  MmioWrite32 (XgmacBase + 0x1d068, s0);   //scratch 0
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);

// s0 = 0;
//  XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, COMMAND, 1);
//  XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 3);

// Call FW to make the change
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_0, s0);
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_1, 0);
//  XP_IOWRITE_BITS(pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);

  phy_data->cur_mode = XGBE_MODE_X;
  //phy_data->cur_mode = XGBE_MODE_SFI; //PS-67-For Liangjiansheng
  
  //DEBUG ((EFI_D_ERROR, "1GbE X mode set\n"));
  gBS->Stall (10);
}
VOID
STATIC
xgbe_phy_sgmii_1000_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT16 s0;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_sgmii_1000_mode\n"));
  gBS->Stall (10);

  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

  s0 = 0x0201;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);



//  // 1G/SGMII
// s0 = 0;
//   XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, COMMAND, 1);
//  XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 2);
//  DEBUG ((EFI_D_ERROR, "sgmii_1000 values to FW = %x\n", s0));
// Call FW to make the change
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_0, s0);
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_1, 0);
//  XP_IOWRITE_BITS(pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_SGMII_1000;
  DEBUG ((EFI_D_ERROR, "1GbE SGMII mode set\n"));
  gBS->Stall (10);
}

VOID
xgbe_phy_sgmii_100_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT32 s0;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;

  DEBUG ((EFI_D_ERROR, "xgbe_phy_sgmii_100_mode\n"));
  gBS->Stall (10);
  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

  s0 = 0x0101;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);


// 1G/SGMII
// s0 = 0;
// XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, COMMAND, 1);
// XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 1);

// Call FW to make the change
// XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_0, s0);
// XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_1, 0);
// XP_IOWRITE_BITS(pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_SGMII_100;
  DEBUG ((EFI_D_ERROR, "100MbE SGMII mode set\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_kx_1000_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT32 s0;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_kx_1000_mode\n"));
  gBS->Stall (10);

  pdata->xprop_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_MAC_CTRL_OFFSET;
  pdata->xi2c_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  DEBUG ((EFI_D_ERROR, "pdata->xprop_regs = %x \n", pdata->xprop_regs));
  gBS->Stall (10);

  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

// 1G/KX
  s0 = 0x0301;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);

 //  s0 = 0;
  //XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, COMMAND, 1);
 // XP_SET_BITS(s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 3);

// Call FW to make the change
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_0, s0);
//  XP_IOWRITE(pdata, XP_DRIVER_SCRATCH_1, 0);
//  XP_IOWRITE_BITS(pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_KX_1000;
  DEBUG ((EFI_D_ERROR, "1GbE KX mode set\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_kx_2500_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 s0;
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_kx_2500_mode\n"));
  gBS->Stall (10);

  pdata->xprop_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_MAC_CTRL_OFFSET;
  pdata->xi2c_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  DEBUG ((EFI_D_ERROR, "pdata->xprop_regs = %x \n", pdata->xprop_regs));
  gBS->Stall (10);

  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

// 2.5G/KX
  s0 = 0x0002;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);


//s0 = 0;
// XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, COMMAND, 2);
// XP_SET_BITS (s0, XP_DRIVER_SCRATCH_0, SUB_COMMAND, 0);
// Call FW to make the change
//  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_0, s0);
//  XP_IOWRITE (pdata, XP_DRIVER_SCRATCH_1, 0);
//  XP_IOWRITE_BITS (pdata, XP_DRIVER_INT_REQ, REQUEST, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_KX_2500;
  DEBUG ((EFI_D_ERROR, "2.5GbE KX mode set\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_kr_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 s0;
  XGBE_PHY_DATA *phy_data = pdata->phy_data;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  DEBUG ((EFI_D_ERROR, "xgbe_phy_kr_mode\n"));
  gBS->Stall (10);

  pdata->xprop_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_MAC_CTRL_OFFSET;
  pdata->xi2c_regs = pdata->xgmac_pdata.xgmac_regs + XGBE_I2C_CTRL_OFFSET;
  DEBUG ((EFI_D_ERROR, "pdata->xprop_regs = %x \n", pdata->xprop_regs));
  gBS->Stall (10);

  xgbe_phy_set_redrv_mode (pdata);
  xgbe_phy_start_ratechange (pdata);

  // 10G/KR
  s0 = 0x0004;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);

  xgbe_phy_complete_ratechange (pdata);
  phy_data->cur_mode = XGBE_MODE_KR;
  DEBUG ((EFI_D_ERROR, "10GbE KR mode set\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_phy_set_mode (
  ENET_DXE_PRIVATE_DATA *pdata,
  enum xgbe_mode mode
  )
{
  switch (mode) {
  case XGBE_MODE_KX_1000:
    xgbe_phy_kx_1000_mode (pdata);
    break;
  case XGBE_MODE_KX_2500:
    xgbe_phy_kx_2500_mode (pdata);
    break;
  case XGBE_MODE_KR:
    xgbe_phy_kr_mode (pdata);
    break;
  case XGBE_MODE_SGMII_100:
    xgbe_phy_sgmii_100_mode (pdata);
    break;
  case XGBE_MODE_SGMII_1000:
    xgbe_phy_sgmii_1000_mode (pdata);
    break;
  case XGBE_MODE_X:
    xgbe_phy_x_mode (pdata);
    break;
  case XGBE_MODE_SFI:
    DEBUG ((EFI_D_ERROR, "PS-67: Set highest supported mode to XGBE_MODE_SFI\n"));
    xgbe_phy_sfi_mode (pdata);
    break;
  default:
    break;
  }
}

VOID
STATIC
xgbe_phy_rrc (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 s0;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_pdata.xgmac_regs;
  xgbe_phy_start_ratechange (pdata);

//  if (!XP_IOREAD_BITS (pdata, XP_DRIVER_INT_RO, STATUS))
//  return;
//  DEBUG ((EFI_D_ERROR, "firmware mailbox not ready for command \n"));

// Receiver Reset Cycle
  s0 = 0x0005;
  MmioWrite32 (XgmacBase + 0x1d068, s0);
  MmioWrite32 (XgmacBase + 0x1d06c, 0);
  MmioWrite32 (XgmacBase + 0x1d060, 1);

  xgbe_phy_complete_ratechange (pdata);
  DEBUG ((EFI_D_ERROR, "receiver reset complete\n"));
  gBS->Stall (10);
}

UINTN
read_mmd_regs (
  IN       UINTN XpcsBase,
  IN       UINTN mmd_reg
  )
{

  UINTN mmd_address;
  UINTN mmd_data;
  UINTN XpcsWin;
  UINTN XpcsWinSize;
  UINTN XpcsWinMask;
  UINTN reg;
  UINTN index;
  UINTN offset;

  reg = XPCS_IOREAD32 (XpcsBase, PCS_V2_WINDOW_DEF);
  XpcsWin = XPCS_GET_BITS (reg, PCS_V2_WINDOW_DEF, OFFSET);
  XpcsWin <<= 6;
  XpcsWinSize = XPCS_GET_BITS (reg, PCS_V2_WINDOW_DEF, SIZE);
  XpcsWinSize = 1ULL << (XpcsWinSize + 7);
  XpcsWinMask = XpcsWinSize - 1;
  if (mmd_reg & MII_ADDR_C45) {
    mmd_address = mmd_reg & ~MII_ADDR_C45;
  } else
    mmd_address = (MDIO_MMD_PCS << 16) | (mmd_reg & 0xffff);

//The PCS registers are accessed using mmio. The underlying
// management interface uses indirect addressing to access the MMD
// register sets. This requires accessing of the PCS register in two
// phases, an address phase and a data phase.

//The mmio interface is based on 16-bit offsets and values. All
//register offsets must therefore be adjusted by left shifting the
//offset 1 bit and reading 16 bits of data.

  mmd_address <<= 1;
  index = mmd_address & ~XpcsWinMask;
  offset = XpcsWin + (mmd_address & XpcsWinMask);
//  spin_lock_irqsave (&pdata->xpcs_lock, flags);
  XPCS_IOWRITE32 (XpcsBase, PCS_V2_WINDOW_SELECT, (UINT32)index);
  mmd_data = XPCS_IOREAD16 (XpcsBase, offset);
//  spin_unlock_irqrestore (&pdata->xpcs_lock, flags);

  return mmd_data;
}

VOID
STATIC
write_mmd_regs (
  IN       UINTN XpcsBase,
  IN       UINTN mmd_reg,
  IN       UINTN mmd_data
  )
{
  UINTN mmd_address;
  UINTN XpcsWin;
  UINTN XpcsWinSize;
  UINTN XpcsWinMask;
  UINTN reg;
  UINTN index;
  UINTN offset;

  reg = XPCS_IOREAD32 (XpcsBase, PCS_V2_WINDOW_DEF);
  XpcsWin = XPCS_GET_BITS (reg, PCS_V2_WINDOW_DEF, OFFSET);
  XpcsWin <<= 6;
  XpcsWinSize = XPCS_GET_BITS (reg, PCS_V2_WINDOW_DEF, SIZE);
  XpcsWinSize = 1ULL << (XpcsWinSize + 7);
  XpcsWinMask = XpcsWinSize - 1;

  if (mmd_reg & MII_ADDR_C45) {
    mmd_address = mmd_reg & ~MII_ADDR_C45;
  } else mmd_address = (MDIO_MMD_PCS << 16) | (mmd_reg & 0xffff);

  mmd_address <<= 1;
  index = mmd_address & ~XpcsWinMask;
  offset = XpcsWin + (mmd_address & XpcsWinMask);

  XPCS_IOWRITE32 (XpcsBase, PCS_V2_WINDOW_SELECT, (UINT32)index);
  XPCS_IOWRITE16 (XpcsBase, offset, (UINT16)mmd_data);
}

VOID
STATIC
xgmac_pcs_reset (
  IN       ENET_DXE_PRIVATE_DATA *pdata
  )
{
//XGBE_PHY_DATA *phy_data = pdata->phy_data;

//  enum xgbe_mode cur_mode;
  UINTN  ret;
  xgbe_x_mode (pdata);

  //Reset by power cycling the PHY */
//  cur_mode = phy_data->cur_mode;
//  xgbe_phy_power_off (pdata);
//  xgbe_phy_sgmii_1000_mode (pdata);
//  xgbe_phy_x_mode (pdata);
//  xgbe_phy_kx_1000_mode (pdata);
//  xgbe_phy_set_mode (pdata, cur_mode);

//  Reset the external PHY
  ret = xgbe_phy_mdio_reset (pdata);
  if (ret) {
    DEBUG ((EFI_D_ERROR, "***ERROR xgbe_phy_mdio_reset  \n"));
	gBS->Stall (10);
  }
}

VOID
STATIC
xgmac_pcs_enable_kr_training (
  IN  UINTN XpcsBase
  )
{
  // Enable KR training
  UINTN reg;
  UINTN count;
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
  reg |= MDIO_PMA_10GBR_CTRL_KRTRAIN;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL, reg);
  count = 50;
  do {
    gBS->Stall (100);
    reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
  } while  ((reg & MDIO_PMA_10GBR_CTRL) && --count);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
}

VOID
STATIC
xgmac_pcs_disable_kr_training (
  IN       UINTN XpcsBase
  )
{
  UINTN reg;
    // Disable KR training
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
  reg &= ~MDIO_PMA_10GBR_CTRL_KRTRAIN;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL, reg);
  gBS->Stall (100);
}

VOID
STATIC
xgmac_restart_kr_training (
  IN  UINTN XpcsBase
  )
{
  UINTN reg;
  // set adapter_reset

  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
  reg |= MDIO_PMA_10GBR_CTRL_KRTRAIN_START;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL, reg);
  gBS->Stall (100);

//  XMDIO_WRITE_BITS (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL,
//       MDIO_PMA_10GBR_CTRL_KRTRAIN_START, MDIO_PMA_10GBR_CTRL_KRTRAIN_START);
  // release adapter_reset

}

VOID
STATIC
xgmac_pcs_power_cycle (
  IN       UINTN XpcsBase
  )
{
  UINTN reg;
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_CTRL1);
  reg |= MDIO_CTRL1_LPOWER;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PCS, MDIO_CTRL1, reg);
  gBS->Stall (100);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_CTRL1);
  reg &= 0xF7FF;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PCS, MDIO_CTRL1, reg);
}

VOID
STATIC
xgmac_start_ratechange (
  IN  UINT32 XgmacBase
  )
{
  UINT32 reg;
  do {
    reg = getBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_RES, 0, 1);
  } while (reg != 0);
}

VOID
STATIC
xgmac_pcs_gmii_10_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase
  )
{

  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 0x1);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0x0);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);
}

VOID
STATIC
xgmac_pcs_gmii_100_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase

  )
{
  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 0x1);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0x1);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);
}

VOID
STATIC
xgmac_pcs_gmii_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase

  )
{

  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 0x1);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0x2);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);

 // XGMAC_IOWRITE32 (pdata, MC_DRIVER_MB_S0, 1);
 // XGMAC_IOWRITE32 (pdata, MC_DRIVER_MB_S1, 0);

}

VOID
STATIC
xgmac_pcs_gmii_1GKR_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase

  )
{

  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 0x1);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0x3);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);
}

VOID
STATIC
xgmac_pcs_gmii_2500_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase

  )
{
//  xgmac_pcs_gmii_speed (XpcsBase, XgmacBase);
  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 2);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);

}

VOID
STATIC
xgbe_pcs_xgmii_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase
  )
{
  xgmac_pcs_enable_kr_training (XpcsBase);
  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 3);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);
  DEBUG ((EFI_D_ERROR, "X--xgbe_pcs_xgmii_speed\n"));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_pcs_xgmii_kr_speed (
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase
  )
{
  xgmac_pcs_enable_kr_training (XpcsBase);
  xgmac_start_ratechange (XgmacBase);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S0, 4);
  MmioWrite32 (XgmacBase + MC_DRIVER_MB_S1, 0);
  setBitfieldReg32 (XgmacBase + MC_DRIVER_MB_INT_REQ, 0, 1, 1);
  xgmac_start_ratechange (XgmacBase);
  DEBUG ((EFI_D_ERROR, "X--xgbe_pcs_xgmii_speed\n"));
  gBS->Stall (10);
}


EFI_STATUS
xgbe_test_kr_train_sts (
  IN  UINTN XpcsBase
  )
{
  UINTN   RegVal;
  // test if training is complete
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_STS);
  if (RegVal & MDIO_PMA_10GBR_STS_TR_FAIL) {
    return EFI_DEVICE_ERROR;
  }
  if (RegVal & MDIO_PMA_10GBR_STS_RCV_STS) {
    // now check LD and LP Receiver Ready ( from KR Coefficient Status reg )
    if ((MDIO_PMA_10GBR_CE_RR & XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_LP_CESTS)) &&
      (MDIO_PMA_10GBR_CE_RR & XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_LD_CESTS))) {
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_READY;
}

EFI_STATUS
xgbe_check_kr_train (
  IN  UINTN XpcsBase
  )
{
  UINTN reg;
  EFI_STATUS  Status;
  // Check KR TR enabled
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
  if ((reg & MDIO_PMA_10GBR_CTRL_KRTRAIN) != MDIO_PMA_10GBR_CTRL_KRTRAIN) {
    DEBUG ((EFI_D_ERROR, "::KR training was disabled, enabling it\n"));
	gBS->Stall (10);
    reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL);
    reg |= MDIO_PMA_10GBR_CTRL_KRTRAIN;
    XMDIO_WRITE (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_CTRL, reg);
  }
  Status = xgbe_test_kr_train_sts (XpcsBase);
  return Status;
}

BOOLEAN
IsLpPresent (
  IN  ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINTN RegVal;
  UINTN AnIntReg;
  UINTN AnLp2;
  UINTN WaitCount;
  UINTN XpcsBase;
  XpcsBase = Private->xpcs_regs;
  // Clear interrupt (s)
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
  gBS->Stall (100);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);

  // Restart AN
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1);
  RegVal |= (MDIO_AN_CTRL1_ENABLE | MDIO_AN_CTRL1_RESTART);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1, RegVal);

  // Wait for interrupt - PG_RCV
  WaitCount = 0;
  while (WaitCount < 10000) {
  // 1000 mS
    AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );

    if (AnIntReg & AN_INC_LNK) {
      return FALSE;
    }
    if (AnIntReg & AN_PG_RCV) {
      // Clear PG_RCV
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
      gBS->Stall (100);
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));

      // Verify LP
      AnLp2 = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_AN_LP_ABL2);
//      if (AnLp2 & 0xA0)  return TRUE;
      if (AnLp2 & 0x20) {
        return TRUE;
      }
    }
    gBS->Stall (100);
    WaitCount++;
  } // end of wait for INT

  return  FALSE;
}


VOID
EFIAPI
LinkStsTimerEventHandler (
  IN EFI_EVENT    Timer,
  IN VOID         *Context
  )
{
  UINTN RegVal;
  UINTN XpcsBase;
  UINT32 XgmacBase;
  ENET_DXE_PRIVATE_DATA *Private;
  Private =  (ENET_DXE_PRIVATE_DATA*)Context;
  XpcsBase = Private->xpcs_regs;
   XgmacBase = Private->xgmac_pdata.xgmac_regs;

  // Stop timer for period of executing this handler
  if ( LinkStsTimerEvent != NULL) {
    gBS->SetTimer (LinkStsTimerEvent, TimerCancel, 0);
  }

  // This part originally for 1GB int, which happens after AN
  RegVal = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
  if (RegVal != OldIntVal) {
    OldIntVal = RegVal;
    if (RegVal != 0) {
      // Clear interrupt(s)
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
      gBS->Stall (100);
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
    }
  }

  // Check link status
  UpdateLinkStatus (Private);
  if (!Private->Mode.MediaPresent) {
    DEBUG ((EFI_D_ERROR, "MediaPresent FAIL   \n"));
	gBS->Stall (10);
    // Check if LP present
    if (IsLpPresent (Private)) {
      xgbe_an_run (Private);
    }
  }

  // Restart timer at the end of event handler
  if ( LinkStsTimerEvent != NULL) {
    gBS->SetTimer (
                    LinkStsTimerEvent,
                    TimerPeriodic,
                    500 * 10000U    // 500 * (100nS * 10000) mS
                    );
  }
}

VOID
StartTmr (
  IN  ENET_DXE_PRIVATE_DATA *Private
  )
{

  gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    LinkStsTimerEventHandler,
                    (VOID*)Private,
                    &LinkStsTimerEvent
                    );

  if ( LinkStsTimerEvent != NULL) {
    gBS->SetTimer (
                   LinkStsTimerEvent,
                   TimerPeriodic,
                   500 * 10000U    // 500 * (100nS * 10000) mS
                   );
  }

  OldIntVal = 0;
}

VOID
STATIC
xgbe_an73_init (
  ENET_DXE_PRIVATE_DATA *pdata,
  UINT32 XpcsBase
  )
{

  UINTN reg;
  //unsigned int advertising, reg;
  //  Currently Forcing for CL73
  DEBUG ((EFI_D_ERROR, "xgbe_an73_init \n"));
  gBS->Stall (10);
  //  advertising = pdata->phy_if.phy_impl.an_advertising(pdata);
  //  Set up Advertisement register 3 first
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0012);
  DEBUG ((EFI_D_ERROR, "AN Register 3 BEFORE Programming= %04X \n", reg));
  gBS->Stall (10);
  // if (advertising & ADVERTISED_10000baseR_FEC)
  //reg |= 0xc000;
  //else
  reg &= ~0xc000;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, 0x0012, reg);

  // Set up Advertisement register 2 next
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0011);
  DEBUG ((EFI_D_ERROR, "AN Register 2 BEFORE Programming= %04X \n", reg));
  gBS->Stall (10);
  //  if (advertising & ADVERTISED_10000baseKR_Full)
  //  reg |= 0x80;
  //  else
  reg &= ~0x80;
  //  if ((advertising & ADVERTISED_1000baseKX_Full) ||
  //   (advertising & ADVERTISED_2500baseX_Full))
  reg |= 0x20;
  //  else
  //  reg &= ~0x20;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, 0x0011, reg);

  //  Set up Advertisement register 1 last
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0010);
  DEBUG ((EFI_D_ERROR, "AN Register 1 BEFORE Programming= %04x \n", reg));
  gBS->Stall (10);
  //  if (advertising & ADVERTISED_Pause)
  //    reg |= 0x400;
  //   else
  reg &= ~0x400;
  //  if (advertising & ADVERTISED_Asym_Pause)
  //  reg |= 0x800;
  //  else
  reg &= ~0x800;

  // We don't intend to perform XNP
  reg &= ~XGBE_XNP_NP_EXCHANGE;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, 0x0010, reg);
  DEBUG ((EFI_D_ERROR, "CL73 AN initialized \n"));
  gBS->Stall (10);
}

AN_NEXT_STEP
run_an_step (
  IN  ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32 XpcsBase,
  IN       UINT32 XgmacBase
  )
{
  UINTN WaitCount;
  UINTN RegVal;
  UINTN AnLp2;
  UINTN AnIntReg;
  xgbe_an73_disable (Private);
  // Clear interrupt(s)
  DEBUG ((EFI_D_ERROR, " Clear interrupts\n"));
  gBS->Stall (10);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
  gBS->Stall (1000);
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, 0);
  DEBUG ((EFI_D_ERROR, " Program 3 of AN registers\n"));
  gBS->Stall (10);
  xgbe_an73_init (Private, XpcsBase);

  //Eable Interrupt
  xgbe_an73_enable_interrupts (XpcsBase);
  // Restart AN
  DEBUG ((EFI_D_ERROR, " Restart Auto Negotiation\n"));
  gBS->Stall (10);
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1);
  RegVal |= (MDIO_AN_CTRL1_ENABLE | MDIO_AN_CTRL1_RESTART);    //Set Bit 9 and 12 of the SR AN MMD Control Register to 1
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1, RegVal);
  gBS->Stall (100);
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0012);
  DEBUG ((EFI_D_ERROR, "AN Register 3 After Programming= %04x \n", RegVal));
  gBS->Stall (10);
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0011);
  DEBUG ((EFI_D_ERROR, "AN Register 2 After Programming= %04x \n", RegVal));
  gBS->Stall (10);
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_AN, 0x0010);
  DEBUG ((EFI_D_ERROR, "AN Register 1 After Programming= %04x \n", RegVal));
  gBS->Stall (10);

  // Wait for interrupt - PG_RCV
  WaitCount = 0;
  while (WaitCount < 10000) {
    gBS->Stall (100);
    AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
    // DEBUG ((EFI_D_ERROR, "PG_RCV AnIntReg = %x\n", AnIntReg));
    if (AnIntReg & AN_INC_LNK) {
      AnLp2 = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_AN_LP_ABL2);
    DEBUG ((EFI_D_ERROR, "AnLp2 = %x\n", AnLp2));
	gBS->Stall (10);
    AnLp2 &= 0x20;              //Forcing to settle for 1G only.
      if (AnLp2 & 0x80) {
        xgbe_pcs_xgmii_kr_speed (XpcsBase, XgmacBase);
      }
      return RESTART_AN;
    }
    if (AnIntReg & AN_PG_RCV) {
      // Clear PG_RCV
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
      gBS->Stall (100);
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
      return READ_LP;
    }
    gBS->Stall (100);
    WaitCount++;
  } // end of wait for INT

  return RESTART_AN;
}

AN_NEXT_STEP
read_lp_step (
  IN  UINTN XpcsBase,
  IN  ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINTN AnLp2;
  UINTN AnIntReg;
  AnLp2 = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_AN_LP_ABL2);
  AnLp2&= 0x20;               //Forcing to settle for 1G only
  if (AnLp2 & 0x80) {
    // LP supports KR - 10GB
    switch ( mCurrLanSpeed) {
    case 10:
      break;

    case 1:
      mCurrLanSpeed = 10;
      XpcsInit (Private);
      XgmacSetSpeed (Private, Private->xgmac_pdata.xgmac_regs);
      return RESTART_AN;             // and restart AN

    case 2:
      if (AnLp2 & 0x20) {
        return WAIT_INT_CMPLT;
      }
      return EXIT_AN_RUN;

    default:
      DEBUG ((EFI_D_ERROR, "::Unsupported LP speed\n"));
	    gBS->Stall (10);
      return EXIT_AN_RUN;
    }
    // Check for Fault
    AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
    if (AnIntReg & (AN_INC_LNK | AN_INT_CMPLT)) {
      return RESTART_AN;
    }

    return KR_TRAIN;
  } else if (AnLp2 & 0x20) {
    // LP supports KX - 1GB
    switch (mCurrLanSpeed) {
    case  1:
    case  2:
      break;

    case  10:
      mCurrLanSpeed = 1;
      XpcsInit (Private);
      XgmacSetSpeed (Private, Private->xgmac_pdata.xgmac_regs);
      return RESTART_AN;

    default:
      DEBUG ((EFI_D_ERROR, "::Unsupported LP speed\n"));
	    gBS->Stall (10);
      return EXIT_AN_RUN;
    }
    return WAIT_INT_CMPLT;
  } else {
    // unsupported
    DEBUG ((EFI_D_ERROR, "::Unsupported LP speed\n"));
	gBS->Stall (10);
    return EXIT_AN_RUN;
  }
}

AN_NEXT_STEP
kr_train_step (
  IN  UINTN XpcsBase
  )
{
  UINTN AnIntReg;
  AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
  if (AnIntReg & AN_PG_RCV) {
    XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
    gBS->Stall (100);
    XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
  }

  if (EFI_DEVICE_ERROR == xgbe_check_kr_train (XpcsBase)) {
    return RESTART_AN;
  }
  AnIntReg = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT );
  if (AnIntReg & AN_INC_LNK) {
    return RESTART_AN;
  }
  return WAIT_INT_CMPLT;
}

AN_NEXT_STEP
wait_int_cmplt_step (
  IN  UINTN XpcsBase
  )
{
  UINTN WaitCount;
  UINTN AnIntReg;
  // Wait for INT_CMPLT
  WaitCount = 0;
  while (WaitCount < 10000) {
  // 1000 mS
    AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
    if (AnIntReg & AN_PG_RCV) {
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
      gBS->Stall (100);
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_PG_RCV));
    }
    AnIntReg = XMDIO_READ (XpcsBase,  MDIO_MMD_AN, MDIO_AN_INT );
    if (AnIntReg & AN_INC_LNK) {
      return RESTART_AN;
    }
    if (AnIntReg & AN_INT_CMPLT) {
    // Clear INT_CMPLT
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_INT_CMPLT));
      gBS->Stall (100);
      XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_AN_INT, (AnIntReg & ~AN_INT_CMPLT));
      return EXIT_AN_RUN;
    }
    gBS->Stall (100);
    WaitCount++;
  }
  // if we are here - no INT_CMPLT received
  return RESTART_AN;
}

VOID
xgbe_an_run (
  IN  ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINT32 XpcsBase;
  UINT32 XgmacBase;
  UINTN AnCycleCount;
  UINTN RegVal;
  AN_NEXT_STEP NextStep;
  XpcsBase = Private->xpcs_regs;
  XgmacBase = Private->xgmac_pdata.xgmac_regs;

  for (AnCycleCount = 0; AnCycleCount < AN_MAX_RETR; AnCycleCount++) {
    NextStep = run_an_step (Private, XpcsBase, XgmacBase);
    if ( NextStep == RESTART_AN ) {
      continue;
    }
    NextStep = read_lp_step (XpcsBase, Private);
    if ( NextStep == EXIT_AN_RUN ) {
      break;
    }
    if ( NextStep == RESTART_AN ) {
      continue;
    }
    if ( NextStep == KR_TRAIN ) {
      NextStep = kr_train_step (XpcsBase);
      if ( NextStep == RESTART_AN ) {
        continue;
      }
    }
    NextStep = wait_int_cmplt_step (XpcsBase);
    if ( NextStep == EXIT_AN_RUN ) {
      break;
    }
  }

  DEBUG ((EFI_D_ERROR, "::LINK SPEED : %d  PCS LINK STS - ", mCurrLanSpeed));
  gBS->Stall (10);
  // Get and display Link status from PCS register

  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_PCS_STS1);
  gBS->Stall (1000);
  RegVal = XMDIO_READ (XpcsBase, MDIO_MMD_PCS, MDIO_PCS_STS1);

  if ((RegVal & PCS_STS1_RLU) != 0) {
    DEBUG ((EFI_D_ERROR, "UP\n"));
	gBS->Stall (10);
  } else {
    DEBUG ((EFI_D_ERROR, "DOWN\n"));
	gBS->Stall (10);
  }
  DEBUG ((EFI_D_ERROR, "STS1 RegVal : %x ", RegVal));
  gBS->Stall (10);
  StartTmr (Private);
}

VOID
STATIC
xgbe_an73_set (
  UINTN XpcsBase,
  BOOLEAN enable,
  BOOLEAN restart
  )
{
  UINTN reg;
  DEBUG ((EFI_D_ERROR, "xgbe_an73_set\n" ));
  gBS->Stall (10);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1);
  reg &= ~MDIO_AN_CTRL1_ENABLE;

  if (enable) {
    reg |= MDIO_AN_CTRL1_ENABLE;
  }
  if (restart) {
    reg |= MDIO_AN_CTRL1_RESTART;
  }
  XMDIO_WRITE (XpcsBase, MDIO_MMD_AN, MDIO_CTRL1, reg);
}

VOID
STATIC
xgbe_an37_set (
  UINTN XpcsBase,
  BOOLEAN enable,
  BOOLEAN restart
  )
{
  UINTN reg;
  DEBUG ((EFI_D_ERROR, "xgbe_an37_set\n" ));
  gBS->Stall (10);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_VEND2, MDIO_CTRL1);
  reg &= ~MDIO_VEND2_CTRL1_AN_ENABLE;

  if (enable) {
    reg |= MDIO_VEND2_CTRL1_AN_ENABLE;
  }
  if (restart) {
    reg |= MDIO_VEND2_CTRL1_AN_RESTART;
  }
  XMDIO_WRITE (XpcsBase, MDIO_MMD_VEND2, MDIO_CTRL1, reg);
}


VOID
STATIC
xgbe_an73_disable (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN XpcsBase;
  DEBUG ((EFI_D_ERROR, "xgbe_an73_disable\n" ));
  gBS->Stall (10);
  XpcsBase = pdata->xpcs_regs;
  xgbe_an73_set (XpcsBase, FALSE, FALSE);
  xgbe_an73_disable_interrupts (XpcsBase);
  DEBUG ((EFI_D_ERROR, "CL73 AN disabled\n" ));
  gBS->Stall (10);

}


VOID
STATIC
xgbe_an37_disable (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINTN XpcsBase;
  DEBUG ((EFI_D_ERROR, "xgbe_an37_disable\n" ));
  gBS->Stall (10);
  XpcsBase = pdata->xpcs_regs;
  xgbe_an37_set (XpcsBase, FALSE, FALSE);
  xgbe_an37_disable_interrupts (XpcsBase);
  DEBUG ((EFI_D_ERROR, "CL37 AN disabled\n" ));
  gBS->Stall (10);
}

VOID
STATIC
xgbe_an_disable (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  DEBUG ((EFI_D_ERROR, "xgbe_an_disable\n" ));
  gBS->Stall (10);
  xgbe_an73_disable (pdata);
  xgbe_an37_disable (pdata);

// Later-- Forcing for CL73 for now....
//DEBUG ((EFI_D_ERROR, "xgbe_an_disable\n" ));
//DEBUG ((EFI_D_ERROR, "pdata->an_mode = %x\n", pdata->an_mode ));
//  switch (pdata->an_mode) {
//  case XGBE_AN_MODE_CL73:
//  case XGBE_AN_MODE_CL73_REDRV:
//    xgbe_an73_disable(pdata);
//    break;
//  case XGBE_AN_MODE_CL37:
//  case XGBE_AN_MODE_CL37_SGMII:
//    xgbe_an37_disable(pdata);
//    break;
//  default:
//   break;
//  }

}

VOID
XpcsInit (
  IN OUT   ENET_DXE_PRIVATE_DATA *Private
  )
{
  xgmac_pcs_reset (Private);
}

VOID xgmac_get_all_hw_features (
  IN  xgmac_prv_data *pdata
  )
{
  UINT32  mac_hfr0;
  UINT32  mac_hfr1;
  UINT32  mac_hfr2;
  xgmac_hw_features *hw_feat = &pdata->hw_feat;

  mac_hfr0 = XGMAC_IOREAD32 (pdata, MAC_HWF0R);
  mac_hfr1 = XGMAC_IOREAD32 (pdata, MAC_HWF1R);
  mac_hfr2 = XGMAC_IOREAD32 (pdata, MAC_HWF2R);
  SetMem (hw_feat, sizeof (pdata->hw_feat), 0);
  hw_feat->version = XGMAC_IOREAD32 (pdata, MAC_VR);

  // Hardware feature register 0
  hw_feat->gmii        = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, GMIISEL);
  hw_feat->vlhash      = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, VLHASH);
  hw_feat->sma         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, SMASEL);
  hw_feat->rwk         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, RWKSEL);
  hw_feat->mgk         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, MGKSEL);
  hw_feat->mmc         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, MMCSEL);
  hw_feat->aoe         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, ARPOFFSEL);
  hw_feat->ts          = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, TSSEL);
  hw_feat->eee         = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, EEESEL);
  hw_feat->tx_coe      = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, TXCOESEL);
  hw_feat->rx_coe      = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, RXCOESEL);
  hw_feat->addn_mac    = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R,
                            ADDMACADRSEL);
  hw_feat->ts_src      = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, TSSTSSEL);
  hw_feat->sa_vlan_ins = XGMAC_GET_BITS (mac_hfr0, MAC_HWF0R, SAVLANINS);

  // Hardware feature register 1
  hw_feat->rx_fifo_size  = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R,
                            RXFIFOSIZE);
  hw_feat->tx_fifo_size  = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R,
                             TXFIFOSIZE);
  hw_feat->adv_ts_hi     = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, ADVTHWORD);
  hw_feat->dma_width     = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, ADDR64);
  hw_feat->dcb           = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, DCBEN);
  hw_feat->sph           = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, SPHEN);
  hw_feat->tso           = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, TSOEN);
  hw_feat->dma_debug     = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, DBGMEMA);
  hw_feat->rss           = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, RSSEN);
  hw_feat->tc_cnt        = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R, NUMTC);
  hw_feat->hash_table_size = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R,
                               HASHTBLSZ);
  hw_feat->l3l4_filter_num = XGMAC_GET_BITS (mac_hfr1, MAC_HWF1R,
                              L3L4FNUM);

  // Hardware feature register 2
  hw_feat->rx_q_cnt     = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, RXQCNT);
  hw_feat->tx_q_cnt     = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, TXQCNT);
  hw_feat->rx_ch_cnt    = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, RXCHCNT);
  hw_feat->tx_ch_cnt    = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, TXCHCNT);
  hw_feat->pps_out_num  = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, PPSOUTNUM);
  hw_feat->aux_snap_num = XGMAC_GET_BITS (mac_hfr2, MAC_HWF2R, AUXSNAPNUM);

  //Translate the Hash Table size into actual number */
  switch (hw_feat->hash_table_size) {
    case 0:
      break;
    case 1:
      hw_feat->hash_table_size = 64;
      break;
    case 2:
      hw_feat->hash_table_size = 128;
      break;
    case 3:
      hw_feat->hash_table_size = 256;
      break;
  }

  // Translate the address width setting into actual number
  switch (hw_feat->dma_width) {
    case 0:
      hw_feat->dma_width = 32;
      break;
    case 1:
      hw_feat->dma_width = 40;
      break;
    case 2:
    hw_feat->dma_width = 48;
    break;
    default:
      hw_feat->dma_width = 32;
  }

  //The Queue, Channel and TC counts are zero based so increment them
  // to get the actual number

  hw_feat->rx_q_cnt++;
  hw_feat->tx_q_cnt++;
  hw_feat->rx_ch_cnt++;
  hw_feat->tx_ch_cnt++;
  hw_feat->tc_cnt++;

  // Translate the fifo sizes into actual numbers
  hw_feat->rx_fifo_size = 1 << (hw_feat->rx_fifo_size + 7);
  hw_feat->tx_fifo_size = 1 << (hw_feat->tx_fifo_size + 7);
}

VOID
xgmac_default_config (
  IN       xgmac_prv_data *pdata
  )
{
  pdata->pblx8 = 1;
  pdata->tx_sf_mode = XGMAC_TSF_ENABLE;
  pdata->tx_threshold = XGMAC_TX_THRESHOLD_64;
  pdata->tx_pbl = XGMAC_PBL_16;
  pdata->tx_osp_mode = 1;
  pdata->rx_sf_mode = XGMAC_RSF_DISABLE;
  pdata->rx_threshold = XGMAC_RX_THRESHOLD_64;
  pdata->rx_pbl = XGMAC_PBL_16;
  pdata->flow_ctrl = XGMAC_FLOW_CTRL_TX_RX;
  pdata->oldflow_ctrl = XGMAC_FLOW_CTRL_TX_RX;
  pdata->power_down = 0;

  if (mCurrLanSpeed == 10) {
    pdata->phy_interface = PHY_INTERFACE_MODE_XGMII;
    pdata->default_autoneg = AUTONEG_DISABLE;
    pdata->default_speed = SPEED_10000;
  } else if (mCurrLanSpeed == 1) {
    pdata->phy_interface = PHY_INTERFACE_MODE_GMII;
    pdata->default_autoneg = AUTONEG_DISABLE;
    pdata->default_speed = SPEED_1000;
  } else if (mCurrLanSpeed == 2) {
    pdata->phy_interface = PHY_INTERFACE_MODE_GMII;
    pdata->default_autoneg = AUTONEG_DISABLE;
    pdata->default_speed = SPEED_2500;
  } else {
    ASSERT (0 && "Unsupported LAN speed value");
  }

}

UINT32
STATIC
flush_tx_queues (
  IN  xgmac_prv_data *pdata
  )
{
  UINT32  i;
  UINT32  count;
   DEBUG ((EFI_D_ERROR, "-->flush_tx_queues\n"));
   gBS->Stall (10);

  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, FTQ, 0x1);
  }
  // Poll Until Poll Condition
  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    count = 2000;
    while (count-- && XGMAC_MTL_IOREAD32_BITS (pdata, i,
                MTL_Q_TQOMR, FTQ))
      gBS->Stall (1000);

    if (!count) {
      return (UINT32) - 1;
    }
  }
   DEBUG ((EFI_D_ERROR, "X--flush_tx_queues\n"));
   gBS->Stall (10);
  return 0;
}

VOID
STATIC
config_dma_bus (
  IN       xgmac_prv_data *pdata
  )
{
  // Set enhanced addressing mode
  XGMAC_IOWRITE32_BITS (pdata, DMA_SBMR, EAME, 0x1);
  // Set the System Bus mode
  XGMAC_IOWRITE32_BITS (pdata, DMA_SBMR, UNDEF, 0x1);
  XGMAC_IOWRITE32_BITS (pdata, DMA_SBMR, BLEN_256, 0x1);
}

VOID
STATIC
config_dma_cache (
  IN  xgmac_prv_data *pdata
  )
{
  UINT32  arcache;
  UINT32  awcache;

  arcache = 0;
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, DRC, DMA_ARCACHE_SETTING_CE);
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, DRD, DMA_ARDOMAIN_SETTING_CE);
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, TEC, DMA_ARCACHE_SETTING_CE);
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, TED, DMA_ARDOMAIN_SETTING_CE);
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, THC, DMA_ARCACHE_SETTING_CE);
  XGMAC_SET_BITS (arcache, DMA_AXIARCR, THD, DMA_ARDOMAIN_SETTING_CE);

  XGMAC_IOWRITE32 (pdata, DMA_AXIARCR, arcache);

  awcache = 0;
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, DWC, DMA_AWCACHE_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, DWD, DMA_AWDOMAIN_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, RPC, DMA_AWCACHE_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, RPD, DMA_AWDOMAIN_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, RHC, DMA_AWCACHE_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, RHD, DMA_AWDOMAIN_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, TDC, DMA_AWCACHE_SETTING_CE);
  XGMAC_SET_BITS (awcache, DMA_AXIAWCR, TDD, DMA_AWDOMAIN_SETTING_CE);

  XGMAC_IOWRITE32 (pdata, DMA_AXIAWCR, awcache);
}

INT32
STATIC
set_promiscuous_mode (
  IN  xgmac_prv_data *pdata,
  IN  UINT32 enable
  )
{
  UINT32 val;

  val = enable ? 1 : 0;
  if (XGMAC_IOREAD32_BITS (pdata, MAC_PFR, PR) == val) {
    return 0;
  }

  XGMAC_IOWRITE32_BITS (pdata, MAC_PFR, PR, val);
  return 0;
}

INT32
STATIC
set_all_multicast_mode (
  IN       xgmac_prv_data *pdata,
  IN       UINT32 enable
  )
{
  UINT32 val;
  val = enable ? 1 : 0;
  if (XGMAC_IOREAD32_BITS (pdata, MAC_PFR, PM) == val) {
    return 0;
  }
  XGMAC_IOWRITE32_BITS (pdata, MAC_PFR, PM, val);
  return 0;
}

INT32
STATIC
set_addn_mac_addrs (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32 am_mode
  )
{
  xgmac_prv_data *pdata;
  UINT32 mac_reg;
  UINT32 mac_addr_hi;
  UINT32 mac_addr_lo;
  UINT32 mac_addr_tmp;
  UINT8 *mac_addr;
  UINT32 i;

  pdata = &Private->xgmac_pdata;
  XGMAC_IOWRITE32_BITS (pdata, MAC_PFR, HUC, 0);
  XGMAC_IOWRITE32_BITS (pdata, MAC_PFR, HMC, 0);

  i = 0;
  mac_addr_tmp = 0;
  mac_reg = MAC_MACA1HR;

  if (!am_mode) {
    mac_addr_lo = mac_addr_hi = 0;
    mac_addr = (UINT8 *)&mac_addr_lo;
    mac_addr_tmp = Private->DevicePath.EthDevicePath.MacAddress.Addr[0];
    SET_BIT_FIELD (mac_addr_tmp, 0, 1, 1);
    mac_addr[0] = (UINT8)mac_addr_tmp;
    mac_addr[1] = Private->DevicePath.EthDevicePath.MacAddress.Addr[1];
    mac_addr[2] = Private->DevicePath.EthDevicePath.MacAddress.Addr[2];
    mac_addr[3] = Private->DevicePath.EthDevicePath.MacAddress.Addr[3];
    mac_addr = (UINT8 *)&mac_addr_hi;
    mac_addr[0] = Private->DevicePath.EthDevicePath.MacAddress.Addr[4];
    mac_addr[1] = Private->DevicePath.EthDevicePath.MacAddress.Addr[5];

    XGMAC_SET_BITS (mac_addr_hi, MAC_MACA1HR, AE, 1);

    XGMAC_IOWRITE32 (pdata, mac_reg, mac_addr_hi);
    mac_reg += MAC_MACA_INC;
    XGMAC_IOWRITE32 (pdata, mac_reg, mac_addr_lo);
    mac_reg += MAC_MACA_INC;
  } else {
    mac_addr_lo = mac_addr_hi = 0;
    mac_addr = (UINT8 *)&mac_addr_lo;
    mac_addr_tmp = Private->DevicePath.EthDevicePath.MacAddress.Addr[0];
    SET_BIT_FIELD (mac_addr_tmp, 0, 1, 0);
    mac_addr[0] = (UINT8)mac_addr_tmp;
    mac_addr[1] = Private->DevicePath.EthDevicePath.MacAddress.Addr[1];
    mac_addr[2] = Private->DevicePath.EthDevicePath.MacAddress.Addr[2];
    mac_addr[3] = Private->DevicePath.EthDevicePath.MacAddress.Addr[3];
    mac_addr = (UINT8 *)&mac_addr_hi;
    mac_addr[0] = Private->DevicePath.EthDevicePath.MacAddress.Addr[4];
    mac_addr[1] = Private->DevicePath.EthDevicePath.MacAddress.Addr[5];

    XGMAC_SET_BITS (mac_addr_hi, MAC_MACA1HR, AE, 1);
    XGMAC_IOWRITE32 (pdata, mac_reg, mac_addr_hi);
    mac_reg += MAC_MACA_INC;
    XGMAC_IOWRITE32 (pdata, mac_reg, mac_addr_lo);
    mac_reg += MAC_MACA_INC;
  }
  // Clear remaining additional MAC address entries
  for (; i < pdata->hw_feat.addn_mac; i++) {
    XGMAC_IOWRITE32 (pdata, mac_reg, 0);
    mac_reg += MAC_MACA_INC;
    XGMAC_IOWRITE32 (pdata, mac_reg, 0);
    mac_reg += MAC_MACA_INC;
  }
  return 0;
}

VOID
STATIC
xgmac_set_rx_mode (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINT32 pr_mode;
  UINT32 am_mode;

  pr_mode = 1;
  am_mode = 1;

  DEBUG ((EFI_D_ERROR, "-->xgmac_set_rx_mode\n"));
  gBS->Stall (10);

  set_promiscuous_mode (&Private->xgmac_pdata, pr_mode);
  set_all_multicast_mode (&Private->xgmac_pdata, am_mode);
  if (!pr_mode) {
    set_addn_mac_addrs (Private, am_mode);
  }
  DEBUG ((EFI_D_ERROR, "X--xgmac_set_rx_mode\n"));
  gBS->Stall (10);
}

INT32
STATIC
config_osp_mode (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;

  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_TCR, OSP, pdata->tx_osp_mode);
  return 0;
}

INT32
STATIC
config_pblx8 (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;

  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_CR, PBLX8, pdata->pblx8);

  return 0;
}

INT32
STATIC
config_tx_pbl_val (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;

  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_TCR, PBL, pdata->tx_pbl);

  return 0;
}

INT32
STATIC
config_rx_pbl_val (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;

  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_RCR, PBL, pdata->rx_pbl);

  return 0;
}

INT32
STATIC
config_rx_coalesce (
  IN      xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  pdata->rx_use_riwt = (pdata->rx_riwt || pdata->rx_frames) ? 1 : 0;
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_RIWT, RWT, pdata->rx_riwt);
  return 0;
}

VOID
STATIC
config_rx_buffer_size (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_RCR, RBSZ, pdata->rx_buf_size);
}

VOID
STATIC
config_tso_mode (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_TCR, TSE, 1);
}

VOID
STATIC
tx_desc_init (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  xgmac_channel *channel;

  channel = &Private->xgmac_pdata.channel;

  // Make sure everything is written to the descriptor(s) before
  // telling the device about them
  //
  Private->desc_tx_index = 0;   // Init descriptor index

  // Update the total no of Tx descriptors count
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_TDRLR, TX_DESC_CNT - 1);

  // Update the starting address of desc chain/ring
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_TDLR_HI,
          upper_32_bits (Private->desc_tx));
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_TDLR_LO,
          lower_32_bits (Private->desc_tx));
}

VOID
STATIC
rx_desc_init (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  xgmac_channel *channel;
  UINT32 rx_coalesce;
  UINT32 rx_frames;
  UINT32 i;
  UINT32 Index;

  channel = &Private->xgmac_pdata.channel;

  rx_coalesce = channel->pdata->rx_use_riwt;
  rx_frames = channel->pdata->rx_frames;

  // Initialize all descriptors
  for (i = 1; i <= RX_DESC_CNT; i++) {
    // Initialize Rx descriptor
    //   Set buffer 1 (lo) address to dma address (lo)
    //   Set buffer 1 (hi) address to dma address (hi)
    //   Set buffer 2 (lo) address to zero
    //   Set buffer 2 (hi) address to zero and set control
    //   bits OWN and INTE as appropriate
    //
    Index = (i - 1);
    Private->desc_rx[Index].RDES0 = lower_32_bits (Private->pkt_rx_dma[Index].DeviceAddress);
    Private->desc_rx[Index].RDES1 = upper_32_bits (Private->pkt_rx_dma[Index].DeviceAddress);
    Private->desc_rx[Index].RDES2 = 0;
    Private->desc_rx[Index].RDES3 = 0;
    XGMAC_SET_BITS (Private->desc_rx[Index].RDES3, RX_NORMAL_DESC3, OWN, 0x1);
    XGMAC_SET_BITS (Private->desc_rx[Index].RDES3, RX_NORMAL_DESC3, INTE, 0x1);
    //rdata->interrupt = 1;
    Private->desc_rx_interrupt[Index] = TRUE;
    if (rx_coalesce && (!rx_frames || (i % rx_frames))) {
      // Clear interrupt on completion bit
      XGMAC_SET_BITS (Private->desc_rx[Index].RDES3, RX_NORMAL_DESC3, INTE, 0);
      //rdata->interrupt = 0;
      Private->desc_rx_interrupt[Index] = FALSE;
    }
  }

  // Make sure everything is written to the descriptors before
  // telling the device about them
  //

  Private->desc_rx_index = 0;   // Init descriptor index

  // Update the total no of Rx descriptors count
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_RDRLR, RX_DESC_CNT - 1);

  // Update the starting address of desc chain/ring
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_RDLR_HI,
          upper_32_bits (Private->desc_rx));
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_RDLR_LO,
          lower_32_bits (Private->desc_rx));

  // Update the Rx Descriptor Tail Pointer
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_RDTR_LO,
          lower_32_bits (&Private->desc_rx[RX_DESC_CNT - 1]));
}

VOID
STATIC
enable_dma_interrupts (
  IN  xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  UINT32 dma_ch_isr;
  UINT32 dma_ch_ier;

  channel = &pdata->channel;
  // Clear all the interrupts which are set
  dma_ch_isr = XGMAC_DMA_IOREAD32 (channel, DMA_CH_SR);
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_SR, dma_ch_isr);

  // Clear all interrupt enable bits
  dma_ch_ier = 0;

  // Enable following interrupts
  //   NIE  - Normal Interrupt Summary Enable
  //   AIE  - Abnormal Interrupt Summary Enable
  //   FBEE - Fatal Bus Error Enable
  //
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, NIE, 1);
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, AIE, 1);
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, FBEE, 1);

  // Enable the following Tx interrupts
  //   TBUE - Transmit Buffer Unavailable Enable
  //   TXSE - Transmit Stopped Enable
  //   TIE  - Transmit Interrupt Enable (unless polling)
  //
  //XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, TBUE, 1);
  //XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, TXSE, 1);
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, TIE, 1);

  // Enable following Rx interrupts
  //   RSE  - Receive Stoppped Enable
  //   RBUE - Receive Buffer Unavailable Enable
  //   RIE  - Receive Interrupt Enable
  //
  //XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, RSE, 1);
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, RBUE, 1);
  XGMAC_SET_BITS (dma_ch_ier, DMA_CH_IER, RIE, 1);

  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_IER, dma_ch_ier);
}

VOID
STATIC
config_mtl_mode (
  IN  xgmac_prv_data *pdata
  )
{
  UINT32 i;

  // Set Tx to weighted round robin scheduling algorithm
  XGMAC_IOWRITE32_BITS (pdata, MTL_OMR, ETSALG, XGMAC_MTL_ETSALG_WRR);

  // Set Tx traffic classes to strict priority algorithm
  for (i = 0; i < XGBE_TC_CNT; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_TC_ETSCR, TSA, XGMAC_MTL_TSA_SP);
  }

  // Set Rx to strict priority algorithm
  XGMAC_IOWRITE32_BITS (pdata, MTL_OMR, RAA, XGMAC_MTL_RAA_SP);
}

VOID
STATIC
config_rx_queue_mapping (
  IN  xgmac_prv_data *pdata
  )
{
  UINT32 i;
  UINT32 reg;
  UINT32 reg_val;
  UINT32 q_count;

  q_count = pdata->hw_feat.rx_q_cnt;
  // Select dynamic mapping of MTL Rx queue to DMA Rx channel
  reg = MTL_RQDCM0R;
  reg_val = 0;
  for (i = 0; i < q_count;) {
    reg_val |= (0x80 << ((i++ % MTL_RQDCM_Q_PER_REG) << 3));

    if ((i % MTL_RQDCM_Q_PER_REG) && (i != q_count)) {
      continue;
    }

    XGMAC_IOWRITE32 (pdata, reg, reg_val);

    reg += MTL_RQDCM_INC;
    reg_val = 0;
  }

}

INT32
STATIC
config_tsf_mode (
  IN       xgmac_prv_data *pdata,
  IN       UINT32 val
  )
{
  UINT32 i;

  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, TSF, val);
  }

  return 0;
}

INT32
STATIC
config_rsf_mode (
  IN       xgmac_prv_data *pdata,
  IN       UINT32 val
  )
{
  UINT32 i;

  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, RSF, val);
  }

  return 0;
}

INT32
STATIC
config_tx_threshold (
  IN       xgmac_prv_data *pdata,
  IN       UINT32 val
  )
{
  UINT32 i;

  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, TTC, val);
  }

  return 0;
}

INT32
STATIC
config_rx_threshold (
  IN       xgmac_prv_data *pdata,
  IN       UINT32 val
  )
{
  UINT32 i;

  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, RTC, val);
  }

  return 0;
}


VOID
STATIC
config_tx_fifo_size (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 fifo_size;
  UINT32 i;

  if (pdata->hw_feat.tx_q_cnt != 0) {
    fifo_size = pdata->hw_feat.tx_fifo_size / pdata->hw_feat.tx_q_cnt / 256;
  } else {
    fifo_size = 0;
  }
  if (fifo_size >= 1) {
    fifo_size -= 1;
  }

  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, TQS, fifo_size);
  }

  DEBUG ((EFI_D_ERROR, "%d Tx queues, %d byte fifo per queue\n",
          pdata->hw_feat.tx_q_cnt, ((fifo_size + 1) * 256)));
  gBS->Stall (10);
}

VOID
STATIC
config_rx_fifo_size (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 fifo_size;
  UINT32 i;

  if (pdata->hw_feat.rx_q_cnt != 0) {
    fifo_size = pdata->hw_feat.rx_fifo_size / pdata->hw_feat.rx_q_cnt / 256;
  } else {
    fifo_size = 0;
  }
  if (fifo_size >= 1) {
    fifo_size -= 1;
  }

  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, RQS, fifo_size);
  }

  DEBUG ((EFI_D_ERROR, "%d Rx queues, %d byte fifo per queue\n",
          pdata->hw_feat.rx_q_cnt, ((fifo_size + 1) * 256)));
	gBS->Stall (10);
}

VOID
STATIC
config_flow_control_threshold (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 i;

  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    // Activate flow control when less than 4k left in fifo
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, RFA, 2);

    // De-activate flow control when more than 6k left in fifo
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, RFD, 4);
  }
}

VOID
STATIC
enable_mtl_interrupts (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 mtl_q_isr;
  UINT32 q_count;
  UINT32 i;

  q_count = MAX (pdata->hw_feat.tx_q_cnt, pdata->hw_feat.rx_q_cnt);
  for (i = 0; i < q_count; i++) {
    // Clear all the interrupts which are set
    mtl_q_isr = XGMAC_MTL_IOREAD32 (pdata, i, MTL_Q_ISR);
    XGMAC_MTL_IOWRITE32 (pdata, i, MTL_Q_ISR, mtl_q_isr);

    // No MTL interrupts to be enabled
    XGMAC_MTL_IOWRITE32 (pdata, i, MTL_Q_ISR, 0);
  }
}

VOID
STATIC
config_mac_address (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  UINT32 mac_hi;
  UINT32 mac_lo;
  xgmac_prv_data *pdata;

  pdata = &Private->xgmac_pdata;
  mac_hi = mac_lo = 0;
  mac_hi |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[5] << 8);
  mac_hi |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[4] << 0);
  mac_lo |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[3] << 24);
  mac_lo |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[2] << 16);
  mac_lo |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[1] << 8);
  mac_lo |= (Private->DevicePath.EthDevicePath.MacAddress.Addr[0] << 0);

  XGMAC_IOWRITE32 (pdata, MAC_MACA0HR, mac_hi);
  XGMAC_IOWRITE32 (pdata, MAC_MACA0LR, mac_lo);
}

VOID
STATIC
config_jumbo_enable (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 val;

  //val = (pdata->netdev->mtu > XGMAC_STD_PACKET_MTU) ? 1 : 0;
  val = 0;

  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, JE, val);
}

INT32
STATIC
disable_tx_flow_ctrl (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 max_q_count;
  UINT32 q_count;
  UINT32 reg;
  UINT32 reg_val;
  UINT32 i;
//  UINT32 z;

  // Clear MTL flow control
  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, EFHC, 0);
  }

  // Clear MAC flow control
  max_q_count = XGMAC_MAX_FLOW_CONTROL_QUEUES;
  q_count = min_t (pdata->hw_feat.rx_q_cnt, max_q_count);
  reg = MAC_Q0TFCR;
  for (i = 0; i < q_count; i++) {
    reg_val = XGMAC_IOREAD32 (pdata, reg);
    XGMAC_SET_BITS (reg_val, MAC_Q0TFCR, TFE, 0);
    XGMAC_IOWRITE32 (pdata, reg, reg_val);

    reg += MAC_QTFCR_INC;
  }

  return 0;
}

INT32
STATIC
enable_tx_flow_ctrl (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 max_q_count;
  UINT32 q_count;
  UINT32 reg;
  UINT32 reg_val;
  UINT32 i;

  // Set MTL flow control
  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_RQOMR, EFHC, 1);
  }

  // Set MAC flow control
  max_q_count = XGMAC_MAX_FLOW_CONTROL_QUEUES;
  q_count = min_t (pdata->hw_feat.rx_q_cnt, max_q_count);
  reg = MAC_Q0TFCR;
  for (i = 0; i < q_count; i++) {
    reg_val = XGMAC_IOREAD32 (pdata, reg);

    // Enable transmit flow control
    XGMAC_SET_BITS (reg_val, MAC_Q0TFCR, TFE, 1);
    // Set pause time
    XGMAC_SET_BITS (reg_val, MAC_Q0TFCR, PT, 0xffff);

    XGMAC_IOWRITE32 (pdata, reg, reg_val);

    reg += MAC_QTFCR_INC;
  }

  return 0;
}

INT32
STATIC
disable_rx_flow_ctrl (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_RFCR, RFE, 0);

  return 0;
}

INT32
STATIC
enable_rx_flow_ctrl (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_RFCR, RFE, 1);

  return 0;
}

VOID
STATIC
config_flow_control (
  IN       xgmac_prv_data *pdata
  )
{
  if (pdata->flow_ctrl & XGMAC_FLOW_CTRL_TX) {
    enable_tx_flow_ctrl (pdata);
  } else {
    disable_tx_flow_ctrl (pdata);
  }

  if (pdata->flow_ctrl & XGMAC_FLOW_CTRL_RX) {
    enable_rx_flow_ctrl (pdata);
  } else {
    disable_rx_flow_ctrl (pdata);
  }
}

INT32
STATIC
disable_rx_csum (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, IPC, 0);

  return 0;
}

INT32
STATIC
enable_rx_csum (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, IPC, 0x1);

  return 0;
}

VOID
STATIC
config_checksum_offload (
  IN       xgmac_prv_data *pdata
  )
{
  if (TRUE) {
    enable_rx_csum (pdata);
  } else {
    disable_rx_csum (pdata);
  }
}

VOID
STATIC
config_mmc (
  IN       xgmac_prv_data *pdata
  )
{
  // Set counters to reset on read
  XGMAC_IOWRITE32_BITS (pdata, MMC_CR, ROR, 1);

  // Reset the counters
  XGMAC_IOWRITE32_BITS (pdata, MMC_CR, CR, 1);
}

INT32
STATIC
xgbe_enable_rx_vlan_stripping (
  IN       xgmac_prv_data *pdata
  )
{
  // Put the VLAN tag in the Rx descriptor
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, EVLRXS, 1);

  // Don't check the VLAN type
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, DOVLTC, 1);

  // Check only C-TAG (0x8100) packets
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, ERSVLM, 0);

  // Don't consider an S-TAG (0x88A8) packet as a VLAN packet
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, ESVL, 0);

  // Enable VLAN tag stripping
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, EVLS, 0x3);

  return 0;
}

INT32
STATIC
xgbe_disable_rx_vlan_stripping (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_VLANTR, EVLS, 0);

  return 0;
}

VOID
STATIC
config_vlan_support (
  IN       xgmac_prv_data *pdata
  )
{
  if (FALSE) {
    xgbe_enable_rx_vlan_stripping (pdata);
  } else {
    xgbe_disable_rx_vlan_stripping (pdata);
  }
}

VOID
STATIC
enable_mac_interrupts (
  IN       xgmac_prv_data *pdata
  )
{
  // No MAC interrupts to be enabled
  XGMAC_IOWRITE32 (pdata, MAC_IER, 0);

  // Enable all counter interrupts
  XGMAC_IOWRITE32_BITS (pdata, MMC_RIER, ALL_INTERRUPTS, 0xff);
  XGMAC_IOWRITE32_BITS (pdata, MMC_TIER, ALL_INTERRUPTS, 0xff);
}

VOID
STATIC
enable_tx (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  UINT32 i;

  // Enable each Tx DMA channel
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_TCR, ST, 1);

  // Enable each Tx queue
  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, TXQEN,
           MTL_Q_ENABLED);
  }

  // Enable MAC Tx
  XGMAC_IOWRITE32_BITS (pdata, MAC_TCR, TE, 1);
}

VOID
STATIC
disable_tx (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  UINT32 i;

  // Disable MAC Tx
  XGMAC_IOWRITE32_BITS (pdata, MAC_TCR, TE, 0);

  // Disable each Tx queue
  for (i = 0; i < pdata->hw_feat.tx_q_cnt; i++) {
    XGMAC_MTL_IOWRITE32_BITS (pdata, i, MTL_Q_TQOMR, TXQEN, 0);
  }

  // Disable each Tx DMA channel
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_TCR, ST, 0);

}

VOID
STATIC
enable_rx (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;
  UINT32 reg_val;
  UINT32 i;

  // Enable each Rx DMA channel
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_RCR, SR, 1);

  // Enable each Rx queue
  reg_val = 0;
  for (i = 0; i < pdata->hw_feat.rx_q_cnt; i++) {
    reg_val |= (0x02 << (i << 1));
  }
  XGMAC_IOWRITE32 (pdata, MAC_RQC0R, reg_val);

  // Enable MAC Rx
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, DCRCC, 0x1);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, CST, 0x1);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, ACS, 0x1);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, RE, 0x1);
}

VOID
STATIC
disable_rx (
  IN       xgmac_prv_data *pdata
  )
{
  xgmac_channel *channel;

  // Disable MAC Rx
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, DCRCC, 0);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, CST, 0);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, ACS, 0);
  XGMAC_IOWRITE32_BITS (pdata, MAC_RCR, RE, 0);

  // Disable each Rx queue
  XGMAC_IOWRITE32 (pdata, MAC_RQC0R, 0);

  // Disable each Rx DMA channel
  channel = &pdata->channel;
  XGMAC_DMA_IOWRITE32_BITS (channel, DMA_CH_RCR, SR, 0);

  // Poll to be sure the channels have stopped?
  //while (count--) {
  //  dma_sr0 = XGMAC_IOREAD32_BITS(pdata, DMA_DSR0, RPS);
  //  if (dma_sr0 == 3 || dma_sr0 == 4)
  //    break;
  //  mdelay(1);
  //}
}

INT32
STATIC
set_gmii_speed (
  IN       xgmac_prv_data *pdata
  )
{
  UINT32 reg;
  UINT32 XgmacBase;
  XgmacBase = pdata->xgmac_regs;

  XGMAC_IOWRITE32_BITS (pdata, MAC_TCR, SS, 0x3);
  reg = XGMAC_IOREAD32 (pdata, MAC_TCR);
  DEBUG ((EFI_D_ERROR, "gmii_speed set to %x\n", reg));
  gBS->Stall (10);
  return 0;
}

INT32
STATIC
set_gmii_2500_speed (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_TCR, SS, 0x2);

  return 0;
}

INT32
STATIC
set_xgmii_speed (
  IN       xgmac_prv_data *pdata
  )
{
  XGMAC_IOWRITE32_BITS (pdata, MAC_TCR, SS, 0);
  return 0;
}

VOID
STATIC
rx_desc_reset (
     OUT   ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32 CurrIndex
  )
{
  // Reset the Rx descriptor
  //   Set buffer 1 (lo) address to dma address (lo)
  //   Set buffer 1 (hi) address to dma address (hi)
  //   Set buffer 2 (lo) address to zero
  //   Set buffer 2 (hi) address to zero and set control bits
  //     OWN and INTE
  //
  //EnetDmaFreeBuffer (&Private->pkt_rx_dma[CurrIndex]);
  //EFI_STATUS Status = EnetDmaAllocBuffer (ENET_MAX_PACKET_SIZE, &Private->pkt_rx_dma[CurrIndex]);
  //ASSERT_EFI_ERROR (Status);
  Private->desc_rx[CurrIndex].RDES0 = lower_32_bits (Private->pkt_rx_dma[CurrIndex].DeviceAddress);
  Private->desc_rx[CurrIndex].RDES1 = upper_32_bits (Private->pkt_rx_dma[CurrIndex].DeviceAddress);
  Private->desc_rx[CurrIndex].RDES2 = 0;

  Private->desc_rx[CurrIndex].RDES3 = 0;
  if (Private->desc_rx_interrupt[CurrIndex] == TRUE) {
    XGMAC_SET_BITS (Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, INTE, 1);
  }

  // Since the Rx DMA engine is likely running, make sure everything
  // is written to the descriptor(s) before setting the OWN bit
  // for the descriptor
  //

  XGMAC_SET_BITS (Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, OWN, 1);

}

EFI_STATUS
xgmac_dev_read (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID            *Buffer,
  IN       UINTN           *BufferSize
  )
{
  EFI_STATUS Status = EFI_NOT_READY;
  UINTN   BuffSize;
//  UINTN   BuSize;
  xgmac_channel *channel;
  UINT32  CurrIndex;
  UINT32  own;
  UINT32  err_summary;
//   UINT32 XgmacBase;
gBS->Stall (10);
  BuffSize = *BufferSize;
  channel = &Private->xgmac_pdata.channel;
  CurrIndex = GET_CURR_RX_DESC_INDEX (&Private->desc_rx_index);

  // Check for data availability
  own = XGMAC_GET_BITS (Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, OWN);
  if (own == 0) {
    // Check whether it is good packet or bad packet
    err_summary = XGMAC_GET_BITS (Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, ES);
    if (err_summary == 0) {
      // Get the packet length
      *BufferSize = XGMAC_GET_BITS (Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, PL);
      if ((*BufferSize != 0) && (*BufferSize <= BuffSize)) {
        CopyMem (Buffer, (VOID *) (UINTN) Private->pkt_rx_dma[CurrIndex].DeviceAddress, *BufferSize);
        Status = EFI_SUCCESS;
      } else if (*BufferSize > BuffSize) {
        return EFI_BUFFER_TOO_SMALL;
      } else {
        Status = EFI_NOT_READY;
      }
    } else {
      //
      //UINT32 err_type;
      //err_type = XGMAC_GET_BITS(Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, ETLT);
      //DEBUG ((
      //  EFI_D_ERROR, "\nBad packet, error type is 0x%X, size is 0x%X\n",
      //  err_type,
      //  XGMAC_GET_BITS(Private->desc_rx[CurrIndex].RDES3, RX_NORMAL_DESC3, PL)
      //  ));
      //
      Status = EFI_NOT_READY;
    }
gBS->Stall (100);
    rx_desc_reset (Private, CurrIndex);
    // Update the Rx Tail Pointer Register with address of
    // descriptors from which data is read
    XGMAC_DMA_IOWRITE32 (channel, DMA_CH_RDTR_LO,
      lower_32_bits (&Private->desc_rx[CurrIndex]));
    Private->desc_rx_index++;
  }

  return Status;
}

VOID
STATIC
tx_desc_reset (
     OUT   ENET_DXE_PRIVATE_DATA *Private,
  IN       UINT32 CurrIndex
  )
{
  // Reset the Tx descriptor
  //   Set buffer 1 (lo) address to zero
  //   Set buffer 1 (hi) address to zero
  //   Reset all other control bits (IC, TTSE, B2L & B1L)
  //   Reset all other control bits (OWN, CTXT, FD, LD, CPC, CIC, etc)
  //
  Private->desc_tx[CurrIndex].TDES0 = 0;
  Private->desc_tx[CurrIndex].TDES1 = 0;
  Private->desc_tx[CurrIndex].TDES2 = 0;
  Private->desc_tx[CurrIndex].TDES3 = 0;
}

EFI_STATUS
xgmac_pre_xmit (
  IN       ENET_DXE_PRIVATE_DATA *Private,
  IN       VOID       *Buffer,
  IN       UINTN      BufferSize
  )
{
  xgmac_channel *channel;
  UINT32 csum;
  UINT32 CurrIndex;
  UINT32 StartIndex;
  UINT32 DmaOwnBit;
  UINTN count;
  EFI_STATUS Status = EFI_SUCCESS;

  channel = &Private->xgmac_pdata.channel;
  csum = 1;
  CurrIndex = GET_CURR_TX_DESC_INDEX (&Private->desc_tx_index);
  StartIndex = CurrIndex;
  DmaOwnBit = 1;

  // Update buffer address (for TSO this is the header)
  CopyMem ((VOID *) (UINTN) Private->pkt_tx_dma[CurrIndex].DeviceAddress, Buffer, BufferSize);
  Private->desc_tx[CurrIndex].TDES0 =
    lower_32_bits (Private->pkt_tx_dma[CurrIndex].DeviceAddress);
  Private->desc_tx[CurrIndex].TDES1 =
    upper_32_bits (Private->pkt_tx_dma[CurrIndex].DeviceAddress);

  // Update the buffer length
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES2, TX_NORMAL_DESC2, HL_B1L,
           BufferSize);

  // Mark it as First Descriptor
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, FD, 1);

  // Mark it as a NORMAL descriptor
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, CTXT, 0);

  // Set OWN bit if not the first descriptor
  if (GET_CURR_TX_DESC_INDEX (&Private->desc_tx_index) != StartIndex) {
    XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, OWN, 0x1);
  }

  // Enable CRC and Pad Insertion
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, CPC, 0);

  // Enable HW CSUM
  if (csum) {
    XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, CIC, 0x3);
  }

  // Set the total length to be transmitted
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, FL,
                 BufferSize);

  // Set LAST bit for the last descriptor
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, LD, 0x1);

  // Set Interrupt on Completion bit for the last descriptor
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC2, IC, 0x1);

  // In case the Tx DMA engine is running, make sure everything
  // is written to the descriptor(s) before setting the OWN bit
  // for the first descriptor
  //

  // Set OWN bit for the first descriptor
  XGMAC_SET_BITS (Private->desc_tx[CurrIndex].TDES3, TX_NORMAL_DESC3, OWN, 0x1);


  // Issue a poll command to Tx DMA by writing address
  // of next immediate free descriptor
  Private->desc_tx_index++;
  CurrIndex = GET_CURR_TX_DESC_INDEX (&Private->desc_tx_index);
  XGMAC_DMA_IOWRITE32 (channel, DMA_CH_TDTR_LO,
          lower_32_bits (&Private->desc_tx[CurrIndex]));

  // make sure it had been sent, wait maximum 5mS
  for (count = 0; count < XGMAC_TX_DMA_MAX_WAIT_MCS / 2; count++) {
    DmaOwnBit = SNP_GET_BIT_FIELD (Private->desc_tx[StartIndex].TDES3, TX_NORMAL_DESC3, OWN);
    if (DmaOwnBit == 0) {
      break;
    }
    gBS->Stall (2);
  }

  if (DmaOwnBit == 1) {
    Status = EFI_DEVICE_ERROR;
  }

  tx_desc_reset (Private, StartIndex);
  return Status;
}

VOID
XgmacInit (
  IN OUT   ENET_DXE_PRIVATE_DATA *Private,
  IN       UINTN XgmacBase
  )
{
  UINT32 CheckValue;
  // S/W Reset

  MmioWrite32 (XgmacBase + 0x00003000, 0x00000001);
  while (1) {
  // wait for done
    CheckValue = getBitfieldReg32 ((UINT32) XgmacBase + 0x00003000, 0, 1);
    if (CheckValue == 0x0) {
      break;
    }
    gBS->Stall (1000);
  }
  //Later. Forcing for 1G for now.
  set_gmii_speed (&Private->xgmac_pdata);
#if 0
  //PS-67++start
  if (mCurrLanSpeed == 1) {
    set_gmii_speed (&Private->xgmac_pdata);
  } else if (mCurrLanSpeed == 2) {
    set_gmii_2500_speed (&Private->xgmac_pdata);
  } else if (mCurrLanSpeed == 10) {
    set_xgmii_speed (&Private->xgmac_pdata);
  } else {
    ASSERT (0 && "Unsupported LAN speed value");
  }
  //PS-67++end
#endif
  // Flush Tx queues
  if (flush_tx_queues (&Private->xgmac_pdata)) {
  DEBUG ((EFI_D_ERROR, "Exiting out of flush_tx_queues\n"));
  gBS->Stall (10);
    return;
  }

  //
  // Initialize DMA related features
  //
  DEBUG ((EFI_D_ERROR, "Initialize DMA related features\n"));
  gBS->Stall (10);
  config_dma_bus (&Private->xgmac_pdata);
  config_dma_cache (&Private->xgmac_pdata);
  config_osp_mode (&Private->xgmac_pdata);
  config_pblx8 (&Private->xgmac_pdata);
  config_tx_pbl_val (&Private->xgmac_pdata);
  config_rx_pbl_val (&Private->xgmac_pdata);
  config_rx_coalesce (&Private->xgmac_pdata);
  config_rx_buffer_size (&Private->xgmac_pdata);
  config_tso_mode (&Private->xgmac_pdata);
  tx_desc_init (Private);     //desc_if->wrapper_tx_desc_init(pdata);
  rx_desc_init (Private);     //desc_if->wrapper_rx_desc_init(pdata);
  enable_dma_interrupts (&Private->xgmac_pdata);

  //
  // Initialize MTL related features
  //
  config_mtl_mode (&Private->xgmac_pdata);
  config_rx_queue_mapping (&Private->xgmac_pdata);
  //  Program the priorities mapped to the Selected Traffic Classes
  //  in MTL_TC_Prty_Map0-3 registers
  //  Program the Transmit Selection Algorithm (TSA) in MTL_TC[n]_ETS_Control
  //  register for all the Selected Traffic Classes (n=0, 1, , Max selected
  //  number of TCs - 1)
  config_tsf_mode (&Private->xgmac_pdata, Private->xgmac_pdata.tx_sf_mode);
  config_rsf_mode (&Private->xgmac_pdata, Private->xgmac_pdata.rx_sf_mode);
  config_tx_threshold (&Private->xgmac_pdata, Private->xgmac_pdata.tx_threshold);
  config_rx_threshold (&Private->xgmac_pdata, Private->xgmac_pdata.rx_threshold);
  config_tx_fifo_size (&Private->xgmac_pdata);
  config_rx_fifo_size (&Private->xgmac_pdata);
  config_flow_control_threshold (&Private->xgmac_pdata);
  //  Queue to Traffic Class Mapping (Q2TCMAP)
  //  Error Packet and undersized good Packet forwarding enable (FEP and FUP)
  enable_mtl_interrupts (&Private->xgmac_pdata);

  // Transmit Class Weight
  XGMAC_IOWRITE32_BITS (&Private->xgmac_pdata, MTL_Q_TCQWR, QW, 0x10);

  //
  // Initialize MAC related features
  //
  xgmac_set_rx_mode (Private);
  config_mac_address (Private);
  config_jumbo_enable (&Private->xgmac_pdata);
  config_flow_control (&Private->xgmac_pdata);
  config_checksum_offload (&Private->xgmac_pdata);
  config_vlan_support (&Private->xgmac_pdata);
  config_mmc (&Private->xgmac_pdata);
  enable_mac_interrupts (&Private->xgmac_pdata);

  // Enable Tx/Rx
  enable_tx (&Private->xgmac_pdata);
  enable_rx (&Private->xgmac_pdata);
}

VOID
XgmacSetSpeed (
  IN OUT   ENET_DXE_PRIVATE_DATA *Private,
  IN       UINTN XgmacBase
  )
{

  UINT32 CheckValue;
  // S/W Reset
  MmioWrite32 (XgmacBase + 0x00003000, 0x00000001);
  while (1) {
  // wait for done
    CheckValue = getBitfieldReg32 ((UINT32)XgmacBase + 0x00003000, 0, 1);
    if (CheckValue == 0x0) {
      break;
    }
    gBS->Stall (1000);
  }
  //Later Forcing for 1G for now
  set_gmii_speed (&Private->xgmac_pdata);
#if 0
//PS-67++enable the following codes
  if (mCurrLanSpeed == 1) {
    set_gmii_speed (&Private->xgmac_pdata);
  } else if (mCurrLanSpeed == 2) {
    set_gmii_2500_speed (&Private->xgmac_pdata);
  } else if (mCurrLanSpeed == 10) {
    set_xgmii_speed (&Private->xgmac_pdata);
  } else {
    ASSERT (0 && "Unsupported LAN speed value");
  }
#endif
}


VOID
UpdateLinkStatus (
  IN       ENET_DXE_PRIVATE_DATA *Private
  )
{
  XGBE_PHY_DATA *phy_data = Private->phy_data;
  UINTN Reg32;

  Reg32 = XMDIO_READ (Private->xpcs_regs, MDIO_MMD_PCS, MDIO_PCS_STS1);
  // Read again as bits in register can be latched
  Reg32 = XMDIO_READ (Private->xpcs_regs, MDIO_MMD_PCS, MDIO_PCS_STS1);

  Private->Mode.MediaPresent = ((Reg32 & PCS_STS1_RLU) == PCS_STS1_RLU)?TRUE:FALSE;
  if ( Private->Mode.MediaPresent == FALSE ) {
    DEBUG ((EFI_D_ERROR, "MediaPresent FAIL   \n"));
	gBS->Stall (10);
    // No link, attempt a receiver reset cycle
  if (phy_data->rrc_count++) {
    phy_data->rrc_count = 0;
    xgbe_phy_rrc (Private);
  }
  }
}

EFI_STATUS
XgbeInit (
  ENET_DXE_PRIVATE_DATA *Private
  )
{
  xgbe_phy_power_off (Private);
  xgbe_an_disable (Private);
  xgbe_an_clear_interrupts_all (Private);
//  xgbe_phy_sfi_mode (Private);

 return EFI_SUCCESS;
  }

VOID
STATIC
xgbe_an73_disable_kr_training (
  UINTN XpcsBase
  )
{
  UINTN reg;
  DEBUG ((EFI_D_ERROR, "xgbe_an73_disable_kr_training\n"));
  gBS->Stall (10);
  reg = XMDIO_READ (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_PMD_CTRL);

  reg &= ~XGBE_KR_TRAINING_ENABLE;
  XMDIO_WRITE (XpcsBase, MDIO_MMD_PMAPMD, MDIO_PMA_10GBR_PMD_CTRL, reg);
}

VOID
STATIC
xgbe_an73_restart (
  UINT32 XpcsBase
  )
{
  DEBUG ((EFI_D_ERROR, "xgbe_an73_restart\n"));
  gBS->Stall (10);
  xgbe_an73_enable_interrupts (XpcsBase);
  xgbe_an73_set (XpcsBase, TRUE, TRUE);
  DEBUG ((EFI_D_ERROR, "CL73 AN enabled / restarted\n"));
  gBS->Stall (10);
}

VOID
xgbe_x_mode (
  ENET_DXE_PRIVATE_DATA *pdata
  )
{
  UINT32 XpcsBase;
  DEBUG ((EFI_D_ERROR, "xgbe_x_mode\n"));
  gBS->Stall (10);
  XpcsBase = pdata->xpcs_regs;
  // Disable KR training
  xgbe_an73_disable_kr_training (XpcsBase);
  // Later Forcing for 1G for now. Set MAC to 1G speed
  set_gmii_speed (&pdata->xgmac_pdata); 
#if 0
  //PS-67-SFP++add the following data
  DEBUG ((EFI_D_ERROR, "PS-67: mCurrLanSpeed = %d\n", mCurrLanSpeed));
  if (mCurrLanSpeed == 1) {
    set_gmii_speed (&pdata->xgmac_pdata);
  } else if (mCurrLanSpeed == 2) {
    set_gmii_2500_speed (&pdata->xgmac_pdata);
  } else if (mCurrLanSpeed == 10) {
    set_xgmii_speed (&pdata->xgmac_pdata);
  } else {
    ASSERT (0 && "Unsupported LAN speed value");
  }
  //PS-67-SFP++end
#endif
  // Call PHY implementation support to complete rate change
  xgbe_phy_x_mode (pdata);
  xgbe_an73_init (pdata, XpcsBase);
  xgbe_an73_enable_interrupts (XpcsBase);
  xgbe_an_disable (pdata);
  xgbe_an_clear_interrupts_all (pdata);
  xgbe_an73_init (pdata, XpcsBase);
  xgbe_an73_restart (XpcsBase);
// pdata->hw_if.set_speed(pdata, SPEED_1000);
//pdata->phy_if.phy_impl.set_mode(pdata, XGBE_MODE_X);
}
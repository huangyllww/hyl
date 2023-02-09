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

#include <UsbSiRoutine.h>

/**
 * set usb 2.0 swing
 * @param Die die number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
Usb2TxSwing (
  UINT32  DieBusNum,
	UINT8   Port,
  UINT8   Value
)
{
  UINT32          LaneCtlSmnAddr=0;
  LANEPARACTL0    LaneCtl;

	switch (Port) {
		case 0:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK0_SMN_ADDRESS;
			break;
		case 1:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK1_SMN_ADDRESS;
			break;
		case 2:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK2_SMN_ADDRESS;
			break;
		case 3:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK3_SMN_ADDRESS;
			break;
	}
	
	FchSmnRead (DieBusNum, LaneCtlSmnAddr, &LaneCtl.Data32, NULL);
	LaneCtl.Reg.TXVREFTUNE = Value;
  FchSmnWrite (DieBusNum, LaneCtlSmnAddr, &LaneCtl.Data32, NULL);
  
	return;
}

/**
 * set usb 2.0 preemphasis
 * @param Die die number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
Usb2TxPreemphasis (
  UINT32  DieBusNum,
	UINT8   Port,
  UINT8   Value
)
{
  UINT32          LaneCtlSmnAddr=0;
  LANEPARACTL0    LaneCtl;

	switch (Port) {
		case 0:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK0_SMN_ADDRESS;
			break;
		case 1:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK1_SMN_ADDRESS;
			break;
		case 2:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK2_SMN_ADDRESS;
			break;
		case 3:
		  LaneCtlSmnAddr = LANEPARACTL0_LINK3_SMN_ADDRESS;
			break;
	}

  FchSmnRead (DieBusNum, LaneCtlSmnAddr, &LaneCtl.Data32, NULL);
  LaneCtl.Reg.TXPREEMPAMPTUNE = Value;
  FchSmnWrite (DieBusNum, LaneCtlSmnAddr, &LaneCtl.Data32, NULL);
  
	return;
}

/**
 * set usb 3.0 vboost
 * @param Die die number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
Usb3TxVboost_V1 (
  UINT32  DieBusNum,
	UINT8   Port,
  UINT8   Value
)
{
	PHYPARACTL2        PhyCtl2;
  UINT32             PhyCtl2SmnAddr=0;

	switch(Port){
		case 0:
		  PhyCtl2SmnAddr = PHYPARACTL2_LINK0_SMN_ADDRESS;
			break;
		case 1:
		  PhyCtl2SmnAddr = PHYPARACTL2_LINK1_SMN_ADDRESS;
			break;
		case 2:
		  PhyCtl2SmnAddr = PHYPARACTL2_LINK2_SMN_ADDRESS;
			break;
		case 3:
		  PhyCtl2SmnAddr = PHYPARACTL2_LINK3_SMN_ADDRESS;
			break;
	}

  FchSmnRead (DieBusNum, PhyCtl2SmnAddr, &PhyCtl2.Data32, NULL);
  PhyCtl2.Reg.tx_vboost_lvl = Value;
  FchSmnWrite (DieBusNum, PhyCtl2SmnAddr, &PhyCtl2.Data32, NULL);
  
	return;

}

/**
 * set usb 3.0 deemph by phy
 * @param Die die number
 * @param Port port number
 * @param Value set value
 * @return VOID
 */
VOID
Usb3TxDeemph (
  UINT32  DieBusNum,
	UINT8   Port,
  UINT8   Value
)
{
	TX_OVRD_DRV_LO     TxOverDrvLo;
  UINT32             TxOverDrvLoAddr=0;

	switch (Port) {
		case 0:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE0;
			break;
		case 1:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE1;
			break;
		case 2:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE2;
			break;
		case 3:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE3;
			break;
	}

  FchSmnRead (DieBusNum, TxOverDrvLoAddr, &TxOverDrvLo.Data32, NULL);
  TxOverDrvLo.Reg.PREEMPH = Value;
  TxOverDrvLo.Reg.AMPLITUDE = 0x7F;
  TxOverDrvLo.Reg.EN = 1;
  FchSmnWrite (DieBusNum, TxOverDrvLoAddr, &TxOverDrvLo.Data32, NULL);
  
	return;

}

/**
 * Disable usb 3.0 deemph by phy
 * @param Die die number
 * @param Port port number
 * @return VOID
 */
VOID
DisableUsb3TxDeemph (
  UINT32  DieBusNum,
	UINT8   Port
)
{
	TX_OVRD_DRV_LO     TxOverDrvLo;
  UINT32             TxOverDrvLoAddr=0;

	switch(Port){
		case 0:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE0;
			break;
		case 1:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE1;
			break;
		case 2:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE2;
			break;
		case 3:
		  TxOverDrvLoAddr = TX_OVRD_DRV_LO_LANE3;
			break;
	}
 
  FchSmnRead (DieBusNum, TxOverDrvLoAddr, &TxOverDrvLo.Data32, NULL);
  TxOverDrvLo.Reg.PREEMPH = 0;
  TxOverDrvLo.Reg.AMPLITUDE = 0;
  TxOverDrvLo.Reg.EN = 0;
  FchSmnWrite (DieBusNum, TxOverDrvLoAddr, &TxOverDrvLo.Data32, NULL);
  
  return;
}

/**
 * Set USB3 Gen1 TXEQ
 * @param Die die number
 * @param Port port number
 * @param TxMainCursor TXEQ Main-cursor
 * @param TxPreCursor TXEQ Pre-cursor
 * @param TxPostCursor TXEQ Post-cursor
 * @return VOID
 */
VOID 
Usb3Gen1Txeq (
  UINT32 DieBusNum,
	UINT8  Port,
	UINT8  TxMainCursor,
	UINT8  TxPreCursor,
	UINT8  TxPostCursor
)
{
  UINT32            UsbTxEq0Addr;
	UINT32            UsbTxEq1Addr;
	PHY_EXT_TX_EQ0    TxEq0;
	PHY_EXT_TX_EQ1    TxEq1;

	UsbTxEq0Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET;
	UsbTxEq1Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET;
	
  FchSmnRead (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnRead (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
  
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G1 = 1;
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G1 = TxMainCursor;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G1  = TxPreCursor;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G1 = TxPostCursor;
	
	FchSmnWrite (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
	FchSmnWrite (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
	
	return;
}

/**
 * Disable USB3 Gen1 TXEQ
 * @param Die die number
 * @param Port port number
 * @return VOID
 */
VOID 
DisableUsb3Gen1Txeq (
  UINT32 DieBusNum,
	UINT8  Port
)
{
  UINT32            UsbTxEq0Addr;
	UINT32            UsbTxEq1Addr;
	PHY_EXT_TX_EQ0    TxEq0;
	PHY_EXT_TX_EQ1    TxEq1;

	UsbTxEq0Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET;
	UsbTxEq1Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET;
	
  FchSmnRead (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnRead (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
	
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G1 = 0;
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G1 = 0;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G1  = 0;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G1 = 0;
	
  FchSmnWrite (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnWrite (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
	
	return;
}

/**
 * Set USB3 Gen2 TXEQ
 * @param Die die number
 * @param Port port number
 * @param TxMainCursor TXEQ Main-cursor
 * @param TxPreCursor TXEQ Pre-cursor
 * @param TxPostCursor TXEQ Post-cursor
 * @return VOID
 */
VOID 
Usb3Gen2Txeq (
  UINT32 DieBusNum,
	UINT8  Port,
	UINT8  TxMainCursor,
	UINT8  TxPreCursor,
	UINT8  TxPostCursor
)
{
  UINT32            UsbTxEq0Addr;
	UINT32            UsbTxEq1Addr;
	PHY_EXT_TX_EQ0    TxEq0;
	PHY_EXT_TX_EQ1    TxEq1;
	
	UsbTxEq0Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET;
	UsbTxEq1Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET;

  FchSmnRead (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnRead (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
  
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G2 = 1;
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G2 = TxMainCursor;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G2  = TxPreCursor;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G2 = TxPostCursor;

  FchSmnWrite (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnWrite (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
	
	return;
}

/**
 * Disable USB3 Gen2 TXEQ
 * @param Die die number
 * @param Port Port port number
 */
VOID 
DisableUsb3Gen2Txeq (
  UINT32 DieBusNum,
	UINT8  Port
)
{
  UINT32            UsbTxEq0Addr;
	UINT32            UsbTxEq1Addr;
	PHY_EXT_TX_EQ0    TxEq0;
	PHY_EXT_TX_EQ1    TxEq1;

	UsbTxEq0Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ0_OFFSET;
	UsbTxEq1Addr = USB_MAP_PHY_BASE + Port * 0x400 + PHY_EXT_TX_EQ1_OFFSET;

  FchSmnRead (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnRead (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);

	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_OVRD_G2 = 0;
	TxEq0.Reg.PROTOCOL_EXT_TX_EQ_MAIN_G2 = 0;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_PRE_G2  = 0;
	TxEq1.Reg.PROTOCOL_EXT_TX_EQ_POST_G2 = 0;

  FchSmnWrite (DieBusNum, UsbTxEq0Addr, &TxEq0.Data32, NULL);
  FchSmnWrite (DieBusNum, UsbTxEq1Addr, &TxEq1.Data32, NULL);
	
	return;
}

/**
 * Set USB3 TX vboost
 * @param Die die number
 * @param Port port number
 * @param BoostLvl boost level
 * @return VOID
 */
VOID 
Usb3TxVboost_V2 (
  UINT32 DieBusNum,
	UINT8  Port,
	UINT8  BoostLvl
)
{
  UINT32                PhySelectAddr;
	UINT32                SupDigLvlOverInAddr;
  SUP_DIG_LVL_OVER_IN   SupDigLvlOverIn;

	PhySelectAddr       = PHY_CR_SELECT;
	SupDigLvlOverInAddr = USB_PHY_BASE + SUP_DIG_LVL_OVER_IN_OFFSET * 4;
    
	//select port
	FchSmnWrite (DieBusNum, PhySelectAddr, (UINT32*)&Port, NULL);
	
  //set boost level
	FchSmnRead16 (DieBusNum, SupDigLvlOverInAddr, &SupDigLvlOverIn.Data16, NULL);
	SupDigLvlOverIn.Reg.TX_VBOOST_LVL_EN = 1;
  SupDigLvlOverIn.Reg.TX_VBOOST_LVL    = BoostLvl;
  FchSmnWrite16 (DieBusNum, SupDigLvlOverInAddr, &SupDigLvlOverIn.Data16, NULL);

	return;
}

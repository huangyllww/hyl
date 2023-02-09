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
 * ***************************************************************************
 */
	Scope (\_SB.PCI0) 
	{


		Method(_OSC,4)
		{ // Check for proper UUID
			Name(SUPP,0) // PCI _OSC Support Field value
			Name(CTRL,0) // PCI _OSC Control Field value

			If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
			{
				// Create DWord-adressable fields from the Capabilities Buffer
				CreateDWordField(Arg3,0,CDW1)
				CreateDWordField(Arg3,4,CDW2)
				CreateDWordField(Arg3,8,CDW3)

				// Save Capabilities DWord2 & 3
				Store(CDW2,SUPP)
				Store(CDW3,CTRL)
				
		               // Do not allow native hot plug control if OS does not support:
		               // * ASPM
		               // * Clock PM
		               // * MSI/MSI-X
		               // Do not allow native hot plug control if AMHP is 0x80000000
				If(LNotEqual(And(SUPP, 0x16), 0x16))
				{
					And(CTRL,0x1E) // Mask bit 0 (and undefined bits)
				}
				// Always allow native PME, AER (no dependencies)
				// Never allow SHPC (no SHPC controller in this system)

//				 And(CTRL,0x1D,CTRL)
				//Tokens ACPI.sdl
				if (LNot(PEHP))//BIOS Supports PCI Express HP? 
				{
					And(CTRL,0x1E,CTRL)
				}
				if (LNot(SHPC)) //BIOS Supports SHPC Hot Plug ? 
				{
					And(CTRL,0x1D,CTRL)
				}
				if (LNot(PEPM)) //BIOS Supports PCI Express ASPM?  
				{
					And(CTRL,0x1B,CTRL)
				}
				if (LNot(PEER))	//BIOS Supports PCI Express Adv Error Reporting? 	 
				{
					And(CTRL,0x15,CTRL)
				}
				if (LNot(PECS)) //BIOS Provides PCI Express MCFG Table?  
				{
					And(CTRL,0xF,CTRL)
				}
		
        if(\OSCF) 
        {
				If(Not(And(CDW1,1))) // Query flag clear?
				{ // Disable GPEs for features granted native control.
					If(And(CTRL,0x01)) // Hot plug control granted?
					{
						//Store(0,HPCE) // clear the hot plug SCI enable bit
						//Store(1,HPCS) // clear the hot plug SCI status bit
					}

					// 			Native 			Legacy 
					//IO 0x74[24]              1                      0
					//IO 0x74[25]              0                      1
					//IO 0xBA[14] 	           0                      1
					If(And(CTRL,0x04)) // PME control granted?
					{
						Store(0x01, \EPNM)	//Enable PCIE Native mode
				                //Store(0x00, \DPPF)   	//Unmask PCIE wake events
						//Store(0x00, \PWDE)	
//						Store(0x77, \P80_)
					}
					else
					{
                                                Store(0x00, \EPNM)	//Disable PCIE Native mode
				                //Store(0x01, \DPPF)	//Mask PCIE wake event since we will legacy GEvent mechanism
				                //Store(0x01, \PWDE) 
//						Store(0x88, \P80_)	
					}
					If(And(CTRL,0x10)) // OS restoring PCIe cap structure?
					{ 
						// Set status to not restore PCIe cap structure
						// upon resume from S3
						//Store(1,S3CR)
					}
				}
        } //OSCF
				If(LNotEqual(Arg1,One))
				{ // Unknown revision
					Or(CDW1,0x08,CDW1)
				}
				If(LNotEqual(CDW3,CTRL))
				{ // Capabilities bits were masked
					Or(CDW1,0x10,CDW1)
				}
				// Update DWORD3 in the buffer
				Store(CTRL,CDW3)

				Return(Arg3)
			} 
			Else 
			{
				Or(CDW1,4,CDW1) // Unrecognized UUID
				Return(Arg3)
			}
		} // End _OSC
	}


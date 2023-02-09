#include <AmdPcieComplex.h>       
#include "CustomerCpmTable.h"

DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0Normal[] = { 
               {// P0 - SATA port
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 7, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_SATA (
                   DxioPortEnabled                       // Port Present
                 )
               },
               { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};

DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCard[] = { 
               {// P0 - SATA port
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_SATA (
                   DxioPortEnabled                       // Port Present
                 )
               },
               { // P0 - EtherNet Port 0
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                    DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8
                                                                                                 /*< Platform Config
                                                                                                 *  0 = Reserved
                                                                                                 *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                                 *  2 = 2.5G BackPlane
                                                                                                 *  3= Soldered down 1000Base-T
                                                                                                 *  4 = Soldered down 1000Base-X
                                                                                                 *  5 = Soldered down NBase-T
                                                                                                 *  6 = Soldered down 10GBase-T
                                                                                                 *  7 = Soldered down 10GBase-r
                                                                                                 *  8 = SFP+ Connector
                                                                                                 */
                     0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                     0xB, /// mSuppSpeed   0xB
                                                                                                  /*< Supported Speeds by Platform             !!!!!!!
                                                                                                  *  1 = 100M Supported
                                                                                                  *  2 = 1G Supported
                                                                                                  *  4 = 2.5G Supported
                                                                                                  *  8 = 10G Supported
                          */
                     1, /// mConnType   1
                                                                                                  /*< Supported Speeds by Platform
                                                                                                  *  0 = Port not Used
                                                                                                  *  1 = SFP+ Connection I2C interface
                                                                                                  *  2 = MDIO PHY
                                                                                                  *  4 = Backplane Connection     !!!!!
                                                                                                  */
                     0, /// mMdioReset   0
                                                                                                  /*< MDIO Reset Type   
                                                                                                  *  0 = None
                                                                                                  *  1 = I2C GPIO
                                                                                                  *  2 = Integrated GPIO
                                                                                                  *  3 = Reserved
                                                                                                  *  If an external MDIO PHY is used, this field 
                                                                                                  *  indicates the method used by the software 
                                                                                                  *  driver to control for the reset to that PHY
                                                                                                  *  11b = Reserved
                                                                                                  *  10b = Integrated GPIO
                                                                                                  *  01b = I2C GPIO
                                                                                                  *  00b = None
                                                                                                  */
                     0, /// mMdioGpioResetNum
                                                                                                  /*
                                                                                                   * If MDIO Reset Type is set to 10b, this field 
                                                                                                   * indicates the Ethernet integrated GPIO number 
                                                                                                   * used to control the reset.
                                                                                                   */
                     3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                   /*
                                                                                                    * If an external MDIO PHY is used in 
                                                                                                    combination with an I2C GPIO reset, this field 
                                                                                                    indicates the lower I2C address of the 
                                                                                                    PCA9535 GPIO expander used to drive the reset.
                                                                                                    If this port is used as part of an SFP+ 
                                                                                                    connection, this field indicates the lower I2C 
                                                                                                    address of the PCA9535 GPIO expander used 
                                                                                                    to drive the SFP+ sideband signals
                                                                                                    */
                     0x2, /// mTxFault,   0xE
                                                                                                  /*
                                                                                                    * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control either the SFP+ TX_FAULT 
                                                                                                    signal or the MDIO reset associated with this 
                                                                                                    port. If neither of those features are in use, set 
                                                                                                    this to 0.
                                                                                                    */
                     0, /// mRs,
                                                                                                  /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control the SFP+ RS signal. If this port 
                                                                                                    is not used with an SFP+ connector, set this 
                                                                                                    field to 0.
                                                                                                    */
                     0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                                  //  set this field to 0.
                                                                                                 
                     0x1, /// mRxLoss,    0xD      
                                                                                                  /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control the SFP+ Rx_LOS signal. If 
                                                                                                    this port is not used with an SFP+ connector, 
                                                                                                    set this field to 0.
                                                                                                    */
                     2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                     0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                              /*
                                                                                               * Set this field to indicate the lower address of 
                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                SFP+ connector. If no multiplexing is used, set 
                                                                                                this field to 111b. If this port is not used with 
                                                                                                an SFP+ connector, set this field to 0.
                                                                                                000b-011b = PCA9545 lower address
                                                                                                100b-110b = Reserved
                                                                                                111b = SFP+ directly connected to I2C
                                                                                               * */
                     0, /// mSfpBusSeg,
                                                                                               /*
                                                                                               * Set this field to indicate the downstream 
                                                                                                channel of the PCA9545 I2C multiplexer used 
                                                                                                to drive the SFP+ connector. If no multiplexing
                                                                                                is used, set this field to 111b. If this port is not 
                                                                                                used with an SFP+ connector, set this field to 
                                                                                                0.
                                                                                                000b-011b = PCA9545 lower address
                                                                                                100b-110b = Reserved
                                                                                                111b = SFP+ directly connected to processor 
                                                                                                I2C*/
                     0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                              /*
                                                                                               * Set this field to indicate the upper address of 
                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                SFP+ connector. PCA9545 comes in 3 variants 
                                                                                                each with a different upper address. 
                                                                                                11100b = PCA9545A
                                                                                                11010b = PCA9545B
                                                                                                10110b = PCA9545C
                                                                                                11111b = SFP+ directly connected to processor
                                                                                                I2C
                                                                                                If no multiplexing is used, set this field to 
                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                connector, set this field to 0.
                                                                                                */
                     0x40, /// mRedriverAddress, 0x40
                                                                                                  /*
                                                                                                   * This field encodes the I2C or MDIO address 
                                                                                                    used to control a platform level redriver 
                                                                                                    connected to this port. If no redriver is used, 
                                                                                                    set this field to 0. If the redriver is connected 
                                                                                                    using MDIO, the upper 2 address bits must be 
                                                                                                    set to 0.
                                                                                                  */
                     1, /// mRedriverInterface,  1
                                                                                                  /*
                                                                                                   * This field encodes the I2C or MDIO address 
                                                                                                    used to control a platform level redriver 
                                                                                                    connected to this port. If no redriver is used, 
                                                                                                    set this field to 0. If the redriver is connected 
                                                                                                    using MDIO, the upper 2 address bits must be 
                                                                                                    set to 0.
                                                                                                    */
                     0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                     0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                     0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                     4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                     0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                     0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                     0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                     0, /// mTxEqPre,  TBD
                     0, /// mTxEqMain, TBD    0x28
                     0  /// mTxEqPost  TBD
                     )
                 },
                 { // P0 - EtherNet Port 1
                     0,
                     DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                     DXIO_PORT_DATA_INITIALIZER_ENET (
                       DxioPortEnabled,                      // Port Present
                       1, /// mPortNum
                       8, /// mPlatConf   8
                       0, /// mMdioId
                       0xB, /// mSuppSpeed   0xB
                       1, /// mConnType   1
                       0, /// mMdioReset   0
                       0, /// mMdioGpioResetNum
                       3, /// mSfpGpioAdd   6
                       6, /// mTxFault,   
                       0, /// mRs,
                       4, /// mModAbs,
                       5, /// mRxLoss,
                       2, /// mSfpGpioMask,  2
                       0, /// mSfpMux,
                       1, /// mSfpBusSeg,
                       0x1C, /// mSfpMuxUpAdd,  0x1C
                       0x40, /// mRedriverAddress, 0x40
                       1, /// mRedriverInterface,  1
                       1, /// mRedriverLane,
                       0, /// mRedriverModel,
                       0, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                       4, /// mPadMux0,  4
                       0, /// mPadMux1,
                       0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                       0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                       0, /// mTxEqPre,  TBD
                       0, /// mTxEqMain, TBD    0x28
                       0  /// mTxEqPost  TBD
                     )
                 },
                 { // P0 - EtherNet Port 2
                       0,
                       DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                       DXIO_PORT_DATA_INITIALIZER_ENET (
                         DxioPortEnabled,                      // Port Present
                         2, /// mPortNum
                         8, /// mPlatConf   8
                         0, /// mMdioId
                         0xB, /// mSuppSpeed   0xB
                         1, /// mConnType   1
                         0, /// mMdioReset   0
                         0, /// mMdioGpioResetNum
                         3, /// mSfpGpioAdd   6
                         0xA, /// mTxFault,   0xE
                         0, /// mRs,
                         8, /// mModAbs,   0xC
                         9, /// mRxLoss,    0xD
                         2, /// mSfpGpioMask,  2
                         0, /// mSfpMux,
                         2, /// mSfpBusSeg,
                         0x1C, /// mSfpMuxUpAdd,  0x1C
                         0x40, /// mRedriverAddress, 0x40
                         1, /// mRedriverInterface,  1
                         2, /// mRedriverLane,
                         0, /// mRedriverModel,
                         0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                         4, /// mPadMux0,  4
                         0, /// mPadMux1,
                         0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                         0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                         0, /// mTxEqPre,  TBD
                         0, /// mTxEqMain, TBD    0x28
                         0  /// mTxEqPost  TBD
                       )
                 },
                 { // P0 - EtherNet Port 3
                       0,
                       DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                       DXIO_PORT_DATA_INITIALIZER_ENET (
                         DxioPortEnabled,                      // Port Present
                         3, /// mPortNum
                         8, /// mPlatConf   8
                         0, /// mMdioId
                         0xB, /// mSuppSpeed   0xB
                         1, /// mConnType   1
                         0, /// mMdioReset   0
                         0, /// mMdioGpioResetNum
                         3, /// mSfpGpioAdd   6
                         0xE, /// mTxFault,  
                         0, /// mRs,
                         0xC, /// mModAbs,   
                         0xD, /// mRxLoss,    
                         0, /// mSfpGpioMask,  2
                         0, /// mSfpMux,
                         3, /// mSfpBusSeg,
                         0x1C, /// mSfpMuxUpAdd,  0x1C
                         0x40, /// mRedriverAddress, 0x40
                         1, /// mRedriverInterface,  1
                         3, /// mRedriverLane,
                         0, /// mRedriverModel,
                         0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                         4, /// mPadMux0,  4
                         0, /// mPadMux1,
                         0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                         0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                         0, /// mTxEqPre,  TBD
                         0, /// mTxEqMain, TBD    0x28
                         0  /// mTxEqPost  TBD
                     )
                },
               { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};

DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCardPDE[] = { 
               {// P0 - SATA port
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_SATA (
                   DxioPortEnabled                       // Port Present
                 )
               },
               { // P0 - EtherNet Port 0
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                    DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8
                                                                                                 /*< Platform Config
                                                                                                 *  0 = Reserved
                                                                                                 *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                                 *  2 = 2.5G BackPlane
                                                                                                 *  3= Soldered down 1000Base-T
                                                                                                 *  4 = Soldered down 1000Base-X
                                                                                                 *  5 = Soldered down NBase-T
                                                                                                 *  6 = Soldered down 10GBase-T
                                                                                                 *  7 = Soldered down 10GBase-r
                                                                                                 *  8 = SFP+ Connector
                                                                                                 */
                     0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                     0xB, /// mSuppSpeed   0xB
                                                                                                  /*< Supported Speeds by Platform             !!!!!!!
                                                                                                  *  1 = 100M Supported
                                                                                                  *  2 = 1G Supported
                                                                                                  *  4 = 2.5G Supported
                                                                                                  *  8 = 10G Supported
                          */
                     1, /// mConnType   1
                                                                                                  /*< Supported Speeds by Platform
                                                                                                  *  0 = Port not Used
                                                                                                  *  1 = SFP+ Connection I2C interface
                                                                                                  *  2 = MDIO PHY
                                                                                                  *  4 = Backplane Connection     !!!!!
                                                                                                  */
                     0, /// mMdioReset   0
                                                                                                  /*< MDIO Reset Type   
                                                                                                  *  0 = None
                                                                                                  *  1 = I2C GPIO
                                                                                                  *  2 = Integrated GPIO
                                                                                                  *  3 = Reserved
                                                                                                  *  If an external MDIO PHY is used, this field 
                                                                                                  *  indicates the method used by the software 
                                                                                                  *  driver to control for the reset to that PHY
                                                                                                  *  11b = Reserved
                                                                                                  *  10b = Integrated GPIO
                                                                                                  *  01b = I2C GPIO
                                                                                                  *  00b = None
                                                                                                  */
                     0, /// mMdioGpioResetNum
                                                                                                  /*
                                                                                                   * If MDIO Reset Type is set to 10b, this field 
                                                                                                   * indicates the Ethernet integrated GPIO number 
                                                                                                   * used to control the reset.
                                                                                                   */
                     3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                   /*
                                                                                                    * If an external MDIO PHY is used in 
                                                                                                    combination with an I2C GPIO reset, this field 
                                                                                                    indicates the lower I2C address of the 
                                                                                                    PCA9535 GPIO expander used to drive the reset.
                                                                                                    If this port is used as part of an SFP+ 
                                                                                                    connection, this field indicates the lower I2C 
                                                                                                    address of the PCA9535 GPIO expander used 
                                                                                                    to drive the SFP+ sideband signals
                                                                                                    */
                     0x2, /// mTxFault,   0xE
                                                                                                  /*
                                                                                                    * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control either the SFP+ TX_FAULT 
                                                                                                    signal or the MDIO reset associated with this 
                                                                                                    port. If neither of those features are in use, set 
                                                                                                    this to 0.
                                                                                                    */
                     0, /// mRs,
                                                                                                  /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control the SFP+ RS signal. If this port 
                                                                                                    is not used with an SFP+ connector, set this 
                                                                                                    field to 0.
                                                                                                    */
                     0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                                  //  set this field to 0.
                                                                                                 
                     0x1, /// mRxLoss,    0xD      
                                                                                                  /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                    used to control the SFP+ Rx_LOS signal. If 
                                                                                                    this port is not used with an SFP+ connector, 
                                                                                                    set this field to 0.
                                                                                                    */
                     2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                     0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                              /*
                                                                                               * Set this field to indicate the lower address of 
                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                SFP+ connector. If no multiplexing is used, set 
                                                                                                this field to 111b. If this port is not used with 
                                                                                                an SFP+ connector, set this field to 0.
                                                                                                000b-011b = PCA9545 lower address
                                                                                                100b-110b = Reserved
                                                                                                111b = SFP+ directly connected to I2C
                                                                                               * */
                     0, /// mSfpBusSeg,
                                                                                               /*
                                                                                               * Set this field to indicate the downstream 
                                                                                                channel of the PCA9545 I2C multiplexer used 
                                                                                                to drive the SFP+ connector. If no multiplexing
                                                                                                is used, set this field to 111b. If this port is not 
                                                                                                used with an SFP+ connector, set this field to 
                                                                                                0.
                                                                                                000b-011b = PCA9545 lower address
                                                                                                100b-110b = Reserved
                                                                                                111b = SFP+ directly connected to processor 
                                                                                                I2C*/
                     0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                              /*
                                                                                               * Set this field to indicate the upper address of 
                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                SFP+ connector. PCA9545 comes in 3 variants 
                                                                                                each with a different upper address. 
                                                                                                11100b = PCA9545A
                                                                                                11010b = PCA9545B
                                                                                                10110b = PCA9545C
                                                                                                11111b = SFP+ directly connected to processor
                                                                                                I2C
                                                                                                If no multiplexing is used, set this field to 
                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                connector, set this field to 0.
                                                                                                */
                     0x40, /// mRedriverAddress, 0x40
                                                                                                  /*
                                                                                                   * This field encodes the I2C or MDIO address 
                                                                                                    used to control a platform level redriver 
                                                                                                    connected to this port. If no redriver is used, 
                                                                                                    set this field to 0. If the redriver is connected 
                                                                                                    using MDIO, the upper 2 address bits must be 
                                                                                                    set to 0.
                                                                                                  */
                     1, /// mRedriverInterface,  1
                                                                                                  /*
                                                                                                   * This field encodes the I2C or MDIO address 
                                                                                                    used to control a platform level redriver 
                                                                                                    connected to this port. If no redriver is used, 
                                                                                                    set this field to 0. If the redriver is connected 
                                                                                                    using MDIO, the upper 2 address bits must be 
                                                                                                    set to 0.
                                                                                                    */
                     0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                     0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                     0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                     4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                     0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                     0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                     0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                     0, /// mTxEqPre,  TBD
                     0, /// mTxEqMain, TBD    0x28
                     0  /// mTxEqPost  TBD
                     )
                 },
                 { // P0 - EtherNet Port 1
                     0,
                     DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                     DXIO_PORT_DATA_INITIALIZER_ENET (
                       DxioPortEnabled,                      // Port Present
                       1, /// mPortNum
                       8, /// mPlatConf   8
                       0, /// mMdioId
                       0xB, /// mSuppSpeed   0xB
                       1, /// mConnType   1
                       0, /// mMdioReset   0
                       0, /// mMdioGpioResetNum
                       3, /// mSfpGpioAdd   6
                       6, /// mTxFault,   
                       0, /// mRs,
                       4, /// mModAbs,
                       5, /// mRxLoss,
                       2, /// mSfpGpioMask,  2
                       0, /// mSfpMux,
                       1, /// mSfpBusSeg,
                       0x1C, /// mSfpMuxUpAdd,  0x1C
                       0x40, /// mRedriverAddress, 0x40
                       1, /// mRedriverInterface,  1
                       1, /// mRedriverLane,
                       0, /// mRedriverModel,
                       0, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                       4, /// mPadMux0,  4
                       0, /// mPadMux1,
                       0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                       0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                       0, /// mTxEqPre,  TBD
                       0, /// mTxEqMain, TBD    0x28
                       0  /// mTxEqPost  TBD
                     )
                 },
                 { // P0 - EtherNet Port 2
                       0,
                       DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                       DXIO_PORT_DATA_INITIALIZER_ENET (
                         DxioPortEnabled,                      // Port Present
                         2, /// mPortNum
                         8, /// mPlatConf   8
                         0, /// mMdioId
                         0xB, /// mSuppSpeed   0xB
                         1, /// mConnType   1
                         0, /// mMdioReset   0
                         0, /// mMdioGpioResetNum
                         3, /// mSfpGpioAdd   6
                         0xA, /// mTxFault,   0xE
                         0, /// mRs,
                         8, /// mModAbs,   0xC
                         9, /// mRxLoss,    0xD
                         2, /// mSfpGpioMask,  2
                         0, /// mSfpMux,
                         2, /// mSfpBusSeg,
                         0x1C, /// mSfpMuxUpAdd,  0x1C
                         0x40, /// mRedriverAddress, 0x40
                         1, /// mRedriverInterface,  1
                         2, /// mRedriverLane,
                         0, /// mRedriverModel,
                         0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                         4, /// mPadMux0,  4
                         0, /// mPadMux1,
                         0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                         0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                         0, /// mTxEqPre,  TBD
                         0, /// mTxEqMain, TBD    0x28
                         0  /// mTxEqPost  TBD
                       )
                 },
                 { // P0 - EtherNet Port 3
                       0,
                       DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                       DXIO_PORT_DATA_INITIALIZER_ENET (
                         DxioPortEnabled,                      // Port Present
                         3, /// mPortNum
                         8, /// mPlatConf   8
                         0, /// mMdioId
                         0xB, /// mSuppSpeed   0xB
                         1, /// mConnType   1
                         0, /// mMdioReset   0
                         0, /// mMdioGpioResetNum
                         3, /// mSfpGpioAdd   6
                         0xE, /// mTxFault,  
                         0, /// mRs,
                         0xC, /// mModAbs,   
                         0xD, /// mRxLoss,    
                         0, /// mSfpGpioMask,  2
                         0, /// mSfpMux,
                         3, /// mSfpBusSeg,
                         0x1C, /// mSfpMuxUpAdd,  0x1C
                         0x40, /// mRedriverAddress, 0x40
                         1, /// mRedriverInterface,  1
                         3, /// mRedriverLane,
                         0, /// mRedriverModel,
                         0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                         4, /// mPadMux0,  4
                         0, /// mPadMux1,
                         0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                         0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                         0, /// mTxEqPre,  TBD
                         0, /// mTxEqMain, TBD    0x28
                         0  /// mTxEqPost  TBD
                     )
                },
               { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortDisabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};


DXIO_PORT_DESCRIPTOR    DxioCfgPcie4X4[] = {
             { //Configuration-4X4:P0 - x4
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 3, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 1,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             },
             { //Configuration-4X4:P0 - x4
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 7, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 2,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             },
             { //Configuration-4X4:P0 - x4
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 3,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             },
             { //Configuration-4X4:P0 - x4
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 15, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 4,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcie2X8[] = {
             { //Configuration-2X8:P0 - x8
                 1,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 7, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 2,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             },
             { //Configuration-2X8:P0 - x8
                 1,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 4,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcie1X16[] = {
             { //Configuration-2X8:P0 - x8
                 1,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 0, 15, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 1,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard1[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                /*< Platform Config
                                                                                                *  0 = Reserved
                                                                                                *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                                *  2 = 2.5G BackPlane
                                                                                                *  3= Soldered down 1000Base-T
                                                                                                *  4 = Soldered down 1000Base-X
                                                                                                *  5 = Soldered down NBase-T
                                                                                                *  6 = Soldered down 10GBase-T
                                                                                                *  7 = Soldered down 10GBase-r
                                                                                                *  8 = SFP+ Connector
                                                                                                */
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 /*< Supported Speeds by Platform             !!!!!!!
                                                                                                 *  1 = 100M Supported
                                                                                                 *  2 = 1G Supported
                                                                                                 *  4 = 2.5G Supported
                                                                                                 *  8 = 10G Supported
                         */
                    1, /// mConnType   1
                                                                                                 /*< Supported Speeds by Platform
                                                                                                 *  0 = Port not Used
                                                                                                 *  1 = SFP+ Connection I2C interface
                                                                                                 *  2 = MDIO PHY
                                                                                                 *  4 = Backplane Connection     !!!!!
                                                                                                 */
                    0, /// mMdioReset   0
                                                                                                 /*< MDIO Reset Type   
                                                                                                 *  0 = None
                                                                                                 *  1 = I2C GPIO
                                                                                                 *  2 = Integrated GPIO
                                                                                                 *  3 = Reserved
                                                                                                 *  If an external MDIO PHY is used, this field 
                                                                                                 *  indicates the method used by the software 
                                                                                                 *  driver to control for the reset to that PHY
                                                                                                 *  11b = Reserved
                                                                                                 *  10b = Integrated GPIO
                                                                                                 *  01b = I2C GPIO
                                                                                                 *  00b = None
                                                                                                 */
                    0, /// mMdioGpioResetNum
                                                                                                 /*
                                                                                                  * If MDIO Reset Type is set to 10b, this field 
                                                                                                  * indicates the Ethernet integrated GPIO number 
                                                                                                  * used to control the reset.
                                                                                                  */
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  /*
                                                                                                   * If an external MDIO PHY is used in 
                                                                                                   combination with an I2C GPIO reset, this field 
                                                                                                   indicates the lower I2C address of the 
                                                                                                   PCA9535 GPIO expander used to drive the reset.
                                                                                                   If this port is used as part of an SFP+ 
                                                                                                   connection, this field indicates the lower I2C 
                                                                                                   address of the PCA9535 GPIO expander used 
                                                                                                   to drive the SFP+ sideband signals
                                                                                                   */
                    0x2, /// mTxFault,   0xE
                                                                                                 /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control either the SFP+ TX_FAULT 
                                                                                                   signal or the MDIO reset associated with this 
                                                                                                   port. If neither of those features are in use, set 
                                                                                                   this to 0.
                                                                                                   */
                    0, /// mRs,
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ RS signal. If this port 
                                                                                                   is not used with an SFP+ connector, set this 
                                                                                                   field to 0.
                                                                                                   */
                    0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                                 //  set this field to 0.
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ Rx_LOS signal. If 
                                                                                                   this port is not used with an SFP+ connector, 
                                                                                                   set this field to 0.
                                                                                                   */
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             /*
                                                                                              * Set this field to indicate the lower address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. If no multiplexing is used, set 
                                                                                               this field to 111b. If this port is not used with 
                                                                                               an SFP+ connector, set this field to 0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to I2C
                                                                                              * */
                    0, /// mSfpBusSeg,
                                                                                              /*
                                                                                              * Set this field to indicate the downstream 
                                                                                               channel of the PCA9545 I2C multiplexer used 
                                                                                               to drive the SFP+ connector. If no multiplexing
                                                                                               is used, set this field to 111b. If this port is not 
                                                                                               used with an SFP+ connector, set this field to 
                                                                                               0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to processor 
                                                                                               I2C*/
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             /*
                                                                                              * Set this field to indicate the upper address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. PCA9545 comes in 3 variants 
                                                                                               each with a different upper address. 
                                                                                               11100b = PCA9545A
                                                                                               11010b = PCA9545B
                                                                                               10110b = PCA9545C
                                                                                               11111b = SFP+ directly connected to processor
                                                                                               I2C
                                                                                               If no multiplexing is used, set this field to 
                                                                                               11111b. If this port is not used with an SFP+ 
                                                                                               connector, set this field to 0.
                                                                                               */
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                 */
                    1, /// mRedriverInterface,  1
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                   */
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     2, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      2,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard2[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                /*< Platform Config
                                                                                                *  0 = Reserved
                                                                                                *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                                *  2 = 2.5G BackPlane
                                                                                                *  3= Soldered down 1000Base-T
                                                                                                *  4 = Soldered down 1000Base-X
                                                                                                *  5 = Soldered down NBase-T
                                                                                                *  6 = Soldered down 10GBase-T
                                                                                                *  7 = Soldered down 10GBase-r
                                                                                                *  8 = SFP+ Connector
                                                                                                */
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 /*< Supported Speeds by Platform             !!!!!!!
                                                                                                 *  1 = 100M Supported
                                                                                                 *  2 = 1G Supported
                                                                                                 *  4 = 2.5G Supported
                                                                                                 *  8 = 10G Supported
                         */
                    1, /// mConnType   1
                                                                                                 /*< Supported Speeds by Platform
                                                                                                 *  0 = Port not Used
                                                                                                 *  1 = SFP+ Connection I2C interface
                                                                                                 *  2 = MDIO PHY
                                                                                                 *  4 = Backplane Connection     !!!!!
                                                                                                 */
                    0, /// mMdioReset   0
                                                                                                 /*< MDIO Reset Type   
                                                                                                 *  0 = None
                                                                                                 *  1 = I2C GPIO
                                                                                                 *  2 = Integrated GPIO
                                                                                                 *  3 = Reserved
                                                                                                 *  If an external MDIO PHY is used, this field 
                                                                                                 *  indicates the method used by the software 
                                                                                                 *  driver to control for the reset to that PHY
                                                                                                 *  11b = Reserved
                                                                                                 *  10b = Integrated GPIO
                                                                                                 *  01b = I2C GPIO
                                                                                                 *  00b = None
                                                                                                 */
                    0, /// mMdioGpioResetNum
                                                                                                 /*
                                                                                                  * If MDIO Reset Type is set to 10b, this field 
                                                                                                  * indicates the Ethernet integrated GPIO number 
                                                                                                  * used to control the reset.
                                                                                                  */
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  /*
                                                                                                   * If an external MDIO PHY is used in 
                                                                                                   combination with an I2C GPIO reset, this field 
                                                                                                   indicates the lower I2C address of the 
                                                                                                   PCA9535 GPIO expander used to drive the reset.
                                                                                                   If this port is used as part of an SFP+ 
                                                                                                   connection, this field indicates the lower I2C 
                                                                                                   address of the PCA9535 GPIO expander used 
                                                                                                   to drive the SFP+ sideband signals
                                                                                                   */
                    0x2, /// mTxFault,   0xE
                                                                                                 /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control either the SFP+ TX_FAULT 
                                                                                                   signal or the MDIO reset associated with this 
                                                                                                   port. If neither of those features are in use, set 
                                                                                                   this to 0.
                                                                                                   */
                    0, /// mRs,
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ RS signal. If this port 
                                                                                                   is not used with an SFP+ connector, set this 
                                                                                                   field to 0.
                                                                                                   */
                    0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                                 //  set this field to 0.
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ Rx_LOS signal. If 
                                                                                                   this port is not used with an SFP+ connector, 
                                                                                                   set this field to 0.
                                                                                                   */
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             /*
                                                                                              * Set this field to indicate the lower address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. If no multiplexing is used, set 
                                                                                               this field to 111b. If this port is not used with 
                                                                                               an SFP+ connector, set this field to 0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to I2C
                                                                                              * */
                    0, /// mSfpBusSeg,
                                                                                              /*
                                                                                              * Set this field to indicate the downstream 
                                                                                               channel of the PCA9545 I2C multiplexer used 
                                                                                               to drive the SFP+ connector. If no multiplexing
                                                                                               is used, set this field to 111b. If this port is not 
                                                                                               used with an SFP+ connector, set this field to 
                                                                                               0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to processor 
                                                                                               I2C*/
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             /*
                                                                                              * Set this field to indicate the upper address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. PCA9545 comes in 3 variants 
                                                                                               each with a different upper address. 
                                                                                               11100b = PCA9545A
                                                                                               11010b = PCA9545B
                                                                                               10110b = PCA9545C
                                                                                               11111b = SFP+ directly connected to processor
                                                                                               I2C
                                                                                               If no multiplexing is used, set this field to 
                                                                                               11111b. If this port is not used with an SFP+ 
                                                                                               connector, set this field to 0.
                                                                                               */
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                 */
                    1, /// mRedriverInterface,  1
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                   */
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     1, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSfpSlotUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1, /// mSfpMux,
                        1, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1, /// mSfpMux,
                        1, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard3[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                /*< Platform Config
                                                                                                *  0 = Reserved
                                                                                                *  1 = 10G/1G BackPlane          !!!!!!!!
                                                                                                *  2 = 2.5G BackPlane
                                                                                                *  3= Soldered down 1000Base-T
                                                                                                *  4 = Soldered down 1000Base-X
                                                                                                *  5 = Soldered down NBase-T
                                                                                                *  6 = Soldered down 10GBase-T
                                                                                                *  7 = Soldered down 10GBase-r
                                                                                                *  8 = SFP+ Connector
                                                                                                */
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 /*< Supported Speeds by Platform             !!!!!!!
                                                                                                 *  1 = 100M Supported
                                                                                                 *  2 = 1G Supported
                                                                                                 *  4 = 2.5G Supported
                                                                                                 *  8 = 10G Supported
                         */
                    1, /// mConnType   1
                                                                                                 /*< Supported Speeds by Platform
                                                                                                 *  0 = Port not Used
                                                                                                 *  1 = SFP+ Connection I2C interface
                                                                                                 *  2 = MDIO PHY
                                                                                                 *  4 = Backplane Connection     !!!!!
                                                                                                 */
                    0, /// mMdioReset   0
                                                                                                 /*< MDIO Reset Type   
                                                                                                 *  0 = None
                                                                                                 *  1 = I2C GPIO
                                                                                                 *  2 = Integrated GPIO
                                                                                                 *  3 = Reserved
                                                                                                 *  If an external MDIO PHY is used, this field 
                                                                                                 *  indicates the method used by the software 
                                                                                                 *  driver to control for the reset to that PHY
                                                                                                 *  11b = Reserved
                                                                                                 *  10b = Integrated GPIO
                                                                                                 *  01b = I2C GPIO
                                                                                                 *  00b = None
                                                                                                 */
                    0, /// mMdioGpioResetNum
                                                                                                 /*
                                                                                                  * If MDIO Reset Type is set to 10b, this field 
                                                                                                  * indicates the Ethernet integrated GPIO number 
                                                                                                  * used to control the reset.
                                                                                                  */
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  /*
                                                                                                   * If an external MDIO PHY is used in 
                                                                                                   combination with an I2C GPIO reset, this field 
                                                                                                   indicates the lower I2C address of the 
                                                                                                   PCA9535 GPIO expander used to drive the reset.
                                                                                                   If this port is used as part of an SFP+ 
                                                                                                   connection, this field indicates the lower I2C 
                                                                                                   address of the PCA9535 GPIO expander used 
                                                                                                   to drive the SFP+ sideband signals
                                                                                                   */
                    0x2, /// mTxFault,   0xE
                                                                                                 /*
                                                                                                   * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control either the SFP+ TX_FAULT 
                                                                                                   signal or the MDIO reset associated with this 
                                                                                                   port. If neither of those features are in use, set 
                                                                                                   this to 0.
                                                                                                   */
                    0, /// mRs,
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ RS signal. If this port 
                                                                                                   is not used with an SFP+ connector, set this 
                                                                                                   field to 0.
                                                                                                   */
                    0x0, /// mModAbs,   0xC                                     //Set this field to the PCA9535 GPIO number used to control the SFP+ Mod_ABS signal. If this port is not used with an SFP+ connector,
                                                                                                 //  set this field to 0.
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 /*
                                                                                                  * Set this field to the PCA9535 GPIO number 
                                                                                                   used to control the SFP+ Rx_LOS signal. If 
                                                                                                   this port is not used with an SFP+ connector, 
                                                                                                   set this field to 0.
                                                                                                   */
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             /*
                                                                                              * Set this field to indicate the lower address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. If no multiplexing is used, set 
                                                                                               this field to 111b. If this port is not used with 
                                                                                               an SFP+ connector, set this field to 0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to I2C
                                                                                              * */
                    0, /// mSfpBusSeg,
                                                                                              /*
                                                                                              * Set this field to indicate the downstream 
                                                                                               channel of the PCA9545 I2C multiplexer used 
                                                                                               to drive the SFP+ connector. If no multiplexing
                                                                                               is used, set this field to 111b. If this port is not 
                                                                                               used with an SFP+ connector, set this field to 
                                                                                               0.
                                                                                               000b-011b = PCA9545 lower address
                                                                                               100b-110b = Reserved
                                                                                               111b = SFP+ directly connected to processor 
                                                                                               I2C*/
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             /*
                                                                                              * Set this field to indicate the upper address of 
                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                               SFP+ connector. PCA9545 comes in 3 variants 
                                                                                               each with a different upper address. 
                                                                                               11100b = PCA9545A
                                                                                               11010b = PCA9545B
                                                                                               10110b = PCA9545C
                                                                                               11111b = SFP+ directly connected to processor
                                                                                               I2C
                                                                                               If no multiplexing is used, set this field to 
                                                                                               11111b. If this port is not used with an SFP+ 
                                                                                               connector, set this field to 0.
                                                                                               */
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                 */
                    1, /// mRedriverInterface,  1
                                                                                                 /*
                                                                                                  * This field encodes the I2C or MDIO address 
                                                                                                   used to control a platform level redriver 
                                                                                                   connected to this port. If no redriver is used, 
                                                                                                   set this field to 0. If the redriver is connected 
                                                                                                   using MDIO, the upper 2 address bits must be 
                                                                                                   set to 0.
                                                                                                   */
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     0, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1, /// mSlotMux,
                      0, /// mSlotBusSeg,
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1, /// mSlotMux,
                        0, /// mSlotBusSeg,
                        0x1C, /// mSlotMuxUpAdd,  0x1C
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1, /// mSlotMux,
                        0, /// mSlotBusSeg,
                        0x1C, /// mSlotMuxUpAdd,  0x1C
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgX8SataX8Pcie[] = {
            { // GPIO88 && GPIO92 is high SATA x4 ports
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 7, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_SATA (
                 DxioPortEnabled                       // Port Present
                 )
             },
             { //Configuration-2X8:P0 - x8
                 1,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                 DxioPortEnabled,                      // Port not Present
                 1,                                    // Requested Device
                 1,                                    // Requested Function
                 DxioHotplugDisabled,                  // Hotplug
                 DxioGenMaxSupported,                            // Max Link Speed
                 DxioGenMaxSupported,                            // Max Link Capability
                 DxioAspmDisabled,                        // ASPM
                 DxioAspmDisabled,                     // ASPM L1.1 disabled
                 DxioAspmDisabled,                     // ASPM L1.2 disabled
                 DxioClkPmSupportDisabled              // Clock PM
                )
             }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_2SFP_2SGMII_1[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8                                                                                              
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB                                                                                                  
                    1, /// mConnType   1                                                                                                
                    0, /// mMdioReset   0
                                                                                                
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                 
                    0x0, /// mSfpMux,                                        
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     2, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      2,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        1, /// mMdioId     1
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_4SGMII_1[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   3, /// mPlatConf   3
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    2, /// mConnType   2
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     2, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    0x10, /// mPadMux0,  0x10      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      3, /// mPlatConf   3
                      1, /// mMdioId     1
                      0xB, /// mSuppSpeed   0xB
                      2, /// mConnType   2
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      2,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      0x10, /// mPadMux0,  0x10
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        2, /// mMdioId     2
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        3, /// mMdioId     3
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};
DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCard_2SFP_2SGMII[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8                                                                                              
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB                                                                                                  
                    1, /// mConnType   1                                                                                                
                    0, /// mMdioReset   0
                                                                                                
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                 
                    0x0, /// mSfpMux,                                        
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        1, /// mMdioId     1
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};

DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCard_4SGMII[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   3, /// mPlatConf   3
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    2, /// mConnType   2
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                    0x10, /// mPadMux0,  0x10      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      3, /// mPlatConf   3
                      1, /// mMdioId     1
                      0xB, /// mSuppSpeed   0xB
                      2, /// mConnType   2
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                      0x10, /// mPadMux0,  0x10
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        2, /// mMdioId     2
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        3, /// mMdioId     3
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};

DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCard_KP_inphi4223[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                               
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 
                         
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                                                                                                 
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                            
                    0, /// mSfpBusSeg,
                                                                                              
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      1, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
                      { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};
DXIO_PORT_DESCRIPTOR    DxioTypeACfgDie0FieldCard_KR_BCM84328[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    2, /// mRedriverModel, 2  // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                     0x1, /// mSlotMux,        //
                                                                                                         /*
                                                                                                          * Set this field to indicate the lower address of 
                                                                                                            the PCA9545 I2C multiplexer used to drive the
                                                                                                            slot which FieldCard inserted . If no multiplexing 
                                                                                                            is used, set this field to 111b.
                                                                                                            000b-011b = PCA9545 lower address
                                                                                                            100b-110b = Reserved
                                                                                                            111b = No Slot Mux used
                                                                                                           * */
                      3, /// mSlotBusSeg,
                                                                                                           /*
                                                                                                            * Set this field to indicate the downstream 
                                                                                                              channel of the PCA9545 I2C multiplexer used 
                                                                                                              to drive the slot which FieldCard inserted. 
                                                                                                              If no multiplexing is used, set this field 
                                                                                                              to 111b. 
                                                                                                              000b-011b = PCA9545 lower address
                                                                                                              100b-110b = Reserved
                                                                                                              111b = SFP+ directly connected to processor 
                                                                                                              I2C*/
                       0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                             /*
                                                                                                              * Set this field to indicate the upper address of 
                                                                                                                the PCA9545 I2C multiplexer used to drive the
                                                                                                                slot which FieldCard inserted. PCA9545 comes
                                                                                                                in 3 variants ,each with a different upper address. 
                                                                                                                11100b = PCA9545A
                                                                                                                11010b = PCA9545B
                                                                                                                10110b = PCA9545C
                                                                                                                11111b = SFP+ directly connected to processor I2C

                                                                                                                If no multiplexing is used, set this field to 
                                                                                                                 11111b. If this port is not used with an SFP+ 
                                                                                                                connector, set this field to 0.
                                                                                                                 */
                     0x4,///mInfoRomAdd,At24c08d low i2c address
                     0x14,///mInfoRomAddUP,At24c08d up i2c address
                     1, ///InfoRomLocation,0=on board,1=field card  
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      2, /// mRedriverModel,    2
                      1, /// mRedriverPresent,  1
                       1, /// mSlotMux
                       3, /// mSlotBusSeg
                       0x1c, ///mSlotMuxUpAdd
                       0x4,///mInfoRomAdd,At24c08d low i2c address
                       0x14,///mInfoRomAddUP,At24c08d up i2c address
                       1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                         1, /// mSlotMux
                         3, /// mSlotBusSeg
                         0x1c, ///mSlotMuxUpAdd
                         0x4,///mInfoRomAdd,At24c08d low i2c address
                         0x14,///mInfoRomAddUP,At24c08d up i2c address
                         1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
                      { // P0 - x4 I350
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 11, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                  // Max Link Speed
                   DxioGenMaxSupported,                  // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
               },
               { // P0 - x1 BMC
                 0,
                 DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 12, 12, DxioHotplugDisabled, 1),
                 DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port Present
                   1,                                    // Requested Device
                   2,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGen1,                             // Max Link Speed
                   DxioGen1,                             // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                 )
                }, 
                { // P0 - x1 slot
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 13, 13, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (
                    DxioPortDisabled,                      // Port Present
                    1,                                    // Requested Device
                    3,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                    )
                },
                { // P0 - x2
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 14, 15, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_PCIE (                  
                    DxioPortEnabled,                      // Port Present
                    1,                                    // Requested Device
                    4,                                    // Requested Function
                    DxioHotplugDisabled,                  // Hotplug
                    DxioGenMaxSupported,                  // Max Link Speed
                    DxioGenMaxSupported,                  // Max Link Capability
                    DxioAspmDisabled,                        // ASPM
                    DxioAspmDisabled,                     // ASPM L1.1 disabled
                    DxioAspmDisabled,                     // ASPM L1.2 disabled
                    DxioClkPmSupportDisabled              // Clock PM
                  )
                }
};
DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KP_inphi4223_1[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                               
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 
                         
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                                                                                                 
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                            
                    0, /// mSfpBusSeg,
                                                                                              
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     2, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      2,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KR_BCM84328_1[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    2, /// mRedriverModel, 2  // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     2, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      2, /// mRedriverModel,    2
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      2,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        2,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_2SFP_2SGMII_2[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8                                                                                              
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB                                                                                                  
                    1, /// mConnType   1                                                                                                
                    0, /// mMdioReset   0
                                                                                                
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                 
                    0x0, /// mSfpMux,                                        
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     1, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      1,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        1, /// mMdioId     1
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_4SGMII_2[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   3, /// mPlatConf   3
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    2, /// mConnType   2
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     1, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    0x10, /// mPadMux0,  0x10      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      3, /// mPlatConf   3
                      1, /// mMdioId     1
                      0xB, /// mSuppSpeed   0xB
                      2, /// mConnType   2
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      1,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      0x10, /// mPadMux0,  0x10
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        2, /// mMdioId     2
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        3, /// mMdioId     3
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KP_inphi4223_2[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                               
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 
                         
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                                                                                                 
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                            
                    0, /// mSfpBusSeg,
                                                                                              
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     1, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      1,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KR_BCM84328_2[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    2, /// mRedriverModel, 2  // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     1, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      2, /// mRedriverModel,    2
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      1,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        1,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_2SFP_2SGMII_3[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                    0, /// mPortNum
                    8, /// mPlatConf   8                                                                                              
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB                                                                                                  
                    1, /// mConnType   1                                                                                                
                    0, /// mMdioReset   0
                                                                                                
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                 
                    0x0, /// mSfpMux,                                        
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     0, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      0,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        1, /// mMdioId     1
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_4SGMII_3[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   3, /// mPlatConf   3
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    2, /// mConnType   2
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    0, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     0, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    0x10, /// mPadMux0,  0x10      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      3, /// mPlatConf   3
                      1, /// mMdioId     1
                      0xB, /// mSuppSpeed   0xB
                      2, /// mConnType   2
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      0, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      0, /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      0x10, /// mPadMux0,  0x10
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        3, /// mPlatConf   3
                        2, /// mMdioId     2
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        3, /// mPlatConf   3
                        3, /// mMdioId     3
                        0xB, /// mSuppSpeed   0xB
                        2, /// mConnType   2
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        0, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        0x10, /// mPadMux0,  0x10
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KP_inphi4223_3[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                               
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                 
                         
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                                                                                                 
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                     
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                            
                    0, /// mSfpBusSeg,
                                                                                              
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                 
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    0, /// mRedriverModel,   // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     0, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      0, /// mRedriverModel,
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      0,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        0, /// mRedriverModel,
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

DXIO_PORT_DESCRIPTOR    DxioCfgPcieFieldCard_KR_BCM84328_3[] = {
             { // GPIO88 && GPIO92 is high SATA x4 ports
                  0,
                  DXIO_ENGINE_DATA_INITIALIZER (DxioSATAEngine, 0, 3, DxioHotplugDisabled, 1),
                  DXIO_PORT_DATA_INITIALIZER_SATA (
                  DxioPortEnabled                       // Port Present
                  )
              },
              { // P0 - EtherNet Port 0
                   0,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 4, 4, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_ENET (
                   DxioPortEnabled,                      // Port Present
                   0, /// mPortNum
                   8, /// mPlatConf   8
                                                                                                
                    0, /// mMdioId  //< MDIO ID when MDIO Side band is used       ?????????
                    0xB, /// mSuppSpeed   0xB
                                                                                                
                    1, /// mConnType   1
                                                                                                 
                    0, /// mMdioReset   0
                    0, /// mMdioGpioResetNum
                                                                                                 
                    3, /// mSfpGpioAdd   6    //on FILED card, PCA9535 A2A1A0 = 3b100 
                                                                                                  
                    0x2, /// mTxFault,   0xE
                                                                                                 
                    0, /// mRs,
                                                                                                 
                    0x0, /// mModAbs,   0xC                                    
                                                                                                
                    0x1, /// mRxLoss,    0xD      
                                                                                                 
                    2, /// mSfpGpioMask,  2                                //Set bits in this field to indicate SFP+ sideband signals which are not supported at the platform 
                    0x0, /// mSfpMux,                                        // On field card, PCA9545  A1A0 = 2b10
                                                                                             
                    0, /// mSfpBusSeg,
                                                                                             
                    0x1C, /// mSfpMuxUpAdd,  0x1C
                                                                                             
                    0x40, /// mRedriverAddress, 0x40
                                                                                                
                    1, /// mRedriverInterface,  1
                                                                                                 
                    0, /// mRedriverLane,    //This field indicates the lane on the redriver connected to the processor port.    
                    2, /// mRedriverModel, 2  // This field indicates the control interface used by the redriver 0=MDIO   1=I2C
                    1, /// mRedriverPresent,  1   //0=Platform level redriver is not used     1=Platform level redriver is connected to the processor PHY 
                    0x1, /// mSlotMux,        //
                                                                                                        /*
                                                                                                         * Set this field to indicate the lower address of 
                                                                                                           the PCA9545 I2C multiplexer used to drive the
                                                                                                           slot which FieldCard inserted . If no multiplexing 
                                                                                                           is used, set this field to 111b.
                                                                                                           000b-011b = PCA9545 lower address
                                                                                                           100b-110b = Reserved
                                                                                                           111b = No Slot Mux used
                                                                                                          * */
                     0, /// mSlotBusSeg,
                                                                                                          /*
                                                                                                           * Set this field to indicate the downstream 
                                                                                                             channel of the PCA9545 I2C multiplexer used 
                                                                                                             to drive the slot which FieldCard inserted. 
                                                                                                             If no multiplexing is used, set this field 
                                                                                                             to 111b. 
                                                                                                             000b-011b = PCA9545 lower address
                                                                                                             100b-110b = Reserved
                                                                                                             111b = SFP+ directly connected to processor 
                                                                                                             I2C*/
                      0x1C, /// mSlotMuxUpAdd,  0x1C
                                                                                                            /*
                                                                                                             * Set this field to indicate the upper address of 
                                                                                                               the PCA9545 I2C multiplexer used to drive the
                                                                                                               slot which FieldCard inserted. PCA9545 comes
                                                                                                               in 3 variants ,each with a different upper address. 
                                                                                                               11100b = PCA9545A
                                                                                                               11010b = PCA9545B
                                                                                                               10110b = PCA9545C
                                                                                                               11111b = SFP+ directly connected to processor I2C

                                                                                                               If no multiplexing is used, set this field to 
                                                                                                                11111b. If this port is not used with an SFP+ 
                                                                                                               connector, set this field to 0.
                                                                                                               */
                    0x4,///mInfoRomAdd,At24c08d low i2c address
                    0x14,///mInfoRomAddUP,At24c08d up i2c address
                    1, ///InfoRomLocation,0=on board,1=field card
                    4, /// mPadMux0,  4      //XGBECTRL::PAD_MUX0[7:0] = 4h  I2C mode and use SFP_SCL/SFP_SDA
                    0, /// mPadMux1,          //When use GPIO mode, set this bit to 1 or 2h
                    0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                    0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                    0, /// mTxEqPre,  TBD
                    0, /// mTxEqMain, TBD    0x28
                    0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 1
                    0,
                    DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 5, 5, DxioHotplugDisabled, 1),
                    DXIO_PORT_DATA_INITIALIZER_ENET (
                      DxioPortEnabled,                      // Port Present
                      1, /// mPortNum
                      8, /// mPlatConf   8
                      0, /// mMdioId
                      0xB, /// mSuppSpeed   0xB
                      1, /// mConnType   1
                      0, /// mMdioReset   0
                      0, /// mMdioGpioResetNum
                      3, /// mSfpGpioAdd   6
                      6, /// mTxFault,   
                      0, /// mRs,
                      4, /// mModAbs,
                      5, /// mRxLoss,
                      2, /// mSfpGpioMask,  2
                      0, /// mSfpMux,
                      1, /// mSfpBusSeg,
                      0x1C, /// mSfpMuxUpAdd,  0x1C
                      0x40, /// mRedriverAddress, 0x40
                      1, /// mRedriverInterface,  1
                      1, /// mRedriverLane,
                      2, /// mRedriverModel,    2
                      1, /// mRedriverPresent,  1
                      1,  /// mSlotMux
                      0,  /// mSlotBusSeg
                      0x1c, /// mSlotMuxUpAdd
                      0x4,///mInfoRomAdd,At24c08d low i2c address
                      0x14,///mInfoRomAddUP,At24c08d up i2c address
                      1, ///InfoRomLocation,0=on board,1=field card
                      4, /// mPadMux0,  4
                      0, /// mPadMux1,
                      0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                      0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                      0, /// mTxEqPre,  TBD
                      0, /// mTxEqMain, TBD    0x28
                      0  /// mTxEqPost  TBD
                    )
                },
                { // P0 - EtherNet Port 2
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 6, 6, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        2, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xA, /// mTxFault,   0xE
                        0, /// mRs,
                        8, /// mModAbs,   0xC
                        9, /// mRxLoss,    0xD
                        2, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        2, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        2, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                      )
                },
                { // P0 - EtherNet Port 3
                      0,
                      DXIO_ENGINE_DATA_INITIALIZER (DxioEthernetEngine, 7, 7, DxioHotplugDisabled, 1),
                      DXIO_PORT_DATA_INITIALIZER_ENET (
                        DxioPortEnabled,                      // Port Present
                        3, /// mPortNum
                        8, /// mPlatConf   8
                        0, /// mMdioId
                        0xB, /// mSuppSpeed   0xB
                        1, /// mConnType   1
                        0, /// mMdioReset   0
                        0, /// mMdioGpioResetNum
                        3, /// mSfpGpioAdd   6
                        0xE, /// mTxFault,  
                        0, /// mRs,
                        0xC, /// mModAbs,   
                        0xD, /// mRxLoss,    
                        0, /// mSfpGpioMask,  2
                        0, /// mSfpMux,
                        3, /// mSfpBusSeg,
                        0x1C, /// mSfpMuxUpAdd,  0x1C
                        0x40, /// mRedriverAddress, 0x40
                        1, /// mRedriverInterface,  1
                        3, /// mRedriverLane,
                        2, /// mRedriverModel,    2
                        1, /// mRedriverPresent,  1
                        1,  /// mSlotMux
                        0,  /// mSlotBusSeg
                        0x1c, /// mSlotMuxUpAdd
                        0x4,///mInfoRomAdd,At24c08d low i2c address
                        0x14,///mInfoRomAddUP,At24c08d up i2c address
                        1, ///InfoRomLocation,0=on board,1=field card
                        4, /// mPadMux0,  4
                        0, /// mPadMux1,
                        0, /// mMacAddressLo, (TO BE FILLED IN BY CODE)
                        0, /// mMacAddressHi, (TO BE FILLED IN BY CODE)
                        0, /// mTxEqPre,  TBD
                        0, /// mTxEqMain, TBD    0x28
                        0  /// mTxEqPost  TBD
                    )
               }, 
               { //Configuration-2X8:P0 - x8
                   1,
                   DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, DxioHotplugDisabled, 1),
                   DXIO_PORT_DATA_INITIALIZER_PCIE (
                   DxioPortEnabled,                      // Port not Present
                   1,                                    // Requested Device
                   1,                                    // Requested Function
                   DxioHotplugDisabled,                  // Hotplug
                   DxioGenMaxSupported,                            // Max Link Speed
                   DxioGenMaxSupported,                            // Max Link Capability
                   DxioAspmDisabled,                        // ASPM
                   DxioAspmDisabled,                     // ASPM L1.1 disabled
                   DxioAspmDisabled,                     // ASPM L1.2 disabled
                   DxioClkPmSupportDisabled              // Clock PM
                   )
               }
};

CUSTOMER_DXIO_PLAN  Die0Type0DxioConfigPlan[]={
    {DxioTypeACfgDie0Normal,5},
    {DxioTypeACfgDie0FieldCard,9},
    {DxioTypeACfgDie0FieldCard_2SFP_2SGMII,9},
    {DxioTypeACfgDie0FieldCard_4SGMII,9},
    {DxioTypeACfgDie0FieldCard_KR_BCM84328,9},
    {DxioTypeACfgDie0FieldCard_KP_inphi4223,9},
    {DxioCfgPcie1X16,1},
    {DxioTypeACfgDie0FieldCardPDE,9},
};
CUSTOMER_DXIO_PLAN  Die0Type1DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}    
};
CUSTOMER_DXIO_PLAN  Die1Type0DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcieFieldCard1,6},
    {DxioCfgPcieFieldCard_2SFP_2SGMII_1,6},
    {DxioCfgPcieFieldCard_4SGMII_1,6},
    {DxioCfgPcieFieldCard_KR_BCM84328_1,6},
    {DxioCfgPcieFieldCard_KP_inphi4223_1,6},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4},
    {DxioCfgX8SataX8Pcie,2}
};
CUSTOMER_DXIO_PLAN  Die1Type1DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}         
};
CUSTOMER_DXIO_PLAN  Die2Type0DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcieFieldCard2,6},
    {DxioCfgPcieFieldCard_2SFP_2SGMII_2,6},
    {DxioCfgPcieFieldCard_4SGMII_2,6},
    {DxioCfgPcieFieldCard_KR_BCM84328_2,6},
    {DxioCfgPcieFieldCard_KP_inphi4223_2,6},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}         
};
CUSTOMER_DXIO_PLAN  Die2Type1DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}         
};
CUSTOMER_DXIO_PLAN  Die3Type0DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcieFieldCard3,6},
    {DxioCfgPcieFieldCard_2SFP_2SGMII_3,6},
    {DxioCfgPcieFieldCard_4SGMII_3,6},
    {DxioCfgPcieFieldCard_KR_BCM84328_3,6},
    {DxioCfgPcieFieldCard_KP_inphi4223_3,6},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}         
};
CUSTOMER_DXIO_PLAN  Die3Type1DxioConfigPlan[]={
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie1X16,1},
    {DxioCfgPcie2X8,2},
    {DxioCfgPcie4X4,4}         
};


/** @file  OemPCIeDevCodeId.h

    Header file for Pci Code Id to name

**/

#ifndef __OEM_PCIE_DEV_CODE_ID_H__
#define __OEM_PCIE_DEV_CODE_ID_H__

typedef struct _PCI_DEV_CLASS_INFO{
  UINT8		BaseClass;
  UINT8		SubClass;
  UINT8		ProgInterface;
  CHAR16  *DeviceName;
} PCI_DEV_CLASS_INFO;

typedef struct _PCI_DEV_VID_INFO{
  UINT16   VendorId;
  CHAR16   *VName;
} PCI_DEV_VID_INFO;

PCI_DEV_NINFO PCIDevDB[] = {//CPL VERSION 10.3

//  VendorID DeviceID SubsystemID   Speed        BrandName

 //jingjiamicro
  { 0x0731,  0x7200,  0x7201,  Speed_Undefined,  L"jingjiamicro JM7201"                                   },  //<Songkl001-20211124 Update CPL V9.9 >

  //LSI
  { 0x1000,  0x0016,  0x9461,  Speed_Undefined,  L"BCM 9460 -E3x8 8i SE 2GB Tri-Mode RAID"                },  //<Fancf002-20210330 Update CPL V9.3 >
  { 0x1000,  0x005D,  0x0608,  Speed_Undefined,  L"LSI Logic SAS3108"                                     },  //<Lvshh003-20220507 Update CPL V10.3 >
  { 0x1000,  0x005D,  0x9361,  Speed_Undefined,  L"LSI Logic SAS3108"                                     },  //<duanjun 20211027 change to LSI Logic SAS3108 as cpl 9.6>
  { 0x1000,  0x005D,  0x936A,  Speed_Undefined,  L"LSI Logic SAS3108"                                     },  //<Lvshh003-20220507 Update CPL V10.3 >
  { 0x1000,  0x005F,  0x9341,  Speed_Undefined,  L"LSI 9341-8i -E3x8 SE SAS 12G RAID"                     },  //<Fancf002-20210330 Update CPL V9.3 >
  { 0x1000,  0x0097,  0x0097,  Speed_Undefined,  L"SH08-L3008 8i SAS 12G HBA"                             },  
  { 0x1000,  0x0097,  0x0098,  Speed_Undefined,  L"SH08-L3008 8i SAS 12Gb HBA"                            },  //<Fancf003-20201225 Update CPL V9.1 >
  { 0x1000,  0x0097,  0x101E,  Speed_Undefined,  L"LSI Logic SAS3008"                                     },  //<Lvshh003-20220507 Update CPL V10.3 >
  { 0x1000,  0x0097,  0x3008,  Speed_Undefined,  L"SH08-L3008 8i SAS 12G RAID"                            },
  { 0x1000,  0x0097,  0x3090,  Speed_Undefined,  L"LSI Logic SAS3008"                                     },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x1000,  0x00C9,  0x3180,  Speed_Undefined,  L"SH16-L3216 16i SAS 12G HBA"                            },
  { 0x1000,  0x00C9,  0x3190,  Speed_Undefined,  L"LSI 9305 16i SAS 12G HBA"                              },
  { 0x1000,  0x00CE,  0x3316,  Speed_Undefined,  L"SR16-L3316 16i 4G SAS 12G RAID"                        },
  { 0x1000,  0x00CE,  0x9371,  Speed_Undefined,  L"LSI Logic SAS3316"                                     },  //<Fancf001-20210106 Update PCIe device from 2021-Q1 >
  { 0x1000,  0x00E6,  0x4050,  Speed_Undefined,  L"LSI Tri-Mode SAS3816"                                  },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1000,  0x00E6,  0x4060,  Speed_Undefined,  L"LSI Tri-Mode SAS3808"                                  },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1000,  0x00AF,  0x3010,  Speed_Undefined,  L"BCM 9400 -E3x8 8i SE Tri-Mode HBA"                     },  //<Fancf002-20210330 Update CPL V9.3 >
  { 0x1000,  0x10E2,  0x4000,  Speed_Undefined,  L"LSI Tri-Mode SAS3916"                                  },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1000,  0x10E2,  0x4010,  Speed_Undefined,  L"LSI Tri-Mode SAS3908"                                  },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1000,  0x10E6,  0x40D5,  Speed_Undefined,  L"LSI Tri-Mode SAS3808"                                  },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >

  //AMD
  { 0x1002,  0x6611,  0x1869,  Speed_Undefined,  L"AMD R520"                                              },  //<Songkl001-20211025 Update CPL V9.8 >
  { 0x1002,  0x6611,  0x3000,  Speed_Undefined,  L"AMD Radeon R7 340"                                     },  //<Songkl001-20211025 Update CPL V9.8 >
  { 0x1002,  0x6779,  0x2300,  Speed_Undefined,  L"Sinead AMD R5 230"                                     },
//{ 0x1002,  0x66A1,  0x0834,  Speed_Undefined,  L"AMD RADEON INSTINCT MI60"                              },  //Need to be verified.
//{ 0x1002,  0x66A1,  0x0834,  Speed_Undefined,  L"AMD RADEON INSTINCT MI50"                              },  //Need to be verified.
  { 0x1002,  0x699F,  0x0000,  Speed_Undefined,  L"AMD RX550"                                             },  //<Songkl001-20211025 Update CPL V9.8 >
  
  //NVIDIA
  { 0x10DE,  0x1287,  0x1287,  Speed_Undefined,  L"GT 730"                                                },  //<Lvshh001-20211105 Add PCIe device from 2021-11 >  
  { 0x10DE,  0x1287,  0x730B,  Speed_Undefined,  L"Geforce GT 730"                                        },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x128B,  0x118B,  Speed_Undefined,  L"NVIDIA GeForce GT 710"                                 },
  { 0x10DE,  0x1B30,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P6000"                                   },
  { 0x10DE,  0x1BB0,  0x11B2,  Speed_Undefined,  L"NVIDIA Quadro P5000"                                   },
  { 0x10DE,  0x1BB1,  0x11A3,  Speed_Undefined,  L"NVIDIA Quadro P4000"                                   },
  { 0x10DE,  0x1C30,  0x11B3,  Speed_Undefined,  L"NVIDIA Quadro P2000"                                   }, 
  { 0x10DE,  0x1C31,  0x131B,  Speed_Undefined,  L"NVIDIA Quadro P2200"                                   },
  { 0x10DE,  0x1CB1,  0x11BC,  Speed_Undefined,  L"NVIDIA Quadro P1000"                                   },
  { 0x10DE,  0x1CB3,  0x11BE,  Speed_Undefined,  L"NVIDIA Quadro P400"                                    },
  { 0x10DE,  0x1CB6,  0x1264,  Speed_Undefined,  L"NVIDIA Quadro P620"                                    },
  { 0x10DE,  0x1D02,  0x1D02,  Speed_Undefined,  L"Geforce GT 1010"                                       },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x10DE,  0x1DBA,  0x121A,  Speed_Undefined,  L"NVIDIA Quadro GV100"                                   },
  { 0x10DE,  0x1DF2,  0x13F6,  Speed_Undefined,  L"NVIDIA PG503-216"                                      },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x10DE,  0x1DF6,  0x13D6,  Speed_Undefined,  L"NVIDIA Tesla V100S 32G"                                },
  { 0x10DE,  0x1E07,  0x12FA,  Speed_Undefined,  L"Sinead RTX 2080 Ti A"                                  },
  { 0x10DE,  0x1E07,  0x1E07,  Speed_Undefined,  L"Leadtek 2080 Ti LT OC "                                },
  { 0x10DE,  0x1E30,  0x129E,  Speed_Undefined,  L"NVIDIA Quadro RTX 8000"                                },
  { 0x10DE,  0x1E30,  0x12BA,  Speed_Undefined,  L"NVIDIA Quadro RTX 6000"                                },
  { 0x10DE,  0x1E78,  0x13D8,  Speed_Undefined,  L"NVIDIA Quadro RTX 8000 Passive"                        },
  { 0x10DE,  0x1E78,  0x13D9,  Speed_Undefined,  L"NVIDIA Quadro RTX 6000 Passive "                       },
  { 0x10DE,  0x1EB0,  0x129F,  Speed_Undefined,  L"NVIDIA Quadro RTX 5000"                                },
  { 0x10DE,  0x1EB1,  0x12A0,  Speed_Undefined,  L"NVIDIA Quadro RTX 4000"                                },
  { 0x10DE,  0x1EB8,  0x12A2,  Speed_Undefined,  L"NVIDIA Tesla T4 16G"                                   },
  { 0x10DE,  0x1FB0,  0x12DB,  Speed_Undefined,  L"NV T1000 4GB"                                          },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x1FB1,  0x1488,  Speed_Undefined,  L"NV T600"                                               },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x1FB2,  0x1489,  Speed_Undefined,  L"NV T400"                                               },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x1FF0,  0x1612,  Speed_Undefined,  L"NV T1000 8GB"                                          },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x1FF2,  0x1613,  Speed_Undefined,  L"NV T400 4GB"                                           },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x20B5,  0x1533,  Speed_Undefined,  L"NVIDIA A100 80G"                                       },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x20B7,  0x1532,  Speed_Undefined,  L"NVIDIA A30 24G"                                        },  //<xuyj1001-20210621 add A30 24G 165W NVIDIA GPU Card>
  { 0x10DE,  0x20F1,  0x145F,  Speed_Undefined,  L"NVIDIA Tesla A100 40G"                                 },
  { 0x10DE,  0x2204,  0x0000,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },  //chenpeng-20201124 +>Add RTX 3090 GPU card.
  { 0x10DE,  0x2204,  0x1454,  Speed_Undefined,  L"RTX 3090"                                              },  //<Songkl001-20211025 Update CPL V9.8 >
  { 0x10DE,  0x2204,  0x1455,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },  //<ouyyx-20201219-Add SND GPU RTX3090 Info For x620>
  { 0x10DE,  0x2204,  0x2204,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },  //<libj-20210426 Update Device ID for NVIDIA GeForce RTX 3090 +>
  { 0x10DE,  0x2204,  0x3881,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },  //<Hugc-20210913 Add MSI GPU Info>
  { 0x10DE,  0x2204,  0x87D5,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },  //<ouyyx-20201219-Add ASUS GPU RTX3090 Info For x620>
  { 0x10DE,  0x2204,  0xFFFF,  Speed_Undefined,  L"NVIDIA GeForce RTX 3090"                               },
  { 0x10DE,  0x2206,  0x0000,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080"                               },  //chenpeng-20201124 +>Add RTX 3080 GPU card.
  { 0x10DE,  0x2206,  0x1454,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080"                               },  //<ouyyx-20201219-Add SND GPU RTX3080 Info For x620>
  { 0x10DE,  0x2206,  0x3896,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080 LHR"                           },  //<Hugc-20210913 Add MSI GPU Info>
  { 0x10DE,  0x2206,  0x389A,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080"                               },  //<Hugc-20210913 Add MSI GPU Info>
  { 0x10DE,  0x2206,  0xFFFF,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080"                               },  //<ouyyx-20201219-Add GPU RTX3080 Info For x620>
  { 0x10DE,  0x2206,  0x1455,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080"                               },  //<libj-20210426 Update Device ID for NVIDIA GeForce RTX 3080 +>
  { 0x10DE,  0x2208,  0x389F,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080 Ti"                            },  //<Hugc-20210913 Add MSI GPU Info>    
  { 0x10DE,  0x2216,  0x2216,  Speed_Undefined,  L"Geforce RTX 3080"                                      },  //<ChengXW001-20210831 Add RTX 3080>
  { 0x10DE,  0x2216,  0xFFFF,  Speed_Undefined,  L"NVIDIA GeForce RTX 3080 LHR"                           },  //<Hugc-20210913 Add MSI GPU Info>
  { 0x10DE,  0X2230,  0X1459,  Speed_Undefined,  L"NV RTX A6000"                                          },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0X2231,  0X147E,  Speed_Undefined,  L"NV RTX A5000"                                          },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x2232,  0x163C,  Speed_Undefined,  L"NV RTX A4500 20GB"                                     },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x2235,  0x145A,  Speed_Undefined,  L"NVIDIA A40 48G"                                        },  //<Lvshh001-20201120 Update CPL V9.0>
  { 0x10DE,  0x2236,  0x1482,  Speed_Undefined,  L"NVIDIA A10 24G"                                        },  //<Lvshh001-20201120 Update CPL V9.0>
  { 0x10DE,  0x2484,  0x146B,  Speed_Undefined,  L"Geforce RTX 3070"                                      },  //<Yanshp-20210610 Add RTX 3070 >
  { 0x10DE,  0x2484,  0x2484,  Speed_Undefined,  L"Geforce RTX 3070"                                      },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x10DE,  0x24B0,  0X14ad,  Speed_Undefined,  L"NV RTX A4000"                                          },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x10DE,  0x2503,  0x2503,  Speed_Undefined,  L"Geforce RTX 3060"                                      },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x10DE,  0x2531,  0x151D,  Speed_Undefined,  L"NV RTX A2000 6GB"                                      },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x2571,  0x1611,  Speed_Undefined,  L"NV RTX A2000 12GB"                                     },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x10DE,  0x25B6,  0x14A9,  Speed_Undefined,  L"NVIDIA A16"                                            },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x10DE,  0x25B6,  0x157E,  Speed_Undefined,  L"NV A2"                                                 },  //<Lvshh004-20220507 Fix issue >

  //ZOTAC
  { 0x10DE,  0x128B,  0x7326,  Speed_Undefined,  L"NVIDIA GT710"                                          },  //<Fancf001-20210106 Update PCIe device from 2021-Q1 >
    
  //REALTEK
  { 0x10EC,  0x8168,  0x0001,  Speed_1Gbps,      L"Realtek 1G RJ45 1-Port NIC"                            },  //<Lvshh001-20210515 Add PC Onboard RTL8111HN-CG NIC>
  { 0x10EC,  0x8168,  0x0123,  Speed_1Gbps,      L"Realtek EGR1-8111HN 1-port 1G SFP NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >

  //Highpoint
  { 0x1103,  0x3720,  0x0000,  Speed_Undefined,  L"RocketRAID 3720c"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >

  //Micron
  { 0x1344,  0x51B1,  0x4000,  Speed_Undefined,  L"Micron 9300 U.2 "                                      },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1344,  0x51B1,  0x5000,  Speed_Undefined,  L"Micron 9300 U.2 "                                      },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1344,  0x51B1,  0x6000,  Speed_Undefined,  L"Micron 9300 U.2 "                                      },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1344,  0x51C0,  0x4000,  Speed_Undefined,  L"Micron 7400 U.2 "                                      },  //<Lvshh001-20220216 Add PCIe device from 2022-Q1 >
  
  //Samsung
  { 0x144D,  0xA808,  0xA802,  Speed_Undefined,  L"Samsung MZQLB1T9HAJR-3"                                },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x144D,  0xA80A,  0xA812,  Speed_Undefined,  L"MZ1L2960HCJR-00A07"                                    },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x144D,  0xA80A,  0xA813,  Speed_Undefined,  L"Samsung PM9A3 U.2"                                     },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x144D,  0xA80A,  0xA818,  Speed_Undefined,  L"Samsung MZQL23T8HCLS"                                  },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x144D,  0xA824,  0xA801,  Speed_Undefined,  L"Samsung PM1733 Gen4 U.2"                               },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >

  //BCM  //BROADCOM
  { 0x14E4,  0x16D6,  0x4124,  Speed_10Gbps,     L"Broadcom BCM957412A4120AC 10G SFP+ 2-port NIC"         },  //<Lvshh001-20210728 Add PCIe device from 2021-Q3 >
  { 0x14E4,  0x16D6,  0x4126,  Speed_10Gbps,     L"broadcom BCM957412N4120C 2-Ports 10G SFP+ OCP3.0 NIC"  },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x14E4,  0x16D7,  0x1402,  Speed_25Gbps,     L"BCM BCM957414A4142CC 2-port 25G SFP28 NIC"             },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x14E4,  0x16D7,  0x4146,  Speed_25Gbps,     L"broadcom BCM957414N4140C 2-Ports 25G SFP28 OCP3.0 NIC" },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x14E4,  0x16D8,  0x4166,  Speed_10Gbps,     L"broadcom BCM957416N4160C 2-Ports 10G RJ45 OCP3.0 NIC"  },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x14E4,  0x1750,  0x2100,  Speed_100Gbps,    L"BCM BCM957508-P2100G 2-port 100G QSFP56 NIC"           },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x14E4,  0x1750,  0x5208,  Speed_100Gbps,    L"Broadcom BCM957508-N2100G 100G QSFP56 2-port NIC"      },  //<Lvshh001-20210728 Add PCIe device from 2021-Q3 >
  { 0x14E4,  0x1751,  0x4250,  Speed_25Gbps,     L"BCM BCM957504-P425G 4-port 25G SFP28 NIC"              },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x14E4,  0x1751,  0x5100,  Speed_100Gbps,    L"BCM BCM957504-N1100G 1-port 100G O3SFF QSFP56 NIC"     },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x14E4,  0x1751,  0x5105,  Speed_100Gbps,    L"BCM BCM957504-N1100GD 1-port 100G O3SFF DSFP56 NIC"    },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x14E4,  0x1751,  0x5425,  Speed_25Gbps,     L"BCM BCM957504-N425G 4-port 25G O3SFF SFP28 NIC"        },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x14E4,  0xD802,  0x8021,  Speed_25Gbps,     L"BCM BCM958802A8046C 2-port 25G SFP28 SmartNIC"         },  //<Lvshh002-20220215 Update CPL V10.1 >
  
  //Mellanox
  { 0x15B3,  0x1015,  0x0001,  Speed_25Gbps,     L"MLX MCX4111A-ACAT 1-port 25G SFP28 NIC"                },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0003,  Speed_25Gbps,     L"MLX MCX4121A-ACAT 2-port 25G SFP28 NIC"                },  //<Fancf001-20210204 Modify MLX NIC name >
  { 0x15B3,  0x1015,  0x0098,  Speed_25Gbps,     L"E25GM2-CX4-Lx-A 2-port 25G SFP28 NIC"                  },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x15B3,  0x1015,  0x0004,  Speed_10Gbps,     L"MLX MCX4121A-XCAT 2-port 10G SFP+ NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0102,  Speed_10Gbps,     L"E10GM2-CX4-Lx-A 2-port 10G SFP+ NIC"                   },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x15B3,  0x1015,  0x0021,  Speed_25Gbps,     L"MLX MCX4421A-ACQN 2-port 25G OCP SFP28 NIC"            },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0047,  Speed_25Gbps,     L"MLX MCX4621A-ACAB 2-port 25G O3SFF SFP28 NIC"          },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0083,  Speed_25Gbps,     L"MLX MCX4411A-ACQN_C07 1-port 25G OCP SFP28 NIC"        },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0087,  Speed_25Gbps,     L"MLX MCX4421A-ACQN_C07 2-port 25G OCP SFP28 NIC"        },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1015,  0x0091,  Speed_10Gbps,     L"MLX MCX4121A-XCAT 2-port 10G SFP+ NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0006,  Speed_100Gbps,    L"MLX MCX515A-CCAT 1-port 100G QSFP28 NIC"               },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0007,  Speed_100Gbps,    L"MLX MCX516A-CCAT 2-port 100G QSFP28 NIC"               },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0020,  Speed_25Gbps,     L"MLX MCX512A-ACAT 2-port 25G SFP28 NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0052,  Speed_25Gbps,     L"MLX MCX562A-ACAB 2-port 25G O3SFF SFP28 NIC"           },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0068,  Speed_25Gbps,     L"MLX MCX542B-ACAN 2-port 25G OCP SFP28 NIC"             },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0072,  Speed_25Gbps,     L"MLX MCX542B-ACAN_C07 2-port 25G OCP SFP28 NIC"         },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x1017,  0x0121,  Speed_25Gbps,     L"MLX MCX512A-ACUT 2-port 25G SFP28 NIC"                 },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x15B3,  0x1017,  0x0117,  Speed_100Gbps,    L"MLX MCX516A-CCHT 2-port 100G QSFP28 NIC"               },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x15B3,  0x1019,  0x0067,  Speed_100Gbps,    L"MLX MCX566A-CDAB 2-port 100G O3SFF QSFP28 NIC"         },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x101B,  0x0007,  Speed_200Gbps,    L"MLX MCX653105A-HDAT 1-port 200G HDR QSFP56 HCA"        },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x101B,  0x0008,  Speed_100Gbps,    L"MLX MCX653106A-ECAT 2-port 100G HDR100 QSFP56 HCA"     },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x101D,  0x0006,  Speed_100Gbps,    L"MLX MCX623436AN-CDAB 2-port 100G O3SFF QSFP56 NIC"     },  //<Fancf002-20210330 Update CPL V9.3 >
  { 0x15B3,  0x101D,  0x0011,  Speed_25Gbps,     L"MLX MCX623102AN-ADAT 2-port 25G SFP28 NIC"             },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x15B3,  0x101D,  0x0013,  Speed_25Gbps,     L"MLX MCX623432AN-ADAB 2-port 25G O3SFF SFP28 NIC"       },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x15B3,  0x101D,  0x0016,  Speed_100Gbps,    L"MLX 2-port 100G PCIE QSFP56 NIC 2-port 100G QSFP56 NIC"},  //<Songkl001-20211022 Update CPL V9.8 >
  { 0x15B3,  0x101D,  0x0018,  Speed_200Gbps,    L"MLX MCX623105AN-VDAT 1-port 200G QSFP56 NIC"           },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0039,  Speed_100Gbps,    L"MLX MCX623105AN-CDAT 1-port 100G QSFP56 NIC"           },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0067,  Speed_100Gbps,    L"MLX CX623439MS-CDAB_28 1-port 100G O3SFF DSFP56 NIC"   },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0076,  Speed_100Gbps,    L"MLX CX623435MN-CDAB_28 1-port 100G O3SFF QSFP56 NIC"   },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0107,  Speed_25Gbps,     L"MLX MCX623432AS-ADAB 2-port 25G O3SFF SFP28 NIC"       },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0108,  Speed_25Gbps,     L"MLX MCX623102AS-ADAT 2-port 25G SFP28 NIC"             },  //<Lvshh001-20210727 Update CPL V9.6 >
  { 0x15B3,  0x101D,  0x0114,  Speed_100Gbps,    L"MLX MCX623430MS-CDAB 2-port 100G O3SFF DSFP56 NIC"     },  //<Songkl001-20211124 Update CPL V9.9 >
  { 0x15B3,  0x101F,  0x0001,  Speed_25Gbps,     L"E25GM2-CX6-Lx-A2-port 25G SFP28 NIC"                   },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x15B3,  0x101F,  0x0004,  Speed_25Gbps,     L"MLX MCX631432AN-ADAB 2-port 25G O3SFF SFP28 NIC"       },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x15B3,  0x4119,  0x0052,  Speed_25Gbps,     L"MLX MCX562A-ACAB 25G SFP28 2-Ports OCP NIC"            },  //<Lvshh001-20201120 Update CPL V9.0>
  { 0x15B3,  0x4121,  0x0067,  Speed_100Gbps,    L"MLX MCX566A-CDAB 100G QSFP28 2-Ports OCP NIC"          },  //<Lvshh001-20201120 Update CPL V9.0>
  { 0x15B3,  0x4125,  0x0006,  Speed_100Gbps,    L"MLX MCX623436AN-CDAB 100G QSFP56 2-Ports OCP NIC"      },  //<Lvshh001-20201120 Update CPL V9.0>
  { 0x15B3,  0xA2D6,  0x0052,  Speed_25Gbps,     L"MLX MBF2H332A-AEEOT 8C16GE 2-port 25G SFP28 SmartNIC"  },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x15B3,  0xA2D6,  0x0060,  Speed_25Gbps,     L"MLX MBF2H332A-AENOT"                                   },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x15B3,  0xA2D6,  0x0061,  Speed_25Gbps,     L"MLX MBF2H332A-AEEOT 8C16GE 2-port 25G SFP28 SmartNIC"  },  //<Fancf001-20210106 Update PCIe device from 2021-Q1 >
  { 0x15B3,  0xA2D2,  0x0082,  Speed_25Gbps,     L"MLX MBF1M332A-ASCAT 16C16GE 2-port 25G SFP28 SmartNIC" },  //<Fancf001-20210106 Update PCIe device from 2021-Q1 >
  { 0x15B3,  0xA2D2,  0x0085,  Speed_25Gbps,     L"MLX MBF1M332A-ASNAT 16C16GE 2-port 25G SFP28 SmartNIC" },  //<Fancf001-20210129 Add SmartNIC >
  { 0x15B3,  0xA2D6,  0x0109,  Speed_25Gbps,     L"MLX MBF2H512C-AESOT 8C16GE 2-port 25G SFP56 SmartNIC"  },  //<Lvshh002-20220215 Update CPL V10.1 >
  
  //ASMedia Technology Inc.
  { 0x1B21,  0x0622,  0x1060,  Speed_Undefined,  L"SR02-A1061R 2i SATA 6G RAID"                           },

  //WD
  { 0x1B96,  0x2400,  0x0000,  Speed_Undefined,  L"WD Aspen Plus U.2"                                     },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1B96,  0x2500,  0x0000,  Speed_Undefined,  L"WD SN840 Gen3 U.2"                                     },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >

  //Memblaze
  { 0x1C5F,  0x000D,  0x0220,  Speed_Undefined,  L"Memblaze PB5 520 AIC"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x0221,  Speed_Undefined,  L"Memblaze PB5 520 U.2"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x0230,  Speed_Undefined,  L"Memblaze PB5 520 AIC"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x0231,  Speed_Undefined,  L"Memblaze PB5 520 U.2"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x4220,  Speed_Undefined,  L"Memblaze PB5 526 AIC"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x4221,  Speed_Undefined,  L"Memblaze PB5 526 U.2"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x4230,  Speed_Undefined,  L"Memblaze PB5 526 AIC"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000D,  0x4231,  Speed_Undefined,  L"Memblaze PB5 526 U.2"                                  },  //<Fancf002-20210409 Add MB U.2/AIC info as specifications >
  { 0x1C5F,  0x000E,  0x0B20,  Speed_Undefined,  L"Memblaze P6530CH0192M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x0B21,  Speed_Undefined,  L"Memblaze P6530DT0192M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x0B30,  Speed_Undefined,  L"Memblaze P6530CH0384M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x0B31,  Speed_Undefined,  L"Memblaze P6530DT0384M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x0B40,  Speed_Undefined,  L"Memblaze P6530CH0768M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x0B41,  Speed_Undefined,  L"Memblaze P6530DT0768M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B20,  Speed_Undefined,  L"Memblaze P6536CH0160M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B21,  Speed_Undefined,  L"Memblaze P6536DT0160M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B30,  Speed_Undefined,  L"Memblaze P6536CH0320M00 "                              },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B31,  Speed_Undefined,  L"Memblaze P6536DT0320M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B40,  Speed_Undefined,  L"Memblaze P6536CH0640M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x000E,  0x4B41,  Speed_Undefined,  L"Memblaze P6536DT0640M00"                               },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1C5F,  0x003D,  0x0A30,  Speed_Undefined,  L"Memblaze PB5 920 AIC"                                  },  //<Fancf002-20200919 Update PCIe device table based on CPL-V8.8 >
  { 0x1C5F,  0x003E,  0x0A31,  Speed_Undefined,  L"Memblaze P6920 U.2"                                    },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1C5F,  0x003E,  0x4A31,  Speed_Undefined,  L"Memblaze P6920 U.2"                                    },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1C5F,  0x003D,  0x4A40,  Speed_Undefined,  L"Memblaze PB5 926 AIC"                                  },  //<Fancf002-20200919 Update PCIe device table based on CPL-V8.8 >
  { 0x1C5F,  0x003D,  0x4A41,  Speed_Undefined,  L"Memblaze PB5 926 U.2"                                  },  //<Fancf002-20200919 Update PCIe device table based on CPL-V8.8 >
  { 0x1C5F,  0x003E,  0x0A41,  Speed_Undefined,  L"Memblaze P6920 U.2"                                    },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1C5F,  0x003E,  0x4A41,  Speed_Undefined,  L"Memblaze P6920 U.2"                                    },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1C5F,  0x0557,  0x055B,  Speed_Undefined,  L"Memblaze P5910DT1536M00"                               },  //<Lvshh001-20210727 Update CPL V9.6 >
  
  //KUNLUN
  { 0x1D22,  0x3684,  0x0000,  Speed_Undefined,  L"KUNLUN SKU K100/K200"                                  },  //<ChengXW001-20210728 Add KUNLUN card from BaiDu custom requirement>

  //DERA
  { 0x1D78,  0x1512,  0x2004,  Speed_Undefined,  L"DERA D5437 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x2006,  Speed_Undefined,  L"DERA D5437 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x2008,  Speed_Undefined,  L"DERA D5437 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x2104,  Speed_Undefined,  L"DERA D5437 U.2"                                        },  //<Fancf001-20200914 Add DERA D5437 U.2 support >
  { 0x1D78,  0x1512,  0x2106,  Speed_Undefined,  L"DERA D5437 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1D78,  0x1512,  0x2108,  Speed_Undefined,  L"DERA D5437 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1D78,  0x1512,  0x3003,  Speed_Undefined,  L"DERA D5457 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x3005,  Speed_Undefined,  L"DERA D5457 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x3007,  Speed_Undefined,  L"DERA D5457 AIC"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x3103,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1D78,  0x1512,  0x3105,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1D78,  0x1512,  0x3107,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1D78,  0x1512,  0x3103,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x3105,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x3107,  Speed_Undefined,  L"DERA D5457 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x5003,  Speed_Undefined,  L"DERA D5447 AIC"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5004,  Speed_Undefined,  L"DERA D5427 AIC"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5005,  Speed_Undefined,  L"DERA D5447 AIC"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5006,  Speed_Undefined,  L"DERA D5427 AIC"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5103,  Speed_Undefined,  L"DERA D5447 U.2"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5104,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x5106,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x5107,  Speed_Undefined,  L"DERA D5447 U.2"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1512,  0x5108,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  { 0x1D78,  0x1512,  0x5114,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Lvshh001-20211106 Change SSID from FW 144 to 148 >
  { 0x1D78,  0x1512,  0x5116,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Lvshh001-20211106 Change SSID from FW 144 to 148 >
  { 0x1D78,  0x1512,  0x5118,  Speed_Undefined,  L"DERA D5427 U.2"                                        },  //<Lvshh001-20211106 Change SSID from FW 144 to 148 >
  { 0x1D78,  0x1512,  0x5105,  Speed_Undefined,  L"DERA D5447 U.2"                                        },  //<Lvshh002-20211106 Add D5427 AIC/D5447 U.2/D5447 AIC >
  { 0x1D78,  0x1513,  0x2104,  Speed_Undefined,  L"DERA P34TTR-02T0U-ST"                                  },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1D78,  0x1513,  0x2106,  Speed_Undefined,  L"DERA P34TTR-04T0U-ST"                                  },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1D78,  0x1513,  0x2108,  Speed_Undefined,  L"DERA P34TTR-08T0U-ST"                                  },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1D78,  0x1513,  0x3105,  Speed_Undefined,  L"DERA P34TTM-03T2U-ST"                                  },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1D78,  0x1513,  0x3107,  Speed_Undefined,  L"DERA P34TTM-06T4U-ST"                                  },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x1D78,  0x1513,  0x6106,  Speed_Undefined,  L"DERAP34TTR04T0US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6105,  Speed_Undefined,  L"DERAP34TTM03T2US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6107,  Speed_Undefined,  L"DERAP34TTM06T4US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6108,  Speed_Undefined,  L"DERAP34TTR08T0US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6115,  Speed_Undefined,  L"DERAP34YGM03T2US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6116,  Speed_Undefined,  L"DERAP34YGR04T0US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6117,  Speed_Undefined,  L"DERAP34YGM06T4US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1D78,  0x1513,  0x6118,  Speed_Undefined,  L"DERAP34YGR08T0US"                                      },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >

  
  //Hygon DCU
  { 0x1D94,  0x55B7,  0x55B7,  Speed_Undefined,  L"DCU Z100L 32G"                                         },  //<Lvshh003-20220507 Update CPL V10.3 >
  { 0x1D94,  0x54B7,  0x54B7,  Speed_Undefined,  L"DCU Z100 32G"                                          },  //<Lvshh003-20220507 Update CPL V10.3 >
 
  //Kioxia
  { 0x1E0F,  0x0007,  0x0001,  Speed_Undefined,  L"Kioxia CM6 U.2"                                        },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x1E0F,  0x0007,  0x0021,  Speed_Undefined,  L"Kioxia CD6L U.2"                                       },  //<Lvshh003-20211106 Add Kioxia CD6L U.2 >
  { 0x1E0F,  0x0011,  0x0001,  Speed_Undefined,  L"KCD71RUG960G"                                          },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  
  //DAPU
  { 0x1E3B,  0x0600,  0x0050,  Speed_Undefined,  L"DPRD3104T0"                                            },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1E3B,  0x0600,  0x0051,  Speed_Undefined,  L"DPRD3104T0"                                            },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1E3B,  0x0600,  0x0053,  Speed_Undefined,  L"DPRD3104T0"                                            },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1E3B,  0x0600,  0x0054,  Speed_Undefined,  L"DPRD3104T0"                                            },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x1E3B,  0x0600,  0x0082,  Speed_Undefined,  L"DPN5314T000"                                           },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x1E3B,  0x1098,  0x0009,  Speed_Undefined,  L"DPH311T4T000T8"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x000A,  Speed_Undefined,  L"DPH311T4T000T9"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x000C,  Speed_Undefined,  L"DPH311T4T001T6"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x000D,  Speed_Undefined,  L"DPH311T4T001T9"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0014,  Speed_Undefined,  L"DPH311T4T003T2"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0015,  Speed_Undefined,  L"DPH311T4T003T8"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0021,  Speed_Undefined,  L"DPH311T4T006T4"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0022,  Speed_Undefined,  L"DPH311T4T007T6"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x006C,  Speed_Undefined,  L"DPH312T4T000T8"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x006D,  Speed_Undefined,  L"DPH312T4T000T9"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x006F,  Speed_Undefined,  L"DPH312T4T001T6"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0070,  Speed_Undefined,  L"DPH312T4T001T9"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x007C,  Speed_Undefined,  L"DPH312T4T003T2"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x007D,  Speed_Undefined,  L"DPH312T4T003T8"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x007F,  Speed_Undefined,  L"DPH312T4T006T4"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0080,  Speed_Undefined,  L"DPH312T4T007T6"                                        },  //<Lvshh001-20210807 Add some DapuStor PCIe device >
  { 0x1E3B,  0x1098,  0x0100,  Speed_Undefined,  L"DPH312T4T001T9 1.92T HHHL -E3x4 R SSD"	              },  //<Songkl001-20211124 Update CPL V9.9 >
  
  //SUNIX
  { 0x1FD4,  0x1999,  0x0004,  Speed_Undefined,  L"SUNIX Multiport serial controller"                     },  //<Lvshh002-20220507 Add SUNIX Multiport serial controller>
  { 0x1FD4,  0x1999,  0x0100,  Speed_Undefined,  L"SUNIX 1-Port Parallel Card"                            },  //<Lvshh001-20211102 Add SUNIX 1-Port Parallel Card>
  
  //Intel  //SiNEAD
  { 0x8086,  0x10C9,  0x10C9,  Speed_1Gbps,      L"Intel I350 1Gb RJ45 NIC"                               },  // 0x8086   24000611   NIC	Silicom PEG2i6 PCI-E 2 port 1Gb RJ45 NIC
  { 0x8086,  0x10C9,  0xA04C,  Speed_1Gbps,      L"Intel I350 1Gb RJ45 NIC"                               },  // 0x8086   24000606   NIC	Intel I350 1Gb RJ45 NIC
  { 0x8086,  0x10FB,  0x0003,  Speed_10Gbps,     L"Intel 82599 2-port 10G SFP+ NIC"                       },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x10FB,  0x0301,  Speed_10Gbps,     L"SND OE10GI2-82599 2-port 10G OCP SFP+ NIC"             },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x10FB,  0x030A,  Speed_10Gbps,     L"SND E10GI2-82599-US 2-port 10G SFP+ NIC"               },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x10FB,  0x82A1,  Speed_10Gbps,     L"SCC 82599 2-port 10G SFP+ NIC"                         }, //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x8086,  0x1521,  0x0300,  Speed_1Gbps,      L"SND OE1GI2-I350 2-port 1G OCP RJ45 NIC"                },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1521,  0x0308,  Speed_1Gbps,      L"SND EGI2-I350-US 2-port 1G RJ45 NIC"                   },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1521,  0x0309,  Speed_1Gbps,      L"SND EGI4-I350-US 4-port 1G RJ45 NIC"                   },  //<Lvshh001-20210809 Update CPL V9.6 >
//{ 0x8086,  0x1521,  0x0311,  Speed_1Gbps,      L"BLD I350 1G RJ45 2-Ports OCP3.0 NIC"                   },
  { 0x8086,  0x1521,  0x0311,  Speed_1Gbps,      L"SND O3E1GI2-I350 2-port 1Gb RJ45 OCP3.0 NIC"           },  //<Fancf003-20201225 Update CPL V9.1 >
//{ 0x8086,  0x1521,  0x0312,  Speed_1Gbps,      L"BLD I350 1G RJ45 4-Ports OCP3.0 NIC"                   },
  { 0x8086,  0x1521,  0x0312,  Speed_1Gbps,      L"SND O3E1GI4-I350 4-port 1G O3SFF RJ45 NIC"             },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1521,  0x1521,  Speed_1Gbps,      L"Intel I350 1G RJ45 NIC"                                },  // 0x8086   24000740   NIC	SLC Intel I350 1G RJ45 NIC
  { 0x8086,  0x1521,  0x50A1,  Speed_1Gbps,      L"SCC I350 2-port 1G RJ45 NIC"                           }, //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x8086,  0x1522,  0x030F,  Speed_1Gbps,      L"SND E1GI2-I350 2-port 1G SFP NIC"                      },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1522,  0x1522,  Speed_1Gbps,      L"Intel I350 1G SFP NIC"                                 },  // 0x8086   24001225   NIC	SLC Intel I350 1G SFP NIC  
  { 0x8086,  0x1526,  0x0000,  Speed_1Gbps,      L"Intel I350 1G RJ45 NIC"                                },  // 0x8086   24000591   NIC	Intel I350 1G RJ45 NIC
  { 0x8086,  0x1528,  0x0001,  Speed_10Gbps,     L"Intel X540 10G RJ45 NIC"                               },  // 0x8086   24000766   NIC	Intel X540 10G RJ45 NIC
  { 0x8086,  0x1528,  0x001A,  Speed_10Gbps,     L"Intel X540-T2 2-port 10G RJ45 NIC"                     },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1528,  0x5003,  Speed_10Gbps,     L"Intel X540 10G RJ45 NIC"                               },  // 0x8086   24000891   NIC	BLD Intel X540 10G RJ45 NIC
  { 0x8086,  0x1563,  0x0302,  Speed_10Gbps,     L"SND OE10GI2-X550 2-port 10G OCP RJ45 NIC"              },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1563,  0x0307,  Speed_10Gbps,     L"SND E10GI2-X550 2-port 10G RJ45 NIC"                   },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1563,  0x0316,  Speed_10Gbps,     L"SND O3E10GI2-X550 2-port 10G O3SFF NIC"                }, //<Lvshh003-20220507 Update CPL V10.3 >
  { 0x8086,  0x1572,  0x0007,  Speed_10Gbps,     L"Intel X710-DA2 2-port 10G SFP+ NIC"                    },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1572,  0x0304,  Speed_10Gbps,     L"SND E10GI2-X710 2-port 10G SFP+ NIC"                   },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1572,  0x0305,  Speed_10Gbps,     L"SND OE10GI2-X710 2-port 10G OCP SFP+ NIC"              },  //<Lvshh001-20210809 Update CPL V9.6 >
//{ 0x8086,  0x1572,  0x0313,  Speed_10Gbps,     L"BLD X710 10G 2-Ports OCP NIC"                          },
  { 0x8086,  0x1572,  0x0313,  Speed_10Gbps,     L"SND O3E10GI2-X710 2-port 10G O3SFF SFP+ NIC"           },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1572,  0x031A,  Speed_10Gbps,     L"SND E10GI4-X710-B 4-port 10G SFP+ NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1572,  0x031E,  Speed_10Gbps,     L"SND OE10GI2-X710 2-port 10G OCP SFP+ NIC"              },  //<Lvshh001-20210809 Update CPL V9.6 >
//{ 0x8086,  0x1572,  0x71A1,  Speed_10Gbps,     L"SCC X710 -E3x8 2-port 10G SFP+ NIC"                    },  //<Lvshh001-20211105 Add PCIe device from 2021-Q4 > //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x8086,  0x1572,  0x71A1,  Speed_10Gbps,     L"SCC X710 2-port 10G SFP+ NIC"                          }, //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x8086,  0x1583,  0x0002,  Speed_40Gbps,     L"Intel EXL710QDA2 2-port 40G QSFP+ NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1584,  0x0002,  Speed_40Gbps,     L"Intel EXL710QDA1 1-port 40G QSFP+ NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x158B,  0x0001,  Speed_25Gbps,     L"Intel XXV710-DA2 2-port 25G SFP28 NIC"                 },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x1592,  0x000E,  Speed_100Gbps,    L"Intel E810-2CQDA2 100G SFP28 2-Ports E4x16 NIC"        },  //<Lvshh001-20211021 Add PCIe device from 2021-Q4 >
  { 0x8086,  0x1592,  0x0331,  Speed_100Gbps,    L"SND E100GI2-E810 2-port 100G QSFP28 NIC"               },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x8086,  0x1592,  0x0333,  Speed_100Gbps,    L"SND O3E100GI2-E810 2-port 100G O3SFF QSFP28 NIC"       },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x8086,  0x1593,  0x0005,  Speed_25Gbps,     L"Intel E810-XXVDA4 25Gb SFP28 4-Ports E4x16"            },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x8086,  0x1593,  0x0009,  Speed_25Gbps,     L"INTEL E810-XXVDA2 25G SFP28 2-Ports OCP NIC"           },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x8086,  0x159B,  0x0003,  Speed_25Gbps,     L"Intel E810-XXVDA2 2-port 25G SFP28 NIC"                },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x159b,  0x0005,  Speed_25Gbps,     L"Intel E810-XXVDA2 2-port 25G O3SFF SFP28 NIC"          },  //<Lvshh001-20210809 Update CPL V9.6 >
//{ 0x8086,  0x159B,  0x0330,  Speed_25Gbps,     L"SND E100GI2-E810 2-port 25G SFP28 NIC"                 },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x8086,  0x159B,  0x0330,  Speed_25Gbps,     L"SND E25GI2-E810 2-port 25G SFP28 NIC"                  },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x8086,  0x159B,  0x0331,  Speed_100Gbps,    L"SND E100GI2-E810 2-port 100G QSFP28 NIC"               },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x8086,  0x159B,  0x0332,  Speed_25Gbps,     L"SND O3E25GI2-E810 2-port 25G O3SFF SFP28 NIC"          },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x8086,  0x24F0,  0x0004,  Speed_100Gbps,    L"Sugon 100HFA016LS OPA 100Gb 1-Port HFA"                },  //<huangjin001-20190920 Add Sugon OPA card brandname > 
  { 0x8086,  0x37D0,  0x0000,  Speed_10Gbps,     L"Intel X722 2-port 10G SFP+ NIC"                        },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x37D1,  0x0000,  Speed_1Gbps,      L"Intel X722 2-port 1G RJ45 NIC"                         },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8086,  0x37D2,  0x0000,  Speed_10Gbps,     L"Intel X722 2-port 10G OCP RJ45 NIC"                    },  //<Lvshh001-20210809 Update CPL V9.6 >

  //Intel SSD
  { 0x8086,  0x0B60,  0x5008,  Speed_Undefined,  L"Intel P5500 U.2"                                       },  //<Fancf001-20201119 Add PCIe device from 2020-Q4 >
  { 0x8086,  0x0B60,  0x8008,  Speed_Undefined,  L"Intel P5510 U.2"                                       },  //<Fancf001-20210106 Update PCIe device from 2021-Q1 >
  { 0x8086,  0x0B60,  0x9008,  Speed_Undefined,  L"SSDPF2KX019T1"                                         },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x8086,  0x0B60,  0x9108,  Speed_Undefined,  L"SSDPF2KE016T1"                                         },  //<Lvshh001-20220215 Add PCIe device from 2022-Q1 >
  { 0x8086,  0x0B60,  0xC008,  Speed_Undefined,  L"SSDPF2KX960HZ"                                         },  //<Lvshh001-20220507 Add PCIe device from 2022-Q2 >
  { 0x8086,  0x4140,  0xAF08,  Speed_Undefined,  L"Intel Optane SSD DC P5800X U.2"                        },  //<Fancf001-20210415 Add PCIe device from 2021-Q2 >
  
  //Net-swift
  { 0x8088,  0x0101,  0x0201,  Speed_1Gbps,      L"Net-Swift SF200 2-port 1G RJ45 NIC"                    },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8088,  0x0101,  0xc201,  Speed_1Gbps,      L"Net-Swift WX1860A2 2-port 1G RJ45 LOM NIC"             },  //<ChengXW001-20210915 Add Onboard Net-Swift WX1860A2>
  { 0x8088,  0x1001,  0x0000,  Speed_10Gbps,     L"Net-Swift RP1000P2SFP 2-port 10G SFP+ NIC"             },  //<Lvshh001-20210809 Update CPL V9.6 >
  { 0x8088,  0x1001,  0x2000,  Speed_10Gbps,     L"Net-Swift RP2000P2 2-port 10G PCIE NMF Fiber NIC"      },  //<Fancf-20200526 Add Wangxun Net-swift RP2000P2 support base on CPL8.4>
  
  //PMC
  { 0x9005,  0x028f,  0x0900,  Speed_Undefined,  L"PMC 2100-8i"                                           },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >
  { 0x9005,  0x028f,  0x1282,  Speed_Undefined,  L"PMC 2100-16i"                                          },  //<Lvshh001-20210726 Add PCIe device from 2021-Q3 >

  //Cambricon
  { 0xCABC,  0x0100,  0x0013,  Speed_Undefined,  L"Cambricon MLU100-V3"                                   },
  { 0xCABC,  0x0100,  0x00C3,  Speed_Undefined,  L"Cambricon MLU100-C3"                                   },
  { 0xCABC,  0x0100,  0x00C4,  Speed_Undefined,  L"Cambricon MLU100-C4"                                   },
  { 0xCABC,  0x0270,  0x0012,  Speed_Undefined,  L"Cambricon MLU270-S4"                                   },
  { 0xCABC,  0x0270,  0x0016,  Speed_Undefined,  L"Cambricon MLU270-X5K"                                  },  //Mizl-20211027- Add For Cambricon MLU270-X5K+>
  { 0xCABC,  0x0270,  0x0024,  Speed_Undefined,  L"Cambricon MLU270-V4K"                                  },
  { 0xCABC,  0x0370,  0x0052,  Speed_Undefined,  L"MLU370-X4L"                                            },  //<Lvshh003-20220507 Update CPL V10.3 >
  { 0xCABC,  0x0370,  0x0053,  Speed_Undefined,  L"Cambricon MLU370-S4"                                   },  //<duanjun 202010125 add CAMBRICON MLU370-S4 >
  { 0xCABC,  0x0370,  0x0054,  Speed_Undefined,  L"Cambricon MLU370-X8"                                   },  //Mizl-20211027- Add For Cambricon MLU370-X8+>
  { 0xCABC,  0x0370,  0x0057,  Speed_Undefined,  L"Cambricon MLU370-X4"                                   },  //Mizl-20211027- Add For Cambricon MLU370-X4+>

  
//Please add new device above this line. 
/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
//Please do not add new device Below this line. 


  //LSI Corporation Named by controller
  { 0x1000,  0x0014,  0xFFFF,  Speed_Undefined,  L"LSI Tri-Mode SAS3516"                                  },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x0016,  0xFFFF,  Speed_Undefined,  L"LSI Tri-Mode SAS3508"                                  },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x0017,  0xFFFF,  Speed_Undefined,  L"LSI Tri-Mode SAS3408"                                  },  //<LiuYB001-20190712 Add LSI Devices>
  { 0x1000,  0x005B,  0xFFFF,  Speed_Undefined,  L"LSI Logic RAID2208"                                    },
  { 0x1000,  0x005D,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3108"                                     },
  { 0x1000,  0x0079,  0xFFFF,  Speed_Undefined,  L"LSI Logic RAID2108"                                    },
  { 0x1000,  0x0086,  0xFFFF,  Speed_Undefined,  L"LSI Logic RAID2308"                                    },
  { 0x1000,  0x0087,  0xFFFF,  Speed_Undefined,  L"LSI Logic RAID2308"                                    },
  { 0x1000,  0x0097,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3008"                                     },
  { 0x1000,  0x00C4,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3216"                                     },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x00C9,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3216"                                     },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x00CE,  0x9A71,  Speed_Undefined,  L"LSI SAS3108 16i 2G SAS RAID"                           },  // 0x1000   24001601   RAID	LSI 9361-16i -E3x8 SE 2GB SAS 12G RAID
  { 0x1000,  0x00CE,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3316"                                     },
  { 0x1000,  0x005D,  0x9364,  Speed_Undefined,  L"LSI 9362 8i 2G SAS RAID"                               },
  { 0x1000,  0x005F,  0xFFFF,  Speed_Undefined,  L"LSI Logic SAS3008"                                     },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x00AC,  0xFFFF,  Speed_Undefined,  L"LSI Tri-Mode SAS3416"                                  },  //<LiuYB001-20190215 Add LSI Devices>
  { 0x1000,  0x00AF,  0xFFFF,  Speed_Undefined,  L"LSI Tri-Mode SAS3408"                                  },  //<LiuYB001-20190215 Add LSI Devices>

  //AMD
  { 0x1002,  0x1CB3,  0xFFFF,  Speed_Undefined,  L"AMD Radeon R5 230"                                     },
  { 0x1002,  0x6613,  0x3000,  Speed_Undefined,  L"AMD Radeon R7 340"                                     },  //<hanzhr001-20200311-Add GPU for W760, not in CPL >
  { 0x1002,  0x66A1,  0xFFFF,  Speed_Undefined,  L"AMD RADEON INSTINCT MI60"                              },  // 0x1002   AMD GPU Card
  { 0x1002,  0x6779,  0xFFFF,  Speed_Undefined,  L"AMD Radeon R5 230"                                     },

  //Qlogic
  { 0x1077,  0x2031,  0x0241,  Speed_16Gbps,     L"Qlogic QLE 2670 16G 1-Port HBA"                        },  // 0x1077   24001032   HBA	Qlogic QLE2670 16Gb PCI-E 
  { 0x1077,  0x2031,  0x0249,  Speed_16Gbps,     L"Qlogic QLE 2672 16G 2-Ports HBA"                       },  // 0x1077   24001028   HBA	Qlogic QLE2672 16Gb PCI-E 
  { 0x1077,  0x2532,  0x015C,  Speed_8Gbps,      L"Qlogic QLE 2560 8G 1-Port HBA"                         },  // 0x1077   24000846   HBA	QLOGIC QLE 2560-ACE-BK 8Gb PCI-E 
  { 0x1077,  0x2532,  0x015D,  Speed_8Gbps,      L"Qlogic QLE 2562 8G 2-Ports HBA"                        },  // 0x1077   24000847   HBA	QLOGIC QLE 2562-ACE-BK 8Gb PCI-E 

  //GIGABYTE
  { 0x10DE,  0x0A65,  0xFFFF,  Speed_Undefined,  L"GIGABYTE GV-N210SL-1GI"                                },

  //NVIDIA
  { 0x10DE,  0x013B,  0xFFFF,  Speed_Undefined,  L"NVIDIA QUADRO K620"                                    },
  { 0x10DE,  0x0E0A,  0xFFFF,  Speed_Undefined,  L"NVIDIA GK104 Audio"                                    },
  { 0x10DE,  0x0E1B,  0xFFFF,  Speed_Undefined,  L"NVIDIA GK107 Audio"                                    },
  { 0x10DE,  0x0FF3,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro K420"                                    },  //<ChengXW001-20171110 Add NVIDIA QUADRO K420 info>
  { 0x10DE,  0x0FFE,  0xFFFF,  Speed_Undefined,  L"NVIDIA GK107GL[Quadro K2000]"                          },
  { 0x10DE,  0x1023,  0xFFFF,  Speed_Undefined,  L"NVIDIA K40[3D Controller]"                             },
  { 0x10DE,  0x102D,  0xFFFF,  Speed_Undefined,  L"NVIDIA K80[3D Controller]"                             },
  { 0x10DE,  0x11BA,  0xFFFF,  Speed_Undefined,  L"NVIDIA GK104[Quadro K5000]"                            },
  { 0x10DE,  0x128B,  0xFFFF,  Speed_Undefined,  L"NVIDIA GeForce GT 710"                                 },
  { 0x10DE,  0x13BD,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla M10"                                      },  // 0x10DE   24001343   GPU	NV TESLA M10 R2L 32G -E3x16 225W
  { 0x10DE,  0x13F2,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla M60"                                      },  // 0x10DE   24001246   GPU	NV Tesla M60 16GB -E3x16 300W
  { 0x10DE,  0x15F0,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro GP100"                                   },  // 0x10DE   24001513   Graphic_Card	NV Quadro GP100 16GB E3x16 235W 
  { 0x10DE,  0x15F7,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla P100 12GB"                                },  // 0x10DE   24001344   GPU	NV TESLA P100  12GB -E3x16 250W
  { 0x10DE,  0x15F8,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla P100 16GB"                                },  // 0x10DE   24001345   GPU	NV TESLA P100  16GB -E3x16 250W
  { 0x10DE,  0x15F9,  0x116B,  Speed_Undefined,  L"NVIDIA Tesla P100 SXM2 16GB"                           },  //<Yinnn-20190411-add Tesla P100 16GB for X795 >
  { 0x10DE,  0x15F9,  0xFFFF,  Speed_Undefined,  L"NVIDIA GP100GL"                                        },
  { 0x10DE,  0x1B38,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla P40"                                      },  // 0x10DE   24001366   GPU	NV TESLA P40 24GB -E3x16 250W
  { 0x10DE,  0x1BB0,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P5000"                                   },  // 0x10DE   24001437   Graphic_Card	NV Quadro p5000 16GB E3x16 180W
  { 0x10DE,  0x1BB1,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P4000"                                   },  // 0x10DE   24001508   Graphic_Card	NV Quadro P4000 8GB E3x16 105W
  { 0x10DE,  0x1BB3,  0x1BB3,  Speed_Undefined,  L"NVIDIA TESLA P4"                                       },  // 0x10DE   24001421   GPU	NV TESLA P4  8GB -E3x16 75W 
  { 0x10DE,  0x1BB3,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla P4"                                       },  // 0x10DE   24001421   GPU	NV TESLA P4  8GB -E3x16 75W
  { 0x10DE,  0x1C30,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P2000"                                   },  // 0x10DE   24001510   Graphic_Card	NV Quadro P2000 5GB E3x16 75W
  { 0x10DE,  0x1CB2,  0x11BD,  Speed_Undefined,  L"NVIDIA Quadro P600"                                    },
  { 0x10DE,  0x1CB3,  0x1384,  Speed_Undefined,  L"NVIDIA Quadro P400"                                    },  // 0x10DE   24001511   Graphic_Card	NV Quadro P400 2GB E3x16 30W
  { 0x10DE,  0x1CB2,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P600"                                    },  // 0x10DE   24001512   Graphic_Card	NV Quadro p600 2GB E3x16 40W 
  { 0x10DE,  0x1CB1,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro P1000"                                   },  // 0x10DE   24001514   Graphic_Card	NV Quadro P1000 4GB E3x16 47W 
  { 0x10DE,  0x1DB1,  0x1212,  Speed_Undefined,  L"NVIDIA Tesla V100 SXM2 16GB"                           },  //<Yinnn-20190411-add Tesla V100 sxm2 16GB for X795.
  { 0x10DE,  0x1DB4,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla V100 16GB"                                },  //<Sun002-20180529-add Tesla V100 16GB>
  { 0x10DE,  0x1DB5,  0x1249,  Speed_Undefined,  L"NVIDIA Tesla V100 SXM2 32GB"                           },  //<Yinnn-20181020-add NVIDIA Tesla V100 SXM2 32G support>
  { 0x10DE,  0x1DB6,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla V100 32GB"                                },  //<Sun002-20180529-add Tesla V100 32GB>
  { 0x10DE,  0x1DBA,  0xFFFF,  Speed_Undefined,  L"NVIDIA Quadro GV100"                                   },
  { 0x10DE,  0x1E04,  0x2777,  Speed_Undefined,  L"NVIDIA RTX 2080 Ti"                                    },  //<Sunhp002-20181211-add GPU for W760, not in CPL>
  { 0x10DE,  0x1E04,  0x3712,  Speed_Undefined,  L"NVIDIA RTX 2080 Ti"                                    },
//{ 0x10DE,  0x1E07,  0x12FA,  Speed_Undefined,  L"NVIDIA GeForce RTX 2080 Ti"                            },  //<Kangmm-20201119 Remove repeat information>
//{ 0x10DE,  0x1E07,  0x1E07,  Speed_Undefined,  L"NVIDIA GeForce RTX 2080 Ti"                            },  //<Kangmm-20201119 Remove repeat information>
  { 0x10DE,  0x1E07,  0x2777,  Speed_Undefined,  L"NVIDIA RTX 2080 Ti"                                    },
  { 0x10DE,  0x1E82,  0x2776,  Speed_Undefined,  L"NVIDIA GeForce RTX 2080"                               },
  { 0x10DE,  0x1E87,  0x2776,  Speed_Undefined,  L"NVIDIA GeForce RTX 2080"                               },
  { 0x10DE,  0x1EB8,  0xFFFF,  Speed_Undefined,  L"NVIDIA Tesla T4"                                       },  //<Yinnn-20181204-Add NVIDIA T4 support>
  { 0x10DE,  0x1F82,  0x1320,  Speed_Undefined,  L"NVIDIA GTX1650 4G D5"                                  },  //<yanshp1-20201119-Add NVIDIA GTX1650 support>

  //Emulex
  { 0x10DF,  0xE200,  0xE280,  Speed_16Gbps,     L"Emulex LPe16002B-M6 2-port 16G FC HBA"                 },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xE200,  0xE281,  Speed_16Gbps,     L"Emulex LPe16000B-M6 1-port 16G FC HBA"                 },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xE200,  0xF281,  Speed_16Gbps,     L"Emulex 16000B-M6"                                      },
  { 0x10DF,  0xE300,  0xE300,  Speed_32Gbps,     L"Emulex LPe32002-AP 2-port 32G FC HBA"                  },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xE300,  0xE301,  Speed_32Gbps,     L"Emulex LPe32000-AP 1-port 32G FC HBA"                  },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xE300,  0xE310,  Speed_16Gbps,     L"Emulex LPe31002-AP 2-port 16G FC HBA"                  },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xE300,  0xE311,  Speed_16Gbps,     L"Emulex LPe31000-AP 1-port 16G FC HBA"                  },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x10DF,  0xF015,  0xF015,  Speed_8Gbps,      L"Emulex LPe1250-AP 1-port 8G FC HBA"                    },  //<Lvshh001-20211008 Update CPL V9.7> 
  { 0x10DF,  0xF100,  0xF100,  Speed_8Gbps,      L"Emulex LPe12000-AP 1-port 8G FC HBA"                   },  //<Lvshh001-20211008 Update CPL V9.7>
  { 0x19A2,  0x0710,  0xFFFF,  Speed_10Gbps,     L"Emulex OneConnect 10Gb NIC(BE3)"                       },
  
  //REALTEK
  { 0x10EC,  0x8136,  0xFFFF,  Speed_1Gbps,      L"Realtek NIC"                                           },

  //MTK
//{ 0x10EC,  0x8168,  0x8168,  Speed_Undefined,  L"MTK"                                                   },  //<Songts001_20190810 Add MTK support >
  { 0x10ec,  0x8168,  0xFFFF,  Speed_Undefined,  L"MTK"                                                   },  //<Songts001_20200103 Modify the asset information for MTK >

  //Alibaba
  { 0x1172,  0xE003,  0x0101,  Speed_Undefined,  L"TASS SJK19150 Crypto Engine"                           },
  { 0x1DED,  0x3010,  0x3010,  Speed_Undefined,  L"Alibaba Flash"                                         },
  { 0x1DED,  0x7010,  0x0007,  Speed_Undefined,  L"AliHEC-X15DP-16G FPGA"                                 },
  { 0x1DED,  0x7016,  0x0007,  Speed_Undefined,  L"AliHEC-X15DP-16G FPGA"                                 },
  { 0xBABA,  0x0000,  0x0000,  Speed_Undefined,  L"Alibaba FPGA"                                          },
  { 0xBABA,  0x0500,  0x0001,  Speed_Undefined,  L"Alibaba Corporation Ethernet controller"               },

  //SAMSUNG
//{ 0x144D,  0xA802,  0xA801,  Speed_Undefined,  L"SAMSUNG MZ1LV480HCHP"                                  },
//{ 0x144D,  0xA802,  0xA801,  Speed_Undefined,  L"SAMSUNG MZVPV128HDGM"                                  },
//{ 0x144D,  0xA802,  0xFFFF,  Speed_Undefined,  L"SAMSUNG MZVPV512HDGL"                                  },
  { 0x144D,  0xA804,  0xA801,  Speed_Undefined,  L"Samsung PM963 U.2"                                     },
//{ 0x144D,  0xA804,  0xA801,  Speed_Undefined,  L"Samsung MZ1LW960HMJP"                                  },  //<hety-20220401 Remove repeat infomation >
  { 0x144D,  0xA808,  0xA801,  Speed_Undefined,  L"Samsung PM983 U.2"                                     },
  { 0x144D,  0xA821,  0xA801,  Speed_Undefined,  L"Samsung MZWLK800HCHP"                                  },
  { 0x144D,  0xA821,  0xA802,  Speed_Undefined,  L"Samsung MZWLK1T6HCHP"                                  },
  { 0x144D,  0xA822,  0xA804,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA807,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA809,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA811,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA812,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA814,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA816,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA822,  0xA818,  Speed_Undefined,  L"Samsung PM1725B AIC"                                   },
  { 0x144D,  0xA824,  0xA812,  Speed_Undefined,  L"Samsung PM1735 AIC"                                    },
  { 0x144D,  0xA824,  0xA813,  Speed_Undefined,  L"Samsung PM1735 AIC"                                    },
  { 0x144D,  0xA824,  0xA814,  Speed_Undefined,  L"Samsung PM1735 AIC"                                    },
  { 0x144D,  0xA824,  0xA815,  Speed_Undefined,  L"Samsung PM1735 AIC"                                    },
  { 0x144D,  0xA825,  0xA809,  Speed_Undefined,  L"Samsung PM1733A Gen4 U.2"                              },
  
  //LITE-ON //Liteon
  { 0x14A4,  0x0224,  0xFFFF,  Speed_Undefined,  L"LITE-ON EP1 GB480"                                     },
//{ 0x14A4,  0x0224,  0xFFFF,  Speed_Undefined,  L"Liteon EP1 GB480"                                      },  //<hety-20220401 Remove repeat infomation >
  { 0x14A4,  0x22A0,  0xFFFF,  Speed_Undefined,  L"LITE-ON EP2 KB960"                                     },
//{ 0x14A4,  0x22A0,  0xFFFF,  Speed_Undefined,  L"Liteon EP2 KB960"                                      },  //<hety-20220401 Remove repeat infomation >
  { 0x14A4,  0x61A0,  0xFFFF,  Speed_Undefined,  L"LITE-ON ED1 2W3840"                                    },
//{ 0x14A4,  0x61A0,  0xFFFF,  Speed_Undefined,  L"Liteon ED1 2W3840"                                     },  //<hety-20220401 Remove repeat infomation >

  //Broadcom
  { 0x14E4,  0x1614,  0x4540,  Speed_100Gbps,    L"Broadcom 57454 100G QSFP28 1-Port PCIe NIC"            },
  { 0x14E4,  0x1614,  0x4541,  Speed_100Gbps,    L"Broadcom 57454 100G QSFP28 1-Port OCP NIC"             },
  { 0x14E4,  0x165F,  0x2003,  Speed_1Gbps,      L"Broadcom 5720 1G RJ45 2-Ports NIC"                     },
  { 0x14E4,  0x16A1,  0x0002,  Speed_10Gbps,     L"Broadcom 57840 10Gb 4-port NIC"                        },  //<Songts045_20190519 Add Broadcom FC card Info >
  { 0x14E4,  0x16A1,  0x0001,  Speed_20Gbps,     L"Broadcom 57840 20Gb 2-port CNA NIC"                    },  //<Songts045_20190519 Add Broadcom FC card Info >
  { 0x14E4,  0x16A4,  0x0001,  Speed_20Gbps,     L"Broadcom 57840 20Gb 2-port CNA NIC"                    },  //<Songts045_20190519 Add Broadcom FC card Info >
  { 0x14E4,  0x16A4,  0x0002,  Speed_10Gbps,     L"Broadcom 57840 10Gb 4-port NIC"                        },  //<Songts045_20190519 Add Broadcom FC card Info >
  { 0x14E4,  0x16C9,  0x0205,  Speed_10Gbps,     L"Broadcom 57302 10G SFP+ 2-Ports PCIe NIC"              },
  { 0x14E4,  0x16C9,  0xFFFF,  Speed_Undefined,  L"Broadcom NIC"                                          },
  { 0x14E4,  0x16D6,  0x1202,  Speed_25Gbps,     L"Broadcom 57412 25G SFP28 1-Port OCP NIC"               },
  { 0x14E4,  0x16D6,  0x4123,  Speed_10Gbps,     L"Broadcom 57412 10G SFP+ 2-Ports OCP NIC"               },
  { 0x14E4,  0x16D6,  0x4124,  Speed_10Gbps,     L"Broadcom 57412 10G SFP+ 2-Ports PCIe NIC"              },
  { 0x14E4,  0x16D7,  0x1404,  Speed_25Gbps,     L"BCM BCM957414M4142C 2-port 25G OCP SFP28 NIC"          },	//<Lvshh002-20220215 Update CPL V10.1 >	
  { 0x14E4,  0x16D7,  0x1406,  Speed_25Gbps,     L"Broadcom 57414 25G SFP28 2-Ports PCIe NIC"             },
  { 0x14E4,  0x16D7,  0x1408,  Speed_25Gbps,     L"Broadcom 57414 25G SFP28 2-Ports OCP NIC"              },  
  { 0x14E4,  0x16D7,  0x4143,  Speed_50Gbps,     L"Broadcom 57414 50G SFP56 1-Port OCP NIC"               },
  { 0x14E4,  0x16D8,  0x1602,  Speed_10Gbps,     L"Broadcom 57416 10G SFP+ 2-Ports PCIe NIC"              },
  { 0x14E4,  0x16D8,  0x4163,  Speed_10Gbps,     L"Broadcom 57416 10G SFP+ 2-Ports OCP NIC"               },
  { 0x14E4,  0x16D8,  0xFFFF,  Speed_10Gbps,     L"Broadcom Dual-port 10G Ethernet Adapter"               },  //<Chendl001-20180704 Fixed Broadcom Network card hang 92 issue +>
  { 0x14E4,  0x16F1,  0x4520,  Speed_50Gbps,     L"Broadcom 57452 50G SFP56 1-Port OCP NIC"               },

  //Mellanox
  { 0x15B3,  0x0209,  0xFFFF,  Speed_100Gbps,    L"Mellanox MT27704A0 [ConnectX-4]"                       },
  { 0x15B3,  0x1003,  0x0017,  Speed_40Gbps,     L"Mellanox MCX353A 40G 1-Port QDR HCA"                   },  // 0x15B3   24000753   HCA	Mellanox MCX353A 40G 1-Port QDR HCA
  { 0x15B3,  0x1003,  0x0028,  Speed_40Gbps,     L"Mellanox MCX354A 40G 2-Ports QDR HCA"                  },  // 0x15B3   24000754   HCA	Mellanox MCX354A 40G 2-Ports QDR HCA
  { 0x15B3,  0x1003,  0x0049,  Speed_10Gbps,     L"Mellanox MCX312A 10G SFP+ 2-Ports NIC"                 },
  { 0x15B3,  0x1003,  0x0050,  Speed_56Gbps,     L"Mellanox MCX354A 56G 2-Ports FDR HCA"                  },  // 0x15B3   24000684   HCA	Mellanox MCX354A 56G 2-Ports FDR HCA
  { 0x15B3,  0x1003,  0x0051,  Speed_56Gbps,     L"Mellanox MCX353A 56G 1-Port FDR HCA"                   },  // 0x15B3   24000683   HCA	Mellanox MCX353A 56G 1-Port FDR HCA
  { 0x15B3,  0x1003,  0x0055,  Speed_10Gbps,     L"Mellanox MCX311A 10G SFP+ 1-Port NIC"                  },
  { 0x15B3,  0x1003,  0xFFFF,  Speed_Undefined,  L"Mellanox MT27500 [ConnectX-3]"                         },
  { 0x15B3,  0x1013,  0x0003,  Speed_40Gbps,     L"Mellanox MCX413A 40G QSFP 1-Port PCIe NIC"             },
  { 0x15B3,  0x1013,  0x0004,  Speed_40Gbps,     L"Mellanox MCX414A 40G QSFP 2-Ports PCIe NIC"            },
  { 0x15B3,  0x1013,  0x0005,  Speed_40Gbps,     L"Mellanox MCX415A 40G QSFP 1-Port PCIe NIC"             },
  { 0x15B3,  0x1013,  0x0006,  Speed_40Gbps,     L"Mellanox MCX416A 40G QSFP 2-Ports PCIe NIC"            },
  { 0x15B3,  0x1013,  0x0007,  Speed_100Gbps,    L"Mellanox MCX415A 100G QSFP28 1-Port PCIe NIC"          },
  { 0x15B3,  0x1013,  0x0008,  Speed_100Gbps,    L"Mellanox MCX416A 100G QSFP28 2-Ports PCIe NIC"         },
  { 0x15B3,  0x1013,  0x0009,  Speed_56Gbps,     L"MLX MCX453A-FCAT 1-port 56G FDR QSFP+ HCA"             },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x1013,  0x0010,  Speed_56Gbps,     L"MLX MCX454A-FCAT 2-port 56G FDR QSFP+ HCA"             },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x1013,  0x0013,  Speed_100Gbps,    L"MLX MCX455A-ECAT 1-port 100G EDR QSFP28 HCA"           },  // 0x15B3   24001119   HCA//<Lvshh002-20220215 Update CPL V10.1 >	
  { 0x15B3,  0x1013,  0x0014,  Speed_100Gbps,    L"MLX MCX456A-ECAT 2-port 100G EDR QSFP28 HCA"           },  // 0x15B3   24001128   HCA//<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x1013,  0x0100,  Speed_100Gbps,    L"Mellanox MCX455A 100G 1-Port EDR HCA"                  },
  { 0x15B3,  0x1013,  0x010B,  Speed_100Gbps,    L"Mellanox MT27704A0-FDCF-EVM"                           },  //<Nieht20170629-add a new IB card+>
  { 0x15B3,  0x1013,  0x010D,  Speed_100Gbps,    L"Mellanox MT27704A0-FDCR-EV"                            },  //<Nieht20170629-add a new IB card+>
  { 0x15B3,  0x1015,  0x0002,  Speed_10Gbps,     L"Mellanox MCX4111A-XCAT 10G SFP+ 1-Port NIC"            },
  { 0x15B3,  0x1015,  0x0005,  Speed_40Gbps,     L"Mellanox MCX4131A 40G SFP+ 1-Port NIC"                 },
  { 0x15B3,  0x1015,  0x0011,  Speed_50Gbps,     L"Mellanox MCX4431A 50G QSFP 1-Port OCP NIC"             },
  { 0x15B3,  0x1015,  0x0020,  Speed_25Gbps,     L"Mellanox MCX4411A 25G SFP28 1-Port OCP NIC"            },  //<YinQiang001-20181106 add a new MLX card>
//{ 0x15B3,  0x1015,  0x0091,  Speed_10Gbps,     L"MLX MCX4421A-XCQN 2-port 10G OCP-CAT1 SFP+ NIC"        },  //<Kangmm-20201119 Remove repeat information>
  { 0x15B3,  0x1017,  0x0001,  Speed_100Gbps,    L"MLX MCX556A-ECAT 2-port 100G EDR QSFP28 HCA"           },  // 0x15B3   24001453   HCA//<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x1017,  0x0003,  Speed_100Gbps,    L"MLX MCX555A-ECAT 1-port 100G EDR QSFP28 HCA"           },  // 0x15B3   24001417   HCA//<Lvshh002-20220215 Update CPL V10.1 >	
  { 0x15B3,  0x1017,  0x0004,  Speed_50Gbps,     L"Mellanox MCX515A 50G SFP56 1-Port PCIe NIC"            },
  { 0x15B3,  0x1017,  0x0021,  Speed_50Gbps,     L"Mellanox MCX516A 50G SFP56 2-Ports PCIe NIC"           },
  { 0x15B3,  0x1017,  0x0035,  Speed_40Gbps,     L"Mellanox MCX516A 40G QSFP 2-Ports PCIe NIC"            },
  { 0x15B3,  0x1017,  0x0050,  Speed_25Gbps,     L"Mellanox MCX542A 25G SFP28 2-Ports OCP NIC"            },  //<Yinnn-20181205-Add MLX CX5 OCP support >
  { 0x15B3,  0x1017,  0x0059,  Speed_25Gbps,     L"Mellanox MCX542A-ACAN_C06 OCP NIC"                     },  //<Yinnn-20181205-Add MLX CX5 OCP support >
  { 0x15B3,  0x1019,  0x0008,  Speed_100Gbps,    L"Mellanox MCX516A-CDAT 100G 2-Ports QSFP28"             },  //<hujf1001-20200925 Update Device ID for MLX MCX516A-CDAT >
  { 0x15B3,  0x101B,  0x0001,  Speed_200Gbps,    L"Mellanox CX6 200G HDR Multihost QSFP56 HCA"            },  //<Wanxg-20190302-Add MLX CX6 HDR Multihost support >
  { 0x15B3,  0x101B,  0x0002,  Speed_200Gbps,    L"Mellanox CX6 200G HDR Singlehost QSFP56 HCA"           },  //<Kangmm001-20200908 Add Mellanox CX6 200G HDR Singlehost card >
  { 0x15B3,  0x101B,  0x0006,  Speed_100Gbps,    L"MLX MCX653105A-ECAT 1-port 100G HDR100 QSFP56 HCA"     },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x101B,  0x0009,  Speed_200Gbps,    L"Mellanox MCX653106A-HDAT200G 2-Ports HDR IB Card"      },  //<hujf1001-20200925 Update Device ID for MLX MCX653106A-HDAT >
  { 0x15B3,  0x101B,  0x0010,  Speed_200Gbps,    L"MLX MCX654105A-HCAT 1-port 200G HDR QSFP56 HCA"        },  //<Lvshh002-20220215 Update CPL V10.1 >
  { 0x15B3,  0x101B,  0x1234,  Speed_200Gbps,    L"Mellanox CX6 200G HDR Multihost QSFP56 HCA"            },  //<Zoujie-20201203 Change SVID for FW upgarde>
  { 0x15B3,  0x101B,  0x2345,  Speed_200Gbps,    L"Mellanox CX6 200G HDR Singlehost QSFP56 HCA"           },  //<Zoujie-20201203 Change SVID for FW upgarde>
  { 0x15B3,  0x101D,  0x0059,  Speed_200Gbps,    L"MLX MCX623435AN-VDAB 200G O3SFF 1-Ports OCP NIC"       },  //<xuyj1001-20201012 Add New Mellanox card >
  { 0x15B3,  0x4117,  0x0002,  Speed_10Gbps,     L"Mellanox MCX4111A 10Gb SFP+ 1-Port NIC"                },
  { 0x15B3,  0x4117,  0x0091,  Speed_10Gbps,     L"Mellanox MCX4421A 10Gb SFP+ 2-Ports OCP NIC"           },
  { 0x15B3,  0x4117,  0xFFFF,  Speed_25Gbps,     L"Mellanox MCX4-Lx EN 25Gb SFP+ NIC"                     },  // 0x0000   24001470   NIC	Mellanox MCX4-Lx EN 25Gb SFP+ NIC
  { 0x15B3,  0x4121,  0x0008,  Speed_100Gbps,    L"MLX MCX516A-CDAT 100G QSFP28 2-ports NIC"              },  //<xuyj1001-20201012 Add New Mellanox card >
//{ 0x15B3,  0x4125,  0x0006,  Speed_100Gbps,    L"MLX MCX623106AN-CDAT 100G QSFP56 2-Ports NIC"          },  //<hety-20220401 Remove repeat infomation >
  { 0x15B3,  0x4125,  0x0016,  Speed_100Gbps,    L"MLX MCX623436AN-CDAB 100G O3SFF QSFP56 2-Ports NIC"    },
  { 0x15B3,  0x4125,  0x0018,  Speed_200Gbps,    L"MLX MCX623105AN-VDAT 200G QSFP56 1-Ports NIC"          },
  { 0x15B3,  0x4125,  0x0059,  Speed_200Gbps,    L"MLX MCX623435AN-VDAB 200G O3SFF 1-Ports NIC"           },
  { 0x15B3,  0xA2D2,  0x0001,  Speed_25Gbps,     L"Mellanox MBF1M332A-AENAT 25G SFP28 2-Ports PCIe SmartNIC"},  //<Wanxg-20190627-Add MLX BF1 PCIE SmartNIC support > 

  //Huawei
  { 0x19E5,  0x0202,  0xD304,  Speed_32Gbps,     L"Huawei IN300 32G SP526 2-Ports HBA"                    },
  { 0x19E5,  0x0203,  0xD305,  Speed_32Gbps,     L"Huawei IN300 16G SP525 2-Ports HBA"                    },
  { 0x19E5,  0x0206,  0xD138,  Speed_25Gbps,     L"Huawei IN200 25G SFP28 2-ports PCIe NIC"               },  //<Wanxg-20190627 Add Huawei PCIE NIC info >
  { 0x19E5,  0x0212,  0xD306,  Speed_32Gbps,     L"Huawei IN300 8G  SP523 2-Ports HBA"                    },
  { 0x19E5,  0x1822,  0xD136,  Speed_25Gbps,     L"Huawei IN200 25G SFP28 4-ports PCIe NIC"               },  //<Wanxg-20190627 Add Huawei PCIE NIC info >
  { 0x19E5,  0x3714,  0x5311,  Speed_Undefined,  L"Huawei ES3500P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5312,  Speed_Undefined,  L"Huawei ES3500P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5313,  Speed_Undefined,  L"Huawei ES3500P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5314,  Speed_Undefined,  L"Huawei ES3500P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5121,  Speed_Undefined,  L"Huawei ES3610P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5122,  Speed_Undefined,  L"Huawei ES3610P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5123,  Speed_Undefined,  L"Huawei ES3610P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5124,  Speed_Undefined,  L"Huawei ES3610P V5 U.2"                                 },
  { 0x19E5,  0x3714,  0x5131,  Speed_Undefined,  L"Huawei ES3610C V5 AIC"                                 },
  { 0x19E5,  0x3714,  0x5132,  Speed_Undefined,  L"Huawei ES3610C V5 AIC"                                 },
  { 0x19E5,  0x3714,  0x5133,  Speed_Undefined,  L"Huawei ES3610C V5 AIC"                                 },
  { 0x19E5,  0x3714,  0x5134,  Speed_Undefined,  L"Huawei ES3610C V5 AIC"                                 },
  
  //Cambricon
  { 0x19E5,  0x3536,  0xFFFF,  Speed_Undefined,  L"Cambricon MLU100-C3-VI"                                },  // Cambricon MLU100-C3-VI
  { 0x4943,  0x4E43,  0x4E43,  Speed_Undefined,  L"NetFirm-C502F"                                         },
  { 0xCABC,  0x0100,  0x0014,  Speed_Undefined,  L"Cambricon MLU100-V4"                                   },  // 0xCABC   GPU   Cambricon MLU100-V4
  { 0xCABC,  0x0100,  0x00D3,  Speed_Undefined,  L"Cambricon MLU100-D3"                                   },  // 0xCABC   GPU   Cambricon MLU100-D3
  { 0xCABC,  0x0100,  0x00D4,  Speed_Undefined,  L"Cambricon MLU100-D4"                                   },  // 0xCABC   GPU   Cambricon MLU100-D4
  { 0xCABC,  0x0270,  0x0014,  Speed_Undefined,  L"Cambricon MLU270"                                      },  // 0xCABC   GPU   Cambricon MLU270

  //Aspeed
  { 0x1A03,  0x2000,  0xFFFF,  Speed_Undefined,  L"Aspeed AST2500 VGA"                                    },
        
  //Tilera
  { 0x1A41,  0x5E43,  0xFFFF,  Speed_Undefined,  L"Tilera VideoSpeed-C200"                                },
  
  //Seagate
  { 0x1BB1,  0x0100,  0x0101,  Speed_Undefined,  L"Seagate ST1600HM0011"                                  },
  { 0x1BB1,  0x0100,  0xFFFF,  Speed_Undefined,  L"Seagate ST800KN0021"                                   },
  
  //Western Digital
  { 0x1C58,  0x0023,  0x0023,  Speed_Undefined,  L"WD Omaha Series"                                       },

  //Memblaze
  { 0x1C5F,  0x000D,  0x0202,  Speed_Undefined,  L"Memblaze PB5 520 AIC"                                  },
  { 0x1C5F,  0x000D,  0x0204,  Speed_Undefined,  L"Memblaze PB5 520 AIC"                                  },
  { 0x1C5F,  0x003D,  0x0A04,  Speed_Undefined,  L"Memblaze PB5 920 AIC"                                  },
  { 0x1C5F,  0x003D,  0x0A08,  Speed_Undefined,  L"Memblaze PB5 926 AIC"                                  },
  { 0x1C5F,  0x003D,  0x0A31,  Speed_Undefined,  L"Memblaze PB5 920 U.2"                                  },  //lvshh001-20200803 Update PB920/926 U.2/AIC info as specifications
  { 0x1C5F,  0x003D,  0x0A40,  Speed_Undefined,  L"Memblaze PB5 920 AIC"                                  },  //lvshh001-20200803 Update PB920/926 U.2/AIC info as specifications
  { 0x1C5F,  0x003D,  0x0A41,  Speed_Undefined,  L"Memblaze PB5 920 U.2"                                  },  //lvshh001-20200803 Update PB920/926 U.2/AIC info as specifications
  { 0x1C5F,  0x003D,  0x4A30,  Speed_Undefined,  L"Memblaze PB5 926 AIC"                                  },  //lvshh001-20200803 Update PB920/926 U.2/AIC info as specifications
  { 0x1C5F,  0x003D,  0x4A31,  Speed_Undefined,  L"Memblaze PB5 926 U.2"                                  },  //lvshh001-20200803 Update PB920/926 U.2/AIC info as specifications
//{ 0x1C5F,  0x003D,  0x4A40,  Speed_Undefined,  L"Memblaze PB5 926 AIC"                                  },  //<Kangmm-20201119 Remove repeat information>
  { 0x1C5F,  0x010D,  0x0202,  Speed_Undefined,  L"Memblaze PB5 520 U.2"                                  },
  { 0x1C5F,  0x010D,  0x0204,  Speed_Undefined,  L"Memblaze PB5 520 U.2"                                  },
  { 0x1C5F,  0x013D,  0x0A04,  Speed_Undefined,  L"Memblaze PB5 920 U.2"                                  },
  { 0x1C5F,  0x013D,  0x0A08,  Speed_Undefined,  L"Memblaze PB5 926 U.2"                                  },
  { 0x1C5F,  0x0540,  0x0547,  Speed_Undefined,  L"Memblaze SSD PBlaze4 HHHL M SSD Series"                },
  { 0x1C5F,  0x0550,  0xFFFF,  Speed_Undefined,  L"Memblaze SSD PBlaze5 Series"                           },        
  { 0x1C5F,  0x0555,  0x0551,  Speed_Undefined,  L"Memblaze PB5 516 AIC"                                  },
  { 0x1C5F,  0x0555,  0x0552,  Speed_Undefined,  L"Memblaze PB5 516 U.2"                                  },
  { 0x1C5F,  0x0555,  0x055A,  Speed_Undefined,  L"Memblaze PB5 510 AIC"                                  },
  { 0x1C5F,  0x0555,  0x055B,  Speed_Undefined,  L"Memblaze PB5 510 U.2"                                  },
  { 0x1C5F,  0x0557,  0x0551,  Speed_Undefined,  L"Memblaze PB5 916 AIC"                                  },
  { 0x1C5F,  0x0557,  0x0552,  Speed_Undefined,  L"Memblaze PB5 916 U.2"                                  },
  { 0x1C5F,  0x0557,  0x055A,  Speed_Undefined,  L"Memblaze PB5 910 AIC"                                  },
//{ 0x1C5F,  0x0557,  0x055B,  Speed_Undefined,  L"Memblaze PB5 910 U.2"                                  },  //<hety-20220401 Remove repeat infomation >
  
  //Shannon Systems
  { 0x1cb0,  0x05a5,  0xFFFF,  Speed_Undefined,  L"Shannon G5i U.2"                                       },
  { 0x1cb0,  0xd000,  0xFFFF,  Speed_Undefined,  L"Shannon Venice-E U.2"                                  },
  
  //Hygon DCU
  { 0x1d94,  0x51b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100S"                                       },
  { 0x1d94,  0x54b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100"                                        },
  { 0x1d94,  0x55b7,  0xFFFF,  Speed_Undefined,  L"Hygon DCU Z100L"                                       },

  //Habana
  { 0x1DA3,  0x0001,  0x0001,  Speed_Undefined,  L"Habana Goya HL-100"                                    },  //<Songts001_20190810 Add Habana Goya HL-100 support >

  //Kingston
  { 0x2646,  0x0010,  0xFFFF,  Speed_Undefined,  L"Kingston SHPM2280P2"                                   },

  //Intel Corporation
//{ 0x10EC,  0x8168,  0x0123,  Speed_1Gbps,      L"Sugon 1G SFP+ 1-Ports  NIC"                            },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x0953,  0x3704,  Speed_Undefined,  L"Intel SSD DC P3520 HHHL R SSD Series"                  },
  { 0x8086,  0x0953,  0x3705,  Speed_Undefined,  L"Intel SSD DC P3500 Series"                             },
  { 0x8086,  0x0953,  0x370A,  Speed_Undefined,  L"Intel SSD DC P3600 Series"                             },  //<Sunhp001-20180319-"I620-G30 20180319.xlsx">
  { 0x8086,  0x0953,  0xFFFF,  Speed_Undefined,  L"Intel SSD DC P3600 Series"                             },
  { 0x8086,  0x0A53,  0x4702,  Speed_Undefined,  L"Intel SSD DC P4510 Series"                             },  //<Yinnn-20181204-Add P4510 ssd support,CPL6.9 .+>
  { 0x8086,  0x0A53,  0xFFFF,  Speed_Undefined,  L"Intel SSD DC P3520 Series"                             },
  { 0x8086,  0x0A54,  0x4702,  Speed_Undefined,  L"Intel SSD DC P4500 Series"                             },
  { 0x8086,  0x0A54,  0x4704,  Speed_Undefined,  L"Intel SSD DC P4500 HHHL R SSD Series"                  },
  { 0x8086,  0x0A54,  0x4714,  Speed_Undefined,  L"Intel SSD DC P4610 Series"                             },
  { 0x8086,  0x10C9,  0xFFFF,  Speed_1Gbps,      L"Intel 82576 1Gb RJ45 2-Ports NIC"                      },
  { 0x8086,  0x10D3,  0xFFFF,  Speed_1Gbps,      L"Intel 82574 1Gb RJ45 NIC"                              },
  { 0x8086,  0x10E6,  0x10E6,  Speed_1Gbps,      L"Intel 82576 1Gb SFP NIC"                               },
  { 0x8086,  0x10F8,  0xFFFF,  Speed_10Gbps,     L"Intel 82599 10Gb SFP+ 2-Ports NIC"                     },  //<Songts043_20190506 Add 82599 SFP MEZZ card support. >
//{ 0x8086,  0x10FB,  0x0003,  Speed_10Gbps,     L"Intel 82599 10Gb SFP+ NIC"                             },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x10FB,  0x0006,  Speed_10Gbps,     L"Intel 82599 10Gb SFP+ 1-Port NIC"                      },
  { 0x8086,  0x10FB,  0x000C,  Speed_10Gbps,     L"Intel 82599 10Gb SFP+ NIC"                             },
//{ 0x8086,  0x10FB,  0x0301,  Speed_10Gbps,     L"Intel 82599 10G SFP+ OCP-CAT1 2-Ports NIC"             },  //<hety-20220401 Remove repeat infometion>
//{ 0x8086,  0x10FB,  0x030A,  Speed_10Gbps,     L"BLD 82599 10Gb SFP+ 2-Ports NIC"                       },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x10FB,  0xFFFF,  Speed_10Gbps,     L"Intel 82599 10Gb SFP+ 2-Ports NIC"                     },
  { 0x8086,  0x151F,  0xFFFF,  Speed_1Gbps,      L"Intel I350 1Gb RJ45 NIC"                               },
//{ 0x8086,  0x1521,  0x0300,  Speed_1Gbps,      L"Intel I350 1Gb RJ45 OCP-CAT1 2-Ports NIC"              },  //<Kangmm-20201119 Remove repeat information>
//{ 0x8086,  0x1521,  0x0308,  Speed_1Gbps,      L"BLD I350 1G RJ45 2-Ports PCIe NIC"                     },  //<Kangmm-20201119 Remove repeat information>
//{ 0x8086,  0x1521,  0x0309,  Speed_1Gbps,      L"BLD I350 1G RJ45 4-Ports PCIe NIC"                     },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1521,  0xFFFF,  Speed_1Gbps,      L"Intel I350 1Gb RJ45 NIC"                               },
//{ 0x8086,  0x1522,  0x030F,  Speed_1Gbps,      L"BLD I350 1G SFP 2-Ports PCIe NIC"                      },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1522,  0x0310,  Speed_1Gbps,      L"BLD I350 1G SFP 4-Ports PCIe NIC"                      },
  { 0x8086,  0x1522,  0xFFFF,  Speed_1Gbps,      L"Intel I350 1Gb SFP NIC"                                },
  { 0x8086,  0x1523,  0xFFFF,  Speed_1Gbps,      L"Intel I350 1Gb SFP NIC"                                },
  { 0x8086,  0x1526,  0xFFFF,  Speed_1Gbps,      L"Intel 82576 1Gb RJ45 4-Ports NIC"                      },  //<Sunhp001-20181024-device list version is 20181023>
  { 0x8086,  0x1528,  0x030B,  Speed_10Gbps,     L"BLD X540 10Gb RJ45 2-Ports PCIe NIC"                   },
  { 0x8086,  0x1528,  0xFFFF,  Speed_10Gbps,     L"Intel X540 10Gb RJ45 NIC"                              },
  { 0x8086,  0x1533,  0xFFFF,  Speed_1Gbps,      L"Intel I210 1Gb"                                        },
  { 0x8086,  0x1537,  0xFFFF,  Speed_1Gbps,      L"Intel I210 1000BASE-KX/BX backplane"                   },  //<Xielsh-20181202-Add Intel I210 1000BASE-KX/BX backplane support.>  
  { 0x8086,  0x1563,  0x0001,  Speed_10Gbps,     L"Intel X550 10Gb RJ45 2-Ports NIC"                      },
//{ 0x8086,  0x1563,  0x0302,  Speed_10Gbps,     L"BLD X550 10Gb SFP+ 2-Ports OCP NIC"                    },  //<Kangmm-20201119 Remove repeat information>
//{ 0x8086,  0x1563,  0x0307,  Speed_10Gbps,     L"BLD X550 10Gb RJ45 2-Ports PCIe NIC"                   },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1563,  0x8712,  Speed_10Gbps,     L"ASUS X550 10Gb RJ45 2-Ports OCP NIC"                   },
  { 0x8086,  0x1563,  0xFFFF,  Speed_10Gbps,     L"Intel X550 10Gb RJ45 NIC"                              },
//{ 0x8086,  0x1572,  0x0000,  Speed_10Gbps,     L"Intel XL710 10Gb SFP+ 4-Ports NIC"                     },  //<Nieht-20181119-Update X710 info as PM required>
  { 0x8086,  0x1572,  0x0000,  Speed_10Gbps,     L"Intel XL710 10G SFP+ 4-Ports OCP NIC"                  },  //<Lvshh001-20201023 Modify Manufacturer Information>
  { 0x8086,  0x1572,  0x0001,  Speed_10Gbps,     L"Silicom X710 10G SFP+ 2-Ports NIC"                     },
//{ 0x8086,  0x1572,  0x0001,  Speed_10Gbps,     L"Intel XL710 10Gb SFP+ NIC"                             },  //<hety-20220401 Remove repeat infomation>
//{ 0x8086,  0x1572,  0x0007,  Speed_10Gbps,     L"Intel X710 10G SFP+ 2-Ports NIC"                       },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1572,  0x0008,  Speed_10Gbps,     L"Intel X710 10G SFP+ 2-Ports NIC"                       },
  { 0x8086,  0x1572,  0x0252,  Speed_10Gbps,     L"Silicom XL710 10G SFP+ 4-Ports NIC"                    },
  { 0x8086,  0x1572,  0x0275,  Speed_10Gbps,     L"Silicom X710 10G SFP+ 2-Ports OCP NIC"                 },
  { 0x8086,  0x1572,  0x0301,  Speed_10Gbps,     L"BLD 82599 10G SFP+ 2-Ports OCP NIC"                    },
//{ 0x8086,  0x1572,  0x0304,  Speed_10Gbps,     L"BLD X710 10G SFP+ 2-Ports NIC"                         },  //<Kangmm-20201119 Remove repeat information>
//{ 0x8086,  0x1572,  0x0305,  Speed_10Gbps,     L"BLD X710 10G SFP+ 2-Ports OCP NIC"                     },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1572,  0x0306,  Speed_10Gbps,     L"BLD XL710 10G SFP+ 4-Ports OCP NIC"                    }, 
  { 0x8086,  0x1572,  0x030C,  Speed_10Gbps,     L"BLD XL710 10G SFP+ 4-Ports NIC"                        },
//{ 0x8086,  0x1572,  0x030c,  Speed_10Gbps,     L"Intel XL710 10G SFP+ 4-Ports NIC"                      },  //<hety-20220401 Remove repeat infomation>
  { 0x8086,  0x1572,  0x031D,  Speed_10Gbps,     L"BLD X710 10G SFP+ 2-Ports NIC"                         },  //<zhaoyf1-20200321-add X710 for GPU X785>
//{ 0x8086,  0x1572,  0x031E,  Speed_10Gbps,     L"BLD X710 10G 2-Ports OCP NIC"                          },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1572,  0xFFFF,  Speed_10Gbps,     L"Intel X710 10Gb SFP+ NIC"                              },  //Should not be removed.
  { 0x8086,  0x1583,  0x0000,  Speed_40Gbps,     L"Intel XL710 40G QSFP+ 2-Ports PCIe NIC"                },
  { 0x8086,  0x1583,  0x0001,  Speed_40Gbps,     L"Intel XL710 40G QSFP+ 2-Ports PCIe NIC"                },
//{ 0x8086,  0x1583,  0x030D,  Speed_40Gbps,     L"BLD E40GI2-XL710 2-ports 40Gb SFP28 NIC"               },  //<Nieht-20181119-Update X710 info as PM required>
  { 0x8086,  0x1583,  0x030E,  Speed_40Gbps,     L"BLD XL710 40G QSFP+ 2-Ports NIC"                       },
//{ 0x8086,  0x1583,  0x0002,  Speed_40Gbps,     L"Intel XL710 40G QSFP+ 2-Ports PCIe NIC"                },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1584,  0x0001,  Speed_40Gbps,     L"Intel XL710 40G QSFP+ 1-Port PCIe NIC"                 },
//{ 0x8086,  0x1584,  0x0002,  Speed_40Gbps,     L"Intel XL710 40G QSFP+ 1-Port PCIe NIC"                 },  //<Kangmm-20201119 Remove repeat information>
  { 0x8086,  0x1584,  0x030D,  Speed_40Gbps,     L"BLD XL710 40G QSFP+ 1-Ports NIC"                       },
//{ 0x8086,  0x1584,  0x030E,  Speed_40Gbps,     L"BLD E40GI1-XL710 1-port 40Gb SFP28 NIC"                },  //<Nieht-20181119-Update X710 info as PM required>
  { 0x8086,  0x158B,  0x0000,  Speed_25Gbps,     L"Intel XXV710 25Gb SFP28 2-Ports NIC"                   },
  { 0x8086,  0x158B,  0xFFFF,  Speed_25Gbps,     L"Intel XXV710 25Gb SFP28 2-Ports NIC"                   },  //Should not be removed.
  { 0x8086,  0x24F0,  0x24F0,  Speed_100Gbps,    L"Intel OPA HSI Silicon 100 Series-BSF"                  },  //<Nieht20170629-add a OPA BrandName+>
  { 0x8086,  0x24F0,  0x2628,  Speed_100Gbps,    L"Intel 100HFA016LS OPA 100Gb 1-Port HFA"                },  // 0x8086   24001215   HFA	Intel 100HFA016LS OPA 100Gb HFA
  { 0x8086,  0x24F0,  0x2629,  Speed_56Gbps,     L"Intel 100HFA018LS OPA 56Gb 1-Port HFA"                 },  // 0x8086   24001217   HFA	Intel 100HFA018LS OPA 58Gb HFA
  { 0x8086,  0x374C,  0xFFFF,  Speed_10Gbps,     L"Intel SFP+ NIC"                                        }, 
  { 0x8086,  0x37CE,  0xFFFF,  Speed_10Gbps,     L"Intel SFP+ 4-Ports NIC"                                }, 
  { 0x8086,  0x37CF,  0xFFFF,  Speed_10Gbps,     L"Intel SFP+ 4-Ports NIC"                                }, 
  { 0x8086,  0x37D0,  0xFFFF,  Speed_10Gbps,     L"Intel X722 10Gb SFP+ NIC"                              },  //SFI 10g
  { 0x8086,  0x37CC,  0xFFFF,  Speed_1Gbps,      L"Intel 1Gbe with Phy Marvell-88E1514 RJ45"              },  //no ocp-c 
  { 0x8086,  0x37D1,  0xFFFF,  Speed_1Gbps,      L"Intel 1Gbe with Phy Marvell-88E1514 RJ45"              },  //Marvel 8514 1g //<Sunhp001-20180206-update the 1G nic name>
  { 0x8086,  0x37D2,  0xFFFF,  Speed_10Gbps,     L"Intel X557 10Gb RJ45 OCP-CCT1 2-Ports NIC"             },  //Intel X557 

  //Beijing Wangxun Technology Co., Ltd.
  { 0x8088,  0x0101,  0x2000,  Speed_10Gbps,     L"Net-Swift RP2000P2 SFP+ 2-port NIC"                    },
  { 0x8088,  0x0103,  0x0401,  Speed_1Gbps,      L"Net-Swift SF400 4-port 1G RJ45 NIC"                    },  //<Fancf-20200508 Add Wangxun Net-swift SF400 support>
  { 0x8088,  0x0107,  0x0402,  Speed_1Gbps,      L"Net-Swift SF400 4-port 1G RJ45 NIC"                    },
  { 0x8088,  0x1001,  0x0105,  Speed_1Gbps,      L"Net-Swift SF200 2-port 1G RJ45 NIC"                    },
  { 0x8088,  0x1001,  0x1000,  Speed_10Gbps,     L"Net-swift RP1000P 10G SFP+ 2-Ports PCIe NIC"           },
  { 0x8088,  0x1007,  0x0402,  Speed_1Gbps,      L"Net-Swift SF400 4-port 1G RJ45 NIC"                    },  //<Fancf-20200508 Add Wangxun Net-swift SF400 support>
  { 0x8088,  0x2001,  0x2000,  Speed_10Gbps,     L"Net-Swift RP2000P2SFP 2-port 10G SFP+ NIC"             },  //<Lvshh001-20210814 Fix the CPL V9.6 error>

  //HiKvision
  { 0xCABC,  0x0220,  0x0030,  Speed_Undefined,  L"HiKvision DS-IK002"                                    },

  //Centec
  { 0xCB10,  0x7148,  0xFFFF,  Speed_Undefined,  L"CTC7148"                                               },  //<jiangzhy001-20200523-Add CTC8096 CTC7148 support >
  { 0xCB10,  0xC010,  0xFFFF,  Speed_Undefined,  L"CTC8096"                                               },  //<jiangzhy001-20200523-Add CTC8096 CTC7148 support >

  //Unknown
  { 0xFFFF,  0xFFFF,  0xFFFF,  Speed_Undefined,  L"New Device"                                            },

};

PCI_DEV_CLASS_INFO PCIDevClassDB[] = {
  //BaseClass       //SubClass    	 //ProgInterface   //DeviceName

  //Base 00
  {0x00,            0x00,            0xFF,             L"except VGA-compatible devices"},
  {0x00,            0x01,            0xFF,             L"VGA-compatible device"},

  //Base 01
  {0x01,            0x00,            0xFF,             L"SCSI controller"},
  {0x01,            0x01,            0xFF,             L"IDE controller"},
  {0x01,            0x02,            0xFF,             L"Floppy disk controller"},
  {0x01,            0x03,            0xFF,             L"IPI bus controller"},
  {0x01,            0x04,            0xFF,             L"RAID controller"},
  {0x01,            0x05,            0xFF,             L"ATA controller"},
  {0x01,            0x06,            0xFF,             L"Serial ATA controller"},
  {0x01,            0x07,            0xFF,             L"SAS controller"},
  {0x01,            0x08,            0xFF,             L"NVMe SSD"},
  {0x01,            0x09,            0xFF,             L"UFS controller"},
  {0x01,            0x80,            0xFF,             L"mass storage controller"},

  //Base 02
  {0x02,            0x00,            0xFF,             L"Ethernet controller"},
  {0x02,            0x01,            0xFF,             L"Token Ring controller"},
  {0x02,            0x02,            0xFF,             L"FDDI controller"},
  {0x02,            0x03,            0xFF,             L"ATM controller"},
  {0x02,            0x04,            0xFF,             L"ISDN controller"},
  {0x02,            0x05,            0xFF,             L"WorldFip controller"},
  {0x02,            0x06,            0xFF,             L"Multi Computing"},
  {0x02,            0x07,            0xFF,             L"InfiniBand Controller"},
  {0x02,            0x08,            0xFF,             L"Host fabric controller"},
  {0x02,            0x80,            0xFF,             L"network controller"},

  //Base 03
  {0x03,            0x00,            0xFF,             L"VGA-compatible controller"},
  {0x03,            0x01,            0xFF,             L"XGA controller"},
  {0x03,            0x02,            0xFF,             L"3D controller"},
  {0x03,            0x80,            0xFF,             L"display controller"},

  //Base 04
  {0x04,            0x00,            0xFF,             L"Video device"},
  {0x04,            0x01,            0xFF,             L"Audio device"},
  {0x04,            0x02,            0xFF,             L"Computer telephony device"},
  {0x04,            0x03,            0xFF,             L"High Definition Audio device"},
  {0x04,            0x80,            0xFF,             L"multimedia device"},

  //Base 05
  {0x05,            0x00,            0xFF,             L"RAM"},
  {0x05,            0x01,            0xFF,             L"Flash"},
  {0x05,            0x80,            0xFF,             L"memory controller"},

  //Base 06
  {0x06,            0x00,            0xFF,             L"Host bridge"},
  {0x06,            0x01,            0xFF,             L"ISA bridge"},
  {0x06,            0x02,            0xFF,             L"EISA bridge"},
  {0x06,            0x03,            0xFF,             L"MCA bridge"},
  {0x06,            0x04,            0xFF,             L"PCI-to-PCI bridge"},
  {0x06,            0x05,            0xFF,             L"PCMCIA bridge"},
  {0x06,            0x06,            0xFF,             L"NuBus bridge"},
  {0x06,            0x07,            0xFF,             L"CardBus bridge"},
  {0x06,            0x08,            0xFF,             L"RACEway bridge"},
  {0x06,            0x09,            0xFF,             L"Semi-transparent PCI-to-PCI bridge"},
  {0x06,            0x0A,            0xFF,             L"InfiniBand-to-PCI host bridge"},
  {0x06,            0x0B,            0xFF,             L"Advanced Switching to PCI host bridge"},
  {0x06,            0x80,            0xFF,             L"bridge device"},

  //Base 07
  {0x07,            0x00,            0xFF,             L"compatible serial controller"},
  {0x07,            0x01,            0xFF,             L"Parallel port"},
  {0x07,            0x02,            0xFF,             L"Multiport serial controller"},
  {0x07,            0x03,            0xFF,             L"Generic modem"},
  {0x07,            0x04,            0xFF,             L"GPIB controller"},
  {0x07,            0x05,            0xFF,             L"Smart Card"},
  {0x07,            0x80,            0xFF,             L"communications device"},

  //Base 08
  {0x08,            0x00,            0xFF,             L"interrupt controller"},
  {0x08,            0x01,            0xFF,             L"DMA controller"},
  {0x08,            0x02,            0xFF,             L"system timer"},
  {0x08,            0x03,            0xFF,             L"RTC controller"},
  {0x08,            0x04,            0xFF,             L"Generic PCI Hot-Plug controller"},
  {0x08,            0x05,            0xFF,             L"SD Host controller"},
  {0x08,            0x06,            0xFF,             L"IOMMU"},
  {0x08,            0x07,            0xFF,             L"Root Complex Event Collector"},
  {0x08,            0x80,            0xFF,             L"system periphera"},

  //Base 09
  {0x09,            0x00,            0xFF,             L"Keyboard controller"},
  {0x09,            0x01,            0xFF,             L"Digitizer"},
  {0x09,            0x02,            0xFF,             L"Mouse controller"},
  {0x09,            0x03,            0xFF,             L"Scanner controller"},
  {0x09,            0x04,            0xFF,             L"Gameport controller"},
  {0x09,            0x80,            0xFF,             L"input controller"},

  //Base 0A
  {0x0A,            0x00,            0xFF,             L"Generic docking station"},
  {0x0A,            0x80,            0xFF,             L"docking station"},

  //Base 0B
  {0x0B,            0x00,            0xFF,             L"386"},
  {0x0B,            0x01,            0xFF,             L"486"},
  {0x0B,            0x02,            0xFF,             L"Pentium"},
  {0x0B,            0x10,            0xFF,             L"Alpha"},
  {0x0B,            0x20,            0xFF,             L"PowerPC"},
  {0x0B,            0x30,            0xFF,             L"MIPS"},
  {0x0B,            0x40,            0xFF,             L"Co-processor"},
  {0x0B,            0x80,            0xFF,             L"processors"},

  //Base 0C
  {0x0C,            0x00,            0xFF,             L"IEEE 1394"},
  {0x0C,            0x01,            0xFF,             L"ACCESS.bus"},
  {0x0C,            0x02,            0xFF,             L"SSA"},
  {0x0C,            0x03,            0xFF,             L"USB"},
  {0x0C,            0x04,            0xFF,             L"Fibre Channel"},
  {0x0C,            0x05,            0xFF,             L"SMBus"},
  {0x0C,            0x06,            0xFF,             L"InfiniBand"},
  {0x0C,            0x07,            0xFF,             L"IPMI Interface"},
  {0x0C,            0x08,            0xFF,             L"SERCOS Interface Standard"},
  {0x0C,            0x09,            0xFF,             L"CANbus"},
  {0x0C,            0x80,            0xFF,             L"Serial Bus Controllers"},

  //Base 0D
  {0x0D,            0x00,            0xFF,             L"iRDA compatible controller"},
  {0x0D,            0x01,            0xFF,             L"Consumer IR controller"},
  {0x0D,            0x10,            0xFF,             L"RF controller"},
  {0x0D,            0x11,            0xFF,             L"Bluetooth"},
  {0x0D,            0x12,            0xFF,             L"Broadband"},
  {0x0D,            0x20,            0xFF,             L"Ethernet (802.11a - 5 GHz)"},
  {0x0D,            0x21,            0xFF,             L"Ethernet (802.11b - 2.4 GHz)"},
  {0x0D,            0x40,            0xFF,             L"Cellular controller/modem"},
  {0x0D,            0x41,            0xFF,             L"Cellular controller/modem plus Ethernet"},
  {0x0D,            0x80,            0xFF,             L"wireless controller"},

  //Base 0E
  {0x0E,            0x00,            0xFF,             L"I/O controllers"},

  //Base 0F
  {0x0F,            0x01,            0xFF,             L"TV"},
  {0x0F,            0x02,            0xFF,             L"Audio"},
  {0x0F,            0x03,            0xFF,             L"Voice"},
  {0x0F,            0x04,            0xFF,             L"Data"},
  {0x0F,            0x80,            0xFF,             L"satellite communication controller"},

  //Base 10
  {0x10,            0x00,            0xFF,             L"Network and computing en/decryption"},
  {0x10,            0x10,            0xFF,             L"Entertainment en/decryption"},
  {0x10,            0x80,            0xFF,             L"Other en/decryption"},

  //Base 11
  {0x11,            0x00,            0xFF,             L"DPIO modules"},
  {0x11,            0x01,            0xFF,             L"Performance counters"},
  {0x11,            0x10,            0xFF,             L"Communications synchronization plus time and frequency test/measurement"},
  {0x11,            0x20,            0xFF,             L"Management card"},
  {0x11,            0x80,            0xFF,             L"data acquisition/signal processing controllers"},

  //Base 12
  {0x12,            0x00,            0xFF,             L"Processing Accelerator"},

  //Base 13
  {0x13,            0x00,            0xFF,             L"Non-Essential Instrumentation Function"},
  {0xFF,            0xFF,            0xFF,             L"Unknown"}
};

PCI_DEV_VID_INFO PCIDevVidDB[] = {
  //VendorID      //VendorName
  {0x8086,         L"Intel Corporation"},
  {0x1A03,         L"Aspeed"},
  {0x1A41,         L"Tilera Corporation"},
  {0x14A4,         L"Liteon"},
  {0x144D,         L"Samsung"},
  {0x2646,         L"Kingston Technology Co."},
  {0x0033,         L"Paradyne Corp"},
  {0x003D,         L"master"},
  {0x0070,         L"Hauppauge Computer Works Inc."}, 
  {0x0100,         L"USBPDO-8"},
  {0x0123,         L"General Dynamics"},
  {0x0315,         L"SK - Electronics Co., Ltd."},
  {0x0402,         L"Acer aspire one"},
  {0x046D,         L"Logitech Inc."},
  {0x0483,         L"UPEK"},
  {0x04A9,         L"Canon"},
  {0x04B3,         L"IBM"},
  {0x04D9,         L"Filco"},
  {0x04F2,         L"Chicony Electronics Co."},
  {0x051D,         L"ACPI VEN_INT&DEV_33A0"},
  {0x0529,         L"Aladdin E-Token"},
  {0x0553,         L"Aiptek USA"},
  {0x058f,         L"Alcor Micro Corp."},
  {0x0590,         L"Omron Corp"},
  {0x05ac,         L"Apple Inc."},
  {0x05E1,         L"D-MAX"},
  {0x064e,         L"SUYIN Corporation"},
  {0x067B,         L"Prolific Technology Inc."},
  {0x06FE,         L"Acresso Software Inc."},
  {0x0711,         L"SIIG, Inc."},
  {0x093a,         L"KYE Systems Corp. / Pixart Imaging"},
  {0x096E,         L"USB Rockey dongle from Feitain"},
  {0x0A5C,         L"Broadcom Corporation"},
  {0x0A89,         L"BREA Technologies Inc."},
  {0x0A92,         L"Egosys, Inc."},
  {0x0AC8,         L"ASUS"},
  {0x0b05,         L"Toshiba Bluetooth RFBUS, RFCOM, RFHID"},
  {0x0c45,         L"Microdia Ltd."},
  {0x0cf3,         L"TP-Link"},
  {0x0D2E,         L"Feedback Instruments Ltd."},
  {0x0D8C,         L"C-Media Electronics, Inc."},
  {0x0DF6,         L"Sitecom"},
  {0x0E11,         L"Compaq Computer Corp."},
  {0x0E8D,         L"MediaTek Inc."},
  {0x1000,         L"LSI Logic"},
  {0x1001,         L"Kolter Electronic - Germany"},
  {0x1002,         L"Advanced Micro Devices, Inc."},
  {0x1003,         L"ULSI"},
  {0x1004,         L"VLSI Technology"},
  {0x1006,         L"Reply Group"},
  {0x1007,         L"Netframe Systems Inc."},
  {0x1008,         L"Epson"},
  {0x100A,         L"Phoenix"},
  {0x100B,         L"National Semiconductors"},
  {0x100C,         L"Tseng Labs"},
  {0x100D,         L"AST Research"},
  {0x100E,         L"Weitek"},
  {0x1010,         L"Video Logic Ltd."},
  {0x1011,         L"Digital Equipment Corporation"},
  {0x1012,         L"Micronics Computers Inc."},
  {0x1013,         L"Cirrus Logic"},
  {0x1014,         L"International Business Machines Corp."},
  {0x1016,         L"Fujitsu ICL Computers"},
  {0x1017,         L"Spea Software AG"},
  {0x1018,         L"Unisys Systems"},
  {0x1019,         L"Elitegroup Computer System"},
  {0x101A,         L"NCR Corporation"},
  {0x101B,         L"Vitesse Semiconductor"},
  {0x101E,         L"American Megatrends Inc."},
  {0x101F,         L"PictureTel Corp."},
  {0x1020,         L"Hitachi Computer Electronics"},
  {0x1021,         L"Oki Electric Industry"},
  {0x1022,         L"Advanced Micro Devices"},
  {0x1023,         L"TRIDENT MICRO"},
  {0x1025,         L"Acer Incorporated"},
  {0x1028,         L"Dell Inc."},
  {0x102A,         L"LSI Logic Headland Division"},
  {0x102B,         L"Matrox Electronic Systems Ltd."},
  {0x102C,         L"Asiliant (Chips And Technologies)"},
  {0x102D,         L"Wyse Technology"},
  {0x102E,         L"Olivetti Advanced Technology"},
  {0x102F,         L"Toshiba America"},
  {0x1030,         L"TMC Research"},
  {0x1031,         L"miro Computer Products AG"},
  {0x1033,         L"NEC Electronics"},
  {0x1034,         L"Burndy Corporation"},
  {0x1036,         L"Future Domain"},
  {0x1037,         L"Hitachi Micro Systems Inc"},
  {0x1038,         L"AMP Incorporated"},
  {0x1039,         L"Silicon Integrated Systems"},
  {0x103A,         L"Seiko Epson Corporation"},
  {0x103B,         L"Tatung Corp. Of America"},
  {0x103C,         L"Hewlett-Packard"},
  {0x103E,         L"Solliday Engineering"},
  {0x103F,         L"Logic Modeling"},
  {0x1041,         L"Computrend"},
  {0x1043,         L"Asustek Computer Inc."},
  {0x1044,         L"Distributed Processing Tech"},
  {0x1045,         L"OPTi Inc."},
  {0x1046,         L"IPC Corporation LTD"},
  {0x1047,         L"Genoa Systems Corp."},
  {0x1048,         L"ELSA GmbH"},
  {0x1049,         L"Fountain Technology"},
  {0x104A,         L"STMicroelectronics"},
  {0x104B,         L"Mylex / Buslogic"},
  {0x104C,         L"Texas Instruments"},
  {0x104D,         L"Sony Corporation"},
  {0x104E,         L"Oak Technology"},
  {0x104F,         L"Co-Time Computer Ltd."},
  {0x1050,         L"Winbond Electronics Corp."},
  {0x1051,         L"Anigma Corp."},
  {0x1053,         L"Young Micro Systems"},
  {0x1054,         L"Hitachi Ltd"},
  {0x1055,         L"Standard Microsystems Corp."},
  {0x1056,         L"ICL"},
  {0x1057,         L"Motorola"},
  {0x1058,         L"Electronics & Telecommunication Res"},
  {0x1059,         L"Kontron Canada"},
  {0x105A,         L"Promise Technology"},
  {0x105B,         L"Mobham chip"},
  {0x105C,         L"Wipro Infotech Limited"},
  {0x105D,         L"Number Nine Visual Technology"},
  {0x105E,         L"Vtech Engineering Canada Ltd."},
  {0x105F,         L"Infotronic America Inc."},
  {0x1060,         L"United Microelectronics"},
  {0x1061,         L"8x8 Inc."},
  {0x1062,         L"Maspar Computer Corp."},
  {0x1063,         L"Ocean Office Automation"},
  {0x1064,         L"Alcatel Cit"},
  {0x1065,         L"Texas Microsystems"},
  {0x1066,         L"Picopower Technology"},
  {0x1067,         L"Mitsubishi Electronics"},
  {0x1068,         L"Diversified Technology"},
  {0x106A,         L"Aten Research Inc."},
  {0x106B,         L"Apple Inc."},
  {0x106C,         L"Hyundai Electronics America"},
  {0x106D,         L"Sequent Computer Systems"},
  {0x106E,         L"DFI Inc."},
  {0x106F,         L"City Gate Development LTD"},
  {0x1070,         L"Daewoo Telecom Ltd."},
  {0x1071,         L"Mitac"},
  {0x1072,         L"GIT Co. Ltd."},
  {0x1073,         L"Yamaha Corporation"},
  {0x1074,         L"Nexgen Microsystems"},
  {0x1075,         L"Advanced Integration Research"},
  {0x1077,         L"QLogic Corporation"},
  {0x1078,         L"Cyrix Corporation"},
  {0x1079,         L"I-Bus"},
  {0x107A,         L"Networth controls"},
  {0x107B,         L"Gateway 2000"},
  {0x107C,         L"Goldstar Co. Ltd."},
  {0x107D,         L"Leadtek Research"},
  {0x107E,         L"Testernec"},
  {0x107F,         L"Data Technology Corporation"},
  {0x1080,         L"Cypress Semiconductor"},
  {0x1081,         L"Radius Inc."},
  {0x1082,         L"EFA Corporation Of America"},
  {0x1083,         L"Forex Computer Corporation"},
  {0x1084,         L"Parador"},
  {0x1085,         L"Tulip Computers Int'l BV"},
  {0x1086,         L"J. Bond Computer Systems"},
  {0x1087,         L"Cache Computer"},
  {0x1088,         L"Microcomputer Systems (M) Son"},
  {0x1089,         L"Data General Corporation"},
  {0x108A,         L"SBS Operations"},
  {0x108C,         L"Oakleigh Systems Inc."},
  {0x108D,         L"Olicom"},
  {0x108E,         L"Sun Microsystems"},
  {0x108F,         L"Systemsoft Corporation"},
  {0x1090,         L"Encore Computer Corporation"},
  {0x1091,         L"Intergraph Corporation"},
  {0x1092,         L"Diamond Computer Systems"},
  {0x1093,         L"National Instruments"},
  {0x1094,         L"Apostolos"},
  {0x1095,         L"Silicon Image, Inc."},
  {0x1096,         L"Alacron"},
  {0x1097,         L"Appian Graphics"},
  {0x1098,         L"Quantum Designs Ltd."},
  {0x1099,         L"Samsung Electronics Co. Ltd."},
  {0x109A,         L"Packard Bell"},
  {0x109B,         L"Gemlight Computer Ltd."},
  {0x109C,         L"Megachips Corporation"},
  {0x109D,         L"Zida Technologies Ltd."},
  {0x109E,         L"Brooktree Corporation"},
  {0x109F,         L"Trigem Computer Inc."},
  {0x10A0,         L"Meidensha Corporation"},
  {0x10A1,         L"Juko Electronics Inc. Ltd."},
  {0x10A2,         L"Quantum Corporation"},
  {0x10A3,         L"Everex Systems Inc."},
  {0x10A4,         L"Globe Manufacturing Sales"},
  {0x10A5,         L"Racal Interlan"},
  {0x10A8,         L"Sierra Semiconductor"},
  {0x10A9,         L"Silicon Graphics"},
  {0x10AB,         L"Digicom"},
  {0x10AC,         L"Honeywell IASD"},
  {0x10AD,         L"Winbond Systems Labs"},
  {0x10AE,         L"Cornerstone Technology"},
  {0x10AF,         L"Micro Computer Systems Inc."},
  {0x10B0,         L"Gainward GmbH"},
  {0x10B1,         L"Cabletron Systems Inc."},
  {0x10B2,         L"Raytheon Company"},
  {0x10B3,         L"Databook Inc."},
  {0x10B4,         L"STB Systems"},
  {0x10B5,         L"PLX Technology Inc."},
  {0x10B6,         L"Madge Networks"},
  {0x10B7,         L"3Com Corporation"},
  {0x10B8,         L"Standard Microsystems Corporation"},
  {0x10B9,         L"Ali Corporation"},
  {0x10BA,         L"Mitsubishi Electronics Corp."},
  {0x10BB,         L"Dapha Electronics Corporation"},
  {0x10BC,         L"Advanced Logic Research Inc."},
  {0x10BD,         L"Surecom Technology"},
  {0x10BE,         L"Tsenglabs International Corp."},
  {0x10BF,         L"MOST Corp."},
  {0x10C0,         L"Boca Research Inc."},
  {0x10C1,         L"ICM Corp. Ltd."},
  {0x10C2,         L"Auspex Systems Inc."},
  {0x10C3,         L"Samsung Semiconductors"},
  {0x10C4,         L"Award Software Int'l Inc."},
  {0x10C5,         L"Xerox Corporation"},
  {0x10C6,         L"Rambus Inc."},
  {0x10C8,         L"Neomagic Corporation"},
  {0x10C9,         L"Dataexpert Corporation"},
  {0x10CA,         L"Fujitsu Siemens"},
  {0x10CB,         L"Omron Corporation"},
  {0x10CD,         L"Advanced System Products"},
  {0x10CF,         L"Fujitsu Ltd."},
  {0x10D1,         L"Future+ Systems"},
  {0x10D2,         L"Molex Incorporated"},
  {0x10D3,         L"Jabil Circuit Inc."},
  {0x10D4,         L"Hualon Microelectronics"},
  {0x10D5,         L"Autologic Inc."},
  {0x10D6,         L"Wilson .co .ltd"},
  {0x10D7,         L"BCM Advanced Research"},
  {0x10D8,         L"Advanced Peripherals Labs"},
  {0x10D9,         L"Macronix International Co. Ltd."},
  {0x10DB,         L"Rohm Research"},
  {0x10DC,         L"CERN-European Lab. for Particle Physics"},
  {0x10DD,         L"Evans & Sutherland"},
  {0x10DE,         L"NVIDIA"},
  {0x10DF,         L"Emulex Corporation"},
  {0x10E1,         L"Tekram Technology Corp. Ltd."},
  {0x10E2,         L"Aptix Corporation"},
  {0x10E3,         L"Tundra Semiconductor Corp."},
  {0x10E4,         L"Tandem Computers"},
  {0x10E5,         L"Micro Industries Corporation"},
  {0x10E6,         L"Gainbery Computer Products Inc."},
  {0x10E7,         L"Vadem"},
  {0x10E8,         L"Applied Micro Circuits Corp."},
  {0x10E9,         L"Alps Electronic Corp. Ltd."},
  {0x10EA,         L"Tvia, Inc."},
  {0x10EB,         L"Artist Graphics"},
  {0x10EC,         L"Realtek Semiconductor Corp."},
  {0x10ED,         L"Ascii Corporation"},
  {0x10EE,         L"Xilinx Corporation"},
  {0x10EF,         L"Racore Computer Products"},
  {0x10F0,         L"Curtiss-Wright Controls Embedded Computing"},
  {0x10F1,         L"Tyan Computer"},
  {0x10F2,         L"Achme Computer Inc"},
  {0x10F3,         L"Alaris Inc."},
  {0x10F4,         L"S-Mos Systems"},
  {0x10F5,         L"NKK Corporation"},
  {0x10F6,         L"Creative Electronic Systems SA"},
  {0x10F7,         L"Matsushita Electric Industrial Corp."},
  {0x10F8,         L"Altos India Ltd."},
  {0x10F9,         L"PC Direct"},
  {0x10FA,         L"Truevision"},
  {0x10FB,         L"Thesys Microelectronic's"},
  {0x10FC,         L"I-O Data Device Inc."},
  {0x10FD,         L"Soyo Technology Corp. Ltd."},
  {0x10FE,         L"Fast Electronic GmbH"},
  {0x10FF,         L"Ncube"},
  {0x1100,         L"Jazz Multimedia"},
  {0x1101,         L"Initio Corporation"},
  {0x1102,         L"Creative Technology LTD."},
  {0x1103,         L"HighPoint Technologies, Inc."},
  {0x1104,         L"Rasterops"},
  {0x1105,         L"Sigma Designs Inc."},
  {0x1106,         L"VIA Technologies, Inc."},
  {0x1107,         L"Stratus Computer"},
  {0x1108,         L"Proteon Inc."},
  {0x1109,         L"Adaptec/Cogent Data Technologies"},
  {0x110A,         L"Siemens AG"},
  {0x110B,         L"Chromatic Research Inc"},
  {0x110C,         L"Mini-Max Technology Inc."},
  {0x110D,         L"ZNYX Corporation"},
  {0x110E,         L"CPU Technology"},
  {0x110F,         L"Ross Technology"},
  {0x1112,         L"Osicom Technologies Inc."},
  {0x1113,         L"Accton Technology Corporation"},
  {0x1114,         L"Atmel Corp."},
  {0x1116,         L"Data Translation, Inc."},
  {0x1117,         L"Datacube Inc."},
  {0x1118,         L"Berg Electronics"},
  {0x1119,         L"ICP vortex Computersysteme GmbH"},
  {0x111A,         L"Efficent Networks"},
  {0x111C,         L"Tricord Systems Inc."},
  {0x111D,         L"Integrated Device Technology Inc."},
  {0x111F,         L"Precision Digital Images"},
  {0x1120,         L"EMC Corp."},
  {0x1121,         L"Zilog"},
  {0x1123,         L"Excellent Design Inc."},
  {0x1124,         L"Leutron Vision AG"},
  {0x1125,         L"Eurocore/Vigra"},
  {0x1127,         L"FORE Systems"},
  {0x1129,         L"Firmworks"},
  {0x112A,         L"Hermes Electronics Co. Ltd."},
  {0x112C,         L"Zenith Data Systems"},
  {0x112D,         L"Ravicad"},
  {0x112E,         L"Infomedia"},
  {0x1130,         L"Computervision"},
  {0x1131,         L"NXP Semiconductors N.V."},
  {0x1132,         L"Mitel Corp."},
  {0x1133,         L"Eicon Networks Corporation"},
  {0x1134,         L"Mercury Computer Systems Inc."},
  {0x1135,         L"Fuji Xerox Co Ltd"},
  {0x1136,         L"Momentum Data Systems"},
  {0x1137,         L"Cisco Systems Inc"},
  {0x1138,         L"Ziatech Corporation"},
  {0x1139,         L"Dynamic Pictures Inc"},
  {0x113A,         L"FWB Inc"},
  {0x113B,         L"Network Computing Devices"},
  {0x113C,         L"Cyclone Microsystems Inc."},
  {0x113D,         L"Leading Edge Products Inc"},
  {0x113E,         L"Sanyo Electric Co"},
  {0x113F,         L"Equinox Systems"},
  {0x1140,         L"Intervoice Inc"},
  {0x1141,         L"Crest Microsystem Inc"},
  {0x1142,         L"Alliance Semiconductor"},
  {0x1143,         L"Netpower Inc"},
  {0x1144,         L"Cincinnati Milacron"},
  {0x1145,         L"Workbit Corp"},
  {0x1146,         L"Force Computers"},
  {0x1147,         L"Interface Corp"},
  {0x1148,         L"Marvell Semiconductor Germany GmbH"},
  {0x1149,         L"Win System Corporation"},
  {0x114A,         L"VMIC"},
  {0x114B,         L"Canopus corporation"},
  {0x114C,         L"Annabooks"},
  {0x114D,         L"IC Corporation"},
  {0x114E,         L"Nikon Systems Inc"},
  {0x114F,         L"Digi International"},
  {0x1150,         L"Thinking Machines Corporation"},
  {0x1151,         L"JAE Electronics Inc."},
  {0x1153,         L"Land Win Electronic Corp"},
  {0x1154,         L"Melco Inc"},
  {0x1155,         L"Pine Technology Ltd"},
  {0x1156,         L"Periscope Engineering"},
  {0x1157,         L"Avsys Corporation"},
  {0x1158,         L"Voarx R&D Inc"},
  {0x1159,         L"Mutech"},
  {0x115A,         L"Harlequin Ltd"},
  {0x115B,         L"Parallax Graphics"},
  {0x115C,         L"Photron Ltd."},
  {0x115D,         L"Xircom"},
  {0x115E,         L"Peer Protocols Inc"},
  {0x115F,         L"Maxtor Corporation"},
  {0x1160,         L"Megasoft Inc"},
  {0x1161,         L"PFU Ltd"},
  {0x1162,         L"OA Laboratory Co Ltd"},
  {0x1163,         L"mohamed alsherif"},
  {0x1164,         L"Advanced Peripherals Tech"},
  {0x1165,         L"Imagraph Corporation"},
  {0x1166,         L"Broadcom / ServerWorks"},
  {0x1167,         L"Mutoh Industries Inc"},
  {0x1168,         L"Thine Electronics Inc"},
  {0x1169,         L"Centre f/Dev. of Adv. Computing"},
  {0x116A,         L"Luminex Software, Inc"},
  {0x116B,         L"Connectware Inc"},
  {0x116C,         L"Intelligent Resources"},
  {0x116E,         L"Electronics for Imaging"},
  {0x1170,         L"Inventec Corporation"},
  {0x1172,         L"Altera Corporation"},
  {0x1173,         L"Adobe Systems"},
  {0x1174,         L"Bridgeport Machines"},
  {0x1175,         L"Mitron Computer Inc."},
  {0x1176,         L"SBE"},
  {0x1177,         L"Silicon Engineering"},
  {0x1178,         L"Alfa Inc"},
  {0x1179,         L"Toshiba corporation"},
  {0x117A,         L"A-Trend Technology"},
  {0x117B,         L"LG (Lucky Goldstar) Electronics Inc."},
  {0x117C,         L"Atto Technology"},
  {0x117D,         L"Becton & Dickinson"},
  {0x117E,         L"T/R Systems"},
  {0x117F,         L"Integrated Circuit Systems"},
  {0x1180,         L"RicohCompany,Ltd."},
  {0x1183,         L"Fujikura Ltd"},
  {0x1184,         L"Forks Inc"},
  {0x1185,         L"Dataworld"},
  {0x1186,         L"D-Link System Inc"},
  {0x1187,         L"Philips Healthcare"},
  {0x1188,         L"Shima Seiki Manufacturing Ltd."},
  {0x1189,         L"Matsushita Electronics"},
  {0x118A,         L"Hilevel Technology"},
  {0x118B,         L"Hypertec Pty Ltd"},
  {0x118C,         L"Corollary Inc"},
  {0x118D,         L"BitFlow Inc"},
  {0x118E,         L"Hermstedt AG"},
  {0x118F,         L"Green Logic"},
  {0x1190,         L"Tripace"},
  {0x1191,         L"Acard Technology Corp."},
  {0x1192,         L"Densan Co. Ltd"},
  {0x1194,         L"Toucan Technology"},
  {0x1195,         L"Ratoc System Inc"},
  {0x1196,         L"Hytec Electronics Ltd"},
  {0x1197,         L"Gage Applied Technologies"},
  {0x1198,         L"Lambda Systems Inc"},
  {0x1199,         L"Attachmate Corp."},
  {0x119A,         L"Mind/Share Inc."},
  {0x119B,         L"Omega Micro Inc."},
  {0x119C,         L"Information Technology Inst."},
  {0x119D,         L"Bug Sapporo Japan"},
  {0x119E,         L"Fujitsu Microelectronics Ltd."},
  {0x119F,         L"Bull Hn Information Systems"},
  {0x11A1,         L"Hamamatsu Photonics K.K."},
  {0x11A2,         L"Sierra Research and Technology"},
  {0x11A3,         L"Deuretzbacher GmbH & Co. Eng. KG"},
  {0x11A4,         L"Barco"},
  {0x11A5,         L"MicroUnity Systems Engineering Inc."},
  {0x11A6,         L"Pure Data"},
  {0x11A7,         L"Power Computing Corp."},
  {0x11A8,         L"Systech Corp."},
  {0x11A9,         L"InnoSys Inc."},
  {0x11AA,         L"Actel"},
  {0x11AB,         L"Marvell Semiconductor"},
  {0x11AC,         L"Canon Information Systems"},
  {0x11AD,         L"Lite-On Technology Corp."},
  {0x11AE,         L"Scitex Corporation Ltd"},
  {0x11AF,         L"Avid Technology, Inc."},
  {0x11B0,         L"Quicklogic Corp"},
  {0x11B1,         L"Apricot Computers"},
  {0x11B2,         L"Eastman Kodak"},
  {0x11B3,         L"Barr Systems Inc."},
  {0x11B4,         L"Leitch Technology International"},
  {0x11B5,         L"Radstone Technology Ltd."},
  {0x11B6,         L"United Video Corp"},
  {0x11B7,         L"Motorola"},
  {0x11B8,         L"Xpoint Technologies Inc"},
  {0x11B9,         L"Pathlight Technology Inc."},
  {0x11BA,         L"Videotron Corp"},
  {0x11BB,         L"Pyramid Technology"},
  {0x11BC,         L"Network Peripherals Inc"},
  {0x11BD,         L"Pinnacle system"},
  {0x11BE,         L"International Microcircuits Inc"},
  {0x11BF,         L"Astrodesign Inc."},
  {0x11C1,         L"LSI Corporation"},
  {0x11C2,         L"Sand Microelectronics"},
  {0x11C4,         L"Document Technologies Ind."},
  {0x11C5,         L"Shiva Corporatin"},
  {0x11C6,         L"Dainippon Screen Mfg. Co"},
  {0x11C7,         L"D.C.M. Data Systems"},
  {0x11C8,         L"Dolphin Interconnect Solutions"},
  {0x11C9,         L"MAGMA"},
  {0x11CA,         L"LSI Systems Inc"},
  {0x11CB,         L"Specialix International Ltd."},
  {0x11CC,         L"Michels & Kleberhoff Computer GmbH"},
  {0x11CD,         L"HAL Computer Systems Inc."},
  {0x11CE,         L"Primary Rate Inc"},
  {0x11CF,         L"Pioneer Electronic Corporation"},
  {0x11D0,         L"BAE SYSTEMS - Manassas"},
  {0x11D1,         L"AuraVision Corporation"},
  {0x11D2,         L"Intercom Inc."},
  {0x11D3,         L"Trancell Systems Inc"},
  {0x11D4,         L"Analog Devices, Inc."},
  {0x11D5,         L"Tahoma Technology"},
  {0x11D6,         L"Tekelec Technologies"},
  {0x11D7,         L"TRENTON Technology, Inc."},
  {0x11D8,         L"Image Technologies Development"},
  {0x11D9,         L"Tec Corporation"},
  {0x11DA,         L"Novell"},
  {0x11DB,         L"Sega Enterprises Ltd"},
  {0x11DC,         L"Questra Corp"},
  {0x11DD,         L"Crosfield Electronics Ltd"},
  {0x11DE,         L"Zoran Corporation"},
  {0x11E1,         L"Gec Plessey Semi Inc"},
  {0x11E2,         L"Samsung Information Systems America"},
  {0x11E3,         L"Quicklogic Corp"},
  {0x11E4,         L"Second Wave Inc"},
  {0x11E5,         L"IIX Consulting"},
  {0x11E6,         L"Mitsui-Zosen System Research"},
  {0x11E8,         L"Digital Processing Systems Inc"},
  {0x11E9,         L"Highwater Designs Ltd"},
  {0x11EA,         L"Elsag Bailey"},
  {0x11EB,         L"Formation, Inc"},
  {0x11EC,         L"Coreco Inc"},
  {0x11ED,         L"Mediamatics"},
  {0x11EE,         L"Dome Imaging Systems Inc"},
  {0x11EF,         L"Nicolet Technologies BV"},
  {0x11F0,         L"Triya"},
  {0x11F2,         L"Picture Tel Japan KK"},
  {0x11F3,         L"Keithley Instruments, Inc"},
  {0x11F4,         L"Kinetic Systems Corporation"},
  {0x11F5,         L"Computing Devices Intl"},
  {0x11F6,         L"Powermatic Data Systems Ltd"},
  {0x11F7,         L"Scientific Atlanta"},
  {0x11F8,         L"PMC-Sierra Inc."},
  {0x11F9,         L"I-Cube Inc"},
  {0x11FA,         L"Kasan Electronics Co Ltd"},
  {0x11FB,         L"Datel Inc"},
  {0x11FD,         L"High Street Consultants"},
  {0x11FE,         L"Comtrol Corp"},
  {0x11FF,         L"Scion Corp"},
  {0x1200,         L"CSS Corp"},
  {0x1201,         L"Vista Controls Corp"},
  {0x1202,         L"Network General Corp"},
  {0x1203,         L"Bayer Corporation Agfa Div"},
  {0x1204,         L"Lattice Semiconductor Corp"},
  {0x1205,         L"Array Corp"},
  {0x1206,         L"Amdahl Corp"},
  {0x1208,         L"Parsytec GmbH"},
  {0x1209,         L"Sci Systems Inc"},
  {0x120A,         L"Synaptel"},
  {0x120B,         L"Adaptive Solutions"},
  {0x120D,         L"Compression Labs Inc."},
  {0x120E,         L"Cyclades Corporation"},
  {0x120F,         L"Essential Communications"},
  {0x1210,         L"Hyperparallel Technologies"},
  {0x1211,         L"Braintech Inc"},
  {0x1213,         L"Applied Intelligent Systems Inc"},
  {0x1214,         L"Performance Technologies Inc"},
  {0x1215,         L"Interware Co Ltd"},
  {0x1216,         L"Purup-Eskofot A/S"},
  {0x1217,         L"O2Micro Inc"},
  {0x1218,         L"Hybricon Corp"},
  {0x1219,         L"First Virtual Corp"},
  {0x121A,         L"3dfx Interactive Inc"},
  {0x121B,         L"Advanced Telecommunications Modules"},
  {0x121C,         L"Nippon Texa Co Ltd"},
  {0x121D,         L"LiPPERT Embedded Computers GmbH"},
  {0x121E,         L"CSPI"},
  {0x121F,         L"Arcus Technology Inc"},
  {0x1220,         L"Ariel Corporation"},
  {0x1221,         L"Contec Microelectronics Europe BV"},
  {0x1222,         L"Ancor Communications Inc"},
  {0x1223,         L"Artesyn Embedded Technologies"},
  {0x1224,         L"Interactive Images"},
  {0x1225,         L"Power I/O Inc."},
  {0x1227,         L"Tech-Source"},
  {0x1228,         L"Norsk Elektro Optikk A/S"},
  {0x1229,         L"Data Kinesis Inc."},
  {0x122A,         L"Integrated Telecom"},
  {0x122B,         L"LG Industrial Systems Co. Ltd."},
  {0x122C,         L"sci-worx GmbH"},
  {0x122D,         L"Aztech System Ltd"},
  {0x122E,         L"Absolute Analysis"},
  {0x122F,         L"Andrew Corp."},
  {0x1230,         L"Fishcamp Engineering"},
  {0x1231,         L"Woodward McCoach Inc."},
  {0x1233,         L"Bus-Tech Inc."},
  {0x1234,         L"Technical Corp"},
  {0x1236,         L"Sigma Designs, Inc"},
  {0x1237,         L"Alta Technology Corp."},
  {0x1238,         L"Adtran"},
  {0x1239,         L"The 3DO Company"},
  {0x123A,         L"Visicom Laboratories Inc."},
  {0x123B,         L"Seeq Technology Inc."},
  {0x123C,         L"Century Systems Inc."},
  {0x123D,         L"Engineering Design Team Inc."},
  {0x123F,         L"C-Cube Microsystems"},
  {0x1240,         L"Marathon Technologies Corp."},
  {0x1241,         L"DSC Communications"},
  {0x1242,         L"JNI Corporation"},
  {0x1243,         L"Delphax"},
  {0x1244,         L"AVM AUDIOVISUELLES MKTG & Computer GmbH"},
  {0x1245,         L"APD S.A."},
  {0x1246,         L"Dipix Technologies Inc"},
  {0x1247,         L"Xylon Research Inc."},
  {0x1248,         L"Central Data Corp."},
  {0x1249,         L"Samsung Electronics Co. Ltd."},
  {0x124A,         L"AEG Electrocom GmbH"},
  {0x124C,         L"Solitron Technologies Inc."},
  {0x124D,         L"Stallion Technologies"},
  {0x124E,         L"Cylink"},
  {0x124F,         L"Infortrend Technology Inc"},
  {0x1250,         L"Hitachi Microcomputer System Ltd."},
  {0x1251,         L"VLSI Solution OY"},
  {0x1253,         L"Guzik Technical Enterprises"},
  {0x1254,         L"Linear Systems Ltd."},
  {0x1255,         L"Optibase Ltd."},
  {0x1256,         L"Perceptive Solutions Inc."},
  {0x1257,         L"Vertex Networks Inc."},
  {0x1258,         L"Gilbarco Inc."},
  {0x1259,         L"Allied Telesyn International"},
  {0x125A,         L"ABB Power Systems"},
  {0x125B,         L"Asix Electronics Corp."},
  {0x125C,         L"Aurora Technologies Inc."},
  {0x125D,         L"ESS Technology"},
  {0x125E,         L"Specialvideo Engineering SRL"},
  {0x125F,         L"Concurrent Technologies Inc."},
  {0x1260,         L"Intersil Corporation"},
  {0x1261,         L"Matsushita-Kotobuki Electronics Indu"},
  {0x1262,         L"ES Computer Co. Ltd."},
  {0x1263,         L"Sonic Solutions"},
  {0x1264,         L"Aval Nagasaki Corp."},
  {0x1265,         L"Casio Computer Co. Ltd."},
  {0x1266,         L"Microdyne Corp."},
  {0x1267,         L"S.A. Telecommunications"},
  {0x1268,         L"Tektronix"},
  {0x1269,         L"Thomson-CSF/TTM"},
  {0x126A,         L"Lexmark International Inc."},
  {0x126B,         L"Adax Inc."},
  {0x126C,         L"Nortel Networks Corp."},
  {0x126D,         L"Splash Technology Inc."},
  {0x126E,         L"Sumitomo Metal Industries Ltd."},
  {0x126F,         L"Silicon Motion"},
  {0x1270,         L"Olympus Optical Co. Ltd."},
  {0x1271,         L"GW Instruments"},
  {0x1272,         L"themrtaish"},
  {0x1273,         L"Hughes Network Systems"},
  {0x1274,         L"Ensoniq"},
  {0x1275,         L"Network Appliance"},
  {0x1276,         L"Switched Network Technologies Inc."},
  {0x1277,         L"Comstream"},
  {0x1278,         L"Transtech Parallel Systems"},
  {0x1279,         L"Transmeta Corp."},
  {0x127B,         L"Pixera Corp"},
  {0x127C,         L"Crosspoint Solutions Inc."},
  {0x127D,         L"Vela Research LP"},
  {0x127E,         L"Winnov L.P."},
  {0x127F,         L"Fujifilm"},
  {0x1280,         L"Photoscript Group Ltd."},
  {0x1281,         L"Yokogawa Electronic Corp."},
  {0x1282,         L"Davicom Semiconductor Inc."},
  {0x1283,         L"Waldo"},
  {0x1285,         L"Platform Technologies Inc."},
  {0x1286,         L"MAZeT GmbH"},
  {0x1287,         L"LuxSonor Inc."},
  {0x1288,         L"Timestep Corp."},
  {0x1289,         L"AVC Technology Inc."},
  {0x128A,         L"Asante Technologies Inc."},
  {0x128B,         L"Transwitch Corp."},
  {0x128C,         L"Retix Corp."},
  {0x128D,         L"G2 Networks Inc."},
  {0x128F,         L"Tateno Dennou Inc."},
  {0x1290,         L"Sord Computer Corp."},
  {0x1291,         L"NCS Computer Italia"},
  {0x1292,         L"Tritech Microelectronics Intl PTE"},
  {0x1293,         L"Media Reality Technology"},
  {0x1294,         L"Rhetorex Inc."},
  {0x1295,         L"Imagenation Corp."},
  {0x1296,         L"Kofax Image Products"},
  {0x1297,         L"Shuttle Computer"},
  {0x1298,         L"Spellcaster Telecommunications Inc."},
  {0x1299,         L"Knowledge Technology Laboratories"},
  {0x129A,         L"Curtiss Wright Controls Electronic Systems"},
  {0x129B,         L"Image Access"},
  {0x129D,         L"CompCore Multimedia Inc."},
  {0x129E,         L"Victor Co. of Japan Ltd."},
  {0x129F,         L"OEC Medical Systems Inc."},
  {0x12A0,         L"Allen Bradley Co."},
  {0x12A1,         L"Simpact Inc"},
  {0x12A2,         L"NewGen Systems Corp."},
  {0x12A3,         L"Lucent Technologies AMR"},
  {0x12A4,         L"NTT Electronics Corp."},
  {0x12A5,         L"Vision Dynamics Ltd."},
  {0x12A6,         L"Scalable Networks Inc."},
  {0x12A7,         L"AMO GmbH"},
  {0x12A8,         L"News Datacom"},
  {0x12A9,         L"Xiotech Corp."},
  {0x12AA,         L"SDL Communications Inc."},
  {0x12AB,         L"Yuan Yuan Enterprise Co. Ltd."},
  {0x12AC,         L"MeasureX Corp."},
  {0x12AD,         L"MULTIDATA GmbH"},
  {0x12AE,         L"Alteon Networks Inc."},
  {0x12AF,         L"TDK USA Corp."},
  {0x12B0,         L"Jorge Scientific Corp."},
  {0x12B1,         L"GammaLink"},
  {0x12B2,         L"General Signal Networks"},
  {0x12B3,         L"Interface Corp. Ltd."},
  {0x12B4,         L"Future Tel Inc."},
  {0x12B5,         L"Granite Systems Inc."},
  {0x12B7,         L"Acumen"},
  {0x12B8,         L"Korg"},
  {0x12B9,         L"3Com Corporation"},
  {0x12BA,         L"Bittware, Inc"},
  {0x12BB,         L"Nippon Unisoft Corp."},
  {0x12BC,         L"Array Microsystems"},
  {0x12BD,         L"Computerm Corp."},
  {0x12BF,         L"Fujifilm Microdevices"},
  {0x12C0,         L"Infimed"},
  {0x12C1,         L"GMM Research Corp."},
  {0x12C2,         L"Mentec Ltd."},
  {0x12C3,         L"Holtek Microelectronics Inc."},
  {0x12C4,         L"Connect Tech Inc."},
  {0x12C5,         L"Picture Elements Inc."},
  {0x12C6,         L"Mitani Corp."},
  {0x12C7,         L"Dialogic Corp."},
  {0x12C8,         L"G Force Co. Ltd."},
  {0x12C9,         L"Gigi Operations"},
  {0x12CA,         L"Integrated Computing Engines, Inc."},
  {0x12CB,         L"Antex Electronics Corp."},
  {0x12CC,         L"Pluto Technologies International"},
  {0x12CD,         L"Aims Lab"},
  {0x12CE,         L"Netspeed Inc."},
  {0x12CF,         L"Prophet Systems Inc."},
  {0x12D0,         L"GDE Systems Inc."},
  {0x12D1,         L"Huawei Technologies Co., Ltd."},
  {0x12D3,         L"Vingmed Sound A/S"},
  {0x12D4,         L"Ulticom, Inc."},
  {0x12D5,         L"Equator Technologies"},
  {0x12D6,         L"Analogic Corp."},
  {0x12D7,         L"Biotronic SRL"},
  {0x12D8,         L"Pericom Semiconductor"},
  {0x12D9,         L"Aculab Plc."},
  {0x12DA,         L"TrueTime"},
  {0x12DB,         L"Annapolis Micro Systems Inc."},
  {0x12DC,         L"Symicron Computer Communication Ltd."},
  {0x12DD,         L"Management Graphics Inc."},
  {0x12DE,         L"Rainbow Technologies"},
  {0x12DF,         L"SBS Technologies Inc."},
  {0x12E0,         L"Chase Research PLC"},
  {0x12E1,         L"Nintendo Co. Ltd."},
  {0x12E2,         L"Datum Inc. Bancomm-Timing Division"},
  {0x12E3,         L"Imation Corp. - Medical Imaging Syst"},
  {0x12E4,         L"Brooktrout Technology Inc."},
  {0x12E6,         L"Cirel Systems"},
  {0x12E7,         L"Sebring Systems Inc"},
  {0x12E8,         L"CRISC Corp."},
  {0x12E9,         L"GE Spacenet"},
  {0x12EB,         L"Aureal Semiconductor"},
  {0x12EC,         L"3A International Inc."},
  {0x12ED,         L"Optivision Inc."},
  {0x12EE,         L"Orange Micro, Inc."},
  {0x12EF,         L"Vienna Systems"},
  {0x12F0,         L"Pentek"},
  {0x12F1,         L"Sorenson Vision Inc."},
  {0x12F2,         L"Gammagraphx Inc."},
  {0x12F4,         L"Megatel"},
  {0x12F5,         L"Forks"},
  {0x12F7,         L"Cognex"},
  {0x12F8,         L"Electronic-Design GmbH"},
  {0x12F9,         L"FourFold Technologies"},
  {0x12FB,         L"Spectrum Signal Processing"},
  {0x12FC,         L"Capital Equipment Corp"},
  {0x12FE,         L"esd Electronic System Design GmbH"},
  {0x1303,         L"Innovative Integration"},
  {0x1304,         L"Juniper Networks Inc."},
  {0x1307,         L"ComputerBoards"},
  {0x1308,         L"Jato Technologies Inc."},
  {0x130A,         L"Mitsubishi Electric Microcomputer"},
  {0x130B,         L"Colorgraphic Communications Corp"},
  {0x130F,         L"Advanet Inc."},
  {0x1310,         L"Gespac"},
  {0x1312,         L"Microscan Systems Inc"},
  {0x1313,         L"Yaskawa Electric Co."},
  {0x1316,         L"Teradyne Inc."},
  {0x1317,         L"ADMtek Inc"},
  {0x1318,         L"Packet Engines, Inc."},
  {0x1319,         L"Forte Media"},
  {0x131F,         L"SIIG"},
  {0x1325,         L"austriamicrosystems"},
  {0x1326,         L"Seachange International"},
  {0x1328,         L"CIFELLI SYSTEMS CORPORATION"},
  {0x1331,         L"RadiSys Corporation"},
  {0x1332,         L"Curtiss-Wright Controls Embedded Computing"},
  {0x1335,         L"Videomail Inc."},
  {0x133D,         L"Prisa Networks"},
  {0x133F,         L"SCM Microsystems"},
  {0x1342,         L"Promax Systems Inc"},
  {0x1344,         L"Micron Technology, Inc."},
  {0x1347,         L"Spectracom Corporation"},
  {0x134A,         L"DTC Technology Corp."},
  {0x134B,         L"ARK Research Corp."},
  {0x134C,         L"Chori Joho System Co. Ltd"},
  {0x134D,         L"PCTEL Inc."},
  {0x135A,         L"Brain Boxes Limited"},
  {0x135B,         L"Giganet Inc."},
  {0x135C,         L"Quatech Inc"},
  {0x135D,         L"ABB Network Partner AB"},
  {0x135E,         L"Sealevel Systems Inc."},
  {0x135F,         L"I-Data International A-S"},
  {0x1360,         L"Meinberg Funkuhren GmbH & Co. KG"},
  {0x1361,         L"Soliton Systems K.K."},
  {0x1363,         L"Phoenix Technologies Ltd"},
  {0x1365,         L"Hypercope Corp."},
  {0x1366,         L"Teijin Seiki Co. Ltd."},
  {0x1367,         L"Hitachi Zosen Corporation"},
  {0x1368,         L"Skyware Corporation"},
  {0x1369,         L"Digigram"},
  {0x136B,         L"Kawasaki Steel Corporation"},
  {0x136C,         L"Adtek System Science Co Ltd"},
  {0x1375,         L"Boeing - Sunnyvale"},
  {0x137A,         L"Mark Of The Unicorn Inc"},
  {0x137B,         L"PPT Vision"},
  {0x137C,         L"Iwatsu Electric Co Ltd"},
  {0x137D,         L"Dynachip Corporation"},
  {0x137E,         L"Patriot Scientific Corp."},
  {0x1380,         L"Sanritz Automation Co LTC"},
  {0x1381,         L"Brains Co. Ltd"},
  {0x1382,         L"Marian - Electronic & Software"},
  {0x1384,         L"Stellar Semiconductor Inc"},
  {0x1385,         L"Netgear"},
  {0x1387,         L"Curtiss-Wright Controls Electronic Systems"},
  {0x1388,         L"Hitachi Information Technology Co Ltd"},
  {0x1389,         L"Applicom International"},
  {0x138A,         L"Validity Sensors, Inc."},
  {0x138B,         L"Tokimec Inc"},
  {0x138E,         L"Basler GMBH"},
  {0x138F,         L"Patapsco Designs Inc"},
  {0x1390,         L"Concept Development Inc."},
  {0x1393,         L"Moxa Technologies Co Ltd"},
  {0x1394,         L"Level One Communications"},
  {0x1395,         L"Ambicom Inc"},
  {0x1396,         L"Cipher Systems Inc"},
  {0x1397,         L"Cologne Chip Designs GmbH"},
  {0x1398,         L"Clarion Co. Ltd"},
  {0x139A,         L"Alacritech Inc"},
  {0x139D,         L"Xstreams PLC/ EPL Limited"},
  {0x139E,         L"Echostar Data Networks"},
  {0x13A0,         L"Crystal Group Inc"},
  {0x13A1,         L"Kawasaki Heavy Industries Ltd"},
  {0x13A3,         L"HI-FN Inc."},
  {0x13A4,         L"Rascom Inc"},
  {0x13A7,         L"amc330"},
  {0x13A8,         L"Exar Corp."},
  {0x13A9,         L"Siemens Healthcare"},
  {0x13AA,         L"Nortel Networks - BWA Division"},
  {0x13AF,         L"T.Sqware"},
  {0x13B1,         L"Tamura Corporation"},
  {0x13B4,         L"Wellbean Co Inc"},
  {0x13B5,         L"ARM Ltd"},
  {0x13B6,         L"DLoG Gesellschaft f锟斤拷r elektronische Datentechnik mbH"},
  {0x13B8,         L"Nokia Telecommunications OY"},
  {0x13BD,         L"Sharp Corporation"},
  {0x13BF,         L"Sharewave Inc"},
  {0x13C0,         L"Microgate Corp."},
  {0x13C1,         L"LSI"},
  {0x13C2,         L"Technotrend Systemtechnik GMBH"},
  {0x13C3,         L"Janz Computer AG"},
  {0x13C7,         L"Blue Chip Technology Ltd"},
  {0x13CC,         L"Metheus Corporation"},
  {0x13CF,         L"Studio Audio & Video Ltd"},
  {0x13D0,         L"B2C2 Inc"},
  {0x13D1,         L"AboCom Systems, Inc"},
  {0x13D4,         L"Graphics Microsystems Inc"},
  {0x13D6,         L"K.I. Technology Co Ltd"},
  {0x13D7,         L"Toshiba Engineering Corporation"},
  {0x13D8,         L"Phobos Corporation"},
  {0x13D9,         L"Apex Inc"},
  {0x13DC,         L"Netboost Corporation"},
  {0x13DE,         L"ABB Robotics Products AB"},
  {0x13DF,         L"E-Tech Inc."},
  {0x13E0,         L"GVC Corporation"},
  {0x13E3,         L"Nest Inc"},
  {0x13E4,         L"Calculex Inc"},
  {0x13E5,         L"Telesoft Design Ltd"},
  {0x13E9,         L"Intraserver Technology Inc"},
  {0x13EA,         L"Dallas Semiconductor"},
  {0x13F0,         L"IC Plus Corporation"},
  {0x13F1,         L"OCE - Industries S.A."},
  {0x13F4,         L"Troika Networks Inc"},
  {0x13F6,         L"C-Media Electronics Inc."},
  {0x13F9,         L"NTT Advanced Technology Corp."},
  {0x13FA,         L"Pentland Systems Ltd."},
  {0x13FB,         L"Aydin Corp"},
  {0x13FD,         L"Micro Science Inc"},
  {0x13FE,         L"Advantech Co., Ltd."},
  {0x13FF,         L"Silicon Spice Inc."},
  {0x1400,         L"ArtX Inc"},
  {0x1402,         L"Meilhaus Electronic GmbH Germany"},
  {0x1404,         L"Fundamental Software Inc"},
  {0x1406,         L"Oce Print Logics Technologies S.A."},
  {0x1407,         L"Lava Computer MFG Inc."},
  {0x1408,         L"Aloka Co. Ltd"},
  {0x1409,         L"SUNIX Co., Ltd."},
  {0x140A,         L"DSP Research Inc"},
  {0x140B,         L"Ramix Inc"},
  {0x140D,         L"Matsushita Electric Works Ltd"},
  {0x140F,         L"Salient Systems Corp"},
  {0x1412,         L"IC Ensemble, Inc."},
  {0x1413,         L"Addonics"},
  {0x1415,         L"Oxford Semiconductor Ltd - now part of PLX Technology"},
  {0x1418,         L"Kyushu Electronics Systems Inc"},
  {0x1419,         L"Excel Switching Corp"},
  {0x141B,         L"Zoom Telephonics Inc"},
  {0x141E,         L"Fanuc Co. Ltd"},
  {0x141F,         L"Visiontech Ltd"},
  {0x1420,         L"Psion Dacom PLC"},
  {0x1425,         L"Chelsio Communications"},
  {0x1428,         L"Edec Co Ltd"},
  {0x1429,         L"Unex Technology Corp."},
  {0x142A,         L"Kingmax Technology Inc"},
  {0x142B,         L"Radiolan"},
  {0x142C,         L"Minton Optic Industry Co Ltd"},
  {0x142D,         L"Pixstream Inc"},
  {0x1430,         L"ITT Aerospace/Communications Division"},
  {0x1433,         L"Eltec Elektronik AG"},
  {0x1435,         L"RTD Embedded Technologies, Inc."},
  {0x1436,         L"CIS Technology Inc"},
  {0x1437,         L"Nissin Inc Co"},
  {0x1438,         L"Atmel-Dream"},
  {0x143F,         L"Lightwell Co Ltd - Zax Division"},
  {0x1441,         L"Agie SA."},
  {0x1443,         L"Unibrain S.A."},
  {0x1445,         L"Logical Co Ltd"},
  {0x1446,         L"Graphin Co., LTD"},
  {0x1447,         L"Aim GMBH"},
  {0x1448,         L"Alesis Studio"},
  {0x144A,         L"ADLINK Technology Inc"},
  {0x144B,         L"Loronix Information Systems, Inc."},
  //{0x144D,         L"sanyo"},
  {0x1450,         L"Octave Communications Ind."},
  {0x1451,         L"SP3D Chip Design GMBH"},
  {0x1453,         L"Mycom Inc"},
  {0x1458,         L"Giga-Byte Technologies"},
  {0x145C,         L"Cryptek"},
  {0x145F,         L"Baldor Electric Company"},
  {0x1460,         L"Dynarc Inc"},
  {0x1462,         L"Micro-Star International Co Ltd"},
  {0x1463,         L"Fast Corporation"},
  {0x1464,         L"Interactive Circuits & Systems Ltd"},
  {0x1468,         L"Ambit Microsystems Corp."},
  {0x1469,         L"Cleveland Motion Controls"},
  {0x146C,         L"Ruby Tech Corp."},
  {0x146D,         L"Tachyon Inc."},
  {0x146E,         L"WMS Gaming"},
  {0x1471,         L"Integrated Telecom Express Inc"},
  {0x1473,         L"Zapex Technologies Inc"},
  {0x1474,         L"Doug Carson & Associates"},
  {0x1477,         L"Net Insight"},
  {0x1478,         L"Diatrend Corporation"},
  {0x147B,         L"Abit Computer Corp."},
  {0x147F,         L"Nihon Unisys Ltd."},
  {0x1482,         L"Isytec - Integrierte Systemtechnik Gmbh"},
  {0x1483,         L"Labway Coporation"},
  {0x1485,         L"Erma - Electronic GMBH"},
  {0x1489,         L"KYE Systems Corporation"},
  {0x148A,         L"Opto 22"},
  {0x148B,         L"Innomedialogic Inc."},
  {0x148C,         L"C.P. Technology Co. Ltd"},
  {0x148D,         L"Digicom Systems Inc."},
  {0x148E,         L"OSI Plus Corporation"},
  {0x148F,         L"Plant Equipment Inc."},
  {0x1490,         L"TC Labs Pty Ltd."},
  {0x1491,         L"Futronic"},
  {0x1493,         L"Maker Communications"},
  {0x1495,         L"Tokai Communications Industry Co. Ltd"},
  {0x1496,         L"Joytech Computer Co. Ltd."},
  {0x1497,         L"SMA Technologie AG"},
  {0x1498,         L"Tews Technologies"},
  {0x1499,         L"Micro-Technology Co Ltd"},
  {0x149A,         L"Andor Technology Ltd"},
  {0x149B,         L"Seiko Instruments Inc"},
  {0x149E,         L"Mapletree Networks Inc."},
  {0x149F,         L"Lectron Co Ltd"},
  {0x14A0,         L"Softing AG"},
  {0x14A2,         L"Millennium Engineering Inc"},
  //{0x14A4,         L"GVC/BCM Advanced Research"},
  {0x14A9,         L"Hivertec Inc."},
  {0x14AB,         L"Mentor Graphics Corp."},
  {0x14B1,         L"Nextcom K.K."},
  {0x14B3,         L"Xpeed Inc."},
  {0x14B4,         L"Philips Business Electronics B.V."},
  {0x14B5,         L"Creamware GmbH"},
  {0x14B6,         L"Quantum Data Corp."},
  {0x14B7,         L"Proxim Inc."},
  {0x14B9,         L"Aironet Wireless Communication"},
  {0x14BA,         L"Internix Inc."},
  {0x14BB,         L"Semtech Corporation"},
  {0x14BE,         L"L3 Communications"},
  {0x14C0,         L"Compal Electronics, Inc."},
  {0x14C1,         L"Myricom Inc."},
  {0x14C2,         L"DTK Computer"},
  {0x14C4,         L"Iwasaki Information Systems Co Ltd"},
  {0x14C5,         L"ABB AB (Sweden)"},
  {0x14C6,         L"Data Race Inc"},
  {0x14C7,         L"Modular Technology Ltd."},
  {0x14C8,         L"Turbocomm Tech Inc"},
  {0x14C9,         L"Odin Telesystems Inc"},
  {0x14CB,         L"Billionton Systems Inc./Cadmus Micro Inc"},
  {0x14CD,         L"Universal Scientific Ind."},
  {0x14CF,         L"TEK Microsystems Inc."},
  {0x14D4,         L"Panacom Technology Corporation"},
  {0x14D5,         L"Nitsuko Corporation"},
  {0x14D6,         L"Accusys Inc"},
  {0x14D7,         L"Hirakawa Hewtech Corp"},
  {0x14D8,         L"Hopf Elektronik GMBH"},
  {0x14D9,         L"Alpha Processor Inc"},
  {0x14DB,         L"Avlab Technology Inc."},
  {0x14DC,         L"Amplicon Liveline Limited"},
  {0x14DD,         L"Imodl Inc."},
  {0x14DE,         L"Applied Integration Corporation"},
  {0x14E3,         L"Amtelco"},
  {0x14E4,         L"Broadcom"},
  {0x14EA,         L"Planex Communications, Inc."},
  {0x14EB,         L"Seiko Epson Corporation"},
  {0x14EC,         L"Acqiris"},
  {0x14ED,         L"Datakinetics Ltd"},
  {0x14EF,         L"Carry Computer Eng. Co Ltd"},
  {0x14F1,         L"Conexant"},
  {0x14F2,         L"Mobility Electronics, Inc."},
  {0x14F4,         L"Tokyo Electronic Industry Co. Ltd."},
  {0x14F5,         L"Sopac Ltd"},
  {0x14F6,         L"Coyote Technologies LLC"},
  {0x14F7,         L"Wolf Technology Inc"},
  {0x14F8,         L"Audiocodes Inc"},
  {0x14F9,         L"AG Communications"},
  {0x14FB,         L"Transas Marine (UK) Ltd"},
  {0x14FC,         L"Quadrics Ltd"},
  {0x14FD,         L"Silex Technology Inc."},
  {0x14FE,         L"Archtek Telecom Corp."},
  {0x14FF,         L"Twinhead International Corp."},
  {0x1501,         L"Banksoft Canada Ltd"},
  {0x1502,         L"Mitsubishi Electric Logistics Support Co"},
  {0x1503,         L"Kawasaki LSI USA Inc"},
  {0x1504,         L"Kaiser Electronics"},
  {0x1506,         L"Chameleon Systems Inc"},
  {0x1507,         L"Htec Ltd."},
  {0x1509,         L"First International Computer Inc"},
  {0x150B,         L"Yamashita Systems Corp"},
  {0x150C,         L"Kyopal Co Ltd"},
  {0x150D,         L"Warpspped Inc"},
  {0x150E,         L"C-Port Corporation"},
  {0x150F,         L"Intec GMBH"},
  {0x1510,         L"Behavior Tech Computer Corp"},
  {0x1511,         L"Centillium Technology Corp"},
  {0x1512,         L"Rosun Technologies Inc"},
  {0x1513,         L"Raychem"},
  {0x1514,         L"TFL LAN Inc"},
  {0x1515,         L"ICS Advent"},
  {0x1516,         L"Myson Technology Inc"},
  {0x1517,         L"Echotek Corporation"},
  {0x1518,         L"Kontron Modular Computers GmbH (PEP Modular Computers GMBH)"},
  {0x1519,         L"Telefon Aktiebolaget LM Ericsson"},
  {0x151A,         L"Globetek Inc."},
  {0x151B,         L"Combox Ltd"},
  {0x151C,         L"Digital Audio Labs Inc"},
  {0x151D,         L"Fujitsu Computer Products Of America"},
  {0x151E,         L"Matrix Corp."},
  {0x151F,         L"Topic Semiconductor Corp"},
  {0x1520,         L"Chaplet System Inc"},
  {0x1521,         L"Bell Corporation"},
  {0x1522,         L"Mainpine Limited"},
  {0x1523,         L"Music Semiconductors"},
  {0x1524,         L"ENE Technology Inc"},
  {0x1525,         L"Impact Technologies"},
  {0x1526,         L"ISS Inc"},
  {0x1527,         L"Solectron"},
  {0x1528,         L"Acksys"},
  {0x1529,         L"American Microsystems Inc"},
  {0x152A,         L"Quickturn Design Systems"},
  {0x152B,         L"Flytech Technology Co Ltd"},
  {0x152C,         L"Macraigor Systems LLC"},
  {0x152D,         L"Quanta Computer Inc"},
  {0x152E,         L"Melec Inc"},
  {0x152F,         L"Philips - Crypto"},
  {0x1532,         L"Echelon Corporation"},
  {0x1533,         L"Baltimore"},
  {0x1534,         L"Road Corporation"},
  {0x1535,         L"Evergreen Technologies Inc"},
  {0x1537,         L"Datalex Communcations"},
  {0x1538,         L"Aralion Inc."},
  {0x1539,         L"Atelier Informatiques et Electronique Et"},
  {0x153A,         L"ONO Sokki"},
  {0x153B,         L"Terratec Electronic GMBH"},
  {0x153C,         L"Antal Electronic"},
  {0x153D,         L"Filanet Corporation"},
  {0x153E,         L"Techwell Inc"},
  {0x153F,         L"MIPS Technologies, Inc"},
  {0x1540,         L"Provideo Multimedia Co Ltd"},
  {0x1541,         L"Telocity Inc."},
  {0x1542,         L"Vivid Technology Inc"},
  {0x1543,         L"Silicon Laboratories"},
  {0x1544,         L"DCM Technologies Ltd."},
  {0x1545,         L"VisionTek"},
  {0x1546,         L"IOI Technology Corp."},
  {0x1547,         L"Mitutoyo Corporation"},
  {0x1548,         L"Jet Propulsion Laboratory"},
  {0x1549,         L"Interconnect Systems Solutions"},
  {0x154A,         L"Max Technologies Inc."},
  {0x154B,         L"Computex Co Ltd"},
  {0x154C,         L"Visual Technology Inc."},
  {0x154D,         L"PAN International Industrial Corp"},
  {0x154E,         L"Servotest Ltd"},
  {0x154F,         L"Stratabeam Technology"},
  {0x1550,         L"Open Network Co Ltd"},
  {0x1551,         L"Smart Electronic Development GMBH"},
  {0x1553,         L"Chicony Electronics Co Ltd"},
  {0x1554,         L"Prolink Microsystems Corp."},
  {0x1555,         L"Gesytec GmbH"},
  {0x1556,         L"PLDA"},
  {0x1557,         L"Mediastar Co. Ltd"},
  {0x1558,         L"Clevo/Kapok Computer"},
  {0x1559,         L"SI Logic Ltd"},
  {0x155A,         L"Innomedia Inc"},
  {0x155B,         L"Protac International Corp"},
  {0x155C,         L"s"},
  {0x155D,         L"MAC System Co Ltd"},
  {0x155E,         L"KUKA Roboter GmbH"},
  {0x155F,         L"Perle Systems Limited"},
  {0x1560,         L"Terayon Communications Systems"},
  {0x1561,         L"Viewgraphics Inc"},
  {0x1562,         L"Symbol Technologies, Inc."},
  {0x1563,         L"A-Trend Technology Co Ltd"},
  {0x1564,         L"Yamakatsu Electronics Industry Co Ltd"},
  {0x1565,         L"Biostar Microtech Intl Corp"},
  {0x1566,         L"Ardent Technologies Inc"},
  {0x1567,         L"Jungsoft"},
  {0x1568,         L"DDK Electronics Inc"},
  {0x1569,         L"Palit Microsystems Inc"},
  {0x156A,         L"Avtec Systems Inc"},
  {0x156B,         L"S2io Inc"},
  {0x156C,         L"Vidac Electronics GMBH"},
  {0x156D,         L"Alpha-Top Corp"},
  {0x156E,         L"Alfa Inc."},
  {0x156F,         L"M-Systems Flash Disk Pioneers Ltd"},
  {0x1570,         L"Lecroy Corporation"},
  {0x1571,         L"Contemporary Controls"},
  {0x1572,         L"Otis Elevator Company"},
  {0x1573,         L"Lattice - Vantis"},
  {0x1574,         L"Fairchild Semiconductor"},
  {0x1575,         L"Voltaire Advanced Data Security Ltd"},
  {0x1576,         L"Viewcast Com"},
  {0x1578,         L"Hitt"},
  {0x1579,         L"Dual Technology Corporation"},
  {0x157A,         L"Japan Elecronics Ind. Inc"},
  {0x157B,         L"Star Multimedia Corp."},
  {0x157C,         L"Eurosoft (UK)"},
  {0x157D,         L"Gemflex Networks"},
  {0x157E,         L"Transition Networks"},
  {0x157F,         L"PX Instruments Technology Ltd"},
  {0x1580,         L"Primex Aerospace Co."},
  {0x1581,         L"SEH Computertechnik GMBH"},
  {0x1582,         L"Cytec Corporation"},
  {0x1583,         L"Inet Technologies Inc"},
  {0x1584,         L"Vetronix Corporation Engenharia Ltda"},
  {0x1585,         L"Marconi Commerce Systems SRL"},
  {0x1586,         L"Lancast Inc"},
  {0x1587,         L"Konica Corporation"},
  {0x1588,         L"Solidum Systems Corp"},
  {0x1589,         L"Atlantek Microsystems Pty Ltd"},
  {0x158A,         L"Digalog Systems Inc"},
  {0x158B,         L"Allied Data Technologies"},
  {0x158C,         L"Hitachi Semiconductor & Devices Sales Co"},
  {0x158D,         L"Point Multimedia Systems"},
  {0x158E,         L"Lara Technology Inc"},
  {0x158F,         L"Ditect Coop"},
  {0x1590,         L"3pardata Inc."},
  {0x1591,         L"ARN"},
  {0x1592,         L"Syba Tech Ltd."},
  {0x1593,         L"Bops Inc"},
  {0x1594,         L"Netgame Ltd"},
  {0x1595,         L"Diva Systems Corp."},
  {0x1596,         L"Folsom Research Inc"},
  {0x1597,         L"Memec Design Services"},
  {0x1598,         L"Granite Microsystems"},
  {0x1599,         L"Delta Electronics Inc"},
  {0x159A,         L"General Instrument"},
  {0x159B,         L"Faraday Technology Corp"},
  {0x159C,         L"Stratus Computer Systems"},
  {0x159D,         L"Ningbo Harrison Electronics Co Ltd"},
  {0x159E,         L"A-Max Technology Co Ltd"},
  {0x159F,         L"Galea Network Security"},
  {0x15A0,         L"Compumaster SRL"},
  {0x15A1,         L"Geocast Network Systems Inc"},
  {0x15A2,         L"Catalyst Enterprises Inc"},
  {0x15A3,         L"Italtel"},
  {0x15A4,         L"X-Net OY"},
  {0x15A5,         L"Toyota MACS Inc"},
  {0x15A6,         L"Sunlight Ultrasound Technologies Ltd"},
  {0x15A7,         L"SSE Telecom Inc"},
  {0x15A8,         L"Shanghai Communications Technologies Cen"},
  {0x15AA,         L"Moreton Bay"},
  {0x15AB,         L"Bluesteel Networks Inc"},
  {0x15AC,         L"North Atlantic Instruments"},
  {0x15AD,         L"VMware Inc."},
  {0x15AE,         L"Amersham Pharmacia Biotech"},
  {0x15B0,         L"Zoltrix International Limited"},
  {0x15B1,         L"Source Technology Inc"},
  {0x15B2,         L"Mosaid Technologies Inc."},
  {0x15B3,         L"Mellanox Technology"},
  {0x15B4,         L"CCI/Triad"},
  {0x15B5,         L"Cimetrics Inc"},
  {0x15B6,         L"Texas Memory Systems Inc"},
  {0x15B7,         L"Sandisk Corp."},
  {0x15B8,         L"Addi-Data GMBH"},
  {0x15B9,         L"Maestro Digital Communications"},
  {0x15BA,         L"Impacct Technology Corp"},
  {0x15BB,         L"Portwell Inc"},
  {0x15BC,         L"Agilent Technologies"},
  {0x15BD,         L"DFI Inc."},
  {0x15BE,         L"Sola Electronics"},
  {0x15BF,         L"High Tech Computer Corp (HTC)"},
  {0x15C0,         L"BVM Limited"},
  {0x15C1,         L"Quantel"},
  {0x15C2,         L"Newer Technology Inc"},
  {0x15C3,         L"Taiwan Mycomp Co Ltd"},
  {0x15C4,         L"EVSX Inc"},
  {0x15C5,         L"Procomp Informatics Ltd"},
  {0x15C6,         L"Technical University Of Budapest"},
  {0x15C7,         L"Tateyama System Laboratory Co Ltd"},
  {0x15C8,         L"Penta Media Co. Ltd"},
  {0x15C9,         L"Serome Technology Inc"},
  {0x15CA,         L"Bitboys OY"},
  {0x15CB,         L"AG Electronics Ltd"},
  {0x15CC,         L"Hotrail Inc."},
  {0x15CD,         L"Dreamtech Co Ltd"},
  {0x15CE,         L"Genrad Inc."},
  {0x15CF,         L"Hilscher GMBH"},
  {0x15D1,         L"Infineon Technologies AG"},
  {0x15D2,         L"FIC (First International Computer Inc)"},
  {0x15D3,         L"NDS Technologies Israel Ltd"},
  {0x15D4,         L"Iwill Corporation"},
  {0x15D5,         L"Tatung Co."},
  {0x15D6,         L"Entridia Corporation"},
  {0x15D7,         L"Rockwell-Collins Inc"},
  {0x15D8,         L"Cybernetics Technology Co Ltd"},
  {0x15D9,         L"Super Micro Computer Inc"},
  {0x15DA,         L"Cyberfirm Inc."},
  {0x15DB,         L"Applied Computing Systems Inc."},
  {0x15DC,         L"Litronic Inc."},
  {0x15DD,         L"Sigmatel Inc."},
  {0x15DE,         L"Malleable Technologies Inc"},
  {0x15E0,         L"Cacheflow Inc"},
  {0x15E1,         L"Voice Technologies Group"},
  {0x15E2,         L"Quicknet Technologies Inc"},
  {0x15E3,         L"Networth Technologies Inc"},
  {0x15E4,         L"VSN Systemen BV"},
  {0x15E5,         L"Valley Technologies Inc"},
  {0x15E6,         L"Agere Inc."},
  {0x15E7,         L"GET Engineering Corp."},
  {0x15E8,         L"National Datacomm Corp."},
  {0x15E9,         L"Pacific Digital Corp."},
  {0x15EA,         L"Tokyo Denshi Sekei K.K."},
  {0x15EB,         L"Drsearch GMBH"},
  {0x15EC,         L"Beckhoff Automation GmbH"},
  {0x15ED,         L"Macrolink Inc"},
  {0x15EE,         L"IN Win Development Inc."},
  {0x15EF,         L"Intelligent Paradigm Inc"},
  {0x15F0,         L"B-Tree Systems Inc"},
  {0x15F1,         L"Times N Systems Inc"},
  {0x15F2,         L"SPOT Imaging Solutions a division of Diagnostic Instruments, Inc"},
  {0x15F3,         L"Digitmedia Corp."},
  {0x15F4,         L"Valuesoft"},
  {0x15F5,         L"Power Micro Research"},
  {0x15F6,         L"Extreme Packet Device Inc"},
  {0x15F7,         L"Banctec"},
  {0x15F8,         L"Koga Electronics Co"},
  {0x15F9,         L"Zenith Electronics Co"},
  {0x15FA,         L"Axzam Corporation"},
  {0x15FB,         L"Zilog Inc."},
  {0x15FC,         L"Techsan Electronics Co Ltd"},
  {0x15FD,         L"N-Cubed.Net"},
  {0x15FE,         L"Kinpo Electronics Inc"},
  {0x15FF,         L"Fastpoint Technologies Inc."},
  {0x1600,         L"Northrop Grumman - Canada Ltd"},
  {0x1601,         L"Tenta Technology"},
  {0x1602,         L"Prosys-TEC Inc."},
  {0x1603,         L"Nokia Wireless Business Communications"},
  {0x1604,         L"Central System Research Co Ltd"},
  {0x1605,         L"Pairgain Technologies"},
  {0x1606,         L"Europop AG"},
  {0x1607,         L"Lava Semiconductor Manufacturing Inc."},
  {0x1608,         L"Automated Wagering International"},
  {0x1609,         L"Sciemetric Instruments Inc"},
  {0x160A,         L"Kollmorgen Servotronix"},
  {0x160B,         L"Onkyo Corp."},
  {0x160C,         L"Oregon Micro Systems Inc."},
  {0x160D,         L"Aaeon Electronics Inc"},
  {0x160E,         L"CML Emergency Services"},
  {0x160F,         L"ITEC Co Ltd"},
  {0x1610,         L"Tottori Sanyo Electric Co Ltd"},
  {0x1611,         L"Bel Fuse Inc."},
  {0x1612,         L"Telesynergy Research Inc."},
  {0x1613,         L"System Craft Inc."},
  {0x1614,         L"Jace Tech Inc."},
  {0x1615,         L"Equus Computer Systems Inc"},
  {0x1616,         L"Iotech Inc."},
  {0x1617,         L"Rapidstream Inc"},
  {0x1618,         L"Esec SA"},
  {0x1619,         L"FarSite Communications Limited"},
  {0x161B,         L"Mobilian Israel Ltd"},
  {0x161C,         L"Berkshire Products"},
  {0x161D,         L"Gatec"},
  {0x161E,         L"Kyoei Sangyo Co Ltd"},
  {0x161F,         L"Arima Computer Corporation"},
  {0x1620,         L"Sigmacom Co Ltd"},
  {0x1621,         L"Lynx Studio Technology Inc"},
  {0x1622,         L"Nokia Home Communications"},
  {0x1623,         L"KRF Tech Ltd"},
  {0x1624,         L"CE Infosys GMBH"},
  {0x1625,         L"Warp Nine Engineering"},
  {0x1626,         L"TDK Semiconductor Corp."},
  {0x1627,         L"BCom Electronics Inc"},
  {0x1629,         L"Kongsberg Spacetec a.s."},
  {0x162A,         L"Sejin Computerland Co Ltd"},
  {0x162B,         L"Shanghai Bell Company Limited"},
  {0x162C,         L"C&H Technologies Inc"},
  {0x162D,         L"Reprosoft Co Ltd"},
  {0x162E,         L"Margi Systems Inc"},
  {0x162F,         L"Rohde & Schwarz GMBH & Co KG"},
  {0x1630,         L"Sky Computers Inc"},
  {0x1631,         L"NEC Computer International"},
  {0x1632,         L"Verisys Inc"},
  {0x1633,         L"Adac Corporation"},
  {0x1634,         L"Visionglobal Network Corp."},
  {0x1635,         L"Decros / S.ICZ a.s."},
  {0x1636,         L"Jean Company Ltd"},
  {0x1637,         L"NSI"},
  {0x1638,         L"Eumitcom Technology Inc"},
  {0x163A,         L"Air Prime Inc"},
  {0x163B,         L"Glotrex Co Ltd"},
  {0x163C,         L"intel"},
  {0x163D,         L"Heidelberg Digital LLC"},
  {0x163E,         L"3dpower"},
  {0x163F,         L"Renishaw PLC"},
  {0x1640,         L"Intelliworxx Inc"},
  {0x1641,         L"MKNet Corporation"},
  {0x1642,         L"Bitland"},
  {0x1643,         L"Hajime Industries Ltd"},
  {0x1644,         L"Western Avionics Ltd"},
  {0x1645,         L"Quick-Serv. Computer Co. Ltd"},
  {0x1646,         L"Nippon Systemware Co Ltd"},
  {0x1647,         L"Hertz Systemtechnik GMBH"},
  {0x1648,         L"MeltDown Systems LLC"},
  {0x1649,         L"Jupiter Systems"},
  {0x164A,         L"Aiwa Co. Ltd"},
  {0x164C,         L"Department Of Defense"},
  {0x164D,         L"Ishoni Networks"},
  {0x164E,         L"Micrel Inc."},
  {0x164F,         L"Datavoice (Pty) Ltd."},
  {0x1650,         L"Admore Technology Inc."},
  {0x1651,         L"Chaparral Network Storage"},
  {0x1652,         L"Spectrum Digital Inc."},
  {0x1653,         L"Nature Worldwide Technology Corp"},
  {0x1654,         L"Sonicwall Inc"},
  {0x1655,         L"Dazzle Multimedia Inc."},
  {0x1656,         L"Insyde Software Corp"},
  {0x1657,         L"Brocade Communications Systems"},
  {0x1658,         L"Med Associates Inc."},
  {0x1659,         L"Shiba Denshi Systems Inc."},
  {0x165A,         L"Epix Inc."},
  {0x165B,         L"Real-Time Digital Inc."},
  {0x165C,         L"Kondo Kagaku"},
  {0x165D,         L"Hsing Tech. Enterprise Co. Ltd."},
  {0x165E,         L"Hyunju Computer Co. Ltd."},
  {0x165F,         L"Comartsystem Korea"},
  {0x1660,         L"Network Security Technologies Inc. (NetSec)"},
  {0x1661,         L"Worldspace Corp."},
  {0x1662,         L"Int Labs"},
  {0x1663,         L"Elmec Inc. Ltd."},
  {0x1664,         L"Fastfame Technology Co. Ltd."},
  {0x1665,         L"Edax Inc."},
  {0x1666,         L"Norpak Corporation"},
  {0x1667,         L"CoSystems Inc."},
  {0x1668,         L"Actiontec Electronics Inc."},
  {0x166A,         L"Komatsu Ltd."},
  {0x166B,         L"Supernet Inc."},
  {0x166C,         L"Shade Ltd."},
  {0x166D,         L"Sibyte Inc."},
  {0x166E,         L"Schneider Automation Inc."},
  {0x166F,         L"Televox Software Inc."},
  {0x1670,         L"Rearden Steel"},
  {0x1671,         L"Atan Technology Inc."},
  {0x1672,         L"Unitec Co. Ltd."},
  {0x1673,         L"pctel"},
  {0x1675,         L"Square Wave Technology"},
  {0x1676,         L"Emachines Inc."},
  {0x1677,         L"Bernecker + Rainer"},
  {0x1678,         L"INH Semiconductor"},
  {0x1679,         L"Tokyo Electron Device Ltd."},
  {0x167F,         L"iba AG"},
  {0x1680,         L"Dunti Corp."},
  {0x1681,         L"Hercules"},
  {0x1682,         L"PINE Technology, Ltd."},
  {0x1688,         L"CastleNet Technology Inc."},
  {0x168A,         L"Utimaco Safeware AG"},
  {0x168B,         L"Circut Assembly Corp."},
  {0x168C,         L"Atheros Communications Inc."},
  {0x168D,         L"NMI Electronics Ltd."},
  {0x168E,         L"Hyundai MultiCAV Computer Co. Ltd."},
  {0x168F,         L"KDS Innotech Corp."},
  {0x1690,         L"NetContinuum, Inc."},
  {0x1693,         L"FERMA"},
  {0x1695,         L"EPoX Computer Co., Ltd."},
  {0x16AE,         L"SafeNet Inc."},
  {0x16B3,         L"CNF Mobile Solutions"},
  {0x16B8,         L"Sonnet Technologies, Inc."},
  {0x16CA,         L"Cenatek Inc."},
  {0x16CB,         L"Minolta Co. Ltd."},
  {0x16CC,         L"Inari Inc."},
  {0x16D0,         L"Systemax"},
  {0x16E0,         L"Third Millenium Test Solutions, Inc."},
  {0x16E5,         L"Intellon Corporation"},
  {0x16EC,         L"U.S. Robotics"},
  {0x16F0,         L"LaserLinc Inc."},
  {0x16F1,         L"Adicti Corp."},
  {0x16F3,         L"Jetway Information Co., Ltd"},
  {0x16F6,         L"VideoTele.com Inc."},
  {0x1700,         L"Antara LLC"},
  {0x1701,         L"Interactive Computer Products Inc."},
  {0x1702,         L"Internet Machines Corp."},
  {0x1703,         L"Desana Systems"},
  {0x1704,         L"Clearwater Networks"},
  {0x1705,         L"Digital First"},
  {0x1706,         L"Pacific Broadband Communications"},
  {0x1707,         L"Cogency Semiconductor Inc."},
  {0x1708,         L"Harris Corp."},
  {0x1709,         L"Zarlink Semiconductor"},
  {0x170A,         L"Alpine Electronics Inc."},
  {0x170B,         L"NetOctave Inc."},
  {0x170C,         L"YottaYotta Inc."},
  {0x170D,         L"SensoMotoric Instruments GmbH"},
  {0x170E,         L"San Valley Systems, Inc."},
  {0x170F,         L"Cyberdyne Inc."},
  {0x1710,         L"Pelago Networks"},
  {0x1711,         L"MyName Technologies, Inc."},
  {0x1712,         L"NICE Systems Inc."},
  {0x1713,         L"TOPCON Corp."},
  {0x1725,         L"Vitesse Semiconductor"},
  {0x1734,         L"Fujitsu-Siemens Computers GmbH"},
  {0x1737,         L"LinkSys"},
  {0x173B,         L"Altima Communications Inc."},
  {0x1743,         L"Peppercon AG"},
  {0x174B,         L"PC Partner Limited"},
  {0x1752,         L"Global Brands Manufacture Ltd."},
  {0x1753,         L"TeraRecon, Inc."},
  {0x1755,         L"Alchemy Semiconductor Inc."},
  {0x176A,         L"General Dynamics Canada"},
  {0x1775,         L"General Electric"},
  {0x1789,         L"Ennyah Technologies Corp"},
  {0x1793,         L"Unitech Electronics Co., Ltd"},
  {0x17A1,         L"Tascorp"},
  {0x17A7,         L"Start Network Technology Co., Ltd."},
  {0x17AA,         L"Legend Ltd. (Beijing)"},
  {0x17AB,         L"Phillips Components"},
  {0x17AF,         L"Hightech Information Systems, Ltd."},
  {0x17BE,         L"Philips Semiconductors"},
  {0x17C0,         L"Wistron Corp."},
  {0x17C4,         L"Movita"},
  {0x17CC,         L"NetChip"},
  {0x17cd,         L"Cadence Design Systems"},
  {0x17D5,         L"Neterion Inc."},
  {0x17db,         L"Cray, Inc."},
  {0x17E9,         L"DH electronics GmbH / Sabrent"},
  {0x17EE,         L"Connect Components, Ltd."},
  {0x17F3,         L"RDC Semiconductor Co., Ltd."},
  {0x17FE,         L"INPROCOMM"},
  {0x1813,         L"Ambient Technologies Inc"},
  {0x1814,         L"Ralink Technology, Corp."},
  {0x1815,         L"devolo AG"},
  {0x1820,         L"InfiniCon Systems, Inc."},
  {0x1824,         L"Avocent"},
  {0x1841,         L"Panda Platinum"},
  {0x1860,         L"Primagraphics Ltd."},
  {0x186C,         L"Humusoft S.R.O"},
  {0x1887,         L"Elan Digital Systems Ltd"},
  {0x1888,         L"Varisys Limited"},
  {0x188D,         L"Millogic Ltd."},
  {0x1890,         L"Egenera, Inc."},
  {0x18BC,         L"Info-Tek Corp."},
  {0x18C9,         L"ARVOO Engineering BV"},
  {0x18CA,         L"XGI Technology Inc"},
  {0x18F1,         L"Spectrum Systementwicklung Microelectronic GmbH"},
  {0x18F4,         L"Napatech A/S"},
  {0x18F7,         L"Commtech, Inc."},
  {0x18FB,         L"Resilience Corporation"},
  {0x1904,         L"Ritmo"},
  {0x1905,         L"WIS Technology, Inc."},
  {0x1910,         L"Seaway Networks"},
  {0x1912,         L"Renesas Electronics"},
  {0x1931,         L"Option NV"},
  {0x1941,         L"Stelar"},
  {0x1954,         L"One Stop Systems, Inc."},
  {0x1969,         L"Atheros Communications"},
  {0x1971,         L"AGEIA Technologies, Inc."},
  {0x197B,         L"JMicron Technology Corp."},
  {0x198a,         L"Nallatech"},
  {0x1991,         L"Topstar Digital Technologies Co., Ltd."},
  {0x19a2,         L"ServerEngines"},
  {0x19A8,         L"DAQDATA GmbH"},
  {0x19AC,         L"Kasten Chase Applied Research"},
  {0x19B6,         L"Mikrotik"},
  {0x19E2,         L"Vector Informatik GmbH"},
  {0x19E3,         L"DDRdrive LLC"},
  {0x1A08,         L"Linux Networx"},
  //{0x1a41,         L"Tilera Corporation"},
  {0x1A42,         L"Imaginant"},
  {0x1B13,         L"Jaton Corporation USA"},
  {0x1B21,         L"ASMedia Technology Inc."},//<Wanxg001-20190415-Change device vendor name to "ASMedia Technology Inc."+>
  {0x1B6F,         L"Etron"},
  {0x1B73,         L"Fresco Logic Inc."},
  {0x1B91,         L"Averna"},
  {0x1BAD,         L"ReFLEX CES"},
  {0x1C0F,         L"Monarch Innovative Technologies Pvt Ltd's"},
  {0x1C32,         L"Highland Technology, Inc."},
  {0x1c39,         L"Thomson Video Networks"},
  {0x1C5F,         L"Memblaze"},
  {0x1CB0,         L"Shannon Systems"},
  {0x1DE1,         L"Tekram"},
  {0x1FCF,         L"Miranda Technologies Ltd."},
  {0x2001,         L"Temporal Research Ltd"},
  //{0x2646,         L"Kingston Technology Co."},
  {0x270F,         L"ChainTek Computer Co. Ltd."},
  {0x2EC1,         L"Zenic Inc"},
  {0x3388,         L"Hint Corp."},
  {0x3411,         L"Quantum Designs (H.K.) Inc."},
  {0x3513,         L"ARCOM Control Systems Ltd."},
  {0x38EF,         L"4links"},
  {0x3D3D,         L"3Dlabs, Inc. Ltd"},
  {0x4005,         L"Avance Logic Inc."},
  {0x4144,         L"Alpha Data"},
  {0x416C,         L"Aladdin Knowledge Systems"},
  {0x4348,         L"wch.cn"},
  {0x4680,         L"UMAX Computer Corp."},
  {0x4843,         L"Hercules Computer Technology"},
  {0x4943,         L"Growth Networks"},
  {0x4954,         L"Integral Technologies"},
  {0x4978,         L"Axil Computer Inc."},
  {0x4C48,         L"Lung Hwa Electronics"},
  {0x4C53,         L"SBS-OR Industrial Computers"},
  {0x4CA1,         L"Seanix Technology Inc"},
  {0x4D51,         L"Mediaq Inc."},
  {0x4D54,         L"Microtechnica Co Ltd"},
  {0x4DDC,         L"ILC Data Device Corp."},
  {0x4E8,         L"Samsung Windows Portable Devices"},
  {0x5053,         L"TBS/Voyetra Technologies"},
  {0x508A,         L"Samsung T10 MP3 Player"},
  {0x5136,         L"S S Technologies"},
  {0x5143,         L"Qualcomm Inc. USA"},
  {0x5333,         L"S3 Graphics Co., Ltd"},
  {0x544C,         L"Teralogic Inc"},
  {0x5555,         L"Genroco Inc."},
  {0x5853,         L"Citrix Systems, Inc."},
  {0x6409,         L"Logitec Corp."},
  {0x6666,         L"Decision Computer International Co."},
  {0x6945,         L"ASMedia Technology Inc."},
  {0x7604,         L"O.N. Electric Co. Ltd."},
  {0x7d1,         L"D-Link Corporation"},
  {0x8080,         L"Xirlink, Inc"},
  //{0x8086,         L"Intel Corporation"},
  {0x8087,         L"Intel"},
  {0x80EE,         L"Oracle Corporation - InnoTek Systemberatung GmbH"},
  {0x8866,         L"T-Square Design Inc."},
  {0x8888,         L"Silicon Magic"},
  {0x8E0E,         L"Computone Corporation"},
  {0x9004,         L"Adaptec Inc"},
  {0x9005,         L"Adaptec Inc"},
  {0x919A,         L"Gigapixel Corp"},
  {0x9412,         L"Holtek"},
  {0x9699,         L"Omni Media Technology Inc."},
  {0x9710,         L"MosChip Semiconductor Technology"},
  {0x9902,         L"StarGen, Inc."},
  {0xA0A0,         L"Aopen Inc."},
  {0xA0F1,         L"Unisys Corporation"},
  {0xA200,         L"NEC Corp."},
  {0xA259,         L"Hewlett Packard"},
  {0xA304,         L"Sony"},
  {0xA727,         L"3com Corporation"},
  {0xAA42,         L"Abekas, Inc"},
  {0xAC1E,         L"Digital Receiver Technology Inc"},
  {0xB1B3,         L"Shiva Europe Ltd."},
  {0xB894,         L"Brown & Sharpe Mfg. Co."},
  {0xBEEF,         L"Mindstream Computing"},
  {0xC001,         L"TSI Telsys"},
  {0xC0A9,         L"Micron/Crucial Technology"},
  {0xC0DE,         L"Motorola"},
  {0xC0FE,         L"Motion Engineering Inc."},
  {0xC622,         L"Hudson Soft Co Ltd"},
  {0xCA50,         L"Varian, Inc"},
  {0xCAFE,         L"Chrysalis-ITS"},
  {0xCCCC,         L"Catapult Communications"},
  {0xD4D4,         L"Curtiss-Wright Controls Embedded Computing"},
  {0xDC93,         L"Dawicontrol"},
  {0xDEAD,         L"Indigita Corporation"},
  {0xDEAF,         L"Middle Digital, Inc"},
  {0xE159,         L"Tiger Jet Network Inc"},
  {0xE4BF,         L"EKF Elektronik GMBH"},
  {0xEA01,         L"Eagle Technology"},
  {0xEABB,         L"Aashima Technology B.V."},
  {0xEACE,         L"Endace Measurement Systems Ltd."},
  {0xECC0,         L"Echo Digital Audio Corporation"},
  {0xEDD8,         L"ARK Logic, Inc"},
  {0xF5F5,         L"F5 Networks Inc."},
  {0xFA57,         L"Interagon AS"},
  {0xBABA,         L"Alibaba Corporation"},
  {0xCABC,         L"Cambricon"},
  {0x1C58,         L"Western Digital"},
  {0x8088,         L"Beijing Wangxun Technology"},
  {0x1cb8,         L"Dawning Information Industry"},
  {0x1D94,         L"Chengdu Higon Integrated Circuit Design Co., Ltd."},
  {0x1DA3,         L"Habana"}, //<Songts001_20190810 Add Habana Goya HL-100 support +>
  {0xFFFF,         L"New Device"}
};

UINT16 NICSPEEDMAP[]=
{//unit 100M
    0,        //Speed_Undefined = 0,
    1,        //Speed_100Mbps   = 1,
    10,       //Speed_1Gbps     = 2,
    20,       //Speed_2Gbps     = 3,
    25,       //Speed_2_5Gbps   = 4,
    40,       //Speed_4Gbps     = 5,
    80,       //Speed_8Gbps     = 6,
    100,      //Speed_10Gbps    = 7,
    160,      //Speed_16Gbps    = 8,
    200,      //Speed_20Gbps    = 9,
    250,      //Speed_25Gbps    = 10,
    400,      //Speed_40Gbps    = 11,
    560,      //Speed_56Gbps    = 12,
    1000,     //Speed_100Gbps   = 13,
    2000,     //Speed_200Gbps   = 14,
    4000,     //Speed_400Gbps   = 15,
    320,      //Speed_32Gbps    = 16,
    500       //Speed_50Gbps    = 17
};
#endif

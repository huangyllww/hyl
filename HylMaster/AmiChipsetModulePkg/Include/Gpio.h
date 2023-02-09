//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
/** @file Gpio.h
    South bridge default GPIO Definition.

**/
//**********************************************************************

#ifndef _SB_GPIO_H              // To Avoid this header get compiled twice
#define _SB_GPIO_H
#ifdef __cplusplus
extern "C" {
#endif

// This is a explanation for the following structure,
// The first value is a 16 bit width for GPIO number.
// The second value is a 16 bit width for GPIO configuration, please refer to
// GPIO.SDL for its definition.

#ifdef GPIO_000
{ 0 , GPIO_000 },
#endif

#ifdef GPIO_001
{ 1 , GPIO_001 },
#endif

#ifdef GPIO_002
{ 2 , GPIO_002 },
#endif

#ifdef GPIO_003
{ 3 , GPIO_003 },
#endif

#ifdef GPIO_004
{ 4 , GPIO_004 },
#endif

#ifdef GPIO_005
{ 5 , GPIO_005 },
#endif

#ifdef GPIO_006
{ 6 , GPIO_006 },
#endif

#ifdef GPIO_007
{ 7 , GPIO_007 },
#endif

#ifdef GPIO_008
{ 8 , GPIO_008 },
#endif

#ifdef GPIO_009
{ 9 , GPIO_009 },
#endif

#ifdef GPIO_010
{ 10 , GPIO_010 },
#endif

#ifdef GPIO_011
{ 11 , GPIO_011 },
#endif

#ifdef GPIO_012
{ 12 , GPIO_012 },
#endif

#ifdef GPIO_013
{ 13 , GPIO_013 },
#endif

#ifdef GPIO_014
{ 14 , GPIO_014 },
#endif

#ifdef GPIO_015
{ 15 , GPIO_015 },
#endif

#ifdef GPIO_016
{ 16 , GPIO_016 },
#endif

#ifdef GPIO_017
{ 17 , GPIO_017 },
#endif

#ifdef GPIO_018
{ 18 , GPIO_018 },
#endif

#ifdef GPIO_019
{ 19 , GPIO_019 },
#endif

#ifdef GPIO_020
{ 20 , GPIO_020 },
#endif

#ifdef GPIO_021
{ 21 , GPIO_021 },
#endif

#ifdef GPIO_022
{ 22 , GPIO_022 },
#endif

#ifdef GPIO_023
{ 23 , GPIO_023 },
#endif

#ifdef GPIO_024
{ 24 , GPIO_024 },
#endif

#ifdef GPIO_025
{ 25 , GPIO_025 },
#endif

#ifdef GPIO_026
{ 26 , GPIO_026 },
#endif

#ifdef GPIO_027
{ 27 , GPIO_027 },
#endif

#ifdef GPIO_028
{ 28 , GPIO_028 },
#endif

#ifdef GPIO_029
{ 29 , GPIO_029 },
#endif

#ifdef GPIO_030
{ 30 , GPIO_030 },
#endif

#ifdef GPIO_031
{ 31 , GPIO_031 },
#endif

#ifdef GPIO_032
{ 32 , GPIO_032 },
#endif

#ifdef GPIO_033
{ 33 , GPIO_033 },
#endif

#ifdef GPIO_034
{ 34 , GPIO_034 },
#endif

#ifdef GPIO_035
{ 35 , GPIO_035 },
#endif

#ifdef GPIO_036
{ 36 , GPIO_036 },
#endif

#ifdef GPIO_037
{ 37 , GPIO_037 },
#endif

#ifdef GPIO_038
{ 38 , GPIO_038 },
#endif

#ifdef GPIO_064
{ 64 , GPIO_064 },
#endif

#ifdef GPIO_065
{ 65 , GPIO_065 },
#endif

#ifdef GPIO_066
{ 66 , GPIO_066 },
#endif

#ifdef GPIO_067
{ 67 , GPIO_067 },
#endif

#ifdef GPIO_068
{ 68 , GPIO_068 },
#endif

#ifdef GPIO_069
{ 69 , GPIO_069 },
#endif

#ifdef GPIO_070
{ 70 , GPIO_070 },
#endif

#ifdef GPIO_071
{ 71 , GPIO_071 },
#endif

#ifdef GPIO_072
{ 72 , GPIO_072 },
#endif

#ifdef GPIO_073
{ 73 , GPIO_073 },
#endif

#ifdef GPIO_076
{ 76 , GPIO_076 },
#endif

#ifdef GPIO_077
{ 77 , GPIO_077 },
#endif

#ifdef GPIO_084
{ 84 , GPIO_084 },
#endif

#ifdef GPIO_085
{ 85 , GPIO_085 },
#endif

#ifdef GPIO_087
{ 87 , GPIO_087 },
#endif

#ifdef GPIO_088
{ 88 , GPIO_088 },
#endif

#ifdef GPIO_089
{ 89 , GPIO_089 },
#endif

#ifdef GPIO_090
{ 90 , GPIO_090 },
#endif

#ifdef GPIO_091
{ 91 , GPIO_091 },
#endif

#ifdef GPIO_092
{ 92 , GPIO_092 },
#endif

#ifdef GPIO_093
{ 93 , GPIO_093 },
#endif

#ifdef GPIO_094
{ 94 , GPIO_094 },
#endif

#ifdef GPIO_095
{ 95 , GPIO_095 },
#endif

#ifdef GPIO_096
{ 96 , GPIO_096 },
#endif

#ifdef GPIO_097
{ 97 , GPIO_097 },
#endif

#ifdef GPIO_098
{ 98 , GPIO_098 },
#endif

#ifdef GPIO_099
{ 99 , GPIO_099 },
#endif

#ifdef GPIO_100
{ 100 , GPIO_100 },
#endif

#ifdef GPIO_101
{ 101 , GPIO_101 },
#endif

#ifdef GPIO_113
{ 113 , GPIO_113 },
#endif

#ifdef GPIO_114
{ 114 , GPIO_114 },
#endif

#ifdef GPIO_115
{ 115 , GPIO_115 },
#endif

#ifdef GPIO_116
{ 116 , GPIO_116 },
#endif

#ifdef GPIO_126
{ 126 , GPIO_126 },
#endif

#ifdef GPIO_129
{ 129 , GPIO_129 },
#endif

#ifdef GPIO_130
{ 130 , GPIO_130 },
#endif

#ifdef GPIO_131
{ 131 , GPIO_131 },
#endif

#ifdef GPIO_132
{ 132 , GPIO_132 },
#endif

{0xffff, 0xffff}, // End of the table.


/*
//****************************************************************************;
//                  GPIO_Pins_Descriptions_Table                              ;
//****************************************************************************;
Preliminary BIOS and Kernel Developer's Guide (BKDG) for AMD Family 16h Models 30h-3Fh Processors
Publication # 52740 Revision: 0.96
Issue Date: Jul 2013

3.30.12.2 IOMux Registers

Example:
16 - 0: USB_OC0_L\1: GEVENT12/AGPIO16\2: LPC_PD\3:SPI_TPM_CS_L
It means the multi-function GPIO_Pin define as bellow
GPIO: 16
Function-0: USB_OC0_L
Function-1: GEVENT12/AGPIO16
Function-2: LPC_PD
Function-3: SPI_TPM_CS_L

===== System Pin Name =====
######################################
GPIO BANK 0
######################################
0 - 0: PWR_BTN_L
1 - 0: SYS_RESET_L\1: GEVENT19/AGPIO1
2 - 0: WAKE_L\1: GEVENT8/AGPIO2
3 - 2: GEVENT2/AGPIO3
4 - 2: GEVENT4/AGPIO4
5 - 0: VGAPwrDn/CPUClkSelStrap\1: GEVENT7/AGPIO5
6 - 1: GEVENT10/AGPIO6
7 - 1: GEVENT11/AGPIO7
8 - 0: LPC_SMI_L\1: AGPIO8
9 - 1: GEVENT22/AGPIO9
10 - 0: AGPIO10\1: S0I3
11 - 0: GEVENT18/AGPIO11\1: USB_OC7_L
12 - 0: IR_LED\1: AGPIO12
13 - 0: USB_OC5_L\1: GEVENT21/AGPIO13
14 - 0: USB_OC6_L\1: GEVENT6/AGPIO14
15 - 0: IR_RX1\1: GEVENT20/AGPIO15
16 - 0: USB_OC0_L\1: GEVENT12/AGPIO16\2: LPC_PD\3:SPI_TPM_CS_L
17 - 0: USB_OC1_L\1: GEVENT13/AGPIO17
18 - 0: USB_OC2_L\1: GEVENT14/AGPIO18
19 - 0: SCL1\1: EGPIO19
20 - 0: SDA1\1: EGPIO20
21 - 0: GEVENT5/AGPIO21\1: LPC_PD_L\2: SPI_TPM_CS_L
22 - 0: LPC_PME_L\1: GEVENT3/AGPIO22
23 - 0: USB_OC4_L\1: IR_RX0\2: GEVENT16/AGPIO23
24 - 0: USB_OC3_L\1: GEVENT15/AGPIO24
25 - 0: SpiCs3B\2: GEVENT17/AGPIO25
26 - 0: AZ_SDIN0\1: EGPIO26
27 - 0: AZ_SDIN1\1: EGPIO27
28 - 0: AZ_SDIN2\1: EGPIO28
29 - 0: AZ_SDIN3\1: EGPIO29
30 - 0: LPCCLK0\1: EGPIO30
31 - 0: LPCCLK1\1: EGPIO31
32 - 1: GEVENT9/EGPIO32
33 - 0: EGPIO33
34 - 0: EGPIO34
35 - 0: SPI_CS2_L\2: EGPIO35
36 - 0: EGPIO36
37 - 0: EGPIO37
38 - 0: ROM_RSTB\1: EGPIO38
######################################
GPIO BANK 1
######################################
64 - 0: LDRQ1\1: CLK_REQ6_L\2: AGPIO64
65 - 0: Sata_IS2\2: AGPIO65
66 - 1: Shutdown\2: AGPIO66
67 - 0: Sata_IS4/ZP4\1: FINOUT3\2: DEVSLP0\3: AGPIO67
68 - 0: FANIN1\1: AGPIO68
69 - 0: FANIN2\1 AGPIO69
70 - 0: Sata_IS5/ZP5\1: FININ3\2: DEVSLP1\3: AGPIO70
71 - 0: CLK_REQ4_L\1: Sata_IS3\2: AGPIO71
72 - 0: AGPIO72
73 - 0: AGPIO72
76 - 0: AGPIO76
77 - 0: AGPIO77
84 - 0: FANIN0\1: EGPIO84
85 - 0: FANOUT0\1: EGPIO85
87 - 0: SERIRQ\1: EGPIO87
88 - 0: LPC_CLKRUN_L\1: EGPIO88
89 - 0: AGPIO89
90 - 0: AGPIO90
91 - 0: SPKR\1: AGPIO91
92 - 0: CLK_REQ0_L\3: AGPIO92
93 - 0: SPCI_GNTB2\1: SD_LED\2: EGPIO93
94 - 0: EGPIO94
95 - 0: EGPIO95
96 - 0: EGPIO96
97 - 0: EGPIO97
98 - 0: EGPIO98
99 - 0: EGPIO99
100 - 0: EGPIO100
101 - 0: EGPIO101
113 - 0: EGPIO113
114 - 0: EGPIO114
115 - 0: CLK_REQ1_L\1: FANOUT4\2: EGPIO115
116 - 0: CLK_REQ2_L\1: FANIN4\2: EGPIO116
126 - 0: GA20\1: GEVENT0/AGPIO126
######################################
GPIO BANK 2
######################################
129 - 0: KBRST_L\1: GEVENT1/AGPIO129
130 - 0: SATA_ACT_L\1: AGPIO130
131 - 0: CLK_REQ3_L\3: EGPIO131
132 - 0: CLK_REQG_L\1 EGPIO132
*/
//***************************************************************************;
// !!Don't change following equates!!                        ;
//***************************************************************************;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

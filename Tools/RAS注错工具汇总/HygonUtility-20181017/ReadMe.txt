                     Copyright(C) All Rights Reserved.

Name: Utility
Version: 1.01
Function description:
1. Provide functions of that H/W (IO/MEM/PCI etc.) access.

=====================================================================================================================
Usage:

  Help list:
=====================================================================================================================
  /?:     help.
===================================================================================================================== 
  /IO:    Io Access;                                   || /MEM: Memory Access;
  /PCI:   Pci Access;                                  || /SMB: Smbus Access;
  /MSR:   MSR Access;                                  || /SMN: NBIOSmn Access;
  /FLASH: Flash r/w operation;                         || /EDID: Get Edid data;
  
  /RI [Addr, ...]: Read Bit;                           || /RB [Addr, ...]: Read Byte;
  /RW [Addr, ...]: Read Word;                          || /RD [Addr, ...]:  Read Dword; 
  /RQ [Addr, ...]: Read Qword;                         || /WI [Addr, ...] [iData]: Write Bit;
  /WB [Addr, ...] [bData]:  Write Byte;                || /WW [Addr, ...] [wData]: Write Word;
  /WD [Addr, ...] [dData]:  Write Dword;               || /WQ [Addr, ...] [qData]: Write Qword;
  /IO /INDEX /RB [CmdPort DataPort] [Offset]: IoIndex Access

 Example:
===================================================================================================================== 
 Type       Command                                  || Description
=====================================================================================================================
 IO Access:
=====================================================================================================================
  Utility /IO /RB 0x80:                              || Read 1-BYTE from IO port 80h.
  Utility /IO /RW 0x80:                              || Read 1-WORD from IO port 80h
  Utility /IO /RD 0x80:                              || Read 1-DWORD from IO port 80h
  Utility /IO /WB 0x80 0x12:                         || Write 1-BYTE 0x12 to IO port 80h
  Utility /IO /WW 0x80 0x1234:                       || Write 1-WORD 0x1234 to IO port 80h
  Utility /IO /WD 0x80 0x12345678:                   || Write 1-DWORD 0x12345678 to IO port 80h
  Utility /IO /DUMP 0x500:                           || Dump IO data from 0x500.
  Utility /IO /INDEX /RB 0x72 0x73 0x10:             || Read 1-BYTE data from IO port 72h/73h 0x10.
  Utility /IO /INDEX /WB 0x72 0x73 0x10 0xFF:        || Write 1-BYTE 0xFF to IO port 72h/73h 0x10.
  Utility /IO /INDEX /RD 0xCD6 0xCD7 0x10:           || Read 1-DWORD data from IO port CD6h/CD7h 0x10.
  Utility /IO /INDEX /WD 0xCD6 0xCD7 0x10 0xFFFFFFFF:|| Write 1- DWORD 0xFFFFFFFF to IO port CD6h/CD7h 0x10.
  Utility /IO /INDEX /DUMP 0x70 0x71:                || Dump IO Index data from 0x70/0x71.
  Utility /IO /RF IO.bin 0 0x100:                    || Dump IO from 0 t0 0x100 to IO.bin file.
=====================================================================================================================
PCI Access:
=====================================================================================================================
  Utility /PCI /RB 0 0x1f 0 0:                       || Read 1-BYTE from PCI B0D1FF0 offset 0.
  Utility /PCI /RW 0 0x1f 0 0:                       || Read 1-WORD from PCI B0D1FF0 offset 0.
  Utility /PCI /RD 0 0x1f 0 0:                       || Read 1-DWORD from PCI B0D1FF0 offset 0.
  Utility /PCI /WB 0 0x1f 0 0x2C 0x12:               || Write 1-BYTE 0x12 to PCI B0D1FF0 offset 0x2C.
  Utility /PCI /WW 0 0x1f 0 0x2C 0x1234:             || Write 1-WORD 0x1234 to PCI B0D1FF0 offset 0x2C.
  Utility /PCI /WD 0 0x1f 0 0x2C 0x12345678:         || Write 1- DWORD 0x12345678 to PCI B0D1FF0 offset 0x2C.
  Utility /PCI /DUMP /NOMSG:                         || List all PCI controller without data message.
  Utility /PCI /DUMP /LOG:                           || Dump all PCI device data.
=====================================================================================================================
Mem Access:
=====================================================================================================================
  Utility /MEM /WD [MemAddr] [Data]:                 || Write1-DWORD Data to MemAddr
  Utility /MEM /RB 0xFF550:                          || Read 1-BYTE from memory 0xFF550.
  Utility /MEM /RW 0xFF550:                          || Read 1-WORD from memory 0xFF550.
  Utility /MEM /RD 0xFF550:                          || Read 1- DWORD from memory 0xFF550.
  Utility /MEM /WB 0x10000 0x12:                     || Write 1-BYTE 0x12 to memory 0x10000. 
  Utility /MEM /WW 0x10000 0x1234:                   || Write 1-WORD 0x1234 to memory 0x10000.
  Utility /MEM /WD 0x10000 0x12345678:               || Write 1- DWORD 0x12345678 to memory 0x10000.
  Utility /MEM /DUMP 0xFF550:                        || Dump memory data from 0xFF550, and show on screen.
  Utility /MEM /RF SMBIOS.bin 0xFF550 0x100:         || Dump 256 bytes from memory 0xFF550 to SMBIOS.bin file.
=====================================================================================================================
SMBus Access:
=====================================================================================================================
  Utility /SMBUS /RB 0xa0 0:                         || Read 1-BYTE data from smbus 0xA0 offset 0.
  Utility /SMBUS /WB 0xa0 0 0xFF:                    || Write 1-BYTE 0xFF to smbus 0xA0 offset 0.
  Utility /SMBUS /DUMP 0xa0:                         || Dump datas from smbus 0xA0.
  Utility /SMBUS /S 0xA0 0xA8:                       || Search valid smbus device from 0xA0 to 0xA8
=====================================================================================================================
MSR Access:
=====================================================================================================================
  Utility /MSR /WD [CpuNo] [MsrReg] [EDX] [EAX]:     || Write MSR EDX:EAX(64 Bit) to MsrReg of CpuNo.
  Utility /MSR /RD 1 0xC0010058:                     || Read CPU1 MSR from 0xC0010058.
  Utility /MSR /WD 1 0xC0010058 0 0x22222222:        || Write CPU1 MSR 0x00000000_22222222 to 0xC0010058.
  Utility /MSR /RD /ALL [MsrReg]                     || Dump All valid CPUs MsrReg.
  Utility /MSR /WD /ALL [MsrReg] [EDX] [EAX]         || Write All MSR EDX:EAX(64 Bit) to valid CPUs MsrReg.
=====================================================================================================================
 RTC:
=====================================================================================================================
  Utility /RTC [seconds]:                            || Set wakeup timer. If seconds==0, reset.
  Utility /RTC /RESET /T [seconds]:                  || Set timeout after seconds to reset system.
  Utility /RTC /GRESET /T [seconds]:                 || Set timeout after seconds to global reset system.
  Utility /RTC [seconds] /T [seconds2]:              || Set wakeup timer and delay seconds2 in S0.
=====================================================================================================================
SHUTDOWN:
=====================================================================================================================
  Utility /SHUTDOWN                                  || Shutdown system right now.
=====================================================================================================================
SMBIOS:
=====================================================================================================================
  Utility /SMBIOS:                                   || Show some types of SMBIOS.
  Utility /SMBIOS /S [String]:                       || Search String in SMBIOS.
=====================================================================================================================
SMN:
=====================================================================================================================
  Utility /SMN /RD [DieNum] [RegesterIndex]:         || Read SMN address data on witch DieNum.
  Utility /SMN /WD [DieNum] [RegesterIndex] [Data]:  || Write SMN data to address on witch DieNum.
=====================================================================================================================
SMU:
=====================================================================================================================
  Utility /SMU /RD [RequestId] [Data0]...[Data5]:    || Read SMU Message data on witch RequestId.
  Utility /SMU /WD [RequestId] [Data0]...[Data5]:    || Write SMU Message data on witch RequestId.
=====================================================================================================================

FLASH(UEFI only):
=====================================================================================================================
  Utility /FLASH /RD [FileName]:                     || Get Flash data to FileName image.
  Utility /FLASH /WD [FileName]:                     || Write FileName image to flash.
=====================================================================================================================
EDID(UEFI only):
=====================================================================================================================
  Utility /EDID:                                     || Get EDID data.
=====================================================================================================================

1.PS: If need not show on screen, add command /nomsg, like Utility /mem /dump 0xff550 /nomsg
      If need show debug message not pause, add "/log" command, like Utility /pci /dump /log
=====================================================================================================================
 
 

 

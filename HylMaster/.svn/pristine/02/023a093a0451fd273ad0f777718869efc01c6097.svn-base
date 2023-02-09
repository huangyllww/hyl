//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file CPUSmbiosDriver.h
    Install CPU Info Interface.

**/
#ifndef _CPU_SMBIOS_DRIVER_H
#define _CPU_SMBIOS_DRIVER_H

#pragma pack(1)

/**
    Processor Information Structure (Type 4)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SocketDesignation;              // String number
    UINT8                     ProcessotType;
    UINT8                     Family;
    UINT8                     ProcessorManufacturer;          // String number
    UINT32                    ProcessorID_1;
    UINT32                    ProcessorID_2;
    UINT8                     ProcessorVersion;               // String number
    UINT8                     Voltage;
    UINT16                    ExtClockFreq;
    UINT16                    MaxSpeed;
    UINT16                    CurrentSpeed;
    UINT8                     Status;
    UINT8                     Upgrade;
    UINT16                    L1CacheHandle;
    UINT16                    L2CacheHandle;
    UINT16                    L3CacheHandle;
    UINT8                     SerialNumber;                   // String number
    UINT8                     AssetTag;                       // String number
    UINT8                     PartNumber;                     // String number
    UINT8                     CoreCount;                      // Number of cores per processor socket
    UINT8                     CoreEnabled;                    // Number of enabled cores per processor socket
    UINT8                     ThreadCount;                    // Number of threads per processor socket
    UINT16                    ProcessorChar;                  // Defines which functions the processor supports
    UINT16                    Family2;
} CPU_SMBIOS_PROCESSOR_INFO;                                      // TYPE 4

/**
    Cache Information Structure (Type 7)
**/
typedef struct {
    SMBIOS_STRUCTURE_HEADER   StructureType;
    UINT8                     SocketDesignation;
    UINT16                    CacheConfig;
    UINT16                    MaxCacheSize;
    UINT16                    InstalledSize;
    UINT16                    SupportSRAM;
    UINT16                    CurrentSRAM;
    UINT8                     CacheSpeed;
    UINT8                     ErrorCorrectionType;
    UINT8                     SystemCacheType;
    UINT8                     Associativity;
} CPU_SMBIOS_CACHE_INFO;                                          // TYPE 7

#pragma pack()

typedef VOID    (*EFI_OEM_UPDATE_TYPE4_TABLE)   (UINT8 SocketIndex, UINT8 *TableBuffer);

#define PCI_CFG_ADDRESS(bus,dev,func,reg) \
    ((UINT64)((((UINTN)bus) << 24) + (((UINTN)dev) << 16) + (((UINTN)func) << 8) + ((UINTN)reg)))& 0x00000000ffffffff

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

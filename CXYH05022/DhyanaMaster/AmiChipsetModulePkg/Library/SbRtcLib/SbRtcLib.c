//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file SbRtcLib.c
    This file contains code for SbRtcLib.c
    in the DXE stage.

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

// UDK Header
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/RtcLib.h>
#include <Guid/Rtc.h>

#include <token.h>
// AMI Compatibility Header.
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    This function converts data from DEC to BCD format

    @param Dec value to be converted

    @retval UINT8 result of conversion

**/
UINT8 DecToBCD(
    IN UINT8 Dec
)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

/**
    This function converts data from BCD to DEC format

    @param Bcd value to be converted

        UINT8 
    @retval value result of conversion

**/
UINT8 BCDToDec(IN UINT8 Bcd)
{
    UINT8 FirstDigit = Bcd & 0xf;
    UINT8 SecondDigit = Bcd >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

/**
    Read the RTC value at the given Index.

    @param Index RTC Index

        UINT8 
    @retval Value RTC Value read from the provided Index

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Read 0x71 for Data.
              5. Return the Data.
**/
UINT8 ReadRtcIndex(
    IN UINT8 Index
)
{
    UINT8       NMI = IoRead8(RTC_INDEX_REG) & RTC_NMI_MASK;   //Read bit 7 (NMI setting).
    UINT8       volatile Value;
    BOOLEAN     IntState=CPULib_GetInterruptState();
//----------------------
    CPULib_DisableInterrupt();

    IoWrite8(RTC_INDEX_REG, Index | NMI);
    Value = IoRead8(RTC_DATA_REG);               //Read register.

    if(IntState)CPULib_EnableInterrupt();

    if (Index <= RTC_YEAR_REG || Index == ACPI_CENTURY_CMOS) Value = BCDToDec(Value);

    return (UINT8)Value;
}

/**
    Write the RTC value at the given Index.

    @param Index RTC Index
    @param Value Value to be written

    @retval VOID

    @note  Here is the control flow of this function:
              1. Read port 0x70 (RTC Index Register) to get bit 7.
                 Bit 7 is the NMI bit-it should not be changed.
              2. Set Index with the NMI bit setting.
              3. Output 0x70 with the Index.
              4. Write the data to 0x71.
**/
VOID WriteRtcIndex(
    IN UINT8 Index,
    IN UINT8 Value
)
{
    UINT8       NMI = IoRead8(RTC_INDEX_REG) & RTC_NMI_MASK;   //Read bit 7 (NMI setting).
    BOOLEAN     IntState=CPULib_GetInterruptState();
//--------------------
    if (Index <= RTC_YEAR_REG || Index == ACPI_CENTURY_CMOS) Value = DecToBCD(Value);

    CPULib_DisableInterrupt();

    IoWrite8(RTC_INDEX_REG,Index | NMI);
    IoWrite8(RTC_DATA_REG, Value);  //Write Register.

    if(IntState)CPULib_EnableInterrupt();
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

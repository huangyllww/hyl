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
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

//**********************************************************************
//<AMI_FHDR_START>
//
// Name: Dummy.c
//
// Description:	Definition of dummy main () function needed for LibCmin
//
//<AMI_FHDR_END>
//**********************************************************************

int main( int a, char** b)
{
	return 0;
}


INT64
EFIAPI
MultS64x64 (
  IN      INT64                     Multiplicand,
  IN      INT64                     Multiplier
  )
{
  return (INT64)MultU64x64 (Multiplicand, Multiplier);
}


EFIAPI
InternalMathDivRemS64x64 (
  IN      INT64                     Dividend,
  IN      INT64                     Divisor,
  OUT     INT64                     *Remainder  OPTIONAL
  );



INT64
EFIAPI
DivS64x64Remainder (
  IN      INT64                     Dividend,
  IN      INT64                     Divisor,
  OUT     INT64                     *Remainder  OPTIONAL
  )
{
  ASSERT (Divisor != 0);
  return InternalMathDivRemS64x64 (Dividend, Divisor, Remainder);
}


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

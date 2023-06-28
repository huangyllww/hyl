
 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <AmdIdsDebugPrintLocal.h>
#include <Library/SerialPortLib.h>	//For write function
#include <Filecode.h>

#define FILECODE LIBRARY_AMIIDSDEBUGPRINTLIB_AMIIDSDPSERIAL_FILECODE 

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
AmiIdsDpSerialSupport (
  VOID
  )
{

  return TRUE;
}

VOID
AmiIdsDpSerialConstructor (
  VOID  
  )
{
  return;
}

/**
 *  Get Serial customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
AmiIdsDpSerialGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}




/**
 *  Print formated string
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
VOID
AmiIdsDpSerialPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
	//Output to onboard serial
	SerialPortWrite ((UINT8 *)Buffer, BufferSize);
	
}


CONST AMD_IDS_DEBUG_PRINT_INSTANCE   AmiIdsDebugPrintSerialInstance =
{
  AmiIdsDpSerialSupport,
  AmiIdsDpSerialConstructor,
  AmiIdsDpSerialGetFilter,
  AmiIdsDpSerialPrint
};




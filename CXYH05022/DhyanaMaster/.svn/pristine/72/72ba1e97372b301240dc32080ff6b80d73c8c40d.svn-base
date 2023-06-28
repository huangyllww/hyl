
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
#include <Library/OemDebugAlwaysLib.h>   //<Wangyia001-20171128 Mantis 17453: BIOS debug message can not be got by SOL for Baidu>
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
  //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support+>
  if((GetDebugFlag() &  RTS_DEBUG_ENABLED) != RTS_DEBUG_ENABLED)
    return;
    
  SerialPortWrite ((UINT8 *)Buffer, BufferSize);  
  //Mizl-20220222-SGPZT#22027 Add Debug Level Lib Support->
}


CONST AMD_IDS_DEBUG_PRINT_INSTANCE   AmiIdsDebugPrintSerialInstance =
{
  AmiIdsDpSerialSupport,
  AmiIdsDpSerialConstructor,
  AmiIdsDpSerialGetFilter,
  AmiIdsDpSerialPrint
};




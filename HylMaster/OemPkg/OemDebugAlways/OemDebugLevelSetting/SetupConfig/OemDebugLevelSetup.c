
#include <Token.h>
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Guid/MdeModuleHii.h>
#include <AmiDxeLib.h>
#include "Library/AmiHiiUpdateLib.h"
#include "Setup.h"
#include <Library/UefiRuntimeServicesTableLib.h>

#include "OemDebugAlways.h"
#include "FileGuidNameErrorLevelTableHeader.h"
#include "OemDebugLevelSetup.h"
#include "Library/OemDebugLevelLib.h"
//OEM Debug - Module Link
#include "OemDebugLevel_PeiModule.c"
#include "OemDebugLevel_DxeModule.c"


static EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;
UINT32 gAttributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;

static EFI_HII_HANDLE gOemDebugLevelHandle = NULL;
static EFI_GUID gOEMDebugLevelFormSetGuid = OEM_DEBUG_LEVEL_FORM_SET_GUID;

OEM_DEBUG_LEVEL_DATABASE *gOemDebugLevelDatabase = NULL;

UINT32 gDebugLevelPeiEndQuestionId = OEM_DEBUG_LEVEL_PEI_END_QUESTION_ID;
UINT32 gDebugLevelDxeEndQuestionId = OEM_DEBUG_LEVEL_DXE_END_QUESTION_ID;


#define OEM_DEBUG_LEVEL_MODULE_ERROR_LEVEL_OFFSET(TableStart, Index) \
        (UINT16)(TableStart+ sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE)*Index+ sizeof(EFI_GUID))


#define DYNAMIC_DEBUG_NULL_GUID                           \
  {                                                       \
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 \
  }

// DD Policy Option Structure
typedef struct
{
  UINT32 Offset;
  EFI_STRING_ID PromptStringId;
  UINT16 QuestionId;
  BOOLEAN ShowItem;
} DEBUG_LEVEL_POLICY_OPTION;

EFI_STATUS
OemDebugLevelCallBack(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest);

EFI_HII_CONFIG_ACCESS_PROTOCOL CallBack = {NULL, NULL, OemDebugLevelCallBack};

CALLBACK_INFO SetupCallBack[] = {
    // Last field in every structure will be filled by the Setup
    {&gOEMDebugLevelFormSetGuid, &CallBack, OEM_DEBUG_LEVEL_SET_CLASS, 0, 0},
};


/**
  Get/Retrieve Debug Data Base Pointer

  @retval Pointer to OEM_DEBUG_LEVEL_DATABASE configuration if sucessfully retried, else NULL
 */
OEM_DEBUG_LEVEL_DATABASE *
    GetOemDebugLevelDataBaseConfigPointer(VOID)
{
  EFI_STATUS Status;
  UINTN Size;
  OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelConfigData = NULL;

  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

  // Get DynamicDebug Nvram Variable
  Size = 0;
  Status = gRT->GetVariable(
      OEM_DEBUG_LEVEL_DATA_NAME,
      &gOEMDebugLevelDataBaseGuid,
      NULL,
      &Size,
      NULL);
  DEBUG((DEBUG_INFO, " gRT->GetVariable status %r Size: %x \n", Status, Size));

  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    // Allocate memory for Dynamic Debug Nvram Data
    OemDebugLevelConfigData = (OEM_DEBUG_LEVEL_DATABASE *)AllocateZeroPool(Size);
    if (OemDebugLevelConfigData == NULL)
    {
      DEBUG((DEBUG_ERROR, "Failed to allocate memory for OemDebugLevelConfigData! \n"));
      return NULL;
    }

    // Get DynamicDebug Nvram Variable
    Status = gRT->GetVariable(
        OEM_DEBUG_LEVEL_DATA_NAME,
        &gOEMDebugLevelDataBaseGuid,
        &gAttributes,
        &Size,
        OemDebugLevelConfigData);
    DEBUG((DEBUG_INFO, " gRT->GetVariable status %r Size: %x \n", Status, Size));
    if (EFI_ERROR(Status))
    {
      // Free the allocated memory for Nvram data
      FreePool(OemDebugLevelConfigData);
      return NULL;
    }
  }

  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

  return OemDebugLevelConfigData;
}
/**
  This function 
  1) Creates Policy Options for all the module entries in FileGuidPrintErrorLevelTable
  2) Creates IFR Labels for All module and user module list Hii forms
  3) Create Opcodes for all Policy Options and update All module and user module Hii forms

  @param FileGuidPrintErrorLevelTable   Pointer to File Guid PrintErrorLevel Table.
  @param NumberofTableEntries           Number of entries in FileGuidPrintErrorLevelTable
  @param FileNameGuidTable              Pointer File Name Guid Table.
  @param TableOffset                    Offset of Table in Dynamic Debug database
  @param QuestionId                     On input contains base Question Id to be used and on output contains end Question Id
  @param AllModuleFormId                FormId for AllModule form
  @param AllModuleLabelStartNumber      Start label number for AllModule form
  @param AllModuleLabelEndNumber        End label number for AllModule form

  @retval EFI_STATUS                    Returns Success if all the functionality is successful, else error.
**/
EFI_STATUS
CreateModuleOpcodesUpdateForm(
    IN CONST FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable,
    IN UINT32 NumberofTableEntries,
    IN CONST FILE_GUID_NAME_TABLE *FileNameGuidTable,
    IN UINT32 TableOffset,
    IN UINT32 *QuestionId,
    IN EFI_FORM_ID ModuleFormId,
    IN UINT16 ModuleLabelStartNumber,
    IN UINT16 ModuleLabelEndNumber)
{
  DEBUG_LEVEL_POLICY_OPTION *ModuleDebugLevelPolicyOptionData = NULL;
  DEBUG_LEVEL_POLICY_OPTION *ModuleDebugLevelPolicyOption = NULL;

  CHAR16 *UnicodeString = NULL;
  UINTN StrSize = 0x250;
  UINT32 Index;

  VOID *ModuleStartOpCodeHandle;
  VOID *ModuleEndOpCodeHandle;
  EFI_IFR_GUID_LABEL *ModuleStartLabel;
  EFI_IFR_GUID_LABEL *ModuleEndLabel;

  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

  // Allocate Start and End Opcode handle
  ModuleStartOpCodeHandle = HiiAllocateOpCodeHandle();
  ModuleEndOpCodeHandle = HiiAllocateOpCodeHandle();

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Validate the create handles
  if ((ModuleStartOpCodeHandle == NULL) || (ModuleEndOpCodeHandle == NULL))
    return EFI_OUT_OF_RESOURCES;

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Create Hii Extended Label OpCode as the start and end opcode
  ModuleStartLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode(
      ModuleStartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));

  ModuleEndLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode(
      ModuleEndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  if ((ModuleStartLabel == NULL) || (ModuleEndLabel == NULL))
    return EFI_OUT_OF_RESOURCES;

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Fill Extend opcodes and number for labels
  ModuleStartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  ModuleEndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  ModuleStartLabel->Number = ModuleLabelStartNumber;
  ModuleEndLabel->Number = ModuleLabelEndNumber;

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Allocate memory for UnicodeString
  UnicodeString = AllocateZeroPool(StrSize);
  if (UnicodeString == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Allocate memory for Policy options data
  ModuleDebugLevelPolicyOptionData = (DEBUG_LEVEL_POLICY_OPTION *)AllocateZeroPool(NumberofTableEntries * sizeof(DEBUG_LEVEL_POLICY_OPTION));
  if (ModuleDebugLevelPolicyOptionData == NULL)
  {
    FreePool(UnicodeString);
    return EFI_OUT_OF_RESOURCES;
  }
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]NumberofTableEntries:%x\r\n",__FUNCTION__,__LINE__,NumberofTableEntries));
  // Create Policy Options data for all the modules
  Index = 0;
  while (Index < NumberofTableEntries)
  {

    // Fill Policy option for each module
    ModuleDebugLevelPolicyOption = ModuleDebugLevelPolicyOptionData + Index;
    ModuleDebugLevelPolicyOption->Offset = OEM_DEBUG_LEVEL_MODULE_ERROR_LEVEL_OFFSET(TableOffset, Index);
    ModuleDebugLevelPolicyOption->QuestionId = (*QuestionId)++;

    // User selection will be processed to update the List so set ShowItem to FALSE
    ModuleDebugLevelPolicyOption->ShowItem = FALSE;

    // Convert ASCII string to Unicode string
    AsciiStrToUnicodeStrS(FileNameGuidTable[Index].Name, UnicodeString, StrSize);
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Module Name:%a\r\n",__FUNCTION__,__LINE__,FileNameGuidTable[Index].Name));
    ModuleDebugLevelPolicyOption->PromptStringId = HiiSetString(gOemDebugLevelHandle, 0, UnicodeString, NULL);

    // Go to Next module
    Index++;
  }

  // Free Unicode String allocation
  FreePool(UnicodeString);
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Create Numeric Opcode for the modules
  Index = 0;
  while (Index < NumberofTableEntries)
  {

    // Create Numeric Opcode for each module
    ModuleDebugLevelPolicyOption = ModuleDebugLevelPolicyOptionData + Index;

    // Create Opcode for all Items in All case
    HiiCreateNumericOpCode(
        ModuleStartOpCodeHandle,                                //*OpCodeHandle
        ModuleDebugLevelPolicyOption->QuestionId,               //QuestionId
        OEM_DEBUG_LEVEL_DATABASE_VARSTORE_ID,                   //VarStore ID
        ModuleDebugLevelPolicyOption->Offset,                   //Offset
        ModuleDebugLevelPolicyOption->PromptStringId,           //Prompt
        STRING_TOKEN(STR_OEM_PRINT_ERROR_LEVEL_HELP),           //Help
        EFI_IFR_FLAG_INTERACTIVE | EFI_IFR_FLAG_RESET_REQUIRED, //QuestionFlags
        EFI_IFR_NUMERIC_SIZE_4 | EFI_IFR_DISPLAY_UINT_HEX,      //NumericFlags,
        MIN_ERROR_LEVEL_VALUE,                                  //Minimum,
        MAX_ERROR_LEVEL_VALUE,                      //Maximum,
        1,                                                      //Step,
        NULL                                                    //*DefaultsOpCodeHandle  OPTIONAL
    );
    // Increase the Index
    Index++;
  }

  // Update All Module Hii form
  HiiUpdateForm(
      gOemDebugLevelHandle,
      &gOEMDebugLevelFormSetGuid,
      ModuleFormId,
      ModuleStartOpCodeHandle,
      ModuleEndOpCodeHandle);
  // Free Opcode handle allocations
  if (ModuleStartOpCodeHandle != NULL)
  {
    HiiFreeOpCodeHandle(ModuleStartOpCodeHandle);
    HiiFreeOpCodeHandle(ModuleEndOpCodeHandle);
  }

  // // Free Policy Options data allocation
  FreePool(ModuleDebugLevelPolicyOptionData);

  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

  return EFI_SUCCESS;
}
/**
  This function is called by Setup browser to perform callback

  @param This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action           Specifies the type of action taken by the browser.
  @param QuestionId       A unique value which is sent to the original exporting driver
                          so that it can identify the type of data to expect.
  @param Type             The type of value for the question.
  @param Value            A pointer to the data being sent to the original exporting driver.
  @param ActionRequest    On return, points to the action requested by the callback function.

  
  @retval EFI_SUCCESS     The callback successfully handled the action.
  @retval EFI_UNSUPPORTED The action taken by the browser is not supported.

**/
EFI_STATUS
OemDebugLevelCallBack(
    IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
    IN EFI_BROWSER_ACTION Action,
    IN EFI_QUESTION_ID KeyValue,
    IN UINT8 Type,
    IN EFI_IFR_TYPE_VALUE *Value,
    OUT EFI_BROWSER_ACTION_REQUEST *ActionRequest)
{

  EFI_STATUS Status = EFI_UNSUPPORTED;

  // Validate Key Value
  if (((KeyValue >= OEM_DEBUG_LEVEL_PEI_START_QUESTION_ID) && (KeyValue <= gDebugLevelPeiEndQuestionId)) ||
      ((KeyValue >= OEM_DEBUG_LEVEL_DXE_START_QUESTION_ID) && (KeyValue <= gDebugLevelDxeEndQuestionId)))
  {
    // Handle Load Defaults and Action changing
    switch (Action)
    {
    case EFI_BROWSER_ACTION_CHANGING:
      Status = EFI_INVALID_PARAMETER;
      if ((Value->u32 >= MIN_ERROR_LEVEL_VALUE) && (Value->u32 <= MAX_ERROR_LEVEL_VALUE))
        Status = EFI_SUCCESS;
      break;

    case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
    case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
      Value->u32 = LOAD_DEFAULT_IDENTIFICATION_VALUE;
      Status = EFI_SUCCESS;
      break;
    } // switch
  }   //if
  return Status;
}

/**
  This function will dynamically add VFR contents to Nvme BIOS setup page
  using HII library functions.

  @param  Event    Event whose notification function is being invoked.
  @param  Context  pointer to the notification function's context.

  @return VOID

**/
VOID OemDebugLevelMainFormSetCallback(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
  EFI_STATUS    Status;
  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

  // Get Dynamic Debug User Configuration
  gOemDebugLevelDatabase = GetOemDebugLevelDataBaseConfigPointer();
  if (gOemDebugLevelDatabase == NULL)
    return;
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  // Create Module Opcodes for PEI modules and Update form
  Status = CreateModuleOpcodesUpdateForm(
      OEM_DEBUG_LEVEL_PEI_TABLE(gOemDebugLevelDatabase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_PEI_TABLE_ENTRIES(gOemDebugLevelDatabase),
      (FILE_GUID_NAME_TABLE *)BuildToolGeneratedPeiModuleTable,
      gOemDebugLevelDatabase->PeiTableOffset,
      &gDebugLevelPeiEndQuestionId,
      OEM_CONFIG_PEI_USER_FORM_ID,
      OEM_CONFIG_PEI_FORM_LABEL_START,
      OEM_CONFIG_PEI_FORM_LABEL_END);
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]CreateModuleOpcodesUpdateForm For PEI Status:%r\r\n",__FUNCTION__,__LINE__,Status));
  // Create Module Opcodes for DXE modules and Update form
  Status = CreateModuleOpcodesUpdateForm(
      OEM_DEBUG_LEVEL_DXE_TABLE(gOemDebugLevelDatabase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(gOemDebugLevelDatabase),
      (FILE_GUID_NAME_TABLE *)BuildToolGeneratedDxeModuleTable,
      gOemDebugLevelDatabase->DxeTableOffset,
      &gDebugLevelDxeEndQuestionId,
      OEM_CONFIG_DXE_USER_FORM_ID,
      OEM_CONFIG_DXE_FORM_LABEL_START,
      OEM_CONFIG_DXE_FORM_LABEL_END);
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]CreateModuleOpcodesUpdateForm For DXE Status:%r\r\n",__FUNCTION__,__LINE__,Status));
  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));
  return;
}

/** 
  This function is the entry point. Registers callback on TSE event to update IFR data.

  @param  ImageHandle     The image handle.
  @param  SystemTable     The system table.

  @retval  EFI_SUCEESS    Setup page is loaded successfully.
  @return  Error          Return error status
**/
EFI_STATUS
EFIAPI
OemDebugLevelSetupEntry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status = EFI_SUCCESS;
  VOID *SetupRegistration;
  EFI_GUID SetupEnterGuid = AMITSE_SETUP_ENTER_GUID;
  EFI_EVENT SetupEnterEvent;

  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));
  InitAmiLib(ImageHandle, SystemTable);

  // Load setup page and update strings
  Status = LoadResources(ImageHandle, sizeof(SetupCallBack) / sizeof(CALLBACK_INFO), SetupCallBack, NULL);

  if (!EFI_ERROR(Status))
  {
    RegisterProtocolCallback(
        &SetupEnterGuid, OemDebugLevelMainFormSetCallback,
        NULL, &SetupEnterEvent, &SetupRegistration);

    gOemDebugLevelHandle = SetupCallBack[0].HiiHandle;
  }

  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

  return Status;
}
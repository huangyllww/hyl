Prepare : OemDxeHookFiles


OemDxeHookFiles :$(BUILD_DIR)/OemDxeHook.h $(BUILD_DIR)/OemDxeMain.inf

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */
OEM_DXE_HOOK0 = $(subst $(SPACE),,$(OEM_DXE_HOOK)) # remove the spaces
OEM_DXE_HOOK1 = $(subst */$(COMMA),*/,$(OEM_DXE_HOOK0)) # remove commas after the comments
OEM_DXE_HOOK2 = $(subst *//*,,$(OEM_DXE_HOOK1)) # merge the adjacent comments
OEM_DXE_HOOK3 = $(subst */,*/",$(OEM_DXE_HOOK2)) # add quote after the comment to begin the next string
OEM_DXE_HOOK4 = "$(subst $(COMMA),"$(COMMA)",$(OEM_DXE_HOOK3))", # add quotes before and after the commas
OEM_DXE_HOOK5 = $(subst "/*,/*,$(OEM_DXE_HOOK4)) # remove quote before the comments
OEM_DXE_HOOK_STRINGS = $(subst ",\",$(OEM_DXE_HOOK5)) # escape the quotes for echo

$(BUILD_DIR)/OemDxeHook.h : $(BUILD_DIR)/Token.h $(OEM_DXE_MAIN_DIR)/OemDxeMain.mak
	$(ECHO) \
$(EOL)\
#ifndef __OEM_DXE_HOOK_H__ $(EOL)\
#define __OEM_DXE_HOOK_H__ $(EOL)\
$(EOL)\
//Auto Generate By OemPkg\Dxe\OemDxeMain\OemDxeMain.mak,$(EOL)\
//Do Not Edit Manully!!$(EOL)\
typedef EFI_STATUS (OEM_DXE_HOOK)(IN  EFI_SYSTEM_TABLE *SystemTable);$(EOL)\
$(EOL)\
$(EOL)\
#define OEM_DXE_HOOK_LIST $(OEM_DXE_HOOK)$(EOL)\
#define OEM_DXE_HOOK_LIST_NAMES $(OEM_DXE_HOOK_STRINGS)$(EOL)\
$(EOL)\
#endif $(EOL)\
>$(BUILD_DIR)/OemDxeHook.h

$(BUILD_DIR)/OemDxeMain.inf : $(BUILD_DIR)/Token.mak $(OEM_DXE_MAIN_DIR)/OemDxeMain.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   SETUP_DEFINITIONS, SETUP_FORMSETS, SetupStringFiles, and SetupCallbackFiles$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = OemDxeMain$(EOL)\
  FILE_GUID                      = a30c36f5-3798-46f7-b58f-1d4da452914c$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = OemDxeHook_Entry$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(OEM_DXE_MAIN_DIR)/OemDxeMain.c$(EOL)\
	OemDxeHook.h$(EOL)\
"> $(BUILD_DIR)/OemDxeMain.inf
ifneq ($(OEM_DXE_HOOK_FILES_LINK),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(OEM_DXE_HOOK_FILES_LINK)))$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf
endif
	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  OemPkg\OemPkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalOemDxeHookPackages))$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  UefiBootServicesTableLib$(EOL)\
  UefiRuntimeServicesTableLib$(EOL)\
  DxeServicesTableLib$(EOL)\
  DevicePathLib$(EOL)\
  BaseLib$(EOL)\
  UefiLib$(EOL)\
  DebugLib$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalOemDxeHookLibraryClasses))$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"$(EOL)\
[Protocols]$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalOemDxeHookProtocols))$(EOL)"\
>> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"[Guids]$(EOL)\
  gEfiGlobalVariableGuid    ## CONSUMES$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalOemDxeHookGuids))$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"$(EOL)\
[Pcd]$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(AdditionalOemDxeHookPcds))$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

	@$(ECHO) \
"$(EOL)\
[Depex]$(EOL)\
  TRUE$(EOL)\
$(EOL)\
">> $(BUILD_DIR)/OemDxeMain.inf

Prepare : $(BUILD_DIR)/OemDebugLevel_PeiModule.c $(BUILD_DIR)/OemDebugLevel_DxeModule.c

$(BUILD_DIR)/OemDebugLevel_PeiModule.c: $(OEM_DEBUGLEVEL_DIR)/OemDebugLevel.mak
	$(ECHO) \
"#include <$(OEM_MOUDLE_DEBUG_LEVEL_PORTING_FILE_PEI)>$(EOL)\
#define NumberofElementsInBuildToolGeneratedPeiModuleTable OEM_NumberofElementsInBuildToolGeneratedPeiModule$(PEI_ARCHITECTURE)Table$(EOL)\
#define BuildToolGeneratedPeiModuleTable OEM_BuildToolGeneratedPeiModule$(PEI_ARCHITECTURE)Table$(EOL)\
" > $@

$(BUILD_DIR)/OemDebugLevel_DxeModule.c: $(OEM_DEBUGLEVEL_DIR)/OemDebugLevel.mak
	$(ECHO) \
"#include <$(OEM_MOUDLE_DEBUG_LEVEL_PORTING_FILE_DXE)>$(EOL)\
#define NumberofElementsInBuildToolGeneratedDxeModuleTable OEM_NumberofElementsInBuildToolGeneratedDxeModule$(DXE_ARCHITECTURE)Table$(EOL)\
#define BuildToolGeneratedDxeModuleTable OEM_BuildToolGeneratedDxeModule$(DXE_ARCHITECTURE)Table$(EOL)\
" > $@
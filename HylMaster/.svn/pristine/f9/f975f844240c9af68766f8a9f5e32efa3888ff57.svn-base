Prepare : PeiPciEnumerationFiles

PeiPciEnumerationFiles : $(BUILD_DIR)/PeiPciEnumeration.inf

$(BUILD_DIR)/PeiPciEnumeration.inf : $(BUILD_DIR)/Token.mak $(PEI_PCI_ENUMERATION_DIR)/PeiPciEnumeration.mak
	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
#   PeiPciEnumerationFiles, PeiPciEnumerationGuids, PeiPciEnumerationLibraryClasses and PeiPciEnumerationPackages$(EOL)\
#   PeiPciEnumerationPpis, PeiPciEnumerationPcds$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  BASE_NAME                      = PeiPciEnumeration$(EOL)\
  MODULE_TYPE                    = PEIM$(EOL)\
  FILE_GUID                      = 01FB5D53-4FF9-4ad9-ADF5-7DC63659A1B2$(EOL)\
  ENTRY_POINT                    = PeiPciEnumerationEntry$(EOL)\
#$(EOL)\
[Sources]$(EOL)\
  ../$(PEI_PCI_ENUMERATION_DIR)/PeiPciEnumeration.c$(EOL)\
  ../$(PEI_PCI_ENUMERATION_DIR)/PeiPciEnumeration.h$(EOL)\
"> $(BUILD_DIR)/PeiPciEnumeration.inf
ifneq ($(PeiPciEnumerationFiles),"")
	@$(ECHO) \
"  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationFiles)))$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf
endif
	@$(ECHO) \
"$(EOL)\
[Packages]$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  MdeModulePkg/MdeModulePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationPackages))$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"$(EOL)\
[LibraryClasses]$(EOL)\
  PeimEntryPoint$(EOL)\
  DebugLib$(EOL)\
  PeiServicesLib$(EOL)\
  PeiServicesTablePointerLib$(EOL)\
  MemoryAllocationLib$(EOL)\
  HobLib$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationLibraryClasses))$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"$(EOL)\
[Ppis]$(EOL)\
  gAmiPeiPciEnumerationPpiGuid    ## CONSUMES$(EOL)\
  gEfiEndOfPeiSignalPpiGuid       $(EOL)\
  gAmdFabricTopologyServicesPpiGuid $(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationPpis))$(EOL)"\
>> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"[Guids]$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationGuids))$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"$(EOL)\
[Pcd]$(EOL)\
	gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress$(EOL)\
	gAmiModulePkgTokenSpaceGuid.AmiPcdPeiPciEnumerationLastBusNumber$(EOL)\
	gAmiModulePkgTokenSpaceGuid.AmiPcdPeiPciMmioTopAddress$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(PeiPciEnumerationPcds))$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf

	@$(ECHO) \
"$(EOL)\
[Depex]$(EOL)\
  gEfiPciCfg2PpiGuid AND$(EOL)\
  gAmdFabricTopologyServicesPpiGuid$(EOL)\
$(EOL)\
[BuildOptions]$(EOL)\
  *_*_*_BUILD_FLAGS=-s$(EOL)\
">> $(BUILD_DIR)/PeiPciEnumeration.inf
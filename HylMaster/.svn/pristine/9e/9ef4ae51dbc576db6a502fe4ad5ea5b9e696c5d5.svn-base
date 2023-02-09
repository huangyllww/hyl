Prepare : FastVideoFunction

FastVideoFunction : $(BUILD_DIR)/FastVideoFunction.inf

$(BUILD_DIR)/FastVideoFunction.inf : $(BUILD_DIR)/Token.mak $(FAST_VIDEO_DIR)/FastVideoFunction.mak

	@$(ECHO) \
"## @file$(EOL)\
#   The [Sources] section for this file is auto-generated from ELINKs:$(EOL)\
##$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010006$(EOL)\
  BASE_NAME                      = FastVideoInitPeim$(EOL)\
  FILE_GUID                      = 925ef2d8-b45e-4bc3-b90a-bfd494909de3$(EOL)\
  MODULE_TYPE                    = PEIM$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  ENTRY_POINT                    = FastVideoPeimEntryPoint$(EOL)\
#$(EOL)\
[Sources.common]$(EOL)\
  ../$(FAST_VIDEO_DIR)/FastVideoFunction.c$(EOL)\
$(EOL)\
[LibraryClasses]$(EOL)\
  InitVGALib$(EOL)\
  PeimEntryPoint$(EOL)\
  BaseLib$(EOL)\
  AmiPeiLib$(EOL)\
  InitVGALib$(EOL)\
  FastVideoLib$(EOL)\
  PcdLib$(EOL)\
  PciLib$(EOL)\
  BaseGetRbBusLib$(EOL)\
"> $(BUILD_DIR)/FastVideoFunction.inf
	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(FastVideoLibraryClasses))$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"[Ppis]$(EOL)\
  gAmiPeiPciEnumerationPpiGuid    ## CONSUMES$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(FastVideoPpis))$(EOL)"\
>> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"[Guids]$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(FastVideoGuids))$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"[Packages]$(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AgesaPkg/AgesaPkg.dec$(EOL)\
  OemPkg/OemPkg.dec$(EOL)\
  OemPlatPkg/OemPlatPkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(FastVideoPackages))$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"[Pcd]$(EOL)\
    gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress$(EOL)\
    gAmiModulePkgTokenSpaceGuid.AmiPcdPeiPciEnumerationLastBusNumber$(EOL)\
    gAmiModulePkgTokenSpaceGuid.AmiPcdPeiPciMmioTopAddress$(EOL)\
    gOemPkgTokenSpaceGuid.PcdFastVideoMmioBaseAddress$(EOL)\
    gOemPkgTokenSpaceGuid.PcdFastVideoBusNum$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"  $(subst $(SPACE),$(EOL)$(SPACE),$(FastVideoPcds))$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf

	@$(ECHO) \
"$(EOL)\
[Depex]$(EOL)\
    gEfiPeiMemoryDiscoveredPpiGuid AND$(EOL)\
    gEfiPeiDecompressPpiGuid AND$(EOL)\
    gEfiPeiCpuIoPpiInstalledGuid AND$(EOL)\
    gEfiPciCfg2PpiGuid$(EOL)\
$(EOL)\
">> $(BUILD_DIR)/FastVideoFunction.inf
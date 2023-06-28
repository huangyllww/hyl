#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#*************************************************************************
#<AMI_FHDR_START>
#
# Name: AmiPsp.mak
#
# Description: Invokes ROM PSP update before Secure Flash tools
# 
#
#<AMI_FHDR_END>
#*************************************************************************
ifeq ("$(SOC0_PSP_SUPPORT)", "1")
ifeq ("$(SOC1_PSP_SUPPORT)", "1")
ifeq ("$(SOC2_PSP_SUPPORT)", "1")
PrepareComboTable : DeletePspComboData
endif
endif
endif
ifeq ($(SOC0_PSP_SUPPORT),1)
PrepareSoc0 : DeleteSoc0PspData
endif
ifeq ($(SOC1_PSP_SUPPORT),1)
PrepareSoc1 : DeleteSoc1PspData
endif
ifeq ($(SOC2_PSP_SUPPORT),1)
PrepareSoc2 : DeleteSoc2PspData
endif

.PHONY : AmiPspUpdate
AmiPspUpdate :

ifeq ($(SecureMod_SUPPORT),1)
ifeq ($(PSP_USE_HW_AUTH),1)
PsbProcess : $(FWCAPSULE_FILE_NAME)
End : PsbProcess
endif
UNSIGN_BIOS_ROM_FOR_PSP := $(UNSIGNED_BIOS_ROM)
else
UNSIGN_BIOS_ROM_FOR_PSP := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)FV$(PATH_SLASH)AMIROM.fd
End : AfterRom
AfterRom : $(UNSIGN_BIOS_ROM_FOR_PSP)
endif

DeletePspComboData:
ifeq ($(wildcard $(BUILD_DIR)$(PATH_SLASH)PspDataCombo.xml), $(BUILD_DIR)$(PATH_SLASH)PspDataCombo.xml)
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)PspDataCombo.xml
endif

DeleteSoc0PspData:
ifeq ($(wildcard $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml), $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml)
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml
endif

DeleteSoc1PspData:
ifeq ($(wildcard $(BUILD_DIR)$(PATH_SLASH)PspDataSoc1.xml), $(BUILD_DIR)$(PATH_SLASH)PspDataSoc1.xml)
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc1.xml
endif

DeleteSoc2PspData:
ifeq ($(wildcard $(BUILD_DIR)$(PATH_SLASH)PspDataSoc2.xml), $(BUILD_DIR)$(PATH_SLASH)PspDataSoc2.xml)
	-$(RM) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc2.xml
endif

# DXE SIG relative start -------------------
PSP_SIGPAD := -y -l $(FWCAPSULE_MAX_HDR_SIZE)	
ifeq ($(FWCAPSULE_CERT_FORMAT),0)
# use key "-q" to include FwCap hdr into a signature calculation
# otherwise only Payload and RomMap are signed
PSP_SIGPAD += -q
endif

ifeq ($(strip $(FWSIG_KEY)),)
FWSIG_KEY := $(FWpriv)
endif

FV_BB_PATH := $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)FV$(PATH_SLASH)FV_BB.Fv
FV_BB_PATH2 := $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)ResetImage.bin
# DXE SIG relative end   -------------------

$(UNSIGN_BIOS_ROM_FOR_PSP) : AmiPspUpdate
ifeq ($(PSP_USE_HW_AUTH),0)
ifeq ("$(SOC0_PSP_SUPPORT)", "1")
ifeq ("$(SOC1_PSP_SUPPORT)", "1")
ifeq ("$(SOC2_PSP_SUPPORT)", "1")
	$(HYGONPSPTOOL) -o . bb $(UNSIGN_BIOS_ROM_FOR_PSP) $(BUILD_DIR)\PspDataCombo.xml $(BUILD_DIR)\Psp_temp.rom
endif
else
	$(CP) $(UNSIGN_BIOS_ROM_FOR_PSP) $(BUILD_DIR)\Psp_temp.rom 
endif
endif
ifeq ($(SOC0_PSP_SUPPORT),1)
	$(HYGONPSPTOOL) -o . bb $(BUILD_DIR)\Psp_temp.rom $(BUILD_DIR)\PspDataSoc0.xml $(BUILD_DIR)\Psp_temp.rom
endif
ifeq ($(SOC1_PSP_SUPPORT),1)
	$(HYGONPSPTOOL) -o . bb $(BUILD_DIR)\Psp_temp.rom $(BUILD_DIR)\PspDataSoc1.xml $(BUILD_DIR)\Psp_temp.rom
endif
ifeq ($(SOC2_PSP_SUPPORT),1)
	$(HYGONPSPTOOL) -o . bb $(BUILD_DIR)\Psp_temp.rom $(BUILD_DIR)\PspDataSoc2.xml $(BUILD_DIR)\Psp_temp.rom
endif
	$(CP) $(BUILD_DIR)\Psp_temp.rom $(UNSIGN_BIOS_ROM_FOR_PSP)	
endif	
ifeq ($(PSP_USE_DXE_AUTH),1)
	$(PSPTOOL) $(UNSIGN_BIOS_ROM_FOR_PSP) -m $(DXE_SIG_ROM_LAYOUT) -mi $(ROM_LAYOUT_EX)
#generating DXE FV signature
	$(CRYPTCON) -c $(FWpriv) -k $(FWSIG_KEY) $(PSP_SIGPAD) -g $(DXE_SIG_SECTION_MAK_GUID) -r $(DXE_SIG_ROM_LAYOUT) -f $(UNSIGN_BIOS_ROM_FOR_PSP) -o $(UNSIGN_BIOS_ROM_FOR_PSP)
endif

PRE_PSB_FWCAPSULE_FILE_NAME := $(FWCAPSULE_FILE_NAME)
PsbProcess : AmiPspUpdate
	-$(RMDIR) PSB
	-$(RMDIR) $(BUILD_DIR)$(PATH_SLASH)PSB
	$(MKDIR) PSB
	$(MKDIR) $(BUILD_DIR)$(PATH_SLASH)PSB
	$(HYGONPSPTOOL) -o $(BUILD_DIR)$(PATH_SLASH)PSB bd $(PRE_PSB_FWCAPSULE_FILE_NAME) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml
	$(HYGONPSPTOOL) -o $(BUILD_DIR)$(PATH_SLASH)PSB bb $(PRE_PSB_FWCAPSULE_FILE_NAME) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml Psp_temp.rom
	$(HYGONPSPTOOL) -o $(BUILD_DIR)$(PATH_SLASH)PSB dp -p DN -b $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)Psp_temp.rom
	$(CP) /b $(FV_BB_PATH2)+$(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)BiosDirHeaderL1.bin $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)RTM_L1.bin
	$(CP) /b $(FV_BB_PATH2)+$(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)BiosDirHeaderL1.bin+$(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)BiosDirHeaderL2.bin $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)RTM_L1_L2.bin
	$(CP) /Y $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)RTM_L1.bin PSB\RTM_L1.bin
	$(CP) /Y $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)RTM_L1_L2.bin PSB\RTM_L1_L2.bin
	@$(AmiPspPkg_DIR)\wait_start.cmd
	if exist $(BIOS_RTM_SIGNATURE_FILENAME) $(RM) $(BIOS_RTM_SIGNATURE_FILENAME)
	if exist $(BIOS_RECOVERY_RTM_SIGNATURE_FILENAME) $(RM) $(BIOS_RECOVERY_RTM_SIGNATURE_FILENAME)
	$(CP) /Y PSB\RTM_L1.sig $(BIOS_RTM_SIGNATURE_FILENAME)
	$(CP) /Y PSB\RTM_L1_L2.sig $(BIOS_RECOVERY_RTM_SIGNATURE_FILENAME)
	$(HYGONPSPTOOL) -o $(BUILD_DIR)$(PATH_SLASH)PSB bb $(PRE_PSB_FWCAPSULE_FILE_NAME) $(BUILD_DIR)$(PATH_SLASH)PspDataSoc0.xml PsbSigned.rom
	$(CP) /Y $(BUILD_DIR)$(PATH_SLASH)PSB$(PATH_SLASH)PsbSigned.rom $(FWCAPSULE_FILE_NAME)

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

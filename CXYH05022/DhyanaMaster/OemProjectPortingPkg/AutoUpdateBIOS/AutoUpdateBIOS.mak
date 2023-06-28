End : AutoUpdateBios

.PHONY : AutoUpdateBios
AutoUpdateBios:

	@$(IPMITool) -I lanplus -H $(HostIP) -U $(UserName) -P $(UserPwd) chassis power off
	@$(ECHO) Power Off System!!!!

	@$(ECHO) Start Update $(BIOS_TAG).rom Via SISTool
	@$(ECHO) $(SISTool) -x --verbose update bios -H $(HostIP) -U $(UserName) -P $(UserPwd) -I $(BIOS_TAG).rom
	@$(SISTool) -x update bios -H $(HostIP) -U $(UserName) -P $(UserPwd) -I $(BIOS_TAG).rom

	@$(IPMITool) -I lanplus -H $(HostIP) -U $(UserName) -P $(UserPwd) chassis power on
	@$(ECHO) Power On System!!!!
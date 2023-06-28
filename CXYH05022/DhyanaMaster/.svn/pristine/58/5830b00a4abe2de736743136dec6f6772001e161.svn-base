Prepare : $(BUILD_DIR)/AmiAfuProtocolElinks.h

$(BUILD_DIR)/AmiAfuProtocolElinks.h : $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define BIOS_Pre_Update_List $(BIOSPreUpdateList)$(EOL)\
#define BIOS_End_Update_List $(BIOSEndUpdateList)$(EOL)\
#define ME_Pre_Update_List $(MEPreUpdateList)$(EOL)\
#define ME_End_Update_List $(MEEndUpdateList)$(EOL)"\
> $(BUILD_DIR)/AmiAfuProtocolElinks.h

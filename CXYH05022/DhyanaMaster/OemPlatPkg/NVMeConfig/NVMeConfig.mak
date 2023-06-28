Prepare : $(BUILD_DIR)/AutoNVMePortingList.h

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */
AUTO_NVME_PORTING_LIST0 = $(subst $(SPACE),,$(AUTO_NVME_PORTING_LIST)) # remove the spaces
AUTO_NVME_PORTING_LIST1 = $(subst */$(COMMA),*/,$(AUTO_NVME_PORTING_LIST0)) # remove commas after the comments
AUTO_NVME_PORTING_LIST2 = $(subst *//*,,$(AUTO_NVME_PORTING_LIST1)) # merge the adjacent comments
AUTO_NVME_PORTING_LIST3 = $(subst */,*/",$(AUTO_NVME_PORTING_LIST2)) # add quote after the comment to begin the next string
AUTO_NVME_PORTING_LIST4 = "$(subst $(COMMA),"$(COMMA)",$(AUTO_NVME_PORTING_LIST3))", # add quotes before and after the commas
AUTO_NVME_PORTING_LIST5 = $(subst "/*,/*,$(AUTO_NVME_PORTING_LIST4)) # remove quote before the comments
AUTO_NVME_PORTING_LIST_STRINGS = $(subst ",\",$(AUTO_NVME_PORTING_LIST5)) # escape the quotes for echo

$(BUILD_DIR)/AutoNVMePortingList.h : $(BUILD_DIR)/Token.h $(AUTO_DXIO_CONFIGURATION_DIR)/AutoDxioConfiguration.mak
	$(ECHO) \
$(EOL)\
#define AUTO_NVME_PORTING_LIST $(AUTO_NVME_PORTING_LIST)$(EOL)\
#define AUTO_NVME_PORTING_LIST_NAMES $(AUTO_NVME_PORTING_LIST_STRINGS)$(EOL)\
>$(BUILD_DIR)/AutoNVMePortingList.h
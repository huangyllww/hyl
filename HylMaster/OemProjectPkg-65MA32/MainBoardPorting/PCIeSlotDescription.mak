Prepare : $(BUILD_DIR)/AutoPCIeSlotDescriptionList.h

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */
AUTO_PCIE_SLOT_DESC_LIST0 = $(subst $(SPACE),,$(AUTO_PCIE_SLOT_DESC_LIST)) # remove the spaces
AUTO_PCIE_SLOT_DESC_LIST1 = $(subst */$(COMMA),*/,$(AUTO_PCIE_SLOT_DESC_LIST0)) # remove commas after the comments
AUTO_PCIE_SLOT_DESC_LIST2 = $(subst *//*,,$(AUTO_PCIE_SLOT_DESC_LIST1)) # merge the adjacent comments
AUTO_PCIE_SLOT_DESC_LIST3 = $(subst */,*/",$(AUTO_PCIE_SLOT_DESC_LIST2)) # add quote after the comment to begin the next string
AUTO_PCIE_SLOT_DESC_LIST4 = "$(subst $(COMMA),"$(COMMA)",$(AUTO_PCIE_SLOT_DESC_LIST3))", # add quotes before and after the commas
AUTO_PCIE_SLOT_DESC_LIST5 = $(subst "/*,/*,$(AUTO_PCIE_SLOT_DESC_LIST4)) # remove quote before the comments
AUTO_PCIE_SLOT_DESC_LIST_STRINGS = $(subst ",\",$(AUTO_PCIE_SLOT_DESC_LIST5)) # escape the quotes for echo

$(BUILD_DIR)/AutoPCIeSlotDescriptionList.h : $(BUILD_DIR)/Token.h $(AUTO_DXIO_CONFIGURATION_DIR)/AutoDxioConfiguration.mak
	$(ECHO) \
$(EOL)\
#define AUTO_PCIE_SLOT_DESC_LIST $(AUTO_PCIE_SLOT_DESC_LIST)$(EOL)\
#define AUTO_PCIE_SLOT_DESC_LIST_NAMES $(AUTO_PCIE_SLOT_DESC_LIST_STRINGS)$(EOL)\
>$(BUILD_DIR)/AutoPCIeSlotDescriptionList.h
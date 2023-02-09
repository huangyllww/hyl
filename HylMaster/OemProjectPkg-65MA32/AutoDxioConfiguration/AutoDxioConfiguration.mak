Prepare : $(BUILD_DIR)/AutoDxioFunctionList.h

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */
AUTO_DXIO_FUNCTION_LIST0 = $(subst $(SPACE),,$(AUTO_DXIO_FUNCTION_LIST)) # remove the spaces
AUTO_DXIO_FUNCTION_LIST1 = $(subst */$(COMMA),*/,$(AUTO_DXIO_FUNCTION_LIST0)) # remove commas after the comments
AUTO_DXIO_FUNCTION_LIST2 = $(subst *//*,,$(AUTO_DXIO_FUNCTION_LIST1)) # merge the adjacent comments
AUTO_DXIO_FUNCTION_LIST3 = $(subst */,*/",$(AUTO_DXIO_FUNCTION_LIST2)) # add quote after the comment to begin the next string
AUTO_DXIO_FUNCTION_LIST4 = "$(subst $(COMMA),"$(COMMA)",$(AUTO_DXIO_FUNCTION_LIST3))", # add quotes before and after the commas
AUTO_DXIO_FUNCTION_LIST5 = $(subst "/*,/*,$(AUTO_DXIO_FUNCTION_LIST4)) # remove quote before the comments
AUTO_DXIO_FUNCTION_LIST_STRINGS = $(subst ",\",$(AUTO_DXIO_FUNCTION_LIST5)) # escape the quotes for echo

# We are trying to convert a list of comma separated functions to a list of comma 
# separated function name strings.
# The original list of function may contain C comment blocks /* */
AUTO_PCIE_PORTING_LIST0 = $(subst $(SPACE),,$(AUTO_PCIE_PORTING_LIST)) # remove the spaces
AUTO_PCIE_PORTING_LIST1 = $(subst */$(COMMA),*/,$(AUTO_PCIE_PORTING_LIST0)) # remove commas after the comments
AUTO_PCIE_PORTING_LIST2 = $(subst *//*,,$(AUTO_PCIE_PORTING_LIST1)) # merge the adjacent comments
AUTO_PCIE_PORTING_LIST3 = $(subst */,*/",$(AUTO_PCIE_PORTING_LIST2)) # add quote after the comment to begin the next string
AUTO_PCIE_PORTING_LIST4 = "$(subst $(COMMA),"$(COMMA)",$(AUTO_PCIE_PORTING_LIST3))", # add quotes before and after the commas
AUTO_PCIE_PORTING_LIST5 = $(subst "/*,/*,$(AUTO_PCIE_PORTING_LIST4)) # remove quote before the comments
AUTO_PCIE_PORTING_LIST_STRINGS = $(subst ",\",$(AUTO_PCIE_PORTING_LIST5)) # escape the quotes for echo

$(BUILD_DIR)/AutoDxioFunctionList.h : $(BUILD_DIR)/Token.h $(AUTO_DXIO_CONFIGURATION_DIR)/AutoDxioConfiguration.mak
	$(ECHO) \
$(EOL)\
//Define DXIO Function List Used To Update CPM Table $(EOL)\
#define AUTO_DXIO_FUNCTION_LIST $(AUTO_DXIO_FUNCTION_LIST)$(EOL)\
#define AUTO_DXIO_FUNCTION_LIST_NAMES $(AUTO_DXIO_FUNCTION_LIST_STRINGS)$(EOL)\
$(EOL)\
//Define Standrard Interface for PCIe Topy $(EOL)\
#define AUTO_PCIE_PORTING_LIST $(AUTO_PCIE_PORTING_LIST)$(EOL)\
#define AUTO_PCIE_PORTING_LIST_NAMES $(AUTO_PCIE_PORTING_LIST_STRINGS)$(EOL)\
>$(BUILD_DIR)/AutoDxioFunctionList.h
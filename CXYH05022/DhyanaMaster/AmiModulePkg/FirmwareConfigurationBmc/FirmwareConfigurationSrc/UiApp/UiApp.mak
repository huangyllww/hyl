#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**              5555 Oak brook Pkwy, Norcorss, GA 30093             **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header: $
#
# $Revision:  $
#
# $Date: $
#************************************************************************
#<AMI_FHDR_START>
#
# Name:	UiApp.mak
#
# Description:	make file to compress html file gz files in build folder
#
#<AMI_FHDR_END>
#************************************************************************

Prepare : CompressFiles

REMOTE_GZP_DIR = $(WORKSPACE)/$(REMOTE_UI_APP_DIR)
REMOTE_BUILD_DIR = $(WORKSPACE)/$(BUILD_DIR)

ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
	REMOTE_GZIP_EXEC = $(REMOTE_GZP_DIR)/gzip.exe
else
	REMOTE_GZIP_EXEC = gzip
endif		

CompressFiles : $(BUILD_DIR)/Token.mak

ifeq ($(FwComboButton_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/ComboButton/ComboButton.png > $(REMOTE_BUILD_DIR)/ComboButton.png
endif

ifeq ($(FwCss_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/Css/Index.css > $(REMOTE_BUILD_DIR)/Index.css
endif

ifeq ($(FwFavicon_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/Favicon/Favicon.ico > $(REMOTE_BUILD_DIR)/Favicon.ico
endif

ifeq ($(FwIndex_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/Index/Index.html > $(REMOTE_BUILD_DIR)/Index.html
endif

ifeq ($(FwJs_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/Js/Index.js > $(REMOTE_BUILD_DIR)/Index.js
endif

ifeq ($(FwLoaderImage_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/LoaderImage/Loader.png > $(REMOTE_BUILD_DIR)/Loader.png
endif

ifeq ($(FwRbLogo_SUPPORT),1)
	$(REMOTE_GZIP_EXEC) -c $(REMOTE_GZP_DIR)/RbLogo/RbLogo.png > $(REMOTE_BUILD_DIR)/RbLogo.png
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             5555 Oakbrook Pkwy, Norcross, Georgia 30093          **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

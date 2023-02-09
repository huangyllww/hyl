include $(UTILITIES_MAK)
AMISDL = $(JAVA) -jar $(TOOLS_DIR)/AMISDL.jar

All: Prepare

Prepare : NbioNaples

NbioNaples:
	if not exist $(BUILD_DIR)\$(NBIO_BUILDZP_DIR)\     mkdir  $(BUILD_DIR)\buildzp		
	$(CP) PROJECT.LDL /b+$(NBIO_NAPLES_DIR)\NbioNaples.ldl /b  NaplesTarget.ldl  /b  
	$(AMISDL)   NaplesTarget.ldl  $(BUILD_DIR)\buildzp
#	del  $(BUILD_DIR)\PCIDXEINIT.h 
#	$(CP) $(BUILD_DIR)\$(NBIO_BUILDZP_DIR)\PCIDXEINIT.h      $(BUILD_DIR)	
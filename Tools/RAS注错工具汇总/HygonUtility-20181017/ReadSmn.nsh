@echo -off

if "%1" == "-h" or "%1" == "" then
   @goto help
endif

@set DIE_NUM %1
@set SMN_ADDR %2
@Utility.efi /nbio /rd %DIE_NUM% %SMN_ADDR%
@goto endm

:help
@echo ReadSmn.nsh <die number> <smn address>

:endm
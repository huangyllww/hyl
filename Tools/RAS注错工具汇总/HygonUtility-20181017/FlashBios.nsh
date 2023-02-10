@echo -off

if "%1" == "-h" or "%1" == "" then
   @goto help
endif

if not exist "%1" then
   @goto help
endif

@echo Flash BIOS ...
@Utility.efi /flash /wd %1
@goto endm

:help
@echo FlashBios.nsh <BIOS image>

:endm
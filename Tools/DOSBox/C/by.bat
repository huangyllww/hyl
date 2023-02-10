@echo off
masm %1;
if errorlevel 1 pause & edit %1.asm
link %1;

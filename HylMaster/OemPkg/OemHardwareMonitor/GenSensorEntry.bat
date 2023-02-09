@echo off

@cmd /u /c echo. >> Build\SensorEntryStr.uni
for /L %%i in (1, 1, %1) do (  
@cmd /u /c echo #string STR_SENSOR_NAME_%%i_STRING	   		#language eng "" >> Build\SensorEntryStr.uni
@cmd /u /c echo #string STR_SENSOR_VALUE_%%i_STRING	   		#language eng "" >> Build\SensorEntryStr.uni
)

set hm_pholder_str="Add here"
set /a hm_line=0
setlocal EnableDelayedExpansion

for /f "delims=:" %%i in ('findstr /N %hm_pholder_str% OemPkg\OemHardwareMonitor\HardwareMonitor.sd') do (
set hm_phonderl=%%i
)
cd.>Build\SensorEntry.sd
for /f "delims=" %%j in (OemPkg\OemHardwareMonitor\HardwareMonitor.sd) do ( 
set /a hm_line=!hm_line!+1
if not "%%j" == "" (
	if !hm_line! equ %hm_phonderl% (
		for /L %%i in (2, 1, %1) do (
		@echo.>>Build\SensorEntry.sd 
		@echo	 suppressif ideqval HM_V_DATA.SensorFound ^< %%i;>>Build\SensorEntry.sd
		@echo    text>>Build\SensorEntry.sd
		@echo    help  = STRING_TOKEN^(STR_NULL_STRING^),>>Build\SensorEntry.sd
		@echo    text  = STRING_TOKEN^(STR_SENSOR_NAME_%%i_STRING^),>>Build\SensorEntry.sd
		@echo    text  = STRING_TOKEN^(STR_SENSOR_VALUE_%%i_STRING^),>>Build\SensorEntry.sd
		@echo    flags = 0, key = 0;>>Build\SensorEntry.sd 
		@echo    endif;>>Build\SensorEntry.sd
		)
	) else (
		@echo. %%j >>Build\SensorEntry.sd
	)
) else (
	@echo NULL
	@echo. >>Build\SensorEntry.sd
)
)

@set hm_pholder_str=
@set hm_phonderl=
@set hm_line=
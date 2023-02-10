rem set IPMIBMC=ipmitool -I lanplus -H 10.8.21.182 -U admin -P admin
set IPMIBMC=ipmitool -I lanplus -H 10.8.21.182 -U admin -P admin
set "month=%date:~5,2%"
set "day=%date:~8,2%"
set "hour_ten=%time:~0,1%"
set "hour_one=%time:~1,1%"
set "minute=%time:~3,2%"
set "second=%time:~6,2%"
if "%hour_ten%" == " " (
    %IPMIBMC% sol activate > %month%%day%0%hour_one%%minute%%second%.txt
) else (
    %IPMIBMC% sol activate > %month%%day%%hour_ten%%hour_one%%minute%%second%.txt
)
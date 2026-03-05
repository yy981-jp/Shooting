@echo off

rem Debug tool

if "%1" == "" (
    echo error %%^1 is empty
    exit /b 1
)
if "%2" == "d" (
    set mode=debug
) else set mode=

pushd stgc

call stgc ../script/%1.stg %mode%

popd

if "%errorlevel%" == "0" (
    copy "stgc\%1.stg.dat" "assets\main.stg.dat" /y > nul
)

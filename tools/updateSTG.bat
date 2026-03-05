@echo off

rem Debug tool

if "%1" == "" (
    echo error %%^1 is empty
    exit /b 1
)
if "%2" == "d" (
    set mode=debug
) else set mode=

cd stgc

call stgc ../../script/%1.stg %mode%

if "%errorlevel%" == "0" (
    copy "%1.stg.dat" "../../../assets/main.stg.dat" > nul
)



:end
cd ..

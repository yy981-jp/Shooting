@echo off

rem Debug tool

if "%1" == "" (
    echo error %%^1 is empty
    exit /b 1
)

cd stgc

call stgc ../../script/%1.stg

if "%errorlevel%" == "0" (
    copy "%1.stg.dat" "../../Assets/main.stg.dat"
)



:end
cd ..

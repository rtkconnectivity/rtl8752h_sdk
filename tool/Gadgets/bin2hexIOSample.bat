@ECHO off
rem This batch file is used to convert binary image to HEX for keil downloading.

:: Get build bank from config.h
for /f "tokens=2,3" %%i in (..\mem_config.h) do (
    if %%i==BUILD_BANK set BUILD_BANK_CFG=%%j
)

REM echo BUILD_BANK_CFG = %BUILD_BANK_CFG%

:: Get Bank0 & Bank1 App start address
for /f "tokens=2,3" %%i in (..\flash_map.h) do (
    if %%i==BANK0_APP_ADDR set BANK0_APP_ADDR_CFG=%%j
    if %%i==BANK1_APP_ADDR set BANK1_APP_ADDR_CFG=%%j
)

REM echo BANK0_APP_ADDR_CFG = %BANK0_APP_ADDR_CFG%
REM echo BANK1_APP_ADDR_CFG = %BANK1_APP_ADDR_CFG%

:: Select right app address
if %BUILD_BANK_CFG% == 0 (
    set APP_ADDR_CFG=%BANK0_APP_ADDR_CFG%
) else (
    set APP_ADDR_CFG=%BANK1_APP_ADDR_CFG%
)

:: Convert image from binary to Intel HEX format
..\..\..\..\..\..\tool\Gadgets\srecord\srec_cat.exe %1   -binary -offset %APP_ADDR_CFG% -o %3 -intel

echo Output Image for Keil Download: %3
echo Build Bank: Bank%BUILD_BANK_CFG%
echo Download Address: %APP_ADDR_CFG%

::del .\bin\%2_MP.bin
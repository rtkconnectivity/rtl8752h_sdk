@echo off
set local_ver_value=0.0.0.0
set local_ver_default_value=0.0.0.1

set local_BinID_value=ID_APP_DATA0
set local_BinID_default_value=ID_APP_DATA1

goto StartHandle

::-------------step 1 ModifyDefaultVersionValue------------
echo =================================================
echo ==========step 1 ModifyDefaultVersionValue ======
echo =================================================
echo Before call getMPVersionValue local_ver_value is %local_ver_value%
call:getMPVersionValue local_ver_value
echo After call getMPVersionValue local_ver_value is %local_ver_value%

:: ensure default value is 0.0.0.1, actually it is not necessary
if %local_ver_value%==%local_ver_default_value% (
echo Local version value needn't be modified!
) else (
echo Call modifyVersion
call:modifyVersion %local_ver_default_value% %local_ver_value%
call:getMPVersionValue local_ver_value
echo After call modifyVersion local_ver_value value is %local_ver_value%
)

:: -------------step 2 ModifyDefaultBinIDValue------------
echo =================================================
echo ==========step 2 ModifyDefaultBinIDValue ======
echo =================================================
echo Before call getMPBinID local_BinID_value is %local_BinID_value%
call:getMPBinID local_BinID_value
echo After call getMPBinID local_BinID_value is %local_BinID_value%

:: ensure default value is ID_APP_DATA1, actually it is not necessary
if %local_BinID_value%==%local_BinID_default_value% (
echo Local BinID value needn't be modified!
) else (
echo Call modifyBinID
call:modifyBinID %local_BinID_default_value% %local_BinID_value%
call:getMPBinID local_BinID_value
echo After call modifyBinID local_BinID_value is %local_BinID_value%
)



:StartHandle
call:clearAllBinExceptRAWAppdata

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA1
call:RunAppData1_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData1_ver2

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA2
call:RunAppData2_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData2_ver2

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA3
call:RunAppData3_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData3_ver2

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA4
call:RunAppData4_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData4_ver2

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA5
call:RunAppData5_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData5_ver2

call:modifyVersion 0.0.0.1 0.0.0.2
call:modifyBinID ID_APP_DATA6
call:RunAppData6_ver1
call:modifyVersion 0.0.0.2 0.0.0.1
call:RunAppData6_ver2

:EndHandle

goto:eof
pause
::echo.&pause&goto:eof

:modifyVersion
echo.
echo. modifyVersion new version %~1 and old version %~2.
set "aa=Version="
:: new version
set "a=%aa%%~1"
:: old version
set "b=%aa%%~2"
echo input new version %a%
echo input old version %b%

call:getMPVersionValue local_ver_value

(for /f "tokens=1* delims=:" %%a in ('findstr /n .* mp.ini') do if "%%~nxb"=="%b%" (echo %a%) else echo,%%b)>$.ini
move /y $.ini mp.ini

call:getMPVersionValue local_ver_value
goto:eof


:modifyBinID
echo.
call:getMPBinID local_BinID_value
echo. modifyBinID new BinID %~1 and local_BinID_value %local_BinID_value%.
set "bb=BinID="
:: new version
set "a=%bb%%~1"
:: old version
set "b=%bb%%local_BinID_value%"
echo input new BinID %a%
echo input old BinID %b%

:: modify app data version
(for /f "tokens=1* delims=:" %%a in ('findstr /n .* mp.ini') do if "%%~nxb"=="%b%" (echo %a%) else echo,%%b)>$.ini
move /y $.ini mp.ini
call:getMPBinID local_BinID_value
goto:eof

:: will return value by global data
:getMPVersionValue
for /f "delims==, tokens=2" %%i in ('findstr /c:"Version" ".\mp.ini"') do (
set "version=%%i"
)
echo version value is %version% in mp.ini
set "%~1=%version%"
set return=%1
goto:eof

:getMPBinID
for /f "delims==, tokens=2" %%i in ('findstr /c:"BinID" ".\mp.ini"') do (
set "BinID=%%i"
)
echo BinID value is %BinID% in mp.ini
set "%~1=%BinID%"
set return=%1
goto:eof


:RunAppData1_ver1
:: if exist .\ImageAppData1_0.0.0.1.bin (
:: echo need delete firstly!
:: 1>NUL del .\ImageAppData1_0.0.0.1.bin
:: ) else (
:: echo don¡¯t need delete
:: )
1>NUL del .\ImageAppData1_0.0.0.1.bin
1>NUL del .\ImageAppData1_0.0.0.1_MP.bin
1>NUL ren SampleAppData1.bin AppData1_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data1 -p "AppData1_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData1_0.0.0.1.tmp ImageAppData1_0.0.0.1.bin
1>NUL ren AppData1_0.0.0.1_MP.bin ImageAppData1_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData1_0.0.0.1_MP.bin"
1>NUL ren AppData1_0.0.0.1.bin SampleAppData1.bin 
goto:eof

:RunAppData1_ver2
1>NUL del .\ImageAppData1_0.0.0.2.bin
1>NUL del .\ImageAppData1_0.0.0.2_MP.bin
1>NUL ren SampleAppData1.bin AppData1_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data1 -p "AppData1_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData1_0.0.0.2.tmp ImageAppData1_0.0.0.2.bin
1>NUL ren AppData1_0.0.0.2_MP.bin ImageAppData1_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData1_0.0.0.2_MP.bin"
1>NUL ren AppData1_0.0.0.2.bin SampleAppData1.bin 
goto:eof

:RunAppData2_ver1
1>NUL del .\ImageAppData2_0.0.0.1.bin
1>NUL del .\ImageAppData2_0.0.0.1_MP.bin
1>NUL ren SampleAppData2.bin AppData2_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data2 -p "AppData2_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData2_0.0.0.1.tmp ImageAppData2_0.0.0.1.bin
1>NUL ren AppData2_0.0.0.1_MP.bin ImageAppData2_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData2_0.0.0.1_MP.bin"
1>NUL ren AppData2_0.0.0.1.bin SampleAppData2.bin 
goto:eof

:RunAppData2_ver2
1>NUL del .\ImageAppData2_0.0.0.2.bin
1>NUL del .\ImageAppData2_0.0.0.2_MP.bin
1>NUL ren SampleAppData2.bin AppData2_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data2 -p "AppData2_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData2_0.0.0.2.tmp ImageAppData2_0.0.0.2.bin
1>NUL ren AppData2_0.0.0.2_MP.bin ImageAppData2_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData2_0.0.0.2_MP.bin"
1>NUL ren AppData2_0.0.0.2.bin SampleAppData2.bin 
goto:eof

:RunAppData3_ver1
1>NUL del .\ImageAppData3_0.0.0.1.bin
1>NUL del .\ImageAppData3_0.0.0.1_MP.bin
1>NUL ren SampleAppData3.bin AppData3_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data3 -p "AppData3_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData3_0.0.0.1.tmp ImageAppData3_0.0.0.1.bin
1>NUL ren AppData3_0.0.0.1_MP.bin ImageAppData3_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData3_0.0.0.1_MP.bin"
1>NUL ren AppData3_0.0.0.1.bin SampleAppData3.bin 
goto:eof

:RunAppData3_ver2
1>NUL del .\ImageAppData3_0.0.0.2.bin
1>NUL del .\ImageAppData3_0.0.0.2_MP.bin
1>NUL ren SampleAppData3.bin AppData3_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data3 -p "AppData3_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData3_0.0.0.2.tmp ImageAppData3_0.0.0.2.bin
1>NUL ren AppData3_0.0.0.2_MP.bin ImageAppData3_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData3_0.0.0.2_MP.bin"
1>NUL ren AppData3_0.0.0.2.bin SampleAppData3.bin 
goto:eof

:RunAppData4_ver1
1>NUL del .\ImageAppData4_0.0.0.1.bin
1>NUL del .\ImageAppData4_0.0.0.1_MP.bin
1>NUL ren SampleAppData4.bin AppData4_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data4 -p "AppData4_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData4_0.0.0.1.tmp ImageAppData4_0.0.0.1.bin
1>NUL ren AppData4_0.0.0.1_MP.bin ImageAppData4_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData4_0.0.0.1_MP.bin"
1>NUL ren AppData4_0.0.0.1.bin SampleAppData4.bin 
goto:eof

:RunAppData4_ver2
1>NUL del .\ImageAppData4_0.0.0.2.bin
1>NUL del .\ImageAppData4_0.0.0.2_MP.bin
1>NUL ren SampleAppData4.bin AppData4_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data4 -p "AppData4_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData4_0.0.0.2.tmp ImageAppData4_0.0.0.2.bin
1>NUL ren AppData4_0.0.0.2_MP.bin ImageAppData4_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData4_0.0.0.2_MP.bin"
1>NUL ren AppData4_0.0.0.2.bin SampleAppData4.bin 
goto:eof

goto:eof

:RunAppData5_ver1
1>NUL del .\ImageAppData5_0.0.0.1.bin
1>NUL del .\ImageAppData5_0.0.0.1_MP.bin
1>NUL ren SampleAppData5.bin AppData5_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data5 -p "AppData5_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData5_0.0.0.1.tmp ImageAppData5_0.0.0.1.bin
1>NUL ren AppData5_0.0.0.1_MP.bin ImageAppData5_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData5_0.0.0.1_MP.bin"
1>NUL ren AppData5_0.0.0.1.bin SampleAppData5.bin 
goto:eof

:RunAppData5_ver2
1>NUL del .\ImageAppData5_0.0.0.2.bin
1>NUL del .\ImageAppData5_0.0.0.2_MP.bin
1>NUL ren SampleAppData5.bin AppData5_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data5 -p "AppData5_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData5_0.0.0.2.tmp ImageAppData5_0.0.0.2.bin
1>NUL ren AppData5_0.0.0.2_MP.bin ImageAppData5_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData5_0.0.0.2_MP.bin"
1>NUL ren AppData5_0.0.0.2.bin SampleAppData5.bin 
goto:eof

:RunAppData6_ver1
1>NUL del .\ImageAppData6_0.0.0.1.bin
1>NUL del .\ImageAppData6_0.0.0.1_MP.bin
1>NUL ren SampleAppData6.bin AppData6_0.0.0.1.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data6 -p "AppData6_0.0.0.1.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData6_0.0.0.1.tmp ImageAppData6_0.0.0.1.bin
1>NUL ren AppData6_0.0.0.1_MP.bin ImageAppData6_0.0.0.1_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData6_0.0.0.1_MP.bin"
1>NUL ren AppData6_0.0.0.1.bin SampleAppData6.bin 
goto:eof

:RunAppData6_ver2
1>NUL del .\ImageAppData6_0.0.0.2.bin
1>NUL del .\ImageAppData6_0.0.0.2_MP.bin
1>NUL ren SampleAppData6.bin AppData6_0.0.0.2.bin
..\..\..\..\..\tool\prepend_header\prepend_header.exe -t app_data6 -p "AppData6_0.0.0.2.bin" -m 1 -c sha256 -b 12
1>NUL ren AppData6_0.0.0.2.tmp ImageAppData6_0.0.0.2.bin
1>NUL ren AppData6_0.0.0.2_MP.bin ImageAppData6_0.0.0.2_MP.bin
..\..\..\..\..\tool\md5\md5.exe "ImageAppData6_0.0.0.2_MP.bin"
1>NUL ren AppData6_0.0.0.2.bin SampleAppData6.bin 
goto:eof

:clearAllBinExceptRAWAppdata
1>NUL attrib +r .\SampleAppData1.bin
1>NUL attrib +r .\SampleAppData2.bin
1>NUL attrib +r .\SampleAppData3.bin
1>NUL attrib +r .\SampleAppData4.bin
1>NUL attrib +r .\SampleAppData5.bin
1>NUL attrib +r .\SampleAppData6.bin
1>NUL 2>NUL  del .\*.bin
1>NUL attrib -r .\SampleAppData1.bin
1>NUL attrib -r .\SampleAppData2.bin
1>NUL attrib -r .\SampleAppData3.bin
1>NUL attrib -r .\SampleAppData4.bin
1>NUL attrib -r .\SampleAppData5.bin
1>NUL attrib -r .\SampleAppData6.bin
goto:eof
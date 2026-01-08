@echo off
set keil_compiler_include_floder=%1
set full_name_path=%2
set linker_output_file_name=%3
set project_floder_path=%4
set objects_floder_path=%5
::echo %keil_compiler_include_floder%
::echo %full_name_path%
::echo %linker_output_file_name%

%keil_compiler_include_floder%\..\bin\fromelf.exe --bin -o  "bin/" "%full_name_path%"
REM ---------------------------------------------
REM Optional: Generate a disassembly file for debugging.
REM To enable, remove the "::" at the beginning of the command.
REM Note: The "--interleave=source" flag interleaves source code with disassembled code.
REM This helps in correlating the source with its machine code, but the file will
REM contain your source code. Make sure to protect its confidentiality.
REM ---------------------------------------------
::%keil_compiler_include_floder%\..\bin\fromelf.exe -acd --interleave=source -o "bin\%linker_output_file_name%.disasm" "%full_name_path%"
..\..\..\..\..\..\tool\Gadgets\prepend_header\prepend_header.exe -b 16 -t app_code -p "bin\%linker_output_file_name%.bin" -m 1 -i "..\mp.ini" -c sha256 -a "..\..\..\..\..\..\tool\Gadgets\key.json" -r "..\..\..\..\..\..\tool\Gadgets\rtk_rsa.pem"
..\..\..\..\..\..\tool\Gadgets\md5\md5.exe "bin\%linker_output_file_name%.bin"
..\..\..\..\..\..\tool\Gadgets\md5\md5.exe "bin\%linker_output_file_name%_MP.bin"
del "bin\%linker_output_file_name%_MP.bin"

..\..\..\..\..\..\tool\Gadgets\bin2hexIOSample.bat "%project_floder_path%bin\%linker_output_file_name%.bin" %linker_output_file_name% "%objects_floder_path%%linker_output_file_name%.hex"

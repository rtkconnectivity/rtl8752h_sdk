@echo off
echo after_build_special bat

set linker_output_file_name=%1

echo %linker_output_file_name%

..\..\..\tool\srec_cat "bin\%linker_output_file_name%.bin" -binary -offset 0x0082E000 -o "bin\%linker_output_file_name%.hex" -intel

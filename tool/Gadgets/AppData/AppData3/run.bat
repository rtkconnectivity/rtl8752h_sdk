..\..\prepend_header\prepend_header.exe -t app_data3 -p "SampleAppData3.bin" -m 1 -c sha256 -b 16
1>NUL del SampleImageAppData3.bin
1>NUL del SampleImageAppData3_MP.bin
1>NUL ren SampleAppData3.tmp SampleImageAppData3.bin
1>NUL ren SampleAppData3_MP.bin SampleImageAppData3_MP.bin
..\..\md5\md5.exe "SampleImageAppData3_MP.bin"
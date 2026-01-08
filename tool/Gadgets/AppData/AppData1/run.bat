..\..\prepend_header\prepend_header.exe -t app_data1 -p "SampleAppData1.bin" -m 1 -c sha256 -b 16
1>NUL del SampleImageAppData1.bin
1>NUL del SampleImageAppData1_MP.bin
1>NUL ren SampleAppData1.tmp SampleImageAppData1.bin
1>NUL ren SampleAppData1_MP.bin SampleImageAppData1_MP.bin
..\..\md5\md5.exe "SampleImageAppData1_MP.bin"
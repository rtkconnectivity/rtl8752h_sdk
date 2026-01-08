..\..\prepend_header\prepend_header.exe -t app_data4 -p "SampleAppData4.bin" -m 1 -c sha256 -b 16
1>NUL del SampleImageAppData4.bin
1>NUL del SampleImageAppData4_MP.bin
1>NUL ren SampleAppData4.tmp SampleImageAppData4.bin
1>NUL ren SampleAppData4_MP.bin SampleImageAppData4_MP.bin
..\..\md5\md5.exe "SampleImageAppData4_MP.bin"
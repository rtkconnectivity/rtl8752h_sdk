..\..\prepend_header\prepend_header.exe -t app_data5 -p "SampleAppData5.bin" -m 1 -c sha256 -b 16
1>NUL del SampleImageAppData5.bin
1>NUL del SampleImageAppData5_MP.bin
1>NUL ren SampleAppData5.tmp SampleImageAppData5.bin
1>NUL ren SampleAppData5_MP.bin SampleImageAppData5_MP.bin
..\..\md5\md5.exe "SampleImageAppData5_MP.bin"
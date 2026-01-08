..\..\prepend_header\prepend_header.exe -t userdata1 -p "SampleUserData1.bin" -m 1 -c sha256 -b 16
1>NUL ren SampleUserData1.tmp SampleImageUserData1.bin
1>NUL ren SampleUserData1_MP.bin SampleImageUserData1_MP.bin
..\..\md5\md5.exe "SampleImageUserData1_MP.bin"
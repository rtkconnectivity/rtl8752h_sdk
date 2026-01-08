..\..\prepend_header\prepend_header.exe -t userdata2 -p "SampleUserData2.bin" -m 1 -c sha256 -b 16
1>NUL ren SampleUserData2.tmp SampleImageUserData2.bin
1>NUL ren SampleUserData2_MP.bin SampleImageUserData2_MP.bin
..\..\md5\md5.exe "SampleImageUserData2_MP.bin"
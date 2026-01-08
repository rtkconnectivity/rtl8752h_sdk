@echo off
:: generate sign key
..\signImage.exe -g rsa -r .\rtk_rsa -i rsa_3072
..\signImage.exe -g rsa -r .\rtk_rsa_2048 -i rsa_2048
::..\signImage.exe -g rsa -r .\rtk_rsa2
::..\signImage.exe -g rsa -r .\rtk_rsa3
::..\signImage.exe -g rsa -r .\rtk_rsa4
::..\signImage.exe -g ecdsa -r \rtk_ecdsa
::..\signImage.exe -g ed25519 -r \rtk_ed25519

..\..\srecord\srec_cat .\rtk_rsa_hash.bin  -Binary -o ".\public_key_hash.c"  -C-Array public_key_hash  -INClude
..\..\srecord\srec_cat .\rtk_rsa_2048_hash.bin  -Binary -o ".\public_key_2048_hash.c"  -C-Array public_key_2048_hash  -INClude
::..\srec_cat .\rtk_rsa2_hash.bin -Binary -o ".\public_key2_hash.c" -C-Array public_key2_hash -INClude
::..\srec_cat .\rtk_rsa3_hash.bin -Binary -o ".\public_key3_hash.c" -C-Array public_key3_hash -INClude
::..\srec_cat .\rtk_rsa4_hash.bin -Binary -o ".\public_key4_hash.c" -C-Array public_key4_hash -INClude
::..\srec_cat .\rtk_ecdsa_hash.bin -Binary -o ".\ecdsa_public_key_hash.c" -C-Array public_key_hash -INClude
::..\srec_cat .\rtk_ed25519_hash.bin -Binary -o ".\ed25519_public_key_hash.c" -C-Array public_key_hash -INClude

::remove 512bytes MP Header
::bash.exe -c "dd if=OTAHeader_Bank0_MP.bin of=OTAHeader_Bank0.bin bs=1 skip=512"

::sign image
::prepend_header.exe -t rom_patch -b 16 -p ".\ns_patch.bin" -m 1 -r ".\rtk_rsa.pem"
::prepend_header.exe -t ota_header -b 16 -p ".\OTAHeader_Bank0.bin" -m 1 -r ".\rtk_rsa.pem"

::..\srec_cat ".\OTAHeader_Bank0.bin"   -binary -offset 0x0802000 -o ".\OTAHeader_Bank0.hex" -intel



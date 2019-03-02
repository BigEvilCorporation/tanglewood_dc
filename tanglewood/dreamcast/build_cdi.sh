/opt/toolchains/dc/sh-elf/bin/sh-elf-objcopy -R .stack -O binary ../_Build/dreamcast/_intermediates_/dreamcast-release/bigevilcorp/tanglewood/tanglewood/tanglewood.elf temp/boot.bin
/opt/toolchains/dc/kos/utils/scramble/scramble temp/boot.bin disc/1ST_READ.BIN
cp -rv ../assets disc/
mkisofs -G disc/IP.BIN -J -l -r -V disc -o temp/data.iso disc
tools/cdi4dc temp/data.iso output/tanglewd.cdi -d
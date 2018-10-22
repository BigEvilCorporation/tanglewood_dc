REM "..\_Build\dreamcast\_intermediates_\dreamcast-release\bigevilcorp\tanglewood_emu\tanglewood\tanglewood.elf"
C:\msys32\opt\toolchains\dc\sh-elf\bin\sh-elf-objcopy.exe -R .stack -O binary "..\_Build\dreamcast\_intermediates_\dreamcast-release\bigevilcorp\tanglewood\tanglewood\tanglewood.elf" temp\boot.bin
C:\msys32\opt\toolchains\dc\kos\utils\scramble\scramble.exe temp\boot.bin disc\1ST_READ.BIN
xcopy /E /Y ..\assets\*.* disc\assets\
tools\mkisofs.exe -G disc\IP.BIN -J -l -r -V disc -o temp\data.iso disc
tools\cdi4dc.exe temp\data.iso output\tanglewd.cdi -d
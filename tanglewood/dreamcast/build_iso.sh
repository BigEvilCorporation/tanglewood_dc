cp -rv ../assets disc/
mkisofs -G disc/IP.BIN -J -l -r -V disc -o output/data_track.iso disc

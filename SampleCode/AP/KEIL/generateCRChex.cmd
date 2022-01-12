

# input file
obj\APROM_application.bin -binary
#obj\APROM_application.bin -binary -offset 0x5000

-crop 0x0000 0x7A000 -offset 0x5000

# produce the output file
-Output
obj\APROM_application.hex -intel


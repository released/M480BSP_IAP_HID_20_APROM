
# input file
obj\APROM_application.bin -binary	

# just keep code area for CRC calculation
# reserve field , from xxxx , to xxxx
# target size : 0x80000 , 0x80000 - 0x1000*5 for boot loader code size (4K page)
-crop 0x0000 0x6FFFC

# fill code area with 0xFF
# fill 0xFF into the field , from xxxx , to xxxx				
-fill 0xFF 0x0000 0x6FFFC					

# select checksum algorithm
-crc32-l-e 0x6FFFC			

# keep the CRC itself
-crop 0x6FFFC 0x70000

# output hex display											
-Output 
- 
-HEX_Dump

# input file
#obj\APROM_application.bin -binary		

# just keep code area for CRC calculation
#-crop 0x0000 0x6FFFC	

# fill code area with 0xFF	
#-fill 0xFF 0x0000 0x6FFFC

# produce the output file
#-Output
#obj\APROM_application.bin -binary


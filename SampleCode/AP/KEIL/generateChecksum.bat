::srec_cat template.hex -intel -Checksum_Positive_Big_Endian 0x20000 4 -crop 0x20000 0x20004 -o -HEX_Dump
::srec_cat template.hex -intel -Checksum_Positive_Big_Endian 0x20000 4 -o -HEX_Dump

::srec_cat template.hex -intel -checksum-negative-big-endian 0x5000 4 -o -HEX_Dump
::srec_cat template.hex -intel -checksum-negative-big-endian 0x5000 2 2 -o -HEX_Dump
::srec_cat template.hex -intel -Checksum_Positive_Big_Endian 0x5000 2 2 -o -HEX_Dump

::srec_cat template.hex -intel -crc32-l-e 0x5000 -crc32-b-e 0x5010 -checksum-n-b-e 0x5020 2 -checksum-n-l-e 0x5030 2 -checksum-p-b-e 0x5040 2 -checksum-p-l-e 0x5050 2 -o -HEX_Dump

srec_cat @generateChecksum.cmd

srec_cat @generateCRCbinary.cmd

srec_cat @generateCRChex.cmd

::srec_cat template.hex -intel -checksum-negative-big-endian 0x5000 1 -o -HEX_Dump

::pause

				
# M480BSP_IAP_HID_20_APROM
 M480BSP_IAP_HID_20_APROM


update @ 2022/09/08

1. add digit 1 under application code @ APROM 0x10 000 address , to erase checksum and return to boot code @ APROM 0x00 address


update @ 2022/01/12

1. Scenario notice:

	- Boot loader project : IAP_HID_20 
	
		- under sct file (hid_iap.sct) , will allocate flash size (target MCU flash size : 512K , use flash_calculate.xlsx to calculate)
					
				APROM_Bootloader.bin : 0x00000 0x10000 (reserve 64K size , to store extra boot loader code , total boot loader size : 64K)
	
		- when power on , will check power on source (ex : power on reset , nReset)
	
		- use CRC to calculate Application code checksum (address : 0x10000 , to 0x80 000 - 4)
		
		- load Application code checksum , from specific address (at 0x7FFFC)
		
		- if two checksum result are different , will trap in Boot loader , and wait for ISP tool hand-shaking
		
		- if two checksum result are the same , will jump to Application code

		- if reset from application code , will entry timeout counting (5sec) , jump to application code if not receive ISP tool command

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/Under_APROM_z.jpg)
	
	- Application code project : AP
	
		- use SRecord , to calculate application code checksum 
		
		- SRecord file : srec_cat.exe , generateChecksum.bat (will execute generateChecksum.cmd , generateCRCbinary.cmd , generateCRChex)
					
		- under generateChecksum.bat will execute generateChecksum.cmd , generateCRCbinary.cmd , generateCRChex.cmd
	
		- generateChecksum.cmd : calculate checksum by load the original binary file , and display on KEIL project
		
		- generateCRCbinary.cmd : calculate checksum by load the original binary file , and fill 0xFF , range up to 0x6FFFC
		
		- generateCRChex.cmd : conver binary file into hex file
		
		- at KEIL output file , file name is APROM_application , under \obj folder , 
	
			which mapping to generateChecksum.cmd , generateCRCbinary.cmd , generateCRChex.cmd , generateCRChexOffset.cmd
	
			modify the file name in KEIL project , also need to modify the file name in these 4 generate***.cmd		
			
		- check sum calculate will start from 0 to 0x6FFFC , and store in (0x80000 - 4) address (0x7FFFC)
		
		- generateCRChexOffset.cmd , will shift the hex , from 0x00 to 0x10000 
		
		- after project compile finish , binary size will be 448K (total application code size : 0x70000)
		
		- under terminal , use keyboard , 'z' , 'Z' , will RESET and return to boot loader
		
![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/Under_APROM_z.jpg)		
		
		- use ISP tool , to programming Application code project binary (448K) , when under Boot loader flow		
		
		- if plan to use KEIL to programming flash with CRC , refer to below setting
		
![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/program_by_KEIL.jpg)
		
			
2. Flash allocation start address

	- APROM_Bootloader.bin : 0x0000 0x10000
	
	- Application code : 0x10000 0x80000
		
	- Chcecksum storage : 0x7EFFC

3. Function assignment

	- debug port : UART0 (PB12/PB13) , in Boot loader an Application code project
	
	- enable CRC , Timer1 module
	
4. Need to use ICP tool , to programm boot loader project file (APROM_Bootloader.bin , Application code )

below is boot loader APROM setting and program APROM , config 

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/APROM_ICP.jpg)

below is boot loader project , Config setting 

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/Config_Bits.jpg)

in Application project , press 'z' , 'Z' will reset to Boot loader 

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/Under_APROM_z.jpg)

in Application project , press '1' will CLEAR checksum and reset to Boot loader 

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/Under_APROM_1.jpg)

with ISP tool , set connect and wait USB connection from APROM jump to LDROM , and programming application code

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/ISP_connect.jpg)

below is log message , from boot loader , to application code

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/regular_power_on_check_checksum.jpg)

6. if checksum incorret , trap in boot loader 

![image](https://github.com/released/M480BSP_IAP_HID_20_APROM/blob/main/error_checksum_stay_in_boot_loader.jpg)



/**************************************************************************//**
 * @file     isp_user.h
 * @brief    ISP Command header file
 * @version  0x32
 * @date     14, June, 2017
 *
 * @note
 * Copyright (C) 2017-2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef ISP_USER_H
#define ISP_USER_H


#define LDROM_DEBUG(format, args...) 		printf("\033[1;36m" "[LDROM]" format "\033[0m", ##args)


#define FW_VERSION                          0x32

/*
		flash page : 4K (0x1000)
		boot loader in APROM : 4K*5 (pos : start from 0x00000)
		dataflash : 4K , 0x7F000 

		application code address : 0x5000 (pos : start from 0x5000)
		application code checksum : 0x7EFFC
		application code size : 0x7A000

*/
#define FLASH_SIZE				      		(0x00080000UL)	// 512K
#define TARGET_BOOTLOADER_SIZE      		(0x00005000UL)	// 20K , IAP
#define TARGET_FLASH_SIZE      				(0x00001000UL)	// 1K
#define APROM_APPLICATION_SIZE      		(FLASH_SIZE - TARGET_BOOTLOADER_SIZE - TARGET_FLASH_SIZE)	//(0x0007B000UL - 0x1000)

#include "fmc_user.h"

#define CMD_UPDATE_APROM            		0x000000A0
#define CMD_UPDATE_CONFIG           		0x000000A1
#define CMD_READ_CONFIG             		0x000000A2
#define CMD_ERASE_ALL               		0x000000A3
#define CMD_SYNC_PACKNO             		0x000000A4
#define CMD_GET_FWVER               		0x000000A6
#define CMD_RUN_APROM               		0x000000AB
#define CMD_RUN_LDROM               		0x000000AC
#define CMD_RESET                   		0x000000AD
#define CMD_CONNECT                 		0x000000AE
#define CMD_DISCONNECT              		0x000000AF

#define CMD_GET_DEVICEID            		0x000000B1
		
#define CMD_UPDATE_DATAFLASH        		0x000000C3
#define CMD_WRITE_CHECKSUM          		0x000000C9
#define CMD_GET_FLASHMODE           		0x000000CA
		
#define CMD_RESEND_PACKET           		0x000000FF
		
#define V6M_AIRCR_VECTKEY_DATA      		0x05FA0000UL
#define V6M_AIRCR_SYSRESETREQ       		0x00000004UL

extern void GetDataFlashInfo(uint32_t *addr, uint32_t *size);
extern uint32_t GetApromSize(void);
extern int ParseCmd(unsigned char *buffer, uint8_t len);
extern uint32_t g_apromSize, g_dataFlashAddr, g_dataFlashSize;

uint8_t rtc_read_magic_tag(void);
void rtc_write_magic_tag(uint8_t tag);


extern __align(4) uint8_t usb_rcvbuf[];
extern __align(4) uint8_t usb_sendbuf[];
extern __align(4) uint8_t response_buff[64];
#endif  // #ifndef ISP_USER_H

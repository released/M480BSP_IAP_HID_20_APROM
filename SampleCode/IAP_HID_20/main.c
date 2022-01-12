/***************************************************************************//**
 * @file     main.c
 * @brief    ISP tool main function
 * @version  0x32
 * @date     14, June, 2017
 *
 * @note
 * Copyright (C) 2017-2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>

#include "targetdev.h"
#include "hid_transfer.h"
  

#define PLLCON_SETTING          			(CLK_PLLCTL_192MHz_HIRC)
#define PLL_CLOCK               			(192000000)
#define HCLK_DIV                        	(1)
#define USBD_DIV                        	(4)

#define TIMEOUT_INTERVAL    				(5)   // sec
__IO uint32_t timeout_cnt = 0;

#define DEBUG_UART_PORT						(UART0)
#define DEBUG_UART_PORT_IRQn				(UART0_IRQn)
#define DEBUG_UART_IRQHandler				(UART0_IRQHandler)



// TODO:

/*
	boot loader code range :
	LDROM : 4K

	M481 minimum APROM size : 128K = 0x20 000

	for checksum storage start address :	// 4K
	0x1D 000 ~ 0x1D FFF
	
	LDROM extra storage in APROM start address : // 4K
	0x1E 000 ~ 0x1E FFF

	DATA FALSH in APROM start address : // 4K
	0x1F 000 ~ 0x1F FFF
*/

//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint8_t tag;
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    LDROM_DEBUG("Reset Source <0x%08X>\r\n", src);
   
    tag = rtc_read_magic_tag();

	#if 1	// use button : PG15 to control update			  
	if (PG15 == FALSE)
	{
		rtc_write_magic_tag(0);
		LDROM_DEBUG("Enter BOOTLOADER w/ Button pressed\r\n");
		return TRUE;
	}
	#endif
	
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        if (tag == 0xA5) {
            rtc_write_magic_tag(0);
            LDROM_DEBUG("Enter BOOTLOADER from APPLICATION\r\n");
            return TRUE;
        } else if (tag == 0xBB) {
            rtc_write_magic_tag(0);
            LDROM_DEBUG("Upgrade finished...\r\n");
            return FALSE;
        } else {
            LDROM_DEBUG("Enter BOOTLOADER from POR\r\n");
            return FALSE;
        }
    } else if (src & SYS_RSTSTS_PINRF_Msk){
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        LDROM_DEBUG("Enter BOOTLOADER from nRESET pin\r\n");
        return FALSE;
    }
    
    LDROM_DEBUG("Enter BOOTLOADER from unhandle reset source\r\n");
    return FALSE;
}


uint32_t caculate_crc32_checksum(uint32_t start, uint32_t size)
{
    volatile uint32_t addr, data;

    CRC_Open(CRC_32, (CRC_WDATA_RVS | CRC_CHECKSUM_RVS | CRC_CHECKSUM_COM), 0xFFFFFFFF, CRC_CPU_WDATA_32);
    
    for(addr = start; addr < size; addr += 4){
        data = FMC_Read(addr);
        CRC_WRITE_DATA(data);
    }
    return CRC_GetChecksum();
}

uint8_t verify_application_chksum(void)
{
    uint32_t chksum_cal, chksum_app;
       
    const uint32_t bootloader_size = TARGET_BOOTLOADER_SIZE;

    
    LDROM_DEBUG("Verify Checksum\r\n");
    
    chksum_cal = caculate_crc32_checksum(0x00000000 + bootloader_size, (g_apromSize - 4) + bootloader_size);//(g_apromSize - FMC_FLASH_PAGE_SIZE)
    LDROM_DEBUG("Caculated .....<0x%08X>\r\n", chksum_cal);
    
    chksum_app = FMC_Read( (g_apromSize - 4) + bootloader_size); 
    LDROM_DEBUG("In APROM ......<0x%08X>\r\n", chksum_app);
    
    if (chksum_cal == chksum_app) {
        LDROM_DEBUG("Verify ........<PASS>\r\n");
        return TRUE;
    } else {
        LDROM_DEBUG("Verify ........<FAIL>\r\n");
        return FALSE;
    }
}

void BTN_Init(void)	//ETM M487 , BTN1 : PG15
{
	SYS->GPG_MFPH = (SYS->GPG_MFPH & ~(SYS_GPG_MFPH_PG15MFP_Msk)) | (SYS_GPG_MFPH_PG15MFP_GPIO);
	GPIO_SetMode(PG,BIT15,GPIO_MODE_INPUT);
	
}


void UARTx_Process(void)
{
	uint8_t res = 0;

	res = UART_READ(DEBUG_UART_PORT);

	if (res > 0x7F)
	{
		LDROM_DEBUG("invalid command\r\n");
	}
	else
	{
		switch(res)
		{
	
			case '1':

				break;	

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
			
				break;		
			
		}
	}
}

void DEBUG_UART_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(DEBUG_UART_PORT, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(DEBUG_UART_PORT) == 0)
        {
//			set_flag(flag_uart_rx,ENABLE);
			UARTx_Process();
        }
    }

    if(DEBUG_UART_PORT->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(DEBUG_UART_PORT, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }
}

void DEBUG_UART_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(DEBUG_UART_PORT, 115200);

	/* Set UART receive time-out */
	UART_SetTimeoutCnt(DEBUG_UART_PORT, 20);

	DEBUG_UART_PORT->FIFO &= ~UART_FIFO_RFITL_4BYTES;
	DEBUG_UART_PORT->FIFO |= UART_FIFO_RFITL_8BYTES;

	/* Enable UART Interrupt - */
	UART_ENABLE_INT(DEBUG_UART_PORT, UART_INTEN_RDAIEN_Msk | UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk);
	
	NVIC_EnableIRQ(DEBUG_UART_PORT_IRQn);

	LDROM_DEBUG("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	LDROM_DEBUG("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	LDROM_DEBUG("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	LDROM_DEBUG("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	LDROM_DEBUG("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
        timeout_cnt++;
    }
}


void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
//    TIMER_Start(TIMER1);
}

/*--------------------------------------------------------------------------*/
void SYS_Init(void)
{
    uint32_t volatile i;
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);
    CLK->PWRCTL |= (CLK_PWRCTL_HIRCEN_Msk | CLK_PWRCTL_HXTEN_Msk);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_HIRC;

    // Waiting for clock switching ok
    while (CLK->STATUS & CLK_STATUS_CLKSFAIL_Msk);

    CLK->PLLCTL = CLK_PLLCTL_PD_Msk; // Disable PLL
    CLK->PLLCTL = 0x8842E;           // Enable PLL & set frequency 192MHz

    while (!(CLK->STATUS & CLK_STATUS_PLLSTB_Msk));

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    while ((CLK->STATUS & CLK_STATUS_PLLSTB_Msk) != CLK_STATUS_PLLSTB_Msk);

    CLK->CLKDIV0 = CLK->CLKDIV0 & (~CLK_CLKDIV0_HCLKDIV_Msk);   /* PLL/1 */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;
    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;
    SYS->USBPHY &= ~SYS_USBPHY_HSUSBROLE_Msk;    /* select HSUSBD */
    /* Enable USB PHY */
    SYS->USBPHY = (SYS->USBPHY & ~(SYS_USBPHY_HSUSBROLE_Msk | SYS_USBPHY_HSUSBACT_Msk)) | SYS_USBPHY_HSUSBEN_Msk;

    for (i = 0; i < 0x1000; i++);  // delay > 10 us

    SYS->USBPHY |= SYS_USBPHY_HSUSBACT_Msk;
    /* Enable IP clock */
    CLK->AHBCLK |= CLK_AHBCLK_HSUSBDCKEN_Msk;   /* USBD20 */
    /* Enable module clock */
    CLK_EnableModuleClock(USBD_MODULE);
    CLK_EnableModuleClock(ISP_MODULE);

    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

//    CLK_EnableModuleClock(UART1_MODULE);
//    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HIRC, CLK_CLKDIV0_UART1(1));

    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    CLK_EnableModuleClock(RTC_MODULE);
    CLK_SetModuleClock(RTC_MODULE, CLK_CLKSEL3_RTCSEL_LIRC,  NULL);

    CLK_EnableModuleClock(CRC_MODULE);
	
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

//    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
//    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB2MFP_UART1_RXD | SYS_GPB_MFPL_PB3MFP_UART1_TXD);
	
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    /* Init UART to 115200-8n1 */
	
	DEBUG_UART_Init();

	TIMER1_Init();

	BTN_Init();

	CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
    // Enable FMC and APROM update
    //
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();
    
    //
    // Get APROM and data flash information
    //
    g_apromSize = APROM_APPLICATION_SIZE;
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    
    //
    // Stay in BOOTLOADER or jump to APPLICATION
    //
    if (!check_reset_source()) 
	{
        if (verify_application_chksum()) 
		{
            goto _APROM;
        } 
		else 
		{
            LDROM_DEBUG("Stay in BOOTLOADER (checksum)\r\n");
        }
    } 
	else 
    {
        LDROM_DEBUG("Stay in BOOTLOADER (others)\r\n");
        
        //
        // start timer
        //
        LDROM_DEBUG("Time-out counter start....\r\n");
        TIMER_Start(TIMER1);
    }

    HSUSBD_Open(NULL, NULL, NULL);
    /* Endpoint configuration */
    HID_Init();
    /* Enable USBD interrupt */
    NVIC_EnableIRQ(USBD20_IRQn);
    /* Start transaction */
    HSUSBD_CLR_SE0();
    
    while (1)
    {
		if (bUsbDataReady == TRUE)
		{
			ParseCmd((uint8_t *)usb_rcvbuf, 64);
			EPA_Handler();
			bUsbDataReady = FALSE;
			timeout_cnt = 0;
		}

        if (timeout_cnt > TIMEOUT_INTERVAL) {
            LDROM_DEBUG("Time-out, perform CHIP_RST\r\n");
            while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));
        
            // Reset chip to enter bootloader
            SYS_UnlockReg();
            SYS_ResetChip();
        }
    }

_APROM:
    LDROM_DEBUG("Jump to <APPLICATION>\r\n");
    while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));
    
    FMC_SetVectorPageAddr(TARGET_BOOTLOADER_SIZE);             	/* Set vector remap to APROM address 0x0      */
    FMC_SET_APROM_BOOT();                   					/* Change boot source as APROM                */
    SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;    					/* Let CPU reset. Will boot from APROM.       */
    
    /* Trap the CPU */
    while (1);   

	
}

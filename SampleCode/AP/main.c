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
#include "NuMicro.h"

#define APROM_1
//#define APROM_2

#define PLLCON_SETTING          				CLK_PLLCTL_192MHz_HIRC
#define PLL_CLOCK               				192000000
#define HCLK_DIV                        		1

#define LED_R									(PH0)
#define LED_Y									(PH1)
#define LED_G									(PH2)

#define DEBUG_UART_PORT							(UART0)
#define DEBUG_UART_PORT_IRQn					(UART0_IRQn)
#define DEBUG_UART_IRQHandler					(UART0_IRQHandler)

volatile uint32_t counter_tick = 0;

void rtc_write_magic_tag(uint8_t tag)
{
    RTC_EnableSpareAccess();

    RTC->RWEN = RTC_WRITE_KEY;
    while(!(RTC->RWEN & RTC_RWEN_RWENF_Msk));
    
    RTC_WRITE_SPARE_REGISTER(0, tag);
    
    printf("Write MagicTag <0x%02X>\r\n", tag);
}


void tick_counter(void)
{
	counter_tick++;
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}


void UARTx_Process(void)
{
	uint8_t res = 0;

	res = UART_READ(DEBUG_UART_PORT);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
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
	            rtc_write_magic_tag(0xA5);
	        
	            printf("Perform CHIP_RST to enter BOOTLOADER\r\n");
	            while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));
	        
	            // Reset chip to enter bootloader
	            SYS_UnlockReg();
	            SYS_ResetChip();
			
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

	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	
}


void LED_Init(void)
{
	GPIO_SetMode(PH,BIT0,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT1,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT2,GPIO_MODE_OUTPUT);
	
}

void TMR1_IRQHandler(void)
{
	static uint32_t LOG = 0;	
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);

		tick_counter();

		#if defined (APROM_1)
		if ((get_tick() % 1000) == 0)
		{
        	printf("1)%s : %4d\r\n",__FUNCTION__,LOG++);
			LED_G ^= 1;
		}
		#endif

		#if defined (APROM_2)
		if ((get_tick() % 500) == 0)
		{
        	printf("2)%s : %4d\r\n",__FUNCTION__,LOG++);
			LED_Y ^= 1;
		}
		#endif

    }
}


void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);
    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

	#if 0
    /* Enable UART clock */
    CLK_EnableModuleClock(UART1_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HIRC, CLK_CLKDIV0_UART1(1));
	#else
    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
	#endif

    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);


    CLK_EnableModuleClock(RTC_MODULE);
    CLK_SetModuleClock(RTC_MODULE, CLK_CLKSEL3_RTCSEL_LIRC,  NULL);
	
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

//    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
//    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB2MFP_UART1_RXD | SYS_GPB_MFPL_PB3MFP_UART1_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
	
	DEBUG_UART_Init();

	LED_Init();
	TIMER1_Init();
	
    while (1)
    {

    };
	
}

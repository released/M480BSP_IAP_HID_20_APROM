/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include <stdarg.h>

#include "targetdev.h"
#include "hid_transfer.h"

#include "misc_config.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_REVERSE1                   			(flag_PROJ_CTL.bit1)
#define FLAG_PROJ_REVERSE2                 				(flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                              (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_systick = 0;
volatile uint32_t counter_tick = 0;

#define TIMEOUT_INTERVAL    	                        (5)   // sec
__IO uint32_t timeout_cnt = 0;
uint8_t timeout_flag = 0;

#define DEBUG_UART_PORT						            (UART0)
#define DEBUG_UART_PORT_IRQn				            (UART0_IRQn)
#define DEBUG_UART_IRQHandler				            (UART0_IRQHandler)

// #define ENABLE_SW_CRC32

#if defined (ENABLE_SW_CRC32)
#define USE_SRAM_TABLE
// #define USE_FLASH_TABLE

unsigned long state = 0xFFFFFFFF;

#if defined (USE_FLASH_TABLE)
const uint32_t table[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
#elif defined (USE_SRAM_TABLE)
unsigned long table[256];
#define POLYNOMIAL 0xedb88320
void calculate_table(void)
{
    unsigned b = 0;
    unsigned long v = 0;
    int i = 0;

    for (b = 0; b < 256; ++b)
    {
        v = b;
        i = 8;
        for (; --i >= 0; )
            v = (v & 1) ? ((v >> 1) ^ POLYNOMIAL) : (v >> 1);
        table[b] = v;
    }
}
#endif

unsigned long UPDC32(unsigned char octet, unsigned long crc)
{
    // The original code had this as a #define
    return table[(crc ^ octet) & 0xFF] ^ (crc >> 8);
}

#endif

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned int get_systick(void)
{
	return (counter_systick);
}

void set_systick(unsigned int t)
{
	counter_systick = t;
}

void systick_counter(void)
{
	counter_systick++;
}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned int delay)
{  
    
    unsigned int tickstart = get_systick(); 
    unsigned int wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(unsigned int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

// void delay_ms(uint16_t ms)
// {
// 	TIMER_Delay(TIMER0, 1000*ms);
// }


//
// check_reset_source
//
uint8_t check_reset_source(void)
{
    uint32_t src = SYS_GetResetSrc();

    SYS->RSTSTS |= 0x1FF;
    LDROM_DEBUG("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        LDROM_DEBUG("0)POR Reset Flag\r\n");       
    }
    if (src & BIT1)
    {
        LDROM_DEBUG("1)NRESET Pin Reset Flag\r\n");       
    }
    if (src & BIT2)
    {
        LDROM_DEBUG("2)WDT Reset Flag\r\n");       
    }
    if (src & BIT3)
    {
        LDROM_DEBUG("3)LVR Reset Flag\r\n");       
    }
    if (src & BIT4)
    {
        LDROM_DEBUG("4)BOD Reset Flag\r\n");       
    }
    if (src & BIT5)
    {
        LDROM_DEBUG("5)System Reset Flag\r\n");       
    }
    if (src & BIT6)
    {
        LDROM_DEBUG("6)HRESET Reset Flag \r\n");       
    }
    if (src & BIT7)
    {
        LDROM_DEBUG("7)CPU Reset Flag\r\n");       
    }
    if (src & BIT8)
    {
        LDROM_DEBUG("8)CPU Lockup Reset Flag\r\n");       
    }
    #endif
    
    if (src & SYS_RSTSTS_PORF_Msk) {
        SYS_ClearResetSrc(SYS_RSTSTS_PORF_Msk);
        
        LDROM_DEBUG("power on from POR\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_PINRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_PINRF_Msk);
        
        LDROM_DEBUG("power on from nRESET pin\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_WDTRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_WDTRF_Msk);
        
        LDROM_DEBUG("power on from WDT Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_LVRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_LVRF_Msk);
        
        LDROM_DEBUG("power on from LVR Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_BODRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_BODRF_Msk);
        
        LDROM_DEBUG("power on from BOD Reset\r\n");
        return FALSE;
    }    
    else if (src & SYS_RSTSTS_SYSRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_SYSRF_Msk);
        
        LDROM_DEBUG("power on from System Reset\r\n");
        return FALSE;
    } 
    else if (src & SYS_RSTSTS_CPURF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPURF_Msk);

        LDROM_DEBUG("power on from CPU reset\r\n");
        return FALSE;         
    }    
    else if (src & SYS_RSTSTS_CPULKRF_Msk)
    {
        SYS_ClearResetSrc(SYS_RSTSTS_CPULKRF_Msk);
        
        LDROM_DEBUG("power on from CPU Lockup Reset\r\n");
        return FALSE;
    }   
    
    LDROM_DEBUG("power on from unhandle reset source\r\n");
    return FALSE;
}

uint32_t caculate_crc32_checksum(uint32_t start, uint32_t size)
{
    #if defined (ENABLE_SW_CRC32)
    volatile uint32_t addr, data;    

    #if defined (USE_FLASH_TABLE)
    LDROM_DEBUG("ENABLE_SW_CRC32:[FLASH_TABLE]\r\n");    
    #elif defined (USE_SRAM_TABLE)
    LDROM_DEBUG("ENABLE_SW_CRC32:[SRAM_TABLE]\r\n");       
    calculate_table();
    #endif

    state = 0xFFFFFFFF;    
    addr = start;

    for(addr = start; addr < (start+size) ; addr += 4){
        data = FMC_Read(addr);
        state = UPDC32(GET_BYTE0(data), state);
        state = UPDC32(GET_BYTE1(data), state);
        state = UPDC32(GET_BYTE2(data), state);
        state = UPDC32(GET_BYTE3(data), state); 
    }

    return ~state;   

    #else
    volatile uint32_t addr, data;

    LDROM_DEBUG("HW CRC32\r\n");
    CRC_Open(CRC_32, (CRC_WDATA_RVS | CRC_CHECKSUM_RVS | CRC_CHECKSUM_COM), 0xFFFFFFFF, CRC_CPU_WDATA_32);
    
    for(addr = start; addr < (start+size) ; addr += 4){
        data = FMC_Read(addr);
        CRC_WRITE_DATA(data);
    }
    return CRC_GetChecksum();
    #endif
}

uint8_t verify_application_chksum(void)
{
    uint32_t chksum_cal, chksum_app;
    
    LDROM_DEBUG("Verify Checksum\r\n");
    
    chksum_cal = caculate_crc32_checksum(APROM_APPLICATION_START, (g_apromSize - 4));//(g_apromSize - FMC_FLASH_PAGE_SIZE)
    LDROM_DEBUG("Caculated .....<0x%08X>\r\n", chksum_cal);
    
    chksum_app = FMC_Read( (APROM_APPLICATION_START + APROM_APPLICATION_SIZE ) - 4);    
    LDROM_DEBUG("In APROM ......<0x%08X>\r\n", chksum_app);
    
    if (chksum_cal == chksum_app) {
        LDROM_DEBUG("Verify ........<PASS>\r\n");
        return TRUE;
    } else {
        LDROM_DEBUG("Verify ........<FAIL>\r\n");
        return FALSE;
    }
}

void set_TimeoutFlag(uint8_t flag)
{
    timeout_flag = flag;
}

uint8_t get_TimeoutFlag(void)
{
    return timeout_flag;
}

void check_Timeout(void)
{
    if (timeout_cnt > TIMEOUT_INTERVAL) {
        LDROM_DEBUG("Time-out, perform CHIP_RST\r\n");
        while(!UART_IS_TX_EMPTY(DEBUG_UART_PORT));
        SystemReboot_RST(RST_ADDR_APROM,RST_SEL_CHIP);
    }
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
        if (get_TimeoutFlag())
        {
            LDROM_DEBUG("entry counting :%2d\r\n" , timeout_cnt);
        }
        timeout_cnt++;

		// tick_counter();

		// if ((get_tick() % 1000) == 0)
		// {
        //     FLAG_PROJ_TIMER_PERIOD_1000MS = 1;//set_flag(flag_timer_period_1000ms ,ENABLE);
		// }

		// if ((get_tick() % 50) == 0)
		// {

		// }	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
    set_TimeoutFlag(1);
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    // if (FLAG_PROJ_TIMER_PERIOD_1000MS)//(is_flag_set(flag_timer_period_1000ms))
    // {
    //     FLAG_PROJ_TIMER_PERIOD_1000MS = 0;//set_flag(flag_timer_period_1000ms ,DISABLE);

    //     printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
    //     PB0 ^= 1;        
    // }
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
		LDROM_DEBUG("press : %c\r\n" , res);
		switch(res)
		{
			case '1':
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
                SYS_UnlockReg();
				// NVIC_SystemReset();	// Reset I/O and peripherals , only check BS(FMC_ISPCTL[1])
                // SYS_ResetCPU();     // Not reset I/O and peripherals
                SYS_ResetChip();    // Reset I/O and peripherals ,  BS(FMC_ISPCTL[1]) reload from CONFIG setting (CBS)	
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
	LDROM_DEBUG("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	LDROM_DEBUG("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	LDROM_DEBUG("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	LDROM_DEBUG("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	LDROM_DEBUG("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());
	LDROM_DEBUG("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());    	

//    LDROM_DEBUG("Product ID 0x%8X\n", SYS->PDID);
	
    #if 0
    LDROM_DEBUG("FLAG_PROJ_TIMER_PERIOD_1000MS : 0x%2X\r\n",FLAG_PROJ_TIMER_PERIOD_1000MS);
    LDROM_DEBUG("FLAG_PROJ_REVERSE1 : 0x%2X\r\n",FLAG_PROJ_REVERSE1);
    LDROM_DEBUG("FLAG_PROJ_REVERSE2 : 0x%2X\r\n",FLAG_PROJ_REVERSE2);
    LDROM_DEBUG("FLAG_PROJ_REVERSE3 : 0x%2X\r\n",FLAG_PROJ_REVERSE3);
    LDROM_DEBUG("FLAG_PROJ_REVERSE4 : 0x%2X\r\n",FLAG_PROJ_REVERSE4);
    LDROM_DEBUG("FLAG_PROJ_REVERSE5 : 0x%2X\r\n",FLAG_PROJ_REVERSE5);
    LDROM_DEBUG("FLAG_PROJ_REVERSE6 : 0x%2X\r\n",FLAG_PROJ_REVERSE6);
    LDROM_DEBUG("FLAG_PROJ_REVERSE7 : 0x%2X\r\n",FLAG_PROJ_REVERSE7);
    #endif

}

void GPIO_Init (void)
{
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH0MFP_Msk)) | (SYS_GPH_MFPL_PH0MFP_GPIO);
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH1MFP_Msk)) | (SYS_GPH_MFPL_PH1MFP_GPIO);
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH2MFP_Msk)) | (SYS_GPH_MFPL_PH2MFP_GPIO);

	//EVM LED
	GPIO_SetMode(PH,BIT0,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT1,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT2,GPIO_MODE_OUTPUT);
	
}

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
    CLK_EnableModuleClock(HSUSBD_MODULE);
    CLK_EnableModuleClock(ISP_MODULE);

    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    // CLK_EnableModuleClock(ISP_MODULE);	    
	CLK_EnableModuleClock(CRC_MODULE);

    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);
	

    CLK_EnableModuleClock(CRC_MODULE);
	
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

//    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
//    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB2MFP_UART1_RXD | SYS_GPB_MFPL_PB3MFP_UART1_TXD);
	
	/* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

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

	GPIO_Init();
	DEBUG_UART_Init();

	TIMER1_Init();

    SysTick_enable(1000);
    #if defined (ENABLE_TICK_EVENT)
    TickSetTickEvent(1000, TickCallback_processA);  // 1000 ms
    TickSetTickEvent(5000, TickCallback_processB);  // 5000 ms
    #endif

	CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
	SYS_UnlockReg();
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
    if (!check_reset_source()) //   always true
	{
        if (verify_application_chksum()) 
		{
            goto _APROM;
        } 
		else 
		{
            LDROM_DEBUG("Stay in BOOTLOADER (checksum invaild)\r\n");
        }
    } 
	// else 
    // {
    //     LDROM_DEBUG("Stay in BOOTLOADER (from APPLICATION)\r\n");
        
    //     //
    //     // start timer
    //     //
    //     LDROM_DEBUG("Time-out counter start....\r\n");
    //     TIMER_Start(TIMER1);
    //     set_TimeoutFlag(1);
    // }

    SYS_UnlockReg();
    FMC_Open();


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
    
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();
    /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */        
    __set_PRIMASK(1);    
    FMC_SetVectorPageAddr(APROM_APPLICATION_START);                 /* Set vector remap to APROM address 0x0      */

    // Not reset I/O and peripherals
    SYS_ResetCPU(); //SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;          /* Let CPU reset. Will boot from APROM.       */
  
    /* Trap the CPU */
    while (1);   

	
}

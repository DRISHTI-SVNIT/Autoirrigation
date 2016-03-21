#include "extUSART.h"
#include <asf.h>
#define USART_BAUD_RATE 9600
#define USART_SAMPLE_NUM 16
#define SHIFT 32

uint16_t calculate_baud_value(const uint32_t baudrate,const uint32_t peripheral_clock,uint8_t sample_num);

/*Assigning pin to the alternate peripheral function*/
static inline void pin_set_peripheral_function(uint32_t pinmux)
{
	uint8_t port = (uint8_t)((pinmux >> 16)/32);
	PORT->Group[port].PINCFG[((pinmux >> 16) - (port*32))].bit.PMUXEN = 1;
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg &= ~(0xF << (4 * ((pinmux >> 16) & 0x01u)));
	PORT->Group[port].PMUX[((pinmux >> 16) - (port*32))/2].reg |= (uint8_t)((pinmux & 0x0000FFFF) << (4 * ((pinmux >> 16) & 0x01u)));
}
/*
* internal Calculate 64 bit division, ref can be found in
*/
static uint64_t long_division(uint64_t n, uint64_t d)
{
	int32_t i;
	uint64_t q = 0, r = 0, bit_shift;
	for (i = 63; i >= 0; i--) {
		bit_shift = (uint64_t)1 << i;
		r = r << 1;
		if (n & bit_shift) {
			r |= 0x01;
		}
		if (r >= d) {
			r = r - d;
			q |= bit_shift;
		}
	}
	return q;
}
/*
* \internal Calculate asynchronous baudrate value (UART)
*/
uint16_t calculate_baud_value(
const uint32_t baudrate,
const uint32_t peripheral_clock,
uint8_t sample_num)
{
	/* Temporary variables */
	uint64_t ratio = 0;
	uint64_t scale = 0;
	uint64_t baud_calculated = 0;
	uint64_t temp1;
	/* Calculate the BAUD value */
	temp1 = ((sample_num * (uint64_t)baudrate) << SHIFT);
	ratio = long_division(temp1, peripheral_clock);
	scale = ((uint64_t)1 << SHIFT) - ratio;
	baud_calculated = (65536 * scale) >> SHIFT;
	return baud_calculated;
}

/* External connector(SERCOM2) UART bus and generic clock initialization */
void ext_usart_clock_init(void)
{
	struct system_gclk_chan_config gclk_chan_conf;
	uint32_t gclk_index = SERCOM2_GCLK_ID_CORE;
	/* Turn on module in PM */
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_SERCOM2);
	/* Turn on Generic clock for USART */
	system_gclk_chan_get_config_defaults(&gclk_chan_conf);
	//Default is generator 0. Other wise need to configure like below
	gclk_chan_conf.source_generator = GCLK_GENERATOR_1;
	system_gclk_chan_set_config(gclk_index, &gclk_chan_conf);
	system_gclk_chan_enable(gclk_index);
}
/* External connector(SERCOM2) pin initialization */
void ext_usart_pin_init(void)
{
	/* PA08 and PA09 set into peripheral function*/
	pin_set_peripheral_function(PINMUX_PA08D_SERCOM2_PAD0); //tx	
	pin_set_peripheral_function(PINMUX_PA09D_SERCOM2_PAD1);	//rx
}
/* External connector(SERCOM2) UART initialization */
void ext_usart_init(void)
{
	uint16_t baud_value;
	baud_value = calculate_baud_value(USART_BAUD_RATE,system_gclk_chan_get_hz(SERCOM2_GCLK_ID_CORE),USART_SAMPLE_NUM);
	/* By setting the DORD bit LSB is transmitted first and setting the RXPO bit as
	1 corresponding SERCOM PAD[1] will be used for data reception RXD, PAD[0] will be used as TxD
	pin by setting TXPO bit as 0, 16x over-sampling is selected by setting the SAMPR bit as 0,
	Generic clock is enabled in all sleep modes by setting RUNSTDBY bit as 1,
	USART clock mode is selected as USART with internal clock by setting MODE bit into 1.
	*/
	SERCOM2->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD |
	SERCOM_USART_CTRLA_RXPO(0x1) |
	SERCOM_USART_CTRLA_TXPO(0x0) |
	SERCOM_USART_CTRLA_SAMPR(0x0)|
	SERCOM_USART_CTRLA_RUNSTDBY |
	SERCOM_USART_CTRLA_MODE_USART_INT_CLK ;
	/* baud register value corresponds to the device communication baud rate */
	SERCOM2->USART.BAUD.reg = baud_value;
	/* 8-bits size is selected as character size by setting the bit CHSIZE as 0,
	TXEN bit and RXEN bits are set to enable the Transmitter and receiver*/
	SERCOM2->USART.CTRLB.reg = SERCOM_USART_CTRLB_CHSIZE(0x0) |
	SERCOM_USART_CTRLB_TXEN |
	SERCOM_USART_CTRLB_RXEN ;
	/* synchronization busy */
	while(SERCOM2->USART.SYNCBUSY.bit.CTRLB);
	/* SERCOM2 peripheral enabled */
	SERCOM2->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	/* synchronization busy */
	while(SERCOM2->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE);
}


/*send data from USART*/
void USART_Transmitchar(unsigned char data) {
	while(!(SERCOM2->USART.INTFLAG.bit.DRE));
	SERCOM2->USART.DATA.reg = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while(!(SERCOM2->USART.INTFLAG.bit.RXC));
	/* Get and return received data from buffer */
	return 	SERCOM2->USART.DATA.reg ;
}

void USART_TransmitString(char *str)
{
	while(*str>0)
	{
		USART_Transmitchar(*str);
		str++;
	}
}

void USART_TransmitNumber(unsigned long n){
	if(n >= 10){
		USART_TransmitNumber(n/10);
		n = n%10;
	}
	USART_Transmitchar(n+'0'); /* n is between 0 and 9 */
}

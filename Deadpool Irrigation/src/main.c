/**
 * \file
 *
 * \brief SAMD21 ADC Configuration example
 *
 * Copyright (C) 2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/*
* \mainpage
 * \section intro Introduction
 * This example demonstrates how to use enable and configure different features of ADC module.
 *
 * \section files Main Files
 * - conf_board.h: Board Configuration
 * - conf_example.h: ADC Features Configuration
 * - conf_clocks.h: SAM D21 Clock configuration
 * - adc_configure.h: ADC configuration prototype declarations
 * - adc_configure.c: ADC configuration definitions
 * - adc_temp.h: ADC temperature sensor prototype declarations
 * - adc_temp.c: ADC temperature calculation and definitions
 * 

 
 * This example has been tested with the following setup:
 *   - SAMD21 Xplained Pro 
 *   - UART configuration is 115200 baudrate, no parity, data 8 bit.
 *
 * \section example description Description of the example
 * The example helps to configure the ADC module 
 * based on macro definition available in conf_example.h.
 * The Serial Console used to get input from user where necessary
 * display the ADC input voltage after the ADC configuration done.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/avr">Atmel AVR</A>.\n
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
 
#include <asf.h>
#include "adc_configure.h"
#include "hitecServo.h"


/* Structure for USART module instance */
struct usart_module console_instance;
struct adc_module adc_instance;
struct tcc_module tcc_instance;

/**
* \brief Configure serial Console.
*
* This function configures and enable the SERCOM
* module with below Settings.

* GLCK for SERCOM	-> GCLK_GENERATOR_0 (8MHz)
* SERCOM instance	-> 3
* TXD				-> PA22
* RXD				-> PA23
* BAUDRATE			-> 115200
* STOP BIT			-> 1
* CHARACTER			-> 8
* PARITY			-> NONE
*
*/

void configure_console(void)
{
	
	struct usart_config conf_usart;
	
	usart_get_config_defaults(&conf_usart);
	
	conf_usart.baudrate = 115200;
	conf_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	conf_usart.generator_source = GCLK_GENERATOR_1;
	conf_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	conf_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	conf_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	conf_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	stdio_serial_init(&console_instance, EDBG_CDC_MODULE, &conf_usart);
	
	usart_enable(&console_instance);
}

uint16_t adc_ka_result(void)
{
	uint16_t adc_result = 0;
	
	adc_start_conversion(&adc_instance);
	while((adc_get_status(&adc_instance) & ADC_STATUS_RESULT_READY) != 1);
	
	adc_read(&adc_instance, &adc_result);
	
	return adc_result;
}

static void configure_tcc(void)
{
	//! [setup_config]
	struct tcc_config config_tcc;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc, TCC0);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1;
	config_tcc.counter.period =   20000;
	
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.compare.match[2] = 10000;
	
	config_tcc.pins.enable_wave_out_pin[6] = true;
	config_tcc.pins.wave_out_pin[6] = PIN_PB12F_TCC0_WO6;
	config_tcc.pins.wave_out_pin_mux[6] = MUX_PB12F_TCC0_WO6;
	// 	config_tcc.pins.wave_out_pin[6] = PIN_PA10F_TCC0_WO2 ;
	// 	config_tcc.pins.wave_out_pin_mux[6] = MUX_PA10F_TCC0_WO2;

	
	// 	config_tcc.compare.match[0] =  0;
	// 	config_tcc.compare.match[1] =  930;
	// 	config_tcc.compare.match[2] = 1100;
	// 	//! [setup_change_config]

	//! [setup_set_config]
	tcc_init(&tcc_instance, TCC0, &config_tcc);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc_instance);
	//! [setup_enable]
}

int main(void)
{
	/* Configuration of clock and board */
	system_init();
	delay_init();
	hitecServoInit();
	/* Serial Console configuration */
	configure_console();
	configure_adc();
	configure_tcc();
	printf("Up and running!");
	while (1){
		delay_ms(500);
		printf("adc value- ");
		uint16_t aval = adc_ka_result();  
		printf("%d \r",aval);
		setHitecServoAngle(aval*180/4096);
	}
}

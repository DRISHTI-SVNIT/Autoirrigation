/**
 * \file
 *
 * \brief ADC Features Configuration file
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
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

/* Structure for ADC module instance */
extern struct adc_module adc_instance;

/**
* \brief ADC Configuration normal mode.
* This function configures ADC as normal mode with below Settings
*
* GLCK for ADC		-> GCLK_GENERATOR_1 (8MHz)
* CLK_ADC				-> 512 KHz
* REFERENCE			-> 1/2 VDDANA (1.65V)
* POSITIVE INPUT		-> PA02
* NEGATIVE INPUT		-> GND
*/

void configure_adc(void)
{
	struct adc_config conf_adc;
	
	adc_get_config_defaults(&conf_adc);
	
	conf_adc.clock_source = GCLK_GENERATOR_1;
	conf_adc.reference = ADC_REFERENCE_INTVCC1;
	conf_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	conf_adc.positive_input = ADC_POSITIVE_INPUT_PIN0;
	conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
	
	adc_init(&adc_instance, ADC, &conf_adc);
	
	adc_enable(&adc_instance);
}


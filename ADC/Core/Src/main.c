/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <stdio.h>
#include "defines.h"
#include "delay.h"
/*Defines -------------------------------------------------------------------*/

#define LED				D, 12
#define SW				A, 0
#define LED2			D, 13

#define ADC_IN1			1  	//PA1
#define ADC_IN2			2	//PA2
#define ADC_IN3			3	//PA3
#define IN1				A, 1
#define IN2				A, 2
#define IN3				A, 3
/*Typedef ------------------------------------------------------------------*/

/*Global variables ---------------------------------------------------------*/
float volt;
uint16_t adcval;

/****************************************************************************/
/**
 * @brief configura el PLL a 168MHz
 */
void PLL_Config(void);

/**
 * @brief funcion que configura pines como entrada y salida
 */
void GPIO_Config(void);

/**
 * @brief configuracion del ADC para conversion SINGLE
 */
void ADC1_Config(void);

/**
 * @brief configura los canales ADC utilizados
 */
void ADC1_CH_Config(void);
/**
 * @brief realiza la conversion de un canal con el ADC1
 * @param chx: numero del canal a realizar la conversion
 * return uint16_t
 */
uint16_t ADC1_CH_Read(uint8_t chx);
int main(void)
{
	PLL_Config();
	printf("Ejemplo de manejo de conversion ADC\r\n");
	printf("%s %s\r\n", __DATE__, __TIME__);
	GPIO_Config();
	//manejo se secciones criticas de codigo
	__disable_irq();
	SysTick_Init();
	__enable_irq();
	ADC1_Config();
	ADC1_CH_Config();
    /* Loop forever */
	for(;;){
		adcval = ADC1_CH_Read(ADC_IN1);
		volt = adcval * 3.3/(4095);
		printf("ADCVAL: %u  VOLT: %.2f\r\n", adcval, volt);
		delay_ms(1000);
	}
}

void PLL_Config(void){
	//1. apagar el pll y esperar que la bandera se limpie
	RCC->CR &=~ (RCC_CR_PLLON);  //RCC->CR &=~ (1<<24);
	while(RCC->CR & RCC_CR_PLLRDY);
	//2. configurar los parametros del pll
	//->Encender el HSE
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	//->HSE como fuente para el PLL
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;			//->HSE -> PLL
	/**
	 * 8MHZ -> / M <1-2MH> -> * N <50-432MH>
	 * Sysclk = ((8MHz/8) * 336) / 2 = 168MHz
	 */
	//->M
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= 8U<<RCC_PLLCFGR_PLLM_Pos;
	//->N
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= 336<<RCC_PLLCFGR_PLLN_Pos;
	//->P
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLP;
	//3. configurar la latencia de la flash
	FLASH->ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
	FLASH->ACR |= FLASH_ACR_LATENCY_5WS;
	//->encender el PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//4. seleccionar el PLL como fuente de reloj
	//->Configurar los prescalers de los buses APB1 y ABP2
	//->APB1
	RCC->CFGR &=~ RCC_CFGR_PPRE1;
	RCC->CFGR |= 0x5u<<RCC_CFGR_PPRE1_Pos;
	//->APB2
	RCC->CFGR &=~ RCC_CFGR_PPRE2;
	RCC->CFGR |= RCC_CFGR_PPRE2_2;
	//->Seleccionar el PLL como fuente de reloj del sistema
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	SystemCoreClockUpdate();
	return;
}
/***********************************************************************/
void GPIO_Config(void){
	//HABILITAR EL RELOJ
	RCC->AHB1ENR |= GPIOX_CLOCK(LED) | GPIOX_CLOCK(SW);
	GPIOX_MODER(MODE_OUT, LED);
	GPIOX_MODER(MODE_DIGITAL_INPUT, SW);
	GPIOX_PUPDR(MODE_PU_NONE, SW);
	GPIOX_MODER(MODE_OUT, LED2);
}


/**
 * @brief configuracion del ADC para conversion SINGLE
 */
void ADC1_Config(void){
	//1. habilitar el reloj
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN_Msk;
	//2. Configurar el prescaler del reloj del ADC
	ADC123_COMMON->CCR &=~ ADC_CCR_ADCPRE;
	ADC123_COMMON->CCR |= ADC_CCR_ADCPRE_0;		//PCLK2 / 4
	//3. Configurar la resolucion
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	//ADC1->CR1 |= ADC_CR1_RES_1;					//RES 8 BIT (0 - 255)

	//4. encender el ADC
	ADC1->CR2 |= ADC_CR2_ADON;					//Enciende el ADC1 (Listo para conversiones)

	return;
}

void ADC1_CH_Config(void){
	//configurar los pines
	RCC->AHB1ENR |= GPIOX_CLOCK(IN1);
	GPIOX_MODER(MODE_ANALOG_INPUT, IN1);
	GPIOX_MODER(MODE_ANALOG_INPUT, IN2);
	GPIOX_MODER(MODE_ANALOG_INPUT, IN3);
	GPIOX_PUPDR(MODE_PU_NONE, IN1);
	GPIOX_PUPDR(MODE_PU_NONE, IN2);
	GPIOX_PUPDR(MODE_PU_NONE, IN3);
	//Configurar el tiempo de muestreo para cada canal
	ADC1->SMPR2 = 0;
	ADC1->SMPR2 &=~ ADC_SMPR2_SMP1;			//3 ciclos de muestreo
	ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;		//15 ciclos de muestreo
	ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;		//28 ciclos de muestreo
	return;
}

uint16_t ADC1_CH_Read(uint8_t chx){
	/*configurar es la secuencia de conversion*/
	ADC1->SQR1 = 0;							//L = 0 -> 1 conversion
	ADC1->SQR3 = chx;
	//iniciar la conversion (single)
	ADC1->CR2 |= ADC_CR2_SWSTART;			//Inicia la conversion
	//espeerar que finalice la conversion
	while(!(ADC1->SR & ADC_SR_EOC));
	//retornar  conversion
	return (ADC1->DR);
}
/**************************************************/

int __io_putchar(int ch){
	ITM_SendChar(ch);
	return ch;
}
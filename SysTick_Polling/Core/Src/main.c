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
/*Defines -------------------------------------------------------------------*/

#define LED				D, 12
#define SW				A, 0
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
 * @brief
 */
void SysTick_Init(void);
/**
 * @brief funcion para generar retardos en milisegundos
 */
void delay_ms(uint32_t delay);

int main(void)
{
	PLL_Config();
	printf("Ejemplo de manejo de pines\r\n");
	printf("%s %s\r\n", __DATE__, __TIME__);
	GPIO_Config();
	SysTick_Init();
    /* Loop forever */
	for(;;){
		GPIOX_ODR(LED) ^= 1;
		delay_ms(100);
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
}


/**
 * @brief
 */
void SysTick_Init(void){
	uint32_t temp;
	//1. deshabilitar
	SysTick->CTRL = 0;
	//2. calcular el valor del registro LOAD
	temp = SystemCoreClock / 1000;
	SysTick->LOAD = temp - 1;
	SysTick->VAL = 0;
	//3. configurar el registro ctrl
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 	//clock sourse AHB
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;		//habilita el conteo
	return;
}
/**
 * @brief funcion para generar retardos en milisegundos
 */
void delay_ms(uint32_t delay){
	uint32_t i;
	SysTick->VAL = 0;
	for(i = 0; i<delay; i++){
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); //se bloquea durante 1ms
	}
	return;
}
/**************************************************/

int __io_putchar(int ch){
	ITM_SendChar(ch);
	return ch;
}
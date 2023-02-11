/*
 * stm32f4xx_it.c
 *
 *  Created on: 30 ene. 2023
 *      Author: jeffr
 */


#include "stm32f4xx.h"
#include "lwrb.h"
#include <stdbool.h>

extern volatile uint32_t uwTick;
extern lwrb_t rbbuff;
/**
 * @brief rutina de servicio de IT de la Systick
 */

void SysTick_Handler(void){

	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		//todo
		uwTick++;
	}
}


void USART2_IRQHandler(void){
	uint8_t ch;
	if(USART2->SR & USART_SR_RXNE){
		//Todo
		ch = USART2->DR;
		if(lwrb_get_free(&rbbuff)>=1){
			lwrb_write(&rbbuff, &ch, 1);
		}
	}
	return;
}
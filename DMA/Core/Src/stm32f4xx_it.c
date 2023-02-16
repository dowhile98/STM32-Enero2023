/*
 * stm32f4xx_it.c
 *
 *  Created on: 30 ene. 2023
 *      Author: jeffr
 */


#include "stm32f4xx.h"
#include <stdio.h>
extern volatile uint32_t uwTick;
extern uint16_t adcval[3];

/**
 * @brief rutina de servicio de IT de la Systick
 */

void SysTick_Handler(void){

	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		//todo
		uwTick++;
	}
}
void DMA2_Stream4_IRQHandler(void){
	if(DMA2->HISR & DMA_HISR_TCIF4){
		DMA2->HIFCR |= DMA_HIFCR_CTCIF4;	//Clear
		//todo
		printf("IN1: %u IN2:%u IN3:%u\r\n", adcval[0], adcval[1], adcval[2]);
	}
	return;
}
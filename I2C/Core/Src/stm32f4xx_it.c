/*
 * stm32f4xx_it.c
 *
 *  Created on: 30 ene. 2023
 *      Author: jeffr
 */


#include "stm32f4xx.h"

extern volatile uint32_t uwTick;

/**
 * @brief rutina de servicio de IT de la Systick
 */

void SysTick_Handler(void){

	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		//todo
		uwTick++;
	}
}
/*
 * stm32f4xx_it.c
 *
 *  Created on: 10 feb. 2023
 *      Author: jeffr
 */


#include "stm32f4xx.h"


extern uint8_t state;
void TIM2_IRQHandler(void){
	if(TIM2->SR & TIM_SR_CC2IF){
		TIM2->SR = 0;
		//todo
		switch(state){
		case 0:
			TIM2->CCR2 += 1000;			//0.5seg
			state = 1;
			break;
		case 1:
			TIM2->CCR2 += 2000;			//1SEG
			state = 2;
			break;
		case 2:
			TIM2->CCR2 += 4000;			//2seg
			state = 3;
			break;
		case 3:
			TIM2->CCR2 += 10000;
			state = 0;
			break;
		}
	}
}

/*
 * AT_Parser.c
 *
 *  Created on: 6 feb. 2023
 *      Author: jeffr
 */

#include "lwrb.h"
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include <stdbool.h>
#include "delay.h"
#include <stdlib.h>

extern lwrb_t rbbuff;
extern uint8_t data[100];
bool response_redy = false;


void rb_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len){
	uint8_t ch;
	switch (type) {
		case LWRB_EVT_RESET:
			//printf("[EVT] Buffer reset event!\r\n");
			break;
		case LWRB_EVT_READ:
			//printf("[EVT] Buffer read event: %d byte(s)!\r\n", (int)len);
			break;
		case LWRB_EVT_WRITE:
			//printf("[EVT] Buffer write event: %d byte(s)!\r\n", (int)len);
			lwrb_peek(&rbbuff, lwrb_get_full(&rbbuff) - 1 , &ch, 1);
			if(ch == '\n'){
				response_redy = true;
			}
			break;
	        default: break;
	    }

}

void at_send_cmd(uint8_t *cmd, uint32_t timout){
	uint32_t tickstart = GetTick();
	uint32_t len = strlen((char*)cmd);
	while(len>0){
		if(USART2->SR & USART_SR_TXE){
			USART2->DR = *cmd;
			cmd++;
			len--;
		}
		if((GetTick() - tickstart)>timout){
			break;
		}
	}

	return;
}

bool at_verify_response(uint8_t *response, uint32_t timout){
	uint32_t tickstart = GetTick();
	uint32_t size;
	bool state = false;
	uint8_t result[50] = {0};
	while(1){
		if(response_redy == true){
			break;
		}
		//timout
		if((GetTick() - tickstart)>timout){
			return false;
		}
	}
	response_redy = false;
	memset((void*)result,0, 50);
	//leer la respuesta recibida
	size = lwrb_get_full(&rbbuff);

	 lwrb_read(&rbbuff, (void*)result, size);
	//procesado
	if(strstr((char*)result, (char*)response) != NULL){
		//CONTIENE LA RESPUESTA
		state = true;
	}else{
		//TODO
		state = false;
	}
	return state;
}
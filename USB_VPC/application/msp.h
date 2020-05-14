#ifndef __MSP_H_
#define __MSP_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "main.h" 

#define DEBUG 1

#ifdef __cplusplus
extern "C" {
#endif

#define TRACE(fmt, ...)  \
				do { \
						printf("%ld,%s,%d::"fmt,HAL_GetTick(), __func__,__LINE__,##__VA_ARGS__);\
				} while(0)

#ifdef HAL_USART_MODULE_ENABLED
	extern UART_HandleTypeDef *uartout;
#endif

typedef struct debug_byte_t
{
	uint8_t	bit0:1;
	uint8_t	bit1:1;
	uint8_t	bit2:1;
	uint8_t	bit3:1;
	uint8_t	bit4:1;
	uint8_t	bit5:1;
	uint8_t	bit6:1;
	uint8_t	bit7:1;
}debug_byte_t;

uint32_t millis(void);

void Hardware_init(void);

void print_array(uint8_t* pbuf, uint16_t length);
#ifdef __cplusplus
}
#endif

#endif

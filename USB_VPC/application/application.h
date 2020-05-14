#ifndef __APPLICATION_H_
#define __APPLICATION_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"
#include "fifo.h"

void Init(void);
void setup(void);
void loop(void);

void USER_USART_UART_Init(  UART_HandleTypeDef *uart,
                            uint32_t baudrate,uint16_t WordLength,
                            uint16_t StopBits,uint16_t Parity);


#ifdef __cplusplus
}
#endif

#endif

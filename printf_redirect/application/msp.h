#ifndef MSP_H_
#define MSP_H_

#include "main.h"
#include <stdint.h>
#include <stdio.h>

void setup(void);
void loop(void);

static inline void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

static inline uint32_t millis(void)
{
    return HAL_GetTick();
}

#endif

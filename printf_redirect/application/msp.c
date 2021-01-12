#include "msp.h"

extern UART_HandleTypeDef huart1;
UART_HandleTypeDef *trace = &huart1;

#ifdef __MICROLIB //MDK use MICROLIB
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(trace, (uint8_t *)&ch, 1, 0xff);

    return ch;
}

#elif __GNUC__ //GCC
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(trace, (uint8_t *)ptr, len, 0xff * len);
    return len;
}
#else
#error "Please select the correct compiler"
#endif

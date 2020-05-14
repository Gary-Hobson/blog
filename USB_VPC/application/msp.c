#include "msp.h"
#if (defined __GNUC__) && ((defined HAL_USART_MODULE_ENABLED) || ( defined HAL_UART_MODULE_ENABLED))
  extern UART_HandleTypeDef huart1;
  UART_HandleTypeDef *debugout=&huart1;
  int _write(int file, char *ptr, int len)
  {
      //CDC_Transmit_FS(ptr,len);
      HAL_UART_Transmit((UART_HandleTypeDef *)debugout, (uint8_t *)ptr, len, 1000*len);
      return len;
  }
#elif (defined HAL_USART_MODULE_ENABLED)|| (defined HAL_UART_MODULE_ENABLED)
  extern UART_HandleTypeDef huart1;
  UART_HandleTypeDef *debugout=&huart1;
  int fputc(int ch, FILE *f)
  {
    HAL_UART_Transmit((UART_HandleTypeDef *)debugout, (uint8_t *)&ch, 1, 1000);
    return ch;
  }
#endif 

#if 1
void HAL_IncTick(void)
{
  extern __IO uint32_t uwTick;
  uwTick += uwTickFreq;
  if((uwTick&(uint32_t)0x1ff)==0){
      HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  }
}
#endif

void print_array(uint8_t* pbuf, uint16_t length)
{
  for(uint16_t i=0;i<length;i++)
  {
    printf("%02x ",(uint16_t )pbuf[i]);
  }
}

uint32_t millis(void)
{
    return HAL_GetTick();  
}


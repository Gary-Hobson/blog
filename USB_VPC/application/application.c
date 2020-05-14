#include "application.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "fifo.h"

#define TX_BUFF_SIZE 256
#define RX_BUFF_SIZE 128

char USB_TX_BUFF[TX_BUFF_SIZE];
char USB_RX_BUFF[RX_BUFF_SIZE];
FIFO * TX_FIFO;
char rx_ch;
FlagStatus TxCpl_Flag=SET;


void USB_Reset(void);
void Uart_IDLEInit(void);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);
void USER_USART1_UART_Init(uint16_t baudrate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity);


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

void Init(void)
{
	USB_Reset();
}

void setup(void)
{
	TX_FIFO=FIFOCreate(1024,1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)USB_RX_BUFF, RX_BUFF_SIZE);
	Uart_IDLEInit();
}
                                                                                                          
void loop(void)
{

}

/*定时器回调，100us发送缓存区数据到串口*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t Txlen;
	if(htim!=&htim2){
		return;
	}
	//读取缓冲区的数据长度，逐次发送
	Txlen=FIFOReadableAmount(TX_FIFO);

	/*等待串口发送完成*/
	if(Txlen!=0&&TxCpl_Flag==SET)
	{
		if(Txlen>=TX_BUFF_SIZE)
		{
			Txlen=TX_BUFF_SIZE;
		}
		#if DEBUG
		printf("TX data len:%d\n",Txlen);
		#endif
		FIFOBatchRead(TX_FIFO,USB_TX_BUFF,Txlen);
		HAL_UART_Transmit_DMA(&huart2,(uint8_t *)USB_TX_BUFF,Txlen);
		TxCpl_Flag=RESET;
	}
}

/*串口发送完成回调*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	TxCpl_Flag=SET;
}

/*串口DMA接收完成回调*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	#if DEBUG
	printf("RX data len:%d\n",RX_BUFF_SIZE);
	#endif
	CDC_Transmit_FS((uint8_t *)USB_RX_BUFF,RX_BUFF_SIZE);
}

/*串口空闲中断*/
void Uart_IDLEInit(void)
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);  					
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)USB_RX_BUFF, RX_BUFF_SIZE);	
}

/*接收到空闲中断后，发送数据到USB*/
void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	 uint8_t RXDataLength;
	if(RESET == __HAL_UART_GET_FLAG(& huart2, UART_FLAG_IDLE))   
	{	 // 判断是否是空闲中断
		return;			 
	} 

	/*清除空闲中断标志（否则会一直不断进入中断）*/
	__HAL_UART_CLEAR_IDLEFLAG(& huart2); 
	/*停止本次DMA传输*/
    HAL_UART_DMAStop(& huart2);                     
    /*计算接收到的数据长度*/
    RXDataLength  = RX_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);   
	USB_RX_BUFF[RXDataLength]=0;
	#if DEBUG
	printf("RX data len:%d\n",RXDataLength);
	#endif
	CDC_Transmit_FS((uint8_t *)USB_RX_BUFF,RXDataLength);
	// 重启开始DMA传输 
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)USB_RX_BUFF, RX_BUFF_SIZE);  

}

void USB_Reset(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
}

void USER_USART_UART_Init(UART_HandleTypeDef *uart,uint32_t baudrate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity)
{
  uart->Init.BaudRate = baudrate;
  uart->Init.WordLength = WordLength;
  uart->Init.StopBits = StopBits;
  uart->Init.Parity = Parity;
  if (HAL_UART_Init(uart) != HAL_OK)
  {
    Error_Handler();
  }
}

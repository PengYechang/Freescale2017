#include "usart.h"

void Usart_Init(void)
{
	UART_QuickInit( UART2_RX_PD02_TX_PD03,115200);  //UART0 PD02 PD03快速初始化
}


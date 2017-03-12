#include "startLine.h"

volatile bool leftST188 = false;
volatile bool rightST188 = false;

void startLineInit(void)
{
	GPIO_QuickInit(HW_GPIOC,ST188_PIN1,kGPIO_Mode_IPD);
	GPIO_QuickInit(HW_GPIOC,ST188_PIN2,kGPIO_Mode_IPD);	
	LeftST1188OFF;
	RightST188OFF;
}

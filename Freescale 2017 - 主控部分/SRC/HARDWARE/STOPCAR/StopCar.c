#include "StopCar.h"

void GPIO_StopCarInit(void)
{
	GPIO_QuickInit(STARTCAR_PORT,STARTCAR_PIN,STARTCAR_MODE);
	GPIO_QuickInit(STOPCAR_PORT,STOPCAR_PIN,STOPCAR_MODE);
}

void StopCarDetect(void)
{
	if(GPIO_ReadBit(STARTCAR_PORT,STARTCAR_PIN) == 1)
	{
		StopCar = 0;
	}
	if(GPIO_ReadBit(STOPCAR_PORT,STOPCAR_PIN) == 1)
	{
		StopCar = 1;
	}
}

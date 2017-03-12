#include "isr.h"

static void GPIOA_ISR(uint32_t array);			//摄像头场中断
static void DMA0_ISR(void);						//DMA传输中断

uint32_t avoidRamWayEffectTime = 0;

uint16_t row = 0;
uint8_t  ImgOK = false;
uint8_t  ImgDealOK = false;

void ISR_Config(void)
{
	
	//摄像头场中断 A25
	GPIO_CallbackInstall(HW_GPIOA, GPIOA_ISR);				  //配置GPIOA中断，注册回调函数
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, true);//允许GPIOA_29上升沿中断，用于场中断
	
	//摄像头行中断 A26
    GPIO_CallbackInstall(HW_GPIOA, GPIOA_ISR);				  //配置GPIOA中断，注册回调函数
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, true);//先关闭行中断
	
	
	//DMA0传输中断	
//	DMA_CallbackInstall(HW_DMA_CH0, DMA0_ISR);				  //配置DMA0中断，注册回调函数
	GPIO_ITDMAConfig(HW_GPIOA, 24, kGPIO_DMA_RisingEdge, true);//允许GPIOC_4上升沿触发DMA中断，用于DMA的传输
//	DMA_ITConfig(HW_DMA_CH0, kDMA_IT_Major, true);			  //开启DMA0的DMA主循环完成中断
//	

	NVIC_Init(PORTA_IRQn,NVIC_PriorityGroup_2,0,0);			  //场中断
	NVIC_Init(DMA0_IRQn,NVIC_PriorityGroup_2,0,1);			  //DMA0		摄像头的中断在同一个抢占优先级
	NVIC_Init(UART2_RX_TX_IRQn,NVIC_PriorityGroup_2,2,0);	  //上位机通信

}


//摄像头场中断行中断
static void GPIOA_ISR(uint32_t array)
{
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, false);   //先禁止中断
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, false);   //先禁止中断
    if(array & (1 << 25))										  //判断PTA29是否被置位
    {
		PORTA->ISFR |= (1<<25);   //清除标志位
		if(ImgDealOK == true)
		{
			row = 0;
			ImgOK = false;
		}
		
		
    }
	if(array & (1 << 26))
	{
		PORTA->ISFR |= (1<<26); 
		if(ImgOK == false)
		{
			if(row < Image_Height)
			{
				//////////////////////////////////////////////////////////////////////////////////////////////
				DMA_DisableRequest(HW_DMA_CH0);						//禁止通道传输，设置DMA	
				DMA0->TCD[HW_DMA_CH0].DADDR = (uint32_t)Image_Buff1[row];
				DMA0->TCD[HW_DMA_CH0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(Image_Width);
				DMA_EnableRequest(HW_DMA_CH0);						//设置好之后，再开启通道传输，等待传输完毕产生中断
				//////////////////////////////////////////////////////////////////////////////////////////////    
			    row ++;   //下一行  
			}
			else 
			{  
				ImgOK = true;
				ImgDealOK = false;
			}
		}
	 
	}
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, true);    //重新开启中断
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, true);    //重新开启中断
}

//DMA传输中断
//static void DMA0_ISR(void)
//{
//	DMA_ITConfig(HW_DMA_CH0,kDMA_IT_Major,false);				  //禁止DMA中断
//	if( DMA_IsMajorLoopComplete(HW_DMA_CH0) == 0 )                //判断主循环是否已经完成
//	{  
//		row ++;   //下一行   
//	}
//	DMA_ITConfig(HW_DMA_CH0,kDMA_IT_Major,true);				  //重新开启DMA中断
//}


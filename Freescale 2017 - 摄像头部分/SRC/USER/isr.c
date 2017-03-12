#include "isr.h"

static void GPIOA_ISR(uint32_t array);			//����ͷ���ж�
static void DMA0_ISR(void);						//DMA�����ж�

uint32_t avoidRamWayEffectTime = 0;

uint16_t row = 0;
uint8_t  ImgOK = false;
uint8_t  ImgDealOK = false;

void ISR_Config(void)
{
	
	//����ͷ���ж� A25
	GPIO_CallbackInstall(HW_GPIOA, GPIOA_ISR);				  //����GPIOA�жϣ�ע��ص�����
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, true);//����GPIOA_29�������жϣ����ڳ��ж�
	
	//����ͷ���ж� A26
    GPIO_CallbackInstall(HW_GPIOA, GPIOA_ISR);				  //����GPIOA�жϣ�ע��ص�����
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, true);//�ȹر����ж�
	
	
	//DMA0�����ж�	
//	DMA_CallbackInstall(HW_DMA_CH0, DMA0_ISR);				  //����DMA0�жϣ�ע��ص�����
	GPIO_ITDMAConfig(HW_GPIOA, 24, kGPIO_DMA_RisingEdge, true);//����GPIOC_4�����ش���DMA�жϣ�����DMA�Ĵ���
//	DMA_ITConfig(HW_DMA_CH0, kDMA_IT_Major, true);			  //����DMA0��DMA��ѭ������ж�
//	

	NVIC_Init(PORTA_IRQn,NVIC_PriorityGroup_2,0,0);			  //���ж�
	NVIC_Init(DMA0_IRQn,NVIC_PriorityGroup_2,0,1);			  //DMA0		����ͷ���ж���ͬһ����ռ���ȼ�
	NVIC_Init(UART2_RX_TX_IRQn,NVIC_PriorityGroup_2,2,0);	  //��λ��ͨ��

}


//����ͷ���ж����ж�
static void GPIOA_ISR(uint32_t array)
{
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, false);   //�Ƚ�ֹ�ж�
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, false);   //�Ƚ�ֹ�ж�
    if(array & (1 << 25))										  //�ж�PTA29�Ƿ���λ
    {
		PORTA->ISFR |= (1<<25);   //�����־λ
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
				DMA_DisableRequest(HW_DMA_CH0);						//��ֹͨ�����䣬����DMA	
				DMA0->TCD[HW_DMA_CH0].DADDR = (uint32_t)Image_Buff1[row];
				DMA0->TCD[HW_DMA_CH0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(Image_Width);
				DMA_EnableRequest(HW_DMA_CH0);						//���ú�֮���ٿ���ͨ�����䣬�ȴ�������ϲ����ж�
				//////////////////////////////////////////////////////////////////////////////////////////////    
			    row ++;   //��һ��  
			}
			else 
			{  
				ImgOK = true;
				ImgDealOK = false;
			}
		}
	 
	}
	GPIO_ITDMAConfig(HW_GPIOA, 25, kGPIO_IT_RisingEdge, true);    //���¿����ж�
	GPIO_ITDMAConfig(HW_GPIOA, 26, kGPIO_IT_RisingEdge, true);    //���¿����ж�
}

//DMA�����ж�
//static void DMA0_ISR(void)
//{
//	DMA_ITConfig(HW_DMA_CH0,kDMA_IT_Major,false);				  //��ֹDMA�ж�
//	if( DMA_IsMajorLoopComplete(HW_DMA_CH0) == 0 )                //�ж���ѭ���Ƿ��Ѿ����
//	{  
//		row ++;   //��һ��   
//	}
//	DMA_ITConfig(HW_DMA_CH0,kDMA_IT_Major,true);				  //���¿���DMA�ж�
//}


#include "camera.h"

/**
  ******************************************************************************
  * @Version:   V1.0
  * @Date:      2015/12/2
  ******************************************************************************
  * @Description:
  *       ���ܣ����OV7620����ͷ�ĳ�ʼ����DMA�ɼ�����
  *       �ӿڣ�
  *               1. ����ͷ(OV7620)��ʼ��,����������
  *               void DMA_Camera_Init(void);
  * 
  *               2. ���жϺ���,�����ⲿ�ж���ִ��
  *               void VSYNC_OnInterrupt(void);
  * 
  *               3. DMA�жϺ���,����DMA����ж���ִ��
  *               void DMA_Camera_TC_OnInterrupt(void);
  *
  * @Others:      None	// �������ݵ�˵��
  * @Global_Variable:	// ȫ�ֱ����б�
  *				  1.  extern uint8_t  Image_Complete;			// ͼ��ɼ���ɱ�־
  *				  2.  extern uint8_t  Image_Buff1[Image_Size];	// ����1
  *				  3.  extern uint8_t  Image_Buff2[Image_Size];  // ����2
  *				  4.  extern uint8_t *Image_Buff;				// ���ɼ�ͼ��ָ��
  *				  5.  extern uint8_t *Image_Deal;				// ������ͼ��ָ��
  * @Function List:	 	// ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  *     		  1.  void DMA_Camera_Init(void)				// ����ͷ��ʼ��
  * 			  2.  void VSYNC_OnInterrupt(void)				// ���жϺ���
  * 			  3.  void DMA_Camera_TC_OnInterrupt(void)		// DMA�жϺ���
  * 			  5.  void DMA_Camera_Transfer(uint32_t DestBaseAddr)	// DMA�ɼ�����
  * 			  8.  uint8_t SCCB_Config(void)					// SCCB���ú���
  * 			  9.  void sccb_init(void)						// IIC�ӿڳ�ʼ��
  * 			  10. void sccb_wait(void)						// IIC�ȴ�
  * 			  11. void sccb_start(void)						// IIC����
  * 			  12. void sccb_stop(void)						// IICֹͣ
  * 			  13. uint8_t sccb_sendByte(uint8_t data)		// IIC����һ�ֽ�
  * 			  14. uint8_t sccb_regWrite(uint8_t device,uint8_t address,uint8_t data)	// IICд�Ĵ���
**/


uint8_t  Image_Complete = 0;        				//ͼ��ɼ���ɱ�־
uint8_t  Image_TxOk = true;
uint8_t  Image_Buff1[Image_Height][Image_Width];   				//���ɼ�ͼ������
uint8_t  (* Image_Deal)[Image_Width];

void DMA_Camera_Transfer(uint32_t DestBaseAddr);

void DMA_CameraInit(void);						    //����ͷDMA��ʼ��
void GPIO_CameraInit(void); 					    //����ͷ�������ų�ʼ��

uint8_t SCCB_Config(void);  					    //SCCB����
void sccb_init(void);                               //��ʼ��SCCB�˿�ΪGPIOA
void sccb_wait(void);                               //SCCBʱ����ʱ
void sccb_start(void);                              //��ʼ��־
void sccb_stop(void);                               //ֹͣ��־
uint8_t sccb_sendByte(uint8_t data);
uint8_t sccb_regWrite(uint8_t device,uint8_t address,uint8_t data);

Register_Info ov7620_reg[] =
{
    //�Ĵ������Ĵ���ֵ��
    {CLKRC     ,0x00},  //��Ƶ
    {COMC      ,0x24},  //0x24 QVGA 0x04 VGA 
    {HSTART    ,(0x7A-Image_Width/2/2-10)},//��ȡͼ��ͼ��У��
    {HSIZE     ,(0x7A+Image_Width/2/2+10)},   
    {VSTRT     ,(0X7E-Image_Height/2)},
    {VSIZE     ,(0X7E+Image_Height/2)},
    {VEEE      ,0x42},  //��ֱ��Ե��ǿ
    {COMH      ,0x00},     //ɨ��ģʽ

};

/***********************************************************************************************
 ���ܣ�����ͷ�ܳ�ʼ��
 �βΣ�none
 ���أ�none
 ��⣺��main�����е���
************************************************************************************************/
uint8_t tick;   //������

void Camera_Init(void)
{
 	GPIO_CameraInit();	    //����ͷ�������ų�ʼ��     Data:B0-B7 SCL:A27 SDA:A28  ���ж�:A29 DMA����:A25
	sccb_init();			//����SCCB�������ŵ�ʱ��
	tick=SCCB_Config();          //SCCB��ʼ������Ҫ�ǿ���
	DMA_CameraInit();		//DMA1_Channel7��1��0
}


// /***********************************************************************************************
//  ���ܣ�DMA��ʼ��
//  �βΣ�none
//  ���أ�none
//  ��⣺none
// ************************************************************************************************/
void DMA_CameraInit(void)
{
	DMA_InitTypeDef DMA_InitStruct1 = {0};
	
	DMA_InitStruct1.chl = HW_DMA_CH0; 									        //DMAͨ��0
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;							//����Դ,����PORTA
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;              //����ģʽ��ѡ�������ģʽ�������ڴ���
    DMA_InitStruct1.minorLoopByteCnt = 1;										//MINOR LOOP (��ѭ��)һ�δ�����ֽ���
    DMA_InitStruct1.majorLoopCnt = Image_Width;									//MAJOR LOOP ѭ������
    
	//����Դ��ַ�������
    DMA_InitStruct1.sAddr = (uint32_t)(PTB_BASE+0x10);//(uint32_t)PTB->PDIR;	//�����Դ��ַ
    DMA_InitStruct1.sLastAddrAdj = 0;											//��ѭ����Դ��ַ������ƫ��
    DMA_InitStruct1.sAddrOffset = 0;											//��ѭ����Դ��ַ������ƫ��
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;						    //ÿ�δ���8λ���ݳ���		
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;									//��ֹԴ��ַģ��
    
	//����Ŀ�ĵ�ַ�������
    DMA_InitStruct1.dAddr = (uint32_t)&(Image_Buff1[0]);						//�����Ŀ���ַ
    DMA_InitStruct1.dLastAddrAdj = 0;											//��ѭ����Դ��ַ��ƫ��
    DMA_InitStruct1.dAddrOffset = 1;											//��ѭ����Դ��ַƫ��
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;							//ÿ�δ���8λ���ݳ���
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;									//��ֹĿ���ַģ��

    DMA_Init(&DMA_InitStruct1);
	DMA_EnableAutoDisableRequest(HW_DMA_CH0,true);								//�������Զ�ֹͣ
}


/***********************************************************************************************
 ���ܣ�camera��GPIO�ڳ�ʼ��
 �β�: none
 ���أ�none
 ��⣺none
************************************************************************************************/
void GPIO_CameraInit(void)
{
	uint8_t i=0;
	
	for(i=0; i<8; i++)
	{
		GPIO_QuickInit(HW_GPIOB,i,kGPIO_Mode_IPU);   //���ݿ����ų�ʼ��   B0-B7
	}
	
	GPIO_QuickInit(HW_GPIOA,25,kGPIO_Mode_IFT);      //���ж����ų�ʼ��  �ж������������ش���		 A29
	GPIO_QuickInit(HW_GPIOA,26,kGPIO_Mode_IFT);      //���жϳ�ʼ��
	GPIO_QuickInit(HW_GPIOA,24,kGPIO_Mode_IPU);      //DMA�������ų�ʼ�� �ж�������DMA�����ش���     A25
	
	GPIO_QuickInit(HW_GPIOA,28,kGPIO_Mode_IPU);      //SCCB��SDA���ų�ʼ�� 							 A28
	GPIO_QuickInit(HW_GPIOA,27,kGPIO_Mode_OPP);      //SCCB��SCL���ų�ʼ��							 A27
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCCBЭ��
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  SCCB���ó�ʼ��
  * @param  None
  * @retval None
  */
uint8_t cfgnum = sizeof(ov7620_reg)/sizeof(ov7620_reg[0]);   /*�ṹ�������Ա��Ŀ*/

uint8_t SCCB_Config(void)
{
    uint16_t i = 0;
	sccb_init();			//����SCCB�������ŵ�ʱ��
    while( 1 == sccb_regWrite ( 0x42, 0x12, 0xA4 ) ) /*��λsensor */
    {
        i++;
		if(i == 20)
		{
			return 0 ;
		}		
    }
    for( i = 0 ; i < cfgnum ; i++ )
    {
         while( 1 == sccb_regWrite(0x42,ov7620_reg[i].Address, ov7620_reg[i].Value) )
         {
            return 0;
         }
    }
    return 1;
}

/**
  * @brief  SCCB Port Clock ENABLE.
  * @param  None
  * @retval None
  */
void sccb_init(void)
{
	SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;
}

/**
  * @brief  SCCB delay.
  * @param  None
  * @retval None
  */
void sccb_wait(void)
{
  uint32_t i;
  
  for( i=0; i<2000; i++)
  {
//     __asm("nop");
  }
}
/**
  * @brief  SCCB Start.
  * @param  None
  * @retval None
  */
void sccb_start(void)
{
	SCL_OUT;
	SDA_OUT;

	SDA_HIGH;
	//sccb_wait();
	SCL_HIGH;
	sccb_wait();
	SDA_LOW;
	sccb_wait();
	SCL_LOW;
}

/**
  * @brief  SCCB Stop.
  * @param  None
  * @retval None
  */
void sccb_stop(void)
{
	SCL_OUT;
	SDA_OUT;

	SDA_LOW;
	sccb_wait();
	SCL_HIGH;
	sccb_wait();
	SDA_HIGH;
	sccb_wait();
}

/**
  * @brief  SCCB Send Single Byte.
  * @param  None
  * @retval None
  */
uint8_t sccb_sendByte(uint8_t data)
{
	uint8_t i;
	uint8_t ack;
	SDA_OUT;
	for( i=0; i<8; i++)
	{
		if(data & 0x80)
		{
			SDA_HIGH;
		}
		else 
			SDA_LOW;
		data <<= 1;
		sccb_wait();
		SCL_HIGH;
		sccb_wait();
		SCL_LOW;
		sccb_wait();
	}
	SDA_HIGH;
	SDA_IN;
	sccb_wait();
	SCL_HIGH;
	sccb_wait();
	ack = SDA_DATA;
	SCL_LOW;
	sccb_wait();
	return ack;
}

/**
  * @brief  SCCB Write Register.
  * @param  None
  * @retval None
  */
uint8_t sccb_regWrite(uint8_t device,uint8_t address,uint8_t data)
{
	uint8_t i;
	uint8_t ack;
	for( i=0; i<20; i++)
	{
		sccb_start();
		ack = sccb_sendByte(device);
		if( ack != 0 )
		{
			sccb_stop();
			continue;
		}

		ack = sccb_sendByte(address);
		if( ack != 0 )
		{
			sccb_stop();
			continue;
		}

		ack = sccb_sendByte(data);
		if( ack != 0 )
		{
			sccb_stop();
			continue;
		}

		sccb_stop();
		if( ack == 0 ) 
			break;
	}
	return ack;
}

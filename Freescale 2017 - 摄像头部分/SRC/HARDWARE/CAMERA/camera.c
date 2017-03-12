#include "camera.h"

/**
  ******************************************************************************
  * @Version:   V1.0
  * @Date:      2015/12/2
  ******************************************************************************
  * @Description:
  *       功能：完成OV7620摄像头的初始化、DMA采集工作
  *       接口：
  *               1. 摄像头(OV7620)初始化,主函数调用
  *               void DMA_Camera_Init(void);
  * 
  *               2. 场中断函数,放在外部中断内执行
  *               void VSYNC_OnInterrupt(void);
  * 
  *               3. DMA中断函数,放在DMA完成中断内执行
  *               void DMA_Camera_TC_OnInterrupt(void);
  *
  * @Others:      None	// 其它内容的说明
  * @Global_Variable:	// 全局变量列表
  *				  1.  extern uint8_t  Image_Complete;			// 图像采集完成标志
  *				  2.  extern uint8_t  Image_Buff1[Image_Size];	// 缓冲1
  *				  3.  extern uint8_t  Image_Buff2[Image_Size];  // 缓冲2
  *				  4.  extern uint8_t *Image_Buff;				// 待采集图像指针
  *				  5.  extern uint8_t *Image_Deal;				// 待处理图像指针
  * @Function List:	 	// 主要函数列表，每条记录应包括函数名及功能简要说明
  *     		  1.  void DMA_Camera_Init(void)				// 摄像头初始化
  * 			  2.  void VSYNC_OnInterrupt(void)				// 场中断函数
  * 			  3.  void DMA_Camera_TC_OnInterrupt(void)		// DMA中断函数
  * 			  5.  void DMA_Camera_Transfer(uint32_t DestBaseAddr)	// DMA采集函数
  * 			  8.  uint8_t SCCB_Config(void)					// SCCB配置函数
  * 			  9.  void sccb_init(void)						// IIC接口初始化
  * 			  10. void sccb_wait(void)						// IIC等待
  * 			  11. void sccb_start(void)						// IIC开启
  * 			  12. void sccb_stop(void)						// IIC停止
  * 			  13. uint8_t sccb_sendByte(uint8_t data)		// IIC发送一字节
  * 			  14. uint8_t sccb_regWrite(uint8_t device,uint8_t address,uint8_t data)	// IIC写寄存器
**/


uint8_t  Image_Complete = 0;        				//图像采集完成标志
uint8_t  Image_TxOk = true;
uint8_t  Image_Buff1[Image_Height][Image_Width];   				//待采集图像数组
uint8_t  (* Image_Deal)[Image_Width];

void DMA_Camera_Transfer(uint32_t DestBaseAddr);

void DMA_CameraInit(void);						    //摄像头DMA初始化
void GPIO_CameraInit(void); 					    //摄像头所用引脚初始化

uint8_t SCCB_Config(void);  					    //SCCB配置
void sccb_init(void);                               //初始化SCCB端口为GPIOA
void sccb_wait(void);                               //SCCB时序延时
void sccb_start(void);                              //起始标志
void sccb_stop(void);                               //停止标志
uint8_t sccb_sendByte(uint8_t data);
uint8_t sccb_regWrite(uint8_t device,uint8_t address,uint8_t data);

Register_Info ov7620_reg[] =
{
    //寄存器，寄存器值次
    {CLKRC     ,0x00},  //分频
    {COMC      ,0x24},  //0x24 QVGA 0x04 VGA 
    {HSTART    ,(0x7A-Image_Width/2/2-10)},//截取图像，图像校正
    {HSIZE     ,(0x7A+Image_Width/2/2+10)},   
    {VSTRT     ,(0X7E-Image_Height/2)},
    {VSIZE     ,(0X7E+Image_Height/2)},
    {VEEE      ,0x42},  //垂直边缘增强
    {COMH      ,0x00},     //扫描模式

};

/***********************************************************************************************
 功能：摄像头总初始化
 形参：none
 返回：none
 详解：在main函数中调用
************************************************************************************************/
uint8_t tick;   //检测参数

void Camera_Init(void)
{
 	GPIO_CameraInit();	    //摄像头所用引脚初始化     Data:B0-B7 SCL:A27 SDA:A28  场中断:A29 DMA触发:A25
	sccb_init();			//开启SCCB所用引脚的时钟
	tick=SCCB_Config();          //SCCB初始化，主要是开窗
	DMA_CameraInit();		//DMA1_Channel7，1，0
}


// /***********************************************************************************************
//  功能：DMA初始化
//  形参：none
//  返回：none
//  详解：none
// ************************************************************************************************/
void DMA_CameraInit(void)
{
	DMA_InitTypeDef DMA_InitStruct1 = {0};
	
	DMA_InitStruct1.chl = HW_DMA_CH0; 									        //DMA通道0
    DMA_InitStruct1.chlTriggerSource = PORTA_DMAREQ;							//触发源,来自PORTA
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;              //触发模式，选择的正常模式，非周期触发
    DMA_InitStruct1.minorLoopByteCnt = 1;										//MINOR LOOP (副循环)一次传输的字节数
    DMA_InitStruct1.majorLoopCnt = Image_Width;									//MAJOR LOOP 循环次数
    
	//配置源地址传输参数
    DMA_InitStruct1.sAddr = (uint32_t)(PTB_BASE+0x10);//(uint32_t)PTB->PDIR;	//传输的源地址
    DMA_InitStruct1.sLastAddrAdj = 0;											//主循环的源地址不进行偏移
    DMA_InitStruct1.sAddrOffset = 0;											//副循环的源地址不进行偏移
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;						    //每次传输8位数据长度		
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;									//禁止源地址模数
    
	//配置目的地址传输参数
    DMA_InitStruct1.dAddr = (uint32_t)&(Image_Buff1[0]);						//传输的目标地址
    DMA_InitStruct1.dLastAddrAdj = 0;											//主循环的源地址不偏移
    DMA_InitStruct1.dAddrOffset = 1;											//副循环的源地址偏移
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;							//每次传输8位数据长度
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;									//禁止目标地址模数

    DMA_Init(&DMA_InitStruct1);
	DMA_EnableAutoDisableRequest(HW_DMA_CH0,true);								//传输完自动停止
}


/***********************************************************************************************
 功能：camera的GPIO口初始化
 形参: none
 返回：none
 详解：none
************************************************************************************************/
void GPIO_CameraInit(void)
{
	uint8_t i=0;
	
	for(i=0; i<8; i++)
	{
		GPIO_QuickInit(HW_GPIOB,i,kGPIO_Mode_IPU);   //数据口引脚初始化   B0-B7
	}
	
	GPIO_QuickInit(HW_GPIOA,25,kGPIO_Mode_IFT);      //场中断引脚初始化  中断类型是上升沿触发		 A29
	GPIO_QuickInit(HW_GPIOA,26,kGPIO_Mode_IFT);      //行中断初始化
	GPIO_QuickInit(HW_GPIOA,24,kGPIO_Mode_IPU);      //DMA传输引脚初始化 中断类型是DMA上升沿触发     A25
	
	GPIO_QuickInit(HW_GPIOA,28,kGPIO_Mode_IPU);      //SCCB的SDA引脚初始化 							 A28
	GPIO_QuickInit(HW_GPIOA,27,kGPIO_Mode_OPP);      //SCCB的SCL引脚初始化							 A27
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SCCB协议
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  SCCB配置初始化
  * @param  None
  * @retval None
  */
uint8_t cfgnum = sizeof(ov7620_reg)/sizeof(ov7620_reg[0]);   /*结构体数组成员数目*/

uint8_t SCCB_Config(void)
{
    uint16_t i = 0;
	sccb_init();			//开启SCCB所用引脚的时钟
    while( 1 == sccb_regWrite ( 0x42, 0x12, 0xA4 ) ) /*复位sensor */
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

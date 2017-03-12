#ifndef _CAMERA_H__
#define _CAMERA_H__

#include "gpio.h"
#include "dma.h"
#include "common.h"
#include "rtc.h"

#define Image_Width  200//		//改宽度时必须要设置SCCB,此单片机的RAM只有64M
#define Image_Height 160        //宽度必须是4的倍数
#define Image_Size (Image_Width * Image_Height) //图像尺寸

#define SCL_OUT   {PTA->PDDR |= (1<<(27));}		//SCL为串行时钟线
#define SDA_OUT   {PTA->PDDR |= (1<<(28));}		//SDA为串行数据线
#define SDA_IN    {PTA->PDDR &= ~(1<<(28));}

#define SCL_HIGH  {PTA->PDOR |= (1<<(27));}
#define SCL_LOW   {PTA->PDOR &= ~(1<<(27));}
#define SDA_HIGH  {PTA->PDOR |= (1<<(28));}
#define SDA_LOW   {PTA->PDOR &= ~(1<<(28));}
#define SDA_DATA  ((PTA->PDIR >> 28)& 0x01)

#define GAIN      0x00
#define BLUE      0x01     
#define RED       0x02
#define SATC      0x03      //饱和度
#define GAVG      0x06      //亮度控制
#define ASC       0x07
#define WBBC      0x0C      //白平衡背景控制 (蓝)
#define COM4      0x0D      //白平衡背景控制 (红)
#define AEC       0x10      //自曝光控制
#define CLKRC     0x11      //分频 
#define COMA      0x12      //
#define COMB      0x13  
#define COMC      0x14      //0x24 QVGA   0x04 VGA
#define COMD      0x15      
#define FD        0x16      //丢帧
#define HSTART    0x17      //水平窗口开始   Min[05] Max[F6]   0x2f    VGA:(HSIZE-HSTARTHSIZE)*4   
#define HSIZE     0x18      //水平窗口结束   同上              0xCf    QVGA:(HSIZE-HSTART)*2            
#define VSTRT     0x19      //垂直窗口开始   同上              6       VGA:(VSIZE-VSTRT+1)
#define VSIZE     0x1A      //垂直窗口结束   同上              245     QVGA:(VSIZE-VSTRT+1)
#define PSHFT     0x1B      //像素偏移  
#define MIDH      0x1C      
#define MIDL      0x1D      
#define COME      0x20
#define YCOF      0x21      //Y输出偏移调整
#define UCOF      0x22      //U输出偏移调整
#define CCC       0x23      //晶体电流控制
#define AEW       0x24
#define AEB       0x25  
#define COMF      0x26
#define COMG      0x27
#define COMH      0x28     //0x20  连续扫描模式   0x00  隔行扫描
#define COMI      0x29
#define FRA1      0x2A
#define FRA2      0x2B
#define BE        0x2C
#define COMJ      0x2D     //隔行扫描：0xC1  只输出奇场   0x81  奇偶场输出帧率30帧/s
#define VCOF      0x2E     //V输出偏移调整
#define SPCA      0x60
#define SPCB      0x61
#define RGB_GC    0x62
#define Y_GC      0x64
#define SPCC      0x65
#define AWBPC     0x66
#define CPS       0x67
#define SPCD      0x68
#define ANS       0x69
#define VEEE      0x6A    //垂直边缘增强
#define EONC      0x6F    //奇偶场噪声补偿
#define COMK      0x70    
#define COMJJ     0x71  
#define HS1ES     0x72
#define HS2ES     0x73
#define COMM      0x74
#define COMN      0x75
#define COMO      0x76
#define FALS      0x7C

extern uint8_t  Image_Complete;
extern uint8_t  Image_TxOk;
extern uint8_t  Image_Buff1[Image_Height][Image_Width]; 
extern uint8_t *Image_Deal;
extern uint8_t 	count;

void Camera_Init(void);
void VSYNC_OnInterrupt(void);		//摄像头场中断
void DMA_Camera_TC_OnInterrupt(void);

typedef struct
{
	uint8_t Address;			       /*寄存器地址*/
	uint8_t Value;		           /*寄存器值*/
}Register_Info;

#endif



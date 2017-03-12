#include "oled.h"

/*===========================================================
文件名称：OLED驱动文件
引脚设置：OLED_CS        P2.0
          OLED_RES       P2.1
          OLED_DC        P2.2
          OLED_SCLK      P2.3
          OLED_SDIN      P2.4
使用说明：使用时将OLED.c和OLED.h共同添加进工程
          可自行更改引脚设置
主要函数：void OLED_Init(void);
          直接调用以初始化液晶

          void OLED_Fill(uint8_t dat);
          传递字符参数以填充整个屏幕，常用来传递0x00以清屏

          void OLED_Show_Sentence(uint8_t x, uint8_t y, uint8_t *dataptr);
          传递显示横纵坐标x，y，以及字符串指针以在液晶任意位置显示字符串
===========================================================*/

#include "OLED.h"

/*============================================================
                    LED各部分设置函数   开始
=============================================================*/
void SetStartColumn(uint8_t dat)
{
    OLED_Write_Byte((0x00+dat%16), COM);        // Set Lower Column Start Address for Page Addressing Mode
                                                // Default => 0x00
    OLED_Write_Byte((0x10+dat/16), COM);        // Set Higher Column Start Address for Page Addressing Mode
                                                // Default => 0x10
}

void SetAddressingMode(uint8_t dat)
{
    OLED_Write_Byte(0x20, COM);         // Set Memory Addressing Mode
    OLED_Write_Byte(dat, COM);          // Default => 0x02
                                        // 0x00 => Horizontal Addressing Mode
                                        // 0x01 => Vertical Addressing Mode
                                        // 0x02 => Page Addressing Mode
}

void SetColumnAddress(uint8_t a, uint8_t b)
{
    OLED_Write_Byte(0x21, COM);        // Set Column Address
    OLED_Write_Byte(a, COM);           // Default => 0x00 (Column Start Address)
    OLED_Write_Byte(b, COM);           // Default => 0x7F (Column End Address)
}

void SetPageAddress(uint8_t a, uint8_t b)
{
    OLED_Write_Byte(0x22, COM);        // Set Page Address
    OLED_Write_Byte(a, COM);           // Default => 0x00 (Page Start Address)
    OLED_Write_Byte(b, COM);           // Default => 0x07 (Page End Address)
}

void SetStartLine(uint8_t dat)
{
    OLED_Write_Byte((0x40|dat), COM);       // Set Display Start Line
                                            // Default => 0x40 (0x00)
}

void SetContrastControl(uint8_t dat)
{
    OLED_Write_Byte(0x81, COM);          // Set Contrast Control
    OLED_Write_Byte(dat, COM);           // Default => 0x7F
}

void SetChargePump(uint8_t dat)
{
    OLED_Write_Byte(0x8D, COM);             // Set Charge Pump
    OLED_Write_Byte((0x10|dat), COM);       // Default => 0x10
                                            // 0x10 (0x00) => Disable Charge Pump
                                            // 0x14 (0x04) => Enable Charge Pump
}

void SetSegmentRemap(uint8_t dat)
{
    OLED_Write_Byte((0xA0|dat), COM);       // Set Segment Re-Map
                                            // Default => 0xA0
                                            // 0xA0 (0x00) => Column Address 0 Mapped to SEG0
                                            // 0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

void SetEntireDisplay(uint8_t dat)
{
    OLED_Write_Byte((0xA4|dat), COM);       // Set Entire Display On / Off
                                            // Default => 0xA4
                                            // 0xA4 (0x00) => Normal Display
                                            // 0xA5 (0x01) => Entire Display On
}

void SetInverseDisplay(uint8_t dat)
{
    OLED_Write_Byte((0xA6|dat), COM);       // Set Inverse Display On/Off
                                            // Default => 0xA6
                                            // 0xA6 (0x00) => Normal Display
                                            // 0xA7 (0x01) => Inverse Display On
}

void SetMultiplexRatio(uint8_t dat)
{
    OLED_Write_Byte(0xA8, COM);              // Set Multiplex Ratio
    OLED_Write_Byte(dat, COM);               // Default => 0x3F (1/64 Duty)
}

void SetDisplayOnOff(uint8_t dat)
{
    OLED_Write_Byte((0xAE|dat), COM);       // Set Display On/Off
                                            // Default => 0xAE
                                            // 0xAE (0x00) => Display Off
                                            // 0xAF (0x01) => Display On
}

void SetStartPage(uint8_t dat)
{
    OLED_Write_Byte((0xB0|dat), COM);       // Set Page Start Address for Page Addressing Mode
                                            // Default => 0xB0 (0x00)
}

void SetCommonRemap(uint8_t dat)
{
    OLED_Write_Byte((0xC0|dat), COM);       // Set COM Output Scan Direction
                                            // Default => 0xC0
                                            // 0xC0 (0x00) => Scan from COM0 to 63
                                            // 0xC8 (0x08) => Scan from COM63 to 0
}

void SetDisplayOffset(uint8_t dat)
{
    OLED_Write_Byte(0xD3, COM);         // Set Display Offset
    OLED_Write_Byte(dat, COM);          // Default => 0x00
}

void SetDisplayClock(uint8_t dat)
{
    OLED_Write_Byte(0xD5, COM);         // Set Display Clock Divide Ratio / Oscillator Frequency
    OLED_Write_Byte(dat, COM);          // Default => 0x80
                                        // D[3:0] => Display Clock Divider
                                        // D[7:4] => Oscillator Frequency
}

void SetPrechargePeriod(uint8_t dat)
{
    OLED_Write_Byte(0xD9, COM);         // Set Pre-Charge Period
    OLED_Write_Byte(dat, COM);          // Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                                        // D[3:0] => Phase 1 Period in 1~15 Display Clocks
                                        // D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void SetCommonConfig(uint8_t dat)
{
    OLED_Write_Byte(0xDA, COM);             // Set COM Pins Hardware Configuration
    OLED_Write_Byte((0x02|dat), COM);       // Default => 0x12 (0x10)
                                            // Alternative COM Pin Configuration
                                            // Disable COM Left/Right Re-Map
}

void SetVCOMH(uint8_t dat)
{
    OLED_Write_Byte(0xDB, COM);         // Set VCOMH Deselect Level
    OLED_Write_Byte(dat, COM);          // Default => 0x20 (0.77*VCC)
}

void SetNop(void)
{
    OLED_Write_Byte(0xE3, COM);        // Command for No Operation
}

void OLED_Set_XY(uint8_t x, uint8_t y)
{ 
    OLED_Write_Byte((0xb0 + y), COM);
    OLED_Write_Byte(((((x*6) & 0xf0) >> 4) | 0x10), COM);
    OLED_Write_Byte((((x*6) & 0x0f) | 0x00), COM); 
} 
 
/*============================================================
                    LED各部分设置函数   结束
=============================================================*/

/***************************************
名称：填充显示函数
功能：向OLED填充某一字符
参数：uint8_t dat：要填充的数据
返回：无
****************************************/
void OLED_Fill(uint8_t dat)
{
    uint8_t Page, Column;
    
    for(Page = 0; Page < 8; Page++)
    {
        OLED_Write_Byte((0xb0 + Page), COM);  //0xb0+0~7表示页0~7
        OLED_Write_Byte(0x00, COM);           //0x00+0~16表示将128列分成16组其地址在某组中的第几列
        OLED_Write_Byte(0x10, COM);           //0x10+0~16表示将128列分成16组其地址所在第几组
        
        for(Column = 0; Column < 128; Column++)
        {
            OLED_Write_Byte(dat, DAT);
        }
    }
} 

void OLED_Init(void)
{
    uint8_t i;
	
	GPIO_QuickInit(OLED_CS_PORT,OLED_CS_PIN,kGPIO_Mode_OPP);
	GPIO_QuickInit(OLED_DC_PORT,OLED_DC_PIN,kGPIO_Mode_OPP);
	GPIO_QuickInit(OLED_RES_PORT,OLED_RES_PIN,kGPIO_Mode_OPP);
	GPIO_QuickInit(OLED_SDIN_PORT,OLED_SDIN_PIN,kGPIO_Mode_OPP);
	GPIO_QuickInit(OLED_SCLK_PORT,OLED_SCLK_PIN,kGPIO_Mode_OPP);
	
    OLED_SCLK_H;
    OLED_RES_L;
    for(i = 0; i < 100; i++)
    {
        __NOP();              //等待RC复位完毕      
    } 
    OLED_RES_H;
    
    SetDisplayOnOff(0x00);     // Display Off (0x00/0x01)
    SetDisplayClock(0x80);     // Set Clock as 100 Frames/Sec
    SetMultiplexRatio(0x3F);   // 1/64 Duty (0x0F~0x3F)
    SetDisplayOffset(0x00);    // Shift Mapping RAM Counter (0x00~0x3F)
    SetStartLine(0x00);        // Set Mapping RAM Display Start Line (0x00~0x3F)
    SetChargePump(0x04);       // Enable Embedded DC/DC Converter (0x00/0x04)
    SetAddressingMode(0x02);   // Set Page Addressing Mode (0x00/0x01/0x02)
    SetSegmentRemap(0x01);     // Set SEG/Column Mapping     0x00左右反置 0x01正常
    SetCommonRemap(0x08);      // Set COM/Row Scan Direction 0x00上下反置 0x08正常
    SetCommonConfig(0x10);     // Set Sequential Configuration (0x00/0x10)
    SetContrastControl(0xCF);  // Set SEG Output Current
    SetPrechargePeriod(0xF1);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    SetVCOMH(0x40);            // Set VCOM Deselect Level
    SetEntireDisplay(0x00);    // Disable Entire Display On (0x00/0x01)
    SetInverseDisplay(0x00);   // Disable Inverse Display On (0x00/0x01)  
    SetDisplayOnOff(0x01);     // Display On (0x00/0x01)
    OLED_Fill(0x00);            // 初始清屏
    OLED_Set_XY(0, 0);
}

/*********************************************************************
名称：OLED写字节函数
功能：向OLED写一个字节的数据或命令
参数：uint8_t uint8_t：要写入的数据或命令
      uint8_t control：控制写入数据或命令，DAT为数据，COM为命令
返回：无
*********************************************************************/
static void OLED_Write_Byte(uint8_t data, uint8_t control)
{
    uint8_t i = 0;
    
    OLED_CS_L;
    
    if(control == DAT)
    {
        OLED_DC_H;
    }
    if(control == COM)
    {
        OLED_DC_L;
    }
    
    for(i = 0; i < 8; i++)
    {
        if(data&0x80)
        {
            OLED_SDIN_H;
        }
        else
        {
            OLED_SDIN_L;
        }
        
        OLED_SCLK_L;
        data <<= 1;
        OLED_SCLK_H;
    }
    
    OLED_CS_H;
}

/***************************************
名称：光标显示函数
功能：在(x,y)显示光标
参数：X--橫坐标  Y--纵坐标
返回：无
****************************************/
void OLED_Cursor(uint8_t x,uint8_t y)
{
	uint8_t i=0;
    OLED_Set_XY(x,y);        
    for(i = 0; i < 3; i++)
    {
        OLED_Write_Byte(0XFF, DAT);
    }
}

/***************************************
名称：光标清除函数
功能：在(x,y)清除光标
参数：X--橫坐标  Y--纵坐标
返回：无
****************************************/
void OLED_ClearCursor(uint8_t x,uint8_t y)
{
	uint8_t i=0;
    OLED_Set_XY(x,y);
    for(i = 0;i < 3; i++)
    {
        OLED_Write_Byte(0x00, DAT);
    }
}

/***************************************
名称：清屏函数
功能：清屏
参数：无
返回：无
****************************************/
void OLED_CLS(void)
{
	uint8_t y,x;	
	for(y=0;y<8;y++)
	{
		OLED_Write_Byte((0xb0+y),COM);
		OLED_Write_Byte(0x00,COM);
		OLED_Write_Byte(0x10,COM); 
		for(x=0;x<128;x++)
			OLED_Write_Byte(0,DAT);
	}
}

/***************************************
名称：部分清屏函数
功能：从(x,y)开始清除N个字符
参数：无
返回：无
****************************************/
void OLED_PARTCLS(uint8_t x,uint8_t y,uint8_t len)
{
    OLED_Set_XY(x,y);
    len = len*6;
    while(len --)
    {
        OLED_Write_Byte(0,DAT);
    }
}

/***********************************************************
名称：OLED写字符函数
功能：向OLED写入一个字符
参数：uint8_t data：要显示的字符，从空格（32）开始
返回：无
***********************************************************/
static void OLED_Print_ASCII(uint8_t data)
{
    uint8_t i;
    
    if((data-32) > 90)  //若超出范围则直接返回
    {
        return;
    }
    
    for(i = 0; i < 6; i++)
    {
        OLED_Write_Byte(ASCII_Table[data-32][i], DAT);
    }
}

/***************************************************************
名称：OLED字符串显示函数
功能：在OLED上显示一段字符串
参数：uint8_t x：显示的起始横坐标，范围0~21
      uint8_t y：显示的起始纵坐标，范围0~7
      uint8_t *dataptr：显示的字符串指针
例：在(0,0)显示WHUT
OLED_Show_Sentence(0,0,"WHUT");
注意：若传递的第三个参数是字符数组，要确保最后一个元素是‘\0’
****************************************************************/
void OLED_Show_Sentence(uint8_t x, uint8_t y, uint8_t *dataptr)
{
    uint8_t i = 0;
    
    OLED_Set_XY(x, y);
    
    while(*dataptr)
    {        
        OLED_Print_ASCII(*dataptr++);
        
        i++;
    }
}

/***************************************************************
名称：OLED数字显示函数
功能：在OLED上显示数字
参数：uint8_t x：显示的起始横坐标，范围0~21
      uint8_t y：显示的起始纵坐标，范围0~7
      int32_t data:需要显示的数字，范围-99999~99999
例：在(0,0)显示9531
OLED_Show_Number(0,0,9531)
****************************************************************/
void OLED_Show_Number(uint8_t x, uint8_t y, int32_t data)
{
    char s[7]="      ";
    if(data>=0)s[0]=' ';
    else
    {
        s[0]='-';
        data=-data;
    }
    if(data>9999) s[1] = data/10000 + '0';
    if(data>999)  s[2] = data%10000/1000 + '0';
    if(data>99)   s[3] = data%1000/100 + '0';
    if(data>9)    s[4] = data%100/10 + '0';
    s[5] = data%10 + '0';
    s[6] ='\0';
    
    OLED_Show_Sentence(x,y,s);
}

/***************************************************************
名称：OLED字符串+数字显示函数
功能：在OLED上显示字符串+数字
参数：uint8_t x：显示的起始横坐标，范围0~21
     uint8_t y：显示的起始纵坐标，范围0~7
     uint8_t *dataptr：显示的字符串指针 
     int32_t data:需要显示的数字，范围-99999~99999
例：在0,0显示Value:9982
OLED_Show_StrNum(0,0,"Value:",9982);
****************************************************************/
void OLED_Show_StrNum(uint8_t x, uint8_t y,uint8_t *dataptr,int32_t data)
{
    char s[7]="      ";
    char length = 0;
    if(data>=0)s[0]=' ';
    else
    {
        s[0]='-';
        data=-data;
    }
    if(data>9999) s[1] = data/10000 + '0';
    if(data>999)  s[2] = data%10000/1000 + '0';
    if(data>99)   s[3] = data%1000/100 + '0';
    if(data>9)    s[4] = data%100/10 + '0';
    s[5] = data%10 + '0';
    s[6] ='\0';
    length = strlen(dataptr);
    if(length > 15)
    {
	  OLED_Show_Sentence(x,y,"String is too long");  
    }
    else
    {
    	OLED_Show_Sentence(x,y,dataptr);
    	OLED_Show_Sentence(x+length,y,s);
    }
}

/***************************************************************
名称：OLED图片显示函数
功能：显示BMP图片，大小为(x1-x0+1)X((y1-y0+1)*8)
	  BMP图片先取页码y为0以8位(低位在上)为单位取满(x1-x0+1)列
	  再取页码y为1图片显示以8位(低位在上)为单位取满(x1-x0+1)列依次类推
参数：x的范围为0-127，y为页的范围0~7
注意：未测试
****************************************************************/
void OLED_PrintBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t bmp[])
{ 	
	uint16_t ii=0;
	uint8_t x,y;
	for(y=y0;y<=y1;y++)
	{
		OLED_Set_XY(x0,y);				
		for(x=x0;x<x1;x++)
		{      
			OLED_Write_Byte(bmp[ii++],DAT);	    	
		}
	}
}

/***************************************************************
名称：OLED赛道显示函数
功能：显示赛道
参数：Lr为左线数组指针，Rr为右线数组指针，Mr为中线数组指针
注意：未测试
****************************************************************/
#define ZOOMHIGH   64			//OLED中显示赛道的区域的高
#define ZOOMWIDTH  80			//OLED中显示赛道的区域的宽
void Road_Show(uint8_t* Lr,uint8_t* Rr)//,uint8_t* Mr)
{
    uint8_t Data[ZOOMHIGH * ZOOMWIDTH / 8] = {0};		//申请数组
    uint8_t line;
    uint8_t Num2 ,k;
    uint16_t  Num1;
	uint8_t Line = 0;
	uint8_t Row = 0;
	uint8_t Col = 0;
    for(Line = 0;Line < ZOOMHIGH;Line++)	//64行
    {
        line = Line << 1;	   //取图像数组中的偶数行,即0 2 4 6...128
        Num2 = 1 << (Line%8);  //

        if(Lr[line] != 255)	   //所取图像的边线有效
        {
            k =  (Lr[line]) / (Image_Width / ZOOMWIDTH) ;
            Num1 = (Line >> 3) * ZOOMWIDTH + k;
            Data[Num1] |= Num2;
        }
        if(Rr[line] != 255)
        {
            k =  (Rr[line])/ (Image_Width / ZOOMWIDTH);
            Num1 = (Line >> 3) * ZOOMWIDTH + k;
            Data[Num1] |= Num2;
        }
//        if(Mr[line] != 255)
//        { 
//            k =  (Mr[line])/3;
//            Num1 = (Line / 8) * 80 + k;
//            Data[Num1] |= Num2;
//        }
    }
    for(Row = 0; Row < (ZOOMHIGH / 8) ; Row++) //80列
    {
        OLED_Set_XY(0,Row);
        for(Col = 0; Col < ZOOMWIDTH; Col++) 
        {
			OLED_Write_Byte(Data[Row * 80 + Col],DAT);
        }
    }
}

/***************************************************************
名称:OLED字符串+数字显示函数
功能:在OLED上显示字符串+带小数的数字
参数:uint8_t x：显示的起始横坐标，范围0~21
	 uint8_t y：显示的起始纵坐标，范围0~7
     uint8_t *detaptr：显示的字符串指针
     float data：需要显示的数字
例：在0,0显示Double:982.98
OLED_Show_StrDouble(0,0,"Double:",982.98);
****************************************************************/
void OLED_Show_StrDouble(uint8_t x, uint8_t y,uint8_t *dataptr,double data)
{
	uint8_t length = 0;
    int32_t d;
    char s[10];
    if(data>=0)
    {
        s[0]='+';
    }
    else
    {
        s[0]='-';
        data=-data;
    }
    d=(int32_t)data;
    data=data-d;
    s[1]=d/100+'0';
    s[2]=d%100/10+'0';
    s[3]=d%10+'0';
    s[4]='.';
	
    data*=100000;
    d=(long)data;
    d/=10;
    s[5] = d/1000 + '0';
    s[6] = d%1000/100 + '0';
    s[7] = d%100/10 + '0';
    s[8] = d%10+'0';
    s[9] = '\0';
	
	length = strlen(dataptr);
	if(length > 12)
    {
	  	OLED_Show_Sentence(x,y,"String is too long");  
    }
	else
    {
    	OLED_Show_Sentence(x,y,dataptr);
    	OLED_Show_Sentence(x+length,y,s);
    }
}

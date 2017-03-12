/**
  ******************************************************************************
  * @file    common.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * \date    2015.10.04 FreeXc完善了common.h & common.c文件的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    此文件为内部文件，用户无需调用和修改  
  ******************************************************************************
  */
#include "common.h"
#include <string.h>


#define MCGOUT_TO_CORE_DIVIDER           (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_SYSTEM_DIVIDER         (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)>>SIM_CLKDIV1_OUTDIV1_SHIFT) + 1)
#define MCGOUT_TO_BUS_DIVIDER            (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_PERIPHERAL_DIVIDER     (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT) + 1)
#define MCGOUT_TO_FLEXBUS_DIVIDER        (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK)>>SIM_CLKDIV1_OUTDIV3_SHIFT) + 1)
#define MCGOUT_TO_FLASH_DIVIDER          (((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT) + 1)

 /**
 * @brief  获得系统各个总线时钟的频率
 * @code
 *         //获得总线时钟频率
 *         printf("BusClock:%dHz\r\n", GetClock(kBusClock));
 * @endcode
 * @param[in]  clockName 时钟名称
 *              @arg kCoreClock    内核时钟
 *              @arg kSystemClock  系统时钟 = 内核时钟
 *              @arg kBusClock     总线时钟
 *              @arg kFlexBusClock Flexbus总线时钟
 *              @arg kFlashClock   Flash总线时钟
 * @retval 0    成功
 * \retval 非0  错误
 */
uint32_t GetClock(Clock_t clockName)
{
    uint32_t clock = 0;
    /* calualte MCGOutClock system_MKxxx.c must not modified */
    SystemCoreClockUpdate();
    clock = SystemCoreClock * MCGOUT_TO_CORE_DIVIDER;
    switch (clockName)
    {
        case kCoreClock:
            clock = clock / MCGOUT_TO_CORE_DIVIDER;
            break;
        case kBusClock:
            clock = clock / MCGOUT_TO_BUS_DIVIDER;
            break;
        case kFlashClock:
            clock = clock / MCGOUT_TO_FLASH_DIVIDER;
            break;
        case kMCGOutClock:
            break;
        default:
            clock = 0;
    }
    return clock;
}

 /**
 * @brief  进入低功耗模式
 * @param[in]  enSleepOnExit 在系统唤醒时候 是否继续进入低功耗
 * @retval None
 * @note  任何中断 都可以唤醒CPU
 */
void EnterSTOPMode(bool enSleepOnExit)
{
    /* unlock all VLP mode */
    SMC->PMPROT |= 0xFF;
    
    /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    (enSleepOnExit)?(SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk):(SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk);
    
    /* WFI instruction will start entry into STOP mode */
    __ASM("WFI");
}

 /**
 * @brief  编码快速初始化结构 用户不需调用
 * @param[in]  type 快速初始化结构体指针
 * @retval     32位快速初始化编码
 */
uint32_t QuickInitEncode(map_t * type)
{
    return *(uint32_t*)type;
}

/**
 * @brief  获得芯片UID信息(全球唯一识别码)
 * @retval UID信息
 */
uint32_t GetUID(void) 
{
    uint32_t dummy;
    dummy = SIM->UIDL;
    dummy ^= SIM->UIDML;
    dummy ^= SIM->UIDMH;
    return dummy;
}


 /**
 * @brief  解码快速初始化结构 用户不需调用
 * @param[in]  map 32位快速初始化编码
 * @param[out]  type 快速初始化结构指针
 * @retval None
 */
void QuickInitDecode(uint32_t map, map_t * type)
{
    map_t * pMap = (map_t*)&(map);
    memcpy(type, pMap, sizeof(map_t));  
}

/**
 * \brief DWT delay function
 * \retval None
 */
void DWT_DelayInit(void)
{
    /* enable DEM */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    /* enable counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
 * @brief  DWT微秒级延时
 * @param[in]  us 延时微秒数
 * \note DWT(Data and Address Watchpoints)具有一个Core Clock的计数器，通过该计数器来实现us级延时
 * \retval  None
 */
void DWT_DelayUs(uint32_t us)
{
    uint32_t startts, endts, ts;
    startts = DWT->CYCCNT;
    ts =  us * (SystemCoreClock /(1000*1000) ); 
    endts = startts + ts;      
    if(endts > startts)  
    {
        while(DWT->CYCCNT < endts);       
    }
    else
    {
        while(DWT->CYCCNT > endts);
        while(DWT->CYCCNT < endts);
    }
}

/**
 * @brief  DWT毫秒级延时
 * @param[in]  ms 延时毫秒数
 * \retval  None
 */
void DWT_DelayMs(uint32_t ms)
{
    DWT_DelayUs(ms*1000);
}

/**
 * @brief  延时初始化函数
 * @code
 *   // 完成延时初始化配置，
 *   //使用内核的Systick模块实现延时功能
 *   DelayInit();
 * @endcode
 * @retval None
 */
#pragma weak DelayInit
void DelayInit(void)
{
    DWT_DelayInit();
}

/**
 * @brief 抽象毫秒级的延时设置函数
 * @code
 *   // 实现500ms的延时功能
 *   DelayMs(500);
 * @endcode
 * @param[in]  ms 需要延时的时间，单位毫秒
 * @retval None
 * @note  首先需要完成延时初始化配置
 */
#pragma weak DelayMs
void DelayMs(uint32_t ms)
{
    DWT_DelayMs(ms);
}

/**
 * @brief 抽象微秒级的延时设置函数
 * @code
 *   // 实现500us的延时功能
 *   DelayUs(500);
 * @endcode
 * @param[in]  us 需要延时的时间，单位微秒
 * @retval None
 * @note  首先需要完成延时初始化配置
 */
#pragma weak DelayUs
void DelayUs(uint32_t us)
{
    DWT_DelayUs(us);
}



#if (defined(LIB_DEBUG))

/**
 * \brief 断言检测
 * \param[in] file 指向字符串的指针
 * \param[in] line 当前错误处对应的行数
 */
void assert_failed(char * file, uint32_t line)
{
    LIB_TRACE("assert failed @ %s in %d\r\n", file, line);
	//断言失败检测
	while(1);
}
#endif

/**
 * @brief  非可屏蔽中断 non maskable interrupt
 */
void NMI_Handler(void)
{
    /* clear NMI pending bit */
    // MCM->ISR |= MCM_ISR_NMI_MASK;
    // printf("NMI ENTER\r\n");
}


#if (defined(LIB_DEBUG) && defined(DEBUG_FAULT_HANDLER))

/**
 * @brief  Hard Fault中断处理函数入口
 */
void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");
    ?__asm("BKPT #0x03");?
    while(1);
}

/**
 * @brief  Bus Fault中断处理函数入口
 */
void BusFault_Handler(void)
{
    printf("BusFault_Handler\r\n");
    ?__asm("BKPT #0x03");?
    while(1);
}

#endif

/***********************************************************************************************
功能：设置一个中断的优先级
形参：IRQn 中断号
		    PriorityGroup 优先级分组
		   @arg NVIC_PriorityGroup_0       0  PreemptPriority    16  SubPriority
		   @arg NVIC_PriorityGroup_1       2  PreemptPriority    8   SubPriority
		   @arg NVIC_PriorityGroup_2       4  PreemptPriority    4   SubPriority
		   @arg NVIC_PriorityGroup_3       8  PreemptPriority    2   SubPriority
		   @arg NVIC_PriorityGroup_4      16  PreemptPriority    0   SubPriority
		   PreemptPriority  抢占优先级
		   SubPriority      子优先级（响应优先级）
		   
返回：0
详解：如果两个中断的抢占优先级都一样的话，则看哪种中断先发生就先执行
      高优先级的抢占优先级是可以打断低优先级的抢占优先级中断的
      而抢占优先级相同的中断。高优先级的响应优先级不可以打断低优先级的响应优先级
************************************************************************************************/
void NVIC_Init(IRQn_Type IRQn,uint32_t PriorityGroup,uint32_t PreemptPriority,uint32_t SubPriority)
{
	//设置NVIC中断分组
	NVIC_SetPriorityGrouping(PriorityGroup);
	//配置中断优先级
	NVIC_SetPriority(IRQn,NVIC_EncodePriority(PriorityGroup,PreemptPriority,SubPriority));
}

/*
    for(i=0;i<ARRAY_SIZE(I2C_QuickInitTable);i++)
    {
        printf("(0X%08XU)\r\n", QuickInitEncode(&I2C_QuickInitTable[i]));
    }
*/


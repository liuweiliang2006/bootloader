
// 基本定时器TIMx,x[6,7]定时初始化函数
#include "hw_lib.h"
#include "stm32f10x_tim.h"

/// TIMx,x[6,7]中断优先级配置
void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = macTIM_IRQ;
	
		// 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	 
	  // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * TIM_Period / Auto Reload Register(ARR) = 1000   TIM_Prescaler--71 
 * 中断周期为 = 1/(72MHZ /72) * 1000 = 1ms
 *
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIM_Period(ARR) --> 中断 且TIMxCNT重置为0重新计数 
 */

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 *typedef struct
 *{  TIM_Prescaler         都有
 *	TIM_CounterMode			 TIMx,x[6,7]没有，其他都有
 *  TIM_Period            都有
 *  TIM_ClockDivision     TIMx,x[6,7]没有，其他都有
 *  TIM_RepetitionCounter TIMx,x[1,8,15,16,17]才有
 *}TIM_TimeBaseInitTypeDef; 
 *-----------------------------------------------------------------------------
 */


void TIMx_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		// 开启TIMx_CLK,x[6,7],即内部时钟CK_INT=72M
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM6, ENABLE);
	
		// 自动重装载寄存器周的值(计数值)
    TIM_TimeBaseStructure.TIM_Period=10000;
	
    // 累计 TIM_Period个频率后产生一个更新或者中断
	  // 时钟预分频数为71，则驱动计数器的时钟CK_CNT = CK_INT / (71+1)=1M
    TIM_TimeBaseStructure.TIM_Prescaler= 71;
	
		// 时钟分频因子 ，基本定时器TIM6和TIM7没有，不用管
    //TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		
		// 计数器计数模式，基本定时器TIM6和TIM7只能向上计数，没有计数模式的设置，不用管
    //TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		
		// 重复计数器的值，基本定时器TIM6和TIM7没有，不用管
		//TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	
	  // 初始化定时器TIMx, x[6,7]
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
		
		// 清除计数器中断标志位
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	  
		// 开启计数器中断
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
		
		// 使能计数器
//    TIM_Cmd(TIM6, ENABLE);																		
    
		// 暂时关闭TIMx,x[6,7]的时钟，等待使用
//    macTIM_APBxClock_FUN (macTIM_CLK, DISABLE);   
}
/*********************************************END OF FILE**********************/


#include "hw_lib.h"


void NVIC_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;	
    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
	/*TIM6中断配置*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/**/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

void Sys_Init()
{

//	GPIO_DeInit(GPIOA);
//	GPIO_DeInit(GPIOB);
	NVIC_Configuration();
	
	uart_init(115200);	
	delay_init();	   	 	//延时初始化 	
	LED_Init();		  		//初始化与LED连接的硬件接口
//  KEY_Init();				//按键初始化
	TIMx_Configuration(); 
	
	


	
}

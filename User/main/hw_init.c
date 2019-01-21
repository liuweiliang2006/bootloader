
#include "hw_lib.h"


void NVIC_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;	
    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
	/*TIM6�ж�����*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/**/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

void Sys_Init()
{

//	GPIO_DeInit(GPIOA);
//	GPIO_DeInit(GPIOB);
	NVIC_Configuration();
	
	uart_init(115200);	
	delay_init();	   	 	//��ʱ��ʼ�� 	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
//  KEY_Init();				//������ʼ��
	TIMx_Configuration(); 
	
	


	
}

#ifndef __KEY_H
#define __KEY_H	 
#include "hw_lib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

// 
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP

#define   KEYI_PA0

#ifdef    KEYI_PA0
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOA
#define               macKEY1_GPIO_PORT    	                GPIOA			   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_0

#define               macKEY2_GPIO_CLK                      RCC_APB2Periph_GPIOE
#define               macKEY2_GPIO_PORT    	                GPIOE			   
#define               macKEY2_GPIO_PIN		                  GPIO_Pin_3
#define               macKEY3_GPIO_PIN		                  GPIO_Pin_4

#else    //KEY2_PC13
#define               macKEY1_GPIO_CLK                      RCC_APB2Periph_GPIOC
#define               macKEY1_GPIO_PORT    	                GPIOC		   
#define               macKEY1_GPIO_PIN		                  GPIO_Pin_13

#endif

 
#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//��ȡ����2 
#define KEY3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����3(WK_UP) 


#define KEY_UP 		4
#define KEY_LEFT	3
#define KEY_DOWN	2
#define KEY_RIGHT	1

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��		
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
#endif

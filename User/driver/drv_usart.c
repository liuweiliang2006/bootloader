#include "hw_lib.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 
//int fputc(int ch, FILE *f)
//{
//	USART_SendData(USART1, (uint8_t) ch);

//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
//   
//    return ch;
//}
//int GetKey (void)  { 

//    while (!(USART1->SR & USART_FLAG_RXNE));

//    return ((int)(USART1->DR & 0x1FF));
//}

 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       	//����״̬���	  
u16 USART_RX_CNT=0;			//���յ��ֽ���	  
volatile static uint8_t ucRecCount=0;
volatile static uint16_t ucSlaveDataLen=0;
volatile static uint8_t ucRecFlag=0;
void UART5_IRQHandler(void)
{
//	uint8_t  ucTemp;	
	unsigned char rCh;
	static u16 rCnt = 0;
	
	if(USART_GetITStatus( UART5, USART_IT_RXNE ) != RESET)
	{	
		rCh = USART_ReceiveData( UART5 );
		ucReciveBuffer[rCnt] = rCh;
		
		if(rCnt == 0)     //֡ͷ0x5A    
	  {
			rCnt = (0x5A != rCh)?0:rCnt+1;
	  }
	  else if(rCnt == 1) //֡ͷ0xA5  
	  {
			rCnt = (0xA5 != rCh)?0:rCnt+1;
	  }
	  else if(rCnt == 2) //���ȸ��ֽ�
	  {
			ucSlaveDataLen=ucReciveBuffer[rCnt];
			rCnt++;
    }
	  else if(rCnt == 3) //���ȵ��ֽ�
	  {
			ucSlaveDataLen=(ucSlaveDataLen<<8)+ucReciveBuffer[rCnt];
			rCnt = (ucSlaveDataLen>0x0200)?0:rCnt+1;
//				#ifdef DEBUG
//					printf("ucSlaveDataLen=%d \r\n",ucSlaveDataLen);
////					printf("interrupt\r\n");
//				#endif
    }
		else if(rCnt == 4) //����
	  {
//			rCnt = (0xE4 != rCh)?0:rCnt+1;
			rCnt++;
    }	
		else if(rCnt == 5) //�����
	  {
			rCnt++;
    }	
		else if(rCnt == 6) //֡���
	  {
			rCnt++;
    }				
	  else if(rCnt > 6) //ֵvalue
	  {
			rCnt++;
			if(rCnt == Slave_Header_Length+((ucReciveBuffer[2]<<8)+ucReciveBuffer[3]))
			{             
				rCnt = 0;
				ucSalvePackLen=((ucReciveBuffer[2]<<8)+ucReciveBuffer[3])+Slave_Header_Length;
				if(ucReciveBuffer[4] == g_ucCmd)
				{
					g_ucSendFlag=1;
				}
//				if(ucReciveBuffer[4] == 0xE4)
//					g_ucFrameNumCur = ucReciveBuffer[6];
				ucSlaveDataLen=0;
			}				
	  }	
	 } 	 
	rCh=0;
	USART_ClearITPendingBit(UART5,USART_FLAG_RXNE);

} 
#endif	
void uart_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 
	
	//usart5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;										 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
     //USART1_TX   PA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ������
//  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�
  USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 
//	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
//	
//	USART_Init(USART2, &USART_InitStructure); //��ʼ������
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
//  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���
}


 void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART5 */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
 
 


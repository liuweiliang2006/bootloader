#include "hw_lib.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 
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

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       	//接收状态标记	  
u16 USART_RX_CNT=0;			//接收的字节数	  
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
		
		if(rCnt == 0)     //帧头0x5A    
	  {
			rCnt = (0x5A != rCh)?0:rCnt+1;
	  }
	  else if(rCnt == 1) //帧头0xA5  
	  {
			rCnt = (0xA5 != rCh)?0:rCnt+1;
	  }
	  else if(rCnt == 2) //长度高字节
	  {
			ucSlaveDataLen=ucReciveBuffer[rCnt];
			rCnt++;
    }
	  else if(rCnt == 3) //长度低字节
	  {
			ucSlaveDataLen=(ucSlaveDataLen<<8)+ucReciveBuffer[rCnt];
			rCnt = (ucSlaveDataLen>0x0200)?0:rCnt+1;
//				#ifdef DEBUG
//					printf("ucSlaveDataLen=%d \r\n",ucSlaveDataLen);
////					printf("interrupt\r\n");
//				#endif
    }
		else if(rCnt == 4) //类型
	  {
//			rCnt = (0xE4 != rCh)?0:rCnt+1;
			rCnt++;
    }	
		else if(rCnt == 5) //包序号
	  {
			rCnt++;
    }	
		else if(rCnt == 6) //帧序号
	  {
			rCnt++;
    }				
	  else if(rCnt > 6) //值value
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
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(UART5, &USART_InitStructure); //初始化串口
//  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
  USART_Cmd(UART5, ENABLE);                    //使能串口 
//	
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
  USART_Cmd(USART1, ENABLE);                    //使能串口 
//	
//	USART_Init(USART2, &USART_InitStructure); //初始化串口
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
//  USART_Cmd(USART2, ENABLE);                    //使能串口
}


 void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* 发送一个字节数据到USART5 */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
 
 


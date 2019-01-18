#include "hw_lib.h"



u32 time;
uint8_t ucReciveBuffer[530];
uint16_t ucSalvePackLen=0;
uint8_t g_ucBackUPApp[LENGTH]={0};

static u16 applenth=0;


//const u8 temp_flag[2048]  __attribute__ ((at(0x08024000))) = {0};
const u8 temp_flag  __attribute__ ((at(0x08024000))) = 0;
const u8 temp_flag1  __attribute__ ((at(0x08024004))) = 0;
u32 FlagAddr[3]={Restart_Flag,Updata_Flag,StartCopy_Flag};
int main(void)
{	
	
	u8 ucFlagVal[3]={0};
	u8 ucUpdataFlag=0,i;
	u16 j;
	u8 ucRestartFlag=0;
	u8 ucReadyRec[8]={0x5A,0xA5,0x00,0x00,0xF2,0x01,0x00,0x00};
	u8 ucUpdataSucess[9]={0x5A,0xA5,0x00,0x01,0xF3,0x01,0x00,0x01,0x00};
	u8 ucUpdataFail[9]={0x5A,0xA5,0x00,0x01,0xF3,0x00,0x00,0x01,0x00};

	Sys_Init();	
	printf("**bootloader**\r\n");
//	delay_ms(1000);
//	delay_ms(1000);
//	delay_ms(1000);
//	delay_ms(1000);

//	iap_load_app(APP_FLASHAddr);
//	Master_Send(ucReadyRec,8);
//	while(1);
//	printf("bootloader program!\r\n");
//	FLASH_Unlock();
//	FLASH_ErasePage(Symbol_FLASHAddr);
//	FLASH_ProgramHalfWord(FlagAddr[0],0x01);
//	FLASH_ProgramHalfWord(FlagAddr[1],0x00);
//	FLASH_ProgramHalfWord(FlagAddr[2],0x01);
//	FLASH_Lock();//上锁
	
	for(i=0;i<3;i++)
	{
		ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
	}
	
	ucRestartFlag=(*(vu16*)FlagAddr[0]);

	if((ucFlagVal[0] & 0x01) == UPDATE)
	{ //app程序将 升级标志位置1 表示有需要升级程序
		#ifdef DEBUG
				printf("**boot RestartFlag eneble**\r\n");
		#endif
		if((ucFlagVal[2] & 0x01)==0x01)
		{ //APP内的数据正确，可将之拷贝至备份区
			#ifdef DEBUG
				printf("**boot StartCopyFlag eneble**\r\n");
			#endif		
			//应用程序备份
			BackUP_APP();
					
			/*更新标志位*/
			FLASH_Unlock();
			for(i=0;i<3;i++)
			{
				ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
			}
			FLASH_ErasePage(Symbol_FLASHAddr);
			FLASH_ProgramHalfWord(FlagAddr[0],0x01); //升级标志位置1
			FLASH_ProgramHalfWord(FlagAddr[1],0x00); //升级标志位清零。1：成功，0：失败
			FLASH_ProgramHalfWord(FlagAddr[2],0x00); //拷贝标志为清0，该位最终由APP来置1
			FLASH_Lock();			
			delay_ms(10);
			/*发送允许发送指令*/
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
						
			#ifdef DEBUG
				printf("**send receive flag**\r\n");
			#endif
			
			Master_Send(ucReadyRec,8);
			while(1)
			{
				/*启动定时器*/
				TIM_Cmd(TIM6, ENABLE);
				/*到达150ms*/
				if(time != 1000)
				{
					/*串口接收到数据*/
					if((ucSalvePackLen!=0) && (ucReciveBuffer[4] == 0xE4))
					{
//							USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//关闭中断
//						#ifdef DEBUG
//							printf("**boot receive data**\r\n");
//						#endif
						/*关闭定时器*/
						TIM_Cmd(TIM6, DISABLE);
						time=0;
					
						/*校验成功，并将数据存放在的APP地址区，地址：0x08004000开始*/
						if(WriteAppData(applenth) == SUCCESS )
						{
							#ifdef DEBUG
								printf("**boot write data success**\r\n");
							#endif
							/*接收到数据并且写入成功*/
							/*累加包内数据的长度，作为下次写入的偏移*/
							applenth+=((ucReciveBuffer[2]<<8)+ucReciveBuffer[3]);
							ucSalvePackLen=0;
							/*判断是否为尾帧*/
							if(ucReciveBuffer[6] == 0)
							{
								#ifdef DEBUG
									printf("**boot last data**\r\n");
								#endif
								/*更新FLASH代码 */
//								delay_ms(200);
								/*擦除标志位存放区*/								
//								FLASH_ErasePage(Symbol_FLASHAddr);	
//								delay_ms(500);
								/*升级成功*/ 
								/*更新标志位*/
								FLASH_Unlock();
								FLASH_ErasePage(Symbol_FLASHAddr);
								FLASH_ProgramHalfWord(FlagAddr[0],0x00);
								FLASH_ProgramHalfWord(FlagAddr[1],0x01);
								FLASH_ProgramHalfWord(FlagAddr[2],0x00);
								FLASH_Lock();
								#ifdef DEBUG
									printf("**boot updata success**\r\n");
								#endif
								/*发送升级成功命令*/
								Master_Send(ucUpdataSucess,9);
								//执行FLASH APP代码
								#ifdef LOAD_APP
									iap_load_app(APP_FLASHAddr);
								#endif
								#ifdef DEBUG
									printf("**jump is fail**\r\n");
								#endif
								
							}
							/*当前包已经处理完毕，发送响应至从机，从机接收些命令可发送下一包数据*/
							memset(ucReciveBuffer,0,520);
							#ifdef DEBUG	
								printf("**has answer**\r\n");
							#endif
//							USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
							Master_Response_Slave(0x00,0xE4);
						}
						else
						{
							#ifdef DEBUG
								printf("**boot receive worng**\r\n");
							#endif
							/*接收到数据，但是写入过程失败*/
							/*恢复备份区程序*/
							Factory_Reset();
							delay_ms(1000);
							/*发送升级失败命令*/
							Master_Send(ucUpdataFail,9);
							/*升级失败*/ 
							#ifdef LOAD_APP
								iap_load_app(APP_FLASHAddr);
							#endif					
						}
					}
				}
				else
				{
					#ifdef DEBUG
						printf("**boot time out**\r\n");
					#endif
					
					
					for(j=0;j<520;j++)
					{
						printf("ucReciveBuffer[%d]=0x%x\r\n",j,ucReciveBuffer[j]);
					}
					
					Factory_Reset();
					/*升级失败*/ 			
					#ifdef LOAD_APP
						iap_load_app(APP_FLASHAddr);
					#endif					
				}
			}	
		}
		else
		{
			#ifdef DEBUG
				printf("**boot StartCopyFlag disabled**\r\n");
			#endif	
			/*恢复备份区程序*/
			Factory_Reset();
			/*StartCopy_Flag拷贝标志位没有被置1，表示上次APP代码更新没有完成，不将APP的代码进行备份*/
		
			/*发送升级失败命令*/
			Master_Send(ucUpdataFail,9);
			/*升级失败*/ 			
			#ifdef LOAD_APP
				iap_load_app(APP_FLASHAddr);
			#endif
		}
	}
	else
	{
		#ifdef DEBUG
			printf("**boot RestartFlag disabled**\r\n");
		#endif
		/*Restart_Flag标志位无效，非软件重启，直接跳转APP执行*/
		#ifdef LOAD_APP
			iap_load_app(APP_FLASHAddr);
		#endif		
//		iap_load_app(APP_FLASHAddr);//执行FLASH APP代码
	}	   
}














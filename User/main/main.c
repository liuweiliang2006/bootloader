#include "hw_lib.h"



u32 time;
uint8_t ucReciveBuffer[512+8];
uint16_t ucSalvePackLen=0;
uint8_t g_ucBackUPApp[LENGTH]={0};

static u16 applenth=0;


//const u8 temp_flag[2048]  __attribute__ ((at(0x08024000))) = {0};
const u8 temp_flag  __attribute__ ((at(0x08024000))) = 0;
u32 FlagAddr[3]={Restart_Flag,Updata_Flag,StartCopy_Flag};
int main(void)
{	
	
	u8 ucFlagVal[3]={0};
	u8 ucUpdataFlag=0,i;
	u8 ucRestartFlag=0;
	u8 ucReadyRec[8]={0x5A,0xA5,0x00,0x00,0xF1,0x01,0x00,0xF2};

	Sys_Init();	
	printf("bootloader program!\r\n");
	
	
	for(i=0;i<3;i++)
	{
		ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
	}
	

//	FLASH_Unlock();
//	FLASH_EraseAllPages();
//	FLASH_Lock();//上锁
	ucRestartFlag=(*(vu16*)FlagAddr[0]);

	if((ucFlagVal[0] & 0x01) == UPDATE)
	{
		if((ucFlagVal[2] & 0x01)==0x01)
		{
			/*更新标志位*/
			FLASH_Unlock();
			for(i=0;i<3;i++)
			{
				ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
			}
			FLASH_ErasePage(Symbol_FLASHAddr);
			FLASH_ProgramHalfWord(FlagAddr[0],ucFlagVal[0]);
			FLASH_ProgramHalfWord(FlagAddr[1],0x00);
			FLASH_ProgramHalfWord(FlagAddr[2],0x00);
			FLASH_Lock();
			
			//应用程序备份
			BackUP_APP();
			/*发送允许发送指令*/
//		Master_Send(ucReadyRec,8);
			delay_ms(10);
			while(1)
			{
				/*启动定时器*/
//				TIM_Cmd(TIM6, ENABLE);
				/*到达10ms*/
//			if(time != 10)
				{
					/*串口接收到数据*/
					if(ucSalvePackLen!=0) 
					{
						/*关闭定时器*/
//					TIM_Cmd(TIM6, DISABLE);
					
						/*校验成功，并将数据存放在的缓存，地址：0x08010000开始*/
						if(WriteAppData(applenth) == SUCCESS )
						{
							/*接收到数据并且写入成功*/
							/*累加包内数据的长度，作为下次写入的偏移*/
							applenth+=((ucReciveBuffer[2]<<8)+ucReciveBuffer[3]);
							ucSalvePackLen=0;
							/*判断是否为尾帧*/
							if(ucReciveBuffer[6] == 0)
							{
								/*更新FLASH代码 */
								delay_ms(200);
								/*擦除标志位存放区*/								
//								FLASH_ErasePage(Symbol_FLASHAddr);	
								delay_ms(500);
								/*升级成功*/ 
								/*更新标志位*/
								FLASH_Unlock();
								FLASH_ErasePage(Symbol_FLASHAddr);
								FLASH_ProgramHalfWord(FlagAddr[0],0x00);
								FLASH_ProgramHalfWord(FlagAddr[1],0x01);
								FLASH_ProgramHalfWord(FlagAddr[2],0x01);
								FLASH_Lock();
								//执行FLASH APP代码
								iap_load_app(APP_FLASHAddr);								
							}
							/*当前包已经处理完毕，发送响应住处至从机，从机接收些命令可发送下一包数据*/
							Master_Response_Slave(0x00,0xF4);						
						}
						else
						{
							/*接收到数据，但是写入过程失败*/
							/*恢复备份区程序*/
							Factory_Reset();
							FLASH_Unlock();
							FLASH_ErasePage(Symbol_FLASHAddr);
							FLASH_ProgramHalfWord(FlagAddr[0],0x00);
							FLASH_ProgramHalfWord(FlagAddr[1],0x00);
							FLASH_ProgramHalfWord(FlagAddr[2],0x01);
							FLASH_Lock();
							delay_ms(1000);
							/*发送升级失败命令*/
//						Master_Send(ucReadyRec,8);
							/*升级失败*/ 
							iap_load_app(APP_FLASHAddr);						
						}
					}
				}
//				else
//				{
//					Factory_Reset();
//					FLASH_Unlock();
//					FLASH_ErasePage(Symbol_FLASHAddr);
//					FLASH_ProgramHalfWord(FlagAddr[0],0x00);
//					FLASH_ProgramHalfWord(FlagAddr[1],0x00);
//					FLASH_ProgramHalfWord(FlagAddr[2],0x01);
//					FLASH_Lock();
//					delay_ms(1000);
//					iap_load_app(APP_FLASHAddr);	
//				}
			}	
		}
		else
		{
			/*StartCopy_Flag拷贝标志位没有被置1，表示上次APP代码更新没有完成，不将APP的代码进行备份*/
			FLASH_Unlock();
			FLASH_ErasePage(Symbol_FLASHAddr);
			FLASH_ProgramHalfWord(FlagAddr[0],0x00);
			FLASH_ProgramHalfWord(FlagAddr[1],0x01);
			FLASH_ProgramHalfWord(FlagAddr[2],0x00);
			FLASH_Lock();
			/*恢复备份区程序*/
			Factory_Reset();
			/*升级失败*/ 			
			iap_load_app(APP_FLASHAddr);//执行FLASH APP代码
		}
	}
	else
	{
		/*Restart_Flag标志位无效，非软件重启，直接跳转APP执行*/	
		iap_load_app(APP_FLASHAddr);//执行FLASH APP代码
	}	   
}














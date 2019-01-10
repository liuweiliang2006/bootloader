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
//	FLASH_Lock();//����
	ucRestartFlag=(*(vu16*)FlagAddr[0]);

	if((ucFlagVal[0] & 0x01) == UPDATE)
	{
		if((ucFlagVal[2] & 0x01)==0x01)
		{
			/*���±�־λ*/
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
			
			//Ӧ�ó��򱸷�
			BackUP_APP();
			/*����������ָ��*/
//		Master_Send(ucReadyRec,8);
			delay_ms(10);
			while(1)
			{
				/*������ʱ��*/
//				TIM_Cmd(TIM6, ENABLE);
				/*����10ms*/
//			if(time != 10)
				{
					/*���ڽ��յ�����*/
					if(ucSalvePackLen!=0) 
					{
						/*�رն�ʱ��*/
//					TIM_Cmd(TIM6, DISABLE);
					
						/*У��ɹ����������ݴ���ڵĻ��棬��ַ��0x08010000��ʼ*/
						if(WriteAppData(applenth) == SUCCESS )
						{
							/*���յ����ݲ���д��ɹ�*/
							/*�ۼӰ������ݵĳ��ȣ���Ϊ�´�д���ƫ��*/
							applenth+=((ucReciveBuffer[2]<<8)+ucReciveBuffer[3]);
							ucSalvePackLen=0;
							/*�ж��Ƿ�Ϊβ֡*/
							if(ucReciveBuffer[6] == 0)
							{
								/*����FLASH���� */
								delay_ms(200);
								/*������־λ�����*/								
//								FLASH_ErasePage(Symbol_FLASHAddr);	
								delay_ms(500);
								/*�����ɹ�*/ 
								/*���±�־λ*/
								FLASH_Unlock();
								FLASH_ErasePage(Symbol_FLASHAddr);
								FLASH_ProgramHalfWord(FlagAddr[0],0x00);
								FLASH_ProgramHalfWord(FlagAddr[1],0x01);
								FLASH_ProgramHalfWord(FlagAddr[2],0x01);
								FLASH_Lock();
								//ִ��FLASH APP����
								iap_load_app(APP_FLASHAddr);								
							}
							/*��ǰ���Ѿ�������ϣ�������Ӧס�����ӻ����ӻ�����Щ����ɷ�����һ������*/
							Master_Response_Slave(0x00,0xF4);						
						}
						else
						{
							/*���յ����ݣ�����д�����ʧ��*/
							/*�ָ�����������*/
							Factory_Reset();
							FLASH_Unlock();
							FLASH_ErasePage(Symbol_FLASHAddr);
							FLASH_ProgramHalfWord(FlagAddr[0],0x00);
							FLASH_ProgramHalfWord(FlagAddr[1],0x00);
							FLASH_ProgramHalfWord(FlagAddr[2],0x01);
							FLASH_Lock();
							delay_ms(1000);
							/*��������ʧ������*/
//						Master_Send(ucReadyRec,8);
							/*����ʧ��*/ 
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
			/*StartCopy_Flag������־λû�б���1����ʾ�ϴ�APP�������û����ɣ�����APP�Ĵ�����б���*/
			FLASH_Unlock();
			FLASH_ErasePage(Symbol_FLASHAddr);
			FLASH_ProgramHalfWord(FlagAddr[0],0x00);
			FLASH_ProgramHalfWord(FlagAddr[1],0x01);
			FLASH_ProgramHalfWord(FlagAddr[2],0x00);
			FLASH_Lock();
			/*�ָ�����������*/
			Factory_Reset();
			/*����ʧ��*/ 			
			iap_load_app(APP_FLASHAddr);//ִ��FLASH APP����
		}
	}
	else
	{
		/*Restart_Flag��־λ��Ч�������������ֱ����תAPPִ��*/	
		iap_load_app(APP_FLASHAddr);//ִ��FLASH APP����
	}	   
}














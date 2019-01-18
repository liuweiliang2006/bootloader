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
//	FLASH_Lock();//����
	
	for(i=0;i<3;i++)
	{
		ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
	}
	
	ucRestartFlag=(*(vu16*)FlagAddr[0]);

	if((ucFlagVal[0] & 0x01) == UPDATE)
	{ //app���� ������־λ��1 ��ʾ����Ҫ��������
		#ifdef DEBUG
				printf("**boot RestartFlag eneble**\r\n");
		#endif
		if((ucFlagVal[2] & 0x01)==0x01)
		{ //APP�ڵ�������ȷ���ɽ�֮������������
			#ifdef DEBUG
				printf("**boot StartCopyFlag eneble**\r\n");
			#endif		
			//Ӧ�ó��򱸷�
			BackUP_APP();
					
			/*���±�־λ*/
			FLASH_Unlock();
			for(i=0;i<3;i++)
			{
				ucFlagVal[i]=(*(vu8*)FlagAddr[i]);
			}
			FLASH_ErasePage(Symbol_FLASHAddr);
			FLASH_ProgramHalfWord(FlagAddr[0],0x01); //������־λ��1
			FLASH_ProgramHalfWord(FlagAddr[1],0x00); //������־λ���㡣1���ɹ���0��ʧ��
			FLASH_ProgramHalfWord(FlagAddr[2],0x00); //������־Ϊ��0����λ������APP����1
			FLASH_Lock();			
			delay_ms(10);
			/*����������ָ��*/
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�
						
			#ifdef DEBUG
				printf("**send receive flag**\r\n");
			#endif
			
			Master_Send(ucReadyRec,8);
			while(1)
			{
				/*������ʱ��*/
				TIM_Cmd(TIM6, ENABLE);
				/*����150ms*/
				if(time != 1000)
				{
					/*���ڽ��յ�����*/
					if((ucSalvePackLen!=0) && (ucReciveBuffer[4] == 0xE4))
					{
//							USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//�ر��ж�
//						#ifdef DEBUG
//							printf("**boot receive data**\r\n");
//						#endif
						/*�رն�ʱ��*/
						TIM_Cmd(TIM6, DISABLE);
						time=0;
					
						/*У��ɹ����������ݴ���ڵ�APP��ַ������ַ��0x08004000��ʼ*/
						if(WriteAppData(applenth) == SUCCESS )
						{
							#ifdef DEBUG
								printf("**boot write data success**\r\n");
							#endif
							/*���յ����ݲ���д��ɹ�*/
							/*�ۼӰ������ݵĳ��ȣ���Ϊ�´�д���ƫ��*/
							applenth+=((ucReciveBuffer[2]<<8)+ucReciveBuffer[3]);
							ucSalvePackLen=0;
							/*�ж��Ƿ�Ϊβ֡*/
							if(ucReciveBuffer[6] == 0)
							{
								#ifdef DEBUG
									printf("**boot last data**\r\n");
								#endif
								/*����FLASH���� */
//								delay_ms(200);
								/*������־λ�����*/								
//								FLASH_ErasePage(Symbol_FLASHAddr);	
//								delay_ms(500);
								/*�����ɹ�*/ 
								/*���±�־λ*/
								FLASH_Unlock();
								FLASH_ErasePage(Symbol_FLASHAddr);
								FLASH_ProgramHalfWord(FlagAddr[0],0x00);
								FLASH_ProgramHalfWord(FlagAddr[1],0x01);
								FLASH_ProgramHalfWord(FlagAddr[2],0x00);
								FLASH_Lock();
								#ifdef DEBUG
									printf("**boot updata success**\r\n");
								#endif
								/*���������ɹ�����*/
								Master_Send(ucUpdataSucess,9);
								//ִ��FLASH APP����
								#ifdef LOAD_APP
									iap_load_app(APP_FLASHAddr);
								#endif
								#ifdef DEBUG
									printf("**jump is fail**\r\n");
								#endif
								
							}
							/*��ǰ���Ѿ�������ϣ�������Ӧ���ӻ����ӻ�����Щ����ɷ�����һ������*/
							memset(ucReciveBuffer,0,520);
							#ifdef DEBUG	
								printf("**has answer**\r\n");
							#endif
//							USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�
							Master_Response_Slave(0x00,0xE4);
						}
						else
						{
							#ifdef DEBUG
								printf("**boot receive worng**\r\n");
							#endif
							/*���յ����ݣ�����д�����ʧ��*/
							/*�ָ�����������*/
							Factory_Reset();
							delay_ms(1000);
							/*��������ʧ������*/
							Master_Send(ucUpdataFail,9);
							/*����ʧ��*/ 
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
					/*����ʧ��*/ 			
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
			/*�ָ�����������*/
			Factory_Reset();
			/*StartCopy_Flag������־λû�б���1����ʾ�ϴ�APP�������û����ɣ�����APP�Ĵ�����б���*/
		
			/*��������ʧ������*/
			Master_Send(ucUpdataFail,9);
			/*����ʧ��*/ 			
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
		/*Restart_Flag��־λ��Ч�������������ֱ����תAPPִ��*/
		#ifdef LOAD_APP
			iap_load_app(APP_FLASHAddr);
		#endif		
//		iap_load_app(APP_FLASHAddr);//ִ��FLASH APP����
	}	   
}














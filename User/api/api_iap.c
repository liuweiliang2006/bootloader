#include "hw_lib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IAP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////	

iapfun jump2app; 
u16 iapbuf[1024];   
//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
ErrorStatus iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	ErrorStatus status = SUCCESS;
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			status=STMFLASH_Write(fwaddr,iapbuf,1024);	
			if(status != SUCCESS)
			{
				return ERROR;
			}
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)
	{
		status=STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.
		if(status != SUCCESS)
		{
			return ERROR;
		}		
	}
	return SUCCESS;	
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}		

void Cache_Erase(u32 StartAddr,u32 EndAddr)
{
	u8 i;
	FLASH_Unlock();	
	for(i=0;i<(EndAddr-StartAddr)/2048;i++)
	{
		FLASH_ErasePage(i*2048+StartAddr);
	}
	FLASH_Lock();//����
}

#define PERWRITE  10
#define WRITEBYTE (PERWRITE*1024)
void BackUP_APP()
{
	uint16_t i,j;
	uint8_t uc_APPCopyCount;
	uint8_t uc_Div; //��10����������д���ٴ�10K��������
	uint8_t uc_Rem; //ʣ��ķ�10����
	
	uc_Div=APP_FLASH_MAX/PERWRITE;
	uc_Rem=APP_FLASH_MAX%PERWRITE;
	
	FLASH_Unlock();
	Cache_Erase(Cache_FLASHAddr,Cache_FLASH_End+1);
	FLASH_Lock();
	//��10K��С��Ӧ�ó����ȡ��Ȼ��д�뱸����	
	for(j=0;j<uc_Div;j++)
	{
		for(i=0;i<LENGTH;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+APP_FLASHAddr+i));
		}
		iap_write_appbin(Cache_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,LENGTH);
		memset(g_ucBackUPApp,0,LENGTH);
	}
	
	//д��ʣ�ಿ��	
	if(uc_Rem!=0)
	{
		for(i=0;i<uc_Rem*1024;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+APP_FLASHAddr+i));
		}
		iap_write_appbin(Cache_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,uc_Rem*1024);
		memset(g_ucBackUPApp,0,LENGTH);
	}	
}

/*�ָ�ԭ����*/
void Factory_Reset(void)
{
	uint16_t i,j;
	uint8_t uc_APPCopyCount;
	uint8_t uc_Div; //��10����������д���ٴ�10K��������
	uint8_t uc_Rem; //ʣ��ķ�10����
	
	/*�رն�ʱ��*/
	TIM_Cmd(TIM6, DISABLE);
	
	uc_Div=APP_FLASH_MAX/PERWRITE;
	uc_Rem=APP_FLASH_MAX%PERWRITE;
	
	Cache_Erase(APP_FLASHAddr,APP_FLASH_End+1);
	//��10K��С��Ӧ�ó����ȡ��Ȼ��д�뱸����	
	for(j=0;j<uc_Div;j++)
	{
		for(i=0;i<LENGTH;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+Cache_FLASHAddr+i));
		}
		iap_write_appbin(APP_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,LENGTH);
		memset(g_ucBackUPApp,0,LENGTH);
	}
	
	//д��ʣ�ಿ��	
	if(uc_Rem!=0)
	{
		for(i=0;i<uc_Rem*1024;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+Cache_FLASHAddr+i));
		}
		iap_write_appbin(APP_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,uc_Rem*1024);
		memset(g_ucBackUPApp,0,LENGTH);
	}
}












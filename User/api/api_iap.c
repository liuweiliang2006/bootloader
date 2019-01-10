#include "hw_lib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IAP 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////	

iapfun jump2app; 
u16 iapbuf[1024];   
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
ErrorStatus iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	ErrorStatus status = SUCCESS;
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			status=STMFLASH_Write(fwaddr,iapbuf,1024);	
			if(status != SUCCESS)
			{
				return ERROR;
			}
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)
	{
		status=STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.
		if(status != SUCCESS)
		{
			return ERROR;
		}		
	}
	return SUCCESS;	
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
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
	FLASH_Lock();//上锁
}

#define PERWRITE  10
#define WRITEBYTE (PERWRITE*1024)
void BackUP_APP()
{
	uint16_t i,j;
	uint8_t uc_APPCopyCount;
	uint8_t uc_Div; //除10倍数，可以写多少次10K，整数倍
	uint8_t uc_Rem; //剩余的非10倍数
	
	uc_Div=APP_FLASH_MAX/PERWRITE;
	uc_Rem=APP_FLASH_MAX%PERWRITE;
	
	FLASH_Unlock();
	Cache_Erase(Cache_FLASHAddr,Cache_FLASH_End+1);
	FLASH_Lock();
	//以10K大小从应用程序读取，然后写入备份区	
	for(j=0;j<uc_Div;j++)
	{
		for(i=0;i<LENGTH;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+APP_FLASHAddr+i));
		}
		iap_write_appbin(Cache_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,LENGTH);
		memset(g_ucBackUPApp,0,LENGTH);
	}
	
	//写入剩余部分	
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

/*恢复原程序*/
void Factory_Reset(void)
{
	uint16_t i,j;
	uint8_t uc_APPCopyCount;
	uint8_t uc_Div; //除10倍数，可以写多少次10K，整数倍
	uint8_t uc_Rem; //剩余的非10倍数
	
	/*关闭定时器*/
	TIM_Cmd(TIM6, DISABLE);
	
	uc_Div=APP_FLASH_MAX/PERWRITE;
	uc_Rem=APP_FLASH_MAX%PERWRITE;
	
	Cache_Erase(APP_FLASHAddr,APP_FLASH_End+1);
	//以10K大小从应用程序读取，然后写入备份区	
	for(j=0;j<uc_Div;j++)
	{
		for(i=0;i<LENGTH;i++)
		{
			g_ucBackUPApp[i]=(*(vu8*)(WRITEBYTE*j+Cache_FLASHAddr+i));
		}
		iap_write_appbin(APP_FLASHAddr+WRITEBYTE*j,g_ucBackUPApp,LENGTH);
		memset(g_ucBackUPApp,0,LENGTH);
	}
	
	//写入剩余部分	
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












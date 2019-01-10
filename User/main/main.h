


#ifndef _MAIN_H
#define _MAIN_H
#include "hw_lib.h"

/*Bootloader 16K*/
#define Bootloader_FLASHAddr 			((u32)0x08000000)	//bootloader起始地址
#define Bootloader_FLASH_End			((u32)0x08003fff)	//bootloader结束地址
/*app 64K*/	
#define APP_FLASHAddr							((u32)0x08004000)	//APP起始地址
#define APP_FLASH_End							((u32)0x08013fff)	//APP结束地址

#define APP_FLASH_MAX  (64)

/*缓存备份区 64K*/	
#define Cache_FLASHAddr						((u32)0x08014000)	//备份区起始地址
#define Cache_FLASH_End						((u32)0x08023fff)	//备份区结束地址
/*标志位存放区 2K*/	
#define Symbol_FLASHAddr					((u32)0x08024000)	//标志位存放区起始地址
#define Symbol_FLASH_End					((u32)0x080247ff)	//标志位存放区结束地址
#define Restart_Flag							((u32)0x08024000)	//重启标志位，该标志位bootloader与APP共用
#define Updata_Flag								((u32)0x08024004)	//升级是否成功标志位，该标志位bootloader与APP共用
#define StartCopy_Flag								((u32)0x08024006)	//允许拷贝标志

	
/*预留区366K*/	
#define Resever_FLASHAddr					((u32)0x08024800)	//预留起始地址
#define Resever_FLASH_End					((u32)0x0807ffff)	//预留结束地址

#define LENGTH (10*1024)

typedef enum {UPDATE = 1, NOT_UPDATE = !UPDATE} RestartFlag; //flash只能由1变成0

extern uint8_t ucReciveBuffer[512+8];
extern uint16_t ucSalvePackLen;
extern uint8_t g_ucBackUPApp[LENGTH];
extern u32 FlagAddr[3];
#endif




#ifndef _MAIN_H
#define _MAIN_H
#include "hw_lib.h"

//���Խӿ�
#define LOAD_APP	1
//#define DEBUG	1

/*Bootloader 16K*/
#define Bootloader_FLASHAddr 			((u32)0x08000000)	//bootloader��ʼ��ַ
#define Bootloader_FLASH_End			((u32)0x08003fff)	//bootloader������ַ
/*app 64K*/	
#define APP_FLASHAddr							((u32)0x08004000)	//APP��ʼ��ַ
#define APP_FLASH_End							((u32)0x08013fff)	//APP������ַ

#define APP_FLASH_MAX  (64)

/*���汸���� 64K*/	
#define Cache_FLASHAddr						((u32)0x08014000)	//��������ʼ��ַ
#define Cache_FLASH_End						((u32)0x08023fff)	//������������ַ
/*��־λ����� 2K*/	
#define Symbol_FLASHAddr					((u32)0x08024000)	//��־λ�������ʼ��ַ
#define Symbol_FLASH_End					((u32)0x080247ff)	//��־λ�����������ַ
#define Restart_Flag							((u32)0x08024000)	//������־λ���ñ�־λbootloader��APP����
#define Updata_Flag								((u32)0x08024002)	//�����Ƿ�ɹ���־λ���ñ�־λbootloader��APP����
#define StartCopy_Flag						((u32)0x08024004)	//��������־

	
/*Ԥ����366K*/	
#define Resever_FLASHAddr					((u32)0x08024800)	//Ԥ����ʼ��ַ
#define Resever_FLASH_End					((u32)0x0807ffff)	//Ԥ��������ַ

#define LENGTH (10*1024)

typedef enum {UPDATE = 1, NOT_UPDATE = !UPDATE} RestartFlag; //flashֻ����1���0

extern uint8_t ucReciveBuffer[520];
extern uint16_t ucSalvePackLen;
extern uint8_t g_ucBackUPApp[LENGTH];
extern u32 FlagAddr[3];

extern u8 g_ucSendFlag;
extern u8 g_ucCmd;
extern u16 g_ucFrameNumPre,g_ucFrameNumCur;
#endif

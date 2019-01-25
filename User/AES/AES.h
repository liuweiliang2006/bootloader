#include "hw_lib.h"

#ifndef __AES_H_
#define __AES_H_


#define     AES_ENABLE      1
#define     AES_DISABLR     0

extern unsigned char AES128key[16];//√ÿ‘ø
extern void AES_Init(const void *pKey);
extern unsigned char Use_AES;
void AES_UART5_SendData(unsigned char *src,uint16_t len);
unsigned char AES_UART_DECODE(void);


#endif
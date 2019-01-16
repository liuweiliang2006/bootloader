#ifndef _API_USART_H
#define _API_USART_H

#include "hw_lib.h"


void Master_Response_Slave(uint8_t datalength, uint8_t cmd);
void Master_Send(uint8_t *data ,uint8_t len );
u8 WriteAppData(u16);

#endif

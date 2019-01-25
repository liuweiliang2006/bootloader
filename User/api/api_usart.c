#include "hw_lib.h"


static uint8_t ucPackNum=1;
extern unsigned char AES_IV[16];
//���ݰ���ţ�ÿ����һ����ɣ��Զ���1��ȡֵ��Χ 1-255
void Pack_Num_Count()
{
	if(ucPackNum<255)
		ucPackNum++;
	else
		ucPackNum=1;
}

void Master_Send(uint8_t *data ,uint8_t len )
{
	uint8_t i,ucSendData[10],sum=0;;
	for(i=0;i<len-1;i++)
	{
		if(i!=5)
		{
			ucSendData[i] = *(data+i);
		}else
		{
			ucSendData[i] = ucPackNum;
		}
		if(i>=2)
			sum+=ucSendData[i];
	}	
	ucSendData[len-1]=sum;
	for(i=0;i<len;i++)
	{
		Usart_SendByte(UART5,ucSendData[i]);
	}
	Pack_Num_Count();
}



uint8_t ucResponse[26];
//��CRC��ǰ�İ�ͷ��ʽ
//��ͷ1����ͷ2�����ȸߡ����ȵ͡��������������š�֡���
static uint8_t ucSlaveHeader[Slave_Header_Length-1]={0x5A,\
																										0xA5,\
																										0,\
																										0,\
																										0,\
																										0,\
																										0}; 


//����˵����datalength ���Ͱ������ݳ��ȣ�cmd �������
void Master_Response_Slave(uint8_t datalength, uint8_t cmd)
{
	uint8_t i;
	uint8_t ucCrcCheckSum=0;
	uint8_t ucSetThresholdCount=0;
	
	__disable_irq();
/******************�������������ʼ***********************/
	for(i=0;i<Slave_Header_Length-1;i++)
	{
		if(i==0x02)
			ucResponse[i]=0x00;
		else if(i==0x03)
			ucResponse[i]=datalength;
		else if(i==0x04)
			ucResponse[i]=cmd;
		else if(i==0x05)
			ucResponse[i]=ucPackNum;
		else if(i==0x06)
			ucResponse[i]=ucReciveBuffer[6];
		else
			ucResponse[i]=ucSlaveHeader[i];	
		
		if(i>1)
			(uint8_t) (ucCrcCheckSum+=ucResponse[i]);
		
	}
		ucResponse[i]=ucCrcCheckSum;
/******************���������������**********************/	
	
/******************��������**********************/	
	for(i=0;i<8;i++)
	{
		Usart_SendByte(UART5,ucResponse[i]);
	}	
	Pack_Num_Count();
	__enable_irq();
}

/*salve recive buff data Analysis */

//u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0x08050000)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X08050000.    
u8 WriteAppData(u16 pos)
{
	ErrorStatus status = SUCCESS;
	uint16_t i,j;
	uint8_t ucCopyBuf[256],ucAESBuf[512];
	uint16_t ucSlaveCrcCheckSum=0;
	uint16_t DataLen;
	
	DataLen=(ucReciveBuffer[2]<<8)+ucReciveBuffer[3];
	
	 /*������հ���CRCУ�� except slave data CRC*/
	for(i=2;i<ucSalvePackLen-1;i++)
	{		
		(uint8_t) (ucSlaveCrcCheckSum+=ucReciveBuffer[i]);
	}
	/*�жϸ��ݽ��յ������ݵó���CRC��ӻ����͵�CRCУ���Ƿ�һ��*/
	if((ucSlaveCrcCheckSum&0xff)==ucReciveBuffer[ucSalvePackLen-1])  
	{
		/*�жϳ����Ƿ�Ϊ16�����������������ݳ��ȱ���ΪAES_KEY_LENGTH/8��������*/
		if(DataLen%16 == 0)
		{	
			if(g_ucFrameNumCur != 0)
			{
				for(j=0;j<DataLen/256;j++)
				{
					memcpy(ucCopyBuf,&ucReciveBuffer[7+j*256],256);
					/*            ����               ����    ���� ��ʼ������*/
					AES_Decrypt(&ucAESBuf[j*256], ucCopyBuf, 256, AES_IV);     //����
				}				
			}
			else
			{
				for(j=0;j<DataLen/256;j++)
				{
					memcpy(ucCopyBuf,&ucReciveBuffer[7+j*256],256);
					/*            ����             ����      ����   ��ʼ������*/
					AES_Decrypt(&ucAESBuf[j*256], ucCopyBuf, 256, AES_IV);     //����
				}
				memset(ucCopyBuf,0,256);
				memcpy(ucCopyBuf,&ucReciveBuffer[7+j*256],DataLen%256);
				/*            ����               ����      ����   ��ʼ������*/
				AES_Decrypt(&ucAESBuf[j*256], ucCopyBuf, DataLen%256, AES_IV);     //����
			}
//			
//			for(j=0;j<DataLen;j++)
//			{
//				#ifdef DEBUG
//					printf("0x%02x ",ucAESBuf[j]);
//				#endif
//			}
			
			
			/*У�����󣬽����յ�����������APP��	*/
			status =iap_write_appbin((u32)APP_FLASHAddr+pos,ucAESBuf,DataLen);	
			memset(ucAESBuf,0,520);
			if(status != SUCCESS)
			{
			return ERROR;
			}
		}
		else
		{
			return ERROR;
		}
		/*У�����󣬽����յ�����������APP��	*/
//		status =iap_write_appbin((u32)APP_FLASHAddr+pos,&ucReciveBuffer[7],DataLen);	
//		if(status != SUCCESS)
//		{
//			return ERROR;
//		}
//		#ifdef DEBUG
//			printf("CRC is right!\r\n");
//		#endif
		
	}
	else
	{
//		#ifdef DEBUG
//			printf("CRC is wrong!\r\n");
//		#endif
		return ERROR;
	}
		
	
	return SUCCESS;

}

//#include "Includes.h"
#include "AES.h"
#include "..\AES\track_fun_AES.h"
#include "hw_lib.h"
/*----------------usersetting------------*/

/*----------------end usersetting------------*/
unsigned char Use_AES = AES_ENABLE;

unsigned char AES128key[16] = "123456789abcdefa";//��Կ
unsigned char AES_IV[16]= "0102030405123456";//������
//unsigned char AES128expKey[4 * Nc * (Nr + 1)];  //��ʼ����Կ
//void AES_UART5_SendData(unsigned char *src,uint16_t len)
//{
//    //unsigned char AES_Len;
//    unsigned char buf[256];
//    //unsigned char out[16];//�������
//    unsigned char in[256] = {0};
//    uint16_t i,j;
//    if(Use_AES == AES_ENABLE)
//    {
//        AES_Init(AES128key);//AES��ʼ��
//			for(i=0;i<LENGTH1/256;i++)
//			{
//				memcpy(in,&APP1[i*256],256);
//				/*         ����  ����    ����    ����*/
//        AES_Encrypt(in, buf, 256, AES_IV);//����
//				
//				for(j=0;j<256;j++)
//				{
//					Usart_SendByte(UART5,buf[j]);
//				}
//			}
//			
//			memcpy(in,&APP1[LENGTH1/256],LENGTH1%256);
//			/*         ����  ����    ����    ����*/
//      AES_Encrypt(in, buf, LENGTH1%256, AES_IV);//����
//			for(j=0;j<LENGTH1%256;j++)
//			{
//				Usart_SendByte(UART5,buf[j]);
//			}
//    }
//    else 
//    {
//        //UART1_SendData(src, len);
//    }
//}
// ���ڽ���
//�������ݹ���
unsigned char AES_UART_DECODE(void)
{
    unsigned char len=0,cUart1RxData[512];
    if(cUart1RxData[0] == 0xBA)
    {
        if((cUart1RxData[1]%16)==0)
        {
            len = cUart1RxData[1];
					/*                ����            ����         ����  ��ʼ������*/
            AES_Decrypt(cUart1RxData+2, cUart1RxData+2, len, AES_IV);     //����
            
            cUart1RxData[0] = 0xBB;//ת��֡ͷ
            cUart1RxData[1] = cUart1RxData[3] + 2;// �����㳤��
//            cUart1RxData[cUart1RxData[1]+ 2] = HCGetCheckSum(cUart1RxData, cUart1RxData[1]+ 2);//������У��
            return 1;//h����
        }
        else 
        {
            return 2;//������Ϣ���ȴ���
        }
    }
    else 
    {
        return 0;//û����
    }
}
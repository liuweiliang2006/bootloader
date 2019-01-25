//#include "Includes.h"
#include "AES.h"
#include "..\AES\track_fun_AES.h"
#include "hw_lib.h"
/*----------------usersetting------------*/

/*----------------end usersetting------------*/
unsigned char Use_AES = AES_ENABLE;

unsigned char AES128key[16] = "123456789abcdefa";//秘钥
unsigned char AES_IV[16]= "0102030405123456";//向量表
//unsigned char AES128expKey[4 * Nc * (Nr + 1)];  //初始化密钥
//void AES_UART5_SendData(unsigned char *src,uint16_t len)
//{
//    //unsigned char AES_Len;
//    unsigned char buf[256];
//    //unsigned char out[16];//密文输出
//    unsigned char in[256] = {0};
//    uint16_t i,j;
//    if(Use_AES == AES_ENABLE)
//    {
//        AES_Init(AES128key);//AES初始化
//			for(i=0;i<LENGTH1/256;i++)
//			{
//				memcpy(in,&APP1[i*256],256);
//				/*         明文  密文    长度    向量*/
//        AES_Encrypt(in, buf, 256, AES_IV);//加密
//				
//				for(j=0;j<256;j++)
//				{
//					Usart_SendByte(UART5,buf[j]);
//				}
//			}
//			
//			memcpy(in,&APP1[LENGTH1/256],LENGTH1%256);
//			/*         明文  密文    长度    向量*/
//      AES_Encrypt(in, buf, LENGTH1%256, AES_IV);//加密
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
// 串口解密
//串口数据过滤
unsigned char AES_UART_DECODE(void)
{
    unsigned char len=0,cUart1RxData[512];
    if(cUart1RxData[0] == 0xBA)
    {
        if((cUart1RxData[1]%16)==0)
        {
            len = cUart1RxData[1];
					/*                明文            密文         长度  初始化向量*/
            AES_Decrypt(cUart1RxData+2, cUart1RxData+2, len, AES_IV);     //解密
            
            cUart1RxData[0] = 0xBB;//转换帧头
            cUart1RxData[1] = cUart1RxData[3] + 2;// 重新算长度
//            cUart1RxData[cUart1RxData[1]+ 2] = HCGetCheckSum(cUart1RxData, cUart1RxData[1]+ 2);//从新算校验
            return 1;//h加密
        }
        else 
        {
            return 2;//加密信息长度错误
        }
    }
    else 
    {
        return 0;//没加密
    }
}
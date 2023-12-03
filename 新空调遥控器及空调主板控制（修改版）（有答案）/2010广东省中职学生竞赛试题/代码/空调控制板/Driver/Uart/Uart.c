#include "Uart\Uart.h"


//*****����******//

void Uart1_Init(void)		//9600bps@11.0592MHz
{
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	BRT = 0xDC;			//���ö�ʱ����ֵ
	AUXR |= 0x01;		//����1ʹ�ö��������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
	ES = 1;
}

void Uart1_sendbyte(unsigned char byte)
{
	SBUF = byte;
	while(!TI);
	TI = 0;
}



void Uart1_Str(uchar *p)
{
	while(*p)
	{
		Uart1_sendbyte(*p++);
	}
}


void Uart1_Isr(void) interrupt 4
{
	if(RI)
	{
		RI = 0;
	}
}

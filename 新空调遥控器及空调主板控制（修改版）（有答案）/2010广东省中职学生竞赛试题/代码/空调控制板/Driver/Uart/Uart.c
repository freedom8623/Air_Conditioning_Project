#include "Uart\Uart.h"


//*****串口******//

void Uart1_Init(void)		//9600bps@11.0592MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器时钟1T模式
	BRT = 0xDC;			//设置定时重载值
	AUXR |= 0x01;		//串口1使用独立波特率发射器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发射器
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

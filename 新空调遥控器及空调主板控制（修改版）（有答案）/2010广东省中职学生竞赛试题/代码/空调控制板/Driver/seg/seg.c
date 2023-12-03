#include "seg\seg.h"
#include "LED\LED.h"



void Seg_Tran(unsigned char *seg_string,unsigned char *seg_buf)
{
	unsigned char i = 0,j = 0,temp = 0;
	for(i = 0;i < 3;i++,j++)
	{
		switch(seg_string[j])
		{
			case '0':temp=0xc0;break;
			case '1':temp=0xf9;break;
			case '2':temp=0xa4;break;
			case '3':temp=0xb0;break;
			case '4':temp=0x99;break;
			case '5':temp=0x92;break;
			case '6':temp=0x82;break;
			case '7':temp=0xf8;break;
			case '8':temp=0x80;break;
			case '9':temp=0x90;break;
			case 'A':temp=0xc0;break;
			case 'B':temp=0xc0;break;
			case '-':temp=0xbf;break;
			case ' ':temp=0xff;break;
			case 'S':temp=0x92;break;
			case 'H':temp=0x89;break;
			default:temp=0xff;break;
		}
		if(seg_string[j+1]=='.')
		{
			temp=temp&0x7f;
			j++;
		}
		seg_buf[i]=temp;
	}
}
extern unsigned char ucled;

void Seg_Disp(unsigned char *seg_buf,unsigned char pos)
{
	P2 = 0xff;
	P3 = (0xef << pos) | 0x07;	
	P2 = seg_buf[pos];	
}



////*******数码管显示函数********//

//void seg_Disp(unsigned char *seg_str)
//{
//	uchar temp = 0 , i = 0,j = 0;
//	for(i = 0; i < 3; i++)
//	{
//		P3 = 0xef << i;

//		if(seg_str[j + 1] == ' ')
//		{
//			j = j + 1;
//			P2 |= 0xff;
//		}
//		if(seg_str[j + 1] == '.')
//		{
//			j = j + 1;
//			P2 &= 0x7f;
//		}
//		j++;
//		Delay1ms();
//		P2 = 0xff;
//	}	
//}



//********延时函数**********//

void Delay100us()		//@12.000MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	i = 57;
	while (--i);
}






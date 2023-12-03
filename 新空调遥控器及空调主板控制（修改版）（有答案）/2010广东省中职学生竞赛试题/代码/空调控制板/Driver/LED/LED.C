#include "LED\LED.h"



void Led_Disp(unsigned char ucled)
{
	P2 = 0xff;
	P2 = ucled;
	P3 = (0xef << 3) | 0x07;
}




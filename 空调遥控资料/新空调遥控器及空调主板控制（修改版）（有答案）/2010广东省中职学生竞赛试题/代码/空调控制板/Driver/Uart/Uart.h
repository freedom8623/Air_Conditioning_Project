#ifndef _UART_H
#define _UART_H


#include "main.h"
void Uart1_Init(void);		//9600bps@12.000MHz
void Uart1_sendbyte(uchar byte);
void Uart1_Str(uchar *p);



#endif

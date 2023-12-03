#ifndef _IR_H
#define _IR_H

#include "main.h"


sbit IR = P1^0;

void Send_IR(unsigned int i);
void NO_Send_IR(unsigned int i);
void NEC_IR_0(void);
void NEC_IR_1(void);
void Delay9us();		//@12.000MHz
void Delay17us();		//@12.000MHz
void Send_Message(unsigned int Date_code);


#endif

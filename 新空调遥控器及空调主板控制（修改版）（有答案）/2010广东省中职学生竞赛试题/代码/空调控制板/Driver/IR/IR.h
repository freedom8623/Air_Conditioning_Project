#ifndef _IR_H
#define _IR_H

#include "main.h"

sbit IR_INPUT = P3^2;
void IR_Init(void);
void IR_Read(unsigned char *pubuff);
//char IR_Read(unsigned char *pubuff);
void InFrared_Init(void);  

#endif


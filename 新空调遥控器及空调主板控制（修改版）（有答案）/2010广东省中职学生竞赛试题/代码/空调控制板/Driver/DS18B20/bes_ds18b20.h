#ifndef _BES_DS18B20_H
#define _BES_DS18B20_H


#include "STC12C5A60S2.h"

sbit DQ = P3^3;  

unsigned int rd_temperature();  

#endif

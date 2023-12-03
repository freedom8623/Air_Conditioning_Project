#include "adc\adc.h"

#define ADC_POWER 0x80
#define ADC_SPEEDLL 0x00
#define ADC_START 0x08
#define ADC_FLAG    0x10            //ADC complete flag

void InitADC(void)
{
  P1ASF = 0x04;
  ADC_RES=0;
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START|0x02;
	Delay1ms();
	Delay1ms();
}


unsigned char GetADCResult(unsigned char ch)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
	_nop_();						//��ѯǰ����ȴ�
	_nop_();
	_nop_();
	_nop_();
	
	while (!(ADC_CONTR & ADC_FLAG));//�ȴ���ɱ�־
	ADC_CONTR &= ~ADC_FLAG;         //�ر� ADC
	return ADC_RES;                 //����ADC���
}


void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

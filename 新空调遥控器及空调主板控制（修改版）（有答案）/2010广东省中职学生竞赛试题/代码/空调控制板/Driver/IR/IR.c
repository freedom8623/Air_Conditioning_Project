#include "IR\IR.h"
#include "LED\LED.h"
#include "Uart\Uart.h"

unsigned int Low_timer = 0;
unsigned int High_timer = 0;

extern unsigned char IR_Buff[2];
extern unsigned char ucled;
unsigned char arr_storr[10];

bit InFrared_Way = 0;   

//**�жϳ�ʼ��***//
void IR_Init(void)   //������չ�
{	
	IR_INPUT = 1;  //ȷ����ʼ���ߵ�ƽ 			
	IT0 = 1;       //�����½��ش���	
	EX0 = 1;       //���ⲿ�ж�	
}

//***�ж�0***//
void EX0_Isr() interrupt 0    
{
	TH0 = 0;
	TL0 = 0;
	TF0 = 0;
	TR0=1;
	InFrared_Way = 1;
}

//***�����ȡ����***//
void InFrared_Init(void)   
{
	if(InFrared_Way == 1)
	{
		EX0 = 0;         //��ֹ�ⲿ�ж��ٴδ���
		while(IR_INPUT == 0);
		TR0 = 0;
		Low_timer = TH0 * 256 + TL0;
		if((Low_timer < 8500)||(Low_timer > 11000))  //�������9ms���˳�
		{
			InFrared_Way = 0;
			EX0 = 1;
			return;
		}
		
		TH0 = 0;
		TL0 = 0;
		TF0 = 0;
		TR0 = 1;
		while(IR_INPUT == 1);
		TR0 = 0;
		High_timer = TH0 * 256 + TL0;
			
		if((High_timer > 3500)||(High_timer < 4500))  //4.5
		{		
			IR_Read(IR_Buff);
		}		
		InFrared_Way = 0;		
		EX0 = 1;
	}
}



//***��ȡ����***//
void IR_Read(unsigned char *pubuff)
{
	unsigned char i = 0, j = 0,temp = 0;
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 8; j++)
		{
			temp = temp >> 1;
			TL0 = 0;				//���ö�ʱ��ʼֵ
			TH0 = 0;				//���ö�ʱ��ʼֵ	
			TR0 = 1;
			while(IR_INPUT == 0);
			TR0 = 0;
			Low_timer = TH0 * 256 + TL0;  

			TL0 = 0;				//���ö�ʱ��ʼֵ
			TH0 = 0;				//���ö�ʱ��ʼֵ	
			TR0 = 1;	
			while(IR_INPUT);		
			TR0 = 0;
			High_timer = TH0 * 256 + TL0;
			if(Low_timer < 1500)
			return;
			
			if((High_timer > 1500) && (High_timer < 2500))
				temp = temp & 0x7f; //0111 1111
			if((High_timer > 3000) && (High_timer < 5000))
				temp = temp | 0x80; //1000 0000
		}
		pubuff[i] = temp;
		temp = 0;
	}
}



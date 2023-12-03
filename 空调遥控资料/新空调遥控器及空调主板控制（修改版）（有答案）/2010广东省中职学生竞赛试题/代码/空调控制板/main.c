#include "DS18B20\bes_ds18b20.h"
#include "IR\IR.h"
#include "seg\seg.h"
#include "LED\LED.h"
#include "timer\timer.h"
#include "Uart\Uart.h"
#include "adc\adc.h"

#define ADC_FLAG 0x10

//******���Ŷ���******//
sbit BUZZ = P1^7;    //������
sbit JK1 = P1^6;     //�̵���JK1
sbit JK2 = P1^5;     //�̵���JK2
sbit MG1 = P1^4;		 //����
sbit AIR = P1^2;     //��������������

//*****��������******//
void Timer1_Init(void);		        //1����@12.000M
void Seg_Proc(void);              //����ܴ�����
void IR_Proc(void);               //���⴦����
void delay_ms(unsigned int ms);   //��ʱ����ʱ
void BUZZ_proc(unsigned char ms); // ������������
void Duty(unsigned char duty);    //ռ�ձȵ��ں���
void ADC_Proc(void);              //ADC������

//********����ܱ�������*********//
unsigned int Seg_Slow_Down=0;
unsigned char pos = 0;          //�������ʾ��һλ
unsigned char seg_string[7];    //ת�����ݴ洢
unsigned char seg_buf[8];       //����ܴ洢

//********������ձ���*******//
unsigned char IR_Slow_Down = 0;
unsigned char IR_Buff[4] = {0,0,0,0}; //���պ����ֵ
unsigned char storage_arr = 0;   			//������մ洢��ֵ
unsigned char S2_ON = 0;         			//״̬�л�

//****��������*****//
bit flag = 0;
unsigned int count = 0;          //��ʱ����
unsigned char Duty_count = 0;    //ռ�ձȼ���
unsigned char fan_add = 0;      //���ȼ���

//*****LED******//
unsigned char Led_Slow_Down = 0;  //LED����
unsigned char ucled = 0xff;      //��ʼ��ȫ��
unsigned char Led_arr[9] = {0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; //LED2~LED9��ˮ��
unsigned char i = 0;             //����ֵ����

//***�¶ȴ���������****//
unsigned int temp = 0;   //�洢�¶ȴ�������ֵ
float add_temp = 0.0;    //���������¶�
unsigned char fan = 0;   //����ռ�ձ�ֵ

//***ADC����***//
unsigned char ADC_Slow_Down = 0;  
float ADC_Get = 0.0; //ADC��ȡ����ֵ

void main()
{
  P1M0 = 0x90; P1M1 = 0x00; //ʹ��P1^4 P1^7Ϊ����ģʽ
	BUZZ = 0; 								//�������ر�
	IR_Init();								//�жϳ�ʼ��
	Timer0_Init();					  //��ʱ��1��ʼ��
	Timer1_Init();						//��ʱ��2��ʼ��
	Uart1_Init();						 	//����1��ʼ��
	InitADC();								//ADC��ʼ��
	do
	{
		temp = rd_temperature() / 16.0;
	}while(temp == 85);       //��ʼ���¶ȴ�����85
	EA = 1;		 								//���жϴ�
	sprintf((char *)seg_string,"    "); //�ϵ�����ܳ�ʼ��
	Seg_Tran(seg_string,seg_buf);

	MG1 = 0;   //P1^4��ʼ��
	JK1 = 0;   //P1^6��ʼ��
	JK2 = 0;   //P1^5��ʼ��
	
	while(1)
	{
		IR_Proc();			//������մ�����
		InFrared_Init();//�����ȡ����
		Seg_Proc();		  //�������ʾ������
		Duty(fan);      //���÷���ռ�ձȺ���
//���Գ���		ADC_Proc();		
	}
}

//***������մ�����****//

void IR_Proc(void)
{
	if(IR_Slow_Down) return;
		IR_Slow_Down = 1;
	
	storage_arr = IR_Buff[1];
	switch(storage_arr)
	{
		case 0x66:       //S2  OFF\ON	
			S2_ON = 1;
			IR_Buff[1] = 0;
		break;
		case 0xff:     	//S3 L
		if(S2_ON == 2)
		{
			if(fan_add < 4) fan_add ++;  //���ڷ���ת��
			if(i < 4) i++;			
			ucled = Led_arr[i];					 //LED����һ��
			BUZZ_proc(100);							 //��������100ms
		
			if(fan_add == 1)
			{
				if(fan < 8) fan += 5;  
			}
			else if(fan_add >= 2 && fan_add < 4)
			{
				if(fan <= 8) fan += 1;
			}	
		}
			IR_Buff[1] = 0;	
		break;		
		case 0x44:     	//S4   R
		if(S2_ON == 2)
		{
			BUZZ_proc(100);
			if(fan_add > 0) fan_add --;
			if(i > 0) i--;
			ucled = Led_arr[i];	

			if(fan_add < 3 && fan_add >= 1)
			{
				if(fan <= 8) fan -= 1;
			}
			else if(fan_add == 0)
			{
				if(fan <= 8 && fan > 0) fan -= 5;
			}
		}
			IR_Buff[1] = 0;
		break;
		case 0xAA:      //S5 DOWN
			BUZZ_proc(100);
			if(--add_temp <= 0.0)  //�¶����½�����
			{
				add_temp = 0.0;
			}	
			if(add_temp == 0)
			{
				JK1 = 0;   //P1^6
				JK2 = 1;   //P1^5	  LED11����					
			}
			if(add_temp == 1)
			{
				JK1 = 1;   //P1^6
				JK2 = 1;   //P1^5   LED12Ϩ��				
			}
			
			IR_Buff[1] = 0;
		break;		
		case 0x55:      //S6 UP
			BUZZ_proc(100);	
			if(++add_temp >= 3.0) //�¶����½�����
			{
				add_temp = 3.0;
			}
			if(add_temp == 2)
			{
				JK1 = 1;   //P1^6
				JK2 = 1;   //P1^5   LED11Ϩ��
			}
			if(add_temp == 3)
			{
				JK1 = 1;   //P1^6
				JK2 = 0;   //P1^5   LED12����
			}
			IR_Buff[1] = 0;
		break;
	}
}

//****����ܴ�����****//
void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
		Seg_Slow_Down=1;
	
	switch(S2_ON)
	{
		case 1:
		BUZZ_proc(200); //��������200ms
		sprintf((char *)seg_string,"0.0.0.");
		Seg_Tran(seg_string,seg_buf);	
		delay_ms(100);
		JK1 = 0;   //P1^6
		JK2 = 1;   //P1^5  ����ģʽ
		S2_ON = 2;
		break;
		case 2:
		sprintf((char *)seg_string,"%3.1f",(float)(rd_temperature() / 16.0) + add_temp);  //�������ʾ
		break;
	}
	Seg_Tran(seg_string,seg_buf);	
}

//***��ʱ��1�ж�***//
void Timer1_Isr(void) interrupt 3
{
	if(++Seg_Slow_Down == 800) Seg_Slow_Down = 0;
	if(++IR_Slow_Down == 50) IR_Slow_Down = 0;
	if(++Led_Slow_Down == 50) Led_Slow_Down = 0;
	if(++ADC_Slow_Down == 10) ADC_Slow_Down = 0;
	TL1 = 0xCD;				//���ö�ʱ��ʼֵ
	TH1 = 0xD4;				//���ö�ʱ��ʼֵ	

	if(++Duty_count == 9)
	{
		Duty_count = 0;
	}
	if(flag == 1)
	{
		count--;
		if(count == 0)
			flag = 0;
	}
	if(S2_ON == 2)
		ucled &= 0xbf;
		
	Led_Disp(ucled);    		//LED��ʾ����
	if(pos == 3) pos = 0;
	Seg_Disp(seg_buf,pos);	//�������ʾ����
	pos++;
}

void Timer0_Isr(void) interrupt 1
{
	TL0 = 0xA4;				//���ö�ʱ��ʼֵ
	TH0 = 0xFF;				//���ö�ʱ��ʼֵ
}


//***���� ADC������***//
void ADC_Proc(void)   
{
	if(ADC_Slow_Down) return;
		ADC_Slow_Down = 1;
	ADC_Get = (5.0 / 1024.0) * ((GetADCResult(0x02) << 2) | ADC_RESL);

	if(ADC_Get < 4.7 && ADC_Get > 4.5)
	{
		ucled &= 0xef;
	}
	else
	{
		ucled |= 0x10;
	}
	
	if(ADC_Get < 4.5 && ADC_Get > 4.3)
	{
		ucled &= 0xdf;
	}
	else
	{
		ucled |= 0x20;
	}
	
	if(ADC_Get < 4.3 && ADC_Get > 4.1)
	{
		ucled &= 0xbf;
	}
	else
	{
		ucled |= 0x40; 
	}
	
	if(ADC_Get < 4.1 && ADC_Get > 4.0)
	{
		ucled &= 0x7f;
	}
	else
	{
		ucled |=0x80;
	}
}


//***ռ�ձȵ��ں���***//
void Duty(unsigned char duty)
{
	if(Duty_count < duty)
	{
		MG1 = 1; 
	}
	else
	{
		MG1 = 0;
	}
}

//****����������****//
void BUZZ_proc(unsigned char ms)
{
	BUZZ = 1;
	delay_ms(ms);
	BUZZ = 0;
}
//*****��ʱ����ʱ*****//
void delay_ms(unsigned int ms)
{
	count = ms;
	flag = 1;
	while(flag);
}

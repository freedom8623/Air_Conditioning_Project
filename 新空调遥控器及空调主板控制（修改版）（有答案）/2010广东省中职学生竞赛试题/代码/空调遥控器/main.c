#include "main.h"
#include "IR\IR.h"
#include "KEY\key.h"

//***������***//
void Key_Proc(void);

//***��������***//
extern bit flag;
extern uint count;
unsigned char key_value = 0;
unsigned char key_Dowm = 0;
unsigned char key_old = 0;
unsigned int Key_Slow_Down = 0;

//***��ʱ***//
void Delay9us();					//@12.000MHz
void Timer0_Init(void);		//1΢��@12.000MHz
void delay_ms(unsigned int ms);


//***��������***//
unsigned char key_flag = 0;
unsigned int key_count = 0 ;

void main()
{
    IR = 0;
    P32 = 0;
    P33 = 0;
    P34 = 0;
    P35 = 0;  		//������ʼ��

    Timer0_Init();//��ʱ����ʼ��
    EA = 1;				//�����ж�

    while(1)
    {
       Key_Proc();
    }
}


//***����������***//
void Key_Proc(void)
{
    if(Key_Slow_Down) return;   //10ms����һ��
    Key_Slow_Down=1;
    key_value=Key_Read();
    key_Dowm=key_value&(key_value^key_old);
    key_old=key_value;

    switch(key_Dowm)
    {
    case 1:
        key_flag = 5;         //S5 UP
        break;
    case 2:
        key_flag = 7;         //S7 DOWN
        break;
    case 3:
        key_flag = 1;         //S3 L
        break;
    case 5:
        Send_Message(0x66AA); //S2 OFF\ON
        break;
    case 6:
        key_flag = 3;         //S4 R
        break;
    }

    switch(key_flag)
    {
    case 1:
        if(key_value == 3)
        {
            key_flag = 2;
            key_count = 0;
        }
        break;
    case 2:
        if(key_value == 3)
        {
            if(key_count > 300)
            {
                Send_Message(0xff00);  //S3 L
                delay_ms(100);
            }
        }
        else
        {
            key_flag = 0;
            key_count = 0;
        }
        break;

    case 3:
        if(key_value == 6)
        {
            key_flag = 4;
            key_count = 0;
        }
        break;
    case 4:
        if(key_value == 6)
        {
            if(key_count > 300)
            {
                Send_Message(0x4455);  //S4 R
                delay_ms(100);
            }
        }
        else
        {
            key_flag = 0;
            key_count = 0;
        }
        break;

    case 5:
        if(key_value == 1)
        {
            key_flag = 6;
            key_count = 0;
        }
        break;
    case 6:
        if(key_value == 1)
        {
            if(key_count > 300)
            {
                Send_Message(0x55AA);   //S5 UP
                delay_ms(100);
            }
        }
        else
        {
            key_flag = 0;
            key_count = 0;
        }
        break;

    case 7:
        if(key_value == 2)
        {
            key_flag = 8;
            key_count = 0;
        }
        break;
    case 8:
        if(key_value == 2)
        {
            if(key_count > 300)
            {
                Send_Message(0xAA55);  //S7 DOWN
            }
        }
        else
        {
            key_flag = 0;
            key_count = 0;
        }
        break;
    }
}

//***��ʱ���жϷ���***//
void Timer0_Isr(void) interrupt 1
{
    TL0 = 0x20;				//���ö�ʱ��ʼֵ
    TH0 = 0xD1;				//���ö�ʱ��ʼֵ

    if(key_flag == 2 || key_flag == 4 || key_flag == 6 || key_flag == 8)
        key_count++;

    if(flag == 1)    //��ʱ����ʱ
    {
        count--;
        if(count == 0)
            flag = 0;
    }
    if(++Key_Slow_Down == 10) Key_Slow_Down = 0;
}

//***��ʱ��0��ʼ��***//
void Timer0_Init(void)		//1����@12.000MHz
{
    AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;			//���ö�ʱ��ģʽ
    TMOD |= 0x01;			//���ö�ʱ��ģʽ
    TL0 = 0x20;				//���ö�ʱ��ʼֵ
    TH0 = 0xD1;				//���ö�ʱ��ʼֵ
    TF0 = 0;				//���TF0��־
    TR0 = 1;				//��ʱ��0��ʼ��ʱ
    ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

//***��ʱ����ʱ**//
void delay_ms(unsigned int ms)
{
    count = ms;
    flag = 1;
    while(flag);
}




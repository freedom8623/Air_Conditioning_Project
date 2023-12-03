#include "IR\IR.h"

//�ߵ�ƽʱ����9us �͵�ƽʱ����17us �ܹ�26us

uint count = 0;
bit flag = 0;
//�����ز��ź�
void Send_IR(unsigned int i)
{
    while(i--)
    {
        IR = 1;
        Delay9us();
        IR = 0;
        Delay17us();
    }
}


//�������ز��ź�  0
void NO_Send_IR(unsigned int i)
{
    while(i--)
    {
        IR = 0;
        Delay9us();
        Delay17us();
    }
}


//����0
void NEC_IR_0(void)
{
    Send_IR(67);
    NO_Send_IR(67);
}


//����1
void NEC_IR_1(void)
{
    Send_IR(67);
    NO_Send_IR(150);
}


//���͵�ַ�� ��ַ���� ������ ����� ������

void Send_Message(unsigned int Date_code)
{
    unsigned int temp = 0, i = 0;
    Send_IR(342);// 9ms������
    NO_Send_IR(170); //4.5ms�������ز��ź�

    temp = Date_code;

    for(i = 0; i < 16; i++)
    {
        if(temp & 0x0001)
            NEC_IR_1();
        else
            NEC_IR_0();
        temp = temp >> 1;
    }
    //���ͽ�����
    NEC_IR_0();
}

void Delay9us()		//@12.000MHz
{
    unsigned char data i;

    i = 24;
    while (--i);
}

void Delay17us()		//@12.000MHz
{
    unsigned char data i;

    i = 44;
    while (--i);
}

void delay_us(unsigned int us)
{
    count = us;
    flag = 1;
    while(flag);
}

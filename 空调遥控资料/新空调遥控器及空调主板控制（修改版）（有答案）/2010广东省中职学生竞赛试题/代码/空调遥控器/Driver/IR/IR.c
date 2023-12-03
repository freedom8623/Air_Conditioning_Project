#include "IR\IR.h"

//高电平时间是9us 低电平时间是17us 总共26us

uint count = 0;
bit flag = 0;
//发送载波信号
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


//不发送载波信号  0
void NO_Send_IR(unsigned int i)
{
    while(i--)
    {
        IR = 0;
        Delay9us();
        Delay17us();
    }
}


//发送0
void NEC_IR_0(void)
{
    Send_IR(67);
    NO_Send_IR(67);
}


//发送1
void NEC_IR_1(void)
{
    Send_IR(67);
    NO_Send_IR(150);
}


//发送地址码 地址反码 命令码 命令反码 结束码

void Send_Message(unsigned int Date_code)
{
    unsigned int temp = 0, i = 0;
    Send_IR(342);// 9ms引导码
    NO_Send_IR(170); //4.5ms不发送载波信号

    temp = Date_code;

    for(i = 0; i < 16; i++)
    {
        if(temp & 0x0001)
            NEC_IR_1();
        else
            NEC_IR_0();
        temp = temp >> 1;
    }
    //发送结束码
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

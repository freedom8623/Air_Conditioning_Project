#include "DS18B20\bes_ds18b20.h"
#include "IR\IR.h"
#include "seg\seg.h"
#include "LED\LED.h"
#include "timer\timer.h"
#include "Uart\Uart.h"
#include "adc\adc.h"

#define ADC_FLAG 0x10

//******引脚定义******//
sbit BUZZ = P1^7;    //蜂鸣器
sbit JK1 = P1^6;     //继电器JK1
sbit JK2 = P1^5;     //继电器JK2
sbit MG1 = P1^4;		 //风扇
sbit AIR = P1^2;     //空气质量传感器

//*****函数声明******//
void Timer1_Init(void);		        //1毫秒@12.000M
void Seg_Proc(void);              //数码管处理函数
void IR_Proc(void);               //红外处理函数
void delay_ms(unsigned int ms);   //定时器延时
void BUZZ_proc(unsigned char ms); // 蜂鸣器处理函数
void Duty(unsigned char duty);    //占空比调节函数
void ADC_Proc(void);              //ADC处理函数

//********数码管变量声明*********//
unsigned int Seg_Slow_Down=0;
unsigned char pos = 0;          //数码管显示哪一位
unsigned char seg_string[7];    //转化数据存储
unsigned char seg_buf[8];       //数码管存储

//********红外接收变量*******//
unsigned char IR_Slow_Down = 0;
unsigned char IR_Buff[4] = {0,0,0,0}; //接收红外的值
unsigned char storage_arr = 0;   			//红外接收存储的值
unsigned char S2_ON = 0;         			//状态切换

//****计数变量*****//
bit flag = 0;
unsigned int count = 0;          //延时计数
unsigned char Duty_count = 0;    //占空比计数
unsigned char fan_add = 0;      //风扇计数

//*****LED******//
unsigned char Led_Slow_Down = 0;  //LED减速
unsigned char ucled = 0xff;      //初始化全灭
unsigned char Led_arr[9] = {0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f}; //LED2~LED9流水灯
unsigned char i = 0;             //数组值增加

//***温度传感器变量****//
unsigned int temp = 0;   //存储温度传感器的值
float add_temp = 0.0;    //按键增加温度
unsigned char fan = 0;   //风扇占空比值

//***ADC变量***//
unsigned char ADC_Slow_Down = 0;  
float ADC_Get = 0.0; //ADC获取变量值

void main()
{
  P1M0 = 0x90; P1M1 = 0x00; //使能P1^4 P1^7为推挽模式
	BUZZ = 0; 								//蜂鸣器关闭
	IR_Init();								//中断初始化
	Timer0_Init();					  //定时器1初始化
	Timer1_Init();						//定时器2初始化
	Uart1_Init();						 	//串口1初始化
	InitADC();								//ADC初始化
	do
	{
		temp = rd_temperature() / 16.0;
	}while(temp == 85);       //初始化温度传感器85
	EA = 1;		 								//总中断打开
	sprintf((char *)seg_string,"    "); //上电数码管初始化
	Seg_Tran(seg_string,seg_buf);

	MG1 = 0;   //P1^4初始化
	JK1 = 0;   //P1^6初始化
	JK2 = 0;   //P1^5初始化
	
	while(1)
	{
		IR_Proc();			//红外接收处理函数
		InFrared_Init();//红外读取数据
		Seg_Proc();		  //数码管显示处理函数
		Duty(fan);      //设置风扇占空比函数
//测试程序		ADC_Proc();		
	}
}

//***红外接收处理函数****//

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
			if(fan_add < 4) fan_add ++;  //调节风扇转速
			if(i < 4) i++;			
			ucled = Led_arr[i];					 //LED亮哪一个
			BUZZ_proc(100);							 //蜂鸣器响100ms
		
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
			if(--add_temp <= 0.0)  //温度上下降变量
			{
				add_temp = 0.0;
			}	
			if(add_temp == 0)
			{
				JK1 = 0;   //P1^6
				JK2 = 1;   //P1^5	  LED11制冷					
			}
			if(add_temp == 1)
			{
				JK1 = 1;   //P1^6
				JK2 = 1;   //P1^5   LED12熄灭				
			}
			
			IR_Buff[1] = 0;
		break;		
		case 0x55:      //S6 UP
			BUZZ_proc(100);	
			if(++add_temp >= 3.0) //温度上下降变量
			{
				add_temp = 3.0;
			}
			if(add_temp == 2)
			{
				JK1 = 1;   //P1^6
				JK2 = 1;   //P1^5   LED11熄灭
			}
			if(add_temp == 3)
			{
				JK1 = 1;   //P1^6
				JK2 = 0;   //P1^5   LED12制热
			}
			IR_Buff[1] = 0;
		break;
	}
}

//****数码管处理函数****//
void Seg_Proc(void)
{
	if(Seg_Slow_Down) return;
		Seg_Slow_Down=1;
	
	switch(S2_ON)
	{
		case 1:
		BUZZ_proc(200); //蜂鸣器响200ms
		sprintf((char *)seg_string,"0.0.0.");
		Seg_Tran(seg_string,seg_buf);	
		delay_ms(100);
		JK1 = 0;   //P1^6
		JK2 = 1;   //P1^5  制冷模式
		S2_ON = 2;
		break;
		case 2:
		sprintf((char *)seg_string,"%3.1f",(float)(rd_temperature() / 16.0) + add_temp);  //数码管显示
		break;
	}
	Seg_Tran(seg_string,seg_buf);	
}

//***定时器1中断***//
void Timer1_Isr(void) interrupt 3
{
	if(++Seg_Slow_Down == 800) Seg_Slow_Down = 0;
	if(++IR_Slow_Down == 50) IR_Slow_Down = 0;
	if(++Led_Slow_Down == 50) Led_Slow_Down = 0;
	if(++ADC_Slow_Down == 10) ADC_Slow_Down = 0;
	TL1 = 0xCD;				//设置定时初始值
	TH1 = 0xD4;				//设置定时初始值	

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
		
	Led_Disp(ucled);    		//LED显示函数
	if(pos == 3) pos = 0;
	Seg_Disp(seg_buf,pos);	//数码管显示函数
	pos++;
}

void Timer0_Isr(void) interrupt 1
{
	TL0 = 0xA4;				//设置定时初始值
	TH0 = 0xFF;				//设置定时初始值
}


//***测试 ADC处理函数***//
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


//***占空比调节函数***//
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

//****蜂鸣器函数****//
void BUZZ_proc(unsigned char ms)
{
	BUZZ = 1;
	delay_ms(ms);
	BUZZ = 0;
}
//*****定时器延时*****//
void delay_ms(unsigned int ms)
{
	count = ms;
	flag = 1;
	while(flag);
}

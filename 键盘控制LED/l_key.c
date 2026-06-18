/***************************************************************************
标题：按键匹配LED+数码管程序
功能：按下第N个按键，第N个LED点亮，数码管同步显示数字N，松开复位
硬件说明：
1. 按键 key1~key6 依次 P3^7、P3^6、P3^5、P3^4、P3^3、P3^2 低电平触发
2. LED  led1~led6 依次 P2^0、P2^1、P2^2、P2^3、P2^4、P2^5 共阳低电平亮
3. 四位共阳数码管 P0段选，P1^3个位常亮
共阳段码：0:0xC0 1:0xF9 2:0xA4 3:0xB0 4:0x99 5:0x92 6:0x82
******************************************************************************/
#include "reg51.h"

// 数码管位选
sbit SMG_q = P1^0;	//千位
sbit SMG_b = P1^1;	//百位
sbit SMG_s = P1^2;	//十位
sbit SMG_g = P1^3;	//个位

// 6个独立按键
sbit key1 = P3^7;
sbit key2 = P3^6;
sbit key3 = P3^5;
sbit key4 = P3^4;
sbit key5 = P3^3;
sbit key6 = P3^2;

// 6路LED
sbit led1 = P2^0;
sbit led2 = P2^1;
sbit led3 = P2^2;
sbit led4 = P2^3;
sbit led5 = P2^4;
sbit led6 = P2^5;

// 消抖延时函数
void delay(unsigned char t)
{
	unsigned char i,j;
	for(i = t; i > 0; i--)
		for(j = 255; j > 0; j--);
}

// 关闭全部LED
void AllLedOff(void)
{
	led1 = 1;
	led2 = 1;
	led3 = 1;
	led4 = 1;
	led5 = 1;
	led6 = 1;
}

void main(void)
{
	SMG_g = 0;      // 打开个位数码管
	AllLedOff();    // 上电所有LED熄灭
	P0 = 0xC0;      // 默认显示0
	
	while(1)
	{
		if(key1 == 0)
		{
			delay(10);
			if(key1 == 0)
			{
				AllLedOff();
				led1 = 0;    // 1号灯亮
				P0 = 0xF9;   // 显示数字1
			}
			while(key1 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
		else if(key2 == 0)
		{
			delay(10);
			if(key2 == 0)
			{
				AllLedOff();
				led2 = 0;    // 2号灯亮
				P0 = 0xA4;   // 显示数字2
			}
			while(key2 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
		else if(key3 == 0)
		{
			delay(10);
			if(key3 == 0)
			{
				AllLedOff();
				led3 = 0;    // 3号灯亮
				P0 = 0xB0;   // 显示数字3
			}
			while(key3 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
		else if(key4 == 0)
		{
			delay(10);
			if(key4 == 0)
			{
				AllLedOff();
				led4 = 0;    // 4号灯亮
				P0 = 0x99;   // 显示数字4
			}
			while(key4 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
		else if(key5 == 0)
		{
			delay(10);
			if(key5 == 0)
			{
				AllLedOff();
				led5 = 0;    // 5号灯亮
				P0 = 0x92;   // 显示数字5
			}
			while(key5 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
		else if(key6 == 0)
		{
			delay(10);
			if(key6 == 0)
			{
				AllLedOff();
				led6 = 0;    // 6号灯亮
				P0 = 0x82;   // 显示数字6
			}
			while(key6 == 0);
			delay(10);
			AllLedOff();
			P0 = 0xC0;
		}
	}
}
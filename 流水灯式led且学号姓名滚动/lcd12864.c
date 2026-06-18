#include "reg51.h"

sbit LCD_RS = P1^0;
sbit LCD_RW = P1^1;
sbit LCD_E  = P1^2;
sbit PSB    = P1^6;

#define LCD_Data P0
#define Busy     0x80

code unsigned char tmpled[50]={
0X01,0X02,0X04,0X08,0X10,0X20,0X40,0X80,0X81,0X82,
0X84,0X88,0X90,0XA0,0XC0,0XC1,0XC2,0XC4,0XC8,0XD0,
0XE0,0XE1,0XE2,0XE4,0XE8,0XF0,0XF1,0XF2,0XF4,0XF8,
0XF9,0XFA,0XFC,0XFD,0XFE,0XFF,0XFF,0X00,0XFF,0X00,
0XFF,0X00,0XFF,0X00,0XFF,0X00,0XFF,0X00,0XFF,0X00
};
unsigned char led_idx = 0;

typedef struct
{
    unsigned char name[16];
    unsigned char id[16];
} STU;
code STU stu[5] = {
    {"Grade",       "2025ji"},
    {"Luguanzhi",   "210635"},
    {"Laiweijun",   "210641"},
    {"Wangqihua",   "210603"},
    {"Qiyunzhe",    "210604"}
};

void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD, unsigned char BuysC);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void LCDClear(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void Delay5Ms(void);
void Delay400Ms(void);
void ScrollText(unsigned char* show_str);
void delay(unsigned char tmp);
void LedTask(void);

void LedTask(void)
{
    P2 = ~tmpled[led_idx];
    led_idx++;
    if(led_idx >= 50)
        led_idx = 0;
}

void main(void)
{
    unsigned char i,j,p;
    unsigned char show_buf[32];
    PSB = 1;
    Delay400Ms();
    LCDInit();

    while (1)
    {
        // 高频刷新流水灯，不阻塞
        LedTask();

        for (i = 0; i < 5; i++)
        {
            p = 0;
            for (j = 0; stu[i].name[j] != '\0'; j++)
                show_buf[p++] = stu[i].name[j];
            show_buf[p++] = ' ';
            for (j = 0; stu[i].id[j] != '\0'; j++)
                show_buf[p++] = stu[i].id[j];
            show_buf[p] = '\0';

            ScrollText(show_buf);
            LCDClear();
            Delay400Ms();
        }
    }
}

void WriteDataLCD(unsigned char WDLCD)
{
    ReadStatusLCD();
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_Data = WDLCD;
    LCD_E = 1;
    Delay5Ms();
    LCD_E = 0;
}

void WriteCommandLCD(unsigned char WCLCD, unsigned char BuysC)
{
    if (BuysC) ReadStatusLCD();
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_Data = WCLCD;
    LCD_E = 1;
    Delay5Ms();
    LCD_E = 0;
}

unsigned char ReadStatusLCD(void)
{
    unsigned char sta;
    LCD_Data = 0xFF;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_E = 1;
    Delay5Ms();
    sta = LCD_Data;
    while (sta & Busy)
    {
        sta = LCD_Data;
    }
    LCD_E = 0;
    return sta;
}

void LCDInit(void)
{
    WriteCommandLCD(0x30, 1);
    Delay5Ms();
    WriteCommandLCD(0x0C, 1);
    Delay5Ms();
    WriteCommandLCD(0x01, 1);
    Delay5Ms();
    WriteCommandLCD(0x06, 1);
    WriteCommandLCD(0x34, 1);
    Delay5Ms();
}

void LCDClear(void)
{
    WriteCommandLCD(0x01, 1);
    Delay5Ms();
}

void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
    unsigned char addr;
    if (Y != 1)
        return;
    X &= 0x0F;
    addr = 0x80 + X;
    WriteCommandLCD(addr, 0);
    WriteDataLCD(DData);
}

void ScrollText(unsigned char* show_str)
{
    unsigned char i, str_len = 0;
    while (show_str[str_len] != '\0') str_len++;

    LCDClear();
    for (i = 0; i < str_len && i < 16; i++)
    {
        DisplayOneChar(i, 1, show_str[i]);
        LedTask(); // 每显示一个字符刷新一次LED
    }
    // 等待期间持续刷新流水灯，保证流动
    for(i=0;i<100;i++)
    {
        LedTask();
        delay(30);
    }
}

void Delay5Ms(void)
{
    unsigned int TempCyc = 5552;
    while (TempCyc--);
}

void Delay400Ms(void)
{
    unsigned char TempCycA = 5;
    unsigned int TempCycB;
    while (TempCycA--)
    {
        TempCycB = 7269;
        while (TempCycB--);
    }
}

void delay(unsigned char tmp)
{
    unsigned char j;
    while(tmp--)
    {
        j=255;
        while(j--);
    }
}
//代码由流水灯和滚动拼接而成
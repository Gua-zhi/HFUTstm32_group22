#include "reg51.h"

/********IO引脚定义***********************************************************/
sbit LCD_RS = P1 ^ 0;
sbit LCD_RW = P1 ^ 1;
sbit LCD_E = P1 ^ 2;
sbit PSB = P1 ^ 6;		//PSB脚为12864-12系列的串、并通讯功能切换，使用8位并行接口，PSB=1

/********宏定义***********************************************************/
#define LCD_Data P0
#define Busy    0x80 //用于检测LCD状态字中的Busy标识

/********函数声明*************************************************************/
void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD, BuysC);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void LCDClear(void);
void LCDFlash(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void Delay5Ms(void);
void Delay400Ms(void);
void ScrollText(unsigned char* show_str);//滚动函数

/*********结构体姓名学号***************************************************/
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
/***********主函数开始********************************************************/
void main(void)
{
    unsigned char i;
    unsigned char show_buf[32]; //拼接缓存：姓名+空格+学号
    PSB = 1;
    Delay400Ms();
    LCDInit();

    while (1)
    {
        //循环遍历4名学生
        for (i = 0; i < 5; i++)
        {
            unsigned char p = 0, j;
            //1.复制姓名拼音
            for (j = 0; stu[i].name[j] != '\0'; j++)
                show_buf[p++] = stu[i].name[j];
            show_buf[p++] = ' '; //空格分隔姓名与学号
            //2.复制学号
            for (j = 0; stu[i].id[j] != '\0'; j++)
                show_buf[p++] = stu[i].id[j];
            show_buf[p] = '\0'; //字符串结束标记

            ScrollText(show_buf);
            LCDClear();
            Delay400Ms();
        }
    }
}

/***********写数据********************************************************/
void WriteDataLCD(unsigned char WDLCD)
{
    ReadStatusLCD(); //检测忙 
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_Data = WDLCD;
    LCD_E = 1;
    Delay5Ms();
    LCD_E = 0;
}

/***********写指令********************************************************/
void WriteCommandLCD(unsigned char WCLCD, BuysC) //BuysC为0时忽略忙检测
{
    if (BuysC) ReadStatusLCD(); //根据需要检测忙 
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_Data = WCLCD;
    LCD_E = 1;
    Delay5Ms();
    LCD_E = 0;
}

/***********读状态*******************************************************/
unsigned char ReadStatusLCD(void)
{
    LCD_Data = 0xFF;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_E = 1;
    Delay5Ms();
    while (LCD_Data & Busy); //检测忙信号	
    LCD_E = 0;
    return(1);
}

/***********初始化********************************************************/
void LCDInit(void)
{
    WriteCommandLCD(0x30, 1); //先基础模式上电
    Delay5Ms();
    WriteCommandLCD(0x0C, 1); //开显示，无光标
    Delay5Ms();
    WriteCommandLCD(0x01, 1); //清屏
    Delay5Ms();
    WriteCommandLCD(0x06, 1); //地址自增
    // 新增：切换到汉字指令集，支持GB2312汉字
    WriteCommandLCD(0x34, 1);
    Delay5Ms();
}
/***********清屏********************************************************/
void LCDClear(void)
{
    WriteCommandLCD(0x01, 1); //显示清屏
    WriteCommandLCD(0x34, 1); // 显示光标移动设置
    WriteCommandLCD(0x30, 1); // 显示开及光标设置
}


/***********按指定位置显示一个字符*******************************************/
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
    unsigned char addr;
    //只允许第1行输出
    if (Y != 1)
        return;

    X &= 0x0F;  //限制列0~15
    addr = 0x80 + X; //第一行固定基地址

    WriteCommandLCD(addr, 0);
    WriteDataLCD(DData);
}

/***********文字滚动函数*****************************************/
void ScrollText(unsigned char* show_str)
{
    unsigned char i, str_len;
    str_len = 0;
    while (show_str[str_len] != '\0') str_len++;

    LCDClear();
    for (i = 0; i < str_len && i < 16; i++)
    {
        DisplayOneChar(i, 1, show_str[i]);
    }
    Delay400Ms();
    Delay400Ms();
    Delay400Ms();
    Delay400Ms();
}
/***********短延时********************************************************/
void Delay5Ms(void)
{
    unsigned int TempCyc = 5552;
    while (TempCyc--);
}

/***********长延时********************************************************/
void Delay400Ms(void)
{
    unsigned char TempCycA = 5;
    unsigned int TempCycB;
    while (TempCycA--) {
        TempCycB = 7269;
        while (TempCycB--);
    }
}

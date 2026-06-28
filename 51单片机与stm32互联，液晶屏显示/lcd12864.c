/***************************************************************************
主题： LCD12864显示 + 串口接收K210颜色识别数据
       P3.0(RXD) <-- STM32 UART4 TX(PC10)
       9600bps, 12.000MHz (Timer2 baud generator)
       显示: 颜色RGY / 诊断 $:#:F:
******************************************************************************/
#include "reg51.h"
#include "string.h"

/* Timer2 SFRs for 12MHz -> exact 9600 baud (not in reg51.h) */
sfr T2CON  = 0xC8;
sfr RCAP2L = 0xCA;
sfr RCAP2H = 0xCB;
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;

sbit LCD_RS = P1^0;
sbit LCD_RW = P1^1;
sbit LCD_E  = P1^2;
sbit PSB    = P1^6;
#define LCD_Data P0
#define Busy    0x80

void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD, unsigned char BuysC);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void LCDClear(void);
void DisplayString(unsigned char X, unsigned char Y, unsigned char *DData);
void Delay5Ms(void);
void Delay400Ms(void);

unsigned char uart_rxbuf[20];
unsigned char uart_rx_num = 0;
unsigned char uart_newdata = 0;
unsigned char uart_start = 0;
unsigned int  rx_raw = 0;
unsigned int  rx_dollar = 0;
unsigned int  rx_hash = 0;
unsigned int  rx_done = 0;

unsigned char lcd_buf[17] = "R:0  G:0  Y:0   ";
unsigned int  scroll = 0;

void main(void)
{
	unsigned char i;

	/* 12.000MHz -> exact 9600 baud via Timer2 */
	SCON = 0x50;
	T2CON = 0x34; RCAP2H = 0xFF; RCAP2L = 0xD9;
	TH2 = 0xFF; TL2 = 0xD9;
	ES = 1; EA = 1;

	Delay400Ms(); LCDInit(); Delay5Ms(); LCDClear();
	DisplayString(0, 1, lcd_buf);

	while(1)
	{
		scroll++;
		if (scroll < 15000)
		{
			DisplayString(0, 1, lcd_buf);
		}
		else if (scroll < 25000)
		{
			/* diagnostic: $:XXX #:XXX F:XXX */
			unsigned char d[17];
			for (i = 0; i < 16; i++) d[i] = ' ';
			d[0] = '$'; d[1] = ':';
			d[2] = (rx_dollar/100)%10 + '0';
			d[3] = (rx_dollar/10)%10 + '0';
			d[4] = (rx_dollar%10) + '0';
			d[5] = '#'; d[6] = ':';
			d[7] = (rx_hash/100)%10 + '0';
			d[8] = (rx_hash/10)%10 + '0';
			d[9] = (rx_hash%10) + '0';
			d[10] = 'F'; d[11] = ':';
			d[12] = (rx_done/100)%10 + '0';
			d[13] = (rx_done/10)%10 + '0';
			d[14] = (rx_done%10) + '0';
			d[16] = '\0';
			DisplayString(0, 1, d);
		}
		else { scroll = 0; }

		if (uart_newdata == 1)
		{
			uart_newdata = 0;
			if (uart_rxbuf[0] == '$')
			{
				unsigned char rv=0, gv=0, yv=0, j;
				for (j = 1; j < uart_rx_num; j++)
				{
					if (uart_rxbuf[j] == 'R')
					{
						j++; rv = 0;
						while (j < uart_rx_num && uart_rxbuf[j] >= '0' && uart_rxbuf[j] <= '9')
							{ rv = rv * 10 + (uart_rxbuf[j] - '0'); j++; }
						j--;
					}
					if (uart_rxbuf[j] == 'G')
					{
						j++; gv = 0;
						while (j < uart_rx_num && uart_rxbuf[j] >= '0' && uart_rxbuf[j] <= '9')
							{ gv = gv * 10 + (uart_rxbuf[j] - '0'); j++; }
						j--;
					}
					if (uart_rxbuf[j] == 'Y')
					{
						j++; yv = 0;
						while (j < uart_rx_num && uart_rxbuf[j] >= '0' && uart_rxbuf[j] <= '9')
							{ yv = yv * 10 + (uart_rxbuf[j] - '0'); j++; }
						j--;
					}
				}
				for (i = 0; i < 17; i++) lcd_buf[i] = ' ';
				lcd_buf[0] = 'R'; lcd_buf[1] = ':';
				if (rv >= 10) { lcd_buf[2] = (rv/10)+'0'; lcd_buf[3] = (rv%10)+'0'; }
				else          { lcd_buf[2] = rv+'0';        lcd_buf[3] = ' '; }
				lcd_buf[4] = ' '; lcd_buf[5] = ' ';
				lcd_buf[6] = 'G'; lcd_buf[7] = ':';
				if (gv >= 10) { lcd_buf[8] = (gv/10)+'0';  lcd_buf[9] = (gv%10)+'0'; }
				else          { lcd_buf[8] = gv+'0';        lcd_buf[9] = ' '; }
				lcd_buf[10] = ' '; lcd_buf[11] = ' ';
				lcd_buf[12] = 'Y'; lcd_buf[13] = ':';
				if (yv >= 10) { lcd_buf[14] = (yv/10)+'0'; lcd_buf[15] = (yv%10)+'0'; }
				else          { lcd_buf[14] = yv+'0';       lcd_buf[15] = ' '; }
				lcd_buf[16] = '\0';
			}
			memset(uart_rxbuf, 0, sizeof(uart_rxbuf));
		}
	}
}

void UART_ISR(void) interrupt 4
{
	unsigned char rx_data;
	if (RI == 1)
	{
		RI = 0;
		rx_data = SBUF;
		rx_raw++;

		if (rx_data == '$')
		{
			rx_dollar++;
			uart_start = 1;
			uart_rx_num = 0;
			uart_rxbuf[uart_rx_num++] = rx_data;
		}
		else if (uart_start == 1)
		{
			uart_rxbuf[uart_rx_num++] = rx_data;
			if (rx_data == '#')
			{
				rx_hash++;
				uart_newdata = 1;
				rx_done++;
				uart_start = 0;
			}
			if (uart_rx_num >= 19)
			{
				uart_rx_num = 0;
				uart_start = 0;
			}
		}
	}
	if (TI == 1) { TI = 0; }
}

void WriteDataLCD(unsigned char WDLCD)
{
	ReadStatusLCD();
	LCD_RS = 1; LCD_RW = 0;
	LCD_Data = WDLCD;
	LCD_E = 1; LCD_E = 1; LCD_E = 0;
}
void WriteCommandLCD(unsigned char WCLCD, unsigned char BuysC)
{
	if (BuysC) ReadStatusLCD();
	LCD_RS = 0; LCD_RW = 0;
	LCD_Data = WCLCD;
	LCD_E = 1; LCD_E = 1; LCD_E = 0;
}
unsigned char ReadStatusLCD(void)
{
	LCD_Data = 0xFF;
	LCD_RS = 0; LCD_RW = 1;
	LCD_E = 1; LCD_E = 1;
	while (LCD_Data & Busy);
	LCD_E = 0;
	return(1);
}
void LCDInit(void)
{
	WriteCommandLCD(0x30, 1); WriteCommandLCD(0x01, 1);
	WriteCommandLCD(0x06, 1); WriteCommandLCD(0x0C, 1);
}
void LCDClear(void)
{
	WriteCommandLCD(0x01, 1); WriteCommandLCD(0x34, 1); WriteCommandLCD(0x30, 1);
}
void DisplayString(unsigned char X, unsigned char Y, unsigned char *DData)
{
	unsigned char i = 0, X2 = X;
	if (Y < 1) Y = 1; if (Y > 4) Y = 4;
	X &= 0x0F;
	switch (Y) {
	case 1: X2 |= 0X80; break;
	case 2: X2 |= 0X90; break;
	case 3: X2 |= 0X88; break;
	case 4: X2 |= 0X98; break;
	}
	WriteCommandLCD(X2, 1);
	while (DData[i] != '\0' && i < 16) { WriteDataLCD(DData[i]); i++; X++; }
}
void Delay5Ms(void) { unsigned int t = 5552; while (t--); }
void Delay400Ms(void)
{
	unsigned char a = 5; unsigned int b;
	while (a--) { b = 7269; while (b--); }
}

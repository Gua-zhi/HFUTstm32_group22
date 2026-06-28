/**
* @par Copyright (C): 2018-2028, Shenzhen Yahboom Tech
* @file         // main.c
* @author       // lly
* @version      // V1.0
* @date         // 240628
* @brief        // 程序入口 Program entry
* @details
* @par History  // 修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简述
*/

#include "AllHeader.h"
#include "intsever.h"

uint8_t GET_Angle_Way=2;
float Angle_Balance,Gyro_Balance,Gyro_Turn;
int Motor_Left,Motor_Right;
int Temperature;
float Acceleration_Z;
int Mid_Angle;
float Move_X,Move_Z;
u32 g_distance = 0; // 距离值（超声波/其他测距，07项目不使用超声，保持为0） Distance value (ultrasonic/other, unused in 07 project, stays 0)
u8 Stop_Flag = 1;


char showbuf[20]={'\0'};


int main(void)
{
	Mid_Angle = -1;

	bsp_init();

	MPU6050_EXTI_Init();

	OLED_Draw_Line("put down key start!", 1, true, true);

	while(!Key1_State(1));
	Stop_Flag = 0;

	OLED_Draw_Line("Start discern QR!", 1, true, true);

	while(1)
	{
		Change_state();
		BT_Change_State();
	}

}

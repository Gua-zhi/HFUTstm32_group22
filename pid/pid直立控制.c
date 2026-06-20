int Balance_PD(float Angle,float Gyro)
{
    float Angle_bias,Gyro_bias;
    int balance;
    Angle_bias=Mid_Angle-Angle;                    
    Gyro_bias=0-Gyro;
    balance=-Balance_Kp/100*Angle_bias-Gyro_bias*Balance_Kd/100;  
    return balance;
}
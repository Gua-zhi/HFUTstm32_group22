void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  
        Encoder_Left = Read_Encoder(MOTOR_ID_ML);
        Encoder_Right = -Read_Encoder(MOTOR_ID_MR);
        Position_L += Encoder_Left;
        Position_R += Encoder_Right;
        Position_Motor_L = Position_PID(Position_L,Target_Position);
        Position_Motor_R = Position_PID(Position_R,Target_Position);
        motor_L = Incremental_PI(Encoder_Left,Position_Motor_L);
        motor_R = Incremental_PI(Encoder_Right,Position_Motor_R);
        motor_L = PWM_Ignore(motor_L);
        motor_R = PWM_Ignore(motor_R);
        motor_L = PWM_Limit(motor_L,2500,-2500);
        motor_R = PWM_Limit(motor_R,2500,-2500);
        Set_Pwm(motor_L,motor_R);
    }
}

int Position_PID (int position,int target)
{
    static float error,Pwm,Integral_error,Last_error;
    error=target-position;                              
    Integral_error+=error;                              
    Integral_error=PWM_Limit(Integral_error,Target_Velocity,-Target_Velocity); 
    Pwm=Position_KP*error+Position_KI*Integral_error+Position_KD*(error-Last_error);       
    Last_error=error;                                   
    Pwm=PWM_Limit(Pwm,Target_Velocity,-Target_Velocity);    
    return Pwm;                                         
}

int Incremental_PI (int Encoder,int Target)
{
    static float error,Pwm,Last_error,Last_last_error;
    error=Target-Encoder;                             
    Pwm+=Incremental_KP*(error-Last_error)+
    Incremental_KI*error +
    Incremental_KD*(error-2*Last_error+Last_last_error);  
    Last_error=error;                                  
    Last_last_error = Last_error;  //???????
    return Pwm;                                          
}
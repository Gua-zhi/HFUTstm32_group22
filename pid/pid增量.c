int Incremental_PID (int Encoder,int Target)
{    
    static float error,Pwm,Last_error,Last_last_error;  
    error = Target - Encoder;                                      
    Pwm += Incremental_KP*(error-Last_error)+Incremental_KI*error +Incremental_KD*(error-2*Last_error+Last_last_error); 
    Last_last_error = Last_error;  
    Last_error = error;            
    return Pwm;
}
void APP_avoid(void)
{
    if(g_distance<250)
    {
        Move_X = -10;
        my_delay(1);
        Move_X = 0;
        Move_Z = 400;
        my_delay(2);
    }
    else
    {
        Move_X = 15;
        Move_Z = 0;
    }
}
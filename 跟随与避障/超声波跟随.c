void App_Change_Car(void)
{
    
    if(g_newcarstate != enSTOP && g_newcarstate != enAvoid && g_newcarstate != enFollow)
    {
        return;
    }
    
    if(g_distance == 0)
    {
        return;
    }
    if(g_distance >20 && g_distance<100)
    {
        g_newcarstate = enAvoid;
    }
    else if(g_distance >240 && g_distance<300)
    {
        g_newcarstate = enFollow;
    }
    else
    {
        g_newcarstate = enSTOP;
    }
}
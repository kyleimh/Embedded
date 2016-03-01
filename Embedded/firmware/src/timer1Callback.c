#include "app_public.h"
#include "usart.h"

short change = 0;

void vTimerCallback( TimerHandle_t pxTimer )
{
    MESSAGE msg;
    msg.id = 0x30;
    msg.msg = 0x31;
    msg.data1 = 0x30;
    msg.data2 = 0x31;
    
    if( change == 0 )
    {
        msg.id = 0x33;
        change = 1;
    }
    else
    {
        msg.id = 0x30;
        change = 0;
    }
    //USART_send(msg);
}

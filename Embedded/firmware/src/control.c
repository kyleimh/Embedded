#include "control.h"

CONTROL_DATA controlData;
short counter = 0;
MESSAGE ctrlMsgRecv;
MESSAGE ctrlMsgSend;

void CONTROL_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    controlData.state = CONTROL_STATE_INIT;
    
    if (!(controlQueue = xQueueCreate(20, sizeof(MESSAGE)))) 
    {
        outputEvent(CONTROL_QUEUE_NOT_CREATED);
    } 
    else 
    {
        outputEvent(CONTROL_QUEUE_CREATED);
    }
    
    //Initialize the USART communications
    USART_init();
}

void CONTROL_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( controlData.state )
    {
        /* Application's initial state. */
        case CONTROL_STATE_INIT:
        {
            controlData.state = CONTROL_STATE_RUN;
            SYS_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_A, 0x8);
            break;
        }

        case CONTROL_STATE_RUN:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            } 
            else 
            {
                outputEvent(CONTORL_QUEUE_RECEIVED);
                uint8_t mask = 0x08;
                if ( ctrlMsgRecv.id == 0x30 ) 
                {
                    if( ctrlMsgRecv.data1 == 0x0 )
                    {
                        SYS_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_A, mask);
                    }
                    else if( ctrlMsgRecv.data1 == 0x31 )
                    {
                        SYS_PORTS_Set(PORTS_ID_0, PORT_CHANNEL_A, mask, mask);
                    }
                } 
                else if ( ctrlMsgRecv.id == 0x32 ) 
                {
                    counter++;
                    if(counter > 0)
                    {
                        ctrlMsgSend.id    = 0x31;
                        ctrlMsgSend.msg   = ctrlMsgRecv.msg;
                        ctrlMsgSend.data1 = ctrlMsgRecv.data1;
                        ctrlMsgSend.data2 = ctrlMsgRecv.data2;
                        USART_send(ctrlMsgSend);
                        counter = 0;
                    }
                }else if ( ctrlMsgRecv.id == 0x58 ) 
                {
                    if(ctrlMsgRecv.msg == 0x40){ //Set debug scheme to ALL
                        debugScheme = 0;
                    }else if(ctrlMsgRecv.msg == 0x41){ //Set debug scheme to only ADC
                        debugScheme = 1;
                    }else if(ctrlMsgRecv.msg == 0x42){ //Set debug scheme to only LINE
                        debugScheme = 2;
                    }
                }
                else
                {
                    SYS_PORTS_Set(PORTS_ID_0, PORT_CHANNEL_A, mask, mask);
                }
            }
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
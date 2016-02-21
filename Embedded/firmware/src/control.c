#include "control.h"

CONTROL_DATA controlData;

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
            break;
        }

        case CONTROL_STATE_RUN:
        {
            break;
            MESSAGE message;
            if (xQueueReceive(controlQueue, &message, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            } 
            else 
            {
                outputEvent(CONTORL_QUEUE_RECEIVED);
                uint8_t mask = 0x08;
                //if the data portion of the message is 0x30 then turn the led off
                //else turn the led on
                if (message.msg == 0x30) 
                {
                    SYS_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_A, mask);
                } 
                else if (message.msg == 0x32) 
                {
                    if (xQueueSendToBack(motorQueue, (void *)&message, (TickType_t)10) == pdFALSE) 
                    {
                        outputEvent(MOTOR_QUEUE_FULL);
                    } 
                    else
                    {
                        outputEvent(MOTOR_QUEUE_ITEM_SENT);
                    }
                }
                else
                {
                    SYS_PORTS_Set(PORTS_ID_0, PORT_CHANNEL_A, mask, mask);
                }
                //send the message received back to the USART
                USART_send(message);
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
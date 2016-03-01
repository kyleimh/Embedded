#include "motor.h"

MOTOR_DATA motorData;

void MOTOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    motorData.state = MOTOR_STATE_INIT;
    
    if (!(motorQueue = xQueueCreate(10, sizeof(MESSAGE)))) 
    {
        outputEvent(MOTOR_QUEUE_NOT_CREATED);
    } 
    else 
    {
        outputEvent(MOTOR_QUEUE_CREATED);
    }
}

void MOTOR_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( motorData.state )
    {
        /* Application's initial state. */
        case MOTOR_STATE_INIT:
        {
            motorData.state = MOTOR_STATE_RUN;
            break;
        }

        case MOTOR_STATE_RUN:
        {
            MESSAGE message;
            if (xQueueReceive(motorQueue, &message, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(MOTOR_QUEUE_EMPTY);
            } 
            else 
            {
                /* Send response "MOTR" */
                message.id    = 'M';
                message.msg   = 'O';
                message.data1 = 'T';
                message.data2 = 'R';
                USART_send(message);
                outputEvent(MOTOR_QUEUE_RECEIVED);
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
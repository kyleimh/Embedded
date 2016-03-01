#include "sensor.h"

SENSOR_DATA sensorData;
MESSAGE sensorMsg;

void SENSOR_Initialize ( void )
{
    sensorData.state = SENSOR_STATE_INIT;
    
    xQueueADC = xQueueCreate( 1, sizeof(uint16_t) );
    if( xQueueADC == 0 )
    {
        outputEvent(ADC_QUEUE_NOT_CREATED);
    }
    else
    {
        outputEvent(ADC_QUEUE_CREATED);
    }
}

void SENSOR_Tasks ( void )
{
   /* Check the application's current state. */
    switch ( sensorData.state )
    {
        /* Application's initial state. */
        case SENSOR_STATE_INIT:
        {
            DRV_ADC_Open();
            sensorData.state = SENSOR_STATE_RUN;
            break;
        }
        case SENSOR_STATE_RUN:
        {
            uint16_t queueByte;
            if( xQueueReceive( xQueueADC, &queueByte, portMAX_DELAY ) != pdTRUE )
            {
                outputEvent(ADC_QUEUE_EMPTY);
            }
            else
            {
                sensorMsg.id    = 0x32; //From sensor thread(3), To control thread(2)
                sensorMsg.msg   = 0x0;
                sensorMsg.data1 = queueByte & 0xFF;          //Lower byte
                sensorMsg.data2 = (queueByte & 0xFF00) >> 8; //Upper byte
                xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 );
                //outputEvent(RECEIVED_FROM_ADC_QUEUE);     
            }
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}
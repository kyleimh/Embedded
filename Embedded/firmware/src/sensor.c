#include "sensor.h"

SENSOR_DATA sensorData;
MESSAGE sensorMsg;

void SENSOR_Initialize ( void )
{
    sensorData.state = SENSOR_STATE_INIT;
    
    sensorQueue = xQueueCreate( 16, sizeof(MESSAGE) );
    if( sensorQueue == 0 )
    {
        outputEvent(SENSOR_QUEUE_NOT_CREATED);
    }
    else
    {
        outputEvent(SENSOR_QUEUE_CREATED);
    }
    
    raw_cm = true;
    push = false;
    safe = true;
    wait = false;
    intersect = false;
    L2 = false;
    L1 = false;
    C = false;
    R1 = false;
    R2 = false;
    stopped = false;
    
    if(enableADC){
        DRV_ADC_Open();
    }else{
        DRV_ADC_Close();
    }            

    if(enableLineSensor){
        /* Create timer for line sensor */
        lineTimer = xTimerCreate("lineTimer", 8/portTICK_PERIOD_MS, pdFALSE, (void *)0, lineTimerCallback);
        if( lineTimer == NULL ){
            outputEvent(LINETIMER_NOT_CREATED);
        }else{
            if( xTimerStart( lineTimer, 0 ) != pdPASS ){
                outputEvent(LINETIMER_NOT_STARTED);
            }
        }
        readTimer = xTimerCreate("readTimer", 2/portTICK_PERIOD_MS, pdFALSE, (void *)0, readTimerCallback);        
        if( lineTimer == NULL ){
            outputEvent(READTIMER_NOT_CREATED);
        }
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
            sensorData.state = SENSOR_STATE_RUN;
            break;
        }
        case SENSOR_STATE_RUN:
        {
            MESSAGE sensorMsgRecv;
            if( xQueueReceive( sensorQueue, &sensorMsgRecv, portMAX_DELAY ) != pdTRUE )
            {
                outputEvent(SENSOR_QUEUE_EMPTY);
            }
            else
            {
                outputEvent(SENSOR_QUEUE_ITEM_READY);
                if(sensorMsgRecv.id == 0x34){
                    if(sensorMsgRecv.msg == 0x15){
                        raw_cm = false;
                    }
                    else if(sensorMsgRecv.msg == 0x16){
                        raw_cm = true;
                    }
                    else if(sensorMsgRecv.msg == 0x17){
                        push = !push;
                        if(push)
                            DRV_ADC_Close();
                        else
                            DRV_ADC_Open();
                    }
                    else if(sensorMsgRecv.msg == 0x18){
                        wait = !wait;
                    }
                }else if(sensorMsgRecv.id == 0x36){
                    sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                    sensorMsg.msg   = 0x22; // LineSensorData
                    sensorMsg.data1 = sensorMsgRecv.data1;
                    sensorMsg.data2 = 0x0;
                    xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 );
                    if(sensorMsgRecv.data1 == 0){
                        if(!stopped){
                            intersect = false;
                            stopped = true;                            
                            L2 = false;
                            L1 = false;
                            C = false;
                            R1 = false;
                            R2 = false;
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = STOP; // Stop
                            sensorMsg.data1 = 0x0;
                            sensorMsg.data2 = 0x0;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0xFF){
                        if(!intersect){
                            intersect = true;
                            stopped = false;
                            L2 = false;
                            L1 = false;
                            C = false;
                            R1 = false;
                            R2 = false;
                            sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                            sensorMsg.msg   = 0x24; // Line Intersection
                            sensorMsg.data1 = 0x0;
                            sensorMsg.data2 = 0x0;
                            xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0xC0){
                        if(!L2){  
                            intersect = false;
                            stopped = false;
                            L2 = true;
                            L1 = false;
                            C = false;
                            R1 = false;
                            R2 = false;
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = FORWARD; // Forward
                            sensorMsg.data1 = 10;
                            sensorMsg.data2 = 12;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0x30){
                        if(!L1){  
                            intersect = false;
                            stopped = false;
                            L2 = false;
                            L1 = true;
                            C = false;
                            R1 = false;
                            R2 = false;
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = FORWARD; // Forward
                            sensorMsg.data1 = 11;
                            sensorMsg.data2 = 12;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0x18){
                        if(!C){  
                            intersect = false;
                            stopped = false;
                            L2 = false;
                            L1 = false;
                            C = true;
                            R1 = false;
                            R2 = false;
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = FORWARD; // Forward
                            sensorMsg.data1 = 12;
                            sensorMsg.data2 = 12;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0x0C){
                        if(!R1){  
                            intersect = false;
                            stopped = false;
                            L2 = false;
                            L1 = false;
                            C = false;
                            R1 = true;
                            R2 = false;
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = FORWARD; // Forward
                            sensorMsg.data1 = 12;
                            sensorMsg.data2 = 11;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else if(sensorMsgRecv.data1 == 0x03){
                        if(!R2){  
                            intersect = false;
                            stopped = false;
                            L2 = false;
                            L1 = false;
                            C = false;
                            R1 = false;
                            R2 = true;                           
                            sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                            sensorMsg.msg   = FORWARD; // Forward
                            sensorMsg.data1 = 12;
                            sensorMsg.data2 = 10;
                            xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                        }
                    }else{
                        intersect = false;
                        stopped = false;
                        L2 = false;
                        L1 = false;
                        C = false;
                        R1 = false;
                        R2 = false;
                    }
                    
                }                
                else if(sensorMsgRecv.id == 0x35){
                    uint16_t dist = (sensorMsgRecv.data1 << 8) + sensorMsgRecv.data2;
                    if(dist < 100)
                        dist = 100;
                    else if(dist > 830)
                        dist = 830;
                    
                    uint16_t distCM = (4096 / (dist - 10));
                    
                    
                    if(distCM == 4 && safe){
                        safe = false;
                        sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                        sensorMsg.msg   = 0x20; // Not Safe
                        sensorMsg.data1 = 0x0;
                        sensorMsg.data2 = 0x0;
                        xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 ); 
                    }
                    else if(distCM > 4 && !safe){
                        safe = true;
                        sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                        sensorMsg.msg   = 0x21; // Safe
                        sensorMsg.data1 = 0x0;
                        sensorMsg.data2 = 0x0;
                        xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 ); 
                    }
                    if(raw_cm){
                        sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                        sensorMsg.msg   = 0x23; 
                        sensorMsg.data1 = distCM & 0xFF;          //Lower byte
                        sensorMsg.data2 = (distCM & 0xFF00) >> 8; //Upper byte
                        xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 ); 
                    }else{
                        sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                        sensorMsg.msg   = 0x23; 
                        sensorMsg.data1 = dist & 0xFF;          //Lower byte
                        sensorMsg.data2 = (dist & 0xFF00) >> 8; //Upper byte
                        xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 ); 
                    }
                    
                }
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
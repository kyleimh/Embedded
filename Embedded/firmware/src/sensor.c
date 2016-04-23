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
    lineState = NONE;
    line_correction = true;
    
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
                if(sensorMsgRecv.id == 0x31){
                    if(sensorMsgRecv.msg == 50){
                        line_correction = sensorMsgRecv.data1;
                    }                
                }else if(sensorMsgRecv.id == 0x34){
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
                    if(line_correction){
                        sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                        sensorMsg.msg   = 0x22; // LineSensorData
                        sensorMsg.data1 = sensorMsgRecv.data1;
                        sensorMsg.data2 = 0x0;
                        USART_send(sensorMsg);
//                      xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 );
                        if(sensorMsgRecv.data1 == 0 || lineState == OFF_LINE){
                            if(lineState != STOPPED && lineState != OFF_LINE){
                                if(prevLine == 0x80){
                                    lineState = OFF_LINE;
                                    sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                    sensorMsg.msg   = FORWARD; // Forward
                                    sensorMsg.data1 = 200;
                                    sensorMsg.data2 = 204;
                                }else if(prevLine == 0x01){
                                    lineState = OFF_LINE;
                                    sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                    sensorMsg.msg   = FORWARD; // Forward
                                    sensorMsg.data1 = 204;
                                    sensorMsg.data2 = 200;
                                }else{
                                    lineState = STOPPED;
                                    sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                    sensorMsg.msg   = STOP; // Stop
                                    sensorMsg.data1 = 0x0;
                                    sensorMsg.data2 = 0x0;
                                }
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }else{
                                if(sensorMsgRecv.data1 == 0x18){
                                    lineState = NONE;
                                    sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                    sensorMsg.msg   = FORWARD; // Forward
                                    sensorMsg.data1 = 203;
                                    sensorMsg.data2 = 203;
                                    xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                                }
                            }
                        }else if(sensorMsgRecv.data1 == 0xFF){
                            if(lineState != INTERSECT){
                                lineState = INTERSECT;
                                sensorMsg.id    = 0x13; //To control thread(1), From sensor thread(3)
                                sensorMsg.msg   = 0x24; // Line Intersection
                                sensorMsg.data1 = 0x0;
                                sensorMsg.data2 = 0x0;
                                xQueueSend( controlQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else if(sensorMsgRecv.data1 == 0xC0){
                            if(lineState != L2){  
                                lineState = L2;
                                sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                sensorMsg.msg   = FORWARD; // Forward
                                sensorMsg.data1 = 201;
                                sensorMsg.data2 = 203;
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else if(sensorMsgRecv.data1 == 0x30){
                            if(lineState != L1){  
                                lineState = L1;
                                sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                sensorMsg.msg   = FORWARD; // Forward
                                sensorMsg.data1 = 202;
                                sensorMsg.data2 = 203;
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else if(sensorMsgRecv.data1 == 0x18){
                            if(lineState != C){  
                                lineState = C;
                                sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                sensorMsg.msg   = FORWARD; // Forward
                                sensorMsg.data1 = 203;
                                sensorMsg.data2 = 203;
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else if(sensorMsgRecv.data1 == 0x0C){
                            if(lineState != R1){  
                                lineState = R1;
                                sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                sensorMsg.msg   = FORWARD; // Forward
                                sensorMsg.data1 = 203;
                                sensorMsg.data2 = 202;
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else if(sensorMsgRecv.data1 == 0x03){
                            if(lineState != R2){  
                                lineState = R2;
                                sensorMsg.id    = SENSOR_TO_MOTOR; //To motor thread(2), From sensor thread(3)
                                sensorMsg.msg   = FORWARD; // Forward
                                sensorMsg.data1 = 203;
                                sensorMsg.data2 = 201;
                                xQueueSend( motorQueue, &sensorMsg, (TickType_t)0 );
                            }
                        }else{
                            lineState = NONE;
                        }
                        prevLine = sensorMsgRecv.data1;
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
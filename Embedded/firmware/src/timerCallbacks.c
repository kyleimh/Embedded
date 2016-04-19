#include "app_public.h"

void lineTimerCallback( TimerHandle_t pxTimer )
{
    
    outputEvent(LINE_READING_START);
    
    if(!wait)
        if( xTimerStart( lineTimer, 0 ) != pdPASS )
            outputEvent(LINETIMER_NOT_STARTED);
   
    
    //Set Pins to digital output and HIGH
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_11);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_12);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_B, PORTS_BIT_POS_13);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_G, PORTS_BIT_POS_8);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_A, PORTS_BIT_POS_10);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_0);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_F, PORTS_BIT_POS_1);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, PORT_CHANNEL_D, PORTS_BIT_POS_6);
    
    
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_10);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_0);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_1);
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_6);
    
    //Start timer to charge cap and read data
    validData = false;
    if( xTimerStart( readTimer, 0 ) != pdPASS ){
        outputEvent(READTIMER_NOT_STARTED);
    }
}

void readTimerCallback( TimerHandle_t pxTimer )
{
    if(validData){
        outputEvent(LINE_VALID_DATA);
        
        //Read pins and send message to sensor queue
        bool pin1 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_11);
        bool pin2 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_13);
        bool pin3 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_B, PORTS_BIT_POS_12);
        bool pin4 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
        bool pin5 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_10);
        bool pin6 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_0);
        bool pin7 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_1);
        bool pin8 = SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_6);
        
        uint8_t line = 0;      
        if(pin1)
            line += 0x1;
        if(pin2)
            line += 0x2;
        if(pin3)
            line += 0x4;
        if(pin4)
            line += 0x8;
        if(pin5)
            line += 0x10;
        if(pin6)
            line += 0x20;
        if(pin7)
            line += 0x40;
        if(pin8)
            line += 0x80;        
        
        MESSAGE lineMsg;
        lineMsg.id = 0x36;
        lineMsg.msg = 0x12;
        lineMsg.data1 = line;
        lineMsg.data2 = 0x0;    

        if( xQueueSendToBack(sensorQueue, (void *)&lineMsg, (TickType_t)0) != pdPASS )
        {
            outputEvent(SENSOR_QUEUE_FULL);
        }
        else
        {
            outputEvent(SENT_TO_SENSOR_QUEUE);
        }
        validData = false;
        
    }else{
        outputEvent(LINE_CAP_CHARGED);
        
        //Set Pins to digital input
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_B, PORTS_BIT_POS_11);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_B, PORTS_BIT_POS_13);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_B, PORTS_BIT_POS_12);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_G, PORTS_BIT_POS_8);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_A, PORTS_BIT_POS_10);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_F, PORTS_BIT_POS_0);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_F, PORTS_BIT_POS_1);
        SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_D, PORTS_BIT_POS_6);
       
        
        //Reset timer
        validData = true;
        if( xTimerReset( readTimer, 0 ) != pdPASS ){
           outputEvent(READTIMER_NOT_RESET);
        }
    }   
}


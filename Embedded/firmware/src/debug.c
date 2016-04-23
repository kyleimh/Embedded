#include "debug.h"

void outputEvent( uint8_t value  )
{    
    
    switch(debugScheme){
        case 1:{
            if(value == ADC_INTERRUPT_OCCURRED || value == SENT_TO_SENSOR_QUEUE){
                writeDebug(value);
            }            
            break;
        }
        case 2:{
            if(value == LINE_READING_START || value == LINE_CAP_CHARGED || value == LINE_VALID_DATA){
                writeDebug(value);
            }
            break;
        }
        default:{
            writeDebug(value);
            break;
        }
    }
}


void writeDebug( uint8_t value  ){
    //Get current value from Port E
    uint32_t portE = SYS_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_E);
    //Save Port E bits 8-31
    portE = value | (portE & 0xFFFFFF00);
    //Write Value to PortE
    SYS_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_E, portE);    
}



void debugToUART(uint8_t value){
    MESSAGE debugMsg;
    
    debugMsg.id    = 0x48;
    debugMsg.msg   = value;
    debugMsg.data1 = 0;
    debugMsg.data2 = 0;
    
    USART_send(debugMsg);
    
}
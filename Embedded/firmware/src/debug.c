#include "debug.h"

void outputEvent( uint32_t value  )
{    
    //Get current value from Port E
    uint32_t portE = SYS_PORTS_Read(PORTS_ID_0, PORT_CHANNEL_E);
    //Save Port E bits 8-31
    value = value | (portE & 0xFFFFFF00);
    //Write Value to PortE
    SYS_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_E, value);
    
//    if(value >= 130)
//        _general_exception_handler();
}

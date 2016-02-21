#include "usart.h"

void USART_init()
{
    if (!(transmitQueue = xQueueCreate(10, sizeof(MESSAGE)))) 
    {
        outputEvent(TRANSMIT_QUEUE_NOT_CREATED);
    } 
    else 
    {
        outputEvent(TRANSMIT_QUEUE_CREATED);
    }
}

void USART_send(MESSAGE message)
{
    if (xQueueSendToBack(transmitQueue, (void *)&message, (TickType_t)10) == pdFALSE) 
    {
        outputEvent(TRANSMIT_QUEUE_FULL);
    } 
    else 
    {
        outputEvent(TRANSMIT_QUEUE_ITEM_READY);
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
}

#include "control.h"

CONTROL_DATA controlData;
short counter = 0;
MESSAGE ctrlMsgRecv;
MESSAGE ctrlMsgSend;

void CONTROL_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    controlData.state = CONTROL_STATE_INIT;
    /*Set the rover mode to cleanup*/
    //controlData.mode = CONTROL_MODE_CLEAN;
    /*Set the rover mode to deliver*/
    controlData.mode = CONTROL_MODE_DELIVER;
    controlData.delivery_turn = 0;
    controlData.delivery_intctn_ct = 0;
    controlData.out = false;
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

void set_line_correction(bool v) {
    ctrlMsgSend.id = 0x31;
    ctrlMsgSend.msg = 50;
    if (v) {
        ctrlMsgSend.data1 = 1;
    } else {
        ctrlMsgSend.data1 = 0;
    }
    ctrlMsgSend.data2 = 0;
    xQueueSend(sensorQueue, &ctrlMsgSend, (TickType_t) 0);
}

void forward()
{
    ctrlMsgSend.id = 0x21;
    ctrlMsgSend.msg = FORWARD;
    ctrlMsgSend.data1 = 12;
    ctrlMsgSend.data2 = 0x00;
    xQueueSend( motorQueue, &ctrlMsgSend, (TickType_t)0 );
    while (true) {
        if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
        {
            outputEvent(CONTROL_QUEUE_EMPTY);
        }
        else
        {
            if (ctrlMsgRecv.id  == 0x12){
                if (ctrlMsgRecv.msg == 100) { //if done message
//                    debugToUART(101);
                    break;
               }
            }            
        }
    }
}

void backward()
{
    ctrlMsgSend.id = 0x21;
    ctrlMsgSend.msg = BACKWARD;
    ctrlMsgSend.data1 = 15;
    ctrlMsgSend.data2 = 0x00;
    xQueueSend( motorQueue, &ctrlMsgSend, (TickType_t)0 );
    while (true) {
        if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
        {
            outputEvent(CONTROL_QUEUE_EMPTY);
        }
        else
        {
            if (ctrlMsgRecv.id  == 0x12){
                if (ctrlMsgRecv.msg == 100) { //if done message
//                    debugToUART(102);
                    break;
               }
            }            
        }
    }
}

void stop()
{
    set_line_correction(false);
    ctrlMsgSend.id = 0x21;
    ctrlMsgSend.msg = STOP;
    ctrlMsgSend.data1 = 0x00;
    ctrlMsgSend.data2 = 0x00;
    xQueueSend( motorQueue, &ctrlMsgSend, (TickType_t)0 );
}

/**
 * 
 * ctrlMessageSend data not preserved through call
 * ctrlMessageRecv data not preserved through call
 */
void turn_right(uint8_t d)
{
    ctrlMsgSend.id = 0x21;
    ctrlMsgSend.msg = TURN_RIGHT;
    ctrlMsgSend.data1 = d;
    ctrlMsgSend.data2 = 0x00;
    xQueueSend( motorQueue, &ctrlMsgSend, (TickType_t)0 );
    while (true) {
        if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
        {
            outputEvent(CONTROL_QUEUE_EMPTY);
        }
        else
        {
            if (ctrlMsgRecv.id  == 0x12){
                if (ctrlMsgRecv.msg == 100) { //if done message
//                    debugToUART(103);
                    break;
               }
            }            
        }
    }
}

/**
 * 
 * ctrlMessageSend data not preserved through call
 * ctrlMessageRecv data not preserved through call
 */
void turn_left(uint8_t d)
{
    ctrlMsgSend.id = 0x21;
    ctrlMsgSend.msg = TURN_LEFT;
    ctrlMsgSend.data1 = d;
    ctrlMsgSend.data2 = 0x00;
    xQueueSend( motorQueue, &ctrlMsgSend, (TickType_t)0 );
    while (true) {
        if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
        {
            outputEvent(CONTROL_QUEUE_EMPTY);
        }
        else
        {
            if (ctrlMsgRecv.id  == 0x12){
                if (ctrlMsgRecv.msg == 100) { //if done message
                    debugToUART(104);
                    break;
               }
            }            
        }
    }
}

void CONTROL_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( controlData.state )
    {
        /* Application's initial state. */
        case CONTROL_STATE_INIT:
        {
            controlData.state = CONTROL_STATE_STANDBY;
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
                if ( ctrlMsgRecv.id == 0x00 ) 
                {
                    if( ctrlMsgRecv.data1 == 0x0 )
                    {
                        SYS_PORTS_Clear(PORTS_ID_0, PORT_CHANNEL_A, mask);
                    }
                    else if( ctrlMsgRecv.data1 == 0x1 )
                    {
                        SYS_PORTS_Set(PORTS_ID_0, PORT_CHANNEL_A, mask, mask);
                    }
                } else if ( ctrlMsgRecv.id == 0x31 ) 
                {
                    if (xQueueSendToBack(motorQueue, (void *)&ctrlMsgRecv, (TickType_t)10) == pdFALSE) 
                    {
                        outputEvent(MOTOR_QUEUE_FULL);
                    } 
                    else
                    {
                        outputEvent(MOTOR_QUEUE_ITEM_SENT);
                    }
                } 
                else if ( (ctrlMsgRecv.id & 0x40) == 0x40 ) 
                {
                        ctrlMsgSend.id    = 0x41;
                        ctrlMsgSend.msg   = ctrlMsgRecv.msg;
                        ctrlMsgSend.data1 = ctrlMsgRecv.data1;
                        ctrlMsgSend.data2 = ctrlMsgRecv.data2;
                        USART_send(ctrlMsgSend);
                        
                }else if ( ctrlMsgRecv.id  == 0x13 ) {
                    if(ctrlMsgRecv.msg == 0x24){
//                        debugToUART(99);
                        turn_left(90);
                    }
                }
                else
                {
                    ctrlMsgSend.id    = 0x41;
                    ctrlMsgSend.msg   = ctrlMsgRecv.msg;
                    ctrlMsgSend.data1 = ctrlMsgRecv.data1;
                    ctrlMsgSend.data2 = ctrlMsgRecv.data2;
                    USART_send(ctrlMsgSend);
                    SYS_PORTS_Toggle(PORTS_ID_0, PORT_CHANNEL_A, mask);
                }
            }
            break;
        }
        
        case CONTROL_STATE_ECHO:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            }
            else
            {
                ctrlMsgSend = ctrlMsgRecv;
                USART_send(ctrlMsgSend);
            }
            break;
        }
        
        case CONTROL_STATE_STANDBY:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            }
            else
            {
                if (controlData.mode == CONTROL_MODE_CLEAN)
                {
                    controlData.state = CONTROL_STATE_CLEANUP;
                }
                else
                {
                    controlData.state = CONTROL_STATE_DELIVER;
                }
            }
            break;
        }
        
        case CONTROL_STATE_DELIVER:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            }
            else
            {
                //if the rover is out for delivery it will 
                //accept only messages for control
                if ((ctrlMsgRecv.id & 0xf0) == 0x10) {
                    if (controlData.out) {
                        //only accept messages from sensors while out
                        if ((ctrlMsgRecv.id & 0x0f) == 0x03) {
                            //intersection detected
                            if (ctrlMsgRecv.msg == 0x24) {
                                //do things
                                if (controlData.delivery_intctn_ct <= 0) {
                                    //reached destination
                                    set_line_correction(false);
                                    controlData.state = CONTROL_STATE_RETURN;
                                    controlData.return_turns_passed = 0;
                                    backward();
                                    turn_right(180);
                                    set_line_correction(true);
                                } else {
                                    //still in transit
                                    controlData.delivery_intctn_ct--;
                                    if (controlData.delivery_intctn_ct <= 0) {
                                        //need to stop??
                                        if (controlData.delivery_turn) {
                                            set_line_correction(false);
                                            forward();
                                            turn_right(90);
                                            set_line_correction(true);
                                        } else {
                                            set_line_correction(false);
                                            forward();
                                            turn_left(90);
                                            set_line_correction(true);
                                            
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        //while the rover is docked only accept messages from pi
                        if ((ctrlMsgRecv.id & 0x0f) == 0x04) {   
                            if (ctrlMsgRecv.msg == 0x09) {
                                controlData.out = true;
                                debugToUART(150);
                                //set turn and intersection count
                                controlData.delivery_intctn_ct = ctrlMsgRecv.data1 - 5;
                                controlData.return_turns = ctrlMsgRecv.data1 - 5;
                                controlData.delivery_turn = ctrlMsgRecv.data2;
                                forward();
                                set_line_correction(true);
                            } else if (ctrlMsgRecv.msg == CONTROL_MSG_RESET) {
                                controlData.state = CONTROL_STATE_STANDBY;
                            }
                        }
                    }
                } else {
                    //route the message
                    if ((ctrlMsgRecv.id & 0xf0) == 0x40) {
                        //send to usart
                        USART_send(ctrlMsgRecv);
                    } else if ((ctrlMsgRecv.id & 0xf0) == 0x20) {
                        //send to motor
                        xQueueSend( motorQueue, &ctrlMsgRecv, (TickType_t)0 );
                    }    
                }                
            }
            break;
        }
        
        case CONTROL_STATE_RETURN:
        {
//            debugToUART(180);
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
//                debugToUART(4);
            }
            else
            {
                debugToUART(ctrlMsgRecv.id);
                //check if for control thread
                if ((ctrlMsgRecv.id & 0xf0) == 0x10) {
                    //only want sensor data
                    if ((ctrlMsgRecv.id & 0x0f) == 0x03) {
                        if (ctrlMsgRecv.msg == 0x24) {
                            debugToUART(controlData.return_turns_passed);
                            debugToUART(controlData.return_turns);
                            if (controlData.return_turns_passed == 0) {
                                //first intersection
                                if (controlData.delivery_turn) {
                                    set_line_correction(false);
                                    forward();
                                    turn_left(90);
                                    set_line_correction(true);
                                } else {
                                    set_line_correction(false);
                                    forward();
                                    turn_right(90);
                                    set_line_correction(true);
                                }
//                                debugToUART(105);
                                controlData.return_turns_passed++;
                            } else if (controlData.return_turns_passed == controlData.return_turns) {
                                //back home
                                stop();
                                controlData.state = CONTROL_STATE_DELIVER;
                                controlData.out = false;
                            } else {
                                controlData.return_turns_passed++;
                                if (controlData.return_turns_passed == controlData.return_turns) {
                                    set_line_correction(false);
                                    forward();
                                    turn_left(90);
                                    set_line_correction(true);
                                    
                                }
                            }
                        }
                    } 
                } else {
                    //route the message
                    if ((ctrlMsgRecv.id & 0xf0) == 0x40) {
                        //send to usart
                        USART_send(ctrlMsgRecv);
                    } else if ((ctrlMsgRecv.id & 0xf0) == 0x20) {
                        //send to motor
                        xQueueSend( motorQueue, &ctrlMsgRecv, (TickType_t)0 );
                    }
                }
            }
            break;
        }
        
        case CONTROL_STATE_CLEANUP:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            }
            else
            {
                //route the message
                if ((ctrlMsgRecv.id & 0xf0) == 0x40) {
                    //send to usart
                    USART_send(ctrlMsgRecv);
                } else if ((ctrlMsgRecv.id & 0xf0) == 0x20) {
                    //send to motor
                    xQueueSend( motorQueue, &ctrlMsgRecv, (TickType_t)0 );
                } else if ((ctrlMsgRecv.id & 0xf0) == 0x30) {
                    //send to sensors
                    xQueueSend( sensorQueue, &ctrlMsgRecv, (TickType_t)0 );
                }
            }
            break;
        }
        
        case CONTROL_STATE_TEST:
        {
            if (xQueueReceive(controlQueue, &ctrlMsgRecv, portMAX_DELAY) == pdFALSE) 
            {
                outputEvent(CONTROL_QUEUE_EMPTY);
            }
            else
            {
                if (ctrlMsgRecv.msg == CONTROL_MSG_RESET)
                {
                    controlData.state = CONTROL_STATE_STANDBY;
                }
            }
            ctrlMsgSend.id = controlData.state;
            ctrlMsgSend.msg = controlData.state;
            ctrlMsgSend.data1 = controlData.state;
            ctrlMsgSend.data2 = controlData.state;
            USART_send(ctrlMsgSend);
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
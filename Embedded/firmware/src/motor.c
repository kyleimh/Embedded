#include "motor.h"

void INIT_Rover_Motors() 
{
    //Initialize Motors
    //Set up proper pins as outputs (left)
        //pin that handles left motor enable - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_0); //direction
        //pin that handles left motor direction - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14); 
    
    //Keep motors off during initialize
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
            
    //Set up proper pins as outputs (right)
        //pin that handles right motor enable - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_1); //direction
        //pin that handles right motor direction - see data sheet
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1); 
    
    //set motors off to init
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);
    
    //Start OC drivers
    DRV_OC0_Enable(); 
    DRV_OC1_Enable(); 
    
    DRV_OC0_Start();    //Start the OC drivers that talk to the motor shield
    DRV_OC1_Start();
    
    PLIB_PORTS_PinDirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3);    
    PLIB_PORTS_PinClear (PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_3);
}

void move_Forward()
{
    //Make right motor go forward 
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, motor_data.R_speed);
    
    //Make left motor go forward
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, motor_data.L_speed);
    
    checkInstDone(DISTANCE);
}
void move_Backward()
{
    //Make right motor go backward 
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, motor_data.R_speed);
    
    //Make left motor go backward
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, motor_data.L_speed);
    
    checkInstDone(DISTANCE);
}
void move_Stop()
{
    //Make left motor stop 
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, 0);
    //Make right motor stop
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, 0);
    
//    sendDone(0);
}
//Handles turning the rovers, keep turning until a stop is called
void move_Turn_Right()
{
    //Make right motor go forward 
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, motor_data.R_speed);
    
    //Make left motor go forward
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, motor_data.L_speed);
    
    checkInstDone(DEGREE);
}
void move_Turn_Left()
{
    //Make right motor go forward 
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
    PLIB_OC_PulseWidth16BitSet(OC_ID_1, motor_data.R_speed);
    
    //Make left motor go forward
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, motor_data.L_speed);
    
    checkInstDone(DEGREE);
}

void checkInstDone(DISPLACE disOrdeg)
{
    if (inst_setup == false)
    {
        R_encoder = 0; 
        L_encoder = 0;
        
        //set up is now true 
        inst_setup = true;
        
    }
    
    if (Displace[disOrdeg] == 0)
        return;
    
    while ((R_encoder < Displace[disOrdeg] || L_encoder < Displace[disOrdeg]))
    {;}
    
    
    move_Stop();
    sendDone(2);
    //reset needed variables
    setDisplacement(disOrdeg, 0); //reset the distance
    inst_setup = false;
    motor_data.curState = STOP;
}

void setSpeed(int left_speed, int right_speed)
{
    switch (left_speed)
    {
        case 10:
            motor_data.L_speed = SLOW;
            break;
        case 11:
            motor_data.L_speed = AVERAGE;
            break;
        case 12:
            motor_data.L_speed = FAST;
            break;
    }
    
    switch (right_speed)
    {
        case 10:
            motor_data.R_speed = SLOW;
            break;
        case 11:
            motor_data.R_speed = AVERAGE;
            break;
        case 12:
            motor_data.R_speed = FAST;
            break;
    }
}

void test(int speed)
{
    //reset encoder count
    R_encoder = 0; 
    L_encoder = 0;
 
    move_Turn_Right(speed);
    
    while (R_encoder <  55) {;} //delay until count is right
    
   move_Stop();     
}

void setDisplacement(DISPLACE disOrdeg, int value)
{
    value = value - OFFSET; //correct the offset we add before sending
    
    if (disOrdeg == DISTANCE)
    {
//        Displace[DISTANCE] = (value * 17) - (value/5) + (value/10); //inches
        Displace[DISTANCE] = ((value * 180240)+ 34215) / 10000;
    }
    //given a degree
    else 
    {
//        Displace[DEGREE] = (value*3457)/5000 - 9.1429;
        Displace[DEGREE] = value;
        
        if(Displace[DEGREE] < 0)
        {
            Displace[DEGREE] = Displace[DEGREE]*-1;
        }
    }   
}


void MOTOR_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    motor_data.state = MOTOR_STATE_INIT; 
    
    INIT_Rover_Motors();
        
    R_encoder = 0; //initialized and incremented by ISR
    L_encoder = 0;
    
    Displace[DISTANCE] = 0; 
    Displace[DEGREE] = 0;
    motor_data.curState = STOP;
    
    inst_setup = false;
    
    motor_data.R_speed = SLOW - 200; 
    motor_data.L_speed = SLOW - 200;
    
    DRV_TMR0_Start();
    DRV_TMR1_Start();   //timer for encoders
    DRV_TMR2_Start();   //timer for encoders
    
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
    switch ( motor_data.state )
    {
        /* Application's initial state. */
        case MOTOR_STATE_INIT:
        {
            motor_data.state = MOTOR_STATE_RUN;
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
                outputEvent(MOTOR_QUEUE_RECEIVED);
                    switch (message.msg)
                    {
                        case STOP:
                            motor_data.curState = STOP;
                            move_Stop();
                            break;

                        case FORWARD:
                            if(message.id == SENSOR_TO_MOTOR)
                            {
                                //set speeds correctly
                                setSpeed(message.data1, message.data2);
                            }
                            else 
                            {
                                setDisplacement(DISTANCE, message.data1);
                            }  
                            motor_data.curState = FORWARD;
                            move_Forward();
                            break;

                        case BACKWARD:            
                            if(message.id == SENSOR_TO_MOTOR)
                            {
                                setSpeed(message.data1, message.data2);
                            }
                            else 
                            {
                                setDisplacement(DISTANCE, message.data1);
                            }  
                            motor_data.curState = BACKWARD;
                            move_Backward();                           
                            break;

                        case TURN_LEFT:
                            setDisplacement(DEGREE, message.data1);
                            motor_data.curState = TURN_LEFT;
                            move_Turn_Left();
                            break;

                        case TURN_RIGHT:
                            setDisplacement(DEGREE, message.data1);
                            motor_data.curState = TURN_RIGHT;
                            move_Turn_Right();
                            break;

                        default:
                            break;
                    }
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


//void setMotorState(MESSAGE message)
//{
//    //read message in properly
//    switch (message.msg)
//    {
//        case STOP:
//        move_Stop();
//        break;
//                        
//        case FORWARD:
//            if(message.id == SENSOR_TO_MOTOR)
//            {
//                //set speeds correctly
//            }
//            else 
//            {
//                setDisplacement(DISTANCE, message.data1);
//            }  
//            curState = FORWARD;
//            break;
//                        
//        case BACKWARD:            
//            if(message.id == SENSOR_TO_MOTOR)
//            {
//                //set speeds correctly
//            }
//            else 
//            {
//                setDisplacement(DISTANCE, message.data1);
//            }  
//            curState = BACKWARD;
//
//            break;
//                        
//        case TURN_LEFT:
//            setDisplacement(DEGREE, message.data1);
//            curState = TURN_LEFT;
//            break;
//                        
//        case TURN_RIGHT:
//            setDisplacement(DEGREE, message.data1);
//            curState = TURN_RIGHT;
//            break;
//                        
//        default:
//            break;
//    }
//}
//
//void executeState()
//{
//    if (motor_data.curState == FORWARD ||
//            motor_data.curState == FORWARD )
//    {checkInstDone(DISTANCE);}
//    else if (motor_data.curState == TURN_RIGHT ||
//            motor_data.curState == TURN_LEFT )
//    {checkInstDone(DEGREE);}
////    switch(motor_data.curState) 
////    {
////        case FORWARD:
////            move_Forward();
////            break;
////        case BACKWARD:
////            move_Backward();
////            break;
////        case TURN_LEFT:
////            move_Turn_Left();
////            break;
////        case TURN_RIGHT: 
////            move_Turn_Right();
////            break;
////        case STOP:
////            move_Stop();
////            break;
////        default:
////            break;
////    }
//}

void sendDone(int disOrdeg){
    MESSAGE mssge;
        mssge.id    = 0x12;
        mssge.msg   = DONE;
        mssge.data1 = disOrdeg;
        mssge.data2 = 0x0;
        if (xQueueSendToBack(controlQueue, (void *)&mssge, (TickType_t)10) == pdFALSE) 
        {
//            outputEvent;
        } 
        else
        {
//            outputEvent(CONTROL_QUEUE_ITEM_SENT);
        }
}
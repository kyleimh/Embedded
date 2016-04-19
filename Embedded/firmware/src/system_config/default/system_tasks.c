#include "system_config.h"
#include "system_definitions.h"
#include "control.h"
#include "motor.h"
#include "sensor.h"
 
static void _SYS_Tasks ( void );
static void _CONTROL_Tasks(void);
static void _MOTOR_Tasks(void);
static void _SENSOR_Tasks(void);

void SYS_Tasks ( void )
{
    /* Create OS Thread for Sys Tasks. */
    xTaskCreate((TaskFunction_t) _SYS_Tasks,
                "Sys Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for CONTROL Tasks. */
    xTaskCreate((TaskFunction_t) _CONTROL_Tasks,
                "CONTROL Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for MOTOR Tasks. */
    xTaskCreate((TaskFunction_t) _MOTOR_Tasks,
                "MOTOR Tasks",
                1024, NULL, 1, NULL);

    /* Create OS Thread for SENSOR Tasks. */
    xTaskCreate((TaskFunction_t) _SENSOR_Tasks,
                "SENSOR Tasks",
                1024, NULL, 1, NULL);

    /**************
     * Start RTOS * 
     **************/
    vTaskStartScheduler(); /* This function never returns. */
}


/*******************************************************************************
  Function:
    void _SYS_Tasks ( void )

  Summary:
    Maintains state machines of system modules.
*/

static void _SYS_Tasks ( void )
{
    while(1)
    {
        /* Maintain system services */
        SYS_DEVCON_Tasks(sysObj.sysDevcon);

        /* Maintain Device Drivers */

        /* Maintain Middleware */

        /* Task Delay */
    }
}


/*******************************************************************************
  Function:
    void _CONTROL_Tasks ( void )

  Summary:
    Maintains state machine of CONTROL.
*/

static void _CONTROL_Tasks(void)
{
    while(1)
    {
        CONTROL_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _MOTOR_Tasks ( void )

  Summary:
    Maintains state machine of MOTOR.
*/

static void _MOTOR_Tasks(void)
{
    while(1)
    {
        MOTOR_Tasks();
    }
}


/*******************************************************************************
  Function:
    void _SENSOR_Tasks ( void )

  Summary:
    Maintains state machine of SENSOR.
*/

static void _SENSOR_Tasks(void)
{
    while(1)
    {
        SENSOR_Tasks();
    }
}


/*******************************************************************************
 End of File
 */


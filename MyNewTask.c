/*
 * MyNewTask.c
 *
 *  Created on: Mar 27, 2025
 *      Author: gabyd
 */

#include "MyNewTask.h"
#include "LED.h"

osaEventId_t mMyEvents;
/* Global Variable to store our TimerID */
tmrTimerID_t myTimerID = gTmrInvalidTimerID_c;
/* Handler ID for task */
osaTaskId_t gMyTaskHandler_ID;
/* Local variable to store the current state of the LEDs */
static uint8_t ledsState = 0;

/* OSA Task Definition*/
OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE );

/* Main custom task */
void My_Task(osaTaskParam_t argument)
{
	osaEventFlags_t customEvent;
	myTimerID = TMR_AllocateTimer();
	while(1)
	{
		OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c,
				&customEvent);
		if( !gUseRtos_c && !customEvent)
		{
			break;
		}
		/* Depending on the received event */
		switch(customEvent){
		case gMyNewTaskEvent1_c:
			TMR_StartIntervalTimer(myTimerID, /*myTimerID*/
					1000, /* Timer's Timeout */
					myTaskTimerCallback, /* pointer to
myTaskTimerCallback function */
					NULL
			);
			TurnOffLeds(); /* Ensure all LEDs are turned off */
			break;
		case gMyNewTaskEvent2_c: /* Event called from myTaskTimerCallback */
			if(!ledsState) {
				TurnOnLeds();
				ledsState = 1;
			}
			else {
				TurnOffLeds();
				ledsState = 0;
			}
			break;
		case gMyNewTaskEvent3_c: /* Event to stop the timer */
			ledsState = 0;
			TurnOffLeds();
			TMR_StopTimer(myTimerID);
			break;
		default:
			break;
		}
	}
}

/* Function to init the task */
void MyTask_Init(void)
{
	mMyEvents = OSA_EventCreate(TRUE);
	/* The instance of the MAC is passed at task creaton */
	gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
}

/* This is the function called by the Timer each time it expires */
void myTaskTimerCallback(void *param)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent2_c);
}

/* Public function to send an event to stop the timer */
void MyTaskTimer_Stop(void)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent3_c);
}

/* Public function to send an event to start the timer */
void MyTaskTimer_Start(void)
{
	OSA_EventSet(mMyEvents, gMyNewTaskEvent1_c);

}

/* Control RGB según contador */
void UpdateRGBLEDs(uint8_t counter)
{
    switch(counter)
    {
        case 0:
            Led1On();   // Verde
            Led2Off();
            Led3Off();
            Led4Off();
            break;
        case 1:
            Led2On();   // Rojo
            Led1Off();
            Led3Off();
            Led4Off();
            break;
        case 2:
            Led3On();   // Azul
            Led1Off();
            Led2Off();
            Led4Off();
            break;
        case 3:
            // Todos encendidos (Blanco/RGB completo)
            Led1On();
            Led2On();
            Led3On();
            Led4On();
            break;
        default:
            TurnOffLeds();
            break;
    }
}

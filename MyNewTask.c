#include "MyNewTask.h"
#include "LED.h"

/* Variables */
static osaEventId_t mMyEvents;
static tmrTimerID_t mNetworkTimer = gTmrInvalidTimerID_c;
static uint8_t mCounter = 0;
static osaTaskId_t gMyTaskHandler_ID;

static uint32_t mCurrentInterval = 1000;  // Intervalo actual (ms)
static const uint32_t mIntervals[2] = {1000, 5000};  // 1s y 5s
static uint8_t mCurrentIntervalIndex = 0;  // Índice para el valor del timer

/* Callback del timer */
static void NetworkTimerCallback(void *param)
{
    OSA_EventSet(mMyEvents, gMyTask_TimerExpired_c);
}

void MyTask_ChangeTimer(void)
{
    mCurrentIntervalIndex = (mCurrentIntervalIndex + 1) % 2;  // Alterna entre 0 y 1
    mCurrentInterval = mIntervals[mCurrentIntervalIndex];

    // Reinicia el timer con el nuevo intervalo
    if(mNetworkTimer != gTmrInvalidTimerID_c)
    {
        TMR_StopTimer(mNetworkTimer);
        TMR_StartIntervalTimer(mNetworkTimer, mCurrentInterval, NetworkTimerCallback, NULL);
        UpdateRGBLEDs(mCounter);        // Actualizar LEDs
    }

    Serial_Print("[CONFIG] Intervalo cambiado a: %lu ms\n", mCurrentInterval);
}

/* Control RGB según contador */
extern void UpdateRGBLEDs(uint8_t counter)
{
    switch(counter)
    {

    // 3 verde, 2 rojo, 4azul
        case 0:							//Verde
        	Led3On();
        	Led1Off();
			Led2Off();
			Led4Off();
            break;
        case 1:							// Rojo
        	Led2On();
			Led1Off();
			Led3Off();
			Led4Off();
            break;
        case 2:							// Azul
        	Led4On();
			Led3Off();
			Led2Off();
            break;
        case 3:
        	Led2On();
			Led3On();
			Led4On();
            break;
        default:
        	TurnOffLeds();

    }
}


/* Tarea principal */
extern void My_Task(osaTaskParam_t arg)
{
    osaEventFlags_t event;

    while(1)
    {
        OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &event);

        if(event & gMyTask_StartNetwork_c)
        {
            if(mNetworkTimer == gTmrInvalidTimerID_c)
            {
                mNetworkTimer = TMR_AllocateTimer();
                TMR_StartIntervalTimer(mNetworkTimer, 3000, NetworkTimerCallback, NULL);
                mCounter = 0;
                UpdateRGBLEDs(mCounter);  // Estado inicial
            }
        }
        else if(event & gMyTask_TimerExpired_c)
        {
            mCounter = (mCounter + 1) % 4;  // Ciclar 0-3
            UpdateRGBLEDs(mCounter);        // Actualizar LEDs
        }
        else if(event & gMyTask_StopNetwork_c)
        {
            if(mNetworkTimer != gTmrInvalidTimerID_c)
            {
                TMR_StopTimer(mNetworkTimer);
                TMR_FreeTimer(mNetworkTimer);
                mNetworkTimer = gTmrInvalidTimerID_c;
            }
            TurnOffLeds();  // Apagar todos los LEDs
        }
    }
}

OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE );
/* API Pública */
void MyTask_Init(void)
{
    mMyEvents = OSA_EventCreate(TRUE);
    gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
    Serial_Print("Todo bien");
}

void MyTask_StartNetworkReporting(void)
{
    OSA_EventSet(mMyEvents, gMyTask_StartNetwork_c);
}

void MyTask_StopNetworkReporting(void)
{
    OSA_EventSet(mMyEvents, gMyTask_StopNetwork_c);
}


//Getters
uint8_t MyTask_GetCurrentCounter(void)
{
    // OSA_InterruptDisable();
    uint8_t contador = mCounter;
    // OSA_InterruptEnable();
    return contador;
}

void MyTask_SetCounterValue(uint8_t new_value)
{
    mCounter = new_value;
    UpdateRGBLEDs( mCounter);
}

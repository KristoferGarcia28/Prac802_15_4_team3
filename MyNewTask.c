#include "MyNewTask.h"

/* Variables */
static osaEventId_t mMyEvents;
static tmrTimerID_t mNetworkTimer = gTmrInvalidTimerID_c;
static uint8_t mCounter = 0;
static osaTaskId_t mTaskId;

static uint32_t mCurrentInterval = 3000;  // Intervalo actual (ms)
static const uint32_t mIntervals[2] = {3000, 5000};  // 3s y 5s
static uint8_t mCurrentIntervalIndex = 0;  // Índice del intervalo actual

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
    }

    Serial_Print("[CONFIG] Intervalo cambiado a: %lu ms\n", mCurrentInterval);
}

/* Control RGB según contador */
void UpdateRGBLEDs(uint8_t counter)
{
    switch(counter)
    {
        case 0:
            LED_TurnOnGreen();  // Verde
            LED_TurnOffRed();
            LED_TurnOffBlue();
            break;
        case 1:
            LED_TurnOnRed();   // Rojo
            LED_TurnOffGreen();
            LED_TurnOffBlue();
            break;
        case 2:
            LED_TurnOnBlue();  // Azul
            LED_TurnOffRed();
            LED_TurnOffGreen();
            break;
        case 3:
            LED_TurnOnRed();   // Blanco (RGB)
            LED_TurnOnGreen();
            LED_TurnOnBlue();
            break;
        default:
            LED_TurnOffAll();
    }
}


/* Tarea principal */
static void My_Task(osaTaskParam_t arg)
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
            LED_TurnOffAll();  // Apagar todos los LEDs
        }
    }
}

/* API Pública */
void MyTask_Init(void)
{
    mMyEvents = OSA_EventCreate(TRUE);
    mTaskId = OSA_TaskCreate(My_Task, NULL);
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
    /* 1. Validar y ajustar el rango */
    new_value = new_value % 4;  // Fuerza el rango 0-3
    mCounter = new_value;
}

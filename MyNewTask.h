#ifndef MYNEWTASK_H_
#define MYNEWTASK_H_

#include "TimersManager.h"
#include "LED.h"  // Asegúrate que tiene control RGB
#include "FunctionLib.h"
#include "fsl_os_abstraction.h"
#include "fsl_common.h"
#include "EmbeddedTypes.h"

/* Eventos */
#define gMyTask_StartNetwork_c    (1 << 0)
#define gMyTask_TimerExpired_c    (1 << 1)
#define gMyTask_StopNetwork_c     (1 << 2)
#define gMyTask_Switch3Pressed_c  (1 << 3)

/* Definiciones RGB */
#define LED_OFF     0x00
#define LED_RED     0x01
#define LED_GREEN   0x02
#define LED_BLUE    0x04
#define LED_WHITE   (LED_RED | LED_GREEN | LED_BLUE)

#define gMyTaskPriority_c 3
#define gMyTaskStackSize_c 400

void MyTask_StartNetworkReporting(void);
void MyTask_StopNetworkReporting(void);
void MyTask_Init(void);
uint8_t MyTask_GetCurrentCounter(void);
void MyTask_SetCounterValue(uint8_t new_value);
extern void UpdateRGBLEDs(uint8_t counter);
void MyTask_ChangeTimer(void);
bool_t MyTask_GetFlag(void);
void MyTask_SetFlag(bool_t new_value);

/* Declaración pública de la función de la tarea */
extern void My_Task(osaTaskParam_t argument);  // 'extern' es clave aquí
#endif /* MYNEWTASK_H_ */

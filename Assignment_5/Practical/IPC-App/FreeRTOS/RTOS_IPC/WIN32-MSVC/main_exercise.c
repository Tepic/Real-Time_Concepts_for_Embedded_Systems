/******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html

/* Standard includes. */
#include <stdio.h>
#include <assert.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Other Includes */
#include "workerTask.h"
//#include "controllerTask.h"
#include "print.h"
#include "config.h"
#include "event_groups.h"

#define mainNUMBER_OF_SEMAPHORS					( 3 )

/* TODO: output frequencey
*/
#define workersUSELESS_CYCLES_PER_TIME_UNIT		( 1000000UL)

#define TASK_SCHEDULER_PRIORITY 6
#define TASK_SCHEDULER_TICK_TIME 1

#define BIT_4	( 1 << 4 )	// Controller 1 is ACTIVE
#define BIT_3	( 1 << 3 )	// Controller 2 is ACTIVE
#define BIT_2	( 1 << 2 )	// Queue 1 is ready
#define BIT_1	( 1 << 1 )	// Queue 2A is ready
#define BIT_0	( 1 << 0 )	// Queue 2B is ready

/* TODO: output frequencey
*/
TickType_t currentTime = 0;
const TickType_t SCHEDULER_OUTPUT_FREQUENCY_MS = 1 / portTICK_PERIOD_MS;

/* TODO */
static gll_t* global_taskList;
static QueueHandle_t xQueue1, xQueue2A, xQueue2B;
static QueueHandle_t xQueueTickCount;

/* Declare a variable to hold the created event group. */
EventGroupHandle_t xEventGroup;

static void vUselessLoad(uint32_t ulCycles);
static void prvTaskSchedulerHandler(void *pvParameters);
static void prvTaskSensor1(void *pvParameters);
static void prvTaskSensor2(void *pvParameters);
static void prvTaskSensor3(void *pvParameters);
static void prvTaskControl(void *pvParameters);

void vInitialize(TaskFunction_t schedulerHandler,
	TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4,
	TaskFunction_t taskHandler_5);

/*-----------------------------------------------------------*/
#define max(x,y) ((x) >= (y)) ? (x) : (y)

void main_exercise(void)
{
	/* TODO */
	vPrintStringLn("Starting the application...");
	vInitialize(prvTaskSchedulerHandler, prvTaskSensor1, prvTaskSensor2, prvTaskSensor3, prvTaskControl, prvTaskControl);
	vTaskStartScheduler();

	while (true)
	{}
}
/*-----------------------------------------------------------*/


/* TODO */
/*-----------------------------------------------------------*/
static void vUselessLoad(uint32_t ulTimeUnits) {
	uint32_t ulUselessVariable = 0;

	for (uint32_t i = 0; i < ulTimeUnits * workersUSELESS_CYCLES_PER_TIME_UNIT; i++)
	{
		ulUselessVariable = 0;
		ulUselessVariable = ulUselessVariable + 1;
	}
}

// Task Scheduler Process
void prvTaskSchedulerHandler(void *pvParameters) 
{	
	uint32_t uCurrentTickCount = 0;

	gll_t* pTaskList = (gll_t*)pvParameters;
	WorkerTask_t* pWorkerTask = NULL;

	EventBits_t uxBits;
	while (true)
	{
		uxBits = xEventGroupGetBits(xEventGroup);
		for (uint8_t uIndex = 0; uIndex < pTaskList->size; uIndex++)
		{

			pWorkerTask = gll_get(pTaskList, uIndex);

			if (pWorkerTask->xHandle != NULL)
			{
				if ((uCurrentTickCount >= pWorkerTask->uReleaseTime) && // Check if the task is released 
					((uCurrentTickCount % pWorkerTask->uPeriod) == (pWorkerTask->uReleaseTime % pWorkerTask->uPeriod)))
				{
					vTaskResume(pWorkerTask->xHandle);
				}
			}
			else if (uIndex == 3 && (uxBits & BIT_4)!=0)
			{
				xEventGroupClearBits(xEventGroup, BIT_4);
				xEventGroupSetBits(xEventGroup, BIT_3);
			}
		}

		++uCurrentTickCount;
		xQueueOverwrite(xQueueTickCount, (void *)&uCurrentTickCount);
		vTaskDelay(TASK_SCHEDULER_TICK_TIME * SCHEDULER_OUTPUT_FREQUENCY_MS);
	}
}

static void prvTaskSensor1(void * pvParameters)
{
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTask2'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* sensorTask_1 = (WorkerTask_t*)pvParameters;

	QueueHandle_t* pQueueHandle = gll_get(sensorTask_1->pUsedQueueList, 0);  // Queue_1

	EventBits_t uxBits;
	while (true)
	{
		if(++sensorTask_1->uCurrentValue >= sensorTask_1->uEndValue)
		{
			sensorTask_1->uCurrentValue = sensorTask_1->uStartValue;
		}
		xQueueOverwrite(*(pQueueHandle), &sensorTask_1->uCurrentValue);
		uxBits = xEventGroupSetBits(xEventGroup, BIT_2);

		// Task completed one period, suspend it
		vTaskSuspend(sensorTask_1->xHandle);
	}
}

static void prvTaskSensor2(void * pvParameters)
{
	WorkerTask_t* sensorTask_2 = (WorkerTask_t*)pvParameters;

	QueueHandle_t* pQueueHandle = gll_get(sensorTask_2->pUsedQueueList, 0);  // Queue_2A

	EventBits_t uxBits;
	while (true)
	{
		if(++sensorTask_2->uCurrentValue >= sensorTask_2->uEndValue)
		{
			sensorTask_2->uCurrentValue = sensorTask_2->uStartValue;
		}
		xQueueOverwrite(*(pQueueHandle), &sensorTask_2->uCurrentValue);
		uxBits = xEventGroupSetBits(xEventGroup, BIT_1);

		// Task completed one period, suspend it
		vTaskSuspend(sensorTask_2->xHandle);
	}
}

static void prvTaskSensor3(void * pvParameters)
{
	WorkerTask_t* sensorTask_3 = (WorkerTask_t*)pvParameters;

	QueueHandle_t* pQueueHandle = gll_get(sensorTask_3->pUsedQueueList, 0);  // Queue_2B

	EventBits_t uxBits;
	while (true)
	{
		if(++sensorTask_3->uCurrentValue >= sensorTask_3->uEndValue)
		{
			sensorTask_3->uCurrentValue = sensorTask_3->uStartValue;
		}
		xQueueOverwrite(*(pQueueHandle), &sensorTask_3->uCurrentValue);
		uxBits = xEventGroupSetBits(xEventGroup, BIT_0);

		// Task completed one period, suspend it
		vTaskSuspend(sensorTask_3->xHandle);
	}
}

static void prvTaskControl(void * pvParameters)
{
	uint32_t uCurrentTickCount = 0;

	WorkerTask_t* controlTask = (WorkerTask_t*)pvParameters;

	uint16_t uQueuedValue1 = 0;
	uint16_t uQueuedValue2 = 0;
	uint16_t uQueuedValue3 = 0;

	QueueHandle_t*	pQueueHandle = gll_get(controlTask->pUsedQueueList, 0);  // Queue_1
	QueueHandle_t xQueue1 = *pQueueHandle;
					pQueueHandle = gll_get(controlTask->pUsedQueueList, 1);  // Queue_2A
	QueueHandle_t xQueue2A = *pQueueHandle;
					pQueueHandle = gll_get(controlTask->pUsedQueueList, 2);  // Queue_2B
	QueueHandle_t xQueue2B = *pQueueHandle;

	EventBits_t uxBits;
	while (true)
	{
		uQueuedValue1 = uQueuedValue2 = uQueuedValue3 = 0;

		xQueuePeek(xQueueTickCount, &(uCurrentTickCount), (TickType_t)0);


		if (uCurrentTickCount >= 2001 && controlTask->uStartValue == 1)
		{
			vPrintString("Controller 1 has had an error at ");
			vPrintUnsignedInteger(uCurrentTickCount);
			vPrintStringLn(".");
						
			controlTask->xHandle = NULL;
			vTaskDelete(controlTask->xHandle);
			return;
		}

		uxBits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4, pdFALSE, pdFALSE, (TickType_t)0);

		if (((uxBits & (BIT_4)) && controlTask->uStartValue == 1) || ((uxBits & (BIT_3)) && controlTask->uStartValue == 2))
		{
			//if (	(uxBits & (BIT_2))		&&		 (	(uxBits & (BIT_1)) || (uxBits & (BIT_0))	)		)
			//{
				if ((uxBits & (BIT_2)) && ((uxBits & (BIT_1))))
				{
					xQueueReceive(xQueue1, &(uQueuedValue1), (TickType_t)0);
					xQueueReceive(xQueue2A, &(uQueuedValue2), (TickType_t)0);
					uxBits = xEventGroupClearBits(xEventGroup, BIT_1 | BIT_2);

					vPrintString("Controller ");
					vPrintUnsignedInteger(controlTask->uStartValue);
					vPrintString(" has received sensor data at: ");
					vPrintUnsignedInteger(uCurrentTickCount);
					vPrintString("; Sensor1: ");
					vPrintUnsignedInteger(uQueuedValue1);
					vPrintString("; Sensor2: ");
					vPrintUnsignedInteger(uQueuedValue2);
					vPrintStringLn("");
				}
				else if ((uxBits & (BIT_2)) && ((uxBits & (BIT_0))))
				{
					xQueueReceive(xQueue1, &(uQueuedValue1), (TickType_t)0);
					xQueueReceive(xQueue2B, &(uQueuedValue3), (TickType_t)0);
					uxBits = xEventGroupClearBits(xEventGroup, BIT_0 | BIT_2);

					vPrintString("Controller ");
					vPrintUnsignedInteger(controlTask->uStartValue);
					vPrintString(" has received sensor data at: ");
					vPrintUnsignedInteger(uCurrentTickCount);
					vPrintString("; Sensor1: ");
					vPrintUnsignedInteger(uQueuedValue1);
					vPrintString("; Sensor2: ");
					vPrintUnsignedInteger(uQueuedValue3);
					vPrintStringLn("");

				}
			//}
		}

		// Task completed one period, suspend it
		vTaskSuspend(controlTask->xHandle);
	}
}

void vInitialize(
	TaskFunction_t schedulerHandler,
	TaskFunction_t taskHandler_Sensor1,
	TaskFunction_t taskHandler_Sensor2,
	TaskFunction_t taskHandler_Sensor3,
	TaskFunction_t taskHandler_Control1,
	TaskFunction_t taskHandler_Control2)
{
	/* Attempt to create the event group. */
	xEventGroup = xEventGroupCreate();

	WorkerTask_t* pTask_Sensor1 = NULL;
	WorkerTask_t* pTask_Sensor2 = NULL;
	WorkerTask_t* pTask_Sensor3 = NULL;
	WorkerTask_t* pTask_Control1 = NULL;
	WorkerTask_t* pTask_Control2 = NULL;

	global_taskList = gll_init();

	gll_t* queueList_task_sensor1 = gll_init();
	gll_t* queueList_task_sensor2 = gll_init();
	gll_t* queueList_task_sensor3 = gll_init();
	gll_t* queueList_task_control1 = gll_init();
	gll_t* queueList_task_control2 = gll_init();

	xQueue1  = xQueueCreate(1, sizeof(uint16_t));
	xQueue2A = xQueueCreate(1, sizeof(uint16_t));
	xQueue2B = xQueueCreate(1, sizeof(uint16_t));

	xQueueTickCount = xQueueCreate(1, sizeof(TickType_t));

	gll_push(queueList_task_sensor1, &xQueue1);
	pTask_Sensor1 = WorkerTask_Create(taskHandler_Sensor1, 1, 3, 100, 199, 200, queueList_task_sensor1);

	gll_push(queueList_task_sensor2, &xQueue2A);
	pTask_Sensor2 = WorkerTask_Create(taskHandler_Sensor2, 2, 3, 200, 249, 500, queueList_task_sensor2);

	gll_push(queueList_task_sensor3, &xQueue2B);
	pTask_Sensor3 = WorkerTask_Create(taskHandler_Sensor3, 3, 3, 250, 299, 1400, queueList_task_sensor3);

	gll_push(queueList_task_control1, &xQueue2B);
	gll_push(queueList_task_control1, &xQueue2A);
	gll_push(queueList_task_control1, &xQueue1);
	pTask_Control1 = WorkerTask_Create(taskHandler_Control1, 4, 2, 1, 0, 100, queueList_task_control1);

	gll_push(queueList_task_control2, &xQueue2B);
	gll_push(queueList_task_control2, &xQueue2A);
	gll_push(queueList_task_control2, &xQueue1);
	pTask_Control2 = WorkerTask_Create(taskHandler_Control2, 5, 2, 2, 0, 100, queueList_task_control2);

	// push NULL, since we do not want to use index = 0, indexing should start from 1 (e.g. Task_1)
	gll_pushBack(global_taskList, pTask_Sensor1);	// index 0
	gll_pushBack(global_taskList, pTask_Sensor2);	// index 1
	gll_pushBack(global_taskList, pTask_Sensor3);	// index 2
	gll_pushBack(global_taskList, pTask_Control1);	// index 3
	gll_pushBack(global_taskList, pTask_Control2);	// index 4

	// Initialize scheduler 
	TaskHandle_t xTaskSchedulerHandle;
	xTaskCreate(schedulerHandler, "Scheduler", 1000, global_taskList, TASK_SCHEDULER_PRIORITY, &xTaskSchedulerHandle);

	vTaskSuspend(pTask_Sensor1->xHandle);
	vTaskSuspend(pTask_Sensor2->xHandle);
	vTaskSuspend(pTask_Sensor3->xHandle);
	vTaskSuspend(pTask_Control1->xHandle);
	vTaskSuspend(pTask_Control2->xHandle);

	// Activate controller 1
	xEventGroupSetBits(xEventGroup, BIT_4);
}
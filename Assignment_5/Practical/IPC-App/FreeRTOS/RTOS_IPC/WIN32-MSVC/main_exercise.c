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
#include "semphr.h"

/* Other Includes */
#include "workerTask.h"
#include "print.h"
#include "semaphore.h"
#include "config.h"

#define mainNUMBER_OF_SEMAPHORS					( 3 )

/* TODO: output frequencey
*/
#define workersUSELESS_CYCLES_PER_TIME_UNIT		( 1000000UL)

#define TASK_SCHEDULER_PRIORITY 6
#define TASK_SCHEDULER_TICK_TIME 3

/* TODO: output frequencey
*/
TickType_t currentTime = 0;
const TickType_t SCHEDULER_OUTPUT_FREQUENCY_MS = 1 / portTICK_PERIOD_MS;

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif

/* TODO */
static gll_t* global_taskList;

static void vUselessLoad(uint32_t ulCycles);
static void prvTaskSchedulerHandler(void *pvParameters);
static void prvTaskSensor1(void *pvParameters);
static void prvTaskSensor2(void *pvParameters);
static void prvTaskSensor3(void *pvParameters);
static void prvTaskControl1(void *pvParameters);
static void prvTaskControl2(void *pvParameters);

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
	vInitialize(prvTaskSchedulerHandler, prvTaskSensor1, prvTaskSensor2, prvTaskSensor3, prvTaskControl1, prvTaskControl2);
	vTaskStartScheduler();

	while (true)
	{
	}
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
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTaskSchedulerHandler'. NULL Pointer");
#endif
		return;
	}

	uint32_t uCurrentTickCount = 0;
	gll_t* pTaskList = (gll_t*)pvParameters;
	WorkerTask_t* pWorkerTask = NULL;

	while (true) {
#if DEBUG
		vPrintString("Current tick time: "); vPrintInteger(sCurrentTickCount); vPrintString("\n");
#endif
		for (uint8_t uIndex = 0; uIndex < pTaskList->size; uIndex++)
		{

			pWorkerTask = gll_get(pTaskList, uIndex);

			if ((uCurrentTickCount >= pWorkerTask->uReleaseTime) && // Check if the task is released 
				((uCurrentTickCount % pWorkerTask->uPeriod) == (pWorkerTask->uReleaseTime % pWorkerTask->uPeriod)))
			{ // and Check if the current time is the next period the task 
				/*
				eTaskState currentTaskState = eTaskGetState(pWorkerTask->xHandle);

				if (currentTaskState != eRunning)
				{
				// Dispatch the ready task
				vTaskResume(pWorkerTask->xHandle);
				} */

				// Dispatch the ready task
				vTaskResume(pWorkerTask->xHandle);
#if IS_SCHEDULER_RUNNING
				if (eTaskGetState(pWorkerTask->xHandle) == eReady)
				{
					vPrintString("Task #"); vPrintInteger(pWorkerTask->uTaskNumber); vPrintString(" gets READY at tick count: "); vPrintUnsignedInteger(uCurrentTickCount);
					vPrintString(", with priority: "); vPrintUnsignedInteger(uxTaskPriorityGet(pWorkerTask->xHandle));  vPrintStringLn("");
				}
#endif 
			}

		}

		uCurrentTickCount++;
		vTaskDelay(TASK_SCHEDULER_TICK_TIME * SCHEDULER_OUTPUT_FREQUENCY_MS);
	}
}

static void pTask_Sensor1(void *pvParameters)
{
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTask2'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* workerTask_2 = (WorkerTask_t*)pvParameters;
	Semaphore_t* pSemaphore_A = Semaphore_sList_GetSemaphoreById(workerTask_2->pUsedSemaphoreList, 1); // Semaphore B ID = 1
	Semaphore_t* pSemaphore_C = Semaphore_sList_GetSemaphoreById(workerTask_2->pUsedSemaphoreList, 3); // Semaphore C ID = 3

	while (true)
	{
		vPrintStringLn("Output: ");

		// Task completed one period, suspend it
		vTaskSuspend(workerTask_2->xHandle);
	}
}

void vInitialize(
	TaskFunction_t schedulerHandler,
	TaskFunction_t taskHandler_Sensor1,
	TaskFunction_t taskHandler_Sensor2,
	TaskFunction_t taskHandler_Sensor3,
	TaskFunction_t taskHandler_Control1,
	TaskFunction_t taskHandler_Control2) {

	WorkerTask_t* pTask_Sensor1 = NULL;
	WorkerTask_t* pTask_Sensor2 = NULL;
	WorkerTask_t* pTask_Sensor3 = NULL;
	WorkerTask_t* pTask_Control1 = NULL;
	WorkerTask_t* pTask_Control2 = NULL;

	global_taskList = gll_init();
	gll_t* semaphoreList = gll_init();

	gll_t* semaphoreList_task_sensor1 = gll_init();
	gll_t* semaphoreList_task_sensor2 = gll_init();
	gll_t* semaphoreList_task_sensor3 = gll_init();
	gll_t* semaphoreList_task_control1 = gll_init();
	gll_t* semaphoreList_task_control2 = gll_init();

	Semaphore_t* pSemaphore_A = Semaphore_Create(5, 1);
	Semaphore_t* pSemaphore_B = Semaphore_Create(5, 2);
	Semaphore_t* pSemaphore_C = Semaphore_Create(5, 3);

	gll_push(semaphoreList_task_sensor1, pSemaphore_A);
	pTask_Sensor1 = WorkerTask_Create(taskHandler_Sensor1, 1, 5, 100, 199, 200, semaphoreList_task_sensor1);

	gll_push(semaphoreList_task_sensor2, pSemaphore_B);
	pTask_Sensor2 = WorkerTask_Create(taskHandler_Sensor2, 2, 4, 200, 249, 500, semaphoreList_task_sensor2);

	gll_push(semaphoreList_task_sensor3, pSemaphore_C);
	pTask_Sensor3 = WorkerTask_Create(taskHandler_Sensor3, 3, 3, 250, 299, 1400, semaphoreList_task_sensor3);

	gll_push(semaphoreList_task_control1, pSemaphore_A);
	gll_push(semaphoreList_task_control1, pSemaphore_B);
	gll_push(semaphoreList_task_control1, pSemaphore_C);
	pTask_Control1 = WorkerTask_Create(taskHandler_Control1, 4, 2, 0, 0, 100, semaphoreList_task_control1);

	gll_push(semaphoreList_task_control2, pSemaphore_A);
	gll_push(semaphoreList_task_control2, pSemaphore_B);
	gll_push(semaphoreList_task_control2, pSemaphore_C);
	pTask_Control2 = WorkerTask_Create(taskHandler_Control1, 5, 2, 0, 0, 100, semaphoreList_task_control2);

	// push NULL, since we do not want to use index = 0, indexing should start from 1 (e.g. Task_1)
	gll_pushBack(global_taskList, pTask_Sensor1);
	gll_pushBack(global_taskList, pTask_Sensor2);
	gll_pushBack(global_taskList, pTask_Sensor3);
	gll_pushBack(global_taskList, pTask_Control1);
	gll_pushBack(global_taskList, pTask_Control2);

	gll_pushBack(semaphoreList, pSemaphore_A);
	gll_pushBack(semaphoreList, pSemaphore_B);
	gll_pushBack(semaphoreList, pSemaphore_C);

	// Initialize scheduler 
	TaskHandle_t xTaskSchedulerHandle;
	xTaskCreate(schedulerHandler, "Scheduler", 1000, global_taskList, TASK_SCHEDULER_PRIORITY, &xTaskSchedulerHandle);

	vTaskSuspend(pTask_Sensor1->xHandle);
	vTaskSuspend(pTask_Sensor2->xHandle);
	vTaskSuspend(pTask_Sensor3->xHandle);
	//vTaskSuspend(pTask_4->xHandle);
}

static void prvTaskSensor1(void *pvParameters) {}
static void prvTaskSensor2(void *pvParameters) {}
static void prvTaskSensor3(void *pvParameters) {}
static void prvTaskControl1(void *pvParameters){}
static void prvTaskControl2(void *pvParameters){}
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
//#include "config.h"
#include "event_groups.h"

#define mainNUMBER_OF_SEMAPHORS					( 3 )

/* TODO: output frequencey
*/
#define workersUSELESS_CYCLES_PER_TIME_UNIT		( 1000000UL)

#define TASK_SCHEDULER_PRIORITY		6
#define TASK_SCHEDULER_TICK_TIME	1

// Define bits position of specific event flags in event group
#define BIT_4	( 1 << 4 )	// Controller 1 is ACTIVE
#define BIT_3	( 1 << 3 )	// Controller 2 is ACTIVE
#define BIT_2	( 1 << 2 )	// Queue 1 is ready
#define BIT_1	( 1 << 1 )	// Queue 2A is ready
#define BIT_0	( 1 << 0 )	// Queue 2B is ready

/* TODO: output frequencey */
TickType_t currentTime = 0;
const TickType_t SCHEDULER_OUTPUT_FREQUENCY_MS = 1 / portTICK_PERIOD_MS;

/* TODO */
static gll_t* global_taskList;
/*
*	========================================================================
*	========================================================================
*	Since we want to use latest value of sensor's readings,
*	all 3 queues will have size of 1
*	------------------------------------------------------------------------
*	xQueue1  - queue of data which corresponds to data from Sensor 1
*	xQueue2A - queue of data which corresponds to data from Sensor 2A
*	xQueue2B - queue of data which corresponds to data from Sensor 2B
*
*	========================================================================
*	========================================================================
*
*	There is queue which passes values from Scheduler who counts ticks
*	to Sensor Task. This queue also has a size of 1. The queue poses as
*	register where current time is stored in processor.
*	Sensors does not clear this register, just take a peek in order to
*	make other tasks possible to read current time.
*	------------------------------------------------------------------------
*	xQueueTickCount - queue of data which corresponds to data from Sensor 2B
*
*	========================================================================
*	========================================================================
*	
*	The eventGroup is used to simulated status registers which can trigger
*	an event. For instance, some task can wait for a status bit to change
*	and continue execution, or to check if there are some changes.
*	Example: checking event status bit corresponds to better resources
*	management instead of checking if the queue is not empty (firstly doing
*	peek and then receive if it is not empty.
*	------------------------------------------------------------------------
*	                     xEventGroup - 5bit register of events
*	    Bits |                      Description
*	________________________________________________________________________
*	     4   |  Controller 1 status: 1 - Actived; 0 - Deactivated
*	     3   |  Controller 2 status: 1 - Actived; 0 - Deactivated
*	     2   |  Queue 1  status:     1 - new data ready; 0 - empty
*	     1   |  Queue 2A status:     1 - new data ready; 0 - empty
*	     0   |  Queue 2B status:     1 - new data ready; 0 - empty
*	========================================================================
*	========================================================================
*/
static QueueHandle_t xQueue1, xQueue2A, xQueue2B;
static QueueHandle_t xQueueTickCount;
static EventGroupHandle_t xEventGroup;

/*
*	We have used 3 types of tasks (task handlers):
*			1 - Scheduler
*			2 - Sensor task - all sensors use the same task handler
*			3 - Controller task handler (REQUIRENMENTS)
*/
static void prvTaskSchedulerHandler(void *pvParameters);
static void prvTaskSensor(void *pvParameters);
static void prvTaskControl(void *pvParameters);

void vInitialize(TaskFunction_t schedulerHandler,
	TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4,
	TaskFunction_t taskHandler_5);

/*-----------------------------------------------------------*/

void main_exercise(void)
{
	vPrintStringLn("Starting the application...");

	// Initialize Scheduler with working tasks (3 sensors and 2 controllers)
	vInitialize(prvTaskSchedulerHandler, prvTaskSensor, prvTaskSensor, prvTaskSensor, prvTaskControl, prvTaskControl);
	vTaskStartScheduler();

	while (true)
	{}
}
/*-----------------------------------------------------------*/

// Task Scheduler Process
void prvTaskSchedulerHandler(void *pvParameters) 
{	
	uint32_t uCurrentTickCount = 0;

	// Global list of tasks
	gll_t* pTaskList = (gll_t*)pvParameters;

	// The same base for sensor and controller is used
	WorkerTask_t* pWorkerTask = NULL;

	// Variable used for storing the read event status 'register'
	EventBits_t uxBits;
	while (true)
	{
		// Read status BITs
		// BITs that are of schedulers interest are BIT 4 and BIT 3 which corresponds to Controllers status registers
		// Scheduler MUST check if main controller (1) has failed and to start back up controller (2)
		uxBits = xEventGroupGetBits(xEventGroup);

		// Iterate through list of tasks in the scheduler
		for (uint8_t uIndex = 0; uIndex < pTaskList->size; uIndex++)
		{
			/*
			*	Get corresponding task (1..5)
			*	
			*	NOTE: uIndex = Task ID - 1
			*	 ________________________________________
			*	|            |                           |
			*	|   Task ID  |        Description        |
			*	|____________|___________________________|
			*	|			 |                           |
			*	|      1     |         Sensor 1          |
			*	|____________|___________________________|
			*	|			 |                           |
			*	|      2     |         Sensor 2          |
			*	|____________|___________________________|
			*	|			 |                           |
			*	|      3     |         Sensor 3          |
			*	|____________|___________________________|
			*	|			 |                           |
			*	|      4     |       Controller 1        |
			*	|____________|___________________________|
			*	|			 |                           |
			*	|      5     |       Controller 2        |
			*	|____________|___________________________|
			*/
			pWorkerTask = gll_get(pTaskList, uIndex);

			// Firstly check if task is still active
			// When Controller fails, task will not exist (will be deleted by itself)
			if (pWorkerTask->xHandle != NULL)
			{
				// If task exist check if it is released
				if ((uCurrentTickCount >= pWorkerTask->uReleaseTime) &&
					((uCurrentTickCount % pWorkerTask->uPeriod) == (pWorkerTask->uReleaseTime % pWorkerTask->uPeriod)))
				{
					// Execute task
					vTaskResume(pWorkerTask->xHandle);
				}
			}

			/*
			*	If task does NOT exist it means it has failed.
			*	----------------------------------------------------------------------------
			*	NOTE 1:
			*	Since we have only one controller that can fail (considering Assignment 5),
			*	only that Controlle 1 task is checked. Its ID is 4, meaning uIndex is 3.
			*	----------------------------------------------------------------------------
			*	NOTE 2:
			*	Beside task ID, we check if the controller 1 is activated by checking
			*	corresponding BIT_4. If it is 0, it would mean that it is already
			*	deactivated. We do NOT delete the position of Controller 1 task in global
			*	task list in case of it's recovery. In that case BIT_4 should have been
			*	asserted manually in some other place and BIT_3 deasserted in order to
			*	deactivate Controller 2 (back-up controller).
			*	----------------------------------------------------------------------------
			*	NOTE 3:
			*	For further improvements and exceptions free work, failure of Controller 2
			*	should have been checked too. Otherwise, an Assert fail will happend throwing
			*	an exception and blocking the whole system.	
			*	----------------------------------------------------------------------------
			*/
			else if (uIndex == 3 && (uxBits & BIT_4)!=0)
			{
				// Deactivate Controller 1
				xEventGroupClearBits(xEventGroup, BIT_4);
				// Activate Controller 2
				xEventGroupSetBits(xEventGroup, BIT_3);
			}
		}

		// Increase time
		++uCurrentTickCount;
		// Write time in the queue so the other tasks can see time update
		// We use overwrite since the queue has the size 1 and we want to delete old time value
		xQueueOverwrite(xQueueTickCount, (void *)&uCurrentTickCount);
		// Sleep for some time - i.e. 1ms
		vTaskDelay(TASK_SCHEDULER_TICK_TIME * SCHEDULER_OUTPUT_FREQUENCY_MS);
	}
}

/*
*				SENSOR TASK
*/
static void prvTaskSensor(void * pvParameters)
{
	// Initializations
	WorkerTask_t* sensorTask = (WorkerTask_t*)pvParameters;

	// Queue where the read data will be stored
	QueueHandle_t* pQueueHandle = gll_get(sensorTask->pUsedQueueList, 0);

	// event group - register with status bits
	EventBits_t uxBits;
	while (true)
	{
		// Function to wrap the counting
		if(++sensorTask->uCurrentValue >= sensorTask->uEndValue)
		{
			sensorTask->uCurrentValue = sensorTask->uStartValue;
		}

		/*
		*	Get corresponding BIT in event group
		*	uxBits will correspond to position of queue flags of corresponding flags
		*	(description given above)
		*	
		*	uTaskNumber for sensors goes from 1..3, while bits goes 2..0 in corresponding way
		*	(description given above)
		*	
		*	Example: Sensor 1 has an ID = 1 ---> uTaskNumber = 1;
		*			 Corresponding bit of Sensor 1 is 2. Therefor, we need to
		*			 shift bit at position 1 for 2 places to the left. This is 3-uTaskNumber = 3-1 = 2.
		*			 Similary, for Sensor 2A it is BIT_1, so we need one shift which is 3-uTaskNumber = 3-2 = 1
		*			 Also, for Sensor 2B it is BIT_0 and we do NOT need any shifting. But, calculating the
		*			 same formulae, we obtain 3-uTaskNumber = 3-3 = 0.
		*/
		uxBits = (const) BIT_0 << (3 - sensorTask->uTaskNumber);

		// Write sensor 'readings' in the corresponding sensor's queue
		// We use overwrite since the queue has the size 1 and we want to delete old value/old 'sensor reading'
		xQueueOverwrite(*(pQueueHandle), &sensorTask->uCurrentValue);

		// Raise the event flag of the corresponding BIT flag
		uxBits = xEventGroupSetBits(xEventGroup, uxBits);

		// Task completed one period, suspend it
		vTaskSuspend(sensorTask->xHandle);
	}
}

/*
*				CONTROLLER TASK
*/
static void prvTaskControl(void * pvParameters)
{
	// Initializations
	uint32_t uCurrentTickCount = 0;
	WorkerTask_t* controlTask = (WorkerTask_t*)pvParameters;

	// Controllers are connected to all three queues
	uint16_t uQueuedValue1 = 0;
	uint16_t uQueuedValue2 = 0;
	uint16_t uQueuedValue3 = 0;

	// Connect controller to queues
	QueueHandle_t*	pQueueHandle = gll_get(controlTask->pUsedQueueList, 0);  // Queue_1
	QueueHandle_t xQueue1 = *pQueueHandle;
					pQueueHandle = gll_get(controlTask->pUsedQueueList, 1);  // Queue_2A
	QueueHandle_t xQueue2A = *pQueueHandle;
					pQueueHandle = gll_get(controlTask->pUsedQueueList, 2);  // Queue_2B
	QueueHandle_t xQueue2B = *pQueueHandle;

	EventBits_t uxBits;
	while (true)
	{
		// Read the time
		xQueuePeek(xQueueTickCount, &(uCurrentTickCount), (TickType_t)0);

		// Simulate Controller 1 value.
		// uStartValue for controller means if the controller is main (=1) or back-up (=2)
		if (uCurrentTickCount >= 2001 && controlTask->uStartValue == 1)
		{
			vPrintString("Controller 1 has had an error at ");
			vPrintUnsignedInteger(uCurrentTickCount);
			vPrintStringLn(".");
						
			// Delete the task
			controlTask->xHandle = NULL;
			vTaskDelete(controlTask->xHandle);
			return;
		}

		// Read status register
		// first pdFALSE is for xClearOnExit. It is used to notify following function NOT to delete status registers, just to READ THEM
		// second pdFALSE is for xWaitForAllBits. It is used to notify if the function needs to wait for all bits to be asserted (logical AND)
		uxBits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4, pdFALSE, pdFALSE, (TickType_t)0);

		// Check if there is an Controller 1 or 2 runnig
		if (((uxBits & (BIT_4)) && controlTask->uStartValue == 1) || ((uxBits & (BIT_3)) && controlTask->uStartValue == 2))
		{
			// Now we need to check if there are values in Queue 1 and Queue2x
			
			// Check if there are values in Queue 1 and Queue 2A
			if ((uxBits & (BIT_2)) && ((uxBits & (BIT_1))))
			{
				// Read Queue 1 and Queue 2A
				// xQueueReceive will take the value from the queue and delete it from the queue - it will pop the value from the queue
				xQueueReceive(xQueue1, &(uQueuedValue1), (TickType_t)0);
				xQueueReceive(xQueue2A, &(uQueuedValue2), (TickType_t)0);

				// Set status bits of these 2 queues that values are read
				// This is done in order to notify back-up controller that
				// the process has been executed
				uxBits = xEventGroupClearBits(xEventGroup, BIT_1 | BIT_2);

				// Print Out
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
			// Check if there are values in Queue 1 and Queue 2B
			else if ((uxBits & (BIT_2)) && ((uxBits & (BIT_0))))
			{
				// Read Queue 1 and Queue 2B
				// xQueueReceive will take the value from the queue and delete it from the queue - it will pop the value from the queue
				xQueueReceive(xQueue1, &(uQueuedValue1), (TickType_t)0);
				xQueueReceive(xQueue2B, &(uQueuedValue3), (TickType_t)0);

				// Set status bits of these 2 queues that values are read
				// This is done in order to notify back-up controller that
				// the process has been executed
				uxBits = xEventGroupClearBits(xEventGroup, BIT_0 | BIT_2);

				// Print Out
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
	// Create Status register
	xEventGroup = xEventGroupCreate();

	// Define pointer to tasks
	WorkerTask_t* pTask_Sensor1 = NULL;
	WorkerTask_t* pTask_Sensor2 = NULL;
	WorkerTask_t* pTask_Sensor3 = NULL;
	WorkerTask_t* pTask_Control1 = NULL;
	WorkerTask_t* pTask_Control2 = NULL;

	// Initialize global task list
	global_taskList = gll_init();

	// Initialize Queue lists - only controllers have multiple elements in the queueList
	gll_t* queueList_task_sensor1 = gll_init();
	gll_t* queueList_task_sensor2 = gll_init();
	gll_t* queueList_task_sensor3 = gll_init();
	gll_t* queueList_task_control1 = gll_init();
	gll_t* queueList_task_control2 = gll_init();

	// Create Queues
	xQueue1  = xQueueCreate(1, sizeof(uint16_t));
	xQueue2A = xQueueCreate(1, sizeof(uint16_t));
	xQueue2B = xQueueCreate(1, sizeof(uint16_t));

	xQueueTickCount = xQueueCreate(1, sizeof(TickType_t));

	// SENSOR 1
	gll_push(queueList_task_sensor1, &xQueue1);
	// arg = {task handler, task ID, starting counting value, last counted value, task's period, list of queues that sensor is connected to (i.e. the queue has a single element)}
	pTask_Sensor1 = WorkerTask_Create(taskHandler_Sensor1, 1, 3, 100, 199, 200, queueList_task_sensor1);

	// SENSOR 2
	gll_push(queueList_task_sensor2, &xQueue2A);
	// arg = {task handler, task ID, starting counting value, last counted value, task's period, list of queues that sensor is connected to (i.e. the queue has a single element)}
	pTask_Sensor2 = WorkerTask_Create(taskHandler_Sensor2, 2, 3, 200, 249, 500, queueList_task_sensor2);

	// SENSOR 3
	gll_push(queueList_task_sensor3, &xQueue2B);
	// arg = {task handler, task ID, starting counting value, last counted value, task's period, list of queues that sensor is connected to (i.e. the queue has a single element)}
	pTask_Sensor3 = WorkerTask_Create(taskHandler_Sensor3, 3, 3, 250, 299, 1400, queueList_task_sensor3);

	// CONTROLLER 1
	gll_push(queueList_task_control1, &xQueue2B);
	gll_push(queueList_task_control1, &xQueue2A);
	gll_push(queueList_task_control1, &xQueue1);
	// arg = {task handler, task ID, controller # (i.e. #1), not used (any number when defining controller), task's period, list of queues that controller is connected to}
	pTask_Control1 = WorkerTask_Create(taskHandler_Control1, 4, 2, 1, 0, 100, queueList_task_control1);

	// CONTROLLER 2
	gll_push(queueList_task_control2, &xQueue2B);
	gll_push(queueList_task_control2, &xQueue2A);
	gll_push(queueList_task_control2, &xQueue1);
	// arg = {task handler, task ID, controller # (i.e. #2), not used (any number when defining controller), task's period, list of queues that controller is connected to}
	pTask_Control2 = WorkerTask_Create(taskHandler_Control2, 5, 2, 2, 0, 100, queueList_task_control2);

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

	// Controller 1 set to Activated
	xEventGroupSetBits(xEventGroup, BIT_4);
}
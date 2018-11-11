/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 * 
 ******************************************************************************
 *
 * NOTE:  Console input and output relies on Windows system calls, which can
 * interfere with the execution of the FreeRTOS Windows port.  This demo only
 * uses Windows system call occasionally.  Heavier use of Windows system calls
 * can crash the port.
 */

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* TODO: output frequencey
*/
TickType_t currentTime = 0;
const TickType_t mainTASK_CHATTERBOX_OUTPUT_FREQUENCY_MS = 1 / portTICK_PERIOD_MS;
/*-----------------------------------------------------------*/

#define WORKER_TASK_PRIORITY	0 
#define SCHEDULER_TASK_PRIORITY 1
#define MAX_EXE_TIME			1000000U
#define HYPERPERIOD				600U
#define MAX_NUM_WORKER_TASKS	6
#define MAX_NUM_FRAMES			5 + 1
#define FRAME_SIZE_TIME			120U

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif

typedef uint8_t bool_t;

typedef struct workerTask
{
	TaskHandle_t xHandle;
	bool_t isMissbehaved;
	uint32_t uExecutionTime;
	uint8_t uTaskNumber;

} workerTask_t;

void vWorkerTask(void *pvParameters);
void vSchedulerTask(void *pvParameters);
void vPrintString(const char *pcString);
void vInitWorkerTask(workerTask_t* pWorkerTask, uint8_t uTaskNumber, bool_t isMissbehaved, uint32_t uExecutionTime);
void vCheckIfPreviousFrameSuspended(workerTask_t* pWorkerTasks[MAX_NUM_WORKER_TASKS], uint8_t previousFrameTaskCount, uint8_t previousFrameIndex);


typedef struct frameList {

	workerTask_t* pWorkerTasks[MAX_NUM_FRAMES][MAX_NUM_WORKER_TASKS];
	uint8_t numTasks[MAX_NUM_FRAMES];

} frameList_t;

void vWorkerTask(void *pvParameters) {

	uint32_t uCounter = 0;
	workerTask_t* pWorkerTask;

	if ( pvParameters == NULL ) {
		return;
	}

	pWorkerTask = (workerTask_t*) pvParameters;

	while (true)
	{
		while ( uCounter <= MAX_EXE_TIME * pWorkerTask->uExecutionTime || pWorkerTask->isMissbehaved == true ) {

			uCounter++;

		}
		printf("I counted %u cycles, took some time.", MAX_EXE_TIME * pWorkerTask->uExecutionTime);
		vTaskSuspend(pWorkerTask->xHandle);
	}

	vTaskDelete(NULL);
}

void vSchedulerTask(void *pvParameters)
{
	printf("It's the very first frame.\n");
	fflush(stdout);

	frameList_t* pFrameList;
	uint8_t uCurrentFrameCounter = 0;

	if (pvParameters == NULL) {
		return;
	}

	pFrameList = (frameList_t*) pvParameters;
	
	while (1) 
	{
		printf("Scheduling for Frame %u.\n", uCurrentFrameCounter);
		fflush(stdout);

		uint8_t previousFrameIndex = (uCurrentFrameCounter - 1 + MAX_NUM_WORKER_TASKS) % MAX_NUM_WORKER_TASKS;
		vCheckIfPreviousFrameSuspended( pFrameList->pWorkerTasks[previousFrameIndex],
										pFrameList->numTasks[previousFrameIndex],
										previousFrameIndex
									  );
		
		for (uint8_t uTaskIndex = 0; uTaskIndex < pFrameList->numTasks[uCurrentFrameCounter]; uTaskIndex++) {

			workerTask_t* workerTask = pFrameList->pWorkerTasks[uCurrentFrameCounter][uTaskIndex];
			
			eTaskState currentTaskState = eTaskGetState(workerTask->xHandle);
			// TODO: We do not only make tasks runnable but we also change their priorities accordingly so they can run
			// on different times based on the frame list
			if(currentTaskState != eRunning)
			{
				vTaskResume(workerTask->xHandle);
			}
		}
		
		/* Delay for a period. */
		uCurrentFrameCounter = (uCurrentFrameCounter + 1) % MAX_NUM_WORKER_TASKS;
		vTaskDelay( FRAME_SIZE_TIME * mainTASK_CHATTERBOX_OUTPUT_FREQUENCY_MS );
		
	}

}

void vCheckIfPreviousFrameSuspended(workerTask_t* pWorkerTasks[MAX_NUM_WORKER_TASKS], uint8_t previousFrameTaskCount, uint8_t previousFrameIndex) {

	if(pWorkerTasks == NULL)
	{
		return;
	}

	for (uint8_t uTaskIndex = 0; uTaskIndex < previousFrameTaskCount; uTaskIndex++) 
	{

		workerTask_t* task = pWorkerTasks[uTaskIndex];
		task->xHandle;
		eTaskState currentTaskState = eTaskGetState(task->xHandle );

		if ( currentTaskState == eRunning ) {

			printf("Task %u in Frame %u was not suspended. sad.\n", task->uTaskNumber, previousFrameIndex);
			fflush(stdout);
			vTaskSuspend(task->xHandle );
		}
	}

}

void vInitWorkerTask(workerTask_t* pWorkerTask, uint8_t uTaskNumber, bool_t isMissbehaved, uint32_t uExecutionTime) {

	pWorkerTask->isMissbehaved = isMissbehaved;
	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;
}


void vPrintString(const char *pcString)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%s", pcString);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}


void main_exercise( void )
{
	workerTask_t workerTask_0;
	workerTask_t workerTask_1;
	workerTask_t workerTask_2;
	workerTask_t workerTask_3;
	workerTask_t workerTask_4;
	workerTask_t workerTask_5;

	frameList_t frameList = {   { { &workerTask_0, &workerTask_1, &workerTask_2, &workerTask_3, &workerTask_4, &workerTask_5 },
								  { NULL, NULL, NULL, NULL, NULL, NULL },
								  { &workerTask_0, &workerTask_1, &workerTask_4, NULL ,NULL, NULL },
								  { &workerTask_2, &workerTask_3, NULL, NULL ,NULL, NULL },
								  { &workerTask_0, &workerTask_1, &workerTask_4, NULL ,NULL, NULL } },
								  { 6, 0, 3, 2, 3 }
	};
	

	/*
	workerTask_t* pFrameList[MAX_NUM_FRAMES][MAX_NUM_WORKER_TASKS] = {  { &workerTask_0, &workerTask_1, &workerTask_2, &workerTask_3, &workerTask_4, &workerTask_5},
																		{ NULL, NULL, NULL, NULL, NULL, NULL },
																		{ &workerTask_0, &workerTask_1, &workerTask_4, NULL ,NULL, NULL },
																		{ &workerTask_2, &workerTask_3, NULL, NULL ,NULL, NULL },
																		{ &workerTask_0, &workerTask_1, &workerTask_4, NULL ,NULL, NULL }
																	};
																	*/
	vInitWorkerTask(&workerTask_0, 0, false, 1);
	vInitWorkerTask(&workerTask_1, 1, false, 2);
	vInitWorkerTask(&workerTask_2, 2, false, 3);
	vInitWorkerTask(&workerTask_3, 3, false, 4);
	vInitWorkerTask(&workerTask_4, 4, false, 5);
	vInitWorkerTask(&workerTask_5, 5, true,  6);

	/*
	 * TODO
	 */
	xTaskCreate(vWorkerTask, "Task 0", 1000, &workerTask_0, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vWorkerTask, "Task 1", 1000, &workerTask_1, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vWorkerTask, "Task 2", 1000, &workerTask_2, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vWorkerTask, "Task 3", 1000, &workerTask_3, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vWorkerTask, "Task 4", 1000, &workerTask_4, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vWorkerTask, "Task 5", 1000, &workerTask_5, WORKER_TASK_PRIORITY, NULL);
	xTaskCreate(vSchedulerTask, "Scheduler", 1000, &frameList, SCHEDULER_TASK_PRIORITY, NULL);

	vTaskStartScheduler();
	for( ;; );
}
/*-----------------------------------------------------------*/

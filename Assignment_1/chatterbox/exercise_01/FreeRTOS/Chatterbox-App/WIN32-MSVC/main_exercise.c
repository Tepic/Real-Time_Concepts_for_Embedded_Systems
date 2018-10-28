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
 * NOTE: Windows will not be running the FreeRTOS project threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this project application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 * NOTE 2:  This file only contains the source code that is specific to exercise 2
 * Generic functions, such FreeRTOS hook functions, are defined
 * in main.c.
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

/* TODO: Priorities at which the tasks are created.
 */
#define INFITITY 0

#define HIGH_PRIORITY	7
#define MEDIUM_PRIORITY	3
#define LOW_PRIORITY	1

#define TASK_1_PRIORITY MEDIUM_PRIORITY
#define TASK_2_PRIORITY LOW_PRIORITY 
#define TASK_3_PRIORITY HIGH_PRIORITY

#define TASK_1_PERIOD_MS 500
#define TASK_2_PERIOD_MS 500
#define TASK_3_PERIOD_MS 500


/* TODO: output frequencey
 */
TickType_t currentTime = 0;
const TickType_t mainTASK_CHATTERBOX_OUTPUT_FREQUENCY_MS = 1 / portTICK_PERIOD_MS;

/*-----------------------------------------------------------*/

/*
  * TODO: data structure
  */

struct ChatterboxTask
{
	char* name;
	unsigned short priority;
	unsigned long  period_MS;
	unsigned long publishedMessages;

	unsigned long periodicity;
};
typedef struct ChatterboxTask pTask;

/*
 * TODO: C function (prototype) for task
 */
void vPrintString(const char *pcString);
void vTask(void *pvParameters);


void initTask(pTask* task, char* taskName, unsigned short priority, unsigned long period, unsigned long periodicity);
 
/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_exercise( void )
{
	/*
	* TODO: initialize data structures
	*/
	struct ChatterboxTask mTask1;
	struct ChatterboxTask mTask2;
	struct ChatterboxTask mTask3;

	initTask(&mTask1, "Task1\r\n", TASK_1_PRIORITY, TASK_1_PERIOD_MS, INFITITY);	// TASK 1 will be executed infinite number of times
	initTask(&mTask2, "Task2\r\n", TASK_2_PRIORITY, TASK_2_PERIOD_MS, INFITITY);	// TASK 2 will be executed infinite number of times
	initTask(&mTask3, "Task3\r\n", TASK_3_PRIORITY, TASK_3_PERIOD_MS, 5);			// TASK 3 will be deleted after 2 executions

	/* 
	 * TODO: Create the task instances.
     */
	
	// TASK 1
	xTaskCreate(vTask,				/* Pointer to the function that implements the task. */
		        "Task 1",			/* Text name for the task. This is to facilitate debugging only */
				1000,				/* Stack depth - small microcontrollers will use much less stack than this. */
				&mTask1,			/* This is task parameter. */
				mTask1.priority,	/* This task will run at priority assigned to task 1. */
				NULL);				/* This example does not use the task handle. */
		
	// TASK 2
	xTaskCreate(vTask, "Task 2", 1000, &mTask2, mTask2.priority, NULL);

	// TASK 3
	xTaskCreate(vTask, "Task 3", 1000, &mTask3, mTask3.priority, NULL);

	 /*
	 * TODO: Start the task instances.
	 */
	 vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* 
 * TODO: C function for tasks
 */
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

void vTask(void *pvParameters)
{
	pTask* task = (pTask*)pvParameters;
	const char *pcTaskName = task->name;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* Print out the name of this task. */
		//currentTime = xTaskGetTickCount() / portTICK_PERIOD_MS;
		vPrintString(pcTaskName);

		if (task->periodicity != INFITITY)
		{
			task->publishedMessages += 1;
			if (task->publishedMessages >= task->periodicity)
			{
				vPrintString("It has finished its execution - TASK EXECUTED!\r\n");
				break;
			}
		}

		/* Delay for a period. */
		vTaskDelay(task->period_MS * mainTASK_CHATTERBOX_OUTPUT_FREQUENCY_MS);
	}
	vTaskDelete(NULL);
}

void initTask(pTask* task, char* taskName, unsigned short priority, unsigned long period, unsigned long periodicity)
{
	task->name = taskName;
	task->priority = priority;
	task->period_MS = period;
	task->publishedMessages = 0;

	task->periodicity = periodicity;
}
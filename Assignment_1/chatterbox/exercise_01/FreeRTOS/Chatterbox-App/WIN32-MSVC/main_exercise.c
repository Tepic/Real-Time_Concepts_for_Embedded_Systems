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

#define INFITITY 0

/* TODO: Priorities at which the tasks are created.
 */


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
	char message[50];
	unsigned long publishedMessages;

	boolean periodicity;
};
typedef struct ChatterboxTask pTask;

/*
 * TODO: C function (prototype) for task
 */
void vTask(void *pvParameters)
{
	pTask* task = (pTask*) pvParameters;
	const char *pcTaskName = task->name;//"Task 1 is running\r\n";
	
	// volatile uint32_t ul;
	/*	volatile to ensure ul is not optimized away. */
	/* As per most tasks, this task is implemented in an infinite loop. */
	for (;;)
	{
		/* Print out the name of this task. */
		//vPrintString(pcTaskName);
		//currentTime = xTaskGetTickCount() / portTICK_PERIOD_MS;
		printf("%s",pcTaskName);

		if (task->periodicity != INFITITY)
		{
			task->publishedMessages += 1;
			if (task->publishedMessages >= task->periodicity)
			{
				printf("It has finished its execution - TASK EXECUTED!\r\n");
				break;
			}
		}

		/* Delay for a period. */
		vTaskDelay(task->period_MS * mainTASK_CHATTERBOX_OUTPUT_FREQUENCY_MS);
	}
	vTaskDelete(NULL);
}



void initTask(pTask* task, char* taskName, unsigned short priority, unsigned long period, char* message, unsigned long periodicity)
{
	task->name = taskName;
	task->priority = priority;
	task->period_MS = period;
	task->message[0] = message;
	task->publishedMessages = 0;

	task->periodicity = periodicity;
}

 

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

	initTask(&mTask1, "Task1\r\n", 253, 500, "", INFITITY);
	initTask(&mTask2, "Task2\r\n", 254, 750, "", INFITITY);
	initTask(&mTask3, "Task3\r\n", 200, 3000, "", 2);

	/* 
	 * TODO: Create the task instances.
     */
		 // TASK 1
		xTaskCreate(vTask,				/* Pointer to the function that implements the task. */
			        "Task 1",			/* Text name for the task. This is to facilitate debugging only */
					1000,				/* Stack depth - small microcontrollers will use much less stack than this. */
					&mTask1,			/* This example does not use the task parameter. */
					mTask1.priority,	/* This task will run at priority 1. */
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
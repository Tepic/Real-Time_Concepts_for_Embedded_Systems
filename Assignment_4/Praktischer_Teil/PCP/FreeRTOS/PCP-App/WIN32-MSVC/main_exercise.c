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

/* Other Includes */
#include "workerTask.h"
#include "print.h"
#include "semaphore.h"
#include "debug.h"


#define mainNUMBER_OF_SEMAPHORS					( 3 )

// TODO

#define workersUSELESS_CYCLES_PER_TIME_UNIT		( 1000000UL)
/*-----------------------------------------------------------*/

// TODO

static void vUselessLoad(uint32_t ulCycles);
static void prvTask1(void *pvParameters);
static void prvTask2(void *pvParameters);
static void prvTask3(void *pvParameters);
static void prvTask4(void *pvParameters);
static void vInitialize(TaskFunction_t taskHandler_1, TaskFunction_t taskHandler_2, TaskFunction_t taskHandler_3, TaskFunction_t taskHandler_4);

/*-----------------------------------------------------------*/

void main_exercise( void )
{
	// TODO
	SemaphoreHandle_t xSemaphore_A;
	SemaphoreHandle_t xSemaphore_B;
	SemaphoreHandle_t xSemaphore_C;

	// Should we create binary of counting semaphore?
	vSemaphoreCreateBinary(xSemaphore_A);
	vSemaphoreCreateBinary(xSemaphore_B);
	vSemaphoreCreateBinary(xSemaphore_C);

	if (xSemaphore_A == NULL || xSemaphore_B == NULL || xSemaphore_C == NULL)
	{
		vPrintString("The semaphore was created unsuccessfully.");
		vPrintString("Exiting the application.");
		return;
	}

	vPrintStringLn("Starting the application...");
#if DEBUG
	vTest(prvTask1, prvTask2, prvTask3, prvTask4);
#elif !DEBUG
	vInitialize(prvTask1, prvTask2, prvTask3, prvTask4);
#endif

	for( ;; );
}
/*-----------------------------------------------------------*/


static void vUselessLoad(uint32_t ulTimeUnits) {
uint32_t ulUselessVariable = 0;

	for (uint32_t i = 0; i < ulTimeUnits * workersUSELESS_CYCLES_PER_TIME_UNIT; i++)
	{
		ulUselessVariable = 0;
		ulUselessVariable = ulUselessVariable + 1;
	}
}

// TODO

static void prvTask1(void *pvParameters)
{
	while (true) {
		vUselessLoad(1);

	}
		
}

static void prvTask2(void *pvParameters)
{
	while (true) {

	}
}

static void prvTask3(void *pvParameters)
{
	while (true) {

	}
}

static void prvTask4(void *pvParameters)
{
	while (true) {
		vUselessLoad(2);
		//usPrioritySemaphoreWait(pSemaphore_A, prvTask4);
	}
}

void vInitialize(TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4) {

	gll_t* taskList = gll_init();
	gll_t* semaphoreList = gll_init();

	gll_t* semaphoreList_task_1 = gll_init();
	gll_t* semaphoreList_task_2 = gll_init();
	gll_t* semaphoreList_task_3 = gll_init();
	gll_t* semaphoreList_task_4 = gll_init();

	Semaphore_t* pSemaphore_A = Semaphore_Create(4, 1);
	Semaphore_t* pSemaphore_B = Semaphore_Create(5, 2);
	Semaphore_t* pSemaphore_C = Semaphore_Create(5, 3);

	gll_push(semaphoreList_task_1, pSemaphore_B);
	gll_push(semaphoreList_task_1, pSemaphore_C);
	WorkerTask_t* pTask_1 = WorkerTask_Create(taskHandler_1, 1, 5, 10, 10, semaphoreList_task_1);

	gll_push(semaphoreList_task_2, pSemaphore_A);
	gll_push(semaphoreList_task_2, pSemaphore_C);
	WorkerTask_t* pTask_2 = WorkerTask_Create(taskHandler_2, 2, 4, 3, 10, semaphoreList_task_2);

	gll_push(semaphoreList_task_3, pSemaphore_A);
	gll_push(semaphoreList_task_3, pSemaphore_B);
	WorkerTask_t* pTask_3 = WorkerTask_Create(taskHandler_3, 3, 3, 5, 10, semaphoreList_task_3);

	gll_push(semaphoreList_task_4, pSemaphore_A);
	gll_push(semaphoreList_task_4, pSemaphore_B);
	WorkerTask_t* pTask_4 = WorkerTask_Create(taskHandler_4, 4, 2, 0, 10, semaphoreList_task_4);

	// push NULL, since we do not want to use index = 0, indexing should start from 1 (e.g. Task_1)
	gll_pushBack(taskList, NULL);
	gll_pushBack(taskList, pTask_1);
	gll_pushBack(taskList, pTask_2);
	gll_pushBack(taskList, pTask_3);
	gll_pushBack(taskList, pTask_4);

	gll_pushBack(semaphoreList, pSemaphore_A);
	gll_pushBack(semaphoreList, pSemaphore_B);
	gll_pushBack(semaphoreList, pSemaphore_C);

}


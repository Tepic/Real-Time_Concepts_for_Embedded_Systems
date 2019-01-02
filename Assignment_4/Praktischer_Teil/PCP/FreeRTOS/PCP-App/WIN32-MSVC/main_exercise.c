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
void vInitialize(TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4,
	WorkerTask_t* pTask_1,
	WorkerTask_t* pTask_2,
	WorkerTask_t* pTask_3,
	WorkerTask_t* pTask_4);

/*-----------------------------------------------------------*/

void main_exercise( void )
{
	// TODO
	WorkerTask_t* pTask_1 = NULL;
	WorkerTask_t* pTask_2 = NULL;
	WorkerTask_t* pTask_3 = NULL;
	WorkerTask_t* pTask_4 = NULL;

	vPrintStringLn("Starting the application...");
#if DEBUG
	//vTest(prvTask1, prvTask2, prvTask3, prvTask4);
	vInitialize(prvTask1, prvTask2, prvTask3, prvTask4, pTask_1, pTask_2, pTask_3, pTask_4);
	vTaskStartScheduler();
#elif !DEBUG
	vInitialize(prvTask1, prvTask2, prvTask3, prvTask4, pTask_1, pTask_2, pTask_3, pTask_4);
	vTaskStartScheduler();
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
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTask1'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* workerTask_1 = (WorkerTask_t*)pvParameters;

	Semaphore_t* pSemaphore_B = Semaphore_sList_GetSemaphoreById(workerTask_1->pUsedSemaphoreList, 2); // Semaphore B ID = 2
	Semaphore_t* pSemaphore_C = Semaphore_sList_GetSemaphoreById(workerTask_1->pUsedSemaphoreList, 3); // Semaphore C ID = 3
	
	while (true) {
		vUselessLoad(1);

	}
		
}

static void prvTask2(void *pvParameters)
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

	while (true) {

	}
}

static void prvTask3(void *pvParameters)
{
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTask3'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* workerTask_3 = (WorkerTask_t*)pvParameters;

	Semaphore_t* pSemaphore_A = Semaphore_sList_GetSemaphoreById(workerTask_3->pUsedSemaphoreList, 1); // Semaphore A ID = 1
	Semaphore_t* pSemaphore_B = Semaphore_sList_GetSemaphoreById(workerTask_3->pUsedSemaphoreList, 2); // Semaphore B ID = 2

	while (true) {

	}
}

static void prvTask4(void *pvParameters)
{
	if (pvParameters == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'prvTask4'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* workerTask_4 = (WorkerTask_t*)pvParameters;

	Semaphore_t* pSemaphore_A = Semaphore_sList_GetSemaphoreById(workerTask_4->pUsedSemaphoreList, 1); // Semaphore A ID = 1
	Semaphore_t* pSemaphore_B = Semaphore_sList_GetSemaphoreById(workerTask_4->pUsedSemaphoreList, 2); // Semaphore B ID = 2

	while (true) {
		vUselessLoad(2);
		//usPrioritySemaphoreWait(pSemaphore_A, prvTask4);
	}
}

void vInitialize(TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4, 
	WorkerTask_t* pTask_1,
	WorkerTask_t* pTask_2,
	WorkerTask_t* pTask_3,
	WorkerTask_t* pTask_4) {

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
	pTask_1 = WorkerTask_Create(taskHandler_1, 1, 5, 10, 10, semaphoreList_task_1);

	gll_push(semaphoreList_task_2, pSemaphore_A);
	gll_push(semaphoreList_task_2, pSemaphore_C);
	pTask_2 = WorkerTask_Create(taskHandler_2, 2, 4, 3, 10, semaphoreList_task_2);

	gll_push(semaphoreList_task_3, pSemaphore_A);
	gll_push(semaphoreList_task_3, pSemaphore_B);
	pTask_3 = WorkerTask_Create(taskHandler_3, 3, 3, 5, 10, semaphoreList_task_3);

	gll_push(semaphoreList_task_4, pSemaphore_A);
	gll_push(semaphoreList_task_4, pSemaphore_B);
	pTask_4 = WorkerTask_Create(taskHandler_4, 4, 2, 0, 10, semaphoreList_task_4);

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


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

/*-----------------------------------------------------------*/



void vTest() {
	gll_t* taskList = gll_init();
	gll_t* semaphoreList = gll_init();

	WorkerTask_t* pTask_1 = WorkerTask_Create(prvTask1, 1, 5, 5);
	WorkerTask_t* pTask_2 = WorkerTask_Create(prvTask2, 2, 4, 7);
	WorkerTask_t* pTask_3 = WorkerTask_Create(prvTask3, 3, 3, 8);
	WorkerTask_t* pTask_4 = WorkerTask_Create(prvTask4, 4, 2, 9);
	Semaphore_t* pSemaphore_A = Semaphore_Create(4, "A");
	Semaphore_t* pSemaphore_B = Semaphore_Create(5, "B");
	Semaphore_t* pSemaphore_C = Semaphore_Create(5, "C");

	WorkerTask_vPrint(pTask_1);
	WorkerTask_vPrint(pTask_2);
	WorkerTask_vPrint(pTask_3);
	WorkerTask_vPrint(pTask_4);

	gll_pushBack(taskList, pTask_1);
	gll_pushBack(taskList, pTask_2);
	gll_pushBack(taskList, pTask_3);
	gll_pushBack(taskList, pTask_4);
	vPrintString("Size of the taskList: "); vPrintInteger(taskList->size); vPrintStringLn("");

	Semaphore_vPrint(pSemaphore_A);
	Semaphore_vPrint(pSemaphore_B);
	Semaphore_vPrint(pSemaphore_C);

	gll_pushBack(semaphoreList, pSemaphore_A);
	gll_pushBack(semaphoreList, pSemaphore_B);
	gll_pushBack(semaphoreList, pSemaphore_C);
	vPrintString("Size of the semaphoreList: "); vPrintInteger(semaphoreList->size); vPrintStringLn("");

	vPrintStringLn("After pushing to the list and popping");

	/* Get from an empty list */
	WorkerTask_vPrint(gll_get(taskList, 0));
	WorkerTask_vPrint(gll_get(taskList, 1));
	WorkerTask_vPrint(gll_get(taskList, 2));
	WorkerTask_vPrint(gll_get(taskList, 3));
	Semaphore_vPrint(pSemaphore_A);
	Semaphore_vPrint(pSemaphore_B);
	Semaphore_vPrint(pSemaphore_C);

	vPrintStringLn("After pushing to the list and popping");

	/* Get from an empty list */
	WorkerTask_vPrint(gll_get(taskList, 0));
	WorkerTask_vPrint(gll_get(taskList, 1));
	WorkerTask_vPrint(gll_get(taskList, 2));
	WorkerTask_vPrint(gll_get(taskList, 3));

	Semaphore_vPrint(pSemaphore_A);
	Semaphore_vPrint(pSemaphore_B);
	Semaphore_vPrint(pSemaphore_C);

	WorkerTask_vDestroy(pTask_1);
	WorkerTask_vDestroy(pTask_2);
	WorkerTask_vDestroy(pTask_3);
	WorkerTask_vDestroy(pTask_4);
	Semaphore_vDestroy(pSemaphore_A);
	Semaphore_vDestroy(pSemaphore_B);
	Semaphore_vDestroy(pSemaphore_C);
}

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

	vTest();
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
	// TODO
		
}

static void prvTask2(void *pvParameters)
{
	// TODO
}

static void prvTask3(void *pvParameters)
{
	// TODO
}

static void prvTask4(void *pvParameters)
{
	// TODO
}
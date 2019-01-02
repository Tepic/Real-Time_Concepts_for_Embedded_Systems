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

void assert() {
	vPrintStringLn("Test error asserted");
}

void testPassed() {
	vPrintStringLn("Test passed");
}


void vTest_SemaphoreIsNotAquired(Semaphore_t* pSemaphore_A, WorkerTask_t* pTask_1, gll_t* taskList) {
	int8_t retVal = PIP_SemaphoreTake(pSemaphore_A, pTask_1, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	testPassed();
}

void vTest_SemaphoreIsAlreadyAquired(Semaphore_t* pSemaphore_A, WorkerTask_t* pTask_1, WorkerTask_t* pTask_2, gll_t* taskList) {
	int8_t retVal = PIP_SemaphoreTake(pSemaphore_A, pTask_2, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	uint8_t uOld_Task_2_active_priority = WorkerTask_uGetActivePriority(pTask_2);
	retVal = PIP_SemaphoreTake(pSemaphore_A, pTask_1, taskList);

	// Should be retVal == 0
	if (retVal != 0) {
		assert();
		return;
	}

	if (WorkerTask_uGetActivePriority(pTask_1) != WorkerTask_uGetActivePriority(pTask_2)) {
		assert();
		return;
	}

	if (uOld_Task_2_active_priority == WorkerTask_uGetActivePriority(pTask_2)) {
		assert();
		return;
	}

	testPassed();
}

void vTest_SemaphoreIsAlreadyAquiredAndGetsReleased(Semaphore_t* pSemaphore_A, WorkerTask_t* blockedTask, WorkerTask_t* blockingTask, gll_t* taskList) {
	int8_t retVal = PIP_SemaphoreTake(pSemaphore_A, blockingTask, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	retVal = PIP_SemaphoreTake(pSemaphore_A, blockedTask, taskList);

	PIP_vSemaphoreGive(pSemaphore_A, blockingTask);
	if ( WorkerTask_uGetActivePriority(blockingTask) != WorkerTask_uGetNominalPriority(blockingTask) ){
		assert();
		return;
	}

	testPassed();
}

void vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredAndBlockedTasksOnItShouldBeSortedInAList(Semaphore_t* pSemaphore_A, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* pTask_2 = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* blockingTask = gll_get(taskList, 4);

	int8_t retVal = PIP_SemaphoreTake(pSemaphore_A, blockingTask, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	PIP_SemaphoreTake(pSemaphore_A, pTask_3, taskList);
	PIP_SemaphoreTake(pSemaphore_A, pTask_1, taskList);
	PIP_SemaphoreTake(pSemaphore_A, pTask_2, taskList);

	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	testPassed();
}

void vTast_PrintList(gll_t* integerList) {

	if (integerList->size == 0) {
		vPrintStringLn("Empty list");
		return;
	}

	for (uint8_t index = 0; index < integerList->size; ++index) {
		uint8_t* pData = (uint8_t*) gll_get(integerList, index);
		vPrintString("Index: ");  vPrintInteger(index); vPrintString(" , value: "); vPrintInteger(*pData); vPrintStringLn("");
	}
}

void vTest_ListAddDescending(gll_t* pTaskList, uint8_t* data) {

	if (pTaskList->size == 0) {
		gll_push(pTaskList, data);
		return;
	}

	for (uint8_t index = 0; index < pTaskList->size; ++index) {
		uint8_t* pListData = (uint8_t*)gll_get(pTaskList, index);
		
		if (*data >= *pListData) {
			gll_add(pTaskList, data, index);
			return;
		}
	}

	gll_pushBack(pTaskList, data);
}

void vTast_TestAddList() {
	gll_t* integerList = gll_init();
	
	uint8_t a = 1;
	uint8_t b = 2;
	uint8_t c = 3;
	uint8_t d = 4;
	uint8_t data_1, data_2, data_3, data_4, data_5, data_6; 

	//gll_add(integerList, &a, 0); 
	//gll_add(integerList, &b, 0);
	//gll_add(integerList, &c, 0);
	//gll_add(integerList, &d, 0);
	
	vPrintStringLn("Before Calling 'vTest_ListAddDescending' list has following content:"); 
	vTast_PrintList(integerList);

	data_1 = 5;
	vTest_ListAddDescending(integerList, &data_1);
	data_2 = 2;
	vTest_ListAddDescending(integerList, &data_2);
	data_3 = 3;
	vTest_ListAddDescending(integerList, &data_3);
	data_4 = 3;
	vTest_ListAddDescending(integerList, &data_4);
	data_5 = 1;
	vTest_ListAddDescending(integerList, &data_5);
	data_6 = 6;
	vTest_ListAddDescending(integerList, &data_6);

	vPrintStringLn("After Calling 'vTest_ListAddDescending' list has following content:");
	vTast_PrintList(integerList);
}

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

	// push NULL, since we do not want to use index = 0, indexing should start from 1 (e.g. Task_1)
	gll_pushBack(taskList, NULL);
	gll_pushBack(taskList, pTask_1);
	gll_pushBack(taskList, pTask_2);
	gll_pushBack(taskList, pTask_3);
	gll_pushBack(taskList, pTask_4);

	gll_pushBack(semaphoreList, pSemaphore_A);
	gll_pushBack(semaphoreList, pSemaphore_B);
	gll_pushBack(semaphoreList, pSemaphore_C);

	/* Tests */
	//vTest_SemaphoreIsNotAquired(pSemaphore_A, pTask_1, taskList);
	//vTest_SemaphoreIsAlreadyAquired(pSemaphore_A, pTask_1, pTask_2, taskList);
	//vTest_SemaphoreIsAlreadyAquiredAndGetsReleased(pSemaphore_A, pTask_1, pTask_2, taskList);
	vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredAndBlockedTasksOnItShouldBeSortedInAList(pSemaphore_A, taskList);
	//vTast_TestAddList();

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
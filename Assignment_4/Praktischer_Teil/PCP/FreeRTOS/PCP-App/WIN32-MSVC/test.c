#include "test.h"

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


void assert() {
	vPrintStringLn("Test error asserted");
}

void testPassed() {
	vPrintStringLn("Test passed");
}


void vTest_SemaphoreIsNotAquired(Semaphore_t* pSemaphore_A, WorkerTask_t* pTask_1, gll_t* taskList) {
	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_1, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	testPassed();
}

void vTest_SemaphoreIsAlreadyAquired(Semaphore_t* pSemaphore_A, WorkerTask_t* pTask_1, WorkerTask_t* pTask_2, gll_t* taskList) {
	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_2, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	uint8_t uOld_Task_2_active_priority = pTask_2->uActivePriority;
	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_1, taskList);

	// Should be retVal == 0
	if (retVal != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_2->uActivePriority) {
		assert();
		return;
	}

	if (uOld_Task_2_active_priority == pTask_2->uActivePriority) {
		assert();
		return;
	}

	testPassed();
}

void vTest_SemaphoreIsAlreadyAquiredAndGetsReleased(Semaphore_t* pSemaphore_A, WorkerTask_t* blockedTask, WorkerTask_t* blockingTask, gll_t* taskList) {
	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);

	if (retVal != 0) {
		assert();
		return;
	}

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockedTask, taskList);

	PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);
	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	testPassed();
}

void vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredAndBlockedTasksOnItShouldBeSortedInAList(Semaphore_t* pSemaphore_A, gll_t* taskList) {

	WorkerTask_t* blockingTask = gll_get(taskList, 1);
	WorkerTask_t* pTask_2 = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* pTask_4 = gll_get(taskList, 4);

	vPrintStringLn("\nThe BLOCKING Task before aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);

	vPrintStringLn("The BLOCKING Task after aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	/*
	WorkerTask_t* blockingTask = gll_get(taskList, 2);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_1, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_4, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_2, taskList);
	*/
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_4, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_2, taskList);

	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	testPassed();
}

void vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredByHighPriorityTaskAndBlockedTasksOnItShouldBeSortedInAList(Semaphore_t* pSemaphore_A, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* blockingTask = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* pTask_4 = gll_get(taskList, 4);

	vPrintStringLn("\nThe BLOCKING Task before aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);

	vPrintStringLn("The BLOCKING Task after aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_1, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_4, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);

	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	testPassed();
}

void vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredByAndAfterwardsReleaseTheSemaphore(Semaphore_t* pSemaphore_A, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* blockingTask = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* pTask_4 = gll_get(taskList, 4);

	uint8_t blockingTaskPriorityOld = blockingTask->uActivePriority;

	vPrintStringLn("\nThe BLOCKING Task before aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);

	vPrintStringLn("The BLOCKING Task after aquiring the semaphore: ");
	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_1, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_4, taskList);
	PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);

	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("After releasing BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	if (blockingTask->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	testPassed();
}


void vTest_WhenTaskReleasesAllAcquiredSemaphoresThenTaskShouldHaveNominalPriority(gll_t* semaphoreList, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* blockingTask = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* pTask_4 = gll_get(taskList, 4);

	Semaphore_t* pSemaphore_A = gll_get(semaphoreList, 0);
	Semaphore_t* pSemaphore_B = gll_get(semaphoreList, 1);
	Semaphore_t* pSemaphore_C = gll_get(semaphoreList, 2);

	uint8_t blockingTaskPriorityOld = blockingTask->uActivePriority;

	/* Lock Semaphore A */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	WorkerTask_vPrint(blockingTask);

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	/* Lock Semaphore C */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	WorkerTask_vPrint(blockingTask);

	retVal = PIP_BinarySemaphoreTake(pSemaphore_C, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");  vPrintChar(pSemaphore_B->uId - 1 + '0');
	PIP_vBinarySemaphoreGive(pSemaphore_B, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_B->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (blockingTask->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	testPassed();
}


void vTest_WhenTaskAcquiresSemaphorsThenAllOtherTasksAcquiringThatSemaphoreShouldBeBlocked(gll_t* semaphoreList, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* blockingTask = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* pTask_4 = gll_get(taskList, 4);

	Semaphore_t* pSemaphore_A = gll_get(semaphoreList, 0);
	Semaphore_t* pSemaphore_B = gll_get(semaphoreList, 1);
	Semaphore_t* pSemaphore_C = gll_get(semaphoreList, 2);

	uint8_t blockingTaskPriorityOld = blockingTask->uActivePriority;

	/* Lock Semaphore A */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	WorkerTask_vPrint(blockingTask);

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	/* Lock Semaphore C */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	WorkerTask_vPrint(blockingTask);

	retVal = PIP_BinarySemaphoreTake(pSemaphore_C, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vPrint(blockingTask);
	vPrintStringLn("");

	if (retVal != 0) {
		assert();
		return;
	}

	WorkerTask_vListPrintPriority(pSemaphore_C->pBlockedTaskList);
	vPrintStringLn("The BLOCKING Task: ");
	WorkerTask_vPrint(blockingTask);

	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");  vPrintChar(pSemaphore_B->uId - 1 + '0');
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_C, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_C->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (retVal != 0) {
		assert();
		return;
	}

	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");  vPrintChar(pSemaphore_A->uId - 1 + '0');
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (retVal != 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	testPassed();
}

void vTest_WhenSemaphoreIsReleasedThenReleasingTaskGetsMaximumPriorityOfAllBlockedTasks(gll_t* semaphoreList, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 1);
	WorkerTask_t* pTask_2 = gll_get(taskList, 2);
	WorkerTask_t* pTask_3 = gll_get(taskList, 3);
	WorkerTask_t* blockingTask = gll_get(taskList, 4);

	Semaphore_t* pSemaphore_A = gll_get(semaphoreList, 0);
	Semaphore_t* pSemaphore_B = gll_get(semaphoreList, 1);
	Semaphore_t* pSemaphore_C = gll_get(semaphoreList, 2);

	uint8_t blockingTaskPriorityOld = blockingTask->uActivePriority;

	/* The Blocking Task locks Semaphore A */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");

	int8_t retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	/* The Blocking Task locks Semaphore B */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  
	vPrintChar(pSemaphore_B->uId - 1 + '0'); vPrintStringLn("");


	retVal = PIP_BinarySemaphoreTake(pSemaphore_B, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	/* The Task 3 tries to lock Semaphore A */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority == blockingTask->uNominalPriority ||
		blockingTask->uActivePriority != pTask_3->uActivePriority) {
		assert();
		return;
	}

	/* The Task 3 tries to lock Semaphore B */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_B, pTask_3, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority == blockingTask->uNominalPriority ||
		blockingTask->uActivePriority != pTask_3->uActivePriority) {
		assert();
		return;
	}

	/* The Task 2 tries to lock Semaphore A */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_2, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority == blockingTask->uNominalPriority ||
		blockingTask->uActivePriority != pTask_2->uActivePriority) {
		assert();
		return;
	}

	/* The Task 1 tries to lock Semaphore B */
	vPrintString("\nThe BLOCKING Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_B, pTask_1, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority == blockingTask->uNominalPriority ||
		blockingTask->uActivePriority != pTask_1->uActivePriority) {
		assert();
		return;
	}

	uint8_t size_A = pSemaphore_A->pBlockedTaskList->size;
	uint8_t size_B = pSemaphore_B->pBlockedTaskList->size;

	/* The Blocking Task unlocks Semaphore B */
	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_vBinarySemaphoreGive(pSemaphore_B, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_B->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (retVal != 0) {
		assert();
		return;
	}

	/* The Task 3 unlocks Semaphore B */
	vPrintString("The BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + '0'); vPrintStringLn("");

	retVal = PIP_vBinarySemaphoreGive(pSemaphore_B, pTask_3);
	WorkerTask_vListPrintPriority(pSemaphore_B->pBlockedTaskList);


	if (retVal != 0) {
		assert();
		return;
	}

	/* The Blocking Task unlocks Semaphore A */
	blockingTaskPriorityOld = blockingTask->uActivePriority;

	vPrintString("The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");  
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (retVal != 0) {
		assert();
		return;
	}

	/* The Task 2 unlocks Semaphore A */
	blockingTaskPriorityOld = blockingTask->uActivePriority;

	vPrintString("The BLOCKING Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_2);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	/* The Task 3 unlocks Semaphore A */
	blockingTaskPriorityOld = blockingTask->uActivePriority;

	vPrintString("The BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + '0'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_3);
	WorkerTask_vListPrintPriority(pSemaphore_A->pBlockedTaskList);
	WorkerTask_vPrint(blockingTask);

	if (retVal != 0) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	if (pSemaphore_A->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	testPassed();
}


void vTast_PrintList(gll_t* integerList) {

	if (integerList->size == 0) {
		vPrintStringLn("Empty list");
		return;
	}

	for (uint8_t index = 0; index < integerList->size; ++index) {
		uint8_t* pData = (uint8_t*)gll_get(integerList, index);
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

void vTest(TaskFunction_t taskHandler_1,
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

	/* Tests */
	//vTest_SemaphoreIsNotAquired(pSemaphore_A, pTask_1, taskList);
	//vTest_SemaphoreIsAlreadyAquired(pSemaphore_A, pTask_1, pTask_2, taskList);
	//vTest_SemaphoreIsAlreadyAquiredAndGetsReleased(pSemaphore_A, pTask_1, pTask_2, taskList);
	//vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredAndBlockedTasksOnItShouldBeSortedInAList(pSemaphore_A, taskList);
	//vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredByHighPriorityTaskAndBlockedTasksOnItShouldBeSortedInAList(pSemaphore_A, taskList);
	//vTest_WhenWeTryToAcquireSemaphoreItIsAlreadyAcquiredByAndAfterwardsReleaseTheSemaphore(pSemaphore_A, taskList);
	//vTest_WhenTaskReleasesAllAcquiredSemaphoresThenTaskShouldHaveNominalPriority(semaphoreList, taskList);
	//vTest_WhenTaskAcquiresSemaphorsThenAllOtherTasksAcquiringThatSemaphoreShouldBeBlocked(semaphoreList, taskList);
	vTest_WhenSemaphoreIsReleasedThenReleasingTaskGetsMaximumPriorityOfAllBlockedTasks(semaphoreList, taskList);
	//vTast_TestAddList();

	WorkerTask_vDestroy(pTask_1);
	WorkerTask_vDestroy(pTask_2);
	WorkerTask_vDestroy(pTask_3);
	WorkerTask_vDestroy(pTask_4);
	Semaphore_vDestroy(pSemaphore_A);
	Semaphore_vDestroy(pSemaphore_B);
	Semaphore_vDestroy(pSemaphore_C);
	gll_destroy(taskList);
	gll_destroy(semaphoreList);
	gll_destroy(semaphoreList_task_1);
	gll_destroy(semaphoreList_task_2);
	gll_destroy(semaphoreList_task_3);
	gll_destroy(semaphoreList_task_4);
}
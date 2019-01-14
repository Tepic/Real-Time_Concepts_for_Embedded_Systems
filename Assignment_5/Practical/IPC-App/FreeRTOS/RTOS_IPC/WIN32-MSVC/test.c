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

	WorkerTask_t* pTask_1 = gll_get(taskList, 0);
	WorkerTask_t* blockingTask = gll_get(taskList, 1);
	WorkerTask_t* pTask_3 = gll_get(taskList, 2);
	WorkerTask_t* pTask_4 = gll_get(taskList, 3);

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

	WorkerTask_t* pTask_1 = gll_get(taskList, 0);
	WorkerTask_t* pTask_2 = gll_get(taskList, 1);
	WorkerTask_t* pTask_3 = gll_get(taskList, 2);
	WorkerTask_t* blockingTask = gll_get(taskList, 3);

	Semaphore_t* pSemaphore_A = gll_get(semaphoreList, 0);
	Semaphore_t* pSemaphore_B = gll_get(semaphoreList, 1);
	Semaphore_t* pSemaphore_C = gll_get(semaphoreList, 2);

	int8_t retVal = 0;
	uint8_t blockingTaskPriorityOld = 0;

	uint8_t uCounter = 1;

	blockingTaskPriorityOld = blockingTask->uActivePriority;

	/* The Blocking Task locks Semaphore A */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");  
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	/* The Blocking Task locks Semaphore B */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_B, blockingTask, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	/* The Task 3 tries to lock Semaphore A unsuccessfully */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

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

	/* The Task 3 tries to lock Semaphore B unsuccessfully */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

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

	/* The Task 2 tries to lock Semaphore A unsuccessfully */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

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

	/* The Blocking Task unlocks Semaphore B */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	blockingTaskPriorityOld = blockingTask->uActivePriority;
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_B, blockingTask);

	if (retVal != SEMAPHORE_OK) {
		assert();
		return;
	}

	if (blockingTaskPriorityOld == blockingTask->uActivePriority) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != blockingTask->uNominalPriority) {
		assert();
		return;
	}

	/* The Task 3 unlocks Semaphore B */
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_vBinarySemaphoreGive(pSemaphore_B, pTask_3);

	if (retVal == SEMAPHORE_OK) { // Cannot unlock the non-locked semaphore
		assert();
		return;
	}

	/* The Blocking Task unlocks Semaphore A */
	blockingTaskPriorityOld = blockingTask->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The BLOCKING Task #"); vPrintInteger(blockingTask->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, blockingTask);

	if (retVal != SEMAPHORE_OK) {
		assert();
		return;
	}

	if (blockingTask->uActivePriority != pTask_3->uActivePriority) {
		assert();
		return;
	}


	if (blockingTask->uActivePriority == blockingTaskPriorityOld) { // Priority should be decreased actually after releasing the lock
		assert();
		return;
	}

	if (blockingTask->uActivePriority == blockingTask->uNominalPriority) { // Still there is one blocked task on Semaphore A, 
																		   // blocking task should still not have its nominal priority
		assert();
		return;
	}


	/* The Task 2 unlocks Semaphore A */
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_2);
	
	if (retVal == SEMAPHORE_OK) { // Cannot unlock the non-locked semaphore
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}


	/* The Task 3 unlocks Semaphore A */
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_3);

	if (retVal == SEMAPHORE_OK) { // Cannot unlock the non-locked semaphore
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 tries to lock Semaphore A */
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_2, taskList);
	if (retVal != SEMAPHORE_OK) { // Lock should pass since BLOCKING task has already unlocked the Semaphore
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 3 tries to lock Semaphore A */
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);
	if (retVal == SEMAPHORE_OK) { // Lock should fail since Task 2 has already unlocked the Semaphore
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 unlocks Semaphore A successfully */
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_2);

	if (retVal != SEMAPHORE_OK) { // Task 2 holds the Semaphore A, it should be able to unlock it now
		assert();
		return;
	}

	if (blockingTaskPriorityOld != pTask_2->uActivePriority) { // Active priority of the Task 2 should stay the same 
															   // its period is higher than Period of task 3
		assert();
		return;
	}


	/* The Task 3 tries to lock Semaphore B */
	blockingTaskPriorityOld = pTask_3->uActivePriority;
	
	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);
	if (retVal != SEMAPHORE_OK) { // Lock should pass since no other task holds the Semaphore
		assert();
		return;
	}

	if (blockingTaskPriorityOld != pTask_3->uActivePriority) { // Active priority of the Task 3 should stay the same 
															   // as no other task is blocked on Semaphore B
		assert();
		return;
	}


	/* The Task 3 tries to lock Semaphore B */
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_A, pTask_3, taskList);
	if (retVal == SEMAPHORE_OK) { // Lock should not pass since Task 3 has already locked semaphore A
		assert();
		return;
	}

	if (blockingTaskPriorityOld != pTask_3->uActivePriority) { // Active priority of the Task 3 should stay the same 
															   // as no other task is blocked on Semaphore B
		assert();
		return;
	}

	/* The Task 3 unlocks Semaphore A successfully */
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" releasing the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");
	retVal = PIP_vBinarySemaphoreGive(pSemaphore_A, pTask_3);

	if (retVal != SEMAPHORE_OK) { // Task 2 holds the Semaphore A, it should be able to unlock it now
		assert();
		return;
	}
	
	if (pTask_3->uNominalPriority != pTask_3->uActivePriority) {
															  
		assert();
		return;
	}

	if (blockingTaskPriorityOld != pTask_3->uActivePriority) { // Active priority of the Task 2 should stay the same 
															   // its period is higher than Period of task 3
		assert();
		return;
	}

	/* No other resource is locked, all blocked task lists should be empty*/
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


void vTest_TestQuestionPIP(gll_t* semaphoreList, gll_t* taskList) {

	WorkerTask_t* pTask_1 = gll_get(taskList, 0);
	WorkerTask_t* pTask_2 = gll_get(taskList, 1);
	WorkerTask_t* pTask_3 = gll_get(taskList, 2);
	WorkerTask_t* pTask_4 = gll_get(taskList, 3);

	Semaphore_t* pSemaphore_A = gll_get(semaphoreList, 0);
	Semaphore_t* pSemaphore_B = gll_get(semaphoreList, 1);
	Semaphore_t* pSemaphore_C = gll_get(semaphoreList, 2);

	int8_t retVal = 0;
	uint8_t blockingTaskPriorityOld = 0;

	uint8_t uCounter = 1;

	/* The Task 4 locks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_4->uActivePriority;

	vPrintString("\n"); 
	vPrintString("Time: "); vPrintInteger(2); vPrintString("  ");
	vPrintInteger(uCounter++); vPrintString(".The Task #"); vPrintInteger(pTask_4->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_A, pTask_4, taskList);
	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 locks Semaphore C successfully */
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(4); vPrintString("  ");
	vPrintInteger(uCounter++);  
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_C->uId - 1 + 'A'); vPrintStringLn("");

	retVal = PIP_BinarySemaphoreTake(pSemaphore_C, pTask_2, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != pTask_2->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 unlocks Semaphore C successfully*/
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(6); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_C->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_C, pTask_2, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_2->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 fails to lock Semaphore A */
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(8); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_A, pTask_2, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority ||
		pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	// TODO: Should Task 2 who is blocked by task 4 also inherit priority of task 1?
	if (pTask_2->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_2->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uActivePriority) {
		assert();
		return;
	}

	/* The Task 4 locks Semaphore B successfully */
	blockingTaskPriorityOld = pTask_4->uActivePriority;

	vPrintString("\n"); 
	vPrintString("Time: "); vPrintInteger(9); vPrintString("  ");
	vPrintInteger(uCounter++);  vPrintString(".The Task #"); vPrintInteger(pTask_4->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_B, pTask_4, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_2->uActivePriority){
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority ||
		pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	// TODO: Should Task 2 who is blocked by task 4 also inherit priority of task 1?
	if (pTask_2->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_2->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uActivePriority) {
		assert();
		return;
	}

	/* The Task 1 fails to lock Semaphore B */
	blockingTaskPriorityOld = pTask_1->uActivePriority;

	vPrintString("\n"); 
	vPrintString("Time: "); vPrintInteger(11); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_B, pTask_1, taskList);
	if (retVal == 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority ||
		pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_2->uNominalPriority) {
		assert();
		return;
	}

	// TODO: Should Task 2 who is blocked by task 4 also inherit priority of task 1?
	if (pTask_2->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_2->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 2) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uActivePriority) {
		assert();
		return;
	}


	/* The Task 4 unlocks Semaphore B successfully */
	blockingTaskPriorityOld = pTask_4->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(12); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_4->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_B, pTask_4, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == pTask_4->uNominalPriority ) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority ||
		pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 1 locks Semaphore B successfully*/
	blockingTaskPriorityOld = pTask_1->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(12); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_B, pTask_1, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_1->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 1 unlocks Semaphore B successfully*/
	blockingTaskPriorityOld = pTask_1->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(13); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_B, pTask_1, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum == pTask_1->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if(pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	/* The Task 1 locks Semaphore C successfully*/
	blockingTaskPriorityOld = pTask_1->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(14); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_C->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_C, pTask_1, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum != pTask_1->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	/* The Task 1 unlocks Semaphore C successfully */
	blockingTaskPriorityOld = pTask_1->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(15); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_1->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_C->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_C, pTask_1, taskList);
	if (retVal != 0) {
		assert();
		return;
	}
	
	if (pSemaphore_C->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_C->uLockedByTaskNum == pTask_1->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 1) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != pTask_1->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	/* The Task 4 unlocks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_4->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(18); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_4->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_A, pTask_4, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == pTask_4->uTaskNumber) {
		assert();
		return;
	}

	if (pSemaphore_A->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_4->uActivePriority == blockingTaskPriorityOld) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	/* The Task 2 locks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(18); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_A, pTask_2, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_2->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 2 unlocks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_2->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(19); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_2->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_A, pTask_2, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == pTask_2->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != pTask_2->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_2->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}


	/* The Task 3 locks Semaphore B successfully */
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(22); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_B, pTask_3, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != pTask_3->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}


	/* The Task 3 locks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n");
	vPrintString("Time: "); vPrintInteger(23); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" acquirying the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreWait(pSemaphore_A, pTask_3, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != pTask_3->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}


	/* The Task 3 unlocks Semaphore A successfully*/
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(25); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_A->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_A, pTask_3, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_A->uLockedByTaskNum == pTask_3->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}

	/* The Task 3 unlocks Semaphore B successfully*/
	blockingTaskPriorityOld = pTask_3->uActivePriority;

	vPrintString("\n"); vPrintString("Time: "); vPrintInteger(27); vPrintString("  ");
	vPrintInteger(uCounter++);
	vPrintString(".The Task #"); vPrintInteger(pTask_3->uTaskNumber); vPrintString(" unlocking the semaphore ");
	vPrintChar(pSemaphore_B->uId - 1 + 'A'); vPrintStringLn("");

	retVal = usPrioritySemaphoreSignal(pSemaphore_B, pTask_3, taskList);
	if (retVal != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum != SEMAPHORE_AQUIRED_BY_NONE) {
		assert();
		return;
	}

	if (pSemaphore_B->uLockedByTaskNum == pTask_3->uTaskNumber) {
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

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority == pTask_4->uActivePriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != pTask_3->uNominalPriority) {
		assert();
		return;
	}

	if (pTask_3->uActivePriority != blockingTaskPriorityOld) {
		assert();
		return;
	}


	/* Check if all active priorities are now equal to the nominal*/
	if (pTask_1->uNominalPriority != pTask_1->uActivePriority) {

		assert();
		return;
	}

	if (pTask_2->uNominalPriority != pTask_2->uActivePriority) {

		assert();
		return;
	}

	if (pTask_3->uNominalPriority != pTask_3->uActivePriority) {

		assert();
		return;
	}

	if (pTask_4->uActivePriority != pTask_4->uActivePriority) { // Active priority of the Task 2 should stay the same 
																// its period is higher than Period of task 3
		assert();
		return;
	}


	/* No other resource is locked, all blocked task lists should be empty*/
	if (pSemaphore_A->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pSemaphore_B->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pSemaphore_C->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	/* No tasks should be blocked by other tasks*/
	if (pTask_1->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_2->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_3->pBlockedTaskList->size != 0) {
		assert();
		return;
	}

	if (pTask_4->pBlockedTaskList->size != 0) {
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

	gll_add(integerList, &a, 0); 
	gll_add(integerList, &b, 1);
	gll_add(integerList, &c, 1);
	gll_add(integerList, &d, 1);

	vPrintStringLn("Before Calling 'vTest_ListAddDescending' list has following content:");
	vTast_PrintList(integerList);

	vPrintStringLn("After Calling 'vTest_ListAddDescending' list has following content:");
	vTast_PrintList(integerList);
}

void vTest_TestAddingToTheList(gll_t* pTaskList) {

	WorkerTask_t* pTask_1 = gll_get(pTaskList, 0);
	WorkerTask_t* pTask_1_1 = WorkerTask_Create(999, 1, 5, 0, 0, 3, 999,NULL);
	WorkerTask_t* pTask_1_2 = WorkerTask_Create(999, 1, 5, 0, 0, 3, 999, NULL);

	WorkerTask_t* pTask_2 = gll_get(pTaskList, 1); 
	WorkerTask_t* pTask_2_1 = WorkerTask_Create(999, 2, 4, 0, 0, 3, 999, NULL);
	WorkerTask_t* pTask_2_2 = WorkerTask_Create(999, 2, 4, 0, 0, 3, 999, NULL);

	WorkerTask_t* pTask_3 = gll_get(pTaskList, 2);
	WorkerTask_t* pTask_4 = gll_get(pTaskList, 3);

	gll_t* pTaskListUnderTest = gll_init();

	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_4);
	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_3);

	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_2);
	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_2_1);
	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_2_2);

	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_1);
	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_1_1);
	WorkerTask_vListAddTaskDescendingPriorityOrder(pTaskListUnderTest, pTask_1_2);
	

	WorkerTask_vListPrintPriority(pTaskListUnderTest);
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
	WorkerTask_t* pTask_1 = WorkerTask_Create(taskHandler_1, 1, 5, 0, 0, 10, semaphoreList_task_1,NULL);

	gll_push(semaphoreList_task_2, pSemaphore_A);
	gll_push(semaphoreList_task_2, pSemaphore_C);
	WorkerTask_t* pTask_2 = WorkerTask_Create(taskHandler_2, 2, 4, 0, 0, 10, semaphoreList_task_2, NULL);

	gll_push(semaphoreList_task_3, pSemaphore_A);
	gll_push(semaphoreList_task_3, pSemaphore_B);
	WorkerTask_t* pTask_3 = WorkerTask_Create(taskHandler_3, 3, 3, 0, 0, 10, semaphoreList_task_3, NULL);

	gll_push(semaphoreList_task_4, pSemaphore_A);
	gll_push(semaphoreList_task_4, pSemaphore_B);
	WorkerTask_t* pTask_4 = WorkerTask_Create(taskHandler_4, 4, 2, 0, 0, 10, semaphoreList_task_4, NULL);

	// push NULL, since we do not want to use index = 0, indexing should start from 1 (e.g. Task_1)
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
	//vTest_WhenSemaphoreIsReleasedThenReleasingTaskGetsMaximumPriorityOfAllBlockedTasks(semaphoreList, taskList);

	//vTast_TestAddList();
	//vTest_TestAddingToTheList(taskList);
	vTest_TestQuestionPIP(semaphoreList, taskList);

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
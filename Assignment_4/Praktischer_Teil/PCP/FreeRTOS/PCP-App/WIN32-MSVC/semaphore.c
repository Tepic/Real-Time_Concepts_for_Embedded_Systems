#pragma once
#include "semaphore.h"

// Make Semaphore_t private
struct semaphore
{
	xSemaphoreHandle semphHandle;
	uint8_t* uId;
	uint8_t uPriorityCeiling;
	uint8_t uAcquiredByTaskNum;

};

Semaphore_t* Semaphore_vCreate(uint8_t uPriorityCeiling, const uint8_t* uId) {

	Semaphore_t* pSemaphoreHandle = (Semaphore_t*)malloc(sizeof(Semaphore_t));

	pSemaphoreHandle->uPriorityCeiling = uPriorityCeiling;
	pSemaphoreHandle->uId = uId;
	pSemaphoreHandle->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;
}

void  Semaphore_vDestroy(Semaphore_t* pSemaphore) {
	free(pSemaphore);
}

uint8_t Semaphore_vGetId(Semaphore_t* pSemaphore) {

	if (pSemaphore == NULL) {
		vPrintStringLn("Error in function 'pSemaphoreHandle'. NULL Pointer");
	}
	return *(pSemaphore->uId);
}

void PIP_vSemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskToAquireResource == NULL || pTaskList == NULL) {
		vPrintStringLn("Error in function 'PIP_vSemaphoreTake'. NULL Pointer");
	}

	if (isSemaphoreAcquired(pSemaphore)) {
		// Transmit its active priority to the task that holds the semaphore
		transmitTasksActivePriority(pSemaphore, pTaskToAquireResource, pTaskList);
		// Current task gets blocked on a semaphore
		vTaskSuspend(WorkerTask_vGetHandle(pTaskToAquireResource));
		printf("%s%d%s%c%s", "Task ", WorkerTask_vGetTaskNumber(pTaskToAquireResource), " failed to acquire semaphore ", pSemaphore->uId, "Task gets blocked");
		return;
	}

	// Since lock is successful, update semaphore's info; semaphore locked by the current task
	pSemaphore->uAcquiredByTaskNum = WorkerTask_vGetTaskNumber(pTaskToAquireResource);

	// TODO: change priority of the task for ICPP
	// acquire the semaphore
	vPrintString("Task "); vPrintInteger(WorkerTask_vGetTaskNumber(pTaskToAquireResource)); 
	vPrintString(" acquired resource "); vPrintInteger(Semaphore_vGetId(pSemaphore));
	//TODO: ICCP 
	// vPrintString("Task <x> acquired resource <y> and changed its priority from <i> to <j>.");
}

void PIP_vSemaphoreGive(Semaphore_t* pSemaphoreHandle, WorkerTask_t* pTaskToReleaseResource) {

	if (pSemaphoreHandle == NULL) {
		vPrintStringLn("Error in function 'PIP_vSemaphoreGive'. NULL Pointer");
	}

	pSemaphoreHandle->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;
	// Current task blocked on a semaphore is activated on unlock
	vTaskResume(WorkerTask_vGetHandle(pTaskToReleaseResource));
	//TODO: Add output here
}

/* Transmit its active priority to the task that holds the semaphore */
//void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, WorkerTaskList_t* pTaskList) {
void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList) {
	
	for (uint8_t index = 0; index < pTaskList->size; ++index) {

		WorkerTask_t* pTaskThatHoldsSemaphore = gll_get(pTaskList, index);

		if (pSemaphore->uAcquiredByTaskNum == WorkerTask_vGetTaskNumber(pTaskThatHoldsSemaphore)) {

			uint8_t blockedTaskPriority = WorkerTask_vGetActivePriority(pBlockedTask);
			WorkerTask_vSetActivePriority(pTaskThatHoldsSemaphore, blockedTaskPriority);

			vPrintString("Task "); vPrintInteger(WorkerTask_vGetTaskNumber(pBlockedTask));  
			vPrintString(" transmit its active priority "); vPrintInteger(blockedTaskPriority);   
			vPrintString("to the task "); vPrintInteger(WorkerTask_vGetTaskNumber(pTaskThatHoldsSemaphore));
			vPrintString(" that holds the semaphore "); vPrintString(pSemaphore->uId);
			return;
		}
	}

	vPrintStringLn("Error in 'transmitTasksActivePriority' function");
}

bool_t isSemaphoreAcquired(Semaphore_t* pSemaphore) {

	if (pSemaphore == NULL) {
		vPrintStringLn("Error in function 'isSemaphoreAcquired'. NULL Pointer");
	}

	if (pSemaphore->uAcquiredByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		return false;
	}
	return true;
}

void Semaphore_vPrint(Semaphore_t* semaphore) {
	if (semaphore == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vPrint'. NULL Ptr");
		return;
	}

	vPrintString("Semaphore_t: [uId: "); vPrintString(semaphore->uId);
	vPrintString(", uPriorityCeiling: "); vPrintInteger(semaphore->uPriorityCeiling);
	vPrintString(", uAcquiredByTaskNum: "); vPrintInteger(semaphore->uAcquiredByTaskNum); vPrintStringLn("]");
}


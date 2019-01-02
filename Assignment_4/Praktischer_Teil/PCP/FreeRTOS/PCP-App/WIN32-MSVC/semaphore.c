#pragma once
#include "semaphore.h"

// Make Semaphore_t private
struct semaphore
{
	SemaphoreHandle_t semphHandle;
	uint8_t* uId;
	uint8_t uPriorityCeiling;
	uint8_t uAcquiredByTaskNum;

};

Semaphore_t* Semaphore_Create(uint8_t uPriorityCeiling, const uint8_t* uId) {

	Semaphore_t* pSemaphore = (Semaphore_t*)malloc(sizeof(Semaphore_t));
	pSemaphore->semphHandle = xSemaphoreCreateBinary();

	if (pSemaphore->semphHandle == NULL) {
		return NULL;
	}

	pSemaphore->uPriorityCeiling = uPriorityCeiling;
	pSemaphore->uId = uId;
	pSemaphore->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;

	return pSemaphore;
}

void  Semaphore_vDestroy(Semaphore_t* pSemaphore) {
	free(pSemaphore);
}

uint8_t Semaphore_GetId(Semaphore_t* pSemaphore) {

	if (pSemaphore == NULL) {
		vPrintStringLn("Error in function 'Semaphore_GetId'. NULL Pointer");
	}
	return *(pSemaphore->uId);
}

void PIP_inheritPriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList) {

	WorkerTask_t* pBlockingTask = gll_get(pTaskList, pSemaphore->uAcquiredByTaskNum);

	if (pBlockingTask == pBlockedTask) {
		vPrintStringLn("Error in function 'PIP_inheritPriority'. Same pointer values for pBlockingTask and pBlockedTask");
		return;
	}

	if ( WorkerTask_uGetActivePriority(pBlockingTask) == WorkerTask_uGetActivePriority(pBlockedTask) ) {
		vPrintStringLn("Error in function 'PIP_inheritPriority'. Same uActivePriority values for pBlockingTask and pBlockedTask");
		return;
	}
	// Inherit priority
	WorkerTask_vSetActivePriority( pBlockingTask, WorkerTask_uGetActivePriority(pBlockedTask) );

}

int8_t PIP_SemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskToAquireResource == NULL || pTaskList == NULL) {
		vPrintStringLn("Error in function 'PIP_SemaphoreTake'. NULL Pointer");
	}

	int8_t retVal;

	if (isSemaphoreAcquired(pSemaphore)) {
		// Transmit its active priority to the task that holds the semaphore
		PIP_inheritPriority(pSemaphore, pTaskToAquireResource, pTaskList);
		printf("%s%d%s%c%s\n", "Task ", WorkerTask_vGetTaskNumber(pTaskToAquireResource), " failed to acquire semaphore ", pSemaphore->uId, "and task gets blocked");
		retVal = 1;
	}

	// acquire the semaphore
	if (xSemaphoreTake(pSemaphore->semphHandle, (TickType_t) 0) != pdTRUE) {
		return -1;
	}

	// Since lock is successful, update semaphore's info; semaphore locked by the current task
	pSemaphore->uAcquiredByTaskNum = WorkerTask_vGetTaskNumber(pTaskToAquireResource);
	
	// TODO: change priority of the task for ICPP
	vPrintString("Task "); vPrintInteger(WorkerTask_vGetTaskNumber(pTaskToAquireResource)); 
	vPrintString(" acquired resource "); vPrintString(pSemaphore->uId); vPrintStringLn("");
	//TODO: ICCP 
	// vPrintString("Task <x> acquired resource <y> and changed its priority from <i> to <j>.");
	return 0;
}

int8_t PIP_vSemaphoreGive(Semaphore_t* pSemaphoreHandle, WorkerTask_t* pTaskToReleaseResource, gll_t* pBlockedTaskList) {

	if (pSemaphoreHandle == NULL) {
		vPrintStringLn("Error in function 'PIP_vSemaphoreGive'. NULL Pointer");
	}
	
	pSemaphoreHandle->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;
	// Current task blocked on a semaphore is activated on unlock
	if (xSemaphoreGive(pSemaphoreHandle->semphHandle) != pdTRUE) {
		return -1;
	}

	// unlocks the semaphore set it to the nominal priority or
	WorkerTask_vResetActivePriority(pTaskToReleaseResource);
	/* TODO: Set it to the highest priority of blocked tasks 
	   we need to have a list of blocked tasks, only if the list has 1 element we actually reset to nominal priority
	   else we set tasks priority to 'the highest priority of blocked tasks' */
	
	vPrintString("Resource "); vPrintString(pSemaphoreHandle->uId); vPrintStringLn(" gets released");
	return 0;
}



/* Transmit its active priority to the task that holds the semaphore */
//void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, WorkerTaskList_t* pTaskList) {
void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList) {
	
	for (uint8_t index = 0; index < pTaskList->size; ++index) {

		WorkerTask_t* pTaskThatHoldsSemaphore = gll_get(pTaskList, index);

		if (pSemaphore->uAcquiredByTaskNum == WorkerTask_vGetTaskNumber(pTaskThatHoldsSemaphore)) {

			uint8_t blockedTaskPriority = WorkerTask_uGetActivePriority(pBlockedTask);
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


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

uint8_t Semaphore_vGetId(Semaphore_t* pSemaphoreHandle) {
	return *(pSemaphoreHandle->uId);
}

void PIP_vSemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	if (isSemaphoreAcquired(pSemaphore)) {
		// Transmit its active priority to the task that holds the semaphore
		// Suspend current task
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

void PIP_vSemaphoreGive(Semaphore_t* pSemaphoreHandle) {

	pSemaphoreHandle->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;
}

/* Transmit its active priority to the task that holds the semaphore */
//void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, WorkerTaskList_t* pTaskList) {
void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask) {
	//for (uint8_t index = 0; index < WorkerTaskList_vGetLength(pTaskList); ++index) {

	//	WorkerTask_t* pTaskThatHoldsSemaphore = WorkerTaskList_vGetElement(pTaskList, index);
			/*
		if (pSemaphore->uAcquiredByTaskNum == WorkerTask_vGetTaskNumber(pBlocked) {

		}
		return;*/
	//}

	vPrintStringLn("Error in 'transmitTasksActivePriority' function");
}

bool_t isSemaphoreAcquired(Semaphore_t* pSemaphore) {

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


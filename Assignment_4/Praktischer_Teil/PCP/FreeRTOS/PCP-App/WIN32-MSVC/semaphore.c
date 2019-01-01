#pragma once
#include "semaphore.h"

// Make Semaphore_t private
struct semaphore
{
	xSemaphoreHandle semphHandle;
	uint8_t uPriorityCeiling;
	uint8_t uId;
	uint8_t uAcquiredByTask;

};

void Semaphore_vInit(Semaphore_t* semaphoreHandle, uint8_t priorityCeiling, uint8_t id) {
	semaphoreHandle->uPriorityCeiling = SEMAPHORE_PRIORITY_CEILING_NONE;
	semaphoreHandle->uId = id;
	semaphoreHandle->uAcquiredByTask = SEMAPHORE_AQUIRED_BY_NONE;
}

void PIP_vSemaphoreTake(Semaphore_t* resourceRequestingTask, WorkerTask_t* workerTask) {

	if (isSemaphoreAcquired(resourceRequestingTask)) {
		// Transmit its active priority to the task that holds the semaphore
		// Suspend current task
		return;
	}
	//resourceRequestingTask->uAcquiredByTask = workerTask->uTaskNumber;
	// change priority of the task for ICPP
	// acquire the semaphore
	vPrintString("Task <x> acquired resource <y> and changed its priority from <i> to <j>.");
}

void PIP_vSemaphoreGive(Semaphore_t* semaphoreHandle) {

	semaphoreHandle->uAcquiredByTask = SEMAPHORE_AQUIRED_BY_NONE;
}

bool_t isSemaphoreAcquired(Semaphore_t* resourceRequestingTask) {
	if (resourceRequestingTask->uAcquiredByTask == SEMAPHORE_AQUIRED_BY_NONE) {
		return false;
	}
	return true;
}

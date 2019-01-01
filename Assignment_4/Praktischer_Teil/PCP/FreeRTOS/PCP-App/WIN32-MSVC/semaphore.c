#pragma once
#include "semaphore.h"

// Make Semaphore_t private
struct semaphore
{
	xSemaphoreHandle semphHandle;
	uint8_t priorityCeiling;
	uint8_t id;
	uint8_t acquiredByTask;

};

void Semaphore_vInit(Semaphore_t* semaphoreHandle, uint8_t priorityCeiling, uint8_t id) {
	semaphoreHandle->priorityCeiling = SEMAPHORE_PRIORITY_CEILING_NONE;
	semaphoreHandle->id = id;
	semaphoreHandle->acquiredByTask = SEMAPHORE_AQUIRED_NONE;
}

void PIP_vSemaphoreTake(SemaphoreHandle_t semaphore) {

	if (isSemaphoreHolded(semaphore)) {

		return;
	}

	vPrintString("Task <x> acquired resource <y> and changed its priority from <i> to <j>.");
}

void PIP_vSemaphoreGive(SemaphoreHandle_t semaphore) {

}

bool_t isSemaphoreHolded() {

}
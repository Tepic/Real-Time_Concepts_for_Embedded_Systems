#include "PIP.h"
#include "bool_t.h"

static bool_t isSemaphoreHolded();

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
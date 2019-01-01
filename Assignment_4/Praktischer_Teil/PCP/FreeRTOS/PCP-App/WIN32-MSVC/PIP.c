#include "PIP.h"
#include "bool_t.h"

static bool_t isSemaphoreHolded();

void PIP_vSemaphoreTake(SemaphoreHandle_t semaphore) {
	isSemaphoreHolded();
}

void PIP_vSemaphoreGive(SemaphoreHandle_t semaphore) {

}

bool_t isSemaphoreHolded() {

}
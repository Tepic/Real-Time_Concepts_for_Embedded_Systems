#pragma once
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

/* Standard includes. */
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct semaphore
{
	xSemaphoreHandle semphHandle;
	uint8_t priorityCeiling;
	uint8_t id;

} Semaphore_t;

void Semaphore_vInit(Semaphore_t* pWorkerTask, uint8_t priorityCeiling, uint8_t id);

#endif //end SEMAPHORE_H_
#pragma once
#ifndef ACQUIRED_RESOURCE_H_
#define ACQUIRED_RESOURCE_H_

/* Standard includes. */
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Other includes */
#include "workerTask.h"
#include "semaphore.h"


/* Variable declarations */
typedef struct acquiredResource
{
	uint8_t uTaskPriorityWhenItAcquiredResource;
	uint8_t uSemaphoreId;

}AcquiredResource_t;

/* Function declarations */
AcquiredResource_t* AcquiredResource_Create();
void AcquiredResource_vDestroy(AcquiredResource_t* pAcquiredResource);
void AcquiredResource_vPrint(AcquiredResource_t*);

#endif //end SEMAPHORE_H_
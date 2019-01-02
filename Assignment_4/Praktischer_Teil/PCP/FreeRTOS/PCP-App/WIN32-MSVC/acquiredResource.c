#pragma once
#include "acquiredResource.h"

AcquiredResource_t* AcquiredResource_Create() {

	AcquiredResource_t* pAcquiredResource = (AcquiredResource_t*)malloc(sizeof(AcquiredResource_t));

	if (pAcquiredResource == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'AcquiredResource_Create'. NULL Pointer");
#endif
		return;
	}

	pAcquiredResource->uSemaphoreId = SEMAPHORE_AQUIRED_BY_NONE;
	pAcquiredResource->uTaskPriorityWhenItAcquiredResource = WORKER_TASK_NONE_PRIORITY;

	return pAcquiredResource;
}

void AcquiredResource_vDestroy(AcquiredResource_t* pAcquiredResource) {
	free(pAcquiredResource);
}

void AcquiredResource_vPrint(AcquiredResource_t* pAcquiredResource) {
	if (pAcquiredResource == NULL ) {
#if DEBUG
		vPrintStringLn("Error in function 'AcquiredResource_vPrint'. NULL Pointer");
#endif
		return;
	}

#if DEBUG
	vPrintString("AcquiredResource_t: [uTaskPriorityWhenItAcquiredResource: "); vPrintInteger(pAcquiredResource->uTaskPriorityWhenItAcquiredResource);
	vPrintString(", uSemaphoreId: "); vPrintInteger(pAcquiredResource->uSemaphoreId); vPrintStringLn("]");
#endif
}

#pragma once
#include "semaphore.h"
#include "debug.h"

#define SEMAPHORE_FREE 1
#define SEMAPHORE_LOCKED_BY_OTHER_TASK 2
#define SEMAPHORE_ALREADY_LOCKED_BY_REQUESTING_TASK 3

static int8_t iCheckIfSemaphoreAcquired(WorkerTask_t* pTaskRequestingResource, Semaphore_t* pSemaphore);
static int8_t PIP_iUpdatePriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource);
static void PIP_vInheritPriority(Semaphore_t* pSemaphore, gll_t* pTaskList);

Semaphore_t* Semaphore_Create(uint8_t uPriorityCeiling, const uint8_t uId) {

	Semaphore_t* pSemaphore = (Semaphore_t*)malloc(sizeof(Semaphore_t));

	pSemaphore->semphHandle = xSemaphoreCreateCounting(1,  // uxMaxCount
													   1); // uxInitialCount

	if (pSemaphore->semphHandle == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'Semaphore_Create'. NULL pointer");
#endif
		return NULL;
	}

	pSemaphore->pBlockedTaskList = gll_init();
	if (pSemaphore->pBlockedTaskList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'Semaphore_Create'. NULL pointer");
#endif
		return NULL;
	}

	pSemaphore->uPriorityCeiling = uPriorityCeiling;
	pSemaphore->uId = uId;
	pSemaphore->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;

	return pSemaphore;
}

void Semaphore_vDestroy(Semaphore_t* pSemaphore) {
	
	free(pSemaphore->pBlockedTaskList);
	free(pSemaphore);
}

uint8_t usPrioritySemaphoreWait(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	uint8_t uActivePriorityOld = pTaskToAquireResource->uActivePriority;

	// TODO: Implement ICPP
	PIP_BinarySemaphoreTake(pSemaphore, pTaskToAquireResource, pTaskList);

	uint8_t uActivePriorityNew = pTaskToAquireResource->uActivePriority;

	// TODO: Print Task <x> acquired resource <y> and changed its priority from <i> to <j>.
	vPrintString("Task "); vPrintInteger(pTaskToAquireResource->uTaskNumber);
	vPrintString(" acquired resource ");
	vPrintChar(pSemaphore->uId + 'A' - 1);
	vPrintString(" and changed its priority from "); vPrintInteger(uActivePriorityOld);
	vPrintString(" to "); vPrintInteger(uActivePriorityNew); vPrintStringLn("");

}

uint8_t usPrioritySemaphoreSignal(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	// TODO: Print Task <x> released resource <y> and changed its priority from <i> to <j>.
	uint8_t uActivePriorityOld = pTaskToReleaseResource->uActivePriority;
	// TODO: Implement ICPP
	PIP_vBinarySemaphoreGive(pSemaphore, pTaskToReleaseResource);

	uint8_t uActivePriorityNew = pTaskToReleaseResource->uActivePriority;

	vPrintString("Task "); vPrintInteger(pTaskToReleaseResource->uTaskNumber);
	vPrintString(" released resource "); vPrintChar(pSemaphore->uId - 1 + 'A');
	vPrintString(" and changed its priority from "); vPrintInteger(uActivePriorityOld);
	vPrintString(" to "); vPrintInteger(uActivePriorityNew); vPrintStringLn("");
}

int8_t PIP_BinarySemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskToAquireResource == NULL || pTaskList == NULL) {
		
#if DEBUG
		vPrintStringLn("Error in function 'PIP_BinarySemaphoreTake'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	int8_t retVal = iCheckIfSemaphoreAcquired(pTaskToAquireResource, pSemaphore);

	// Check if the task wanting to request the resource, has already obtained this resource
	// This check needs to be done since we are using binary semaphore
	if (retVal == SEMAPHORE_ALREADY_LOCKED_BY_REQUESTING_TASK) {
		return SEMAPHORE_NOK;
	}

	// When task τi tries to enter a critical section zi,k and resource Rk is already held
	// by a lower - priority task τj, then τi is blocked.τi is said to be blocked by the task
	//	τj that holds the resource. Otherwise, τi enters the critical section zi, k.
	if (retVal == SEMAPHORE_LOCKED_BY_OTHER_TASK){ // Check if some other tasks holds the semaphore

		// Add task τ_i, which is blocked to the list of tasks
		WorkerTask_vListAddTaskDescendingPriorityOrder(pSemaphore->pBlockedTaskList, pTaskToAquireResource);

#if DEBUG
		printf("%s%d%s%c%s%d%s\n", "Task ", pTaskToAquireResource->uTaskNumber, " failed to acquire semaphore ", pSemaphore->uId - 1 + 'A', " and task ", pTaskToAquireResource->uTaskNumber," gets blocked!");
#endif
		// Transmit its active priority to the task that holds the semaphore
		PIP_vInheritPriority(pSemaphore, pTaskList);
		
	}

	// Try to acquire the semaphore
	if ( (xSemaphoreTake(pSemaphore->semphHandle, (TickType_t) 0) != pdTRUE) &&
		  retVal == SEMAPHORE_LOCKED_BY_OTHER_TASK) {

		// The semaphore was not obtained
		vTaskSuspend(pTaskToAquireResource->xHandle);
		return SEMAPHORE_NOK;
	}

	// Since lock request is successful, update semaphore's info; semaphore is now locked by the requesting task
	pSemaphore->uAcquiredByTaskNum = pTaskToAquireResource->uTaskNumber;
	
#if DEBUG
	vPrintString("Task "); vPrintInteger(pTaskToAquireResource->uTaskNumber);
	vPrintString(" acquired resource "); vPrintInteger(pSemaphore->uId); vPrintStringLn("");
#endif

	return SEMAPHORE_OK;
}

/* When τ_j exits a critical section, it unlocks the semaphore, and the highest-priority
    task blocked on that semaphore, if any, is awakened.Moreover, the active priority
	of τ_j is updated as follows : if no other tasks are blocked by τ_j, p_j is set to its
	nominal priority Pj; otherwise it is set to the highest priority of the tasks blocked 
	by τ_j, according to Equation */
int8_t PIP_vBinarySemaphoreGive(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	if (pSemaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_vBinarySemaphoreGive'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	// Current task blocked on a semaphore is activated on unlock
	// Check if the semaphore was obtained
	if (xSemaphoreGive(pSemaphore->semphHandle) != pdTRUE) { // When task τ_j exits a critical section, it unlocks the semaphore
#if DEBUG
		vPrintStringLn("Error in function 'PIP_vBinarySemaphoreGive'. Failed to unlock the semaphore");
#endif
		// The semaphore was not obtained
		return SEMAPHORE_NOK;
	}

	pSemaphore->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;

	// the highest-priority task blocked on that semaphore, if any, is awakened.
	if (pSemaphore->pBlockedTaskList != NULL &&
		pSemaphore->pBlockedTaskList->size > 0) { // Check if there is any task blocked on the semaphore

		WorkerTask_t* pBlockedTaskWithHighestPriority = NULL;

		// Pop up first element from the list, because it's the highest priority task
		// List is sorted with the descending order w.r.t. to its priority
		pBlockedTaskWithHighestPriority = gll_pop(pSemaphore->pBlockedTaskList);

		if (pBlockedTaskWithHighestPriority == NULL) {
			return SEMAPHORE_NOK;
		}

#if DEBUG
		vPrintString("Resource "); vPrintChar(pSemaphore->uId - 1 + 'A');  vPrintStringLn(" gets released");
#endif

		// Avoid 'inversion' of priorities
		// Avoid task with higher priority 'inheriting' priority of the lower priority task
		if (pTaskToReleaseResource->uNominalPriority > pBlockedTaskWithHighestPriority->uNominalPriority) {
			return SEMAPHORE_OK;
		}

#if IS_SCHEDULER_RUNNING
		// The highest-priority task blocked on that semaphore is awakened
		vTaskResume(pBlockedTaskWithHighestPriority->xHandle);
#endif
	}

	return PIP_iUpdatePriority(pSemaphore, pTaskToReleaseResource);
}

int8_t PIP_iUpdatePriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	if (pTaskToReleaseResource == NULL || pSemaphore->pBlockedTaskList == NULL || pSemaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iDecreasePriority'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	// Check if there are other tasks blocked by this task
	if ( pSemaphore->pBlockedTaskList->size == 0 ) {
		// if no other tasks are blocked by the task, task's active priority is set to its nominal priority
		WorkerTask_vResetActivePriorityToNominalPriority(pTaskToReleaseResource);
		return SEMAPHORE_OK;
	}

	// otherwise task's priority is set to the highest priority of the tasks blocked by pTaskToReleaseResource
	WorkerTask_t* pBlockedTaskWithHighestPriority =  gll_first(pSemaphore->pBlockedTaskList);
	if (pBlockedTaskWithHighestPriority == NULL) { 
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iDecreasePriority'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	// Set priority of the tasks the releases the semaphorre to the highest priority of the tasks blocked by it (pTaskToReleaseResource)
	pTaskToReleaseResource->uActivePriority = pBlockedTaskWithHighestPriority->uActivePriority;

	return SEMAPHORE_OK;
}

int8_t iCheckIfSemaphoreAcquired(WorkerTask_t* pTaskRequestingResource, Semaphore_t* pSemaphore) {

	if (pSemaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'isSemaphoreAcquired'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	if (pSemaphore->uAcquiredByTaskNum == pTaskRequestingResource->uTaskNumber) {
#if DEBUG
		vPrintString("Task "); vPrintInteger(pTaskRequestingResource->uTaskNumber); vPrintStringLn(" has already locked by this binary semaphore. Cannot lock it twice!");
#endif
		return SEMAPHORE_ALREADY_LOCKED_BY_REQUESTING_TASK;
	}

	if (pSemaphore->uAcquiredByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		return SEMAPHORE_FREE; 
	}

	return SEMAPHORE_LOCKED_BY_OTHER_TASK;
}


void PIP_vInheritPriority(Semaphore_t* pSemaphore, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_inheritPriority'. NULL pointer");
#endif
		return;
	}

	WorkerTask_t* pBlockingTask = gll_get(pTaskList, pSemaphore->uAcquiredByTaskNum);
	WorkerTask_t* pBlockedTaskWithMaxPriority = gll_get(pSemaphore->pBlockedTaskList, 0);

	// We need to inherit only priority if we are lower priority task
	if (pBlockingTask->uActivePriority < pBlockedTaskWithMaxPriority->uActivePriority) {
		// Inherit priority maximum of all priorities of tasks blocked on the semaphore
		pBlockingTask->uActivePriority = pBlockedTaskWithMaxPriority->uActivePriority;
	}

}

void Semaphore_vPrint(Semaphore_t* semaphore) {
	if (semaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_vPrint'. NULL Ptr");
#endif
		return;
	}

	vPrintString("Semaphore_t: [uId: "); vPrintInteger(semaphore->uId);
	vPrintString(", uPriorityCeiling: "); vPrintInteger(semaphore->uPriorityCeiling);
	vPrintString(", uAcquiredByTaskNum: "); vPrintInteger(semaphore->uAcquiredByTaskNum); vPrintStringLn("]");
}

Semaphore_t* Semaphore_sList_GetSemaphoreById(gll_t* pSemaphoreList, uint8_t uId) {
	if (pSemaphoreList == NULL || uId < 0) {
#if DEBUG
		vPrintStringLn("Error in function 'Semaphore_usList_GetSemaphoreById'. NULL Pointer or wrong uId");
#endif
		return NULL;
	}

	for (uint8_t uIndex = 0; uIndex < pSemaphoreList->size; ++uIndex) {
	
		Semaphore_t* pSemaphore = gll_get(pSemaphoreList, uIndex);
		if (pSemaphore != NULL &&
			pSemaphore->uId == uId) {

			return pSemaphore;
		}
	}

	return NULL;
}

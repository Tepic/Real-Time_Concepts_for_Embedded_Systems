#pragma once
#include "semaphore.h"
#include "debug.h"

#define SEMAPHORE_FREE 1
#define SEMAPHORE_LOCKED_BY_OTHER_TASK 2
#define SEMAPHORE_ALREADY_LOCKED_BY_REQUESTING_TASK 3
#define SEMAPHORE_ALREADY_LOCKED_BY_HIGHER_PRIORITY_TASK 4
#define SEMAPHORE_ALREADY_LOCKED_BY_LOWER_PRIORITY_TASK 5

static void vTransmitActivePriority(WorkerTask_t* pBlockingTask);
static int8_t iCheckIfSemaphoreAcquired(WorkerTask_t* pTaskRequestingResource, Semaphore_t* pSemaphore);
static int8_t PIP_iUpdatePriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource);
static void PIP_vInheritPriority(Semaphore_t* pSemaphore, gll_t* pTaskList);
static int8_t iCheckIfSemaphoreAlreadyHeldByLowPriorityTask(WorkerTask_t* pTaskRequestingResource, Semaphore_t* pSemaphore, gll_t* pTaskList);
static int8_t iGetABlockingTask(WorkerTask_t** pBlockingTask, Semaphore_t* pSemaphore, gll_t* pTaskList);

#define max(x,y) ((x) >= (y)) ? (x) : (y)

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
	pSemaphore->uLockedByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;

	return pSemaphore;
}

void Semaphore_vDestroy(Semaphore_t* pSemaphore) {
	
	free(pSemaphore->pBlockedTaskList);
	free(pSemaphore);
}

int8_t iGetABlockingTask(WorkerTask_t** pBlockingTask, Semaphore_t* pSemaphore, gll_t* pTaskList) {

	if (pSemaphore == NULL  || pTaskList == NULL) {

#if DEBUG
		vPrintStringLn("Error in function 'iGetABlockingTask'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	uint8_t blockingTaskId = pSemaphore->uLockedByTaskNum;

	if (blockingTaskId == SEMAPHORE_AQUIRED_BY_NONE) {
		return SEMAPHORE_FREE;
	}

	if (blockingTaskId - 1 < 0 || blockingTaskId - 1 > pTaskList->size) {
		return SEMAPHORE_NOK;
	}

	*pBlockingTask = gll_get(pTaskList, blockingTaskId - 1);

	return SEMAPHORE_OK;
}

int8_t iCheckIfSemaphoreAlreadyHeldByLowPriorityTask(WorkerTask_t* pTaskRequestingResource, Semaphore_t* pSemaphore, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskRequestingResource == NULL || pTaskList == NULL) {

#if DEBUG
		vPrintStringLn("Error in function 'iCheckIfSemaphoreAlreadyHeldByLowPrioritYTask'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}
	
	WorkerTask_t* pBlockingTask = NULL;
	int8_t retVal = iGetABlockingTask(&pBlockingTask, pSemaphore, pTaskList);
	if ( retVal != SEMAPHORE_OK) {
		return retVal;
	}

	if (pBlockingTask == NULL) {
		return retVal;
	}

	if (pTaskRequestingResource->uActivePriority > pBlockingTask->uActivePriority) {
		return SEMAPHORE_ALREADY_LOCKED_BY_LOWER_PRIORITY_TASK;
	}

	return SEMAPHORE_ALREADY_LOCKED_BY_HIGHER_PRIORITY_TASK;
}

void vTransmitActivePriority(WorkerTask_t* pBlockingTask) {

	if (pBlockingTask == NULL) {

#if DEBUG
		vPrintStringLn("Error in function 'vTransmitActivePriority'. NULL Pointer");
#endif
		return;
	}

	WorkerTask_t* pBlockedTaskWithHighestPriority = gll_first(pBlockingTask->pBlockedTaskList);

	if (pBlockedTaskWithHighestPriority == NULL) {
		return;
	}

	pBlockingTask->uActivePriority = max(pBlockingTask->uNominalPriority, pBlockedTaskWithHighestPriority->uNominalPriority);

}


int8_t PIP_BinarySemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskRequestingResource, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskRequestingResource == NULL || pTaskList == NULL) {
		
#if DEBUG
		vPrintStringLn("Error in function 'PIP_BinarySemaphoreTake'. NULL Pointer");
#endif
		return SEMAPHORE_NOK;
	}

	int8_t retVal = iCheckIfSemaphoreAlreadyHeldByLowPriorityTask(pTaskRequestingResource, pSemaphore, pTaskList);

	if (retVal == SEMAPHORE_ALREADY_LOCKED_BY_LOWER_PRIORITY_TASK) {

		// Task that requests the resource is blocked
		WorkerTask_t* pBlockingTask = NULL;
		int8_t retVal = iGetABlockingTask(&pBlockingTask, pSemaphore, pTaskList);

		if (retVal != SEMAPHORE_OK) {
			return retVal;
		}

		if (pBlockingTask == NULL || pBlockingTask->pBlockedTaskList == NULL) {
			return SEMAPHORE_NOK;
		}

#if DEBUG
		printf("%s%d%s%c%s%d%s\n", "Task ", pTaskRequestingResource->uTaskNumber, " failed to acquire semaphore since its already locked by Lower Priority Task", pSemaphore->uId - 1 + 'A', " and task ", pTaskRequestingResource->uTaskNumber, " gets blocked!");
#endif

		WorkerTask_vListAddTaskDescendingPriorityOrder(pBlockingTask->pBlockedTaskList, pTaskRequestingResource);
		WorkerTask_vListAddTaskDescendingPriorityOrder(pSemaphore->pBlockedTaskList, pTaskRequestingResource);
		vTransmitActivePriority(pBlockingTask);
	}

	// Try to acquire the semaphore
	if (xSemaphoreTake(pSemaphore->semphHandle, (TickType_t)0) != pdTRUE) {

		// The semaphore was not obtained
		vTaskSuspend(pTaskRequestingResource->xHandle);

#if DEBUG
		printf("%s%d%s%c%s%d%s\n", "Task ", pTaskRequestingResource->uTaskNumber, " failed to acquire semaphore ", pSemaphore->uId - 1 + 'A', " and task ", pTaskRequestingResource->uTaskNumber, " gets blocked!");
#endif
		return SEMAPHORE_NOK;
	}

	if (retVal != SEMAPHORE_FREE) {
#if DEBUG
		printf("%s%d%s%c%s%d%s\n", "Task ", pTaskRequestingResource->uTaskNumber, " failed to acquire semaphore ", pSemaphore->uId - 1 + 'A', " and task ", pTaskRequestingResource->uTaskNumber, " gets blocked!");
#endif
		return retVal;
	}

	// Since lock request is successful, update semaphore's info; semaphore is now locked by the requesting task
	pSemaphore->uLockedByTaskNum = pTaskRequestingResource->uTaskNumber;

#if DEBUG
	vPrintString("Task "); vPrintInteger(pTaskRequestingResource->uTaskNumber);
	vPrintString(" acquired resource "); vPrintChar(pSemaphore->uId - 1 + 'A'); vPrintStringLn("");
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

	if (pSemaphore->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		return SEMAPHORE_NOK;
	}

	WorkerTask_t* pHighestPrirorityTaskBlockedBySemaphore = gll_pop(pSemaphore->pBlockedTaskList);

	// the highest - priority task blocked on the unlocked semaphore, if any, is awakened.
	if (pHighestPrirorityTaskBlockedBySemaphore != NULL) {

		// Remove the task we have just unblocked from the list
		gll_pop(pTaskToReleaseResource->pBlockedTaskList);

#if IS_SCHEDULER_RUNNING
		// Blocked task gets unblocked
		vTaskResume(pHighestPrirorityTaskBlockedBySemaphore->xHandle); 
#endif
#if DEBUG
		vPrintString("Task #: "); vPrintInteger(pHighestPrirorityTaskBlockedBySemaphore->uTaskNumber); vPrintStringLn(" gets awaken!");
#endif
	}
	
	// the active priority
	//	of τj is updated as follows : if no other tasks are blocked by τj, pj is set to its
	//	nominal priority Pj; otherwise it is set to the highest priority of the tasks blocked
	//	by τj
	// Update Priority
	if (pTaskToReleaseResource->pBlockedTaskList->size == 0) {
		pTaskToReleaseResource->uActivePriority = pTaskToReleaseResource->uNominalPriority;
	
	}
	else if (pTaskToReleaseResource->pBlockedTaskList->size > 0) {
		
		WorkerTask_t* pHighestPrirorityTaskBlockdByTheTask = gll_first(pTaskToReleaseResource->pBlockedTaskList);

		pTaskToReleaseResource->uActivePriority = max(pTaskToReleaseResource->uNominalPriority, pHighestPrirorityTaskBlockdByTheTask->uNominalPriority);
	}

	pSemaphore->uLockedByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;
#if DEBUG
	vPrintString("Resource "); vPrintChar(pSemaphore->uId - 1 + 'A');  vPrintStringLn(" gets released");
#endif

	return SEMAPHORE_OK;
//PIP_iUpdatePriority(pSemaphore, pTaskToReleaseResource);
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

	if (pSemaphore->uLockedByTaskNum == pTaskRequestingResource->uTaskNumber) {
#if DEBUG
		vPrintString("Task "); vPrintInteger(pTaskRequestingResource->uTaskNumber); vPrintStringLn(" has already locked by this binary semaphore. Cannot lock it twice!");
#endif
		return SEMAPHORE_ALREADY_LOCKED_BY_REQUESTING_TASK;
	}

	if (pSemaphore->uLockedByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
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

	WorkerTask_t* pBlockingTask = gll_get(pTaskList, pSemaphore->uLockedByTaskNum);
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
	vPrintString(", uLockedByTaskNum: "); vPrintInteger(semaphore->uLockedByTaskNum); vPrintStringLn("]");
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

int8_t usPrioritySemaphoreWait(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	int8_t retVal = -1;
	uint8_t uActivePriorityOld = pTaskToAquireResource->uActivePriority;

	// TODO: Implement ICPP
	retVal = PIP_BinarySemaphoreTake(pSemaphore, pTaskToAquireResource, pTaskList);

	uint8_t uActivePriorityNew = pTaskToAquireResource->uActivePriority;

	// TODO: Print Task <x> acquired resource <y> and changed its priority from <i> to <j>.
	vPrintString("Task "); vPrintInteger(pTaskToAquireResource->uTaskNumber);
	vPrintString(" acquired resource ");
	vPrintChar(pSemaphore->uId + 'A' - 1);
	vPrintString(" and changed its priority from "); vPrintInteger(uActivePriorityOld);
	vPrintString(" to "); vPrintInteger(uActivePriorityNew); vPrintStringLn("");

	return retVal;

}

int8_t usPrioritySemaphoreSignal(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	int8_t retVal = -1;
	// TODO: Print Task <x> released resource <y> and changed its priority from <i> to <j>.
	uint8_t uActivePriorityOld = pTaskToReleaseResource->uActivePriority;
	// TODO: Implement ICPP
	retVal = PIP_vBinarySemaphoreGive(pSemaphore, pTaskToReleaseResource);

	uint8_t uActivePriorityNew = pTaskToReleaseResource->uActivePriority;

	vPrintString("Task "); vPrintInteger(pTaskToReleaseResource->uTaskNumber);
	vPrintString(" released resource "); vPrintChar(pSemaphore->uId - 1 + 'A');
	vPrintString(" and changed its priority from "); vPrintInteger(uActivePriorityOld);
	vPrintString(" to "); vPrintInteger(uActivePriorityNew); vPrintStringLn("");

	return retVal;
}
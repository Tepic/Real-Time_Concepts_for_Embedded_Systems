#pragma once
#include "semaphore.h"
#include "acquiredResource.h"
#include "debug.h"

static int8_t Semaphore_iGetIndex(WorkerTask_t* pTaskThatAquiredSemaphore, Semaphore_t* pSemaphoreInUse);
static bool_t isSemaphoreAcquired(Semaphore_t*);
//static void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList);
static int8_t PIP_iUpdatePriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource);

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

void PIP_inheritPriority(Semaphore_t* pSemaphore, gll_t* pTaskList) {

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

int8_t PIP_BinarySemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList) {

	if (pSemaphore == NULL || pTaskToAquireResource == NULL || pTaskList == NULL) {
		
#if DEBUG
		vPrintStringLn("Error in function 'PIP_BinarySemaphoreTake'. NULL Pointer");
#endif
		return -1;
	}

	// Check if the task wanting to request the resource, has already obtained this resource
	// This check needs to be done since we are using binary semaphore
	// TODO: Maybe remove this check code and do if (xSemaphoreTake(pSemaphore->semphHandle, (TickType_t) 0) != pdTRUE) instead?
	if (pSemaphore->uAcquiredByTaskNum == pTaskToAquireResource->uTaskNumber) {
#if DEBUG
		vPrintString("Task "); vPrintInteger(pTaskToAquireResource->uTaskNumber); vPrintString(" has already locked by this binary semaphore. Cannot lock it twice!");
#endif
		return -1;
	}

	int8_t retVal = 0;

	// When task τi tries to enter a critical section zi,k and resource Rk is already held
	// by a lower - priority task τj, then τi is blocked.τi is said to be blocked by the task
	//	τj that holds the resource. Otherwise, τi enters the critical section zi, k.
	if (isSemaphoreAcquired(pSemaphore)) {
#if DEBUG
		//WorkerTask_vListPrintPriority(pSemaphore->pBlockedTaskList);
#endif

		// TODO: Sort list by ascending WorkerTask_t::uActivePriority
		// Insert task to appropriate index inside the list of blocked tasks
		// The blocked tasks ask are sorted in descending order w.r.t. priorities
		// E.g. the blocked task with lowest priority should be at the end of the list
		// Add task τi, which is blocked. τi is said to be blocked by the task τj that holds the resource.
		WorkerTask_vListAddTaskDescendingPriorityOrder(pSemaphore->pBlockedTaskList, pTaskToAquireResource);

#if DEBUG
		printf("%s%d%s%d%s%d%s\n", "Task ", pTaskToAquireResource->uTaskNumber, " failed to acquire semaphore ", pSemaphore->uId, " and task ", pTaskToAquireResource->uTaskNumber," gets blocked!");
#endif
		// Transmit its active priority to the task that holds the semaphore
		PIP_inheritPriority(pSemaphore, pTaskList);
		
		retVal = 1;
#if DEBUG
		//return 2;
#endif
	}

	// Try to acquire the semaphore
	if (xSemaphoreTake(pSemaphore->semphHandle, (TickType_t) 0) != pdTRUE) {

		// The semaphore was not obtained
		vTaskSuspend(pTaskToAquireResource->xHandle);
		return 1;
#if !DEBUG
		return 1;
#endif
	}

	// Since lock is successful, update semaphore's info; semaphore locked by the current task
	pSemaphore->uAcquiredByTaskNum = pTaskToAquireResource->uTaskNumber;
	
	// Store current active priority of the task that locked the semaphore,
	// This information has to be stored, because this task can inherit priority from higher priority tasks
	// We use AcquiredResource_t once we unlock the semaphore to return tasks' priority to the one when it acquired the resource (semaphore)
	//retVal = PIP_iSetPriority(pSemaphore, pTaskToAquireResource);

	// TODO: change priority of the task for ICPP
#if DEBUG
	vPrintString("Task "); vPrintInteger(pTaskToAquireResource->uTaskNumber);
	vPrintString(" acquired resource "); vPrintInteger(pSemaphore->uId); vPrintStringLn("");
#endif
	//TODO: ICCP 
	// vPrintString("Task <x> acquired resource <y> and changed its priority from <i> to <j>.");
	//return retVal;
	return 0;
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
		return -1;
	}

	// Current task blocked on a semaphore is activated on unlock
	// Check if the semaphore was obtained
	if (xSemaphoreGive(pSemaphore->semphHandle) != pdTRUE) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_vBinarySemaphoreGive'. NULL Pointer");
#endif
		// The semaphore was not obtained
		return -1;
	}

	/* When task τj exits a critical section, it unlocks the semaphore, and the highest-priority
	task blocked on that semaphore, if any, is awakened. */
	// Check if there is any task blocked on the semaphore
	if (pSemaphore->pBlockedTaskList != NULL &&
		pSemaphore->pBlockedTaskList->size > 0) {

		WorkerTask_t* pBlockedTaskWithHighestPriority = NULL;
		// Pop up first element from the list, because it's the highest priority task
		// List is sorted with the descending order w.r.t. to its priority
		pBlockedTaskWithHighestPriority = gll_pop(pSemaphore->pBlockedTaskList);

		if (pBlockedTaskWithHighestPriority == NULL) {
			return -1;
		}

		// The highest-priority task blocked on that semaphore is awakened
#if IS_SCHEDULER_RUNNING
		vTaskResume(pBlockedTaskWithHighestPriority->xHandle);
#endif
	}

#if DEBUG
	vPrintString("Resource "); vPrintInteger(pSemaphore->uId); vPrintStringLn(" gets released");
#endif

	pSemaphore->uAcquiredByTaskNum = SEMAPHORE_AQUIRED_BY_NONE;

	return PIP_iUpdatePriority(pSemaphore, pTaskToReleaseResource);
}


int8_t PIP_iUpdatePriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	if (pTaskToReleaseResource== NULL || pSemaphore->pBlockedTaskList == NULL || pSemaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iDecreasePriority'. NULL Pointer");
#endif
		return -1;
	}

	// Check if there are other tasks blocked by this task
	if ( pSemaphore->pBlockedTaskList->size == 0 ) {
		// if no other tasks are blocked by the task, task's active priority is set to its nominal priority
		WorkerTask_vResetActivePriorityToNominalPriority(pTaskToReleaseResource);
	}

	// otherwise task's priority is set to the highest priority of the tasks blocked by pTaskToReleaseResource
	WorkerTask_t* pBlockedTaskWithHighestPriority =  gll_first(pSemaphore->pBlockedTaskList);
	if (pBlockedTaskWithHighestPriority == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iDecreasePriority'. NULL Pointer");
#endif
	}

	// Set priority of the tasks the releases the semaphorre to the highest priority of the tasks blocked by it (pTaskToReleaseResource)
	pTaskToReleaseResource->uActivePriority = pBlockedTaskWithHighestPriority->uActivePriority;

	return 0;
}

/*
int8_t PIP_iResetPriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToReleaseResource) {

	// if no other tasks are blocked by task τ_j, its active priority p_j is set
	// to its nominal priority Pj
	if (pTaskToReleaseResource->pAcquiredResourceList != NULL &&
		pTaskToReleaseResource->pAcquiredResourceList->size == 0) {

		WorkerTask_vResetActivePriorityToNominalPriority(pTaskToReleaseResource);
	}

	// if there are tasks that are blocked by task τ_j, active priority
	//   of task τ_j is set to the highest priority of the tasks block by τ_j 
	int8_t iResourceIndex = Semaphore_iGetIndex(pTaskToReleaseResource, pSemaphore);
	if (iResourceIndex < 0) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iResetPriority'. NULL Semaphore_iGetIndex returned non-positive value");
#endif
		return -1;
	}

	// The task that releases the semaphore can inherit priority from higher priority tasks
	// We use AcquiredResource_t to return tasks' priority to the one when it acquired the resource (semaphore)
	AcquiredResource_t* pAcquiredResource = gll_get(pTaskToReleaseResource->pAcquiredResourceList, iResourceIndex);
	if (pAcquiredResource == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iResetPriority'. NULL Semaphore_iGetIndex returned non-positive value");
#endif
		return -1;
	}
	pTaskToReleaseResource->uActivePriority = pAcquiredResource->uTaskPriorityWhenItAcquiredResource;
	pAcquiredResource->uTaskPriorityWhenItAcquiredResource = WORKER_TASK_NONE_PRIORITY;

	return 0;
}


int8_t PIP_iSetPriority(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource) {

	int8_t iResourceIndex = Semaphore_iGetIndex(pTaskToAquireResource, pSemaphore);
	if (iResourceIndex < 0) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iSetPriority'. NULL Semaphore_iGetIndex returned non-positive value");
#endif
		return -1;
	}

	// Store current active priority of the task that locked the semaphore,
	// This information has to be stored, because this task can inherit priority from higher priority tasks
	// We use AcquiredResource_t once we unlock the semaphore to return tasks' priority to the one when it acquired the resource (semaphore)
	AcquiredResource_t* pAcquiredResource = gll_get(pTaskToAquireResource->pAcquiredResourceList, iResourceIndex);
	if (pAcquiredResource == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'PIP_iSetPriority'. NULL Semaphore_iGetIndex returned non-positive value");
#endif
		return -1;
	}
	
	pAcquiredResource->uTaskPriorityWhenItAcquiredResource = pTaskToAquireResource->uActivePriority;

	return 0;
}
*/

/* Transmit its active priority to the task that holds the semaphore */
//void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, WorkerTaskList_t* pTaskList) {
/*
void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList) {
	
	for (uint8_t index = 0; index < pTaskList->size; ++index) {

		WorkerTask_t* pTaskThatHoldsSemaphore = gll_get(pTaskList, index);

		if (pTaskThatHoldsSemaphore != NULL &&
			pSemaphore->uAcquiredByTaskNum == pTaskThatHoldsSemaphore->uTaskNumber) {

			uint8_t blockedTaskPriority = pBlockedTask->uActivePriority;
			pTaskThatHoldsSemaphore->uActivePriority = blockedTaskPriority;

#if DEBUG
			vPrintString("Task "); vPrintInteger(pBlockedTask->uTaskNumber);  
			vPrintString(" transmit its active priority "); vPrintInteger(blockedTaskPriority);   
			vPrintString("to the task "); vPrintInteger(pTaskThatHoldsSemaphore->uTaskNumber);
			vPrintString(" that holds the semaphore "); vPrintInteger(pSemaphore->uId);
#endif
			return;
		}
	}

#if DEBUG
	vPrintStringLn("Error in 'transmitTasksActivePriority' function");
#endif
}
*/

bool_t isSemaphoreAcquired(Semaphore_t* pSemaphore) {

	if (pSemaphore == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'isSemaphoreAcquired'. NULL Pointer");
#endif
		return false;
	}

	if (pSemaphore->uAcquiredByTaskNum == SEMAPHORE_AQUIRED_BY_NONE) {
		return false;
	}
	return true;
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

static int8_t Semaphore_iGetIndex(WorkerTask_t* pTaskThatAquiredSemaphore, Semaphore_t* pSemaphoreInUse) {
	if (pTaskThatAquiredSemaphore == NULL || pSemaphoreInUse == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_iGetIndex'. NULL Pointer");
#endif
		return -1;
	}

	if (pTaskThatAquiredSemaphore->pAcquiredResourceList->size == 0) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_iGetIndex'. pTaskThatAquiredSemaphore->pAcquiredResourceList->size is 0");
#endif
		return 0;
	}

	for (uint8_t index = 0; index < pTaskThatAquiredSemaphore->pAcquiredResourceList->size; ++index) {
		AcquiredResource_t* pAcquiredResource = gll_get(pTaskThatAquiredSemaphore->pAcquiredResourceList, index);

		if (pAcquiredResource != NULL && 
			pAcquiredResource->uSemaphoreId == pSemaphoreInUse->uId) {
			return index;
		}
	}

	return -1;
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
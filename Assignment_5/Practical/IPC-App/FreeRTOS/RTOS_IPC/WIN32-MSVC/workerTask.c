#include "workerTask.h"
#include "print.h"
#include "config.h"

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler,
	uint8_t uTaskNumber,
	uint8_t nominalPriority,
	uint8_t uStartValue,
	uint8_t uEndValue,
	uint16_t uPeriod,
	gll_t* pUsedSemaphoreList) {

	if (pUsedSemaphoreList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_Create'. NULL Pointer");
#endif
		return NULL;
	}
	
	WorkerTask_t* pWorkerTask = (WorkerTask_t*)malloc(sizeof(WorkerTask_t));

	pWorkerTask->pBlockedTaskList = gll_init();
	pWorkerTask->pUsedSemaphoreList = pUsedSemaphoreList;

	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;
	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uPeriod = uPeriod;
	pWorkerTask->uStartValue = uStartValue;
	pWorkerTask->uCurrentValue = uStartValue;
	pWorkerTask->uEndValue = uEndValue;
	pWorkerTask->uReleaseTime = 0;

	xTaskCreate(taskHandler, "Task 0", 1000, pWorkerTask, pWorkerTask->uNominalPriority, &pWorkerTask->xHandle);

	return pWorkerTask;
}

void  WorkerTask_vDestroy(WorkerTask_t* pWorkerTask) {

	free(pWorkerTask->pBlockedTaskList);
	free(pWorkerTask);
}

// Insert task to appropriate index inside the list of blocked tasks
// The blocked tasks ask are sorted in descending order w.r.t. priorities
// E.g. the blocked task with lowest priority should be at the end of the list
// Same task priorities are added in FIFO order
void WorkerTask_vListAddTaskDescendingPriorityOrder(gll_t* pTaskList, WorkerTask_t* pTaskToInsertIntoTheList) {

	if (pTaskList == NULL || pTaskToInsertIntoTheList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_vListAddTaskDescendingPriorityOrder'. NULL Pointer");
#endif
		return;
	}

	if (pTaskList->size == 0) {
		gll_push(pTaskList, pTaskToInsertIntoTheList);
		return;
	}

	uint8_t uPriority = pTaskToInsertIntoTheList->uActivePriority;
	for (uint8_t index = 0; index < pTaskList->size; ++index) {

		WorkerTask_t* pTask = (WorkerTask_t*) gll_get(pTaskList, index);
		uint8_t uListPriority = pTask->uActivePriority;

		if (uPriority >= uListPriority) {

			// For same priorities add in FIFO order
			uint8_t uInnerIndex = index;
			for (; uInnerIndex < pTaskList->size; ++uInnerIndex) {

				pTask = (WorkerTask_t*)gll_get(pTaskList, uInnerIndex);
				uListPriority = pTask->uActivePriority;

				if (uPriority > uListPriority) {
					// Add to the last element with the same priority
					gll_add(pTaskList, pTaskToInsertIntoTheList, uInnerIndex);
					return;
				}	
			}

			gll_add(pTaskList, pTaskToInsertIntoTheList, uInnerIndex);
			return;
		}
	}

	gll_pushBack(pTaskList, pTaskToInsertIntoTheList);
}

/* Set priority of the task to the nominal value */
void WorkerTask_vResetActivePriorityToNominalPriority(WorkerTask_t* pWorkerTask) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vResetActivePriority'. NULL Pointer");
	}
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;
}

void WorkerTask_vPrint(WorkerTask_t* task) {
	if (task == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_vPrint'. NULL Pointer");
#endif
		return;
	}

	vPrintString("WorkerTask_t: [uTaskNumber: "); vPrintInteger(task->uTaskNumber);
	vPrintString(", uNominalPriority: "); vPrintInteger(task->uNominalPriority);
	vPrintString(", uActivePriority: "); vPrintInteger(task->uActivePriority);
	vPrintString(", uReleaseTime: "); vPrintInteger(task->uReleaseTime);
	vPrintString(", uPeriod: "); vPrintInteger(task->uPeriod); vPrintStringLn("]");
}

void WorkerTask_vListPrintPriority(gll_t* pTaskList) {
	if (pTaskList == NULL || pTaskList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_vListPrintPriority'. NULL Pointer");
#endif
		return;
	}

	if (pTaskList->size == 0) {
#if DEBUG
		vPrintStringLn("Function 'WorkerTask_vListPrintPriority' empty list");
#endif
		return;
	}

#if DEBUG
	vPrintStringLn("");
#endif
	WorkerTask_t* pBlockedTask; 
	for (uint8_t index = 0; index < pTaskList->size; ++index) {
		pBlockedTask = gll_get(pTaskList, index);
#if DEBUG
		WorkerTask_vPrint(pBlockedTask);
#endif
	}
#if DEBUG
	vPrintStringLn("");
#endif
}

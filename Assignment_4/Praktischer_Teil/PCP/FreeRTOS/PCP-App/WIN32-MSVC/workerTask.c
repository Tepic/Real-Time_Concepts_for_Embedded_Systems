#include "workerTask.h"
#include "print.h"
#include "debug.h"

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler,
	uint8_t uTaskNumber,
	uint8_t nominalPriority,
	uint8_t uReleaseTime,
	uint8_t uPeriod,
	gll_t* pUsedSemaphoreList) {

	if (pUsedSemaphoreList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_Create'. NULL Pointer");
#endif
		return NULL;
	}
	
	WorkerTask_t* pWorkerTask = (WorkerTask_t*)malloc(sizeof(WorkerTask_t));

	pWorkerTask->pUsedSemaphoreList = pUsedSemaphoreList;
	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;
	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uPriorityWhenItAcquiredResource = WORKER_TASK_NONE_PRIORITY;
	pWorkerTask->uPeriod = uPeriod;
	pWorkerTask->uReleaseTime = uReleaseTime;

	xTaskCreate(taskHandler, "Task 0", 1000, pWorkerTask, pWorkerTask->uNominalPriority, &pWorkerTask->xHandle);

	return pWorkerTask;
}

void  WorkerTask_vDestroy(WorkerTask_t* pWorkerTask) {

	free(pWorkerTask);
}

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

	for (uint8_t index = 0; index < pTaskList->size; ++index) {

		WorkerTask_t* pListTask = (WorkerTask_t*) gll_get(pTaskList, index);
		uint8_t uListData = pListTask->uActivePriority;
		uint8_t uData = pTaskToInsertIntoTheList->uActivePriority;

		if (uData >= uListData) {
			gll_add(pTaskList, pTaskToInsertIntoTheList, index);
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
	vPrintString(", uPriorityWhenItAcquiredResource: "); vPrintInteger(task->uPriorityWhenItAcquiredResource);
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

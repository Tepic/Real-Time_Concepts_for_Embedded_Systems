#include "controllerTask.h"
#include "print.h"
#include "config.h"

ControllerTask_t* ControllerTask_Create(TaskFunction_t taskHandler,
										uint8_t uTaskNumber,
										uint8_t nominalPriority,
										uint16_t uPeriod,
										gll_t* pUsedSemaphoreList,
										gll_t* pUsedQueueList)
{

	if (pUsedSemaphoreList == NULL) {
#if DEBUG
		vPrintStringLn("Error in function 'WorkerTask_Create'. NULL Pointer");
#endif
		return NULL;
	}

	ControllerTask_t* pControllerTask = (ControllerTask_t*)malloc(sizeof(ControllerTask_t));

	pControllerTask->pBlockedTaskList = gll_init();
	pControllerTask->pUsedSemaphoreList = pUsedSemaphoreList;
	pControllerTask->pUsedQueueList = pUsedQueueList;

	pControllerTask->uNominalPriority = nominalPriority;
	pControllerTask->uActivePriority = pControllerTask->uNominalPriority;
	pControllerTask->uTaskNumber = uTaskNumber;
	pControllerTask->uPeriod = uPeriod;
	pControllerTask->uReleaseTime = 0;

	xTaskCreate(taskHandler, "Task 0", 1000, pControllerTask, pControllerTask->uNominalPriority, &pControllerTask->xHandle);

	return pControllerTask;
}

void  ControllerrTask_vDestroy(ControllerTask_t* pControllerTask) {

	free(pControllerTask);
}

// Insert task to appropriate index inside the list of blocked tasks
// The blocked tasks ask are sorted in descending order w.r.t. priorities
// E.g. the blocked task with lowest priority should be at the end of the list
// Same task priorities are added in FIFO order
void ControllerTask_vListAddTaskDescendingPriorityOrder(gll_t* pTaskList, ControllerTask_t* pTaskToInsertIntoTheList) {

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

		ControllerTask_t* pTask = (ControllerTask_t*)gll_get(pTaskList, index);
		uint8_t uListPriority = pTask->uActivePriority;

		if (uPriority >= uListPriority) {

			// For same priorities add in FIFO order
			uint8_t uInnerIndex = index;
			for (; uInnerIndex < pTaskList->size; ++uInnerIndex) {

				pTask = (ControllerTask_t*)gll_get(pTaskList, uInnerIndex);
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
void ControllerTask_vResetActivePriorityToNominalPriority(ControllerTask_t* pControllerTask) {
	if (pControllerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vResetActivePriority'. NULL Pointer");
	}
	pControllerTask->uActivePriority = pControllerTask->uNominalPriority;
}

void ControllerTask_vPrint(ControllerTask_t* task) {
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

void ControllerTask_vListPrintPriority(gll_t* pTaskList) {
	if (pTaskList == NULL || pTaskList == NULL)
	{
		return;
	}

	if (pTaskList->size == 0)
	{
		return;
	}
	ControllerTask_t* pBlockedTask;
	for (uint8_t index = 0; index < pTaskList->size; ++index) {
		pBlockedTask = gll_get(pTaskList, index);
	}
}

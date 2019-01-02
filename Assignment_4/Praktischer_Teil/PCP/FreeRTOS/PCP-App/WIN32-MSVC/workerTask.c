#include "workerTask.h"
#include "print.h"

/* Make workerTask fields private this way */
struct workerTask 
{
	TaskHandle_t xHandle;
	uint8_t uTaskNumber;
	uint8_t uNominalPriority;
	uint8_t uActivePriority;
	uint32_t uExecutionTime;

};

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime) {

	WorkerTask_t* pWorkerTask = (WorkerTask_t*)malloc(sizeof(WorkerTask_t));

	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;

	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;

	xTaskCreate(taskHandler, "Task 0", 1000, pWorkerTask, pWorkerTask->uNominalPriority, pWorkerTask->xHandle);
	
	return pWorkerTask;
}

void  WorkerTask_vDestroy(WorkerTask_t* pWorkerTask) {
	free(pWorkerTask);
}

uint8_t WorkerTask_vGetTaskNumber(WorkerTask_t* pWorkerTask) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vGetTaskNumber'. NULL Pointer");
	}
	return pWorkerTask->uTaskNumber;
}

TaskHandle_t WorkerTask_vGetHandle(WorkerTask_t* pWorkerTask) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'Semaphore_vGetHandle'. NULL Pointer");
	}
	return pWorkerTask->xHandle;
}

void WorkerTask_vSetActivePriority(WorkerTask_t* pWorkerTask, uint8_t uActivePriority) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vSetActivePriority'. NULL Pointer");
	}
	pWorkerTask->uActivePriority = uActivePriority;
}

uint8_t WorkerTask_vGetActivePriority(WorkerTask_t* pWorkerTask) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vGetActivePriority'. NULL Pointer");
	}
	return pWorkerTask->uActivePriority;
}

void WorkerTask_vResetActivePriority(WorkerTask_t* pWorkerTask) {
	if (pWorkerTask == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vResetActivePriority'. NULL Pointer");
	}
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;
}

uint8_t WorkerTask_vSizeOf() {
	return sizeof(WorkerTask_t);
}

void WorkerTask_vPrint(WorkerTask_t* task) {
	if (task == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vPrint'. NULL Pointer");
		return;
	}

	vPrintString("WorkerTask_t: [uTaskNumber: "); vPrintInteger(task->uTaskNumber);
	vPrintString(", uNominalPriority: "); vPrintInteger(task->uNominalPriority);
	vPrintString(", uActivePriority: "); vPrintInteger(task->uActivePriority);
	vPrintString(", uExecutionTime: "); vPrintInteger(task->uExecutionTime); vPrintStringLn("]");
}
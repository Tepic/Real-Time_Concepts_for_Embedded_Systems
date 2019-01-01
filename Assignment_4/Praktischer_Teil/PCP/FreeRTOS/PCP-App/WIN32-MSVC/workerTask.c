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

WorkerTask_t* WorkerTask_vCreate(uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime) {

	WorkerTask_t* pWorkerTask = (WorkerTask_t*)malloc(sizeof(WorkerTask_t));

	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;

	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;
	
	return pWorkerTask;
}

void  WorkerTask_vDelete(WorkerTask_t* pWorkerTask) {
	free(pWorkerTask);
}

uint8_t WorkerTask_vGetTaskNumber(WorkerTask_t* pWorkerTask) {
	return pWorkerTask->uTaskNumber;
}

void WorkerTask_vSetActivePriority(WorkerTask_t* pWorkerTask, uint8_t uActivePriority) {
	pWorkerTask->uActivePriority = uActivePriority;
}

void WorkerTask_vResetActivePriority(WorkerTask_t* pWorkerTask) {
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;
}

/*
WorkerTask_t* WorkerTask_vArray(WorkerTask_t* pWorkerTasks, uint8_t uIndex) {
	if (pWorkerTasks == NULL || uIndex < 0) {
		vPrintStringLn("Error in WorkerTask_vArray");
		return;
	}

	//return pWorkerTasks[uIndex];
}*/

uint8_t WorkerTask_vSizeOf() {
	return sizeof(WorkerTask_t);
}

void WorkerTask_vPrint(WorkerTask_t* task) {
	if (task == NULL) {
		vPrintStringLn("Error in function 'WorkerTask_vPrint'. NULL Ptr");
		return;
	}

	vPrintString("WorkerTask_t: [uTaskNumber: "); vPrintInteger(task->uTaskNumber);
	vPrintString(", uNominalPriority: "); vPrintInteger(task->uNominalPriority);
	vPrintString(", uActivePriority: "); vPrintInteger(task->uActivePriority);
	vPrintString(", uExecutionTime: "); vPrintInteger(task->uExecutionTime); vPrintStringLn("]");
}
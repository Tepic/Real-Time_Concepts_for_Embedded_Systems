#include "workerTask.h"

/* Make workerTask fields private this way */
struct workerTask 
{
	TaskHandle_t xHandle;
	uint32_t uExecutionTime;
	uint8_t uTaskNumber;
	uint8_t uNominalPriority;
	uint8_t uActivePriority;

};

void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime) {
	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;

	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;
	
}

uint8_t WorkerTask_vGetTaskNumber(WorkerTask_t* pWorkerTask) {
	return pWorkerTask->uTaskNumber;
}
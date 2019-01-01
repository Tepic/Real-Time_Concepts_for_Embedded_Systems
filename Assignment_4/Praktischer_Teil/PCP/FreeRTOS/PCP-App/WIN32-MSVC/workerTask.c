#include "workerTask.h"

typedef struct workerTask
{
	TaskHandle_t xHandle;
	uint32_t uExecutionTime;
	uint8_t uTaskNumber;
	uint8_t uNominalPriority;
	uint8_t uActivePriority;

} WorkerTask_t;

void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime) {
	pWorkerTask->uNominalPriority = nominalPriority;
	pWorkerTask->uActivePriority = pWorkerTask->uNominalPriority;

	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;
	
}
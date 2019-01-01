#include "workerTask.h"

void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t id, uint8_t uTaskNumber, bool_t isMissbehaved, uint32_t uExecutionTime) {
	pWorkerTask->isMissbehaved = isMissbehaved;
	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uExecutionTime = uExecutionTime;
}
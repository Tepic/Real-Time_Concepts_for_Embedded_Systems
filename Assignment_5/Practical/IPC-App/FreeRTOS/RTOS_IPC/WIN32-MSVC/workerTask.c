#include "workerTask.h"
#include "print.h"

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler,
	uint8_t uTaskNumber,
	uint8_t uPriorityLevel,
	uint16_t uStartValue,
	uint16_t uEndValue,
	uint16_t uPeriod,
	gll_t* pUsedQueueList)
	//QueueHandle_t* pQueueHandle)
{	
	WorkerTask_t* pWorkerTask = (WorkerTask_t*)malloc(sizeof(WorkerTask_t));
	
	pWorkerTask->uPriorityLevel = uPriorityLevel;
	pWorkerTask->uTaskNumber = uTaskNumber;
	pWorkerTask->uPeriod = uPeriod;
	pWorkerTask->uStartValue = uStartValue;
	pWorkerTask->uCurrentValue = uStartValue;
	pWorkerTask->uEndValue = uEndValue;
	pWorkerTask->uReleaseTime = 0;
	pWorkerTask->pUsedQueueList = pUsedQueueList;

	xTaskCreate(taskHandler, "Task 0", 1000, pWorkerTask, pWorkerTask->uPriorityLevel, &pWorkerTask->xHandle);

	return pWorkerTask;
}
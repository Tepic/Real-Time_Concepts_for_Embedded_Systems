#include "controllerTask.h"
#include "print.h"

ControllerTask_t* ControllerTask_Create(TaskFunction_t taskHandler,
	uint8_t uTaskNumber,
	uint8_t uPriorityLevel,
	uint16_t uControllerID,
	uint16_t uPeriod,
	gll_t* pUsedQueueList)
{	
	ControllerTask_t* pControllerTask = (ControllerTask_t*)malloc(sizeof(ControllerTask_t));
	
	pControllerTask->uTaskNumber = uTaskNumber;
	pControllerTask->uPriorityLevel = uPriorityLevel;
	pControllerTask->uControllerID = uControllerID;
	pControllerTask->uPeriod = uPeriod;
	pControllerTask->uReleaseTime = 0;
	pControllerTask->pUsedQueueList = pUsedQueueList;

	xTaskCreate(taskHandler, "Task 0", 1000, pControllerTask, pControllerTask->uPriorityLevel, &pControllerTask->xHandle);

	return pControllerTask;
}
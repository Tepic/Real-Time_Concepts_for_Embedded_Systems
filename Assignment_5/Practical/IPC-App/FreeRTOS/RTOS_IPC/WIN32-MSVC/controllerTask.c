#include "controllerTask.h"
#include "print.h"
#include "config.h"

ControllerTask_t* ControllerTask_Create(TaskFunction_t taskHandler,
	uint8_t uTaskNumber,
	uint8_t uPriorityLevel,
	uint16_t uID,
	uint16_t uPeriod,
	gll_t* pUsedQueueList)
	//QueueHandle_t* pQueueHandle)
{
	ControllerTask_t* pControllerTask = (ControllerTask_t*)malloc(sizeof(ControllerTask_t));
	
	pControllerTask->uPriorityLevel = uPriorityLevel;
	pControllerTask->uTaskNumber = uTaskNumber;
	pControllerTask->uPeriod = uPeriod;
	pControllerTask->uID = uID;
	pControllerTask->uReleaseTime = 0;
	pControllerTask->pUsedQueueList = pUsedQueueList;

	xTaskCreate(taskHandler, "Task 0", 1000, pControllerTask, pControllerTask->uPriorityLevel, &pControllerTask->xHandle);

	return pControllerTask;
}
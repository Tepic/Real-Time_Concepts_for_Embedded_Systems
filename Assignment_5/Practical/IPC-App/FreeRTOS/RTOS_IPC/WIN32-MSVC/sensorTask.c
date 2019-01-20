#include "sensorTask.h"
#include "print.h"

SensorTask_t* SensorTask_Create(TaskFunction_t taskHandler,
								uint8_t uTaskNumber,
								uint8_t uPriorityLevel,
								uint16_t uStartValue,
								uint16_t uEndValue,
								uint16_t uPeriod,
								QueueHandle_t* pUsedQueue)
{	
	SensorTask_t* pSensorTask = (SensorTask_t*)malloc(sizeof(SensorTask_t));
	
	pSensorTask->uPriorityLevel = uPriorityLevel;
	pSensorTask->uTaskNumber = uTaskNumber;
	pSensorTask->uPeriod = uPeriod;
	pSensorTask->uStartValue = uStartValue;
	pSensorTask->uCurrentValue = uStartValue;
	pSensorTask->uEndValue = uEndValue;
	pSensorTask->uReleaseTime = 0;
	pSensorTask->pUsedQueue = pUsedQueue;

	xTaskCreate(taskHandler, "Task 0", 1000, pSensorTask, pSensorTask->uPriorityLevel, &pSensorTask->xHandle);

	return pSensorTask;
}
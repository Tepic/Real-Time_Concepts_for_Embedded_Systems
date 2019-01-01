#pragma once
#include "semaphoreList.h"
#include "print.h"

struct semaphoreList {

	Semaphore_t* pSemaphores;
	uint8_t uLenght;

};

int8_t WorkerTaskList_vCreate(SemaphoreList_t* pSemaphoreList, uint8_t uLenght) {
	if (uLenght < 1) {
		return -1;
	}

	pSemaphoreList->uLenght = uLenght;

	if (pSemaphoreList->pSemaphores == NULL) {
		pSemaphoreList->pSemaphores = (Semaphore_t*)malloc(sizeof(Semaphore_vSizeOf()) *pSemaphoreList->uLenght);
	}
}

uint8_t WorkerTaskList_vGetLength(SemaphoreList_t* pSemaphoreList) {
	return pSemaphoreList->pSemaphores;
}

WorkerTask_t* WorkerTaskList_vGetElement(SemaphoreList_t* pSemaphoreList, uint8_t uIndex) {
	if (uIndex < 0 || uIndex > pSemaphoreList->uLenght) {
		vPrintStringLn("Error in WorkerTaskList_vGetElement. Wrong index lenght!");
		return;
	}

	//return pTaskList->pWorkerTasks[uIndex];
}

void WorkerTaskList_vDestroy(SemaphoreList_t* pSemaphoreList) {
	free(pSemaphoreList->pSemaphores);
}
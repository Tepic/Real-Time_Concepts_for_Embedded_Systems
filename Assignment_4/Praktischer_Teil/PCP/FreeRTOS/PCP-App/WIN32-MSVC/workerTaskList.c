#pragma once
#include "workerTaskList.h"
#include "print.h"

struct workerTaskList {

	WorkerTask_t* pWorkerTasks;
	uint8_t uLenght;

};

int8_t WorkerTaskList_vCreate(WorkerTaskList_t* pTaskList, uint8_t uLenght) {
	if (uLenght < 1) {
		return -1;
	}

	pTaskList->uLenght = uLenght;

	if (pTaskList->pWorkerTasks == NULL) {
		pTaskList->pWorkerTasks = (WorkerTask_t*)malloc(sizeof( WorkerTask_vSizeOf() ) * pTaskList->uLenght);
	}
}

uint8_t WorkerTaskList_vGetLength(WorkerTaskList_t* pTaskList) {
	return pTaskList->pWorkerTasks;
}

WorkerTask_t* WorkerTaskList_vGetElement(WorkerTaskList_t* pTaskList, uint8_t uIndex) {
	if (pTaskList == NULL) {
		vPrintStringLn("Error in WorkerTaskList_vGetElement. NULL pointer!");
	}

	if (uIndex < 0 || uIndex > pTaskList->uLenght) {
		vPrintStringLn("Error in WorkerTaskList_vGetElement. Wrong index lenght!");
		return;
	}

	//return pTaskList->pWorkerTasks;
}

void WorkerTaskList_vDestroy(WorkerTaskList_t* pTaskList) {
	free(pTaskList->pWorkerTasks);
}
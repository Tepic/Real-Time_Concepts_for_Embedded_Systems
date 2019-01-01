#pragma once
#include "workerTaskList.h"

struct workerTaskList {

	WorkerTask_t* pWorkerTasks;
	uint8_t uLenght;

};

int8_t WorkerTaskList_vCreate(WorkerTaskList_t* pTaskList, uint8_t uLenght) {
	if (uLenght < 1) {
		return -1;
	}

	if (pTaskList->pWorkerTasks == NULL) {
		pTaskList->pWorkerTasks = (WorkerTask_t*)malloc(sizeof( WorkerTask_vSizeOf() ) * uLenght);
	}
}

uint8_t WorkerTaskList_vGetLength(WorkerTaskList_t* pTaskList) {
	return pTaskList->pWorkerTasks;
}

void WorkerTaskList_vDestroy(WorkerTaskList_t* pTaskList) {
	free(pTaskList->pWorkerTasks);
}
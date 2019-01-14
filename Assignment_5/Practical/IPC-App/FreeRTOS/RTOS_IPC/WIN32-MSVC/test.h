#pragma once
#ifndef TEST_H_
#define TEST_H_

/* Kernel includes. */
#include "FreeRTOS.h"

void vTest(TaskFunction_t taskHandler_1,
	TaskFunction_t taskHandler_2,
	TaskFunction_t taskHandler_3,
	TaskFunction_t taskHandler_4);

#endif // !TEST_H_

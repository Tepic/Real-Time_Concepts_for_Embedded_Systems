#pragma once
#ifndef PIP_H_
#define PIP_H_

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

static void PIP_vSemaphoreTake(SemaphoreHandle_t);
static void PIP_vSemaphoreGive(SemaphoreHandle_t);

#endif // PIP_H_
#pragma once
#ifndef PRINT_H_
#define PRINT_H_

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"

static void vPrintStringLn(const char *pcString);
static void vPrintString(const char *pcString);
static void vPrintInteger(int32_t intVal);
static void vPrintUnsignedInteger(uint32_t intVal);
static void vPrintChar(char charVal);

void vPrintStringLn(const char *pcString)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%s\n", pcString);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}

void vPrintInteger(int8_t intVal)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%d", intVal);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}


void vPrintUnsignedInteger(uint32_t usVal) {
	taskENTER_CRITICAL();
	{
		printf("%u", usVal);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}

void vPrintChar(char charVal)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%c", charVal);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}

void vPrintString(const char *pcString)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%s", pcString);
		fflush(stdout);
	}
	taskEXIT_CRITICAL();
}

#endif // PRINT_H_
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

void vPrintStringLn(const char *pcString)
{
	/* Write the string to stdout, using a critical section as a crude method of mutual exclusion. */
	taskENTER_CRITICAL();
	{
		printf("%s", pcString);
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
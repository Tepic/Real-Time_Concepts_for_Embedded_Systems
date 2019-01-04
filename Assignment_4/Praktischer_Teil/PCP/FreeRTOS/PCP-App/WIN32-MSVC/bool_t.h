#pragma once
#ifndef BOOL_H_
#define BOOL_H_

/* Used to define boolean type*/
#include <stdint.h>

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif

typedef uint8_t bool_t;

#endif // BOOL_H
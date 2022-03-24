#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
#include "errors.h"

typedef uint32_t Time_t;

APP_Error_t TM_Init();
Time_t TM_GetTicks();
void TM_Delay(Time_t t);

#endif // __TIMER_H
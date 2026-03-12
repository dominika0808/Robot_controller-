#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include "common.h"

Status_t Tim3InitImputCaptureMode(void);
Status_t Tim3Start(void);
Status_t Tim3SetInterruptPriority(uint32_t priority);
Status_t Tim3RegisterCallback(void(*callback)(void));
uint32_t Tim3GetValue(void);

#endif

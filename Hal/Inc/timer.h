#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include "common.h"

HAL_Status_t halTim3InitImputCaptureMode(void);
HAL_Status_t halTim3Start(void);
HAL_Status_t halTim3SetInterruptPriority(uint32_t priority);
HAL_Status_t halTim3RegisterCallback(void(*callback)(void));
uint32_t halTim3GetValue(void);

#endif

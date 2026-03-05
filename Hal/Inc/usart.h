#ifndef _USART_H
#define _USART_H

#include <stdint.h>
#include "common.h"

HAL_Status_t halUsart2Init(void);
HAL_Status_t halUsart2Put(const char data, uint32_t timeout);
HAL_Status_t halUsart2Send(const char* txt, uint32_t timeout);

HAL_Status_t halUsart3Init(void);
HAL_Status_t halUsart3Put(uint8_t data, uint32_t timeout);
HAL_Status_t halUsart3SendCommand(uint8_t command, int16_t right_motor, int16_t left_motor, uint32_t timeout);

#endif

#ifndef _USART_H
#define _USART_H

#include <stdint.h>
#include "common.h"

Status_t halUsart2Init(void);
Status_t halUsart2Put(const char data, uint32_t timeout);
Status_t halUsart2Send(const char* txt, uint32_t timeout);

Status_t Usart3DMAInit(void);
Status_t Usart3RegisterCallback(void(*callback)(void));
Status_t Usart3TransmitDMA(uint32_t addr_tx);
Status_t Usart3ReceiverDMA(uint32_t addr_rx);

#endif

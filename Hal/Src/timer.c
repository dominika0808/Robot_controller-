#include "timer.h"
#include "stm32l4xx.h"
#include <stddef.h>

#define SYSTEM_CLOCK	4000000

static volatile void (*_timerCallback)(void) = NULL;

Status_t Tim3InitImputCaptureMode(void)
{
	//Turn-on GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	//Set PC6 as alternative function
	GPIOC->MODER &= ~(GPIO_MODER_MODE6_0);

	//Set alternative function AF2 for PC6
	GPIOC->AFR[0] |= GPIO_AFRL_AFSEL6_1;

	//Turn-on TIM3 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	//Select the active input TI1
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0;

	//Falling edge
	TIM3->CCER |= TIM_CCER_CC1P;

	//Enable capture from the counter
	TIM3->CCER |= TIM_CCER_CC1E;

	//Interrupt
	TIM3->DIER |= TIM_DIER_CC1IE;

	NVIC_SetPriority(TIM3_IRQn, 15);
	NVIC_EnableIRQ(TIM3_IRQn);

	return STATUS_OK;
}

Status_t Tim3Start(void)
{
	TIM3->CR1 |= TIM_CR1_CEN;

	return STATUS_OK;
}

Status_t Tim3SetInterruptPriority(uint32_t priority)
{
	NVIC_SetPriority(TIM3_IRQn, priority);

	return STATUS_OK;
}

Status_t Tim3RegisterCallback(void(*callback)(void))
{
	_timerCallback = callback;

	return STATUS_OK;
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR & TIM_SR_CC1IF)
	{
		if(_timerCallback != NULL)
		{
			_timerCallback();
		}

		TIM3->SR &= ~(TIM_SR_CC1IF);
	}
}

uint32_t Tim3GetValue(void)
{
	uint32_t a = TIM3->CCR1;
	TIM3->CNT = 0;
	return a;
}

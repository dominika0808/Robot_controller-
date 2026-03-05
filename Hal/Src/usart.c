#include "usart.h"
#include "stm32l4xx.h"
#include "time.h"

#define AHB2_CLOCK	4000000
#define USART2_BAUD_RATE 115200

HAL_Status_t halUsart2Init(void)
{
	//Turn-on GPIO clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	//Set PA2 and PA3 as alternative function
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_0);
	GPIOA->MODER &= ~(GPIO_MODER_MODE3_0);

	//Set alternative function AF7 for PA2 and PA3
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2;

	//PA3 open-drein
	GPIOA->OTYPER |= GPIO_OTYPER_OT3;

	//Turn-on USART2 clock
	RCC->APB1ENR1 = RCC_APB1ENR1_USART2EN;

	//USART baud rate 115200
	USART2->BRR = AHB2_CLOCK/USART2_BAUD_RATE;

	//USART enable
	USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;


	return HAL_OK;
}

HAL_Status_t halUsart2Put(const char data, uint32_t timeout)
{
	uint32_t ticks = halGetTicks();

	while(!(USART2->ISR & USART_ISR_TXE))
	{
		if(halGetTicks() > ticks + timeout)
		{
			return HAL_TIMEOUT;
		}

	}

	USART2->TDR = data;

	return HAL_OK;
}

HAL_Status_t halUsart2Send(const char* txt, uint32_t timeout)
{
	while(*txt != 0)
	{
		HAL_Status_t status = halUsart2Put(*txt, timeout);

		if(status == HAL_TIMEOUT)
		{
			return HAL_TIMEOUT;
		}
		txt++;
	}

	return HAL_OK;
}

HAL_Status_t halUsart3Init(void)
{
	//Turn-on GPIO clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

	//Set PB10 and PB11 as alternative function
	GPIOB->MODER &= ~(GPIO_MODER_MODE10_0);
	GPIOB->MODER &= ~(GPIO_MODER_MODE11_0);

	//Set alternative function AF7 for PA2 and PA3
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL11_0 | GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2;

	//PB10 open-drein
	GPIOB->OTYPER |= GPIO_OTYPER_OT10;

	//Turn-on USART3 clock
	RCC->APB1ENR1 = RCC_APB1ENR1_USART3EN;

	//USART baud rate 115200
	USART3->BRR = AHB2_CLOCK/USART2_BAUD_RATE;

	//USART enable
	USART3->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;


	return HAL_OK;
}

HAL_Status_t halUsart3Put(uint8_t data, uint32_t timeout)
{
	uint32_t ticks = halGetTicks();

	while(!(USART3->ISR & USART_ISR_TXE))
	{
		if(halGetTicks() > ticks + timeout)
		{
			return HAL_TIMEOUT;
		}
	}

	USART3->TDR = data;

	return HAL_OK;
}

HAL_Status_t halUsart3SendCommand(uint8_t command, int16_t right_motor, int16_t left_motor, uint32_t timeout)
{
	HAL_Status_t status = halUsart3Put(command, timeout);

	if(status == HAL_TIMEOUT) return HAL_TIMEOUT;

	uint8_t rm1 = (right_motor >> 8) & 0xFF;
	uint8_t rm2 = right_motor & 0xFF;

	uint8_t lm1 = (left_motor >> 8) & 0xFF;
	uint8_t lm2 = left_motor & 0xFF;

	HAL_Status_t status1 = halUsart3Put(rm1, timeout);

	if(status1 == HAL_TIMEOUT) return HAL_TIMEOUT;

	HAL_Status_t status2 = halUsart3Put(rm2, timeout);

	if(status2 == HAL_TIMEOUT) return HAL_TIMEOUT;

	HAL_Status_t status3 = halUsart3Put(lm1, timeout);

	if(status3 == HAL_TIMEOUT) return HAL_TIMEOUT;

	HAL_Status_t status4 = halUsart3Put(lm2, timeout);

	if(status4 == HAL_TIMEOUT) return HAL_TIMEOUT;

	return HAL_OK;
}

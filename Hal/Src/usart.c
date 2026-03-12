#include "usart.h"
#include "stm32l4xx.h"
#include "time.h"
#include <stddef.h>

#define AHB2_CLOCK			4000000
#define USART2_BAUD_RATE 	115200
#define DMA_CSELR_C2S_1 	(0x1UL << (5U))
#define DMA_CSELR_C3S_1 	(0x1UL << (5U))

static volatile void (*_usartCallback)(void) = NULL;

Status_t halUsart2Init(void)
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


	return STATUS_OK;
}

Status_t halUsart2Put(const char data, uint32_t timeout)
{
	uint32_t ticks = halGetTicks();

	while(!(USART2->ISR & USART_ISR_TXE))
	{
		if(halGetTicks() > ticks + timeout)
		{
			return STATUS_TIMEOUT;
		}

	}

	USART2->TDR = data;

	return STATUS_OK;
}

Status_t halUsart2Send(const char* txt, uint32_t timeout)
{
	while(*txt != 0)
	{
		Status_t status = halUsart2Put(*txt, timeout);

		if(status == STATUS_TIMEOUT)
		{
			return STATUS_TIMEOUT;
		}
		txt++;
	}

	return STATUS_OK;
}

Status_t Usart3DMAInit(void)
{
	//Clock configuration
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	//Set PB10 and PB11 as alternative function
	GPIOB->MODER &= ~(GPIO_MODER_MODE10_0);
	GPIOB->MODER &= ~(GPIO_MODER_MODE11_0);

	//Set alternative function AF7 for PA2 and PA3
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL11_0 | GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2;

	//PB11 open-drein
	GPIOB->OTYPER |= GPIO_OTYPER_OT11;

	//USART baud rate 115200
	USART3->BRR = AHB2_CLOCK/USART2_BAUD_RATE;

	//DMA transmitter and receiver enable
	USART3->CR3 |= USART_CR3_DMAT;
	USART3->CR3 |= USART_CR3_DMAR;

	//DMA channel selection
	DMA1_CSELR->CSELR |= DMA_CSELR_C2S_1;
	DMA1_CSELR->CSELR |= DMA_CSELR_C3S_1;

	NVIC_SetPriority(DMA1_Channel2_IRQn, 15);
	//NVIC_SetPriority(DMA1_Channel3_IRQn, 15);
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);
	//NVIC_EnableIRQ(DMA1_Channel3_IRQn);

	//IDEL interrupt
	//USART3->CR1 |= USART_CR1_IDLEIE;

	//USART enable
	USART3->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

	return STATUS_OK;
}

Status_t Usart3TransmitDMA(uint32_t addr_tx)
{
	//Disable channel
	DMA1_Channel2->CCR = 0;

	//Set the peripheral register address
	DMA1_Channel2->CPAR = (uint32_t)&USART3->TDR;

	//Set the memory address
	DMA1_Channel2->CMAR = addr_tx;

	//Numbers of data
	DMA1_Channel2->CNDTR = 5;

	//Channel priority - medium
	DMA1_Channel2->CCR |= DMA_CCR_PL_0;

	//Data transfer direction
	DMA1_Channel2->CCR |= DMA_CCR_DIR;

	//Interrupt - transfer error, transfer complete
	DMA1_Channel2->CCR |= DMA_CCR_TEIE;
	DMA1_Channel2->CCR |= DMA_CCR_TCIE;

	//Memory increment mode
	DMA1_Channel2->CCR |= DMA_CCR_MINC;

	//DMA enable
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	return STATUS_OK;
}

Status_t Usart3ReceiverDMA(uint32_t addr_rx)
{
	//Disable channel
	DMA1_Channel3->CCR = 0;

	//Set the peripheral register address
	DMA1_Channel3->CPAR = (uint32_t)&USART3->RDR;

	//Set the memory address
	DMA1_Channel3->CMAR = addr_rx;

	//Numbers of data
	DMA1_Channel3->CNDTR = 5;

	//Channel priority - medium
	DMA1_Channel3->CCR |= DMA_CCR_PL_0;

	//Interrupt - transfer error, transfer complete
	DMA1_Channel3->CCR |= DMA_CCR_TEIE;

	//Memory increment mode
	DMA1_Channel3->CCR |= DMA_CCR_MINC;

	//DMA enable
	DMA1_Channel3->CCR |= DMA_CCR_EN;

	return STATUS_OK;
}

Status_t Usart3RegisterCallback(void(*callback)(void))
{
	_usartCallback = callback;

	return STATUS_OK;
}

void DMA1_CH2_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_GIF2)
	{
		if(_usartCallback != NULL)
		{
			_usartCallback();
		}

		DMA1->IFCR |= DMA_IFCR_CGIF2 | DMA_IFCR_CHTIF2 | DMA_IFCR_CTCIF2;
	}
}

#include "../include/usart.h"
#include "../include/addons.h"

void usart_init()
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;		// USART1 clock enable
	USART1->CR1 |= USART_CR1_UE;				// USART enable
	USART1->CR1 |= USART_CR1_RXNEIE;			// interrupt of transmision complete
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmitter and receiver enable
	USART1->BRR = (8000000 / 9600);				// baudrate
}

void usart_gpio_init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;				  // enable clock to GPIOA
	GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1; // PA9 as TX, alt. output P-P, 2MHz
	GPIOA->CRH &= ~GPIO_CRH_CNF9_0;					  // disable floating input
													  // PA10 as input, floating point
}

void UART1_Send(int X, int Y, int PA5)
{
	/*********** STEPS FOLLOWED *************

	1. Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this
		 for each data to be transmitted in case of single buffer.
	2. After writing the last data into the USART_DR register, wait until TC=1. This indicates
		 that the transmission of the last frame is complete. This is required for instance when
		 the USART is disabled or enters the Halt mode to avoid corrupting the last transmission.

	****************************************/
	USART1->DR = 123; // wysłanie {
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 88; // wysłanie X
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 58; // wysłanie :
	while (!(USART1->SR & USART_SR_TC))
		;

	send_int_in_ascci(X); // wysłanie wartości osi X

	USART1->DR = 44; // wysłanie ,
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 89; // wysłanie Y
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 58; // wysłanie :
	while (!(USART1->SR & USART_SR_TC))
		;

	send_int_in_ascci(Y); // wysłanie wartości osi X

	USART1->DR = 44; // wysłanie ,
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 80; // wysłanie P
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 34; // wysłanie "
	while (!(USART1->SR & USART_SR_TC))
		;

	USART1->DR = 58; // wysłanie :
	while (!(USART1->SR & USART_SR_TC))
		;

	send_int_in_ascci(PA5); // wysłanie wartości osi X

	USART1->DR = 125; // wysłanie }
	while (!(USART1->SR & USART_SR_TC))
		;
}

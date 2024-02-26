#include "stm32f103xb.h"
#include <stdlib.h>

volatile uint32_t delay_var;
static volatile uint16_t adc_data[2];
int volatile button_a5 , przecinek = 44;
int volatile Ytosend , Xtosend;
int volatile operacje;
int reverse(int liczba)
{
    int reverse = 0, remainder;
    while (liczba != 0) {
        remainder = liczba % 10;
        reverse = reverse * 10 + remainder;
        liczba /= 10;
        operacje+=1;
    }
    return reverse;
}

void send_int_in_ascci(int liczba){
    int loop =1;
    if(liczba==0){
        USART1->DR =80;   // wysłanie 0
	    while (!(USART1->SR & USART_SR_TC));
        loop=0;
    }
    operacje =0;
    liczba = reverse(liczba);
    while (liczba !=0)
    {
        USART1->DR =liczba%10+80;   // wysłanie liczby <1,9>
	    while (!(USART1->SR & USART_SR_TC));
        operacje-=1;
        liczba = liczba/10;
    }
    while(operacje!=0){
        USART1->DR =80;   // wysłanie 0
	    while (!(USART1->SR & USART_SR_TC));
        operacje -=1;
    }
}

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

void SysTick_Handler(void)
{
    if (delay_var) delay_var--;
}

void delay_ms(uint32_t value)
{
    delay_var = value;
    while(delay_var);
}

void UART1_SendChar ()
{
	/*********** STEPS FOLLOWED *************
	
	1. Write the data to send in the USART_DR register (this clears the TXE bit). Repeat this
		 for each data to be transmitted in case of single buffer.
	2. After writing the last data into the USART_DR register, wait until TC=1. This indicates
		 that the transmission of the last frame is complete. This is required for instance when
		 the USART is disabled or enters the Halt mode to avoid corrupting the last transmission.
	
	****************************************/
    Ytosend = adc_data[1];
    Xtosend = adc_data[0];
    USART1->DR =120;   // wysłanie X
	while (!(USART1->SR & USART_SR_TC));

    USART1->DR =61;   // wysłanie =
	while (!(USART1->SR & USART_SR_TC));

    send_int_in_ascci(Xtosend);  // wysłanie wartości osi X

    USART1->DR =13;   // wysłanie enter
	while (!(USART1->SR & USART_SR_TC));

    USART1->DR =121;   // wysłanie Y
	while (!(USART1->SR & USART_SR_TC));

    USART1->DR =61;   // wysłanie =
	while (!(USART1->SR & USART_SR_TC));

    send_int_in_ascci(Ytosend);  // wysłanie wartości osi Y

    USART1->DR =13;   // wysłanie enter
	while (!(USART1->SR & USART_SR_TC));

        USART1->DR =112;   // wysłanie P
	while (!(USART1->SR & USART_SR_TC));

    USART1->DR =61;   // wysłanie =
	while (!(USART1->SR & USART_SR_TC));

    USART1->DR =13;   // wysłanie enter
	while (!(USART1->SR & USART_SR_TC));
}

int main(void) {

    SysTick_Config(8000000/1000);
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;


    // ustawienia gpio
    // PA0 - ADC1-IN0
    // PA1 - ADC1-IN1
    // PA5 - Click
    GPIOA->CRL &= ~GPIO_CRL_CNF0_0; // wejscie analogowe PA0
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0; // wejscie analogowe PA1
    //ustawienia dma
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; // 16 bit bo uint16_t
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1; // 32 bit chociaz nieykorzystane bo adc ma 12 bit
    DMA1_Channel1->CCR |= DMA_CCR_CIRC; // kołowy tryb
    DMA1_Channel1->CCR |= DMA_CCR_MINC; // memory incrementation, bo adc_data[2]
    DMA1_Channel1->CNDTR = 2;   // dwie konwersje
    DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);    // skad ma brać dane
    DMA1_Channel1->CMAR = (uint32_t)adc_data;       // gdzie ma wrzucac dane
    DMA1_Channel1->CCR |= DMA_CCR_EN;               // wlacz DMA


    // ustawienia adc
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SQR1 = ADC_SQR1_L_0;
    ADC1->SQR3 |= 0<<ADC_SQR3_SQ1_Pos;
    ADC1->SQR3 |= 1<<ADC_SQR3_SQ2_Pos;
    ADC1->CR2 |=ADC_CR2_DMA;
    ADC1->CR1 |=ADC_CR1_SCAN;
    usart_init();
    usart_gpio_init();
    while(1)
    {   
        UART1_SendChar();
        ADC1->CR2 |= ADC_CR2_ADON;
        if(GPIOA -> IDR & GPIO_IDR_IDR5){
             button_a5 = 1;
        }
        else{
             button_a5 = 0;
        }

    } /* main */

}


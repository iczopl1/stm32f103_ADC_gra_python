#include "stm32f103xb.h"

extern volatile uint16_t adc_data[2];

void adcdma()
{
    // ustawienia gpio
    // PA0 - ADC1-IN0
    // PA1 - ADC1-IN1
    // PA5 - Click
    GPIOA->CRL &= ~GPIO_CRL_CNF5_0;
    GPIOA->CRL |= GPIO_CRL_CNF5_1;
    GPIOA->ODR |= GPIO_ODR_ODR5;

    GPIOA->CRL &= ~GPIO_CRL_CNF0_0; // wejscie analogowe PA0
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0; // wejscie analogowe PA1
    // ustawienia dma
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;         // 16 bit bo uint16_t
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_1;         // 32 bit chociaz nieykorzystane bo adc ma 12 bit
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;            // kołowy tryb
    DMA1_Channel1->CCR |= DMA_CCR_MINC;            // memory incrementation, bo adc_data[2]
    DMA1_Channel1->CNDTR = 2;                      // dwie konwersje
    DMA1_Channel1->CPAR = (uint32_t) & (ADC1->DR); // skad ma brać dane
    DMA1_Channel1->CMAR = (uint32_t)adc_data;      // gdzie ma wrzucac dane
    DMA1_Channel1->CCR |= DMA_CCR_EN;              // wlacz DMA

   // ustawienia adc
    ADC1->CR2 |= ADC_CR2_ADON; // włączenie przetwornika
    ADC1->SQR1 = ADC_SQR1_L_0; //Ustawienie ilości konwersji w sekwencji regularnej. W tym przypadku ustawienie jednej konwersji 
    ADC1->SQR3 |= 0 << ADC_SQR3_SQ1_Pos;//Konfiguracja pierwszego kanału w sekwencji konwersji. W tym przypadku kanał 0 (PA0).
    ADC1->SQR3 |= 1 << ADC_SQR3_SQ2_Pos;//Konfiguracja drugiego kanału w sekwencji konwersji. W tym przypadku kanał 1 (PA1).
    ADC1->CR2 |= ADC_CR2_DMA;//Włączenie trybu DMA dla ADC. Tryb DMA umożliwia bezpośredni transfer danych z ADC do pamięci
    ADC1->CR1 |= ADC_CR1_SCAN;//Włączenie trybu skanowania (SCAN) dla ADC. W trybie skanowania, po zakończeniu konwersji na kanale, następuje automatyczne przejście do kolejnego kanału w sekwencji konwersji.
}
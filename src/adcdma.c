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
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SQR1 = ADC_SQR1_L_0;
    ADC1->SQR3 |= 0 << ADC_SQR3_SQ1_Pos;
    ADC1->SQR3 |= 1 << ADC_SQR3_SQ2_Pos;
    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR1 |= ADC_CR1_SCAN;
}
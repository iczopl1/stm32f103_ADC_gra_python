#include "stm32f103xb.h"
#include <stdlib.h>
#include "../include/usart.h"
#include "../include/adcdma.h"

volatile uint32_t delay_var;
volatile uint16_t adc_data[2];
int volatile button_a5, zapisane_button_a5;
int volatile Ytosend, Xtosend;

void SysTick_Handler(void)
{
    if (delay_var)
        delay_var--;
}

void delay_ms(uint32_t value)
{
    delay_var = value;
    while (delay_var)
        ;
}

int main(void)
{
    //obliczanie martfej strefy dla analoga 2047 to połowa wartości maksymalnej analoga 
    int martfa_strefa_x=2047,martfa_strefa_y=martfa_strefa_x;
    //przy jakim procencie odchylenie ma zaczącz nadawać
    //jestem ciekaw czy przy takim rozwiązani w pozyci zerowej będzie smużycz a przy maksymalnym wychyleniu da radę
    int wielkosc_martfej_strefy=martfa_strefa_x*0.2;
    int dolna_martfa_strefa_x = martfa_strefa_x-wielkosc_martfej_strefy;
    int gurna_martfa_strefa_x = martfa_strefa_x+wielkosc_martfej_strefy;
    int dolna_martfa_strefa_y = martfa_strefa_y-wielkosc_martfej_strefy;
    int gurna_martfa_strefa_y = martfa_strefa_y+wielkosc_martfej_strefy;
    SysTick_Config(8000000 / 1000);
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    adcdma();
    usart_init();
    usart_gpio_init();
    while (1)
    {
        Ytosend = adc_data[1];
        Xtosend = adc_data[0];

        if (button_a5 != zapisane_button_a5 || Xtosend > gurna_martfa_strefa_x || Xtosend < dolna_martfa_strefa_x || Ytosend > gurna_martfa_strefa_y || Ytosend < dolna_martfa_strefa_y)
        {
            zapisane_button_a5 = button_a5;
            UART1_SendChar(Xtosend, Ytosend, button_a5);
        }
        ADC1->CR2 |= ADC_CR2_ADON;
        if (GPIOA->IDR & GPIO_IDR_IDR5)
        {
            button_a5 = 1;
        }
        else
        {
            button_a5 = 0;
        }

    } /* main */
}

#include "stm32f103xb.h"
#include <stdlib.h>
int volatile operacje;
int reverse(int liczba)
{
    int reverse = 0, remainder;
    while (liczba != 0)
    {
        remainder = liczba % 10;
        reverse = reverse * 10 + remainder;
        liczba /= 10;
        operacje += 1;
    }
    return reverse;
}

void send_int_in_ascci(int liczba)
{
    int loop = 1;
    if (liczba == 0)
    {
        USART1->DR = 48; // wysłanie 0
        while (!(USART1->SR & USART_SR_TC))
            ;
        loop = 0;
    }
    operacje = 0;
    liczba = reverse(liczba);
    while (liczba != 0)
    {
        USART1->DR = liczba % 10 + 48; // wysłanie liczby <1,9>
        while (!(USART1->SR & USART_SR_TC))
            ;
        operacje -= 1;
        liczba = liczba / 10;
    }
    while (operacje != 0)
    {
        USART1->DR = 48; // wysłanie 0
        while (!(USART1->SR & USART_SR_TC))
            ;
        operacje -= 1;
    }
}

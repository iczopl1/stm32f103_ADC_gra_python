#include "stm32f103xb.h"
#include <stdlib.h>
#include <stdio.h>


void usart_init();
void usart_gpio_init();
void UART1_Send(int X,int Y,int PA5);
void send_int_in_ascci(int liczba);
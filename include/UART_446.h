//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// シリアル通信
// 2019.2.3 sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef UART_446_H
#define UART_446_H

#include "stm32f446xx.h"

void PA2_PA3_TXRX_init();
void UART2_init();
void UART2_RX_Intrpt();
void UART2_send_char(char c);
void PutcUSART2(char c);
#endif
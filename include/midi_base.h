//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// MIDIの設定
// 2019.2.3 sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef MIDI_BASE_H
#define MIDI_BASE_H

#include "stm32f446xx.h"
//#include "buffer.h"

void midi_base_init();
void PC10_PC11_TXRX_init();
void UART3_init();
void USART3_IRQ_enable();
void USART3_IRQ_disable();
void send_char(char c);
void send_str(const char *str);
void send_int(int n);
void send_ints3(int n);
void UART3_send_char(char c);
void PutcUSART3(char c);

#endif
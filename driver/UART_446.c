//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// シリアル通信
// 2019.2.3 sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef UART_446_C
#define UART_446_C

#include "UART_446.h"

// =====================================================================================================================
//  UART2 GPIO PA2(Tx) PA3(Rx) setting
// =====================================================================================================================
 void PA2_PA3_TXRX_init() {
    RCC-> AHB1ENR      |= RCC_AHB1ENR_GPIOAEN;         // GPIO-C clock enable from AHB1
     
     // PA2
    GPIOA-> MODER      |= 0b10 << GPIO_MODER_MODE2_Pos; // PA2の役割設定をオルタネイト(0b10)に設定
    GPIOA-> PUPDR      |= 0b01 << GPIO_PUPDR_PUPD2_Pos; // PA2をプルダウン(0b01)に設定
    GPIOA-> AFR[0]     |= 0b0111 << 8; //オルタネイトファンクションの種類を設定(0x7=UART2_Tx)
    
     // PA3
    GPIOA-> MODER      |= 0b10 << GPIO_MODER_MODE3_Pos; // PA3の役割設定をオルタネイト(0b10)に設定
    GPIOA-> PUPDR      |= 0b01 << GPIO_PUPDR_PUPD3_Pos; // PA3をプルダウン(0b01)に設定
    GPIOA-> AFR[0]     |= 0b0111 << 12; //オルタネイトファンクションの種類を設定(0x7=UART2_Rx)

	/*
	Note:
	Altanate Function table see STM32F446xR/C MCU Refarence Manual
	*/
 }
// =====================================================================================================================
//  UART2 init 
// =====================================================================================================================
void UART2_init() {
	PA2_PA3_TXRX_init();
	// UART 2 Clock enable on APB1(45MHz)
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	USART2-> BRR = 0;
	USART2-> BRR = (24 << 4) + 7; //baud ramakete 115.2kHz
    USART2-> CR1 = 0x0;
    //USART3->CR1 &= ~USART_CR1_OVER8; 
    USART2-> CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; //UART通信をenable | Tx enable | Rx enable
    USART2-> CR2 = 0x0;
    USART2-> CR3 = 0x0;

    //UART2_RX_Intrpt();
	/*
	note:
	115200Hz(115.2KHz)
	USARTDIV = 45000000/(8×(2-0)×115200)
	USARTDIV = 24.4140625
	24.7::24.4375->-0.09600000000000364%
	*/
}
// =====================================================================================================================
//  USART2 Rx Interrupt Enable
// =====================================================================================================================

inline void UART2_RX_Intrpt(){
    USART2->CR1 |= (0b1 << USART_CR1_RXNEIE_Pos);
    NVIC_EnableIRQ(USART2_IRQn);
}
// =====================================================================================================================
//  PA2 TX PA3 RX
// =====================================================================================================================
inline void UART2_send_char(char c){
	while (!(USART2->SR & USART_SR_TXE)); //SRのTXEが0のとき､送信データは入力できないので待つ｡
    USART2-> DR = c; 
}

void UART2_send_uchar(unsigned char c){
	while (!(USART2->SR & USART_SR_TXE)); //SRのTXEが0のとき､送信データは入力できないので待つ｡
    USART2-> DR = c; 
}

void PutcUSART2(char c)
{
    if(c == '\n'){
        UART2_send_char('\r');
    }
    UART2_send_char(c);
}


#endif
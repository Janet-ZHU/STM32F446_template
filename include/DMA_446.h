//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// 2019.03.15 Sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef DMA_446_H
#define DMA_446_H

#include "stm32f446xx.h"

// =====================================================================================================================
//  DMA USART2 Tx Init Stream6 ch4 Tx Uart2
// =====================================================================================================================
void DMA_Uart2_Tx_Init(uint8_t * memory_adrs);
// =====================================================================================================================
//  DMA USART2 Rx Init Stream5 ch4 Rx Uart2
// =====================================================================================================================
void DMA_Uart2_Rx_Init(uint8_t * memory_adrs);

#endif
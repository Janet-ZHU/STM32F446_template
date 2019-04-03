//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// 2019.03.15 Sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef DMA_446_C
#define DMA_446_C

#include "DMA_446.h"

// =====================================================================================================================
//  DMA USART2 Tx Init Stream6 ch4 Tx Uart2
// =====================================================================================================================
void DMA_Uart2_Tx_Init(uint8_t * memory_adrs){
    // DMA1 Clock Enable
    RCC->AHB1ENR |= (0b1 << RCC_AHB1ENR_DMA1EN_Pos);
        // FCR レジスタ
    DMA1_Stream6->FCR = (0b0 << DMA_SxFCR_FEIE_Pos) | (0b0 << DMA_SxFCR_DMDIS_Pos) | (0b00 << DMA_SxFCR_FTH_Pos);
    // コントロール レジスタ
    DMA1_Stream6->CR = 0; // reset
    DMA1_Stream6->CR = (0b100 << DMA_SxCR_CHSEL_Pos) |  /* CH 0-7ch */
                         (0b00 << DMA_SxCR_MBURST_Pos) |  /* メモリバースト 00:シングル */
                          (0b00 << DMA_SxCR_PBURST_Pos) |  /*ペリフェラルバースト 00:シングル */
                          (0b0 << DMA_SxCR_CT_Pos) |  /* ダブルバッファのターゲットメモリ（現在値or書き換え）  */
                          (0b0 << DMA_SxCR_DBM_Pos) |  /* 0:ダブルバッファ無効 */
                          (0b11 << DMA_SxCR_PL_Pos) |  /* 優先度 11:最高 */
                          (0b0 << DMA_SxCR_PINCOS_Pos) |  /* ペリフェラルオフセットサイズ 0:PSIZE 1:32ビット固定 */
                          (0b00 << DMA_SxCR_MSIZE_Pos) |  /*メモリオフセット 00:8bit 01:ハーフ 10:ワード */
                          (0b00 << DMA_SxCR_PSIZE_Pos) |  /* ペリフェラルオフセット 00:8bit 01:ハーフ 10:ワード */
                          (0b1 << DMA_SxCR_MINC_Pos) |  /* メモリインクリメント 0:固定 1:MSIZE */
                          (0b0 << DMA_SxCR_PINC_Pos) |  /* ペリフェラルインクリメント 0:固定 1:PSIZE */
                          (0b1 << DMA_SxCR_CIRC_Pos) |  /* 1:サーキュラ有効 ダブルバッファ有効で強制有効 ペリフェラルがフローコントローラのとき強制無効 */
                          (0b01 << DMA_SxCR_DIR_Pos) |  /* 00:P->M 01:M->P 10:M->M */
                          (0b0 << DMA_SxCR_PFCTRL_Pos) |  /* 0:MDAフローコントローラ 1:ペリフェラル */
                          (0b0 << DMA_SxCR_TCIE_Pos) |  /* 転送完了割り込み */
                          (0b0 << DMA_SxCR_HTIE_Pos) |  /* 1/2完了割り込み */
                          (0b0 << DMA_SxCR_TEIE_Pos) |  /* 転送エラー割り込み */
                          (0b0 << DMA_SxCR_DMEIE_Pos) |   /* ダイレクトモードエラー割り込み */
                          (0b0 << DMA_SxCR_EN_Pos); /* ストリーム有効 */
    //DMA1_Stream6->CR = 0xd430540;
    // データ数レジスタ
    DMA1_Stream6->NDTR = 4;
    // ペリフェラルのレジスタアドレス
    DMA1_Stream6->PAR = (uint32_t) & USART2->DR;
    // メモリ0のレジスタアドレス
    DMA1_Stream6->M0AR = (uint32_t)memory_adrs;
    //DMA enable
    DMA1_Stream6->CR |= (0b1 << DMA_SxCR_EN_Pos); // LAST!! DMA START BIT !!
    //USART2 Tx DMA Enable
    USART2->CR3 |= (0b1 << USART_CR3_DMAT_Pos);
}

// =====================================================================================================================
//  DMA USART2 Rx Init Stream5 ch4 Rx Uart2
// =====================================================================================================================
void DMA_Uart2_Rx_Init(uint8_t * memory_adrs){
    // DMA1 Clock Enable
    RCC->AHB1ENR |= (0b1 << RCC_AHB1ENR_DMA1EN_Pos);
        // FCR レジスタ
    DMA1_Stream5->FCR = (0b0 << DMA_SxFCR_FEIE_Pos) |   /* FIFO エラー割り込み1:Enable */
                        (0b00 << DMA_SxFCR_FS_Pos) | /* FIFO Status 0b011 Full*/
                        (0b1 << DMA_SxFCR_DMDIS_Pos) | /* ダイレクトモード0 enable 1 disable */
                        (0b00 << DMA_SxFCR_FTH_Pos); /* FIFO 閾値 */
    // コントロール レジスタ
    DMA1_Stream5->CR = 0; // reset
    DMA1_Stream5->CR = (0b100 << DMA_SxCR_CHSEL_Pos) |  /* CH 0-7ch */
                         (0b00 << DMA_SxCR_MBURST_Pos) |  /* メモリバースト 00:シングル */
                          (0b00 << DMA_SxCR_PBURST_Pos) |  /*ペリフェラルバースト 00:シングル */
                          (0b0 << DMA_SxCR_CT_Pos) |  /* ダブルバッファのターゲットメモリ（現在値or書き換え） M0 or M1 */
                          (0b0 << DMA_SxCR_DBM_Pos) |  /* 0:ダブルバッファ無効 */
                          (0b11 << DMA_SxCR_PL_Pos) |  /* 優先度 11:最高 */
                          (0b0 << DMA_SxCR_PINCOS_Pos) |  /* ペリフェラルオフセットサイズ 0:PSIZE 1:32ビット固定 */
                          (0b00 << DMA_SxCR_MSIZE_Pos) |  /*メモリオフセット 00:8bit 01:ハーフ 10:ワード */
                          (0b00 << DMA_SxCR_PSIZE_Pos) |  /* ペリフェラルオフセット 00:8bit 01:ハーフ 10:ワード */
                          (0b1 << DMA_SxCR_MINC_Pos) |  /* メモリインクリメント 0:固定 1:MSIZE */
                          (0b0 << DMA_SxCR_PINC_Pos) |  /* ペリフェラルインクリメント 0:固定 1:PSIZE */
                          (0b0 << DMA_SxCR_CIRC_Pos) |  /* 1:サーキュラ有効 ダブルバッファ有効で強制有効 ペリフェラルがフローコントローラのとき強制無効 */
                          (0b00 << DMA_SxCR_DIR_Pos) |  /* 00:P->M 01:M->P 10:M->M */
                          (0b0 << DMA_SxCR_PFCTRL_Pos) |  /* 0:MDAフローコントローラ 1:ペリフェラル */
                          (0b0 << DMA_SxCR_TCIE_Pos) |  /* 転送完了割り込み */
                          (0b0 << DMA_SxCR_HTIE_Pos) |  /* 1/2完了割り込み */
                          (0b0 << DMA_SxCR_TEIE_Pos) |  /* 転送エラー割り込み */
                          (0b0 << DMA_SxCR_DMEIE_Pos) |   /* ダイレクトモードエラー割り込み */
                          (0b0 << DMA_SxCR_EN_Pos); /* ストリーム有効 */
    //DMA1_Stream5->CR = 0xd430540;
    // データ数レジスタ
    DMA1_Stream5->NDTR = 20;
    // ペリフェラルのレジスタアドレス
    DMA1_Stream5->PAR = (uint32_t) & USART2->DR;
    // メモリ0のレジスタアドレス
    DMA1_Stream5->M0AR = (uint32_t)memory_adrs;
    //DMA enable
    DMA1_Stream5->CR |= (0b1 << DMA_SxCR_EN_Pos); // LAST!! DMA START BIT !!
    //USART2 Tx DMA Enable
    USART2->CR3 |= (0b1 << USART_CR3_DMAR_Pos);
}

#endif

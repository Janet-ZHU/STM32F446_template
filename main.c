#include "F446_init.h"
#include "GPIO_446.h"
//#include "Timer_446.h"
#include "UART_446.h"
#include "printf.h"
#include "midi_base.h"
#include "fifo.h"



unsigned int message[3] = {0xF9,0xF7,0xF5};
unsigned int out_buf[1] = {0};


// =====================================================================================================================
//  delay func
// =====================================================================================================================
void delay (int a)
{
    volatile int i,j;

    for (i=0 ; i < a ; i++)
    {
        j++;
    }
}

// =====================================================================================================================
//  UART3 IRQ callback Function 
// バッファーが空ならば割り込み継続、バッファーがからでなければ送信
// =====================================================================================================================

void USART2_IRQ_enable(){
    USART2-> CR1 |= USART_CR1_RXNEIE;
}
void USART2_IRQ_disable(){
    USART2-> CR1 &= ~USART_CR1_RXNEIE;    
}
void USART3_IRQHandler() {
    static int flag = 0;
    static unsigned int counter = 0;
    
    printf("%x\n", USART2->DR);
    USART2_IRQ_disable(); // TXEIE Off
    flag =! flag; // toggle flag

    if( flag == 0 ) { 
        GPIOA->BSRR = 1 << 5; // PA5 LED On
    }
    else{
        GPIOA->BSRR = 1 << 21; // PA5 LED Off
    }

}
// =====================================================================================================================
//  UART3 IRQ callback Function 
// バッファーが空ならば割り込み継続、バッファーがからでなければ送信
// =====================================================================================================================
void USART2_IRQHandler() {
    static int flag = 0;
    static unsigned int counter = 0;
    uint8_t data = USART2->DR;
    printf("%x\n", data);
    USART2_IRQ_disable(); // TXEIE Off
    flag =! flag; // toggle flag

    if( flag == 0 ) { 
        GPIOA->BSRR = 1 << 5; // PA5 LED On
    }
    else{
        GPIOA->BSRR = 1 << 21; // PA5 LED Off
    }
}

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
// =====================================================================================================================
//  M A I N
// =====================================================================================================================
int main(void)
{
    uint8_t dma_memory[20];
    uint8_t * dma_memory_p;
    uint8_t dma_data = 0xFA;
    uint32_t flag = 0;
    dma_memory_p = dma_memory;
    uint8_t dma_rx_data = 0;
    dma_memory[0] = 0x01;
    dma_memory[1] = 0x0F;
    dma_memory[2] = 0x03;
    dma_memory[3] = 0x04;


    HSE_PLL_init(); // System Clock:180MHz 
    //midi_base_init(); //UART init
   
    GPIO_init();
    UART2_init(); // 115200HzでNucleo USBから出力 printfで使用可能
    UART3_init();
    init_printf(PutcUSART2);
    //DMA_Uart2_Tx_Init(dma_memory_p);
    DMA_Uart2_Rx_Init(dma_memory_p);
    //WriteBuf(message,3);
    //ReadBuf(out_buf, 1);
    //send_int(out_buf[0]);

//------------------------------------------------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------------------------------------------------

    while(1){
        //delay(1000000);
        for(int i=0;i<20; i++){
            printf(" %x",dma_memory[i]);
        }
        printf(" %x\n",DMA1_Stream5->NDTR);


        //printf("E %x\n",DMA1->HIFCR);
        //printf("%x\n",USART2->DR);
        //printf("%x\n", USART2->DR);
        //printf("%x\n", USART2->SR);
    };
}
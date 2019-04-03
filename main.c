#include "F446_init.h"
#include "GPIO_446.h"
//#include "Timer_446.h"
#include "UART_446.h"
#include "printf.h"
//#include "midi_base.h"
#include "fifo.h"
#include "DMA_446.h"


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
//  ADC
// =====================================================================================================================
void gpio_for_adc() {
    RCC->AHB1ENR |= (0b1 << RCC_AHB1ENR_GPIOAEN_Pos) | (0b1 << RCC_AHB1ENR_GPIOBEN_Pos) | (0b1 << RCC_AHB1ENR_GPIOCEN_Pos);
    // GPIOA->MODER = 0;
    // GPIOA->MODER |= (0b11 << GPIO_MODER_MODE0_Pos) | (0b11 << GPIO_MODER_MODE1_Pos) | (0b11 << GPIO_MODER_MODE4_Pos) | (0b11 << GPIO_MODER_MODE5_Pos) | (0b11 << GPIO_MODER_MODE6_Pos) | (0b11 << GPIO_MODER_MODE7_Pos); //| (0b11 << GPIO_MODER_MODE2_Pos) | (0b11 << GPIO_MODER_MODE3_Pos) 
    // GPIOA->OTYPER = 0;
    // GPIOA->OSPEEDR = 0;
    // GPIOA->PUPDR = 0;
    // // GPIO B
    // GPIOB->MODER = 0;
    // GPIOB->MODER |= (0b11 << GPIO_MODER_MODE0_Pos) | (0b11 << GPIO_MODER_MODE1_Pos);
    // GPIOB->OTYPER = 0;
    // GPIOB->OSPEEDR = 0;
    // GPIOB->PUPDR = 0;
    //GPIO C
    GPIOC->MODER = 0;
    GPIOC->MODER = (0b11 << GPIO_MODER_MODE0_Pos) | (0b11 << GPIO_MODER_MODE1_Pos) | (0b11 << GPIO_MODER_MODE2_Pos) | (0b11 << GPIO_MODER_MODE3_Pos) | (0b11 << GPIO_MODER_MODE4_Pos) | (0b11 << GPIO_MODER_MODE5_Pos) ;
    GPIOC->OTYPER = 0;
    GPIOC->OSPEEDR = 0;
    GPIOC->PUPDR = 0;
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
    uint32_t adc_data;


    HSE_PLL_init(); // System Clock:180MHz 
    //midi_base_init(); //UART init
   
    //GPIO_init();
    gpio_for_adc();
    UART2_init(); // 115200HzでNucleo USBから出力 printfで使用可能
    //UART3_init();
    init_printf(PutcUSART2);
    //DMA_Uart2_Tx_Init(dma_memory_p);
    //DMA_Uart2_Rx_Init(dma_memory_p);
    //WriteBuf(message,3);
    //ReadBuf(out_buf, 1);
    //send_int(out_buf[0]);

//------------------------------------------------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------------------------------------------------
    RCC->APB2ENR |= (0b1 << RCC_APB2ENR_ADC1EN_Pos) | (0b1 << RCC_APB2ENR_ADC2EN_Pos) | (0b1 << RCC_APB2ENR_ADC3EN_Pos);
    ADC1->CR2 = 0;
    ADC1->CR1 = 0;  // (0b0 << ADC_CR1_) | 
    ADC1->CR1 |= (0b0 << ADC_CR1_OVRIE_Pos) | /* オーバーライン割り込み有効1  */
                 (0b11 << ADC_CR1_RES_Pos) |/*  分解能 00:12bit 01:10bit 10:8it 11:6bit */
                 (0b0 << ADC_CR1_AWDEN_Pos) |/* アナログウォッチドッグ有効:1 レギュラ */
                 (0b0 << ADC_CR1_JAWDEN_Pos) | /* アナログウォッチドッグ有効:1 インジェクト */
                 (0b000 << ADC_CR1_DISCNUM_Pos) | /* 不連続モードチャンネル0から8 */
                 (0b0 << ADC_CR1_JDISCEN_Pos) | /* インジェクションチャンネル不連続モード有効:1 */
                 (0b0 << ADC_CR1_DISCEN_Pos) |/* レギュラチャンネル不連続モード有効:1 */
                 (0b0 << ADC_CR1_JAUTO_Pos) |  /* 自動インジェクトグループ変換有効:1 */
                 (0b0 << ADC_CR1_AWDSGL_Pos) | /* スキャンモードでの単一チャンネルのウォッチドッグ有効:1 */
                 (0b1 << ADC_CR1_SCAN_Pos) | /* スキャンモード有効:1 */
                 (0b0 << ADC_CR1_JEOCIE_Pos) | /* インジェクトチャンネル割り込み有効:1 */
                 (0b0 << ADC_CR1_AWDIE_Pos) | /* アナログウォッチドッグ割り込み有効:1 */
                 (0b0 << ADC_CR1_EOCIE_Pos) | /* 変換終了割り込み有効:1 */
                 (0b00000 << ADC_CR1_AWDCH_Pos) ;/* アナログウォッチドッグ選択 */
                 //(0b1 << ADC_CR2_) | 
    ADC1->CR2 |=  (0b0 << ADC_CR2_SWSTART_Pos) | /* レギュラチャンネル変換開始:1 */
                  (0b00 << ADC_CR2_EXTEN_Pos) | /* レギュラチャンネル外部トリガ有効:1 */
                  (0b0000 << ADC_CR2_EXTSEL_Pos) | /* 外部イベント時のイベント選択 */
                  (0b0 << ADC_CR2_JSWSTART_Pos) | /* インジェクトチャンネル変換有効:1 */
                  (0b00 << ADC_CR2_JEXTEN_Pos) | /* インジェクトチャンネル外部イベント有効：1 */
                  (0b0000 << ADC_CR2_JEXTSEL_Pos) | /* インジェクトグループの外部イベント選択 */
                  (0b0 << ADC_CR2_ALIGN_Pos) | /* データの配置 右詰め:1 */
                  (0b0 << ADC_CR2_EOCS_Pos) | /* 変換選択終了 0:レギュラ変換シーケンス終了時セット 1:レギュラ変換終了時セット */
                  (0b0 << ADC_CR2_DDS_Pos) | /* DMAディセーブル選択 0:転送後DMA終了 1:DMA=1ならばDMAは連続 */
                  (0b0 << ADC_CR2_DMA_Pos) | /* DMA有効:1 */
                  (0b1 << ADC_CR2_CONT_Pos) | /* 連続変換:1 シングル変換:0 */
                  (0b0 << ADC_CR2_ADON_Pos);/* AD有効:1 */
        ADC1->SMPR2 = 0;
        ADC1->SMPR2 = 0;
    ADC1->SMPR1 |= (0b010 << ADC_SMPR1_SMP18_Pos)| (0b010 << ADC_SMPR1_SMP17_Pos)| (0b010 << ADC_SMPR1_SMP16_Pos) | (0b010 << ADC_SMPR1_SMP15_Pos)| (0b010 << ADC_SMPR1_SMP14_Pos) | (0b010 << ADC_SMPR1_SMP13_Pos)| (0b010 << ADC_SMPR1_SMP12_Pos)| (0b111 << ADC_SMPR1_SMP11_Pos) | (0b010 << ADC_SMPR1_SMP10_Pos);
    ADC1->SMPR2 |= (0b010 << ADC_SMPR2_SMP9_Pos) | (0b010 << ADC_SMPR2_SMP8_Pos) | (0b010 << ADC_SMPR2_SMP7_Pos) | (0b010 << ADC_SMPR2_SMP6_Pos) | (0b010 << ADC_SMPR2_SMP5_Pos) | (0b010 << ADC_SMPR2_SMP4_Pos) | (0b010 << ADC_SMPR2_SMP3_Pos) | (0b010 << ADC_SMPR2_SMP2_Pos) | (0b010 << ADC_SMPR2_SMP1_Pos) | (0b010 << ADC_SMPR2_SMP0_Pos);
    //ADC1->SQR1 = (0b0001 << ADC_SQR1_L_Pos) | (0b10001 << ADC_SQR1_SQ16_Pos) | (0b10000 << ADC_SQR1_SQ15_Pos) | (0b01111 << ADC_SQR1_SQ14_Pos) | (0b01110 << ADC_SQR1_SQ13_Pos)/* レギュラ変換シーケンスの合計変換数 17 16 15 14*/; 
    //ADC1->SQR2 = (0b00000 << ADC_SQR2_SQ12_Pos) | (0b01101 << ADC_SQR2_SQ11_Pos) | (0b01100 << ADC_SQR2_SQ10_Pos) | (0b01011 << ADC_SQR2_SQ9_Pos) | (0b01010 << ADC_SQR2_SQ8_Pos) | (0b01001 << ADC_SQR2_SQ7_Pos);
    //ADC1->SQR3 = (0b01000 << ADC_SQR3_SQ6_Pos) | (0b00111 << ADC_SQR3_SQ5_Pos) | (0b00110 << ADC_SQR3_SQ4_Pos) | (0b00011 << ADC_SQR3_SQ3_Pos) | (0b00010 << ADC_SQR3_SQ2_Pos) | (0b00001 << ADC_SQR3_SQ1_Pos);
    // ADC123_COMMON->CCR = 0;
    ADC1->SQR1 = (0b0001 << ADC_SQR1_L_Pos);
    ADC1->SQR2 = 0;//(0b00001 << ADC_SQR2_SQ10_Pos);
    ADC1->SQR3 = (0b01011 << ADC_SQR3_SQ1_Pos);
    ADC123_COMMON->CCR |= (0b0 << ADC_CCR_TSVREFE_Pos) | /* 温度センサイネーブル */
                          (0b0 << ADC_CCR_VBATE_Pos) | /* Vbatイネーブル */
                          (0b10 << ADC_CCR_ADCPRE_Pos) | /* ADCプリスケーラ 00:div2 01:div4 10:div6 11:div8*/
                          (0b00 << ADC_CCR_DMA_Pos) | /* マルチADCモードDMA有効 */
                          (0b0 << ADC_CCR_DDS_Pos) | /* DMAディセーブル選択 0:転送後DMA終了 1:DMA=1ならばDMAは連続 */
                          (0b0000 << ADC_CCR_DELAY_Pos) | /* サンプリングフェーズ間の遅延量 */
                          (0b00000 << ADC_CCR_MULTI_Pos) ; /* マルチADCモード選択 */
    ADC1->CR2 |= (0b1 << ADC_CR2_ADON_Pos);
    ADC1->CR2 |= (0b1 << ADC_CR2_SWSTART_Pos);
    while(1){
        
        delay(4000000);
        //printf("1 %x\n",ADC1->CR1);
        //printf("2 %x\n",ADC1->CR2);
        adc_data = (uint32_t)ADC1->DR;
        printf("Data%x\n",adc_data);
        printf("ste %x\n",ADC1->SR);
        // ADC1->SR &= ~(ADC_SR_EOC);
        //ADC1->SR &= ~(ADC_SR_OVR);
        ADC1->CR2 |= (0b1 << ADC_CR2_SWSTART_Pos);
        //printf("ste2 %x\n",ADC1->SR);


        //printf("E%x\n",ADC123_COMMON->CDR);
        //printf("E%x\n",RCC->APB2ENR);
        //printf("a\n");
        //printf("E %x\n",DMA1->HIFCR);
        //printf("%x\n",USART2->DR);
        //printf("%x\n", USART2->DR);
        //printf("%x\n", USART2->SR);
    };
}
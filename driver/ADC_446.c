//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ADCの設定
// 2019.4
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef ADC_446_C
#define ADC_446_C

#include "ADC_446.h"
#include "printf.h"
//ポーリング 割り込み DMA
//ポーリング:定期的に送信/受信等を行うこと。送信/受信をするタイミングを厳密に決定しなくても良いので、実装が簡単で分岐分も基本的に不要だが、タイミングが重要な処理では使えず、重要度の低い送信/受信をループごとに行わなくてはならないので無駄な処理になる可能性がある。
//割り込み:ペリフェラルの機能やピン変化、タイマ割り込みなど、イベントが発生したタイミングで送信/受信を行う。ポーリングはマイコンの処理がポーリングに達するまで待つ必要があるため、ボタンを押した瞬間に処理したい場合などラグが生じる可能性があるが、割り込みは基本的にイベント発生を検知した直後に発生する。
//多用するほどマイコン全体の処理タイミングが崩れ、割り込み同士が衝突すると優先度の順で処理される。
//DMA ペリフェラルの機能として実装されている場合、ペリフェラルからメモリ/メモリからペリフェラルなどへ受信/送信を自動的に行う。
//割り込みと組み合わせることで

// =====================================================================================================================
//  ADC init
// ADCの設定項目は、1.ADCするピンの決定 2.ピンのサンプリング周波数とAPB2の分周の決定 3.ADCの詳細設定 が必要。
// ADCはADCONのあと、START/JSATARTを1にする。
// ピンはマイコンのデータシートのピンアサイン表に乗っているが、ADC123_IN4みたいな不親切な書き方がしてある。
// ADC123はADCの1、2、3で使用でき、チャンネルが4という意味になる。
// =====================================================================================================================
void ADC1_init(){
	// ADC1,2,3のクロック有効
	RCC->APB2ENR |= (0b1 << RCC_APB2ENR_ADC1EN_Pos) | (0b1 << RCC_APB2ENR_ADC2EN_Pos) | (0b1 << RCC_APB2ENR_ADC3EN_Pos);
    //コントロールレジスタの初期化
	ADC1->CR1 = 0;      
	ADC1->CR2 = 0;
	// CR1の設定
    ADC1->CR1 |= (0b0 << ADC_CR1_OVRIE_Pos) | /* オーバーライン割り込み有効1  */
                 (0b00 << ADC_CR1_RES_Pos) |/* ☆分解能 00:12bit 01:10bit 10:8it 11:6bit */
                 (0b0 << ADC_CR1_AWDEN_Pos) |/* アナログウォッチドッグ有効:1 レギュラ */
                 (0b0 << ADC_CR1_JAWDEN_Pos) | /* アナログウォッチドッグ有効:1 インジェクト */
                 (0b010 << ADC_CR1_DISCNUM_Pos) | /* 不連続モードチャンネル0から8 */
                 (0b0 << ADC_CR1_JDISCEN_Pos) | /* インジェクションチャンネル不連続モード有効:1 */
                 (0b1 << ADC_CR1_DISCEN_Pos) |/* レギュラチャンネル不連続モード有効:1 */
                 (0b0 << ADC_CR1_JAUTO_Pos) |  /* 自動インジェクトグループ変換有効:1 */
                 (0b0 << ADC_CR1_AWDSGL_Pos) | /* スキャンモードでの単一チャンネルのウォッチドッグ有効:1 */
                 (0b0 << ADC_CR1_SCAN_Pos) | /* ☆スキャンモード有効:1 */
                 (0b0 << ADC_CR1_JEOCIE_Pos) | /* インジェクトチャンネル割り込み有効:1 */
                 (0b0 << ADC_CR1_AWDIE_Pos) | /* アナログウォッチドッグ割り込み有効:1 */
                 (0b1 << ADC_CR1_EOCIE_Pos) | /* 変換終了割り込み有効:1 */
                 (0b00000 << ADC_CR1_AWDCH_Pos) ;/* アナログウォッチドッグ選択 */
	// CR2の設定
    ADC1->CR2 |=  (0b0 << ADC_CR2_SWSTART_Pos) | /* レギュラチャンネル変換開始:1 */
                  (0b00 << ADC_CR2_EXTEN_Pos) | /* レギュラチャンネル外部トリガ有効:1 */
                  (0b0000 << ADC_CR2_EXTSEL_Pos) | /* 外部イベント時のイベント選択 */
                  (0b0 << ADC_CR2_JSWSTART_Pos) | /* インジェクトチャンネル変換有効:1 */
                  (0b00 << ADC_CR2_JEXTEN_Pos) | /* インジェクトチャンネル外部イベント有効：1 */
                  (0b0000 << ADC_CR2_JEXTSEL_Pos) | /* インジェクトグループの外部イベント選択 */
                  (0b0 << ADC_CR2_ALIGN_Pos) | /* データの配置 右詰め:1 */
                  (0b1 << ADC_CR2_EOCS_Pos) | /* 変換選択終了 0:レギュラ変換シーケンス終了時セット 1:レギュラ変換終了時セット */
                  (0b0 << ADC_CR2_DDS_Pos) | /* DMAディセーブル選択 0:転送後DMA終了 1:DMA=1ならばDMAは連続 */
                  (0b0 << ADC_CR2_DMA_Pos) | /* DMA有効:1 */
                  (0b0 << ADC_CR2_CONT_Pos) | /* ☆連続変換:1 シングル変換:0 */
                  (0b0 << ADC_CR2_ADON_Pos);/* AD有効:1 */
	// ADC全体のコントロールレジスタ
    ADC123_COMMON->CCR |= (0b0 << ADC_CCR_TSVREFE_Pos) | /* 温度センサイネーブル */
                          (0b0 << ADC_CCR_VBATE_Pos) | /* Vbatイネーブル */
                          (0b10 << ADC_CCR_ADCPRE_Pos) | /* ☆ADCプリスケーラ 00:div2 01:div4 10:div6 11:div8*/
                          (0b00 << ADC_CCR_DMA_Pos) | /* マルチADCモードDMA有効 */
                          (0b0 << ADC_CCR_DDS_Pos) | /* DMAディセーブル選択 0:転送後DMA終了 1:DMA=1ならばDMAは連続 */
                          (0b0000 << ADC_CCR_DELAY_Pos) | /* サンプリングフェーズ間の遅延量 */
                          (0b00000 << ADC_CCR_MULTI_Pos) ; /* マルチADCモード選択 */

	// Ch 0-18 のサンプリング周波数
    ADC1->SMPR1 = 0;
    ADC1->SMPR2 = 0;
    // 10-18
	ADC1->SMPR1 |= (0b010 << ADC_SMPR1_SMP18_Pos)| (0b010 << ADC_SMPR1_SMP17_Pos)| (0b010 << ADC_SMPR1_SMP16_Pos) | (0b010 << ADC_SMPR1_SMP15_Pos)| (0b010 << ADC_SMPR1_SMP14_Pos) | (0b010 << ADC_SMPR1_SMP13_Pos)| (0b010 << ADC_SMPR1_SMP12_Pos)| (0b010 << ADC_SMPR1_SMP11_Pos) | (0b010 << ADC_SMPR1_SMP10_Pos);
    // 0-9
	ADC1->SMPR2 |= (0b010 << ADC_SMPR2_SMP9_Pos) | (0b010 << ADC_SMPR2_SMP8_Pos) | (0b010 << ADC_SMPR2_SMP7_Pos) | (0b010 << ADC_SMPR2_SMP6_Pos) | (0b010 << ADC_SMPR2_SMP5_Pos) | (0b010 << ADC_SMPR2_SMP4_Pos) | (0b010 << ADC_SMPR2_SMP3_Pos) | (0b010 << ADC_SMPR2_SMP2_Pos) | (0b010 << ADC_SMPR2_SMP1_Pos) | (0b010 << ADC_SMPR2_SMP0_Pos);
    //ADC1->SQR1 = (0b0001 << ADC_SQR1_L_Pos) | (0b10001 << ADC_SQR1_SQ16_Pos) | (0b10000 << ADC_SQR1_SQ15_Pos) | (0b01111 << ADC_SQR1_SQ14_Pos) | (0b01110 << ADC_SQR1_SQ13_Pos)/* レギュラ変換シーケンスの合計変換数 17 16 15 14*/; 
    ADC1->SQR2 = (15 << ADC_SQR2_SQ8_Pos) | (7 << ADC_SQR2_SQ7_Pos);//(0b00000 << ADC_SQR2_SQ12_Pos) | (0b01101 << ADC_SQR2_SQ11_Pos) | (0b01100 << ADC_SQR2_SQ10_Pos) | (0b01011 << ADC_SQR2_SQ9_Pos) | (15 << ADC_SQR2_SQ8_Pos) | (7 << ADC_SQR2_SQ7_Pos);
    ADC1->SQR3 = (13 << ADC_SQR3_SQ6_Pos) | (10 << ADC_SQR3_SQ5_Pos) | (0 << ADC_SQR3_SQ4_Pos) | (1 << ADC_SQR3_SQ3_Pos) | (8 << ADC_SQR3_SQ2_Pos) | (11 << ADC_SQR3_SQ1_Pos);
    // ADC123_COMMON->CCR = 0;
	// SQRレジスタ:レギュラ変換の変換対象CHの設定
	// SQR1_L 1-16の順序設定のうち、何番目まで変換するか
    ADC1->SQR1 = 0;
    ADC1->SQR1 = (2 << ADC_SQR1_L_Pos);
	// ADを有効
    ADC1->CR2 |= (0b1 << ADC_CR2_ADON_Pos);
	// 変換スタート (必ずCR2->ADON=1のあと！)
    ADC1->CR2 |= (0b1 << ADC_CR2_SWSTART_Pos);
}

void ADC1_init_2(){
	// ADC1,2,3のクロック有効
	RCC->APB2ENR |= (0b1 << RCC_APB2ENR_ADC1EN_Pos) | (0b1 << RCC_APB2ENR_ADC2EN_Pos) | (0b1 << RCC_APB2ENR_ADC3EN_Pos);
    //コントロールレジスタの初期化
	ADC1->CR1 = 0;      
	ADC1->CR2 = 0;
	// CR1の設定
    ADC1->CR1 |= (0b0 << ADC_CR1_OVRIE_Pos) | /* オーバーライン割り込み有効1  */
                 (0b00 << ADC_CR1_RES_Pos) |/* ☆分解能 00:12bit 01:10bit 10:8it 11:6bit */
                 (0b0 << ADC_CR1_AWDEN_Pos) |/* アナログウォッチドッグ有効:1 レギュラ */
                 (0b0 << ADC_CR1_JAWDEN_Pos) | /* アナログウォッチドッグ有効:1 インジェクト */
                 (0b001 << ADC_CR1_DISCNUM_Pos) | /* 不連続モードチャンネル0から8 */
                 (0b0 << ADC_CR1_JDISCEN_Pos) | /* インジェクションチャンネル不連続モード有効:1 */
                 (0b0 << ADC_CR1_DISCEN_Pos) |/* レギュラチャンネル不連続モード有効:1 */
                 (0b0 << ADC_CR1_JAUTO_Pos) |  /* 自動インジェクトグループ変換有効:1 */
                 (0b0 << ADC_CR1_AWDSGL_Pos) | /* スキャンモードでの単一チャンネルのウォッチドッグ有効:1 */
                 (0b0 << ADC_CR1_SCAN_Pos) | /* ☆スキャンモード有効:1 */
                 (0b0 << ADC_CR1_JEOCIE_Pos) | /* インジェクトチャンネル割り込み有効:1 */
                 (0b0 << ADC_CR1_AWDIE_Pos) | /* アナログウォッチドッグ割り込み有効:1 */
                 (0b0 << ADC_CR1_EOCIE_Pos) | /* 変換終了割り込み有効:1 */
                 (0b00000 << ADC_CR1_AWDCH_Pos) ;/* アナログウォッチドッグ選択 */
	// CR2の設定
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
                  (0b1 << ADC_CR2_CONT_Pos) | /* ☆連続変換:1 シングル変換:0 */
                  (0b0 << ADC_CR2_ADON_Pos);/* AD有効:1 */
	// ADC全体のコントロールレジスタ
    ADC123_COMMON->CCR |= (0b0 << ADC_CCR_TSVREFE_Pos) | /* 温度センサイネーブル */
                          (0b0 << ADC_CCR_VBATE_Pos) | /* Vbatイネーブル */
                          (0b10 << ADC_CCR_ADCPRE_Pos) | /* ☆ADCプリスケーラ 00:div2 01:div4 10:div6 11:div8*/
                          (0b00 << ADC_CCR_DMA_Pos) | /* マルチADCモードDMA有効 */
                          (0b0 << ADC_CCR_DDS_Pos) | /* DMAディセーブル選択 0:転送後DMA終了 1:DMA=1ならばDMAは連続 */
                          (0b0000 << ADC_CCR_DELAY_Pos) | /* サンプリングフェーズ間の遅延量 */
                          (0b00000 << ADC_CCR_MULTI_Pos) ; /* マルチADCモード選択 */

	// Ch 0-18 のサンプリング周波数
    ADC1->SMPR1 = 0;
    ADC1->SMPR2 = 0;
    // 10-18
	ADC1->SMPR1 |= (0b010 << ADC_SMPR1_SMP18_Pos)| (0b010 << ADC_SMPR1_SMP17_Pos)| (0b010 << ADC_SMPR1_SMP16_Pos) | (0b010 << ADC_SMPR1_SMP15_Pos)| (0b010 << ADC_SMPR1_SMP14_Pos) | (0b010 << ADC_SMPR1_SMP13_Pos)| (0b010 << ADC_SMPR1_SMP12_Pos)| (0b010 << ADC_SMPR1_SMP11_Pos) | (0b010 << ADC_SMPR1_SMP10_Pos);
    // 0-9
	ADC1->SMPR2 |= (0b010 << ADC_SMPR2_SMP9_Pos) | (0b010 << ADC_SMPR2_SMP8_Pos) | (0b010 << ADC_SMPR2_SMP7_Pos) | (0b010 << ADC_SMPR2_SMP6_Pos) | (0b010 << ADC_SMPR2_SMP5_Pos) | (0b010 << ADC_SMPR2_SMP4_Pos) | (0b010 << ADC_SMPR2_SMP3_Pos) | (0b010 << ADC_SMPR2_SMP2_Pos) | (0b010 << ADC_SMPR2_SMP1_Pos) | (0b010 << ADC_SMPR2_SMP0_Pos);
    // SQR
    ADC1->SQR1 = (2 << ADC_SQR1_L_Pos);
    ADC1->SQR2 = 0;
    ADC1->SQR3 = 0;
	// ADを有効
    ADC1->CR2 |= (0b1 << ADC_CR2_ADON_Pos);
}

inline void ADC1_start(){
    ADC1->CR2 |= (0b1 << ADC_CR2_SWSTART_Pos);
}

inline void ADC1_stop(){
    ADC1->CR2 &= ~(0b1 << ADC_CR2_SWSTART_Pos);
}

inline uint32_t ADC1_status() {
    return ADC1->SR;
}

uint32_t ADC1_read(uint32_t sqr_ch){
    ADC1->SQR3 = sqr_ch;
    ADC1_start();
    return ADC1->DR;
}

#endif
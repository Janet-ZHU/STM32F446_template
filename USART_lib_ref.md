# UART ライブラリの翻訳

1.  UART_HandleTypeDefハンドル構造体を宣言します｡
2.  HAL_UART_MspInit()APIを実装して､UARTの低レベルのリソースを初期化します｡
    1. URATxインターフェイスクロックを有効にします｡
    2. これらのURATピンをオルタネイトファンクションプルアップとして設定します｡
    3. NVICで割り込み処理HAL_UART_Transmit_IT() または HAL_UART_Receive_IT() APIを使用する必要がある場合は以下｡
        1. UARTxの割り込み優先順位を設定します｡
        2. NVIC UART IRQハンドルを有効にします｡
    4. DMA HAL_UART_Receive_DMA()およびHAL_UART_Receive_DMA（) を使用する場合は以下｡
        1. Tx,RxストリームのDMAハンドル構造を宣言します｡
        2.  DMAxインターフェースクロックを有効にします｡
        3.  宣言されたDMAハンドル構造を必要なTxRxパラメータを設定します｡
        4.  DMA Tx/Rxストリームを設定します｡
        5.  初期化されたDMAハンドルをUART DMA Tx/Rxハンドルに関連付けます｡
        6.  優先度を設定し､転送完了のNVICをDMA Tx/Rxストリーム上で有効にする｡
3. ボーレート、ワード長、ストップビット、パリティ、ハードウェアフロー制御、モード（Tx/Rx）をプログラムする｡
4. UART 非同期モードの場合は､HAL_UART_Init（）APIを設定｡
5. UART半二重モードの場合は、HAL_HalfDuplex_Init（）APIを設定｡
6.  LINモードでは､HAL_LIN_init()APIを呼び出してUARTレジスタを初期化します｡
7.  マルチプロセッサモードでは､UARRTレジスタを呼び出すHAL_MultiProcessor_Init()APIを設定｡

- 特定のUART割り込み(送信完了割り込み､RXNE割り込み､エラー割り込み)は､送受信プロセスの中で__HAL_UART_ENABLE_IT() と __HAL_UART_DISABLE_IT() を使用して管理されます｡
- これらのAPI（HAL_UART_Init（）およびHAL_HalfDuplex_Init（））は、カスタマイズされたHAL_UART_MspInit（）APIを呼び出すことによって、低レベルのハードウェアGPIO、CLOCK、CORTEXなどを構成します。

## このドライバでは､次の3つの動作モードがあります｡

### ポーリングモードIO
1. HAL_UART_Transmit（）を使用してブロックモードで一定量のデータを送信する｡
2. HAL_UART_Receive（）を使用してブロックモードで一定量のデータを受信する｡

### 割り込みモード

1. HAL_UART_Transmit_IT（）を使用して非ブロッキングモードで一定量のデータを送信する｡
2. 転送の送信終了時にHAL_UART_TxCpltCallbackが実行され、ユーザは関数ポインタHAL_UART_TxCpltCallbackをカスタマイズして独自のコードを追加する｡
3. HAL_UART_Receive_IT（）を使用して非ブロッキングモードで一定量のデータを受信する｡
4. 転送の受信終了時に、HAL_UART_RxCpltCallbackが実行され、ユーザは関数ポインタHAL_UART_RxCpltCallbackをカスタマイズして独自のコードを追加する｡
5. 転送エラーの場合、HAL_UART_ErrorCallback（）関数が実行され、ユーザは関数ポインタHAL_UART_ErrorCallbackをカスタマイズして独自のコードを追加する｡

### DMAモード

1. HAL_UART_Transmit_DMA（）を使用して非ブロッキングモード（DMA）でデータ量を送信する｡
2. ハーフ転送の送信終了時HAL_UART_TxHalfCpltCallbackが実行され、関数ポインタHAL_UART_TxHalfCpltCallbackのカスタマイズによって自分のコードを追加する｡
3. 転送の送信終了時にHAL_UART_TxCpltCallbackが実行され、ユーザは関数ポインタHAL_UART_TxCpltCallbackをカスタマイズして独自のコードを追加する｡
4. HAL_UART_Receive_DMA（）を使用して非ブロッキングモード（DMA）でデータ量を受信する｡
5. 半転送の受信終了時に、HAL_UART_RxHalfCpltCallbackが実行され、ユーザは関数ポインタHAL_UART_RxHalfCpltCallbackのカスタマイズによって自分のコードを追加する｡
6. 転送の受信終了時に、HAL_UART_RxCpltCallbackが実行され、ユーザは関数ポインタHAL_UART_RxCpltCallbackをカスタマイズして独自のコードを追加する｡
7. 転送エラーの場合、HAL_UART_ErrorCallback（）関数が実行され、ユーザは関数ポインタHAL_UART_ErrorCallbackをカスタマイズして独自のコードを追加する｡
    - HAL_UART_DMAPause（）を使用してDMA転送を一時停止する
    - HAL_UART_DMAResume（）を使用してDMA転送を再開する
    - HAL_UART_DMAStop（）を使用してDMA転送を停止する

### UART HAL ドライバでよく使用する関数一覧
       
- __HAL_UART_ENABLE：UART周辺機器を有効にする｡
- __HAL_UART_DISABLE：UART周辺機器を無効にする｡
- __HAL_UART_GET_FLAG：指定されたUARTフラグがセットされているかどうかをチェックする｡
- __HAL_UART_CLEAR_FLAG：指定されたUARTペンディングフラグをクリアする｡
- __HAL_UART_ENABLE_IT：指定されたUART割り込みを有効にします｡
- __HAL_UART_DISABLE_IT：指定されたUART割り込みを無効にする｡
- __HAL_UART_GET_IT_SOURCE：指定されたUART割り込みが発生しているかどうかを確認する｡ 

（@）より有用なマクロについては、UART HALドライバのヘッダファイルを参照できます｡

```c
/**
  ******************************************************************************
  * @file    stm32f4xx_hal_uart.c
  * @author  MCD Application Team
  * @brief   UART HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Universal Asynchronous Receiver Transmitter (UART) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions  
  *           + Peripheral State and Errors functions  
  *           
  @verbatim       
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    The UART HAL driver can be used as follows:
    
    (#) Declare a UART_HandleTypeDef handle structure.
  
    (#) Initialize the UART low level resources by implementing the HAL_UART_MspInit() API:
        (##) Enable the USARTx interface clock.
        (##) UART pins configuration:
            (+++) Enable the clock for the UART GPIOs.
            (+++) Configure these UART pins as alternate function pull-up.
        (##) NVIC configuration if you need to use interrupt process (HAL_UART_Transmit_IT()
             and HAL_UART_Receive_IT() APIs):
            (+++) Configure the USARTx interrupt priority.
            (+++) Enable the NVIC USART IRQ handle.
        (##) DMA Configuration if you need to use DMA process (HAL_UART_Transmit_DMA()
             and HAL_UART_Receive_DMA() APIs):
            (+++) Declare a DMA handle structure for the Tx/Rx stream.
            (+++) Enable the DMAx interface clock.
            (+++) Configure the declared DMA handle structure with the required 
                  Tx/Rx parameters.                
            (+++) Configure the DMA Tx/Rx Stream.
            (+++) Associate the initialized DMA handle to the UART DMA Tx/Rx handle.
            (+++) Configure the priority and enable the NVIC for the transfer complete 
                  interrupt on the DMA Tx/Rx Stream.

    (#) Program the Baud Rate, Word Length, Stop Bit, Parity, Hardware 
        flow control and Mode(Receiver/Transmitter) in the Init structure.

    (#) For the UART asynchronous mode, initialize the UART registers by calling
        the HAL_UART_Init() API.
    
    (#) For the UART Half duplex mode, initialize the UART registers by calling 
        the HAL_HalfDuplex_Init() API.
    
    (#) For the LIN mode, initialize the UART registers by calling the HAL_LIN_Init() API.
    
    (#) For the Multi-Processor mode, initialize the UART registers by calling 
        the HAL_MultiProcessor_Init() API.
        
     [..] 
       (@) The specific UART interrupts (Transmission complete interrupt, 
            RXNE interrupt and Error Interrupts) will be managed using the macros
            __HAL_UART_ENABLE_IT() and __HAL_UART_DISABLE_IT() inside the transmit 
            and receive process.
          
     [..] 
       (@) These APIs (HAL_UART_Init() and HAL_HalfDuplex_Init()) configure also the 
            low level Hardware GPIO, CLOCK, CORTEX...etc) by calling the customized 
            HAL_UART_MspInit() API.
          
     [..] 
        Three operation modes are available within this driver :     
  
     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Send an amount of data in blocking mode using HAL_UART_Transmit() 
       (+) Receive an amount of data in blocking mode using HAL_UART_Receive()
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Send an amount of data in non blocking mode using HAL_UART_Transmit_IT() 
       (+) At transmission end of transfer HAL_UART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode using HAL_UART_Receive_IT() 
       (+) At reception end of transfer HAL_UART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxCpltCallback
       (+) In case of transfer Error, HAL_UART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_UART_ErrorCallback

     *** DMA mode IO operation ***    
     ==============================
     [..] 
       (+) Send an amount of data in non blocking mode (DMA) using HAL_UART_Transmit_DMA() 
       (+) At transmission end of half transfer HAL_UART_TxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxHalfCpltCallback 
       (+) At transmission end of transfer HAL_UART_TxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_TxCpltCallback
       (+) Receive an amount of data in non blocking mode (DMA) using HAL_UART_Receive_DMA() 
       (+) At reception end of half transfer HAL_UART_RxHalfCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxHalfCpltCallback 
       (+) At reception end of transfer HAL_UART_RxCpltCallback is executed and user can 
            add his own code by customization of function pointer HAL_UART_RxCpltCallback
       (+) In case of transfer Error, HAL_UART_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_UART_ErrorCallback
       (+) Pause the DMA Transfer using HAL_UART_DMAPause()      
       (+) Resume the DMA Transfer using HAL_UART_DMAResume()  
       (+) Stop the DMA Transfer using HAL_UART_DMAStop()      
    
     *** UART HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in UART HAL driver.
       
      (+) __HAL_UART_ENABLE: Enable the UART peripheral 
      (+) __HAL_UART_DISABLE: Disable the UART peripheral     
      (+) __HAL_UART_GET_FLAG : Check whether the specified UART flag is set or not
      (+) __HAL_UART_CLEAR_FLAG : Clear the specified UART pending flag
      (+) __HAL_UART_ENABLE_IT: Enable the specified UART interrupt
      (+) __HAL_UART_DISABLE_IT: Disable the specified UART interrupt
      (+) __HAL_UART_GET_IT_SOURCE: Check whether the specified UART interrupt has occurred or not
      
     [..] 
       (@) You can refer to the UART HAL driver header file for more useful macros 
      
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
```
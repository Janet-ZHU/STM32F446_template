//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//GPIOの設定
// 2019.2.3 sakai created.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef GPIO_446_C
#define GPIO_446_C

#include "GPIO_446.h"

// =====================================================================================================================
// GPIO init
// =====================================================================================================================
void GPIO_init(){

    /* GPIOA Periph clock enable */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /*!< 0x00000001 */

    /* Configure PA5 in output  mode  */
    GPIOA->MODER |= (GPIO_MODER_MODER5_0) ;/*!< 0x00000400 */

    // Ensure push pull mode selected--default
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5) ; /*!< 0x00000020 */
    
	//Ensure all pull up pull down resistors are disabled
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5);/*!< 0x00000C00 */

    // PA8 MCO1 AF1 setting --------------------------------------------
    GPIOA->MODER |= 0b10 << GPIO_MODER_MODE8_Pos;
    GPIOA->AFR[1] |= 0x0;
    //example, not run
    //RCC->CFGR |= 0b00 << RCC_CFGR_MCO1_Pos;
}
#endif
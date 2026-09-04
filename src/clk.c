#include "clk.h"
#include "flash.h"
void RCC_AHBCLK_INIT(void)
{
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
    FLASH->ACR |= FLASH_ACR_LATENCY_5T;
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_MASK;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_X8;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_MASK;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_X2;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_MASK;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_X336;
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |=  RCC_CFGR_HPRE_X1;  
}

void RCC_APB1CLK_INIT(void)
{
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_X4;
}

void RCC_APB2CLK_INIT(void)
{
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    RCC->CFGR |= RCC_CFGR_PPRE2_X2;
}

void systeminit(void)
{
    RCC_AHBCLK_INIT();                    // 1. PLL 就绪

     // 2. AHB 分频
    RCC_APB1CLK_INIT();                   //    APB1 分频
    RCC_APB2CLK_INIT();                   //    APB2 分频

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_PLL;        // 3. 切到 PLL
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);   // 4. 等切换完成
}

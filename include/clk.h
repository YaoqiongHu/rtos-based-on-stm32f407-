#ifndef CLK_H_
#define CLK_H_ 

#include "int.h"

/* ========== RCC 结构体 ========== */
typedef struct {
    __IO uint32 CR;            /* 0x00 时钟控制寄存器 */
    __IO uint32 PLLCFGR;       /* 0x04 PLL 配置寄存器 */
    __IO uint32 CFGR;          /* 0x08 时钟配置寄存器 */
    __IO uint32 CIR;           /* 0x0C 时钟中断寄存器 */
    __IO uint32 AHB1RSTR;      /* 0x10 AHB1 外设复位寄存器 */
    __IO uint32 AHB2RSTR;      /* 0x14 AHB2 外设复位寄存器 */
    __IO uint32 AHB3RSTR;      /* 0x18 AHB3 外设复位寄存器 */
    uint32      RESERVED0;     /* 0x1C 保留 */
    __IO uint32 APB1RSTR;      /* 0x20 APB1 外设复位寄存器 */
    __IO uint32 APB2RSTR;      /* 0x24 APB2 外设复位寄存器 */
    uint32      RESERVED1[2];  /* 0x28-0x2C 保留 */
    __IO uint32 AHB1ENR;       /* 0x30 AHB1 外设时钟使能寄存器 */
    __IO uint32 AHB2ENR;       /* 0x34 AHB2 外设时钟使能寄存器 */
    __IO uint32 AHB3ENR;       /* 0x38 AHB3 外设时钟使能寄存器 */
    uint32      RESERVED2;     /* 0x3C 保留 */
    __IO uint32 APB1ENR;       /* 0x40 APB1 外设时钟使能寄存器 */
    __IO uint32 APB2ENR;       /* 0x44 APB2 外设时钟使能寄存器 */
    uint32      RESERVED3[2];  /* 0x48-0x4C 保留 */
    __IO uint32 AHB1LPENR;     /* 0x50 AHB1 睡眠模式时钟使能寄存器 */
    __IO uint32 AHB2LPENR;     /* 0x54 AHB2 睡眠模式时钟使能寄存器 */
    __IO uint32 AHB3LPENR;     /* 0x58 AHB3 睡眠模式时钟使能寄存器 */
    uint32      RESERVED4;     /* 0x5C 保留 */
    __IO uint32 APB1LPENR;     /* 0x60 APB1 睡眠模式时钟使能寄存器 */
    __IO uint32 APB2LPENR;     /* 0x64 APB2 睡眠模式时钟使能寄存器 */
    uint32      RESERVED5[2];  /* 0x68-0x6C 保留 */
    __IO uint32 BDCR;          /* 0x70 备份域控制寄存器 */
    __IO uint32 CSR;           /* 0x74 时钟控制/状态寄存器 */
    uint32      RESERVED6[2];  /* 0x78-0x7C 保留 */
    __IO uint32 SSCGR;         /* 0x80 展频时钟生成寄存器 */
    __IO uint32 PLLI2SCFGR;    /* 0x84 PLLI2S 配置寄存器 */
} RCC_TypeDef;

#define RCC_BASE                0x40023800UL
#define RCC                     ((RCC_TypeDef *)RCC_BASE)



//rcc_cr位定义
#define RCC_CR_PLLRDY   (1UL << 25)     // PLL 锁定
#define RCC_CR_PLLON    (1UL << 24)     // PLL 使能
#define RCC_CR_HSERDY   (1UL << 17)     // HSE 锁定
#define RCC_CR_HSEON    (1UL << 16)     // HSE 使能
#define RCC_CR_HSIRDY   (1UL << 1)      // HSI 锁定
#define RCC_CR_HSION    (1UL << 0)      // HSI 使能



//rcc_pllcfgr
#define RCC_PLLCFGR_PLLM_MASK  (0x3f << 0)
#define RCC_PLLCFGR_PLLM_X8    (0x8 << 0)
#define RCC_PLLCFGR_PLLP_MASK  (0x3F << 16)
#define RCC_PLLCFGR_PLLP_X2    (0x0 << 16)
#define RCC_PLLCFGR_PLLN_MASK   (0x1ff << 6)
#define RCC_PLLCFGR_PLLN_X336    (0x150 << 6)
#define RCC_PLLCFGR_PLLSRC    (0x1 << 22)


//rcc_cfgr位定义
#define RCC_CFGR_MCO2   (0x3 << 30)     // MCO2 选择
#define RCC_CFGR_MCO2_sysclkOUT    (0x0 << 30)     // 系统时钟输出
#define RCC_CFGR_MCO1   (0x3 << 21)             // MCO1 选择
#define RCC_CFGR_MCO1_HSEOUT    (0x2 << 21)  // HSE输出

#define RCC_CFGR_PPRE2_Pos  13          // APB2 预分频
#define RCC_CFGR_PPRE2  (0x7UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_X1      (0x0UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_X2       (0x4UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_X4       (0x5UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_X8       (0x6UL << RCC_CFGR_PPRE2_Pos)
#define RCC_CFGR_PPRE2_X16     (0x7UL << RCC_CFGR_PPRE2_Pos)

#define RCC_CFGR_PPRE1_Pos  10          //APB1预分频
#define RCC_CFGR_PPRE1  (0x7UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_X1      (0x0UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_X2       (0x4UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_X4       (0x5UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_X8       (0x6UL << RCC_CFGR_PPRE1_Pos)
#define RCC_CFGR_PPRE1_X16     (0x7UL << RCC_CFGR_PPRE1_Pos)

#define RCC_CFGR_HPRE_Pos  4            // HCLK 预分频（AHB）
#define RCC_CFGR_HPRE  (0xFUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X1      (0x0UL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X2       (0x8UL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X4       (0x9UL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_8       (0xaUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X16     (0xbUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X64     (0xcUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_128    (0xdUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X256    (0xeUL << RCC_CFGR_HPRE_Pos)
#define RCC_CFGR_HPRE_X512    (0xfUL << RCC_CFGR_HPRE_Pos)

#define RCC_CFGR_SWS_Pos  2         // 系统时钟选择标志
#define RCC_CFGR_SWS  (0x3UL << RCC_CFGR_SWS_Pos)

#define RCC_CFGR_SWS_HSE  (0x1UL << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SWS_PLL  (0x2ul << RCC_CFGR_SWS_Pos)
#define RCC_CFGR_SW_Pos  0    // 系统时钟选择
#define RCC_CFGR_SW  (0x3UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSI  (0x0UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_HSE  (0x1UL << RCC_CFGR_SW_Pos)
#define RCC_CFGR_SW_PLL  (0x2UL << RCC_CFGR_SW_Pos)

//RCC_AHB1RSTR位定义
#define RCC_AHB1RSTR_GPIOIRST  (1UL << 8)  // GPIOH复位
#define RCC_AHB1RSTR_GPIOHRST  (1UL << 7)
#define RCC_AHB1RSTR_GPIOFRST  (1UL << 6)
#define RCC_AHB1RSTR_GPIOGRST  (1UL << 5)
#define RCC_AHB1RSTR_GPIOERST  (1UL << 4)
#define RCC_AHB1RSTR_GPIODRST  (1UL << 3)
#define RCC_AHB1RSTR_GPIOCRST  (1UL << 2)
#define RCC_AHB1RSTR_GPIOBRST  (1UL << 1)
#define RCC_AHB1RSTR_GPIOARST  (1UL << 0)

//RCC_AHB1ENR位定义
#define RCC_AHB1ENR_GPIOIEN  (1UL << 8)      //GPIOI时钟使能位
#define RCC_AHB1ENR_GPIOHEN  (1UL << 7)
#define RCC_AHB1ENR_GPIOFEN  (1UL << 6)
#define RCC_AHB1ENR_GPIOGEN  (1UL << 5)
#define RCC_AHB1ENR_GPIOEEN  (1UL << 4)
#define RCC_AHB1ENR_GPIODEN  (1UL << 3)
#define RCC_AHB1ENR_GPIOCEN  (1UL << 2)
#define RCC_AHB1ENR_GPIOBEN  (1UL << 1)
#define RCC_AHB1ENR_GPIOAEN  (1UL << 0)


void RCC_AHBCLK_INIT(void);
void RCC_APB1CLK_INIT(void);
void RCC_APB2CLK_INIT(void);

#define RCC_GPIOA_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define RCC_GPIOB_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define RCC_GPIOC_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define RCC_GPIOD_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define RCC_GPIOE_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN)
#define RCC_GPIOF_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN)
#define RCC_GPIOG_CLK_ENABLE()  (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN)


#endif
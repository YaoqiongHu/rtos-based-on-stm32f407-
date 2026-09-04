#ifndef __GPIO_H__
#define __GPIO_H__

#include "int.h"   


typedef struct {
    __IO uint32 MODER;    /* 0x00 模式寄存器 */
    __IO uint32 OTYPER;   /* 0x04 输出类型寄存器 */
    __IO uint32 OSPEEDR;  /* 0x08 输出速度寄存器 */
    __IO uint32 PUPDR;    /* 0x0C 上拉/下拉寄存器 */
    __IO uint32 IDR;      /* 0x10 输入数据寄存器 */
    __IO uint32 ODR;      /* 0x14 输出数据寄存器 */
    __IO uint32 BSRR;     /* 0x18 置位/复位寄存器 */
    __IO uint32 LCKR;      /* 0x1C 锁定寄存器 */
    __IO uint32 AFR[2];   /* 0x20 复用功能低位, 0x24 复用功能高位 */
} GPIO_TypeDef;


#define GPIOA_BASE              0x40020000UL
#define GPIOB_BASE              0x40020400UL
#define GPIOC_BASE              0x40020800UL
#define GPIOD_BASE              0x40020C00UL
#define GPIOE_BASE              0x40021000UL
#define GPIOF_BASE              0x40021400UL
#define GPIOG_BASE              0x40021800UL
#define GPIOH_BASE              0x40021C00UL
#define GPIOI_BASE              0x40022000UL

#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD                   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE                   ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF                   ((GPIO_TypeDef *)GPIOF_BASE)
#define GPIOG                   ((GPIO_TypeDef *)GPIOG_BASE)
#define GPIOH                   ((GPIO_TypeDef *)GPIOH_BASE)
#define GPIOI                   ((GPIO_TypeDef *)GPIOI_BASE)

/* ========== GPIO_MODER 位定义 ========== */
#define GPIO_MODER_Msk              (0x3UL)
#define GPIO_MODER_IN               (0x0UL)     /* 输入模式 */
#define GPIO_MODER_OUT              (0x1UL)     /* 通用输出模式 */
#define GPIO_MODER_AF               (0x2UL)     /* 复用功能模式 */
#define GPIO_MODER_ANALOG           (0x3UL)     /* 模拟模式 */

/* ========== GPIO_OTYPER 位定义 ========== */
#define GPIO_OTYPER_PP              (0x0UL)     /* 推挽输出 */
#define GPIO_OTYPER_OD              (0x1UL)     /* 开漏输出 */

/* ========== GPIO_OSPEEDR 位定义 ========== */
#define GPIO_OSPEEDR_Msk            (0x3UL)
#define GPIO_OSPEEDR_LOW            (0x0UL)     /* 2 MHz 低速 */
#define GPIO_OSPEEDR_MEDIUM         (0x1UL)     /* 25 MHz 中速 */
#define GPIO_OSPEEDR_HIGH           (0x2UL)     /* 50 MHz 快速 */
#define GPIO_OSPEEDR_FAST           (0x3UL)     /* 100 MHz 高速 */

/* ========== GPIO_PUPDR 位定义 ========== */
#define GPIO_PUPDR_Msk              (0x3UL)
#define GPIO_PUPDR_NONE             (0x0UL)     /* 无上下拉 */
#define GPIO_PUPDR_PU               (0x1UL)     /* 上拉 */
#define GPIO_PUPDR_PD               (0x2UL)     /* 下拉 */

/* ========== GPIO_PIN 位定义 ========== */
#define GPIO_PIN_0                  (0x0001U)   /* Pin 0  */
#define GPIO_PIN_1                  (0x0002U)   /* Pin 1  */
#define GPIO_PIN_2                  (0x0004U)   /* Pin 2  */
#define GPIO_PIN_3                  (0x0008U)   /* Pin 3  */
#define GPIO_PIN_4                  (0x0010U)   /* Pin 4  */
#define GPIO_PIN_5                  (0x0020U)   /* Pin 5  */
#define GPIO_PIN_6                  (0x0040U)   /* Pin 6  */
#define GPIO_PIN_7                  (0x0080U)   /* Pin 7  */
#define GPIO_PIN_8                  (0x0100U)   /* Pin 8  */
#define GPIO_PIN_9                  (0x0200U)   /* Pin 9  */
#define GPIO_PIN_10                 (0x0400U)   /* Pin 10 */
#define GPIO_PIN_11                 (0x0800U)   /* Pin 11 */
#define GPIO_PIN_12                 (0x1000U)   /* Pin 12 */
#define GPIO_PIN_13                 (0x2000U)   /* Pin 13 */
#define GPIO_PIN_14                 (0x4000U)   /* Pin 14 */
#define GPIO_PIN_15                 (0x8000U)   /* Pin 15 */
#define GPIO_PIN_ALL                (0xFFFFU)   /* 全部引脚 */


#define GPIO_AFRL_AF0               (0x00000000U)   // 复用功能0
#define GPIO_AFRL_AF1               (0x00000001U)
#define GPIO_AFRL_AF2               (0x00000002U)
#define GPIO_AFRL_AF3               (0x00000003U)
#define GPIO_AFRL_AF4               (0x00000004U)
#define GPIO_AFRL_AF5               (0x00000005U)
#define GPIO_AFRL_AF6               (0x00000006U)
#define GPIO_AFRL_AF7               (0x00000007U)
#define GPIO_AFRL_AF8               (0x00000008U)
#define GPIO_AFRL_AF9               (0x00000009U)
#define GPIO_AFRL_AF10              (0x0000000AU)
#define GPIO_AFRL_AF11              (0x0000000BU)
#define GPIO_AFRL_AF12              (0x0000000CU)
#define GPIO_AFRL_AF13              (0x0000000DU)
#define GPIO_AFRL_AF14              (0x0000000EU)
#define GPIO_AFRL_AF15              (0x0000000FU)   // 复用功能15

/* ========== 简易驱动 API ========== */
void gpio_init_out(GPIO_TypeDef* port, uint32 pins); /* 引脚配成推挽高速输出 */
void gpio_write(GPIO_TypeDef* port, uint32 pins, uint32 level); /* 1=置高 0=置低(BSRR 原子) */
void gpio_toggle(GPIO_TypeDef* port, uint32 pins);







#endif

#ifndef CORE_H
#define CORE_H

#include "int.h"

/* ========== SysTick(ARM 内核定时器,地址由架构固定)========== */
typedef struct {
    __IO uint32 CSR;     /* 0x00 控制与状态 */
    __IO uint32 RVR;     /* 0x04 重装载值 */
    __IO uint32 CVR;     /* 0x08 当前值 */
    __IO uint32 CALIB;   /* 0x0C 校准值 */
} SysTick_Type;

#define SYSTICK_BASE  0xE000E010UL
#define SysTick       ((SysTick_Type *)SYSTICK_BASE)

#define SYST_CSR_ENABLE     (1UL << 0)   /* 计数器使能 */
#define SYST_CSR_TICKINT    (1UL << 1)   /* 计数到 0 产生 SysTick 异常 */
#define SYST_CSR_CLKSOURCE  (1UL << 2)   /* 1 = 用处理器时钟(AHB 168MHz) */

/* ========== SCB(系统控制块)========== */
typedef struct {
    __IO uint32 CPUID;   /* 0x00 */
    __IO uint32 ICSR;    /* 0x04 中断控制状态(bit28 = PENDSVSET) */
    __IO uint32 VTOR;    /* 0x08 */
    __IO uint32 AIRCR;   /* 0x0C */
    __IO uint32 SCR;     /* 0x10 */
    __IO uint32 CCR;     /* 0x14 */
    __IO uint32 SHPR1;   /* 0x18 */
    __IO uint32 SHPR2;   /* 0x1C SVC 优先级(byte0) */
    __IO uint32 SHPR3;   /* 0x20 PendSV(byte2)、SysTick(byte3) */
} SCB_Type;

#define SCB_BASE  0xE000ED00UL
#define SCB       ((SCB_Type *)SCB_BASE)

#define SCB_ICSR_PENDSVSET  (1UL << 28)   /* 置 1 触发 PendSV */

#endif

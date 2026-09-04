.syntax unified
.cpu   cortex-m4
.fpu   softvfp
.thumb

// 中断/异常向量表
.section .isr_vector, "a", %progbits
.align  9                              /* 2^9 = 512 字节对齐 */
isr_vector:
    .word  init_sp                       /* 栈顶指针                          */
    .word  Reset_Handler                /* 复位                              */
    .word  NMI_Handler                  /* NMI                               */
    .word  HardFault_Handler            /* 硬错误                            */
    .word  MemManage_Handler            /* MPU 存储管理错误                 */
    .word  BusFault_Handler             /* 总线错误                          */
    .word  UsageFault_Handler           /* 用法错误 (Undef Instruction 等)  */
    .word  0                            /* 保留                              */
    .word  0                            /* 保留                              */
    .word  0                            /* 保留                              */
    .word  0                            /* 保留                              */
    .word  SVC_Handler                  /* SVC (系统调用)                    */
    .word  DebugMon_Handler             /* 调试监视器                        */
    .word  0                            /* 保留                              */
    .word  PendSV_Handler               /* PendSV                            */
    .word  SysTick_Handler              /* SysTick 滴答定时器               */

    .rept 82
    .word Default_Handler
    .endr

__isr_vector_end:

/* ============================================================
 * Reset_Handler：复位后启动流程
 *   1) 将 .data 初值从 flash(__data_load_addr) 拷贝到 SRAM(__start_data)
 *   2) 将 .bss 区(__start_bss ~ __end_bss)清零
 *   3) 跳转 main()，main 返回后死循环
 * 栈指针(init_sp)由硬件在复位时从向量表第 0 项自动加载，无需设置
 * ============================================================ */
.section .text.Reset_Handler, "ax", %progbits
.thumb_func
.globl  Reset_Handler
.type   Reset_Handler, %function
Reset_Handler:
    /* 1. 拷贝 .data：r0=源(flash)，r1=目的(SRAM)，r2=长度 */
    ldr     r0, =__data_load_addr
    ldr     r1, =__start_data
    ldr     r2, =__end_data
    subs    r2, r2, r1                  /* 长度 = __end_data - __start_data */
    beq     .Lbss_init                  /* 长度为 0 则跳过拷贝 */

.Ldata_copy:
    ldr     r3, [r0], #4                /* 从 flash 读一个字，源地址 +4 */
    str     r3, [r1], #4                /* 写入 SRAM，目的地址 +4 */
    subs    r2, r2, #4
    bne     .Ldata_copy

    /* 2. 清零 .bss：r0=起始，r1=长度 */
.Lbss_init:
    ldr     r0, =__start_bss
    ldr     r1, =__end_bss
    subs    r1, r1, r0
    beq     .Lcall_os
    movs    r2, #0

.Lbss_zero:
    str     r2, [r0], #4
    subs    r1, r1, #4
    bne     .Lbss_zero

    /* 3. 进入 C 程序 */
.Lcall_os:
    bl      systeminit
    bl      main

    /* main 返回后不应继续执行，死循环 */
.Lhalt:
    b       .Lhalt

/* ============================================================
 * Default_Handler：未实现中断的默认入口
 * 弱定义：用户可在 C 中定义同名强符号覆盖
 * ============================================================ */
.section .text.Default_Handler, "ax", %progbits
.thumb_func
.weak   Default_Handler
.type   Default_Handler, %function
Default_Handler:
    b       .

/* ============================================================
 * 其余异常处理函数：弱定义，链接期别名指向 Default_Handler
 * 用户若实现同名强符号，向量表自动改用用户版本
 * ============================================================ */
.macro  WEAK_ALIAS name
    .section .text.\name, "ax", %progbits
    .weak   \name
    .thumb_set \name, Default_Handler
.endm

    WEAK_ALIAS NMI_Handler
    WEAK_ALIAS HardFault_Handler
    WEAK_ALIAS MemManage_Handler
    WEAK_ALIAS BusFault_Handler
    WEAK_ALIAS UsageFault_Handler
    WEAK_ALIAS SVC_Handler
    WEAK_ALIAS DebugMon_Handler
    WEAK_ALIAS PendSV_Handler
    WEAK_ALIAS SysTick_Handler

    .end

#include "gpio.h"
#include "int.h"

/* 把 pins(位 0~15)配成推挽高速输出 */
void gpio_init_out(GPIO_TypeDef* port, uint32 pins)
{
    for (uint32 i = 0; i < 16; i++)
    {
        if (pins & (1UL << i))
        {
            port->MODER    &= ~(GPIO_MODER_Msk << (i * 2));
            port->MODER    |=  (GPIO_MODER_OUT << (i * 2));      /* 01 = 通用输出 */
            port->OTYPER   &= ~(1UL << i);                       /* 推挽 */
            port->OSPEEDR  |=  (GPIO_OSPEEDR_FAST << (i * 2));   /* 高速 */
            port->PUPDR    &= ~(GPIO_PUPDR_Msk << (i * 2));      /* 无上下拉 */
        }
    }
}

/* 写电平:BSRR 单次写,原子,不破坏其它引脚 */
void gpio_write(GPIO_TypeDef* port, uint32 pins, uint32 level)
{
    if (level)
        port->BSRR = pins;              /* BS0..15:置位 */
    else
        port->BSRR = (pins << 16);      /* BR0..15:复位 */
}

void gpio_toggle(GPIO_TypeDef* port, uint32 pins)
{
    /* 读 ODR 判断当前电平,再用 BSRR 单写翻转(读与写各自原子,不会丢位) */
    if (port->ODR & pins)
        port->BSRR = (pins << 16);
    else
        port->BSRR = pins;
}

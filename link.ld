MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

init_sp =  0x2001FFFC;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
        __end_isr_vector = .;
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        . = ALIGN(4);
        __end_text = .;
    } > FLASH

    .rodata ALIGN(4) :
    {
        *(.rodata)
        *(.rodata.*)
    } > FLASH

    /* .data 段：VMA 在 SRAM，LMA 在 FLASH（AT> FLASH 保证初值烧进 flash） */
    .data :
    {
        . = ALIGN(4);
        __start_data = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        __end_data = .;
    } > SRAM AT> FLASH

    /* 必须放在 .data 定义之后，LOADADDR 才能取到正确的 flash 地址 */
    __data_load_addr = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        __start_bss = .;
        *(.bss)
        . = ALIGN(4);
        __end_bss = .;
    } > SRAM
}

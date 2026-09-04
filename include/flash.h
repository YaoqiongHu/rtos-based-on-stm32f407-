#ifndef FLASH_H
#define FLASH_H



#include "int.h"
typedef struct flash
{
 
__IO uint32 ACR;
__IO uint32 KEY;
__IO uint32 OPTKEY;
__IO uint32 SR ;
__IO uint32 CR ;

}flashtype;


/*外设flashi*/
#define FLASH_BASE    0x40023C00U
#define FLASH         ((flashtype *)FLASH_BASE)

/*flash_ACR位定义*/


#define FLASH_ACR_LATENCY_Msk           (0x7UL << 0)
#define FLASH_ACR_LATENCY_5T             (0x5UL << 0)    // 5个时钟周期延迟 

/*flash_KEY钥匙*/
#define FLASH_KEY1          0x45670123
#define FLASH_KEY2          0xCDEF89AB   

/*flash_SR位定义*/
#define FLASH_SR_BSY        (1UL << 16) // 忙标志 Busy
#define FLASH_SR_EOP        (1UL << 0)  // 操作完成 End Of Progra

/*flash_CR位定义*/
#define FLASH_CR_PSIZE_MASK     (0x3UL << 8)
#define FLASH_CR_PSIZE_X32     (0x2UL << 8)  // 32位（3.3V常规使用）

#define FLASH_CR_SNB_Pos    3
#define FLASH_CR_SNB_Msk        (0xfUL << FLASH_CR_SNB_Pos)    //选择擦除扇区
#define FLASH_CR_SNB_S5  (0x5ul << FLASH_CR_SNB_Pos)       // 扇区编号
#define FLASH_CR_SNB_S6  (0x6ul << FLASH_CR_SNB_Pos)      
#define FLASH_CR_SNB_S7  (0x7ul << FLASH_CR_SNB_Pos)
#define FLASH_CR_SNB_S8  (0x8ul << FLASH_CR_SNB_Pos)
#define FLASH_CR_SNB_S9  (0x9ul << FLASH_CR_SNB_Pos)
#define FLASH_CR_SNB_S10  (0xaul << FLASH_CR_SNB_Pos)       //扇区编号

#define FLASH_CR_STRT        (1UL << 16) //启动擦除
#define FLASH_CR_SER        (1UL << 1)  // 扇区擦除使能 Sector Erase
#define FLASH_CR_PG         (1UL << 0)  // 编程使能 Program
#define FLASH_CR_LOCK       (1UL << 31) // 上锁位


void flash_unlock(void);
//uint32 FLASH_GetStatus(void);
//void FLASH_ClearFlags(void);
void flash_earse(uint8 sector);
void flash_program(uint32* flashaddr, uint32* ramaddr);
void flash_lock(void);



#endif


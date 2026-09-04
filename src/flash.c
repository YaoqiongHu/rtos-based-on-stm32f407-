#include "flash.h"
#include "int.h"

void flash_unlock(void)
{
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->KEY = FLASH_KEY1;
    FLASH->KEY = FLASH_KEY2;
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
    FLASH->ACR |= FLASH_ACR_LATENCY_5T;

}

void flash_lock(void)
{
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->CR |= FLASH_CR_LOCK;
    
}


void flash_earse(uint8 sector)
{
    switch(sector)
    {
        case 1:
            for(uint8 i = 0; i < 3; i++)
            {
                while(FLASH->SR & FLASH_SR_BSY);
                FLASH->CR &= ~FLASH_CR_SNB_Msk ;
                FLASH->CR |= (FLASH_CR_SNB_S5 + (i << FLASH_CR_SNB_Pos));
                FLASH->CR |= FLASH_CR_SER;
                FLASH->CR |= FLASH_CR_STRT;
            }
            while(FLASH->SR & FLASH_SR_BSY);
            FLASH->CR &= ~FLASH_CR_SER;
            break;
        case 2:
            for(uint8 i = 0; i < 3; i++)
            { 
                while(FLASH->SR & FLASH_SR_BSY);
                FLASH->CR &= ~FLASH_CR_SNB_Msk ;
                FLASH->CR |= (FLASH_CR_SNB_S8 + (i << FLASH_CR_SNB_Pos));
                FLASH->CR |= FLASH_CR_SER;
                FLASH->CR |= FLASH_CR_STRT;
            }
            while(FLASH->SR & FLASH_SR_BSY);
            FLASH->CR &= ~FLASH_CR_SER;
            break;
    }

}

void flash_program(uint32* flashaddr, uint32* ramaddr)
{
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_PSIZE_X32;
    FLASH->CR |= FLASH_CR_PG;
    *flashaddr = *ramaddr;
    while(FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PG;
}


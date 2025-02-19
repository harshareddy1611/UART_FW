#include "stm32f4xx.h"
#include <string.h>

#define FLASH_START_ADDRESS 0x08080000  
#define FLASH_SECTOR        8  
#define FLASH_SECTOR_SIZE   0x4000  

void UART2_Init(void);
void UART2_SendChar(char c);
void Flash_Write(uint32_t address, uint8_t* data, uint32_t length);
void Flash_EraseSector(uint8_t sector);
void Flash_Read(uint32_t address, uint8_t* buffer, uint32_t length);
void UART2_SendString(const char* str);

int main(void) {
    uint8_t data_read[100];  
    UART2_Init();  
    UART2_SendString("Storing data in Flash memory...\n");


    char data[] = "Hello, STM32F407 - Stored in Flash memory!";

 
    Flash_EraseSector(FLASH_SECTOR);
    UART2_SendString("Sector erased.\n");

    Flash_Write(FLASH_START_ADDRESS, (uint8_t*)data, strlen(data));
    UART2_SendString("Data written to Flash.\n");

    UART2_SendString("Reading from Flash...\n");
    Flash_Read(FLASH_START_ADDRESS, data_read, strlen(data)); 

    while (1);
}

void UART2_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);

    USART2->BRR = 6667;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void UART2_SendChar(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void UART2_SendString(const char* str) {
    while (*str) {
        UART2_SendChar(*str);
        str++;
    }
}

void Flash_EraseSector(uint8_t sector) {
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }

    while (FLASH->SR & FLASH_SR_BSY);  

    FLASH->CR &= ~FLASH_CR_SNB;  
    FLASH->CR |= (sector << FLASH_CR_SNB_Pos);  
    FLASH->CR |= FLASH_CR_SER;  
    FLASH->CR |= FLASH_CR_STRT; 

    while (FLASH->SR & FLASH_SR_BSY);  

    FLASH->CR &= ~FLASH_CR_SER;  
    FLASH->SR |= FLASH_SR_EOP;   
    FLASH->CR |= FLASH_CR_LOCK;  


    if (FLASH->SR & FLASH_SR_EOP) {
        UART2_SendString("Erase completed.\n");
    } else {
        UART2_SendString("Erase failed.\n");
    }
}

void Flash_Write(uint32_t address, uint8_t* data, uint32_t length) {
    uint32_t i;
    uint32_t wordData;

    
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }

    while (FLASH->SR & FLASH_SR_BSY);  

    FLASH->CR |= FLASH_CR_PSIZE_1;  
    FLASH->CR |= FLASH_CR_PG; 

    for (i = 0; i < length; i += 4) {
        wordData = 0xFFFFFFFF;  

        
        for (uint8_t j = 0; j < 4 && (i + j) < length; j++) {
            ((uint8_t*)&wordData)[j] = data[i + j];
        }

        *(__IO uint32_t*)address = wordData;

        while (FLASH->SR & FLASH_SR_BSY);  

        address += 4;  
    }

    FLASH->CR &= ~FLASH_CR_PG;  
    FLASH->SR |= FLASH_SR_EOP;   
    FLASH->CR |= FLASH_CR_LOCK;  

   
    if (FLASH->SR & FLASH_SR_EOP) {
        UART2_SendString("Write completed.\n");
    } else {
        UART2_SendString("Write failed.\n");
    }
}

void Flash_Read(uint32_t address, uint8_t* buffer, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        buffer[i] = *(__IO uint8_t*)(address + i);
        UART2_SendChar(buffer[i]);
    }
}

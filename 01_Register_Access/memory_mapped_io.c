/*
 * memory_mapped_io.c
 *
 * Demonstrates memory-mapped I/O concepts in STM32
 * Learning objectives:
 * - Understanding memory map architecture
 * - Pointer-based peripheral access
 * - Type casting for register access
 * - Volatile keyword importance
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>
#include <stdint.h>

void demonstrate_memory_map(void)
{
    printf("=== STM32F446RE Memory Map ===\n\n");
    
    printf("FLASH Memory:     0x08000000 - 0x0807FFFF (512 KB)\n");
    printf("SRAM1:            0x%08lX - 0x2001BFFF (112 KB)\n", 
           (unsigned long)SRAM1_BASEADDR);
    printf("SRAM2:            0x%08lX - 0x2001FFFF (16 KB)\n",
           (unsigned long)SRAM2_BASEADDR);
    printf("Peripherals Base: 0x%08lX\n\n", (unsigned long)PERIPH_BASEADDR);
    
    printf("Peripheral Buses:\n");
    printf("  APB1: 0x%08lX (Low-speed peripherals)\n", 
           (unsigned long)APB1_PERIPH_BASEADDR);
    printf("  APB2: 0x%08lX (High-speed peripherals)\n",
           (unsigned long)APB2_PERIPH_BASEADDR);
    printf("  AHB1: 0x%08lX (GPIO, DMA, RCC)\n",
           (unsigned long)AHB1_PERIPH_BASEADDR);
    printf("  AHB2: 0x%08lX\n\n", (unsigned long)AHB2_PERIPH_BASEADDR);
}

void demonstrate_pointer_access(void)
{
    printf("=== Pointer-Based Register Access ===\n\n");
    
    /* Method 1: Direct address calculation */
    printf("1. Direct Address Calculation:\n");
    volatile uint32_t *rcc_ahb1enr = (volatile uint32_t *)(RCC_BASEADDR + 0x30);
    printf("   RCC_AHB1ENR address: 0x%08lX\n", (unsigned long)rcc_ahb1enr);
    printf("   Current value: 0x%08lX\n\n", (unsigned long)*rcc_ahb1enr);
    
    /* Method 2: Structure-based access (preferred) */
    printf("2. Structure-Based Access (Preferred):\n");
    printf("   RCC base address: 0x%08lX\n", (unsigned long)RCC);
    printf("   RCC->AHB1ENR address: 0x%08lX\n", (unsigned long)&(RCC->AHB1ENR));
    printf("   RCC->AHB1ENR value: 0x%08lX\n\n", (unsigned long)RCC->AHB1ENR);
    
    /* Method 3: GPIO structure pointer */
    printf("3. GPIO Register Access:\n");
    GPIO_RegDef_t *pGPIOA = (GPIO_RegDef_t *)GPIOA_BASEADDR;
    printf("   GPIOA base: 0x%08lX\n", (unsigned long)pGPIOA);
    printf("   MODER offset: 0x00, address: 0x%08lX\n", 
           (unsigned long)&(pGPIOA->MODER));
    printf("   ODR offset:   0x14, address: 0x%08lX\n",
           (unsigned long)&(pGPIOA->ODR));
    printf("   IDR offset:   0x10, address: 0x%08lX\n\n",
           (unsigned long)&(pGPIOA->IDR));
}

void demonstrate_volatile_importance(void)
{
    printf("=== Importance of 'volatile' Keyword ===\n\n");
    
    printf("Without 'volatile':\n");
    printf("  Compiler may optimize away repeated reads\n");
    printf("  Hardware changes might not be detected\n");
    printf("  Example: uint32_t *ptr = (uint32_t*)0x40020000;\n\n");
    
    printf("With 'volatile':\n");
    printf("  Forces compiler to always read from memory\n");
    printf("  Essential for memory-mapped I/O\n");
    printf("  Example: volatile uint32_t *ptr = (volatile uint32_t*)0x40020000;\n\n");
    
    /* Demonstrate with actual register */
    RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA
    
    volatile uint32_t *idr_volatile = &(GPIOA->IDR);
    printf("Reading volatile IDR register:\n");
    for (int i = 0; i < 3; i++) {
        printf("  Read %d: 0x%04lX (always from memory)\n",
               i + 1, (unsigned long)(*idr_volatile & 0xFFFF));
    }
}

void demonstrate_register_layout(void)
{
    printf("\n=== GPIO Register Layout Example ===\n\n");
    
    printf("GPIO Port A Register Structure:\n");
    printf("Offset | Register | Description\n");
    printf("-------|----------|----------------------------------\n");
    printf("0x00   | MODER    | Mode (Input/Output/AF/Analog)\n");
    printf("0x04   | OTYPER   | Output Type (PP/OD)\n");
    printf("0x08   | OSPEEDR  | Output Speed\n");
    printf("0x0C   | PUPDR    | Pull-up/Pull-down\n");
    printf("0x10   | IDR      | Input Data Register\n");
    printf("0x14   | ODR      | Output Data Register\n");
    printf("0x18   | BSRRL    | Bit Set Register (lower)\n");
    printf("0x1A   | BSRRH    | Bit Reset Register (upper)\n");
    printf("0x1C   | LCKR     | Lock Register\n");
    printf("0x20   | AFR[0]   | Alt Function Low (pins 0-7)\n");
    printf("0x24   | AFR[1]   | Alt Function High (pins 8-15)\n\n");
    
    /* Show actual addresses */
    printf("GPIOA Actual Register Addresses:\n");
    printf("  MODER:   0x%08lX\n", (unsigned long)&(GPIOA->MODER));
    printf("  OTYPER:  0x%08lX\n", (unsigned long)&(GPIOA->OTYPER));
    printf("  OSPEEDR: 0x%08lX\n", (unsigned long)&(GPIOA->OSPEEDR));
    printf("  PUPDR:   0x%08lX\n", (unsigned long)&(GPIOA->PUPDR));
    printf("  IDR:     0x%08lX\n", (unsigned long)&(GPIOA->IDR));
    printf("  ODR:     0x%08lX\n", (unsigned long)&(GPIOA->ODR));
}

void demonstrate_peripheral_addresses(void)
{
    printf("\n=== Peripheral Base Addresses ===\n\n");
    
    printf("GPIO Ports:\n");
    printf("  GPIOA: 0x%08lX\n", (unsigned long)GPIOA_BASEADDR);
    printf("  GPIOB: 0x%08lX\n", (unsigned long)GPIOB_BASEADDR);
    printf("  GPIOC: 0x%08lX\n", (unsigned long)GPIOC_BASEADDR);
    printf("  (Offset between ports: 0x0400 = 1024 bytes)\n\n");
    
    printf("Communication Peripherals:\n");
    printf("  USART1: 0x%08lX (APB2)\n", (unsigned long)USART1_BASEADDR);
    printf("  USART2: 0x%08lX (APB1)\n", (unsigned long)USART2_BASEADDR);
    printf("  SPI1:   0x%08lX (APB2)\n", (unsigned long)SPI1_BASEADDR);
    printf("  SPI2:   0x%08lX (APB1)\n", (unsigned long)SPI2_BASEADDR);
    printf("  I2C1:   0x%08lX (APB1)\n\n", (unsigned long)I2C1_BASEADDR);
    
    printf("System Configuration:\n");
    printf("  RCC:    0x%08lX (Reset & Clock Control)\n", (unsigned long)RCC_BASEADDR);
    printf("  SYSCFG: 0x%08lX (System Configuration)\n", (unsigned long)SYSCFC_BASEADDR);
}

int main(void)
{
    printf("=== Memory-Mapped I/O Demonstration ===\n\n");
    
    demonstrate_memory_map();
    demonstrate_pointer_access();
    demonstrate_volatile_importance();
    demonstrate_register_layout();
    demonstrate_peripheral_addresses();
    
    printf("\n=== Key Concepts Summary ===\n");
    printf("1. Peripherals are accessed via memory addresses\n");
    printf("2. Structure pointers provide clean register access\n");
    printf("3. 'volatile' keyword prevents compiler optimization\n");
    printf("4. Register offsets are fixed in hardware\n");
    printf("5. Different buses for different peripheral speeds\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

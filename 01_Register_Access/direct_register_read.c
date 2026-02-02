/*
 * direct_register_read.c
 *
 * Demonstrates direct GPIO register reading without HAL abstraction
 * Learning objectives:
 * - Understanding memory-mapped I/O
 * - Reading GPIO Input Data Register (IDR)
 * - Working with volatile pointers
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

int main(void)
{
    printf("=== Direct Register Read Example ===\n\n");
    
    /* Enable GPIOA clock - Direct register write to RCC */
    printf("1. Enabling GPIOA clock via RCC->AHB1ENR\n");
    RCC->AHB1ENR |= (1 << 0);  // Set bit 0 to enable GPIOA clock
    printf("   RCC->AHB1ENR = 0x%08lX\n\n", (unsigned long)RCC->AHB1ENR);
    
    /* Configure PA5 as input - Direct access to MODER register */
    printf("2. Configuring PA5 as input (MODER = 00)\n");
    GPIOA->MODER &= ~(0x3 << 10);  // Clear bits [11:10] for PA5
    printf("   GPIOA->MODER = 0x%08lX\n\n", (unsigned long)GPIOA->MODER);
    
    /* Configure pull-up for PA5 - Direct access to PUPDR register */
    printf("3. Enabling pull-up resistor on PA5\n");
    GPIOA->PUPDR &= ~(0x3 << 10);  // Clear bits [11:10]
    GPIOA->PUPDR |= (0x1 << 10);   // Set to 01 for pull-up
    printf("   GPIOA->PUPDR = 0x%08lX\n\n", (unsigned long)GPIOA->PUPDR);
    
    /* Read GPIO Input Data Register (IDR) */
    printf("4. Reading GPIO Input Data Register (IDR)\n");
    uint32_t idr_value = GPIOA->IDR;
    printf("   GPIOA->IDR = 0x%08lX\n", (unsigned long)idr_value);
    
    /* Extract specific pin state (PA5 = bit 5) */
    uint8_t pa5_state = (idr_value >> 5) & 0x1;
    printf("   PA5 state = %d\n\n", pa5_state);
    
    /* Read multiple pins at once */
    printf("5. Reading multiple pins from Port A:\n");
    for (int i = 0; i < 8; i++) {
        uint8_t pin_state = (idr_value >> i) & 0x1;
        printf("   PA%d = %d\n", i, pin_state);
    }
    
    /* Demonstrate reading from different GPIO ports */
    printf("\n6. Reading from multiple GPIO ports:\n");
    
    // Enable GPIOB and GPIOC clocks
    RCC->AHB1ENR |= (1 << 1) | (1 << 2);
    
    printf("   GPIOA->IDR = 0x%04lX\n", (unsigned long)(GPIOA->IDR & 0xFFFF));
    printf("   GPIOB->IDR = 0x%04lX\n", (unsigned long)(GPIOB->IDR & 0xFFFF));
    printf("   GPIOC->IDR = 0x%04lX\n", (unsigned long)(GPIOC->IDR & 0xFFFF));
    
    /* Show register addresses */
    printf("\n7. GPIO Register Base Addresses:\n");
    printf("   GPIOA base: 0x%08lX\n", (unsigned long)GPIOA_BASEADDR);
    printf("   GPIOB base: 0x%08lX\n", (unsigned long)GPIOB_BASEADDR);
    printf("   GPIOC base: 0x%08lX\n", (unsigned long)GPIOC_BASEADDR);
    printf("   IDR offset: 0x10 (from base)\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

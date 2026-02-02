/*
 * direct_register_write.c
 *
 * Demonstrates direct GPIO register writing without HAL abstraction
 * Learning objectives:
 * - Writing to GPIO Output Data Register (ODR)
 * - Using Bit Set/Reset Register (BSRR) for atomic operations
 * - Understanding register write operations
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Simple delay function */
void delay(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++);
}

int main(void)
{
    printf("=== Direct Register Write Example ===\n\n");
    
    /* Enable GPIOA clock */
    printf("1. Enabling GPIOA peripheral clock\n");
    RCC->AHB1ENR |= (1 << 0);
    printf("   RCC->AHB1ENR = 0x%08lX\n\n", (unsigned long)RCC->AHB1ENR);
    
    /* Configure PA5 as output using MODER register */
    printf("2. Configuring PA5 as output (MODER = 01)\n");
    GPIOA->MODER &= ~(0x3 << 10);  // Clear bits [11:10]
    GPIOA->MODER |= (0x1 << 10);   // Set to 01 for output
    printf("   GPIOA->MODER = 0x%08lX\n\n", (unsigned long)GPIOA->MODER);
    
    /* Configure output type as push-pull */
    printf("3. Configuring PA5 as push-pull output\n");
    GPIOA->OTYPER &= ~(1 << 5);    // Clear bit 5 for push-pull
    printf("   GPIOA->OTYPER = 0x%08lX\n\n", (unsigned long)GPIOA->OTYPER);
    
    /* Set output speed to high */
    printf("4. Setting PA5 speed to HIGH\n");
    GPIOA->OSPEEDR &= ~(0x3 << 10); // Clear bits [11:10]
    GPIOA->OSPEEDR |= (0x3 << 10);  // Set to 11 for high speed
    printf("   GPIOA->OSPEEDR = 0x%08lX\n\n", (unsigned long)GPIOA->OSPEEDR);
    
    /* Method 1: Writing to ODR register directly */
    printf("5. Method 1: Writing to ODR (Output Data Register)\n");
    printf("   Setting PA5 HIGH using ODR\n");
    GPIOA->ODR |= (1 << 5);
    printf("   GPIOA->ODR = 0x%08lX\n", (unsigned long)GPIOA->ODR);
    delay(1000000);
    
    printf("   Clearing PA5 using ODR\n");
    GPIOA->ODR &= ~(1 << 5);
    printf("   GPIOA->ODR = 0x%08lX\n\n", (unsigned long)GPIOA->ODR);
    delay(1000000);
    
    /* Method 2: Using BSRR for atomic set/reset (preferred method) */
    printf("6. Method 2: Using BSRR (Bit Set/Reset Register) - Atomic!\n");
    printf("   Note: BSRR provides atomic bit manipulation without read-modify-write\n\n");
    
    // Set PA5 using BSRR lower 16 bits (BS - Bit Set)
    printf("   Setting PA5 HIGH using BSRR (bit 5)\n");
    GPIOA->BSRRL = (1 << 5);
    printf("   Wrote 0x%04X to BSRRL\n", (1 << 5));
    printf("   GPIOA->ODR = 0x%08lX\n", (unsigned long)GPIOA->ODR);
    delay(1000000);
    
    // Reset PA5 using BSRR upper 16 bits (BR - Bit Reset)
    printf("   Clearing PA5 using BSRRH (bit 5)\n");
    GPIOA->BSRRH = (1 << 5);
    printf("   Wrote 0x%04X to BSRRH\n", (1 << 5));
    printf("   GPIOA->ODR = 0x%08lX\n\n", (unsigned long)GPIOA->ODR);
    delay(1000000);
    
    /* LED Blink demonstration */
    printf("7. Blinking PA5 LED 5 times using BSRR\n");
    for (int i = 0; i < 5; i++) {
        printf("   Blink %d: ON\n", i + 1);
        GPIOA->BSRRL = (1 << 5);  // Set
        delay(500000);
        
        printf("   Blink %d: OFF\n", i + 1);
        GPIOA->BSRRH = (1 << 5);  // Reset
        delay(500000);
    }
    
    /* Control multiple pins simultaneously */
    printf("\n8. Controlling multiple pins (PA5, PA6, PA7)\n");
    
    // Configure PA6 and PA7 as outputs
    GPIOA->MODER &= ~((0x3 << 12) | (0x3 << 14));
    GPIOA->MODER |= ((0x1 << 12) | (0x1 << 14));
    
    printf("   Setting PA5, PA6, PA7 HIGH simultaneously\n");
    GPIOA->BSRRL = (1 << 5) | (1 << 6) | (1 << 7);
    printf("   GPIOA->ODR = 0x%08lX\n", (unsigned long)GPIOA->ODR);
    delay(1000000);
    
    printf("   Clearing PA5, PA6, PA7 simultaneously\n");
    GPIOA->BSRRH = (1 << 5) | (1 << 6) | (1 << 7);
    printf("   GPIOA->ODR = 0x%08lX\n", (unsigned long)GPIOA->ODR);
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

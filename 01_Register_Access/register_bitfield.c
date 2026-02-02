/*
 * register_bitfield.c
 *
 * Demonstrates bitfield operations on STM32 registers
 * Learning objectives:
 * - Bit manipulation techniques
 * - Setting, clearing, toggling, and reading bits
 * - Working with multi-bit fields
 * - Register masking operations
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Bit manipulation macros */
#define SET_BIT(REG, BIT)       ((REG) |= (1UL << (BIT)))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1UL << (BIT)))
#define TOGGLE_BIT(REG, BIT)    ((REG) ^= (1UL << (BIT)))
#define READ_BIT(REG, BIT)      (((REG) >> (BIT)) & 1UL)
#define WRITE_BIT(REG, BIT, VAL) \
    do { if (VAL) SET_BIT(REG, BIT); else CLEAR_BIT(REG, BIT); } while(0)

/* Multi-bit field macros */
#define SET_FIELD(REG, MASK, OFFSET, VALUE) \
    do { \
        (REG) &= ~((MASK) << (OFFSET)); \
        (REG) |= (((VALUE) & (MASK)) << (OFFSET)); \
    } while(0)

#define READ_FIELD(REG, MASK, OFFSET) \
    (((REG) >> (OFFSET)) & (MASK))

void print_binary(uint32_t value, uint8_t bits)
{
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0 && i != 0) printf(" ");
    }
}

int main(void)
{
    printf("=== Register Bitfield Operations Example ===\n\n");
    
    /* Enable GPIO clocks */
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOA and GPIOB
    
    /* Example 1: Single bit operations */
    printf("1. SINGLE BIT OPERATIONS\n");
    printf("   Initial RCC->AHB1ENR: 0x%08lX\n", (unsigned long)RCC->AHB1ENR);
    
    SET_BIT(RCC->AHB1ENR, 2);  // Enable GPIOC
    printf("   After SET_BIT(2): 0x%08lX (GPIOC enabled)\n", 
           (unsigned long)RCC->AHB1ENR);
    
    CLEAR_BIT(RCC->AHB1ENR, 2);  // Disable GPIOC
    printf("   After CLEAR_BIT(2): 0x%08lX (GPIOC disabled)\n",
           (unsigned long)RCC->AHB1ENR);
    
    printf("   Reading bit 0: %d (GPIOA clock status)\n\n",
           READ_BIT(RCC->AHB1ENR, 0));
    
    /* Example 2: Multi-bit field operations (MODER) */
    printf("2. MULTI-BIT FIELD OPERATIONS (GPIO MODER)\n");
    printf("   Each pin uses 2 bits: 00=Input, 01=Output, 10=Alt Func, 11=Analog\n\n");
    
    printf("   Initial GPIOA->MODER: 0x%08lX\n", (unsigned long)GPIOA->MODER);
    
    // Configure PA5 as output (01)
    SET_FIELD(GPIOA->MODER, 0x3, 10, 0x1);  // Bits [11:10] = 01
    printf("   After setting PA5 as output: 0x%08lX\n", (unsigned long)GPIOA->MODER);
    printf("   PA5 mode bits [11:10]: ");
    print_binary(READ_FIELD(GPIOA->MODER, 0x3, 10), 2);
    printf(" (Output)\n\n");
    
    // Configure PA6 as alternate function (10)
    SET_FIELD(GPIOA->MODER, 0x3, 12, 0x2);  // Bits [13:12] = 10
    printf("   After setting PA6 as alternate function: 0x%08lX\n", 
           (unsigned long)GPIOA->MODER);
    printf("   PA6 mode bits [13:12]: ");
    print_binary(READ_FIELD(GPIOA->MODER, 0x3, 12), 2);
    printf(" (Alt Func)\n\n");
    
    /* Example 3: Speed configuration (OSPEEDR) */
    printf("3. SPEED CONFIGURATION (OSPEEDR)\n");
    printf("   Speed: 00=Low, 01=Medium, 10=Fast, 11=High\n\n");
    
    printf("   Initial GPIOA->OSPEEDR: 0x%08lX\n", (unsigned long)GPIOA->OSPEEDR);
    
    // Set PA5 to high speed (11)
    SET_FIELD(GPIOA->OSPEEDR, 0x3, 10, 0x3);
    printf("   PA5 speed set to HIGH: 0x%08lX\n", (unsigned long)GPIOA->OSPEEDR);
    printf("   PA5 speed bits [11:10]: ");
    print_binary(READ_FIELD(GPIOA->OSPEEDR, 0x3, 10), 2);
    printf(" (High)\n\n");
    
    /* Example 4: Pull-up/Pull-down configuration (PUPDR) */
    printf("4. PULL-UP/PULL-DOWN CONFIGURATION (PUPDR)\n");
    printf("   00=No pull, 01=Pull-up, 10=Pull-down, 11=Reserved\n\n");
    
    printf("   Initial GPIOA->PUPDR: 0x%08lX\n", (unsigned long)GPIOA->PUPDR);
    
    // Configure PA5 with pull-up (01)
    SET_FIELD(GPIOA->PUPDR, 0x3, 10, 0x1);
    printf("   PA5 configured with pull-up: 0x%08lX\n", (unsigned long)GPIOA->PUPDR);
    printf("   PA5 PUPD bits [11:10]: ");
    print_binary(READ_FIELD(GPIOA->PUPDR, 0x3, 10), 2);
    printf(" (Pull-up)\n\n");
    
    /* Example 5: Output type (OTYPER) */
    printf("5. OUTPUT TYPE CONFIGURATION (OTYPER)\n");
    printf("   0=Push-pull, 1=Open-drain\n\n");
    
    printf("   Initial GPIOA->OTYPER: 0x%08lX\n", (unsigned long)GPIOA->OTYPER);
    
    // Set PA5 to open-drain
    SET_BIT(GPIOA->OTYPER, 5);
    printf("   PA5 set to open-drain: 0x%08lX\n", (unsigned long)GPIOA->OTYPER);
    printf("   Bit 5: %d (Open-drain)\n\n", READ_BIT(GPIOA->OTYPER, 5));
    
    // Set PA5 back to push-pull
    CLEAR_BIT(GPIOA->OTYPER, 5);
    printf("   PA5 set to push-pull: 0x%08lX\n", (unsigned long)GPIOA->OTYPER);
    printf("   Bit 5: %d (Push-pull)\n\n", READ_BIT(GPIOA->OTYPER, 5));
    
    /* Example 6: Alternate function configuration (AFR) */
    printf("6. ALTERNATE FUNCTION CONFIGURATION (AFR)\n");
    printf("   4 bits per pin: 0000-1111 selects AF0-AF15\n\n");
    
    printf("   Initial GPIOA->AFR[0] (pins 0-7): 0x%08lX\n", 
           (unsigned long)GPIOA->AFR[0]);
    
    // Configure PA5 as AF7 (bits [23:20] of AFR[0])
    SET_FIELD(GPIOA->AFR[0], 0xF, 20, 0x7);
    printf("   PA5 configured as AF7: 0x%08lX\n", (unsigned long)GPIOA->AFR[0]);
    printf("   PA5 AF bits [23:20]: ");
    print_binary(READ_FIELD(GPIOA->AFR[0], 0xF, 20), 4);
    printf(" (AF7)\n\n");
    
    /* Example 7: Bit toggling */
    printf("7. BIT TOGGLE DEMONSTRATION\n");
    
    GPIOA->MODER &= ~(0x3 << 10);
    GPIOA->MODER |= (0x1 << 10);  // PA5 as output
    
    printf("   Initial ODR: 0x%08lX (bit 5 = %d)\n",
           (unsigned long)GPIOA->ODR, READ_BIT(GPIOA->ODR, 5));
    
    for (int i = 0; i < 4; i++) {
        TOGGLE_BIT(GPIOA->ODR, 5);
        printf("   Toggle %d: ODR = 0x%08lX (bit 5 = %d)\n",
               i + 1, (unsigned long)GPIOA->ODR, READ_BIT(GPIOA->ODR, 5));
    }
    
    /* Example 8: Reading and displaying full register in binary */
    printf("\n8. COMPLETE REGISTER VISUALIZATION\n");
    printf("   GPIOA->MODER (32-bit): ");
    print_binary(GPIOA->MODER, 32);
    printf("\n");
    
    printf("   Bits [31:28]: ");
    print_binary(READ_FIELD(GPIOA->MODER, 0xF, 28), 4);
    printf(" (PA15-PA14)\n");
    
    printf("   Bits [15:12]: ");
    print_binary(READ_FIELD(GPIOA->MODER, 0xF, 12), 4);
    printf(" (PA7-PA6)\n");
    
    printf("   Bits [11:8]:  ");
    print_binary(READ_FIELD(GPIOA->MODER, 0xF, 8), 4);
    printf(" (PA5-PA4)\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

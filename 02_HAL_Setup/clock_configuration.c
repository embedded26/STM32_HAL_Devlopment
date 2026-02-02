/*
 * clock_configuration.c
 *
 * Demonstrates system clock configuration for STM32F446RE
 * Learning objectives:
 * - Understanding clock tree architecture
 * - Configuring PLL for higher frequencies
 * - Setting up bus prescalers
 * - Flash latency configuration
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Clock configuration constants */
#define HSI_CLOCK_FREQ      16000000UL  // Internal RC oscillator
#define HSE_CLOCK_FREQ      8000000UL   // External crystal (Nucleo board)
#define TARGET_SYSCLK_FREQ  84000000UL  // Target system clock

void print_clock_configuration(void)
{
    printf("=== Current Clock Configuration ===\n\n");
    
    /* Read clock control register */
    printf("RCC->CR (Clock Control Register): 0x%08lX\n", (unsigned long)RCC->CR);
    printf("  HSI ON: %s\n", (RCC->CR & (1 << 0)) ? "Yes" : "No");
    printf("  HSI RDY: %s\n", (RCC->CR & (1 << 1)) ? "Yes" : "No");
    printf("  HSE ON: %s\n", (RCC->CR & (1 << 16)) ? "Yes" : "No");
    printf("  HSE RDY: %s\n", (RCC->CR & (1 << 17)) ? "Yes" : "No");
    printf("  PLL ON: %s\n", (RCC->CR & (1 << 24)) ? "Yes" : "No");
    printf("  PLL RDY: %s\n\n", (RCC->CR & (1 << 25)) ? "Yes" : "No");
    
    /* Read clock configuration register */
    printf("RCC->CFGR (Clock Configuration Register): 0x%08lX\n", 
           (unsigned long)RCC->CFGR);
    
    uint8_t sws = (RCC->CFGR >> 2) & 0x3;
    printf("  System Clock Source: ");
    switch(sws) {
        case 0: printf("HSI\n"); break;
        case 1: printf("HSE\n"); break;
        case 2: printf("PLL\n"); break;
        default: printf("Unknown\n");
    }
    
    uint8_t ahb_prescaler = (RCC->CFGR >> 4) & 0xF;
    printf("  AHB Prescaler: %d\n", ahb_prescaler);
    
    uint8_t apb1_prescaler = (RCC->CFGR >> 10) & 0x7;
    printf("  APB1 Prescaler: %d\n", apb1_prescaler);
    
    uint8_t apb2_prescaler = (RCC->CFGR >> 13) & 0x7;
    printf("  APB2 Prescaler: %d\n\n", apb2_prescaler);
    
    /* Read PLL configuration */
    printf("RCC->PLLCFGR (PLL Configuration): 0x%08lX\n", 
           (unsigned long)RCC->PLLCFGR);
    
    uint8_t pllm = RCC->PLLCFGR & 0x3F;
    uint16_t plln = (RCC->PLLCFGR >> 6) & 0x1FF;
    uint8_t pllp = ((RCC->PLLCFGR >> 16) & 0x3);
    uint8_t pllq = (RCC->PLLCFGR >> 24) & 0xF;
    
    printf("  PLLM: %d\n", pllm);
    printf("  PLLN: %d\n", plln);
    printf("  PLLP: %d\n", pllp);
    printf("  PLLQ: %d\n", pllq);
}

void configure_system_clock_hsi(void)
{
    printf("\n=== Configuring System Clock to HSI (16 MHz) ===\n");
    
    /* HSI is enabled by default, just switch to it */
    
    // 1. Wait for HSI to be ready
    printf("1. Waiting for HSI ready...\n");
    while (!(RCC->CR & (1 << 1)));
    printf("   HSI is ready!\n");
    
    // 2. Switch system clock to HSI
    printf("2. Switching system clock to HSI...\n");
    RCC->CFGR &= ~(0x3 << 0);  // Clear SW[1:0] bits
    RCC->CFGR |= 0x0;          // SW = 00 for HSI
    
    // 3. Wait for switch to complete
    printf("3. Waiting for clock switch...\n");
    while (((RCC->CFGR >> 2) & 0x3) != 0x0);
    printf("   System clock switched to HSI\n");
    
    printf("System Clock: 16 MHz (HSI)\n");
}

void configure_system_clock_pll(void)
{
    printf("\n=== Configuring System Clock to PLL (84 MHz) ===\n");
    
    /*
     * PLL Configuration for 84 MHz:
     * - Input: HSI (16 MHz)
     * - VCO Input: 16 MHz / PLLM = 16 MHz / 8 = 2 MHz
     * - VCO Output: 2 MHz * PLLN = 2 MHz * 168 = 336 MHz
     * - System Clock: 336 MHz / PLLP = 336 MHz / 4 = 84 MHz
     */
    
    // 1. Enable HSI and wait for it to be ready
    printf("1. Enabling HSI...\n");
    RCC->CR |= (1 << 0);
    while (!(RCC->CR & (1 << 1)));
    printf("   HSI ready\n");
    
    // 2. Disable PLL before configuration
    printf("2. Disabling PLL for reconfiguration...\n");
    RCC->CR &= ~(1 << 24);
    while (RCC->CR & (1 << 25));
    printf("   PLL disabled\n");
    
    // 3. Configure PLL
    printf("3. Configuring PLL...\n");
    RCC->PLLCFGR = 0;  // Clear register
    
    RCC->PLLCFGR |= (8 << 0);     // PLLM = 8
    RCC->PLLCFGR |= (168 << 6);   // PLLN = 168
    RCC->PLLCFGR |= (0 << 16);    // PLLP = 2 (00 = /2, but we want /4)
    RCC->PLLCFGR |= (1 << 16);    // PLLP = 4 (01 = /4)
    RCC->PLLCFGR |= (7 << 24);    // PLLQ = 7
    RCC->PLLCFGR &= ~(1 << 22);   // PLL source = HSI
    
    printf("   PLLM = 8, PLLN = 168, PLLP = 4, PLLQ = 7\n");
    printf("   VCO In: 2 MHz, VCO Out: 336 MHz, Sys: 84 MHz\n");
    
    // 4. Configure Flash latency for 84 MHz
    printf("4. Setting Flash latency...\n");
    volatile uint32_t *flash_acr = (volatile uint32_t *)0x40023C00;
    *flash_acr |= (2 << 0);  // 2 wait states for 84 MHz
    printf("   Flash latency: 2 wait states\n");
    
    // 5. Configure bus prescalers
    printf("5. Configuring bus prescalers...\n");
    RCC->CFGR &= ~(0xF << 4);    // AHB prescaler = 1
    RCC->CFGR &= ~(0x7 << 10);
    RCC->CFGR |= (0x4 << 10);    // APB1 prescaler = 2 (42 MHz max)
    RCC->CFGR &= ~(0x7 << 13);   // APB2 prescaler = 1
    printf("   AHB: /1, APB1: /2, APB2: /1\n");
    
    // 6. Enable PLL
    printf("6. Enabling PLL...\n");
    RCC->CR |= (1 << 24);
    while (!(RCC->CR & (1 << 25)));
    printf("   PLL locked and ready\n");
    
    // 7. Switch system clock to PLL
    printf("7. Switching system clock to PLL...\n");
    RCC->CFGR &= ~(0x3 << 0);
    RCC->CFGR |= (0x2 << 0);  // SW = 10 for PLL
    
    while (((RCC->CFGR >> 2) & 0x3) != 0x2);
    printf("   System clock switched to PLL\n");
    
    printf("\nFinal Clock Configuration:\n");
    printf("  SYSCLK: 84 MHz\n");
    printf("  AHB:    84 MHz\n");
    printf("  APB1:   42 MHz\n");
    printf("  APB2:   84 MHz\n");
}

void demonstrate_clock_tree(void)
{
    printf("\n=== STM32F446RE Clock Tree ===\n\n");
    
    printf("Clock Sources:\n");
    printf("  HSI: 16 MHz (Internal RC oscillator)\n");
    printf("  HSE: 8 MHz (External crystal - Nucleo board)\n");
    printf("  LSI: 32 kHz (Low speed internal)\n");
    printf("  LSE: 32.768 kHz (Low speed external - RTC)\n\n");
    
    printf("PLL Configuration:\n");
    printf("  Input -> /PLLM -> *PLLN -> /PLLP -> SYSCLK\n");
    printf("                          -> /PLLQ -> USB/SDIO (48 MHz)\n\n");
    
    printf("System Clock Distribution:\n");
    printf("  SYSCLK -> AHB Prescaler -> AHB Clock (CPU, DMA, etc.)\n");
    printf("         -> APB1 Prescaler -> APB1 Clock (TIM2-7, USART2-3, I2C)\n");
    printf("         -> APB2 Prescaler -> APB2 Clock (TIM1,8, USART1, SPI1)\n\n");
    
    printf("Maximum Frequencies (STM32F446RE):\n");
    printf("  SYSCLK: 180 MHz\n");
    printf("  AHB:    180 MHz\n");
    printf("  APB1:   45 MHz\n");
    printf("  APB2:   90 MHz\n");
}

void print_peripheral_clocks(void)
{
    printf("\n=== Peripheral Clock Status ===\n\n");
    
    printf("AHB1 Peripherals (RCC->AHB1ENR = 0x%08lX):\n",
           (unsigned long)RCC->AHB1ENR);
    printf("  GPIOA: %s\n", (RCC->AHB1ENR & (1 << 0)) ? "Enabled" : "Disabled");
    printf("  GPIOB: %s\n", (RCC->AHB1ENR & (1 << 1)) ? "Enabled" : "Disabled");
    printf("  GPIOC: %s\n", (RCC->AHB1ENR & (1 << 2)) ? "Enabled" : "Disabled");
    printf("  DMA1:  %s\n", (RCC->AHB1ENR & (1 << 21)) ? "Enabled" : "Disabled");
    printf("  DMA2:  %s\n\n", (RCC->AHB1ENR & (1 << 22)) ? "Enabled" : "Disabled");
}

int main(void)
{
    printf("=== STM32 Clock Configuration Example ===\n");
    
    // Show initial configuration
    print_clock_configuration();
    demonstrate_clock_tree();
    
    // Configure to HSI
    configure_system_clock_hsi();
    print_clock_configuration();
    
    // Configure to PLL for higher performance
    configure_system_clock_pll();
    print_clock_configuration();
    
    // Show peripheral clock status
    print_peripheral_clocks();
    
    printf("\n=== Clock Configuration Complete ===\n");
    
    return 0;
}

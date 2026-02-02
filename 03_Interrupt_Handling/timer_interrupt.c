/*
 * timer_interrupt.c
 *
 * Demonstrates Timer interrupt configuration and usage
 * Learning objectives:
 * - Timer peripheral initialization
 * - Update interrupt configuration
 * - Prescaler and period calculation
 * - Creating periodic interrupts
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Timer Register Structure (TIM2-TIM5) */
typedef struct {
    volatile uint32_t CR1;     // Control register 1
    volatile uint32_t CR2;     // Control register 2
    volatile uint32_t SMCR;    // Slave mode control
    volatile uint32_t DIER;    // DMA/Interrupt enable
    volatile uint32_t SR;      // Status register
    volatile uint32_t EGR;     // Event generation
    volatile uint32_t CCMR1;   // Capture/compare mode 1
    volatile uint32_t CCMR2;   // Capture/compare mode 2
    volatile uint32_t CCER;    // Capture/compare enable
    volatile uint32_t CNT;     // Counter
    volatile uint32_t PSC;     // Prescaler
    volatile uint32_t ARR;     // Auto-reload register
    uint32_t RESERVED1;
    volatile uint32_t CCR1;    // Capture/compare 1
    volatile uint32_t CCR2;    // Capture/compare 2
    volatile uint32_t CCR3;    // Capture/compare 3
    volatile uint32_t CCR4;    // Capture/compare 4
    uint32_t RESERVED2;
    volatile uint32_t DCR;     // DMA control
    volatile uint32_t DMAR;    // DMA address
    volatile uint32_t OR;      // Option register
} TIM_RegDef_t;

#define TIM2 ((TIM_RegDef_t*)0x40000000)
#define TIM3 ((TIM_RegDef_t*)0x40000400)
#define TIM4 ((TIM_RegDef_t*)0x40000800)
#define TIM5 ((TIM_RegDef_t*)0x40000C00)

/* NVIC registers */
#define NVIC_ISER0      ((volatile uint32_t*)0xE000E100)
#define NVIC_IPR        ((volatile uint32_t*)0xE000E400)

/* IRQ numbers */
#define IRQ_TIM2        28
#define IRQ_TIM3        29
#define IRQ_TIM4        30
#define IRQ_TIM5        50

/* Global timer variables */
volatile uint32_t timer_tick_count = 0;
volatile uint32_t milliseconds = 0;

void demonstrate_timer_architecture(void)
{
    printf("=== Timer Architecture ===\n\n");
    
    printf("STM32F446RE Timer Types:\n");
    printf("1. Basic Timers (TIM6, TIM7)\n");
    printf("   - Simple up counters\n");
    printf("   - No capture/compare\n");
    printf("   - Good for timebase generation\n\n");
    
    printf("2. General-Purpose Timers (TIM2-TIM5, TIM9-TIM14)\n");
    printf("   - Up, down, or up/down counting\n");
    printf("   - 4 capture/compare channels\n");
    printf("   - PWM generation\n");
    printf("   - Input capture\n\n");
    
    printf("3. Advanced Timers (TIM1, TIM8)\n");
    printf("   - All general-purpose features\n");
    printf("   - Complementary outputs\n");
    printf("   - Dead-time generation\n");
    printf("   - Break input\n\n");
    
    printf("Timer Frequency Calculation:\n");
    printf("Timer_Freq = APB_Clock / (Prescaler + 1)\n");
    printf("Update_Freq = Timer_Freq / (Period + 1)\n\n");
}

void TIM2_ClockEnable(void)
{
    printf("=== Enabling TIM2 Clock ===\n");
    
    /* TIM2 is on APB1 bus */
    printf("1. TIM2 is on APB1 bus\n");
    printf("2. Enabling TIM2 clock in RCC->APB1ENR...\n");
    
    RCC->APB1ENR |= (1 << 0);  // TIM2EN bit
    
    printf("   RCC->APB1ENR = 0x%08lX\n", (unsigned long)RCC->APB1ENR);
    printf("   TIM2 clock enabled\n\n");
}

void TIM2_BasicConfig(uint16_t prescaler, uint32_t period)
{
    printf("=== Configuring TIM2 ===\n");
    
    printf("1. Setting prescaler to %d...\n", prescaler);
    TIM2->PSC = prescaler;
    printf("   TIM2->PSC = %d\n", TIM2->PSC);
    
    printf("2. Setting auto-reload value to %lu...\n", (unsigned long)period);
    TIM2->ARR = period;
    printf("   TIM2->ARR = %lu\n\n", (unsigned long)TIM2->ARR);
    
    /* Calculate resulting frequency */
    uint32_t apb1_clock = 42000000;  // Assuming 42 MHz
    float timer_freq = (float)apb1_clock / (prescaler + 1);
    float update_freq = timer_freq / (period + 1);
    
    printf("Frequency Calculation:\n");
    printf("   APB1 Clock: %lu Hz\n", (unsigned long)apb1_clock);
    printf("   Timer Frequency: %.2f Hz\n", timer_freq);
    printf("   Update Frequency: %.2f Hz\n", update_freq);
    printf("   Update Period: %.3f ms\n\n", 1000.0f / update_freq);
}

void TIM2_InterruptConfig(void)
{
    printf("=== Configuring TIM2 Interrupt ===\n");
    
    /* Enable update interrupt */
    printf("1. Enabling update interrupt (UIE bit)...\n");
    TIM2->DIER |= (1 << 0);  // UIE - Update interrupt enable
    printf("   TIM2->DIER = 0x%08lX\n\n", (unsigned long)TIM2->DIER);
    
    /* Configure NVIC */
    printf("2. Configuring NVIC for TIM2 (IRQ %d)...\n", IRQ_TIM2);
    
    /* Enable interrupt */
    *NVIC_ISER0 |= (1 << IRQ_TIM2);
    printf("   Interrupt enabled in NVIC\n");
    
    /* Set priority */
    uint8_t priority = 6;
    uint8_t shift = (IRQ_TIM2 % 4) * 8 + 4;
    volatile uint32_t *ipr = NVIC_IPR + (IRQ_TIM2 / 4);
    *ipr &= ~(0xF << shift);
    *ipr |= (priority << shift);
    printf("   Priority set to %d\n\n", priority);
}

void TIM2_Start(void)
{
    printf("=== Starting TIM2 ===\n");
    
    /* Enable counter */
    printf("1. Enabling counter (CEN bit)...\n");
    TIM2->CR1 |= (1 << 0);  // CEN - Counter enable
    
    printf("   TIM2->CR1 = 0x%08lX\n", (unsigned long)TIM2->CR1);
    printf("   Timer started!\n\n");
}

void TIM2_Stop(void)
{
    printf("=== Stopping TIM2 ===\n");
    
    /* Disable counter */
    TIM2->CR1 &= ~(1 << 0);
    printf("   Timer stopped\n\n");
}

void example_1ms_timer(void)
{
    printf("=== Example: 1ms Periodic Timer ===\n\n");
    
    /*
     * Goal: Generate interrupt every 1ms
     * APB1 clock: 42 MHz
     * 
     * For 1ms (1000 Hz):
     * Timer_Freq = 42,000,000 / (41 + 1) = 1,000,000 Hz
     * Update_Freq = 1,000,000 / (999 + 1) = 1,000 Hz = 1ms
     */
    
    printf("Target: 1ms interrupt period\n");
    printf("APB1 Clock: 42 MHz\n\n");
    
    printf("Calculation:\n");
    printf("Prescaler: 42 - 1 = 41\n");
    printf("Period: 1000 - 1 = 999\n");
    printf("Result: 42MHz / 42 / 1000 = 1kHz = 1ms\n\n");
    
    TIM2_ClockEnable();
    TIM2_BasicConfig(41, 999);
    TIM2_InterruptConfig();
    
    printf("1ms timer configured. ISR will be called every 1ms.\n\n");
}

void example_100us_timer(void)
{
    printf("=== Example: 100us Periodic Timer ===\n\n");
    
    /*
     * Goal: Generate interrupt every 100us (10 kHz)
     * APB1 clock: 42 MHz
     * 
     * Timer_Freq = 42,000,000 / (4 + 1) = 8,400,000 Hz
     * Update_Freq = 8,400,000 / (839 + 1) = 10,000 Hz = 100us
     */
    
    printf("Target: 100us interrupt period\n\n");
    
    printf("Calculation:\n");
    printf("Prescaler: 4\n");
    printf("Period: 839\n");
    printf("Result: 42MHz / 5 / 840 = 10kHz = 100us\n\n");
    
    TIM2_ClockEnable();
    TIM2_BasicConfig(4, 839);
    TIM2_InterruptConfig();
    
    printf("100us timer configured.\n\n");
}

void example_1second_timer(void)
{
    printf("=== Example: 1 Second Timer ===\n\n");
    
    /*
     * Goal: Generate interrupt every 1 second (1 Hz)
     * APB1 clock: 42 MHz
     * 
     * Timer_Freq = 42,000,000 / (41999 + 1) = 1,000 Hz
     * Update_Freq = 1,000 / (999 + 1) = 1 Hz = 1 second
     */
    
    printf("Target: 1 second interrupt period\n\n");
    
    printf("Calculation:\n");
    printf("Prescaler: 41999\n");
    printf("Period: 999\n");
    printf("Result: 42MHz / 42000 / 1000 = 1Hz = 1s\n\n");
    
    TIM2_ClockEnable();
    TIM2_BasicConfig(41999, 999);
    TIM2_InterruptConfig();
    
    printf("1 second timer configured.\n\n");
}

void demonstrate_isr_implementation(void)
{
    printf("=== Timer ISR Implementation ===\n\n");
    
    printf("TIM2 Update Interrupt Handler:\n\n");
    
    printf("void TIM2_IRQHandler(void)\n");
    printf("{\n");
    printf("    // Check update interrupt flag\n");
    printf("    if (TIM2->SR & (1 << 0)) {\n");
    printf("        // Clear update interrupt flag (MUST DO!)\n");
    printf("        TIM2->SR &= ~(1 << 0);\n");
    printf("        \n");
    printf("        // Handle timer tick\n");
    printf("        timer_tick_count++;\n");
    printf("        milliseconds++;  // If 1ms timer\n");
    printf("        \n");
    printf("        // Toggle LED every 500ms\n");
    printf("        if (milliseconds %% 500 == 0) {\n");
    printf("            GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);\n");
    printf("        }\n");
    printf("    }\n");
    printf("}\n\n");
    
    printf("Important Notes:\n");
    printf("1. Always check SR register flags\n");
    printf("2. Clear interrupt flag by writing 0\n");
    printf("3. Keep ISR execution short\n");
    printf("4. Avoid blocking operations in ISR\n\n");
}

void demonstrate_timer_modes(void)
{
    printf("=== Timer Counting Modes ===\n\n");
    
    printf("1. Up-counting Mode (default)\n");
    printf("   - Counts from 0 to ARR\n");
    printf("   - Update event on overflow\n");
    printf("   - CR1.DIR = 0\n\n");
    
    printf("2. Down-counting Mode\n");
    printf("   - Counts from ARR to 0\n");
    printf("   - Update event on underflow\n");
    printf("   - CR1.DIR = 1\n\n");
    
    printf("3. Center-aligned Mode\n");
    printf("   - Counts up then down\n");
    printf("   - Update event on both ends\n");
    printf("   - CR1.CMS = 01, 10, or 11\n\n");
    
    printf("Example: Down-counting mode\n");
    printf("TIM2->CR1 |= (1 << 4);  // Set DIR bit\n\n");
}

void demonstrate_one_pulse_mode(void)
{
    printf("=== One-Pulse Mode ===\n\n");
    
    printf("One-pulse mode generates single pulse:\n");
    printf("- Counter stops after update event\n");
    printf("- Useful for delays and timeouts\n\n");
    
    printf("Configuration:\n");
    printf("TIM2->CR1 |= (1 << 3);  // OPM bit\n");
    printf("TIM2->CR1 |= (1 << 0);  // Start counter\n");
    printf("// Timer stops automatically after period\n\n");
}

int main(void)
{
    printf("=== Timer Interrupt Example ===\n\n");
    
    demonstrate_timer_architecture();
    
    example_1ms_timer();
    
    example_100us_timer();
    
    example_1second_timer();
    
    demonstrate_isr_implementation();
    
    demonstrate_timer_modes();
    
    demonstrate_one_pulse_mode();
    
    printf("=== Key Points Summary ===\n");
    printf("1. Enable timer clock in RCC\n");
    printf("2. Configure prescaler and period (ARR)\n");
    printf("3. Enable update interrupt in DIER\n");
    printf("4. Configure and enable NVIC\n");
    printf("5. Start timer by setting CEN bit\n");
    printf("6. Clear SR flag in ISR\n");
    printf("7. Choose appropriate prescaler/period for target frequency\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

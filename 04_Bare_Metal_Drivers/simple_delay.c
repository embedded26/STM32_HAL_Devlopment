/*
 * simple_delay.c
 *
 * Demonstrates software delay implementations for bare-metal systems
 * Learning objectives:
 * - Software delay loops
 * - Cycle counting basics
 * - DWT (Data Watchpoint and Trace) for precise delays
 * - Timer-based delays
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* DWT (Data Watchpoint and Trace) Registers */
#define DWT_CTRL    (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT  (*(volatile uint32_t*)0xE0001004)
#define DEM_CR      (*(volatile uint32_t*)0xE000EDFC)

#define DEM_CR_TRCENA   (1 << 24)
#define DWT_CTRL_CYCCNTENA  (1 << 0)

void demonstrate_delay_concepts(void)
{
    printf("=== Delay Implementation Methods ===\n\n");
    
    printf("1. Simple Loop Delay\n");
    printf("   Pros: No hardware needed, simple\n");
    printf("   Cons: Inaccurate, depends on compiler optimization\n\n");
    
    printf("2. DWT Cycle Counter Delay\n");
    printf("   Pros: Precise, uses CPU cycle counter\n");
    printf("   Cons: Cortex-M only, uses debug hardware\n\n");
    
    printf("3. Timer-based Delay\n");
    printf("   Pros: Precise, can use interrupts\n");
    printf("   Cons: Requires timer peripheral\n\n");
    
    printf("4. Systick Delay\n");
    printf("   Pros: Standard ARM feature, good for RTOS\n");
    printf("   Cons: Interrupts, can be complex\n\n");
}

void delay_cycles_simple(uint32_t cycles)
{
    /*
     * Simple delay loop
     * Note: Actual cycle count depends on optimization level
     * At -O0: ~4 cycles per iteration
     * At -O2: May be optimized away!
     */
    for (volatile uint32_t i = 0; i < cycles; i++) {
        __asm volatile ("nop");
    }
}

void delay_ms_simple(uint32_t milliseconds)
{
    /*
     * Approximate millisecond delay
     * Assumes 16 MHz clock (HSI default)
     * Very rough estimation!
     */
    uint32_t cycles = milliseconds * 4000;  // Rough estimate
    delay_cycles_simple(cycles);
}

void delay_us_simple(uint32_t microseconds)
{
    /*
     * Approximate microsecond delay
     * Assumes 16 MHz clock
     */
    uint32_t cycles = microseconds * 4;  // Rough estimate
    delay_cycles_simple(cycles);
}

void DWT_Init(void)
{
    printf("=== Initializing DWT Cycle Counter ===\n");
    
    /* Enable TRC */
    printf("1. Enabling trace unit...\n");
    DEM_CR |= DEM_CR_TRCENA;
    
    /* Reset cycle counter */
    printf("2. Resetting cycle counter...\n");
    DWT_CYCCNT = 0;
    
    /* Enable cycle counter */
    printf("3. Enabling cycle counter...\n");
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;
    
    printf("   DWT initialized, CYCCNT = %lu\n\n", (unsigned long)DWT_CYCCNT);
}

void delay_cycles_precise(uint32_t cycles)
{
    uint32_t start = DWT_CYCCNT;
    while ((DWT_CYCCNT - start) < cycles);
}

void delay_us_precise(uint32_t microseconds)
{
    /*
     * Precise microsecond delay using DWT
     * Assumes 16 MHz clock (adjust for your system)
     */
    uint32_t cycles = microseconds * 16;  // 16 cycles per us at 16 MHz
    delay_cycles_precise(cycles);
}

void delay_ms_precise(uint32_t milliseconds)
{
    /*
     * Precise millisecond delay using DWT
     * Assumes 16 MHz clock
     */
    uint32_t cycles = milliseconds * 16000;  // 16000 cycles per ms at 16 MHz
    delay_cycles_precise(cycles);
}

void demonstrate_simple_delays(void)
{
    printf("=== Simple Loop Delays ===\n\n");
    
    printf("Delaying 1 second (simple loop)...\n");
    uint32_t start = DWT_CYCCNT;
    delay_ms_simple(1000);
    uint32_t end = DWT_CYCCNT;
    uint32_t actual_cycles = end - start;
    
    printf("Expected cycles: ~16,000,000 (at 16 MHz)\n");
    printf("Actual cycles: %lu\n", (unsigned long)actual_cycles);
    printf("Difference: %ld cycles\n\n", 
           (long)(actual_cycles - 16000000));
}

void demonstrate_precise_delays(void)
{
    printf("=== Precise DWT-based Delays ===\n\n");
    
    printf("Test 1: 1ms delay\n");
    uint32_t start = DWT_CYCCNT;
    delay_ms_precise(1);
    uint32_t end = DWT_CYCCNT;
    printf("  Cycles: %lu (Expected: 16000)\n", (unsigned long)(end - start));
    
    printf("\nTest 2: 100us delay\n");
    start = DWT_CYCCNT;
    delay_us_precise(100);
    end = DWT_CYCCNT;
    printf("  Cycles: %lu (Expected: 1600)\n", (unsigned long)(end - start));
    
    printf("\nTest 3: 10us delay\n");
    start = DWT_CYCCNT;
    delay_us_precise(10);
    end = DWT_CYCCNT;
    printf("  Cycles: %lu (Expected: 160)\n\n", (unsigned long)(end - start));
}

void demonstrate_timer_delay(void)
{
    printf("=== Timer-based Delay (Polling) ===\n\n");
    
    /* Enable TIM2 clock */
    RCC->APB1ENR |= (1 << 0);
    
    /* Configure TIM2 for microsecond timebase */
    /* Assuming APB1 = 42 MHz */
    /* Prescaler = 41 gives 1 MHz (1us per tick) */
    
    TIM_RegDef_t *TIM2 = (TIM_RegDef_t*)0x40000000;
    
    printf("Configuring TIM2 for microsecond delays...\n");
    TIM2->PSC = 41;  // 42 MHz / 42 = 1 MHz = 1us
    TIM2->ARR = 0xFFFFFFFF;  // Maximum period
    TIM2->CR1 |= (1 << 0);  // Enable counter
    
    printf("Timer configured\n\n");
    
    /* Delay 1000us using timer */
    printf("Delaying 1000us using TIM2...\n");
    uint32_t start_cnt = TIM2->CNT;
    while ((TIM2->CNT - start_cnt) < 1000);
    uint32_t end_cnt = TIM2->CNT;
    
    printf("Timer count: %lu (Expected: 1000)\n\n", 
           (unsigned long)(end_cnt - start_cnt));
}

void example_led_blink_delays(void)
{
    printf("=== LED Blink with Different Delays ===\n\n");
    
    /* Configure PA5 as output */
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~(0x3 << 10);
    GPIOA->MODER |= (0x1 << 10);
    
    printf("Blinking LED with precise delays...\n");
    
    for (int i = 0; i < 5; i++) {
        printf("  Blink %d\n", i + 1);
        
        GPIOA->ODR |= (1 << 5);  // LED ON
        delay_ms_precise(500);
        
        GPIOA->ODR &= ~(1 << 5);  // LED OFF
        delay_ms_precise(500);
    }
    
    printf("LED blink complete\n\n");
}

void benchmark_delays(void)
{
    printf("=== Delay Benchmark ===\n\n");
    
    printf("Benchmarking 1ms delay methods:\n\n");
    
    /* Simple loop */
    printf("1. Simple loop delay:\n");
    uint32_t start = DWT_CYCCNT;
    delay_ms_simple(1);
    uint32_t cycles_simple = DWT_CYCCNT - start;
    printf("   Cycles: %lu\n", (unsigned long)cycles_simple);
    printf("   Accuracy: %.2f%%\n\n", 
           (float)cycles_simple / 16000.0f);
    
    /* DWT delay */
    printf("2. DWT-based delay:\n");
    start = DWT_CYCCNT;
    delay_ms_precise(1);
    uint32_t cycles_dwt = DWT_CYCCNT - start;
    printf("   Cycles: %lu\n", (unsigned long)cycles_dwt);
    printf("   Accuracy: %.2f%%\n\n",
           (float)cycles_dwt / 16000.0f);
    
    printf("DWT delay is %lu cycles more accurate\n",
           (unsigned long)(cycles_simple - cycles_dwt));
}

void demonstrate_non_blocking_delay(void)
{
    printf("=== Non-blocking Delay Pattern ===\n\n");
    
    printf("Problem: Blocking delays prevent other operations\n");
    printf("Solution: State machine with time tracking\n\n");
    
    printf("Example Pattern:\n");
    printf("uint32_t last_time = 0;\n");
    printf("enum { IDLE, LED_ON, LED_OFF } state = IDLE;\n\n");
    
    printf("void loop() {\n");
    printf("    uint32_t current_time = GetTick();\n");
    printf("    \n");
    printf("    switch(state) {\n");
    printf("        case LED_ON:\n");
    printf("            if (current_time - last_time >= 500) {\n");
    printf("                LED_OFF();\n");
    printf("                state = LED_OFF;\n");
    printf("                last_time = current_time;\n");
    printf("            }\n");
    printf("            break;\n");
    printf("        // ...\n");
    printf("    }\n");
    printf("}\n\n");
}

int main(void)
{
    printf("=== Software Delay Examples ===\n\n");
    
    demonstrate_delay_concepts();
    
    DWT_Init();
    
    demonstrate_simple_delays();
    
    demonstrate_precise_delays();
    
    demonstrate_timer_delay();
    
    example_led_blink_delays();
    
    benchmark_delays();
    
    demonstrate_non_blocking_delay();
    
    printf("=== Key Takeaways ===\n");
    printf("1. Simple loops are inaccurate and compiler-dependent\n");
    printf("2. DWT cycle counter provides precise delays\n");
    printf("3. Timer-based delays are versatile\n");
    printf("4. Always prefer non-blocking delays in production\n");
    printf("5. Calibrate delays for your specific clock frequency\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

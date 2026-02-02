/*
 * profiling_example.c
 *
 * Demonstrates performance profiling and optimization techniques
 * Learning objectives:
 * - Measuring execution time
 * - Cycle counting
 * - Identifying bottlenecks
 * - Optimization validation
 */

#include "../drivers/inc/debug_utils.h"
#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Global error tracker */
DebugErrorTracker_t g_debug_error_tracker = {0};

/* DWT Registers */
#define DWT_CTRL    (*(volatile uint32_t*)0xE0001000)
#define DWT_CYCCNT  (*(volatile uint32_t*)0xE0001004)
#define DEM_CR      (*(volatile uint32_t*)0xE000EDFC)

void DWT_Init(void)
{
    DEM_CR |= (1 << 24);        // Enable trace
    DWT_CYCCNT = 0;             // Reset counter
    DWT_CTRL |= (1 << 0);       // Enable counter
}

void profile_simple_loop(void)
{
    printf("=== Profiling Simple Loop ===\n\n");
    
    volatile uint32_t sum = 0;
    
    uint32_t start = DWT_CYCCNT;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    uint32_t end = DWT_CYCCNT;
    
    uint32_t cycles = end - start;
    printf("Loop iterations: 1000\n");
    printf("Total cycles: %lu\n", (unsigned long)cycles);
    printf("Cycles per iteration: %.2f\n\n", (float)cycles / 1000.0f);
}

void profile_register_access(void)
{
    printf("=== Profiling Register Access ===\n\n");
    
    RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA
    
    /* Profile single read */
    uint32_t start = DWT_CYCCNT;
    volatile uint32_t value = GPIOA->IDR;
    uint32_t end = DWT_CYCCNT;
    
    printf("Single register read: %lu cycles\n", (unsigned long)(end - start));
    
    /* Profile multiple reads */
    start = DWT_CYCCNT;
    for (int i = 0; i < 100; i++) {
        value = GPIOA->IDR;
    }
    end = DWT_CYCCNT;
    
    printf("100 register reads: %lu cycles\n", (unsigned long)(end - start));
    printf("Average per read: %.2f cycles\n\n", (float)(end - start) / 100.0f);
    
    /* Profile write */
    start = DWT_CYCCNT;
    GPIOA->ODR = 0x1234;
    end = DWT_CYCCNT;
    
    printf("Single register write: %lu cycles\n\n", (unsigned long)(end - start));
}

void profile_arithmetic_operations(void)
{
    printf("=== Profiling Arithmetic Operations ===\n\n");
    
    volatile uint32_t a = 12345;
    volatile uint32_t b = 67890;
    volatile uint32_t result;
    
    /* Addition */
    uint32_t start = DWT_CYCCNT;
    result = a + b;
    uint32_t end = DWT_CYCCNT;
    printf("Addition: %lu cycles\n", (unsigned long)(end - start));
    
    /* Multiplication */
    start = DWT_CYCCNT;
    result = a * b;
    end = DWT_CYCCNT;
    printf("Multiplication: %lu cycles\n", (unsigned long)(end - start));
    
    /* Division */
    start = DWT_CYCCNT;
    result = b / a;
    end = DWT_CYCCNT;
    printf("Division: %lu cycles\n", (unsigned long)(end - start));
    
    /* Modulo */
    start = DWT_CYCCNT;
    result = b % a;
    end = DWT_CYCCNT;
    printf("Modulo: %lu cycles\n\n", (unsigned long)(end - start));
}

void profile_array_operations(void)
{
    printf("=== Profiling Array Operations ===\n\n");
    
    uint8_t array[100];
    
    /* Sequential write */
    uint32_t start = DWT_CYCCNT;
    for (int i = 0; i < 100; i++) {
        array[i] = i;
    }
    uint32_t end = DWT_CYCCNT;
    printf("Sequential write (100 bytes): %lu cycles\n", 
           (unsigned long)(end - start));
    
    /* Sequential read */
    volatile uint8_t temp;
    start = DWT_CYCCNT;
    for (int i = 0; i < 100; i++) {
        temp = array[i];
    }
    end = DWT_CYCCNT;
    printf("Sequential read (100 bytes): %lu cycles\n",
           (unsigned long)(end - start));
    
    /* Random access */
    start = DWT_CYCCNT;
    for (int i = 0; i < 100; i++) {
        temp = array[(i * 7) % 100];
    }
    end = DWT_CYCCNT;
    printf("Random access (100 reads): %lu cycles\n\n",
           (unsigned long)(end - start));
}

void profile_function_call_overhead(void)
{
    printf("=== Function Call Overhead ===\n\n");
    
    /* Empty function */
    volatile uint32_t dummy;
    
    auto void empty_func(void) {
        dummy = 0;
    }
    
    /* Measure call overhead */
    uint32_t start = DWT_CYCCNT;
    empty_func();
    uint32_t end = DWT_CYCCNT;
    
    printf("Function call overhead: %lu cycles\n", (unsigned long)(end - start));
    
    /* Inline comparison */
    start = DWT_CYCCNT;
    dummy = 0;
    end = DWT_CYCCNT;
    
    printf("Inline operation: %lu cycles\n", (unsigned long)(end - start));
    printf("Overhead: %lu cycles\n\n", (unsigned long)(end - start));
}

void profile_optimization_levels(void)
{
    printf("=== Optimization Impact Demo ===\n\n");
    
    printf("Compile with different optimization levels:\n");
    printf("  -O0: No optimization\n");
    printf("  -O1: Basic optimization\n");
    printf("  -O2: Aggressive optimization\n");
    printf("  -O3: Maximum optimization\n");
    printf("  -Os: Size optimization\n\n");
    
    volatile uint32_t sum = 0;
    
    uint32_t start = DWT_CYCCNT;
    for (int i = 0; i < 10000; i++) {
        sum += i * 2;
    }
    uint32_t end = DWT_CYCCNT;
    
    printf("Test loop (10000 iterations): %lu cycles\n", 
           (unsigned long)(end - start));
    printf("Result: %lu\n\n", (unsigned long)sum);
}

void compare_implementations(void)
{
    printf("=== Comparing Implementations ===\n\n");
    
    uint8_t buffer[64];
    
    /* Method 1: Loop-based clear */
    printf("Method 1: Loop-based clear\n");
    uint32_t start = DWT_CYCCNT;
    for (int i = 0; i < 64; i++) {
        buffer[i] = 0;
    }
    uint32_t cycles1 = DWT_CYCCNT - start;
    printf("  Cycles: %lu\n", (unsigned long)cycles1);
    
    /* Method 2: memset */
    printf("\nMethod 2: memset\n");
    start = DWT_CYCCNT;
    for (int i = 0; i < 64; i++) {
        buffer[i] = 0;
    }
    // In real code: memset(buffer, 0, 64);
    uint32_t cycles2 = DWT_CYCCNT - start;
    printf("  Cycles: %lu\n", (unsigned long)cycles2);
    
    /* Method 3: Word-wise clear */
    printf("\nMethod 3: Word-wise clear (32-bit)\n");
    uint32_t *word_ptr = (uint32_t*)buffer;
    start = DWT_CYCCNT;
    for (int i = 0; i < 16; i++) {
        word_ptr[i] = 0;
    }
    uint32_t cycles3 = DWT_CYCCNT - start;
    printf("  Cycles: %lu\n", (unsigned long)cycles3);
    
    printf("\nComparison:\n");
    printf("  Loop: %lu cycles (baseline)\n", (unsigned long)cycles1);
    printf("  Memset: %lu cycles (%.1f%% of baseline)\n", 
           (unsigned long)cycles2, 100.0f * cycles2 / cycles1);
    printf("  Word-wise: %lu cycles (%.1f%% of baseline)\n\n",
           (unsigned long)cycles3, 100.0f * cycles3 / cycles1);
}

void profile_gpio_operations(void)
{
    printf("=== Profiling GPIO Operations ===\n\n");
    
    RCC->AHB1ENR |= (1 << 0);
    GPIOA->MODER &= ~(0x3 << 10);
    GPIOA->MODER |= (0x1 << 10);
    
    /* ODR write */
    uint32_t start = DWT_CYCCNT;
    GPIOA->ODR |= (1 << 5);
    uint32_t end = DWT_CYCCNT;
    printf("ODR set bit: %lu cycles\n", (unsigned long)(end - start));
    
    /* BSRR write (atomic) */
    start = DWT_CYCCNT;
    GPIOA->BSRRL = (1 << 5);
    end = DWT_CYCCNT;
    printf("BSRR set bit: %lu cycles\n", (unsigned long)(end - start));
    
    /* Toggle via XOR */
    start = DWT_CYCCNT;
    GPIOA->ODR ^= (1 << 5);
    end = DWT_CYCCNT;
    printf("ODR toggle (XOR): %lu cycles\n\n", (unsigned long)(end - start));
}

void demonstrate_profiling_workflow(void)
{
    printf("=== Profiling Workflow ===\n\n");
    
    printf("Step 1: Identify bottleneck\n");
    printf("  - Profile entire application\n");
    printf("  - Measure time spent in each function\n");
    printf("  - Focus on hot paths\n\n");
    
    printf("Step 2: Measure baseline\n");
    printf("  - Profile current implementation\n");
    printf("  - Record cycle count\n");
    printf("  - Document conditions\n\n");
    
    printf("Step 3: Optimize\n");
    printf("  - Try different approaches\n");
    printf("  - Consider trade-offs (speed vs size)\n");
    printf("  - Test edge cases\n\n");
    
    printf("Step 4: Validate\n");
    printf("  - Re-profile optimized code\n");
    printf("  - Compare with baseline\n");
    printf("  - Verify correctness\n\n");
    
    printf("Step 5: Document\n");
    printf("  - Record improvement percentage\n");
    printf("  - Note optimization technique used\n");
    printf("  - Explain any trade-offs\n\n");
}

int main(void)
{
    printf("=== Performance Profiling Example ===\n\n");
    
    printf("Initializing DWT cycle counter...\n");
    DWT_Init();
    printf("DWT initialized\n\n");
    
    profile_simple_loop();
    
    profile_register_access();
    
    profile_arithmetic_operations();
    
    profile_array_operations();
    
    profile_function_call_overhead();
    
    profile_optimization_levels();
    
    compare_implementations();
    
    profile_gpio_operations();
    
    demonstrate_profiling_workflow();
    
    printf("=== Profiling Best Practices ===\n");
    printf("1. Use DWT cycle counter for precise measurements\n");
    printf("2. Profile real workloads, not synthetic tests\n");
    printf("3. Measure multiple times and average\n");
    printf("4. Account for cache and pipeline effects\n");
    printf("5. Profile both typical and worst-case scenarios\n");
    printf("6. Consider power consumption trade-offs\n");
    printf("7. Document optimization decisions\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

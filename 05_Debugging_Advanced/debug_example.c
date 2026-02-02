/*
 * debug_example.c
 *
 * Demonstrates using debug_utils.h macros and functions
 * Learning objectives:
 * - Debug logging at different levels
 * - Memory dump utilities
 * - Register inspection
 * - Performance profiling
 */

#include "../drivers/inc/debug_utils.h"
#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* Global error tracker (required by debug_utils.h) */
DebugErrorTracker_t g_debug_error_tracker = {0};

void example_debug_levels(void)
{
    printf("=== Debug Logging Levels Example ===\n\n");
    
    printf("Current DEBUG_LEVEL: %d\n", DEBUG_LEVEL);
    printf("Levels: ERROR=1, WARN=2, INFO=3, DEBUG=4, TRACE=5\n\n");
    
    /* Try different log levels */
    DEBUG_ERROR("This is an error message");
    DEBUG_WARN("This is a warning message");
    DEBUG_INFO("This is an info message");
    DEBUG_LOG("This is a debug message");
    DEBUG_TRACE("This is a trace message");
    
    printf("\nNote: Messages only show if DEBUG_LEVEL is high enough\n");
    printf("Compile with -DDEBUG_LEVEL=5 to see all messages\n\n");
}

void example_assertions(void)
{
    printf("=== Assertions Example ===\n\n");
    
    printf("Assertions help catch bugs during development\n");
    printf("Compile with -DDEBUG_ENABLE_ASSERT to enable\n\n");
    
    uint8_t buffer_size = 10;
    uint8_t index = 5;
    
    DEBUG_INFO("Checking array bounds...");
    // DEBUG_ASSERT(index < buffer_size);  // Would pass
    
    printf("Safe access: index %d < size %d\n", index, buffer_size);
    
    printf("\nExample of failed assertion:\n");
    printf("DEBUG_ASSERT(index >= buffer_size);\n");
    printf("Would print: Assertion failed: index >= buffer_size\n");
    printf("Then halt execution in infinite loop\n\n");
}

void example_register_dump(void)
{
    printf("=== Register Dump Example ===\n\n");
    
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= (1 << 0);
    
    /* Configure PA5 as output */
    GPIOA->MODER &= ~(0x3 << 10);
    GPIOA->MODER |= (0x1 << 10);
    
    /* Dump GPIO registers */
    printf("GPIOA Configuration:\n");
    Debug_DumpRegister(&(GPIOA->MODER), "MODER");
    Debug_DumpRegister(&(GPIOA->OTYPER), "OTYPER");
    Debug_DumpRegister(&(GPIOA->OSPEEDR), "OSPEEDR");
    Debug_DumpRegister(&(GPIOA->PUPDR), "PUPDR");
    Debug_DumpRegister(&(GPIOA->IDR), "IDR");
    Debug_DumpRegister(&(GPIOA->ODR), "ODR");
    
    printf("\nRCC Configuration:\n");
    Debug_DumpRegister(&(RCC->CR), "RCC_CR");
    Debug_DumpRegister(&(RCC->CFGR), "RCC_CFGR");
    Debug_DumpRegister(&(RCC->AHB1ENR), "RCC_AHB1ENR");
}

void example_multiple_registers(void)
{
    printf("\n=== Multiple Register Dump ===\n\n");
    
    const char *gpio_reg_names[] = {
        "MODER", "OTYPER", "OSPEEDR", "PUPDR",
        "IDR", "ODR", "BSRRL", "BSRRH"
    };
    
    Debug_DumpRegisters((volatile uint32_t*)GPIOA, gpio_reg_names, 8);
}

void example_memory_dump(void)
{
    printf("\n=== Memory Dump Example ===\n\n");
    
    /* Create test data */
    uint8_t test_buffer[64];
    for (int i = 0; i < 64; i++) {
        test_buffer[i] = i;
    }
    
    /* Dump memory */
    Debug_DumpMemory(test_buffer, 64, "Test Buffer");
    
    /* Dump with ASCII */
    const char *test_string = "Hello, STM32! This is a test string for memory dump.";
    Debug_DumpMemory(test_string, 54, "Test String");
    
    /* Dump peripheral memory */
    Debug_DumpMemory((void*)GPIOA_BASEADDR, 32, "GPIOA Registers");
}

void example_profiling(void)
{
    printf("\n=== Performance Profiling Example ===\n\n");
    
    DebugProfiler_t profiler;
    
    /* Profile simple operation */
    printf("Profiling simple loop...\n");
    Debug_ProfileStart(&profiler, "Simple Loop");
    
    volatile uint32_t sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    
    Debug_ProfileStop(&profiler);
    
    /* Profile register access */
    printf("\nProfiling register access...\n");
    Debug_ProfileStart(&profiler, "Register Read");
    
    volatile uint32_t reg_value;
    for (int i = 0; i < 100; i++) {
        reg_value = GPIOA->IDR;
    }
    
    Debug_ProfileStop(&profiler);
    
    printf("\nNote: Profiling requires DWT cycle counter\n");
    printf("Implement DWT->CYCCNT reading in debug_utils.h\n");
}

void example_bit_manipulation(void)
{
    printf("\n=== Bit Manipulation Helpers ===\n\n");
    
    uint32_t test_reg = 0x0000;
    
    printf("Initial value: 0x%08lX\n", (unsigned long)test_reg);
    
    BIT_SET(test_reg, 5);
    printf("After BIT_SET(5): 0x%08lX\n", (unsigned long)test_reg);
    
    BIT_SET(test_reg, 10);
    printf("After BIT_SET(10): 0x%08lX\n", (unsigned long)test_reg);
    
    printf("BIT_READ(5): %d\n", BIT_READ(test_reg, 5));
    printf("BIT_READ(7): %d\n", BIT_READ(test_reg, 7));
    
    BIT_TOGGLE(test_reg, 5);
    printf("After BIT_TOGGLE(5): 0x%08lX\n", (unsigned long)test_reg);
    
    BIT_CLEAR(test_reg, 10);
    printf("After BIT_CLEAR(10): 0x%08lX\n\n", (unsigned long)test_reg);
}

void example_error_tracking(void)
{
    printf("=== Error Tracking Example ===\n\n");
    
    /* Simulate errors */
    DEBUG_ERROR("First error occurred");
    Debug_RecordError(__FILE__, __LINE__);
    
    DEBUG_WARN("First warning");
    Debug_RecordWarning();
    
    DEBUG_ERROR("Second error occurred");
    Debug_RecordError(__FILE__, __LINE__);
    
    DEBUG_WARN("Second warning");
    Debug_RecordWarning();
    
    DEBUG_WARN("Third warning");
    Debug_RecordWarning();
    
    /* Print statistics */
    Debug_PrintErrorStats();
}

void example_conditional_debug(void)
{
    printf("=== Conditional Debug Output ===\n\n");
    
    uint8_t sensor_value = 42;
    uint8_t threshold = 50;
    
    printf("Reading sensor value: %d\n", sensor_value);
    
    if (sensor_value > threshold) {
        DEBUG_WARN("Sensor value exceeds threshold!");
    } else {
        DEBUG_INFO("Sensor value within normal range");
    }
    
    /* Debug only in development */
#ifdef DEBUG_MODE
    printf("DEBUG_MODE is enabled\n");
    printf("Detailed diagnostics:\n");
    printf("  Sensor: %d\n", sensor_value);
    printf("  Threshold: %d\n", threshold);
    printf("  Margin: %d\n", threshold - sensor_value);
#endif
}

void example_debug_in_driver(void)
{
    printf("\n=== Debug in Driver Functions ===\n\n");
    
    printf("Example of debug in GPIO driver:\n\n");
    
    printf("void GPIO_Init(GPIO_Handle_t *pGPIOHandle)\n");
    printf("{\n");
    printf("    DEBUG_TRACE(\"Initializing GPIO\");\n");
    printf("    \n");
    printf("    if (!pGPIOHandle) {\n");
    printf("        DEBUG_ERROR(\"NULL pointer passed to GPIO_Init\");\n");
    printf("        return;\n");
    printf("    }\n");
    printf("    \n");
    printf("    DEBUG_LOG(\"Port: 0x%%08X, Pin: %%d\",\n");
    printf("              pGPIOHandle->pGPIOx,\n");
    printf("              pGPIOHandle->GPIO_PINConfig.GPIO_PinNumber);\n");
    printf("    \n");
    printf("    // Configuration code...\n");
    printf("    \n");
    printf("    DEBUG_INFO(\"GPIO initialized successfully\");\n");
    printf("}\n\n");
}

int main(void)
{
    printf("=== Debug Utilities Example ===\n\n");
    
    example_debug_levels();
    
    example_assertions();
    
    example_register_dump();
    
    example_multiple_registers();
    
    example_memory_dump();
    
    example_profiling();
    
    example_bit_manipulation();
    
    example_error_tracking();
    
    example_conditional_debug();
    
    example_debug_in_driver();
    
    printf("=== Key Debugging Practices ===\n");
    printf("1. Use appropriate debug levels (ERROR > WARN > INFO > DEBUG > TRACE)\n");
    printf("2. Enable assertions during development\n");
    printf("3. Dump registers when debugging hardware issues\n");
    printf("4. Use memory dumps for buffer inspection\n");
    printf("5. Profile performance-critical sections\n");
    printf("6. Track errors for diagnostics\n");
    printf("7. Remove or disable verbose debug in production\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

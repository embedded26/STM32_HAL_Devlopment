/*
 * memory_dump_example.c
 *
 * Demonstrates memory inspection and analysis techniques
 * Learning objectives:
 * - Examining memory regions
 * - Stack and heap analysis
 * - Peripheral register inspection
 * - Memory corruption detection
 */

#include "../drivers/inc/debug_utils.h"
#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>
#include <string.h>

/* Global error tracker */
DebugErrorTracker_t g_debug_error_tracker = {0};

/* Test data structures */
typedef struct {
    uint32_t id;
    uint8_t status;
    uint8_t flags;
    uint16_t counter;
    char name[16];
} DeviceInfo_t;

void inspect_memory_regions(void)
{
    printf("=== STM32F446RE Memory Regions ===\n\n");
    
    printf("Memory Map:\n");
    printf("FLASH:  0x08000000 - 0x0807FFFF (512 KB)\n");
    printf("SRAM1:  0x20000000 - 0x2001BFFF (112 KB)\n");
    printf("SRAM2:  0x2001C000 - 0x2001FFFF (16 KB)\n");
    printf("Periph: 0x40000000 - 0x5FFFFFFF\n\n");
    
    /* Inspect various memory regions */
    printf("Sample Memory Regions:\n\n");
    
    /* Flash memory */
    printf("Flash memory (first 64 bytes):\n");
    Debug_DumpMemory((void*)0x08000000, 64, "Flash Start");
    
    /* SRAM */
    printf("SRAM (stack area sample):\n");
    uint32_t stack_var = 0x12345678;
    Debug_DumpMemory(&stack_var, 32, "Stack Variable");
}

void inspect_structure_layout(void)
{
    printf("\n=== Structure Memory Layout ===\n\n");
    
    DeviceInfo_t device = {
        .id = 0xABCD1234,
        .status = 0x5A,
        .flags = 0xF0,
        .counter = 0x9876,
        .name = "STM32F446RE"
    };
    
    printf("DeviceInfo_t structure:\n");
    printf("Size: %zu bytes\n\n", sizeof(DeviceInfo_t));
    
    printf("Field offsets:\n");
    printf("id:      offset %zu\n", offsetof(DeviceInfo_t, id));
    printf("status:  offset %zu\n", offsetof(DeviceInfo_t, status));
    printf("flags:   offset %zu\n", offsetof(DeviceInfo_t, flags));
    printf("counter: offset %zu\n", offsetof(DeviceInfo_t, counter));
    printf("name:    offset %zu\n\n", offsetof(DeviceInfo_t, name));
    
    Debug_DumpMemory(&device, sizeof(device), "DeviceInfo Structure");
}

void inspect_peripheral_memory(void)
{
    printf("\n=== Peripheral Memory Inspection ===\n\n");
    
    /* Enable clocks */
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOA, GPIOB
    
    printf("GPIOA Registers:\n");
    Debug_DumpMemory((void*)GPIOA_BASEADDR, 64, "GPIOA");
    
    printf("\nGPIOB Registers:\n");
    Debug_DumpMemory((void*)GPIOB_BASEADDR, 64, "GPIOB");
    
    printf("\nRCC Registers (first 64 bytes):\n");
    Debug_DumpMemory((void*)RCC_BASEADDR, 64, "RCC");
}

void inspect_array_contents(void)
{
    printf("\n=== Array Contents Inspection ===\n\n");
    
    /* Integer array */
    uint32_t numbers[] = {0x11111111, 0x22222222, 0x33333333, 0x44444444,
                          0x55555555, 0x66666666, 0x77777777, 0x88888888};
    
    Debug_DumpMemory(numbers, sizeof(numbers), "Integer Array");
    
    /* Byte array */
    uint8_t pattern[32];
    for (int i = 0; i < 32; i++) {
        pattern[i] = i * 8;
    }
    
    Debug_DumpMemory(pattern, sizeof(pattern), "Byte Pattern");
    
    /* String data */
    const char *message = "STM32 Memory Dump Example!";
    Debug_DumpMemory(message, strlen(message) + 1, "String Data");
}

void detect_memory_corruption(void)
{
    printf("\n=== Memory Corruption Detection ===\n\n");
    
    uint8_t buffer[16];
    uint32_t canary_before = 0xDEADBEEF;
    uint32_t canary_after = 0xCAFEBABE;
    
    /* Initialize buffer */
    for (int i = 0; i < 16; i++) {
        buffer[i] = i;
    }
    
    printf("Memory layout with canaries:\n");
    printf("Canary Before: 0x%08lX\n", (unsigned long)canary_before);
    printf("Buffer address: %p\n", (void*)buffer);
    printf("Canary After: 0x%08lX\n\n", (unsigned long)canary_after);
    
    /* Dump memory region including canaries */
    uint8_t *start = (uint8_t*)&canary_before;
    size_t total_size = sizeof(canary_before) + sizeof(buffer) + sizeof(canary_after);
    Debug_DumpMemory(start, total_size, "Protected Buffer");
    
    /* Check canaries */
    if (canary_before == 0xDEADBEEF && canary_after == 0xCAFEBABE) {
        printf("\nCanaries intact - No corruption detected\n");
    } else {
        printf("\nWARNING: Canary corruption detected!\n");
        printf("Before: expected 0xDEADBEEF, got 0x%08lX\n", 
               (unsigned long)canary_before);
        printf("After: expected 0xCAFEBABE, got 0x%08lX\n",
               (unsigned long)canary_after);
    }
}

void inspect_stack_usage(void)
{
    printf("\n=== Stack Inspection ===\n\n");
    
    /* Create stack variables */
    uint32_t stack_var1 = 0xAAAAAAAA;
    uint32_t stack_var2 = 0xBBBBBBBB;
    uint32_t stack_var3 = 0xCCCCCCCC;
    
    printf("Stack variables:\n");
    printf("var1 @ %p = 0x%08lX\n", (void*)&stack_var1, (unsigned long)stack_var1);
    printf("var2 @ %p = 0x%08lX\n", (void*)&stack_var2, (unsigned long)stack_var2);
    printf("var3 @ %p = 0x%08lX\n\n", (void*)&stack_var3, (unsigned long)stack_var3);
    
    printf("Stack grows downward (higher address to lower)\n");
    printf("Distance between vars: %ld bytes\n\n",
           (long)((uint8_t*)&stack_var1 - (uint8_t*)&stack_var2));
    
    /* Dump stack region */
    Debug_DumpMemory(&stack_var3, 32, "Stack Region");
}

void compare_memory_regions(void)
{
    printf("\n=== Memory Comparison ===\n\n");
    
    uint8_t buffer1[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    uint8_t buffer2[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,99,16};
    
    printf("Buffer 1:\n");
    Debug_DumpMemory(buffer1, 16, "Buffer 1");
    
    printf("\nBuffer 2:\n");
    Debug_DumpMemory(buffer2, 16, "Buffer 2");
    
    printf("\nComparing buffers...\n");
    for (int i = 0; i < 16; i++) {
        if (buffer1[i] != buffer2[i]) {
            printf("Difference at index %d: 0x%02X vs 0x%02X\n",
                   i, buffer1[i], buffer2[i]);
        }
    }
}

void inspect_pointer_validity(void)
{
    printf("\n=== Pointer Validity Check ===\n\n");
    
    uint32_t valid_var = 0x12345678;
    uint32_t *valid_ptr = &valid_var;
    uint32_t *null_ptr = NULL;
    uint32_t *invalid_ptr = (uint32_t*)0xDEADBEEF;
    
    printf("Valid pointer: %p -> 0x%08lX\n", 
           (void*)valid_ptr, (unsigned long)*valid_ptr);
    
    printf("NULL pointer: %p\n", (void*)null_ptr);
    
    printf("Invalid pointer: %p (don't dereference!)\n", (void*)invalid_ptr);
    
    /* Check pointer ranges */
    printf("\nPointer range checks:\n");
    
    if ((uint32_t)valid_ptr >= 0x20000000 && (uint32_t)valid_ptr < 0x20020000) {
        printf("Valid pointer is in SRAM range ✓\n");
    }
    
    if (null_ptr == NULL) {
        printf("NULL pointer check passed ✓\n");
    }
    
    if ((uint32_t)invalid_ptr < 0x08000000 || (uint32_t)invalid_ptr > 0x60000000) {
        printf("Invalid pointer is outside valid memory range ✗\n");
    }
}

void demonstrate_endianness(void)
{
    printf("\n=== Endianness Inspection ===\n\n");
    
    uint32_t value = 0x12345678;
    uint8_t *bytes = (uint8_t*)&value;
    
    printf("32-bit value: 0x%08lX\n", (unsigned long)value);
    printf("Byte layout:\n");
    printf("  bytes[0]: 0x%02X\n", bytes[0]);
    printf("  bytes[1]: 0x%02X\n", bytes[1]);
    printf("  bytes[2]: 0x%02X\n", bytes[2]);
    printf("  bytes[3]: 0x%02X\n\n", bytes[3]);
    
    if (bytes[0] == 0x78) {
        printf("System is LITTLE-ENDIAN (LSB first)\n");
        printf("ARM Cortex-M is little-endian ✓\n");
    } else {
        printf("System is BIG-ENDIAN (MSB first)\n");
    }
    
    Debug_DumpMemory(&value, 4, "32-bit Value in Memory");
}

int main(void)
{
    printf("=== Memory Dump and Inspection Example ===\n\n");
    
    inspect_memory_regions();
    
    inspect_structure_layout();
    
    inspect_peripheral_memory();
    
    inspect_array_contents();
    
    detect_memory_corruption();
    
    inspect_stack_usage();
    
    compare_memory_regions();
    
    inspect_pointer_validity();
    
    demonstrate_endianness();
    
    printf("\n=== Memory Debugging Best Practices ===\n");
    printf("1. Use canaries to detect buffer overruns\n");
    printf("2. Validate pointer ranges before dereferencing\n");
    printf("3. Inspect structures to verify alignment\n");
    printf("4. Check endianness when working with multi-byte data\n");
    printf("5. Monitor stack usage in embedded systems\n");
    printf("6. Verify peripheral register contents\n");
    printf("7. Compare memory before/after operations\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

/*
 * debug_utils.h
 *
 * Debug Macro System and Development Utilities
 * Provides logging, memory inspection, and profiling helpers
 */

#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_

#include <stdint.h>
#include <stdio.h>

/*********************************************************************
 * Debug Configuration
 *********************************************************************/

// Debug levels
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4
#define DEBUG_LEVEL_TRACE   5

// Set current debug level (can be overridden at compile time)
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#endif

/*********************************************************************
 * Debug Macros
 *********************************************************************/

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
#define DEBUG_ERROR(fmt, ...) \
    printf("[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_ERROR(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
#define DEBUG_WARN(fmt, ...) \
    printf("[WARN]  %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_WARN(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
#define DEBUG_INFO(fmt, ...) \
    printf("[INFO]  " fmt "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG
#define DEBUG_LOG(fmt, ...) \
    printf("[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_TRACE
#define DEBUG_TRACE(fmt, ...) \
    printf("[TRACE] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define DEBUG_TRACE(fmt, ...)
#endif

/*********************************************************************
 * Assert Macros
 *********************************************************************/

#ifdef DEBUG_ENABLE_ASSERT
#define DEBUG_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("[ASSERT] %s:%d: Assertion failed: %s\n", \
                   __FILE__, __LINE__, #condition); \
            while(1); /* Halt execution */ \
        } \
    } while(0)
#else
#define DEBUG_ASSERT(condition) ((void)0)
#endif

/*********************************************************************
 * Memory Inspection Utilities
 *********************************************************************/

// Dump memory region in hex format
static inline void Debug_DumpMemory(const void* addr, size_t len, const char* label)
{
    const uint8_t* ptr = (const uint8_t*)addr;
    
    printf("\n=== Memory Dump: %s ===\n", label ? label : "Unknown");
    printf("Address: 0x%08lX, Length: %zu bytes\n", (unsigned long)addr, len);
    printf("Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    printf("-------  -----------------------------------------------\n");
    
    for (size_t i = 0; i < len; i += 16) {
        printf("%08zX ", i);
        
        // Print hex values
        for (size_t j = 0; j < 16 && (i + j) < len; j++) {
            printf("%02X ", ptr[i + j]);
        }
        
        // Pad if last line is incomplete
        for (size_t j = len - i; j < 16 && j >= 0; j++) {
            printf("   ");
        }
        
        // Print ASCII representation
        printf(" |");
        for (size_t j = 0; j < 16 && (i + j) < len; j++) {
            char c = ptr[i + j];
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("|\n");
    }
    printf("\n");
}

// Dump 32-bit register value
static inline void Debug_DumpRegister(volatile uint32_t* reg, const char* name)
{
    printf("[REG] %s @ 0x%08lX = 0x%08lX (%lu)\n",
           name, (unsigned long)reg, (unsigned long)*reg, (unsigned long)*reg);
}

// Dump multiple registers
static inline void Debug_DumpRegisters(volatile uint32_t* base, const char** names, size_t count)
{
    printf("\n=== Register Dump ===\n");
    for (size_t i = 0; i < count; i++) {
        Debug_DumpRegister(base + i, names[i]);
    }
    printf("\n");
}

/*********************************************************************
 * Performance Profiling Utilities
 *********************************************************************/

typedef struct {
    uint32_t start_tick;
    uint32_t end_tick;
    const char* label;
} DebugProfiler_t;

// Start performance measurement
static inline void Debug_ProfileStart(DebugProfiler_t* profiler, const char* label)
{
    profiler->label = label;
    profiler->start_tick = 0; // Replace with actual tick counter (SysTick, DWT, etc.)
    // For real implementation: profiler->start_tick = DWT->CYCCNT;
}

// Stop performance measurement and print results
static inline void Debug_ProfileStop(DebugProfiler_t* profiler)
{
    profiler->end_tick = 0; // Replace with actual tick counter
    // For real implementation: profiler->end_tick = DWT->CYCCNT;
    
    uint32_t cycles = profiler->end_tick - profiler->start_tick;
    printf("[PROF] %s: %lu cycles\n", profiler->label, (unsigned long)cycles);
}

/*********************************************************************
 * Stack Usage Utilities
 *********************************************************************/

// Check stack usage (requires stack painting)
static inline uint32_t Debug_GetStackUsage(uint32_t* stack_bottom, uint32_t stack_size)
{
    uint32_t* ptr = stack_bottom;
    uint32_t used = 0;
    
    // Find first non-pattern value (assumes stack painted with 0xDEADBEEF)
    while (used < stack_size && *ptr == 0xDEADBEEF) {
        ptr++;
        used += 4;
    }
    
    return stack_size - used;
}

/*********************************************************************
 * Bit Manipulation Helpers
 *********************************************************************/

#define BIT_SET(reg, bit)       ((reg) |= (1UL << (bit)))
#define BIT_CLEAR(reg, bit)     ((reg) &= ~(1UL << (bit)))
#define BIT_TOGGLE(reg, bit)    ((reg) ^= (1UL << (bit)))
#define BIT_READ(reg, bit)      (((reg) >> (bit)) & 1UL)
#define BIT_WRITE(reg, bit, val) \
    do { \
        if (val) BIT_SET(reg, bit); \
        else BIT_CLEAR(reg, bit); \
    } while(0)

/*********************************************************************
 * Error Tracking
 *********************************************************************/

typedef struct {
    uint32_t error_count;
    uint32_t warning_count;
    uint32_t last_error_line;
    const char* last_error_file;
} DebugErrorTracker_t;

extern DebugErrorTracker_t g_debug_error_tracker;

static inline void Debug_RecordError(const char* file, int line)
{
    g_debug_error_tracker.error_count++;
    g_debug_error_tracker.last_error_line = line;
    g_debug_error_tracker.last_error_file = file;
}

static inline void Debug_RecordWarning(void)
{
    g_debug_error_tracker.warning_count++;
}

static inline void Debug_PrintErrorStats(void)
{
    printf("\n=== Error Statistics ===\n");
    printf("Total Errors:   %lu\n", (unsigned long)g_debug_error_tracker.error_count);
    printf("Total Warnings: %lu\n", (unsigned long)g_debug_error_tracker.warning_count);
    if (g_debug_error_tracker.error_count > 0) {
        printf("Last Error: %s:%lu\n",
               g_debug_error_tracker.last_error_file,
               (unsigned long)g_debug_error_tracker.last_error_line);
    }
    printf("\n");
}

#endif /* DEBUG_UTILS_H_ */

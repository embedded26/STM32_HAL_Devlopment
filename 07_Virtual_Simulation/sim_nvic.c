/*
 * sim_nvic.c - Virtual NVIC (Nested Vectored Interrupt Controller) Simulator
 * Simulates interrupt priority, pending, and handling for testing
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_IRQ_LINES 240  // Support all STM32 variants (typical range: 82-90 for common STM32F4, up to 240 for larger devices)
#define MAX_PRIORITY 15    // 4-bit priority (0-15, 0 is highest)

// IRQ States
#define IRQ_STATE_INACTIVE 0
#define IRQ_STATE_PENDING  1
#define IRQ_STATE_ACTIVE   2

// Error codes
#define NVIC_ERROR_NONE         0
#define NVIC_ERROR_INVALID_IRQ  1
#define NVIC_ERROR_PRIORITY     2

// Virtual IRQ Line
typedef struct {
    uint8_t enabled;
    uint8_t pending;
    uint8_t active;
    uint8_t priority;
    void (*handler)(void);
    char name[32];
} VirtualIRQ;

// Virtual NVIC state
static VirtualIRQ irq_lines[MAX_IRQ_LINES];
static uint8_t nvic_initialized = 0;
static uint8_t global_irq_enabled = 1;
static uint8_t error_injection_enabled = 0;
static uint8_t last_error = NVIC_ERROR_NONE;

// Initialize the virtual NVIC
void VirtualNVIC_Init(void) {
    if (nvic_initialized) return;
    
    for (int i = 0; i < MAX_IRQ_LINES; i++) {
        irq_lines[i].enabled = 0;
        irq_lines[i].pending = 0;
        irq_lines[i].active = 0;
        irq_lines[i].priority = MAX_PRIORITY;
        irq_lines[i].handler = NULL;
        snprintf(irq_lines[i].name, sizeof(irq_lines[i].name), "IRQ_%d", i);
    }
    
    nvic_initialized = 1;
    global_irq_enabled = 1;
    printf("[VirtualNVIC] Initialized with %d IRQ lines\n", MAX_IRQ_LINES);
}

// Enable error injection
void VirtualNVIC_SetErrorInjection(uint8_t enable) {
    error_injection_enabled = enable;
    printf("[VirtualNVIC] Error injection %s\n", enable ? "ENABLED" : "DISABLED");
}

// Get last error
uint8_t VirtualNVIC_GetLastError(void) {
    return last_error;
}

// Simulate errors
static uint8_t inject_error(void) {
    if (!error_injection_enabled) return 0;
    
    if (rand() % 10 == 0) {
        last_error = (rand() % 2) + 1;
        printf("[VirtualNVIC] ERROR INJECTED: Code %d\n", last_error);
        return 1;
    }
    return 0;
}

// Enable IRQ line
uint8_t VirtualNVIC_EnableIRQ(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        printf("[VirtualNVIC] ERROR: Invalid IRQ number %d\n", irq_num);
        return 0;
    }
    
    if (inject_error()) return 0;
    
    irq_lines[irq_num].enabled = 1;
    printf("[VirtualNVIC] IRQ %d (%s) enabled\n", irq_num, irq_lines[irq_num].name);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Disable IRQ line
uint8_t VirtualNVIC_DisableIRQ(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        return 0;
    }
    
    irq_lines[irq_num].enabled = 0;
    printf("[VirtualNVIC] IRQ %d (%s) disabled\n", irq_num, irq_lines[irq_num].name);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Set IRQ priority
uint8_t VirtualNVIC_SetPriority(uint8_t irq_num, uint8_t priority) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        return 0;
    }
    
    if (priority > MAX_PRIORITY) {
        last_error = NVIC_ERROR_PRIORITY;
        printf("[VirtualNVIC] ERROR: Invalid priority %d (max %d)\n", 
               priority, MAX_PRIORITY);
        return 0;
    }
    
    if (inject_error()) return 0;
    
    irq_lines[irq_num].priority = priority;
    printf("[VirtualNVIC] IRQ %d priority set to %d\n", irq_num, priority);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Get IRQ priority
uint8_t VirtualNVIC_GetPriority(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        return MAX_PRIORITY;
    }
    
    return irq_lines[irq_num].priority;
}

// Set IRQ handler
uint8_t VirtualNVIC_SetHandler(uint8_t irq_num, void (*handler)(void), const char *name) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        return 0;
    }
    
    irq_lines[irq_num].handler = handler;
    if (name != NULL) {
        strncpy(irq_lines[irq_num].name, name, sizeof(irq_lines[irq_num].name) - 1);
    }
    
    printf("[VirtualNVIC] Handler registered for IRQ %d (%s)\n", 
           irq_num, irq_lines[irq_num].name);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Set pending flag for IRQ
uint8_t VirtualNVIC_SetPending(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        return 0;
    }
    
    if (inject_error()) return 0;
    
    irq_lines[irq_num].pending = 1;
    printf("[VirtualNVIC] IRQ %d (%s) set to PENDING\n", 
           irq_num, irq_lines[irq_num].name);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Clear pending flag for IRQ
uint8_t VirtualNVIC_ClearPending(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        last_error = NVIC_ERROR_INVALID_IRQ;
        return 0;
    }
    
    irq_lines[irq_num].pending = 0;
    printf("[VirtualNVIC] IRQ %d (%s) pending cleared\n", 
           irq_num, irq_lines[irq_num].name);
    
    last_error = NVIC_ERROR_NONE;
    return 1;
}

// Check if IRQ is pending
uint8_t VirtualNVIC_IsPending(uint8_t irq_num) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (irq_num >= MAX_IRQ_LINES) {
        return 0;
    }
    
    return irq_lines[irq_num].pending;
}

// Enable all interrupts globally
void VirtualNVIC_EnableGlobalIRQ(void) {
    global_irq_enabled = 1;
    printf("[VirtualNVIC] Global interrupts ENABLED\n");
}

// Disable all interrupts globally
void VirtualNVIC_DisableGlobalIRQ(void) {
    global_irq_enabled = 0;
    printf("[VirtualNVIC] Global interrupts DISABLED\n");
}

// Find highest priority pending interrupt
static int find_highest_priority_pending(void) {
    int highest_irq = -1;
    uint8_t highest_priority = MAX_PRIORITY + 1;
    
    for (int i = 0; i < MAX_IRQ_LINES; i++) {
        if (irq_lines[i].enabled && irq_lines[i].pending && 
            !irq_lines[i].active) {
            if (irq_lines[i].priority < highest_priority) {
                highest_priority = irq_lines[i].priority;
                highest_irq = i;
            }
        }
    }
    
    return highest_irq;
}

// Process pending interrupts
void VirtualNVIC_ProcessInterrupts(void) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    if (!global_irq_enabled) {
        return;
    }
    
    int irq_num = find_highest_priority_pending();
    
    if (irq_num >= 0) {
        VirtualIRQ *irq = &irq_lines[irq_num];
        
        printf("\n[VirtualNVIC] *** Processing IRQ %d (%s) Priority=%d ***\n",
               irq_num, irq->name, irq->priority);
        
        irq->pending = 0;
        irq->active = 1;
        
        if (irq->handler != NULL) {
            irq->handler();
        } else {
            printf("[VirtualNVIC] WARNING: No handler for IRQ %d\n", irq_num);
        }
        
        irq->active = 0;
        printf("[VirtualNVIC] *** IRQ %d completed ***\n\n", irq_num);
    }
}

// Process all pending interrupts
void VirtualNVIC_ProcessAllPending(void) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    printf("[VirtualNVIC] Processing all pending interrupts...\n");
    
    int processed = 0;
    while (find_highest_priority_pending() >= 0) {
        VirtualNVIC_ProcessInterrupts();
        processed++;
        if (processed > 100) {  // Safety limit
            printf("[VirtualNVIC] WARNING: Too many pending interrupts, stopping\n");
            break;
        }
    }
    
    if (processed == 0) {
        printf("[VirtualNVIC] No pending interrupts\n");
    } else {
        printf("[VirtualNVIC] Processed %d interrupts\n", processed);
    }
}

// Print NVIC state
void VirtualNVIC_PrintState(void) {
    if (!nvic_initialized) VirtualNVIC_Init();
    
    printf("\n=== Virtual NVIC State ===\n");
    printf("Global IRQ: %s\n", global_irq_enabled ? "ENABLED" : "DISABLED");
    printf("\nActive/Pending IRQs:\n");
    printf("IRQ | Name                | En | Pend | Act | Prio\n");
    printf("----+---------------------+----+------+-----+-----\n");
    
    int count = 0;
    for (int i = 0; i < MAX_IRQ_LINES; i++) {
        if (irq_lines[i].enabled || irq_lines[i].pending || irq_lines[i].active) {
            printf("%3d | %-19s | %s  |  %s   | %s   |  %2d\n",
                   i, irq_lines[i].name,
                   irq_lines[i].enabled ? "Y" : "N",
                   irq_lines[i].pending ? "Y" : "N",
                   irq_lines[i].active ? "Y" : "N",
                   irq_lines[i].priority);
            count++;
        }
    }
    
    if (count == 0) {
        printf("(No active or pending interrupts)\n");
    }
    
    printf("========================\n\n");
}

// Example interrupt handlers
void timer_irq_handler(void) {
    printf("  [Handler] Timer interrupt executed\n");
}

void gpio_irq_handler(void) {
    printf("  [Handler] GPIO interrupt executed\n");
}

void usart_irq_handler(void) {
    printf("  [Handler] USART interrupt executed\n");
}

#ifdef RUN_STANDALONE_TEST
// Test function - only compiled when RUN_STANDALONE_TEST is defined
int main(void) {
    printf("=== Virtual NVIC Test ===\n\n");
    
    // Initialize
    VirtualNVIC_Init();
    
    // Test 1: Basic IRQ configuration
    printf("--- Test 1: Basic IRQ Configuration ---\n");
    VirtualNVIC_EnableIRQ(6);   // TIM1 Update
    VirtualNVIC_SetPriority(6, 2);
    VirtualNVIC_SetHandler(6, timer_irq_handler, "TIM1_Update");
    
    // Test 2: Multiple IRQs with different priorities
    printf("\n--- Test 2: Multiple IRQs with Priorities ---\n");
    VirtualNVIC_EnableIRQ(23);  // EXTI9_5
    VirtualNVIC_SetPriority(23, 1);  // Higher priority (lower number)
    VirtualNVIC_SetHandler(23, gpio_irq_handler, "EXTI9_5");
    
    VirtualNVIC_EnableIRQ(37);  // USART1
    VirtualNVIC_SetPriority(37, 3);  // Lower priority
    VirtualNVIC_SetHandler(37, usart_irq_handler, "USART1");
    
    // Test 3: Trigger interrupts
    printf("\n--- Test 3: Trigger and Process Interrupts ---\n");
    VirtualNVIC_SetPending(37);  // USART (priority 3)
    VirtualNVIC_SetPending(6);   // Timer (priority 2)
    VirtualNVIC_SetPending(23);  // GPIO (priority 1 - highest)
    
    VirtualNVIC_PrintState();
    
    printf("Processing interrupts in priority order:\n");
    VirtualNVIC_ProcessAllPending();
    
    // Test 4: Global interrupt control
    printf("\n--- Test 4: Global Interrupt Control ---\n");
    VirtualNVIC_DisableGlobalIRQ();
    VirtualNVIC_SetPending(6);
    printf("Attempting to process with global IRQ disabled:\n");
    VirtualNVIC_ProcessInterrupts();  // Should not execute
    
    VirtualNVIC_EnableGlobalIRQ();
    printf("Processing with global IRQ enabled:\n");
    VirtualNVIC_ProcessInterrupts();
    
    // Test 5: Error injection
    printf("\n--- Test 5: Error Injection ---\n");
    VirtualNVIC_SetErrorInjection(1);
    for (int i = 0; i < 5; i++) {
        if (!VirtualNVIC_EnableIRQ(50 + i)) {
            printf("Failed to enable IRQ with error code: %d\n", 
                   VirtualNVIC_GetLastError());
        }
    }
    VirtualNVIC_SetErrorInjection(0);
    
    // Test 6: Final state
    printf("\n--- Test 6: Final State ---\n");
    VirtualNVIC_PrintState();
    
    printf("=== All Tests Complete ===\n");
    return 0;
}
#endif  // RUN_STANDALONE_TEST

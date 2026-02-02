/*
 * interrupt_basics.c
 *
 * Demonstrates basic interrupt handling concepts
 * Learning objectives:
 * - Interrupt enable/disable
 * - NVIC configuration
 * - Interrupt priority
 * - Interrupt service routines
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* NVIC Register addresses */
#define NVIC_ISER0      ((volatile uint32_t*)0xE000E100)  // Interrupt Set-Enable Register
#define NVIC_ICER0      ((volatile uint32_t*)0xE000E180)  // Interrupt Clear-Enable Register
#define NVIC_ISPR0      ((volatile uint32_t*)0xE000E200)  // Interrupt Set-Pending Register
#define NVIC_ICPR0      ((volatile uint32_t*)0xE000E280)  // Interrupt Clear-Pending Register
#define NVIC_IPR        ((volatile uint32_t*)0xE000E400)  // Interrupt Priority Register

/* IRQ Numbers for STM32F446RE */
#define IRQ_EXTI0       6
#define IRQ_EXTI1       7
#define IRQ_EXTI15_10   40
#define IRQ_TIM2        28
#define IRQ_USART2      38

/* Global interrupt counter */
volatile uint32_t interrupt_count = 0;

void demonstrate_nvic_structure(void)
{
    printf("=== NVIC (Nested Vectored Interrupt Controller) ===\n\n");
    
    printf("Key NVIC Registers:\n");
    printf("1. ISER (Interrupt Set-Enable)\n");
    printf("   - Enable interrupts\n");
    printf("   - Address: 0x%08lX\n", (unsigned long)NVIC_ISER0);
    printf("   - 8 registers (ISER0-ISER7) for 240 interrupts\n\n");
    
    printf("2. ICER (Interrupt Clear-Enable)\n");
    printf("   - Disable interrupts\n");
    printf("   - Address: 0x%08lX\n", (unsigned long)NVIC_ICER0);
    printf("   - Writing 1 disables corresponding interrupt\n\n");
    
    printf("3. ISPR (Interrupt Set-Pending)\n");
    printf("   - Set interrupt pending flag\n");
    printf("   - Address: 0x%08lX\n", (unsigned long)NVIC_ISPR0);
    printf("   - Useful for software-triggered interrupts\n\n");
    
    printf("4. IPR (Interrupt Priority)\n");
    printf("   - Set interrupt priority\n");
    printf("   - Address: 0x%08lX\n", (unsigned long)NVIC_IPR);
    printf("   - 4 bits per interrupt (16 priority levels)\n\n");
}

void demonstrate_interrupt_enable(void)
{
    printf("=== Enabling Interrupts ===\n\n");
    
    printf("Example: Enabling EXTI0 (IRQ 6)\n");
    printf("1. Calculate register and bit position:\n");
    printf("   IRQ Number: %d\n", IRQ_EXTI0);
    printf("   Register: ISER%d (IRQ/32 = %d/32 = %d)\n", 
           IRQ_EXTI0/32, IRQ_EXTI0, IRQ_EXTI0/32);
    printf("   Bit position: %d (IRQ %% 32 = %d %% 32 = %d)\n\n",
           IRQ_EXTI0 % 32, IRQ_EXTI0, IRQ_EXTI0 % 32);
    
    printf("2. Enable interrupt:\n");
    printf("   *NVIC_ISER0 |= (1 << %d);\n\n", IRQ_EXTI0 % 32);
    
    /* Enable EXTI0 interrupt */
    *NVIC_ISER0 |= (1 << (IRQ_EXTI0 % 32));
    printf("   NVIC_ISER0 = 0x%08lX\n", (unsigned long)*NVIC_ISER0);
    printf("   EXTI0 interrupt enabled!\n\n");
}

void demonstrate_interrupt_disable(void)
{
    printf("=== Disabling Interrupts ===\n\n");
    
    printf("Example: Disabling EXTI0 (IRQ 6)\n");
    printf("1. Use ICER register:\n");
    printf("   *NVIC_ICER0 |= (1 << %d);\n\n", IRQ_EXTI0 % 32);
    
    /* Disable EXTI0 interrupt */
    *NVIC_ICER0 |= (1 << (IRQ_EXTI0 % 32));
    printf("   NVIC_ISER0 = 0x%08lX\n", (unsigned long)*NVIC_ISER0);
    printf("   EXTI0 interrupt disabled!\n\n");
}

void demonstrate_interrupt_priority(void)
{
    printf("=== Interrupt Priority Configuration ===\n\n");
    
    printf("STM32F4 Priority Levels:\n");
    printf("- 4 bits per interrupt = 16 priority levels (0-15)\n");
    printf("- 0 = Highest priority, 15 = Lowest priority\n");
    printf("- Lower number = Higher priority\n\n");
    
    printf("Example: Setting EXTI0 priority to 5\n");
    printf("1. Calculate IPR register:\n");
    printf("   IPR register: IPR[%d] (IRQ/4 = %d/4 = %d)\n",
           IRQ_EXTI0/4, IRQ_EXTI0, IRQ_EXTI0/4);
    printf("   Byte position: %d (IRQ %% 4 = %d %% 4 = %d)\n",
           IRQ_EXTI0 % 4, IRQ_EXTI0, IRQ_EXTI0 % 4);
    printf("   Bit shift: %d (byte_pos * 8 + 4)\n\n", (IRQ_EXTI0 % 4) * 8 + 4);
    
    printf("2. Set priority:\n");
    uint8_t priority = 5;
    uint8_t shift = (IRQ_EXTI0 % 4) * 8 + 4;  // Upper 4 bits of byte
    
    volatile uint32_t *ipr = NVIC_IPR + (IRQ_EXTI0 / 4);
    *ipr &= ~(0xF << shift);  // Clear
    *ipr |= (priority << shift);  // Set
    
    printf("   IPR[%d] = 0x%08lX\n", IRQ_EXTI0/4, (unsigned long)*ipr);
    printf("   EXTI0 priority set to %d\n\n", priority);
}

void demonstrate_priority_grouping(void)
{
    printf("=== Priority Grouping ===\n\n");
    
    printf("PRIGROUP field in SCB->AIRCR:\n");
    printf("Splits priority into:\n");
    printf("- Preemption priority (can interrupt lower priority ISRs)\n");
    printf("- Sub-priority (determines order when both pending)\n\n");
    
    printf("Priority Group Settings:\n");
    printf("Group 0: 0 bits preemption, 4 bits sub (no preemption)\n");
    printf("Group 1: 1 bit preemption,  3 bits sub\n");
    printf("Group 2: 2 bits preemption, 2 bits sub\n");
    printf("Group 3: 3 bits preemption, 1 bit sub\n");
    printf("Group 4: 4 bits preemption, 0 bits sub (all preemption)\n\n");
    
    printf("Example: Priority group 2\n");
    printf("  2 bits preemption (0-3)\n");
    printf("  2 bits sub-priority (0-3)\n");
    printf("  Priority value 0x50 = binary 0101\n");
    printf("    Preemption: 01 (1)\n");
    printf("    Sub-priority: 01 (1)\n\n");
}

void demonstrate_pending_flags(void)
{
    printf("=== Interrupt Pending Flags ===\n\n");
    
    printf("Reading pending status:\n");
    printf("NVIC_ISPR0 = 0x%08lX\n", (unsigned long)*NVIC_ISPR0);
    
    printf("\nSetting interrupt pending (software trigger):\n");
    printf("Example: Trigger EXTI0 from software\n");
    *NVIC_ISPR0 |= (1 << (IRQ_EXTI0 % 32));
    printf("NVIC_ISPR0 = 0x%08lX\n", (unsigned long)*NVIC_ISPR0);
    
    printf("\nClearing pending flag:\n");
    *NVIC_ICPR0 |= (1 << (IRQ_EXTI0 % 32));
    printf("NVIC_ICPR0 = 0x%08lX\n", (unsigned long)*NVIC_ICPR0);
    printf("Pending flag cleared\n\n");
}

void demonstrate_global_interrupt_control(void)
{
    printf("=== Global Interrupt Control ===\n\n");
    
    printf("ARM Cortex-M provides special instructions:\n\n");
    
    printf("1. CPSID i - Disable all interrupts\n");
    printf("   __asm volatile (\"cpsid i\");\n");
    printf("   - Sets PRIMASK bit\n");
    printf("   - Blocks all exceptions except NMI and HardFault\n\n");
    
    printf("2. CPSIE i - Enable all interrupts\n");
    printf("   __asm volatile (\"cpsie i\");\n");
    printf("   - Clears PRIMASK bit\n");
    printf("   - Allows interrupt processing\n\n");
    
    printf("3. Critical Section Example:\n");
    printf("   __disable_irq();  // Disable interrupts\n");
    printf("   // Critical code here\n");
    printf("   __enable_irq();   // Re-enable interrupts\n\n");
}

void demonstrate_isr_template(void)
{
    printf("=== Interrupt Service Routine (ISR) Template ===\n\n");
    
    printf("ISR Requirements:\n");
    printf("1. Named according to vector table\n");
    printf("2. void return type, no parameters\n");
    printf("3. Keep execution time short\n");
    printf("4. Clear interrupt flag\n");
    printf("5. Use volatile for shared variables\n\n");
    
    printf("Example ISR:\n");
    printf("void EXTI0_IRQHandler(void)\n");
    printf("{\n");
    printf("    // 1. Check which caused interrupt\n");
    printf("    if (EXTI->PR & (1 << 0)) {\n");
    printf("        // 2. Handle interrupt\n");
    printf("        interrupt_count++;\n");
    printf("        \n");
    printf("        // 3. Clear pending flag (IMPORTANT!)\n");
    printf("        EXTI->PR |= (1 << 0);\n");
    printf("    }\n");
    printf("}\n\n");
}

int main(void)
{
    printf("=== Interrupt Basics Tutorial ===\n\n");
    
    demonstrate_nvic_structure();
    demonstrate_interrupt_enable();
    demonstrate_interrupt_disable();
    demonstrate_interrupt_priority();
    demonstrate_priority_grouping();
    demonstrate_pending_flags();
    demonstrate_global_interrupt_control();
    demonstrate_isr_template();
    
    printf("=== Key Concepts Summary ===\n");
    printf("1. NVIC manages all interrupts in Cortex-M\n");
    printf("2. Each interrupt has enable, pending, and priority bits\n");
    printf("3. Lower priority number = Higher priority\n");
    printf("4. Always clear interrupt flag in ISR\n");
    printf("5. Keep ISR execution time minimal\n");
    printf("6. Use volatile for variables shared with ISRs\n");
    printf("7. Priority grouping splits preemption/sub-priority\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

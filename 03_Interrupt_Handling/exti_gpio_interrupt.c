/*
 * exti_gpio_interrupt.c
 *
 * Demonstrates External Interrupt (EXTI) configuration for GPIO
 * Learning objectives:
 * - EXTI line configuration
 * - GPIO to EXTI mapping via SYSCFG
 * - Trigger detection (rising/falling/both)
 * - Interrupt handling for button press
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* EXTI Register Structure */
typedef struct {
    volatile uint32_t IMR;    // Interrupt mask register
    volatile uint32_t EMR;    // Event mask register
    volatile uint32_t RTSR;   // Rising trigger selection
    volatile uint32_t FTSR;   // Falling trigger selection
    volatile uint32_t SWIER;  // Software interrupt event
    volatile uint32_t PR;     // Pending register
} EXTI_RegDef_t;

#define EXTI ((EXTI_RegDef_t*)EXT1_BASEADDR)

/* SYSCFG Register Structure (for EXTI config) */
typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t PMC;
    volatile uint32_t EXTICR[4];  // External interrupt configuration
    uint32_t RESERVED[2];
    volatile uint32_t CMPCR;
} SYSCFG_RegDef_t;

#define SYSCFG ((SYSCFG_RegDef_t*)SYSCFC_BASEADDR)

/* NVIC registers */
#define NVIC_ISER0      ((volatile uint32_t*)0xE000E100)
#define NVIC_IPR        ((volatile uint32_t*)0xE000E400)

/* IRQ numbers */
#define IRQ_EXTI0       6
#define IRQ_EXTI1       7
#define IRQ_EXTI2       8
#define IRQ_EXTI3       9
#define IRQ_EXTI4       10
#define IRQ_EXTI15_10   40

/* Global variables */
volatile uint32_t button_press_count = 0;
volatile uint32_t last_interrupt_time = 0;

void demonstrate_exti_architecture(void)
{
    printf("=== EXTI (External Interrupt) Architecture ===\n\n");
    
    printf("EXTI Overview:\n");
    printf("- 23 EXTI lines (0-22) on STM32F446\n");
    printf("- Lines 0-15: Connected to GPIO pins\n");
    printf("- Lines 16-22: Internal peripherals\n");
    printf("- Each line can detect rising/falling edges\n\n");
    
    printf("GPIO to EXTI Mapping:\n");
    printf("- EXTI0 can connect to PA0, PB0, PC0, etc.\n");
    printf("- EXTI1 can connect to PA1, PB1, PC1, etc.\n");
    printf("- Selection done via SYSCFG->EXTICR registers\n\n");
    
    printf("EXTI Registers:\n");
    printf("IMR:   Interrupt Mask (enable/disable)\n");
    printf("RTSR:  Rising Trigger Selection\n");
    printf("FTSR:  Falling Trigger Selection\n");
    printf("PR:    Pending Register (must clear in ISR)\n\n");
}

void EXTI_GPIO_Config(uint8_t gpio_port, uint8_t pin_number, uint8_t trigger)
{
    printf("=== Configuring EXTI Line %d ===\n", pin_number);
    
    /* Step 1: Enable SYSCFG clock */
    printf("1. Enabling SYSCFG clock...\n");
    RCC->APB2ENR |= (1 << 14);
    printf("   SYSCFG clock enabled\n");
    
    /* Step 2: Configure SYSCFG EXTICR to map GPIO to EXTI line */
    printf("2. Mapping GPIO Port %c to EXTI%d...\n", 
           'A' + gpio_port, pin_number);
    
    uint8_t exticr_index = pin_number / 4;
    uint8_t exticr_position = (pin_number % 4) * 4;
    
    SYSCFG->EXTICR[exticr_index] &= ~(0xF << exticr_position);
    SYSCFG->EXTICR[exticr_index] |= (gpio_port << exticr_position);
    
    printf("   SYSCFG->EXTICR[%d] = 0x%08lX\n", 
           exticr_index, (unsigned long)SYSCFG->EXTICR[exticr_index]);
    
    /* Step 3: Configure trigger detection */
    printf("3. Configuring trigger detection...\n");
    
    if (trigger == 0 || trigger == 2) {  // Rising or both
        EXTI->RTSR |= (1 << pin_number);
        printf("   Rising edge trigger enabled\n");
    }
    
    if (trigger == 1 || trigger == 2) {  // Falling or both
        EXTI->FTSR |= (1 << pin_number);
        printf("   Falling edge trigger enabled\n");
    }
    
    printf("   EXTI->RTSR = 0x%08lX\n", (unsigned long)EXTI->RTSR);
    printf("   EXTI->FTSR = 0x%08lX\n", (unsigned long)EXTI->FTSR);
    
    /* Step 4: Enable EXTI line interrupt */
    printf("4. Enabling EXTI line in IMR...\n");
    EXTI->IMR |= (1 << pin_number);
    printf("   EXTI->IMR = 0x%08lX\n\n", (unsigned long)EXTI->IMR);
}

void EXTI_NVIC_Config(uint8_t pin_number, uint8_t priority)
{
    printf("=== Configuring NVIC for EXTI%d ===\n", pin_number);
    
    /* Determine IRQ number based on pin */
    uint8_t irq_number;
    if (pin_number <= 4) {
        irq_number = IRQ_EXTI0 + pin_number;
    } else if (pin_number <= 9) {
        irq_number = 23;  // EXTI9_5
    } else {
        irq_number = IRQ_EXTI15_10;
    }
    
    printf("1. IRQ Number: %d\n", irq_number);
    
    /* Enable interrupt in NVIC */
    printf("2. Enabling interrupt in NVIC...\n");
    *NVIC_ISER0 |= (1 << irq_number);
    printf("   NVIC_ISER0 = 0x%08lX\n", (unsigned long)*NVIC_ISER0);
    
    /* Set priority */
    printf("3. Setting priority to %d...\n", priority);
    uint8_t shift = (irq_number % 4) * 8 + 4;
    volatile uint32_t *ipr = NVIC_IPR + (irq_number / 4);
    *ipr &= ~(0xF << shift);
    *ipr |= (priority << shift);
    printf("   Priority configured\n\n");
}

void example_button_interrupt_pc13(void)
{
    printf("=== Example: User Button on PC13 ===\n");
    printf("(User button on Nucleo board)\n\n");
    
    /* Step 1: Enable GPIOC clock */
    printf("1. Configuring GPIO...\n");
    RCC->AHB1ENR |= (1 << 2);  // GPIOC
    
    /* Configure PC13 as input with pull-up */
    GPIOC->MODER &= ~(0x3 << 26);  // Input mode
    GPIOC->PUPDR &= ~(0x3 << 26);
    GPIOC->PUPDR |= (0x1 << 26);   // Pull-up
    printf("   PC13 configured as input with pull-up\n\n");
    
    /* Step 2: Configure EXTI13 for PC13 */
    EXTI_GPIO_Config(2, 13, 1);  // Port C, Pin 13, Falling edge
    
    /* Step 3: Configure NVIC */
    EXTI_NVIC_Config(13, 5);  // Priority 5
    
    printf("Setup complete! Button press will trigger EXTI13.\n\n");
}

void example_multiple_exti_lines(void)
{
    printf("=== Example: Multiple EXTI Lines ===\n\n");
    
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= (1 << 0);
    
    /* Configure PA0 and PA1 as inputs */
    printf("Configuring PA0 and PA1 as inputs...\n");
    GPIOA->MODER &= ~((0x3 << 0) | (0x3 << 2));
    GPIOA->PUPDR &= ~((0x3 << 0) | (0x3 << 2));
    GPIOA->PUPDR |= ((0x1 << 0) | (0x1 << 2));  // Pull-up
    printf("GPIO configured\n\n");
    
    /* Configure EXTI0 for PA0 - Rising edge */
    printf("Configuring EXTI0 for PA0 (rising edge):\n");
    EXTI_GPIO_Config(0, 0, 0);  // Port A, Pin 0, Rising
    EXTI_NVIC_Config(0, 3);
    
    /* Configure EXTI1 for PA1 - Falling edge */
    printf("Configuring EXTI1 for PA1 (falling edge):\n");
    EXTI_GPIO_Config(0, 1, 1);  // Port A, Pin 1, Falling
    EXTI_NVIC_Config(1, 4);
    
    printf("Multiple EXTI lines configured!\n\n");
}

void demonstrate_isr_implementation(void)
{
    printf("=== EXTI ISR Implementation ===\n\n");
    
    printf("ISR for EXTI0:\n");
    printf("void EXTI0_IRQHandler(void)\n");
    printf("{\n");
    printf("    // Check if EXTI0 triggered\n");
    printf("    if (EXTI->PR & (1 << 0)) {\n");
    printf("        // Handle interrupt\n");
    printf("        button_press_count++;\n");
    printf("        \n");
    printf("        // MUST clear pending flag!\n");
    printf("        EXTI->PR |= (1 << 0);  // Write 1 to clear\n");
    printf("    }\n");
    printf("}\n\n");
    
    printf("ISR for EXTI15_10 (shared by pins 10-15):\n");
    printf("void EXTI15_10_IRQHandler(void)\n");
    printf("{\n");
    printf("    // Check which line triggered\n");
    printf("    if (EXTI->PR & (1 << 13)) {\n");
    printf("        // PC13 button pressed\n");
    printf("        button_press_count++;\n");
    printf("        EXTI->PR |= (1 << 13);\n");
    printf("    }\n");
    printf("    \n");
    printf("    if (EXTI->PR & (1 << 14)) {\n");
    printf("        // Handle pin 14\n");
    printf("        EXTI->PR |= (1 << 14);\n");
    printf("    }\n");
    printf("}\n\n");
}

void demonstrate_debouncing(void)
{
    printf("=== Button Debouncing ===\n\n");
    
    printf("Problem: Mechanical bounce causes multiple interrupts\n");
    printf("Solution: Software debouncing\n\n");
    
    printf("Method 1: Time-based debouncing\n");
    printf("uint32_t last_time = 0;\n");
    printf("void EXTI0_IRQHandler(void)\n");
    printf("{\n");
    printf("    uint32_t current_time = GetTickCount();\n");
    printf("    if (current_time - last_time > 50) {  // 50ms debounce\n");
    printf("        // Valid button press\n");
    printf("        last_time = current_time;\n");
    printf("    }\n");
    printf("    EXTI->PR |= (1 << 0);\n");
    printf("}\n\n");
    
    printf("Method 2: State machine\n");
    printf("- Check button state in ISR\n");
    printf("- Use timer to verify stable state\n");
    printf("- Only act after stable period\n\n");
}

int main(void)
{
    printf("=== EXTI GPIO Interrupt Example ===\n\n");
    
    demonstrate_exti_architecture();
    
    example_button_interrupt_pc13();
    
    example_multiple_exti_lines();
    
    demonstrate_isr_implementation();
    
    demonstrate_debouncing();
    
    printf("=== Key Points Summary ===\n");
    printf("1. Enable SYSCFG clock for EXTI configuration\n");
    printf("2. Map GPIO port to EXTI line via SYSCFG->EXTICR\n");
    printf("3. Configure trigger (rising/falling/both)\n");
    printf("4. Enable EXTI line in IMR register\n");
    printf("5. Enable and prioritize in NVIC\n");
    printf("6. Always clear EXTI->PR flag in ISR\n");
    printf("7. Implement debouncing for mechanical switches\n");
    
    printf("\n=== Example Complete ===\n");
    
    return 0;
}

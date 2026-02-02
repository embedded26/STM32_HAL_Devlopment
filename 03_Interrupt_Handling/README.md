# 03_Interrupt_Handling

## Overview
This directory contains examples demonstrating interrupt handling in STM32 microcontrollers. Learn how to configure, prioritize, and handle various interrupt sources including GPIO external interrupts and timer interrupts.

## Learning Objectives
- Understand NVIC (Nested Vectored Interrupt Controller)
- Configure external interrupts (EXTI) for GPIO
- Set up timer-based periodic interrupts
- Manage interrupt priorities
- Write efficient interrupt service routines (ISRs)
- Handle interrupt flags correctly

## Files in This Directory

### 1. interrupt_basics.c
**Purpose:** Fundamental interrupt concepts and NVIC configuration

**Key Concepts:**
- NVIC register structure (ISER, ICER, ISPR, IPR)
- Interrupt enable/disable mechanisms
- Priority configuration (16 levels, 0-15)
- Priority grouping (preemption vs sub-priority)
- Global interrupt control (CPSID/CPSIE)
- ISR template and best practices

**NVIC Registers:**
```c
NVIC_ISER0  0xE000E100  // Interrupt Set-Enable
NVIC_ICER0  0xE000E180  // Interrupt Clear-Enable
NVIC_ISPR0  0xE000E200  // Interrupt Set-Pending
NVIC_ICPR0  0xE000E280  // Interrupt Clear-Pending
NVIC_IPR    0xE000E400  // Interrupt Priority
```

**Priority Levels:**
- Lower number = Higher priority
- 0 = Highest priority
- 15 = Lowest priority
- 4 bits per interrupt (upper 4 bits used)

### 2. exti_gpio_interrupt.c
**Purpose:** External interrupt (EXTI) configuration for GPIO pins

**Key Concepts:**
- EXTI line to GPIO mapping via SYSCFG
- Rising/falling/both edge detection
- Multiple EXTI lines
- Shared interrupt handlers (EXTI15_10)
- Button debouncing techniques
- EXTI pending flag management

**Setup Sequence:**
1. Enable SYSCFG clock
2. Map GPIO port to EXTI line (SYSCFG->EXTICR)
3. Configure trigger detection (RTSR/FTSR)
4. Enable EXTI line (IMR)
5. Configure NVIC
6. Clear pending flag in ISR

**EXTI Lines:**
- Lines 0-15: GPIO pins (PxN → EXTIN)
- EXTI0 can be PA0, PB0, PC0, PD0, etc.
- Only one port can be active per line number

**ISR Template:**
```c
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1 << 0)) {
        // Handle interrupt
        
        // MUST clear flag!
        EXTI->PR |= (1 << 0);
    }
}
```

### 3. timer_interrupt.c
**Purpose:** Timer-based periodic interrupts

**Key Concepts:**
- Timer peripheral initialization
- Prescaler and period calculation
- Update interrupt configuration
- Common timer frequencies (1ms, 100us, 1s)
- Timer counting modes
- One-pulse mode

**Timer Frequency Calculation:**
```c
Timer_Freq = APB_Clock / (Prescaler + 1)
Update_Freq = Timer_Freq / (Period + 1)
```

**Example: 1ms Timer**
```c
// APB1 = 42 MHz
// Prescaler = 41 → 42MHz / 42 = 1MHz
// Period = 999 → 1MHz / 1000 = 1kHz = 1ms

TIM2->PSC = 41;
TIM2->ARR = 999;
TIM2->DIER |= (1 << 0);  // Enable update interrupt
TIM2->CR1 |= (1 << 0);   // Start timer
```

**Timer ISR Template:**
```c
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1 << 0)) {
        TIM2->SR &= ~(1 << 0);  // Clear UIF flag
        
        // Handle timer tick
        milliseconds++;
    }
}
```

## Interrupt Priority Grouping

STM32 allows splitting 4-bit priority into preemption and sub-priority:

| Group | Preemption | Sub-Priority | Description |
|-------|------------|--------------|-------------|
| 0 | 0 bits | 4 bits | No preemption |
| 1 | 1 bit | 3 bits | 2 preemption levels |
| 2 | 2 bits | 2 bits | 4 preemption levels |
| 3 | 3 bits | 1 bit | 8 preemption levels |
| 4 | 4 bits | 0 bits | 16 preemption levels |

**Preemption:** Higher priority can interrupt lower priority ISR  
**Sub-priority:** Determines order when both pending (no preemption)

## Common IRQ Numbers (STM32F446RE)

```
EXTI0         6
EXTI1         7
EXTI2         8
EXTI3         9
EXTI4         10
EXTI9_5       23
EXTI15_10     40
TIM2          28
TIM3          29
TIM4          30
TIM5          50
USART1        37
USART2        38
SPI1          35
SPI2          36
```

## ISR Best Practices

### ✅ Do:
1. Keep ISR execution time minimal
2. Always clear interrupt flags
3. Use `volatile` for shared variables
4. Set appropriate priority levels
5. Test edge cases and race conditions
6. Use atomic operations when needed

### ❌ Don't:
1. Perform blocking operations (delays, polling)
2. Use printf/floating-point in ISR
3. Call non-reentrant functions
4. Forget to clear interrupt flags
5. Have long critical sections
6. Disable interrupts unnecessarily

## Debouncing Techniques

### Hardware Debouncing
- RC filter on button input
- Schmitt trigger buffer

### Software Debouncing
```c
// Method 1: Time-based
uint32_t last_time = 0;
if (current_time - last_time > 50) {  // 50ms
    // Valid press
    last_time = current_time;
}

// Method 2: State verification
Read button state multiple times
Only act if stable for N readings
```

## Interrupt Safety

### Critical Sections
```c
// Protect shared variables
__disable_irq();
shared_variable++;
__enable_irq();

// Or use CMSIS intrinsics
uint32_t primask = __get_PRIMASK();
__disable_irq();
// Critical code
__set_PRIMASK(primask);
```

### Volatile Variables
```c
// REQUIRED for ISR-shared variables
volatile uint32_t flag = 0;
volatile uint8_t buffer[128];
```

## Common Pitfalls

❌ **Forgetting to clear flags:**
```c
void EXTI0_IRQHandler(void) {
    // Do something
    // Forgot: EXTI->PR |= (1 << 0);
}
// Result: Continuous interrupts!
```

❌ **Not using volatile:**
```c
uint32_t count = 0;  // BAD!
void TIM2_IRQHandler(void) {
    count++;  // May be optimized away
}
```

❌ **Long ISR execution:**
```c
void EXTI0_IRQHandler(void) {
    delay_ms(1000);  // NEVER DO THIS!
    printf("Interrupt!");  // AVOID!
}
```

✅ **Correct approach:**
```c
volatile uint8_t flag = 0;

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) {
        flag = 1;  // Set flag for main loop
        EXTI->PR |= (1 << 0);  // Clear
    }
}

int main(void) {
    while(1) {
        if (flag) {
            flag = 0;
            // Handle in main context
            process_event();
        }
    }
}
```

## Example Use Cases

### 1. Button Press Detection
- Configure EXTI on button GPIO
- Use falling edge trigger
- Implement software debouncing
- Toggle LED or set flag

### 2. Periodic Task Execution
- Configure timer for desired period
- Increment tick counter in ISR
- Execute tasks based on tick count
- Use for timebase in RTOS

### 3. Sensor Data Acquisition
- Use EXTI for data-ready signal
- Trigger ADC conversion
- Read sensor via I2C/SPI
- Store data in buffer

### 4. Communication Timeouts
- Start timer on TX/RX begin
- Stop timer on completion
- ISR handles timeout condition
- Reset communication state

## Compilation

```bash
# Interrupt basics
gcc interrupt_basics.c -I../drivers/inc -o int_basics
./int_basics

# EXTI example
gcc exti_gpio_interrupt.c -I../drivers/inc -o exti_example
./exti_example

# Timer interrupt
gcc timer_interrupt.c -I../drivers/inc -o timer_example
./timer_example
```

## Hardware Setup

For testing on real hardware:
- **Button (PC13):** Already available on Nucleo board
- **LED (PA5):** Built-in LED on Nucleo board
- **Additional buttons:** Connect to other GPIO pins with pull-up
- **Oscilloscope:** Useful for verifying interrupt timing

## Debugging Interrupts

### Check if interrupt is enabled:
```c
printf("NVIC_ISER0 = 0x%08X\n", *NVIC_ISER0);
printf("EXTI->IMR = 0x%08X\n", EXTI->IMR);
```

### Check pending flags:
```c
printf("NVIC_ISPR0 = 0x%08X\n", *NVIC_ISPR0);
printf("EXTI->PR = 0x%08X\n", EXTI->PR);
```

### Verify priority:
```c
uint8_t irq = 28;  // TIM2
uint8_t shift = (irq % 4) * 8 + 4;
uint32_t priority = (NVIC_IPR[irq/4] >> shift) & 0xF;
printf("Priority: %d\n", priority);
```

## Next Steps
After mastering interrupt handling, proceed to:
- **04_Bare_Metal_Drivers**: Build interrupt-driven drivers
- **05_Debugging_Advanced**: Debug interrupt issues
- **06_Real_World_Projects**: Apply interrupts in projects

## References
- ARM Cortex-M4 Generic User Guide
- STM32F446RE Reference Manual (RM0390) - Chapter 12 (EXTI), Chapter 18 (Timers)
- [Main Project README](../README.md)

---
*Part of the STM32 HAL Development Repository*

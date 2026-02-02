# 04_Bare_Metal_Drivers

## Overview
This directory demonstrates the usage of bare-metal drivers created in the `drivers/` directory. Learn how to use custom HAL implementations and create efficient bare-metal code for STM32 microcontrollers.

## Learning Objectives
- Use custom GPIO driver API effectively
- Implement software delays correctly
- Understand driver abstraction benefits
- Create reusable bare-metal components
- Apply driver best practices

## Files in This Directory

### 1. gpio_driver_example.c
**Purpose:** Comprehensive examples using the custom GPIO driver

**Key Concepts:**
- Driver initialization patterns
- Configuration structure usage
- Pin and port-level operations
- Combining driver functions
- Real-world application examples

**Examples Included:**
1. **LED Control** - Basic output operations
2. **Button-LED** - Input reading and output control
3. **Traffic Light** - Multi-pin coordination
4. **Port Operations** - Binary counter using port writes
5. **Toggle Function** - Efficient LED blinking

**Driver Functions Used:**
```c
GPIO_PeriClockControl()   // Enable peripheral clock
GPIO_Init()                // Initialize GPIO pin
GPIO_ReadFromInputPin()    // Read single pin
GPIO_WriteToOutputPin()    // Write to single pin
GPIO_WriteToOutputPort()   // Write to entire port
GPIO_ToggleOutputPin()     // Toggle pin state
```

### 2. simple_delay.c
**Purpose:** Software delay implementations for bare-metal systems

**Key Concepts:**
- Simple loop delays (inaccurate but simple)
- DWT cycle counter for precise timing
- Timer-based polling delays
- Non-blocking delay patterns
- Delay calibration techniques

**Delay Methods:**

#### Simple Loop Delay
```c
void delay_cycles_simple(uint32_t cycles) {
    for (volatile uint32_t i = 0; i < cycles; i++) {
        __asm volatile ("nop");
    }
}
```
- **Pros:** No hardware needed
- **Cons:** Inaccurate, compiler-dependent

#### DWT Cycle Counter Delay
```c
void delay_cycles_precise(uint32_t cycles) {
    uint32_t start = DWT_CYCCNT;
    while ((DWT_CYCCNT - start) < cycles);
}
```
- **Pros:** Very precise, cycle-accurate
- **Cons:** Cortex-M only, uses debug hardware

#### Timer-Based Delay
```c
TIM2->CNT = 0;
while (TIM2->CNT < desired_ticks);
```
- **Pros:** Precise, doesn't use debug hardware
- **Cons:** Requires timer peripheral

## Bare-Metal Driver Architecture

### Driver Layer Structure
```
Application Code
       ↓
  Driver API (gpio_drivers.h)
       ↓
Driver Implementation (gpio_drivers.c)
       ↓
Register Definitions (stm32f446re.h)
       ↓
    Hardware
```

### Configuration Structure Pattern
```c
// 1. Create handle
GPIO_Handle_t led_handle;

// 2. Configure parameters
led_handle.pGPIOx = GPIOA;
led_handle.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
led_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
// ... more config

// 3. Initialize
GPIO_Init(&led_handle);

// 4. Use driver functions
GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
```

## DWT (Data Watchpoint and Trace)

### DWT Registers
```c
#define DWT_CTRL    0xE0001000  // Control register
#define DWT_CYCCNT  0xE0001004  // Cycle count register
#define DEM_CR      0xE000EDFC  // Debug enable
```

### Initialization
```c
DEM_CR |= (1 << 24);        // Enable trace
DWT_CYCCNT = 0;             // Reset counter
DWT_CTRL |= (1 << 0);       // Enable counter
```

### Usage
```c
uint32_t start = DWT_CYCCNT;
// ... code to measure ...
uint32_t cycles = DWT_CYCCNT - start;
```

## Delay Calculations

### At 16 MHz (HSI)
- 1 cycle = 62.5 ns
- 1 µs = 16 cycles
- 1 ms = 16,000 cycles
- 1 second = 16,000,000 cycles

### At 84 MHz (PLL configured)
- 1 cycle = 11.9 ns
- 1 µs = 84 cycles
- 1 ms = 84,000 cycles
- 1 second = 84,000,000 cycles

### Example Calculations
```c
// For 1ms at 16 MHz
void delay_1ms(void) {
    delay_cycles_precise(16000);
}

// For 100us at 84 MHz
void delay_100us(void) {
    delay_cycles_precise(8400);
}
```

## Non-Blocking Delay Pattern

### Problem with Blocking Delays
```c
// BAD: Blocks entire program
void loop() {
    LED_ON();
    delay_ms(1000);  // Can't do anything else!
    LED_OFF();
    delay_ms(1000);
}
```

### Non-Blocking Solution
```c
// GOOD: Allows other operations
typedef enum {
    STATE_LED_OFF,
    STATE_LED_ON
} LedState;

LedState state = STATE_LED_OFF;
uint32_t last_toggle = 0;

void loop() {
    uint32_t now = GetTick();
    
    if (now - last_toggle >= 1000) {
        if (state == STATE_LED_OFF) {
            LED_ON();
            state = STATE_LED_ON;
        } else {
            LED_OFF();
            state = STATE_LED_OFF;
        }
        last_toggle = now;
    }
    
    // Can do other things here!
    check_buttons();
    update_display();
}
```

## Driver Usage Best Practices

### ✅ Do:
1. Always enable peripheral clock first
2. Use configuration structures
3. Check return values (if implemented)
4. Initialize before using peripheral
5. Use descriptive handle names
6. Group related configurations

### ❌ Don't:
1. Access registers directly when driver exists
2. Skip clock enablement
3. Forget to initialize before use
4. Mix driver and direct register access
5. Ignore peripheral state

## Example Applications

### Traffic Light Controller
```c
// Red, Yellow, Green sequence
GPIO_Handle_t red, yellow, green;

// Initialize all LEDs
init_led(&red, GPIOB, PIN_0);
init_led(&yellow, GPIOB, PIN_1);
init_led(&green, GPIOB, PIN_2);

// Sequence
set_led(&red, ON);    delay_ms(3000);
set_led(&red, OFF);
set_led(&yellow, ON); delay_ms(1000);
set_led(&yellow, OFF);
set_led(&green, ON);  delay_ms(3000);
```

### Binary Counter
```c
// Display 0-15 on 4 LEDs
for (uint8_t count = 0; count < 16; count++) {
    GPIO_WriteToOutputPort(GPIOA, count);
    delay_ms(500);
}
```

### Button Debouncing
```c
uint32_t last_press = 0;

if (button_pressed()) {
    uint32_t now = GetTick();
    if (now - last_press > 50) {  // 50ms debounce
        // Valid press
        handle_button_press();
        last_press = now;
    }
}
```

## Performance Considerations

### Delay Accuracy Comparison
| Method | Accuracy | CPU Usage | Complexity |
|--------|----------|-----------|------------|
| Simple Loop | ±20% | 100% | Low |
| DWT Counter | <1% | 100% | Medium |
| Timer Polling | <1% | 100% | Medium |
| Timer Interrupt | <1% | 0% | High |

### Memory Footprint
- Driver code: ~2-4 KB
- Configuration structures: ~20 bytes each
- Handle structures: ~12 bytes each

## Compilation

```bash
# GPIO driver example
gcc gpio_driver_example.c \
    ../drivers/src/stm32f446re_gpio_drivers.c \
    -I../drivers/inc \
    -o gpio_example
./gpio_example

# Delay example
gcc simple_delay.c -I../drivers/inc -o delay_example
./delay_example
```

## Hardware Setup

For testing these examples:
- **PA5:** Built-in LED on Nucleo board
- **PC13:** Built-in button on Nucleo board
- **PB0-PB2:** External LEDs for traffic light
- **PA0-PA3:** External LEDs for binary counter

## Debugging Tips

### Check Clock Enablement
```c
printf("RCC->AHB1ENR = 0x%08X\n", RCC->AHB1ENR);
// Verify bit 0 set for GPIOA
```

### Verify GPIO Configuration
```c
printf("GPIOA->MODER = 0x%08X\n", GPIOA->MODER);
printf("GPIOA->ODR = 0x%08X\n", GPIOA->ODR);
```

### Check DWT Status
```c
if (!(DWT_CTRL & 1)) {
    printf("DWT cycle counter not enabled!\n");
}
```

## Common Issues

### Delay Not Working
```c
// Problem: Compiler optimized away loop
for (int i = 0; i < 1000; i++);  // Gone!

// Solution: Use volatile
for (volatile int i = 0; i < 1000; i++);
```

### LED Not Blinking
```c
// Check: Clock enabled?
if (!(RCC->AHB1ENR & (1 << 0))) {
    printf("GPIOA clock not enabled!\n");
}

// Check: Pin configured as output?
if ((GPIOA->MODER & (0x3 << 10)) != (0x1 << 10)) {
    printf("PA5 not configured as output!\n");
}
```

## Advanced Topics

### DMA with GPIO
- Use DMA to update GPIO without CPU
- Useful for LED patterns, waveform generation

### Low-Power Delays
- Use sleep modes during delays
- Wake up with timer interrupt
- Saves power compared to busy-wait

### Precise Timing with TIM
- Use timer capture/compare for events
- Generate precise delays without blocking
- Implement PWM for LED dimming

## Next Steps
After mastering bare-metal drivers:
- **05_Debugging_Advanced**: Debug driver issues
- **06_Real_World_Projects**: Build complete applications
- Create your own peripheral drivers (I2C, SPI, UART)

## References
- Custom GPIO Driver: `drivers/src/stm32f446re_gpio_drivers.c`
- Register Definitions: `drivers/inc/stm32f446re.h`
- ARM Cortex-M4 Programming Manual
- [Main Project README](../README.md)

---
*Part of the STM32 HAL Development Repository*

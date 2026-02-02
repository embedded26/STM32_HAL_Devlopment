# 06_Real_World_Projects

## Overview
This directory contains complete, working real-world projects that combine concepts from previous sections. These projects demonstrate professional embedded development practices and can serve as templates for your own applications.

## Learning Objectives
- Build complete embedded applications
- Integrate multiple peripherals
- Implement proper project structure
- Apply debugging techniques
- Create production-quality code
- Follow embedded best practices

## Projects in This Directory

### 1. led_blink.c
**Purpose:** The "Hello World" of embedded systems - LED blinking

**Features:**
- Clean project structure
- GPIO driver usage
- Software delays
- Debug logging
- Well-documented code

**Hardware:** 
- LED on PA5 (built-in green LED on Nucleo)

**Concepts Demonstrated:**
- Basic GPIO output
- Initialization sequence
- Main loop structure
- Code organization

**Expected Behavior:**
```
LED blinks at 1 Hz (500ms ON, 500ms OFF)
Debug output every 10 blinks
```

**Extensions:**
1. Different blink patterns (SOS, heartbeat)
2. Variable blink rate with potentiometer
3. Timer-based blinking (non-blocking)
4. Multiple LEDs with different patterns
5. PWM breathing effect

### 2. button_interrupt.c
**Purpose:** Interrupt-driven button control with debouncing

**Features:**
- EXTI interrupt configuration
- Software debouncing
- State management
- Clean ISR implementation
- Button press counting

**Hardware:**
- Button on PC13 (built-in user button on Nucleo)
- LED on PA5 (built-in green LED)

**Concepts Demonstrated:**
- External interrupts (EXTI)
- SYSCFG configuration
- NVIC setup
- Debouncing techniques
- ISR best practices

**Expected Behavior:**
```
Press button → LED toggles
Debouncing prevents multiple toggles
Press count tracked and displayed
```

**Extensions:**
1. Long press detection
2. Double-click detection
3. Multiple buttons with different actions
4. Morse code input
5. Hold-and-repeat functionality
6. State machine for complex behaviors

### 3. uart_echo.c
**Purpose:** UART echo server with interrupt-driven I/O

**Features:**
- UART peripheral configuration
- Interrupt-driven reception
- Circular buffer implementation
- Character echo with formatting
- Buffer overflow protection

**Hardware:**
- USART2 on PA2 (TX) and PA3 (RX)
- Connect USB-UART adapter or use ST-Link virtual COM

**Concepts Demonstrated:**
- UART initialization
- GPIO alternate function
- Baud rate calculation
- Interrupt handling
- Circular buffer
- Data processing

**Expected Behavior:**
```
Terminal Settings: 115200 baud, 8N1
Type characters → Echoed back
Enter key → CR+LF
Backspace works correctly
```

**Extensions:**
1. Command line interface
2. AT command parser
3. DMA transfers
4. Flow control (RTS/CTS)
5. Line editing with history
6. printf redirection to UART
7. Binary protocol support
8. Dynamic baud rate change

## Project Structure

### Recommended Layout
```
project_name/
├── src/
│   ├── main.c
│   ├── interrupts.c
│   └── peripherals.c
├── inc/
│   ├── config.h
│   └── peripherals.h
├── drivers/          (from repository)
│   ├── inc/
│   └── src/
├── Makefile
├── linker_script.ld
├── startup.s
└── README.md
```

### Makefile Template
```makefile
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=cortex-m4 -mthumb -O2 -Wall -g
INCLUDES = -Iinc -Idrivers/inc

SRCS = src/main.c drivers/src/*.c
TARGET = firmware

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

flash:
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f $(TARGET).elf $(TARGET).bin
```

## Best Practices Applied

### Code Organization
```c
// 1. Includes
#include "drivers/inc/..."
#include "config.h"

// 2. Defines and macros
#define LED_PIN GPIO_PIN_NO_5

// 3. Type definitions
typedef struct { ... } Config_t;

// 4. Global variables
volatile uint8_t flag = 0;

// 5. Function prototypes
void Init_System(void);

// 6. Function implementations
void Init_System(void) { ... }

// 7. Main function
int main(void) { ... }
```

### Initialization Pattern
```c
int main(void)
{
    // 1. Clock configuration
    System_Clock_Config();
    
    // 2. Peripheral initialization
    GPIO_Init();
    UART_Init();
    Timer_Init();
    
    // 3. Interrupt configuration
    NVIC_Config();
    
    // 4. Start peripherals
    Timer_Start();
    
    // 5. Main loop
    while (1) {
        Process_Events();
    }
}
```

### Error Handling
```c
// Check return values
if (UART_Init() != SUCCESS) {
    DEBUG_ERROR("UART init failed");
    Error_Handler();
}

// Validate parameters
if (buffer == NULL || size == 0) {
    return ERROR_INVALID_PARAM;
}

// Handle peripheral errors
if (USART->SR & UART_ERROR_MASK) {
    Clear_UART_Errors();
    Stats.uart_errors++;
}
```

### Resource Management
```c
// Acquire resources
__disable_irq();
SPI_Select();

// Use resource
SPI_Transfer(data, len);

// Release resources
SPI_Deselect();
__enable_irq();
```

## Hardware Setup

### STM32F446RE Nucleo Board
- **LED (PA5):** Built-in green LED
- **Button (PC13):** Built-in user button
- **UART (PA2/PA3):** Available on board edge or via ST-Link
- **Power:** USB cable provides 5V

### External Connections
```
PA2 (TX) ─────► UART RX
PA3 (RX) ◄───── UART TX
GND ──────────── GND

Optional:
PA0-PA7: LEDs with 330Ω resistors
PB0-PB7: Buttons with pull-ups
```

## Compilation and Flashing

### Using Makefile
```bash
make                    # Compile
make flash              # Flash to board
make clean              # Clean build files
```

### Manual Compilation
```bash
arm-none-eabi-gcc \
    -mcpu=cortex-m4 -mthumb \
    -O2 -Wall -g \
    -I../drivers/inc \
    led_blink.c \
    ../drivers/src/stm32f446re_gpio_drivers.c \
    -o led_blink.elf

arm-none-eabi-objcopy \
    -O binary led_blink.elf led_blink.bin
```

### Flashing
```bash
# Using st-flash
st-flash write firmware.bin 0x08000000

# Using OpenOCD
openocd -f board/st_nucleo_f4.cfg \
    -c "program firmware.elf verify reset exit"

# Using STM32CubeProgrammer
STM32_Programmer_CLI -c port=SWD -w firmware.bin 0x08000000
```

## Debugging

### GDB Debugging
```bash
# Start OpenOCD
openocd -f board/st_nucleo_f4.cfg

# In another terminal
arm-none-eabi-gdb firmware.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) break main
(gdb) continue
```

### Serial Debugging
```bash
# Linux
minicom -D /dev/ttyUSB0 -b 115200

# macOS
screen /dev/tty.usbserial 115200

# Windows
PuTTY or Tera Term
```

### Live Variable Monitoring
```c
// In GDB
(gdb) display button_presses
(gdb) display led_state
(gdb) continue
```

## Common Issues and Solutions

### Issue: LED Not Blinking
**Symptoms:** LED stays on/off
**Checks:**
1. Clock enabled? `RCC->AHB1ENR & (1 << 0)`
2. Pin configured? `GPIOA->MODER`
3. Delay working? Try different value
4. Power supply OK?

### Issue: Button Not Responding
**Symptoms:** No interrupt triggered
**Checks:**
1. EXTI configured? `EXTI->IMR`
2. NVIC enabled? `NVIC_ISER`
3. SYSCFG mapping? `SYSCFG->EXTICR`
4. Pending flag cleared? `EXTI->PR`

### Issue: UART Not Working
**Symptoms:** No characters received/transmitted
**Checks:**
1. Baud rate correct?
2. TX/RX swapped?
3. GPIO AF configured?
4. UART enabled? `USART2->CR1 & (1 << 13)`

## Performance Metrics

### Typical Values (84 MHz)
| Operation | Time | Cycles |
|-----------|------|--------|
| GPIO toggle | 12 ns | 1 |
| UART TX char | 87 µs | 7300 |
| ISR latency | <100 ns | <10 |
| Context switch | <1 µs | <84 |

### Memory Usage
| Project | Flash | RAM |
|---------|-------|-----|
| LED Blink | ~2 KB | <100 B |
| Button Int | ~3 KB | <200 B |
| UART Echo | ~4 KB | ~300 B |

## Testing Checklist

- [ ] Compiles without warnings
- [ ] Flashes successfully
- [ ] Basic functionality works
- [ ] Edge cases handled
- [ ] Interrupts work correctly
- [ ] No memory leaks
- [ ] Error handling tested
- [ ] Performance acceptable
- [ ] Code documented
- [ ] README updated

## Future Project Ideas

1. **Temperature Monitor**
   - ADC + UART logging
   - Threshold alerts
   - Graph data

2. **PWM Motor Control**
   - Timer PWM
   - Speed control
   - Direction control

3. **I2C Sensor Interface**
   - OLED display
   - Environmental sensor
   - Data logging

4. **SPI Flash Storage**
   - Data logging
   - Configuration storage
   - Bootloader

5. **RTOS Application**
   - Multiple tasks
   - Semaphores/queues
   - Task scheduling

6. **USB CDC Device**
   - Virtual COM port
   - File transfer
   - Configuration interface

## References
- [01_Register_Access](../01_Register_Access/README.md) - Direct hardware access
- [02_HAL_Setup](../02_HAL_Setup/README.md) - Peripheral initialization
- [03_Interrupt_Handling](../03_Interrupt_Handling/README.md) - Interrupt concepts
- [04_Bare_Metal_Drivers](../04_Bare_Metal_Drivers/README.md) - Driver usage
- [05_Debugging_Advanced](../05_Debugging_Advanced/README.md) - Debugging techniques
- [Main Project README](../README.md)

---
*Part of the STM32 HAL Development Repository*

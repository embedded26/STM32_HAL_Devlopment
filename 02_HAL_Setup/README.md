# 02_HAL_Setup

## Overview
This directory contains examples demonstrating Hardware Abstraction Layer (HAL) setup procedures for STM32F446RE peripherals. These examples show how to properly initialize and configure various peripherals using both custom HAL drivers and standard configuration practices.

## Learning Objectives
- Master peripheral initialization sequences
- Understand clock configuration and management
- Learn proper HAL driver usage
- Configure communication peripherals (UART, SPI)
- Apply systematic setup procedures

## Files in This Directory

### 1. clock_configuration.c
**Purpose:** System clock setup and PLL configuration

**Key Concepts:**
- Clock tree architecture (HSI, HSE, PLL)
- PLL configuration for high performance
- Bus prescalers (AHB, APB1, APB2)
- Flash latency configuration
- Clock source switching

**Configuration Example:**
```c
// Target: 84 MHz from HSI via PLL
// HSI (16 MHz) -> PLL (PLLM=8, PLLN=168, PLLP=4) -> 84 MHz
configure_system_clock_pll();
```

**Learning Points:**
- HSI: 16 MHz internal RC oscillator (default)
- HSE: 8 MHz external crystal (Nucleo board)
- PLL multiplies input frequency
- APB1 limited to 45 MHz, APB2 to 90 MHz
- Flash wait states required for high frequencies

### 2. gpio_hal_setup.c
**Purpose:** GPIO HAL driver initialization and usage

**Key Concepts:**
- Using GPIO_Handle_t configuration structure
- Proper initialization sequence
- Clock enable before configuration
- Pin mode, speed, and type setup
- Input/output operations

**Configuration Example:**
```c
GPIO_Handle_t led;
led.pGPIOx = GPIOA;
led.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
GPIO_Init(&led);
```

**API Functions Demonstrated:**
- `GPIO_PeriClockControl()` - Enable peripheral clock
- `GPIO_Init()` - Initialize pin configuration
- `GPIO_WriteToOutputPin()` - Write to single pin
- `GPIO_ReadFromInputPin()` - Read from single pin
- `GPIO_ToggleOutputPin()` - Toggle pin state
- `GPIO_WriteToOutputPort()` - Write to entire port

### 3. uart_hal_setup.c
**Purpose:** UART/USART peripheral setup skeleton

**Key Concepts:**
- UART clock enablement (APB1/APB2)
- GPIO alternate function configuration
- Baud rate calculation and setup
- Frame format (data bits, parity, stop bits)
- TX/RX mode configuration

**UART Setup Sequence:**
1. Enable peripheral and GPIO clocks
2. Configure GPIO pins as alternate function
3. Calculate and set baud rate (BRR register)
4. Configure frame format (CR1, CR2)
5. Enable UART peripheral

**Baud Rate Calculation:**
```c
// For 115200 baud at 42 MHz APB1:
// BRR = f_PCLK / (16 * baud_rate)
// BRR = 42000000 / (16 * 115200) = 22.786
// Mantissa = 22, Fraction = 13
```

**Pins Used (USART2):**
- PA2: TX (Alternate Function 7)
- PA3: RX (Alternate Function 7)

### 4. spi_hal_setup.c
**Purpose:** SPI peripheral configuration skeleton

**Key Concepts:**
- SPI master/slave mode
- Clock polarity (CPOL) and phase (CPHA)
- Full-duplex, half-duplex, simplex modes
- Baud rate prescaler
- Data frame size (8/16-bit)

**SPI Setup Sequence:**
1. Enable SPI and GPIO clocks
2. Configure GPIO pins as alternate function
3. Configure SPI mode and parameters
4. Enable SPI peripheral

**SPI Clock Modes:**
| Mode | CPOL | CPHA | Description |
|------|------|------|-------------|
| 0 | 0 | 0 | Idle low, sample first edge |
| 1 | 0 | 1 | Idle low, sample second edge |
| 2 | 1 | 0 | Idle high, sample first edge |
| 3 | 1 | 1 | Idle high, sample second edge |

**Pins Used (SPI1):**
- PA5: SCK (Clock) - AF5
- PA6: MISO (Master In, Slave Out) - AF5
- PA7: MOSI (Master Out, Slave In) - AF5

## Common Peripheral Setup Pattern

All peripheral initialization follows this pattern:

```c
// 1. Enable clocks
RCC->APBxENR |= PERIPHERAL_CLOCK_BIT;
GPIO_PeriClockControl(GPIOx, ENABLE);

// 2. Configure GPIO (if needed)
// Set mode to alternate function
// Select appropriate AF number
// Configure speed, pull-up/down

// 3. Configure peripheral registers
// Set mode, speed, format, etc.

// 4. Enable peripheral
PERIPHERAL->CR1 |= ENABLE_BIT;
```

## Clock Tree Overview

```
HSI (16MHz) ─┐
HSE (8MHz)  ─┼─> PLL ─> /PLLP ─> System Clock (SYSCLK)
             │                    |
             │                    ├─> /AHB_Prescaler ─> AHB (CPU, DMA)
             │                    ├─> /APB1_Prescaler ─> APB1 (42MHz max)
             │                    └─> /APB2_Prescaler ─> APB2 (84MHz max)
             │
             └─> Direct selection (bypass PLL)
```

## Important Considerations

### Clock Configuration
- Always configure Flash latency before increasing frequency
- Wait for clock source to stabilize (RDY flag)
- Verify clock switch completion (SWS bits)
- APB1 ≤ 45 MHz, APB2 ≤ 90 MHz (STM32F446RE limits)

### GPIO Configuration
- Enable peripheral clock first
- Configure mode before other parameters
- Use appropriate speed for application
- Remember alternate function numbers

### UART Configuration
- APB1 for USART2/3, APB2 for USART1/6
- Baud rate depends on APB clock frequency
- Recalculate BRR if clock changes
- Enable both TX and RX in CR1

### SPI Configuration
- APB2 typically faster for SPI1
- Match CPOL/CPHA to slave device
- Software slave management for manual CS control
- Wait for TXE before transmit, RXNE before receive

## Compilation

```bash
# Clock configuration example
gcc clock_configuration.c -I../drivers/inc -o clock_example
./clock_example

# GPIO HAL example  
gcc gpio_hal_setup.c -I../drivers/inc ../drivers/src/stm32f446re_gpio_drivers.c -o gpio_hal_example
./gpio_hal_example

# UART setup example
gcc uart_hal_setup.c -I../drivers/inc -o uart_example
./uart_example

# SPI setup example
gcc spi_hal_setup.c -I../drivers/inc -o spi_example
./spi_example
```

## Hardware Requirements
- STM32F446RE Nucleo board
- USB cable for programming
- Optional: UART-USB converter for USART2 testing
- Optional: SPI device for SPI1 testing

## Common Mistakes to Avoid

❌ **Don't:**
```c
// Configuring peripheral before enabling clock
GPIO_Init(&handle);  // Clock not enabled!

// Wrong alternate function number
GPIOA->AFR[0] |= (0x2 << 8);  // Wrong AF for USART2

// Enabling peripheral before full configuration
SPI1->CR1 |= (1 << 6);  // Enable before configuring
SPI1->CR1 |= (1 << 2);  // Configure after enabling - ERROR!
```

✅ **Do:**
```c
// Enable clock first
GPIO_PeriClockControl(GPIOA, ENABLE);
GPIO_Init(&handle);

// Correct AF number
GPIOA->AFR[0] |= (0x7 << 8);  // AF7 for USART2

// Full configuration before enabling
SPI1->CR1 = config_value;  // Configure everything
SPI1->CR1 |= (1 << 6);     // Then enable
```

## Debugging Tips

1. **Clock Issues:**
   - Verify RCC->AHB1ENR / APB1ENR / APB2ENR bits
   - Check HSI/HSE/PLL ready flags
   - Confirm SWS matches SW selection

2. **GPIO Issues:**
   - Ensure clock enabled before access
   - Verify MODER bits (2 bits per pin)
   - Check AFR selection for peripherals

3. **UART Issues:**
   - Verify baud rate calculation
   - Check APB clock frequency
   - Ensure GPIO AF configured correctly

4. **SPI Issues:**
   - Verify CPOL/CPHA match slave
   - Check master mode bit (MSTR)
   - Wait for TXE/RXNE flags

## Next Steps
After mastering HAL setup, proceed to:
- **03_Interrupt_Handling**: Configure and handle interrupts
- **04_Bare_Metal_Drivers**: Build complete peripheral drivers
- **06_Real_World_Projects**: Apply concepts in practical projects

## References
- STM32F446RE Reference Manual (RM0390)
- STM32F446RE Datasheet
- [Main Project README](../README.md)
- STM32F4 HAL User Manual

---
*Part of the STM32 HAL Development Repository*

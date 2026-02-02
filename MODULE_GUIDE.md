# 📚 Module Guide - STM32 HAL Development

Comprehensive documentation for all modules, drivers, and utilities.

---

## 📑 Table of Contents

1. [Virtual Simulation Framework](#virtual-simulation-framework)
2. [Multi-Board Support](#multi-board-support)
3. [GPIO Driver](#gpio-driver)
4. [Debug Utilities](#debug-utilities)
5. [Example Modules](#example-modules)
6. [CI/CD Pipeline](#cicd-pipeline)

---

## 🎮 Virtual Simulation Framework

**Location**: `07_Virtual_Simulation/`

### Overview
Software-based testing environment that simulates STM32 peripherals without hardware.

### Features
- ✅ Virtual GPIO with 16 pins per port
- ✅ Virtual NVIC with 240 IRQ lines
- ✅ Virtual ADC with 16 channels
- ✅ HAL-compatible wrapper API
- ✅ Error injection for testing

### Components

#### 1. Virtual GPIO (`sim_gpio.c`)
Simulates GPIO peripheral with full functionality:

```c
#include "sim_gpio.c"

// Initialize GPIO
SimGPIO_Init();

// Configure pin
SimGPIO_ConfigPin(0, 5, SIM_GPIO_MODE_OUTPUT, SIM_GPIO_PP);

// Write pin
SimGPIO_WritePin(0, 5, 1);

// Read pin
uint8_t value = SimGPIO_ReadPin(0, 5);

// Cleanup
SimGPIO_Cleanup();
```

**Capabilities**:
- Pin modes: Input, Output, Alternate Function, Analog
- Output types: Push-pull, Open-drain
- Pull-up/Pull-down configuration
- Pin multiplexing simulation
- Interrupt simulation

#### 2. Virtual NVIC (`sim_nvic.c`)
Simulates Nested Vectored Interrupt Controller:

```c
#include "sim_nvic.c"

// Initialize NVIC
SimNVIC_Init();

// Enable interrupt with priority
SimNVIC_EnableIRQ(23, 5);  // IRQ 23, priority 5

// Trigger interrupt
SimNVIC_TriggerIRQ(23);

// Execute pending interrupts
SimNVIC_ExecutePending();
```

**Features**:
- 240 interrupt lines (0-239)
- 16 priority levels (0-15, lower = higher priority)
- Priority-based execution
- Pending/active state tracking

#### 3. Virtual ADC (`sim_adc.c`)
Simulates 12-bit ADC:

```c
#include "sim_adc.c"

// Initialize ADC
SimADC_Init();

// Set channel voltage
SimADC_SetChannelVoltage(0, 1.65f);  // Channel 0, 1.65V

// Read ADC (12-bit)
uint16_t raw = SimADC_Read(0);  // Returns 0-4095

// Convert to voltage
float voltage = SimADC_ReadVoltage(0);
```

**Capabilities**:
- 16 channels (0-15)
- 12-bit resolution (0-4095)
- 0-3.3V reference voltage
- Single/continuous conversion

### Build & Test

```bash
cd 07_Virtual_Simulation

# Build all
make

# Run individual tests
./build/test_gpio
./build/test_nvic
./build/test_adc
./build/test_hal_wrapper

# Clean
make clean
```

---

## 🎯 Multi-Board Support

**Location**: `drivers/inc/board_support/`

### Board Configuration System

Automatic board selection and peripheral mapping for multiple STM32 families.

#### Usage

```c
// Method 1: Define before include
#define STM32F4XX
#include "board_support/board_config.h"

// Method 2: Compile-time flag
// gcc -DSTM32F1XX ...

// Get board info
const char* series = Board_GetSeriesName();    // "STM32F4"
uint32_t sysclk = Board_GetSystemClock();      // 180000000

// Check peripheral availability
BoardPeripherals_t periph = Board_GetPeripherals();
if (periph.has_usart6) {
    // USART6 available
}
```

### Supported Boards

#### STM32F0 Series
- **File**: `stm32f0xx.h`
- **Clock**: 48 MHz
- **GPIO**: Ports A-F (6 ports)
- **Peripherals**: USART1/2, SPI1/2, I2C1/2, ADC1

#### STM32F1 Series
- **File**: `stm32f1xx.h`
- **Clock**: 72 MHz
- **GPIO**: Ports A-G (7 ports)
- **Peripherals**: USART1/2/3, SPI1/2, I2C1/2, ADC1/2
- **Note**: Different GPIO register structure (CRL/CRH)

#### STM32F4 Series (Default)
- **File**: `stm32f446re.h`
- **Clock**: 180 MHz
- **GPIO**: Ports A-I (9 ports)
- **Peripherals**: USART1/2/3/6, UART4/5, SPI1/2/3, I2C1/2/3, ADC1/2/3

### Adding New Board

1. Create header file: `stm32fXxx.h`
2. Define base addresses
3. Define register structures
4. Add to `board_config.h`
5. Update peripheral detection

---

## 🔌 GPIO Driver

**Location**: `drivers/inc/stm32f446re_gpio_drivers.h`, `drivers/src/stm32f446re_gpio_drivers.c`

### API Reference

#### Peripheral Clock Control

```c
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);
```

Enable/disable peripheral clock for GPIO port.

**Parameters**:
- `pGPIOx`: GPIO port (GPIOA, GPIOB, etc.)
- `EnorDi`: ENABLE or DISABLE

**Example**:
```c
GPIO_PeriClockControl(GPIOA, ENABLE);
```

#### Initialization

```c
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
```

Initialize GPIO pin with configuration.

**Example**:
```c
GPIO_Handle_t gpio_led;
gpio_led.pGPIOx = GPIOA;
gpio_led.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
gpio_led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
gpio_led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
gpio_led.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
gpio_led.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;

GPIO_Init(&gpio_led);
```

#### Read Operations

```c
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
```

**Example**:
```c
uint8_t button_state = GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13);
uint16_t port_value = GPIO_ReadFromInputPort(GPIOA);
```

#### Write Operations

```c
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
```

**Example**:
```c
GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_SET);
GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
```

#### De-initialization

```c
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);
```

Reset GPIO port to default state.

### Pin Configuration Options

#### Pin Modes
```c
#define GPIO_MODE_IN        0  // Input mode
#define GPIO_MODE_OUT       1  // Output mode
#define GPIO_MODE_ALTFN     2  // Alternate function
#define GPIO_MODE_ANALOG    3  // Analog mode
#define GPIO_MODE_IT_FT     4  // Interrupt falling edge
#define GPIO_MODE_IT_RT     5  // Interrupt rising edge
#define GPIO_MODE_IT_RFT    6  // Interrupt both edges
```

#### Output Types
```c
#define GPIO_OP_TYPE_PP     0  // Push-pull
#define GPIO_OP_TYPE_OD     1  // Open-drain
```

#### Speeds
```c
#define GPIO_SPEED_LOW      0
#define GPIO_SPEED_MEDIUM   1
#define GPIO_SPEED_FAST     2
#define GPIO_SPEED_HIGH     3
```

#### Pull-up/Pull-down
```c
#define GPIO_NO_PUPD        0  // No pull-up/down
#define GPIO_PIN_PU         1  // Pull-up
#define GPIO_PIN_PD         2  // Pull-down
```

---

## 🐛 Debug Utilities

**Location**: `drivers/inc/debug_utils.h`

### Debug Levels

```c
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4
#define DEBUG_LEVEL_TRACE   5
```

Set at compile time:
```bash
gcc -DDEBUG_LEVEL=DEBUG_LEVEL_DEBUG ...
```

### Logging Macros

```c
DEBUG_ERROR("Failed to initialize: %d", error_code);
DEBUG_WARN("Buffer almost full: %d%%", usage);
DEBUG_INFO("System started");
DEBUG_LOG("Processing data: %d bytes", size);
DEBUG_TRACE("Entering function");
```

### Assertions

```c
#define DEBUG_ENABLE_ASSERT
#include "debug_utils.h"

DEBUG_ASSERT(pointer != NULL);
DEBUG_ASSERT(value < MAX_VALUE);
```

### Memory Inspection

```c
uint8_t buffer[256];
Debug_DumpMemory(buffer, sizeof(buffer), "My Buffer");
```

Output:
```
=== Memory Dump: My Buffer ===
Address: 0x20000000, Length: 256 bytes
Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
-------  -----------------------------------------------
00000000 48 65 6C 6C 6F 20 57 6F 72 6C 64 00 ...
```

### Register Dumps

```c
Debug_DumpRegister(&GPIOA->MODER, "GPIOA_MODER");
Debug_DumpRegister(&GPIOA->ODR, "GPIOA_ODR");

// Or dump multiple
const char* gpio_regs[] = {"MODER", "OTYPER", "OSPEEDR", "PUPDR", "IDR", "ODR"};
Debug_DumpRegisters((uint32_t*)GPIOA, gpio_regs, 6);
```

### Performance Profiling

```c
DebugProfiler_t profiler;

Debug_ProfileStart(&profiler, "Data Processing");
// ... code to profile ...
Debug_ProfileStop(&profiler);  // Prints: [PROF] Data Processing: 12345 cycles
```

### Bit Manipulation Helpers

```c
BIT_SET(GPIOA->ODR, 5);        // Set bit 5
BIT_CLEAR(GPIOA->ODR, 5);      // Clear bit 5
BIT_TOGGLE(GPIOA->ODR, 5);     // Toggle bit 5
uint8_t val = BIT_READ(GPIOA->IDR, 13);  // Read bit 13
BIT_WRITE(GPIOA->ODR, 5, 1);   // Write 1 to bit 5
```

---

## 📁 Example Modules

### 01_Register_Access
Direct hardware register manipulation.

**Files**:
- `direct_register_read.c` - Reading registers
- `direct_register_write.c` - Writing registers
- `register_bitfield.c` - Bitfield operations
- `memory_mapped_io.c` - Memory-mapped I/O

**Learning Objectives**:
- Understand memory-mapped registers
- Learn bit manipulation
- Practice direct hardware access

### 02_HAL_Setup
Peripheral configuration using HAL layer.

**Files**:
- `clock_configuration.c` - System clock setup
- `gpio_hal_setup.c` - GPIO initialization
- `uart_hal_setup.c` - UART configuration
- `spi_hal_setup.c` - SPI configuration

**Learning Objectives**:
- Clock tree configuration
- Peripheral initialization sequences
- HAL driver usage

### 03_Interrupt_Handling
Interrupt controller and handlers.

**Files**:
- `interrupt_basics.c` - NVIC basics
- `exti_gpio_interrupt.c` - GPIO interrupts
- `timer_interrupt.c` - Timer interrupts

**Learning Objectives**:
- NVIC configuration
- Interrupt priorities
- ISR implementation

### 04_Bare_Metal_Drivers
Using custom drivers.

**Files**:
- `gpio_driver_example.c` - GPIO driver usage
- `simple_delay.c` - Delay implementations

**Learning Objectives**:
- Driver API usage
- Timing and delays
- Hardware abstraction

### 05_Debugging_Advanced
Advanced debugging techniques.

**Files**:
- `debug_example.c` - Debug utilities
- `memory_dump_example.c` - Memory inspection
- `profiling_example.c` - Performance profiling

**Learning Objectives**:
- Debug logging
- Memory analysis
- Performance optimization

### 06_Real_World_Projects
Complete working applications.

**Files**:
- `led_blink.c` - LED blink project
- `button_interrupt.c` - Button with interrupt
- `uart_echo.c` - UART echo program

**Learning Objectives**:
- Project structure
- Integration testing
- Real-world applications

---

## 🔄 CI/CD Pipeline

**Location**: `.github/workflows/ci.yml`

### Pipeline Jobs

#### 1. Build and Test
- Builds virtual simulation
- Runs all test executables
- Validates build artifacts

#### 2. Code Quality
- Checks file permissions
- Detects trailing whitespace
- Verifies documentation

#### 3. Multi-Compiler
- Tests GCC 9, 10, 11, 12
- Ensures compatibility
- Matrix build strategy

#### 4. Documentation
- Validates README structure
- Checks documentation links
- Verifies required files

### Triggering CI

```bash
# Push to main/develop
git push origin main

# Pull request
git push origin feature/my-feature
# Then create PR on GitHub

# Manual trigger
# Go to Actions tab → Select workflow → Run workflow
```

### Pre-commit Hooks

Install:
```bash
pip install pre-commit
pre-commit install
```

Runs automatically on `git commit`:
- Trailing whitespace removal
- End-of-file fixer
- YAML syntax check
- Large file check
- Build verification
- Test execution

---

## 🔗 Cross-References

- **Quick Start**: [QUICK_START.md](QUICK_START.md)
- **Troubleshooting**: [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
- **Main README**: [README.md](README.md)
- **Simulation Guide**: [Documentation/SIMULATION_GUIDE.md](Documentation/SIMULATION_GUIDE.md)

---

*Last updated: 2026-02-02*

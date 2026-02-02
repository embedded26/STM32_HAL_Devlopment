# 🚀 STM32 HAL Development Guide

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║          🎯  STM32 HAL Development: From Registers to Real Systems          ║
║                                                                              ║
║     Direct Register Access • HAL Setup • Interrupt Handling • Debugging     ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

---

## 📋 Table of Contents

- [🎯 Project Overview](#project-overview)
- [🎯 Board Selection Guide](#board-selection-guide)
- [🏗️ Repository Structure](#repository-structure)
- [⚙️ Core Concepts](#core-concepts)
- [🔧 Getting Started](#getting-started)
- [📚 Module Breakdown](#module-breakdown)
- [🎓 Learning Path](#learning-path)
- [🐛 Debugging Guide](#debugging-guide)
- [⚡ Performance Tips](#performance-tips)
- [🤝 Contributing](#contributing)
- [📖 Resources](#resources)

### 📚 Quick Documentation Links

- 🚀 **[QUICK_START.md](QUICK_START.md)** - Get started in 5 minutes
- 📖 **[MODULE_GUIDE.md](MODULE_GUIDE.md)** - Detailed API and module documentation
- 🔧 **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Common issues and solutions
- 🧪 **[SIMULATION_GUIDE.md](Documentation/SIMULATION_GUIDE.md)** - Virtual testing guide

---

## 🎯 Project Overview

This repository provides a **comprehensive educational framework** for mastering STM32 microcontroller development at multiple abstraction levels:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Abstraction Layers                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  Application Layer                                       │  │
│  │  (Your Projects: Sensors, Motors, Communications)        │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              ▲                                  │
│                              │                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  HAL Abstraction Layer                                   │  │
│  │  (Setup, Configuration, Higher-Level Operations)         │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              ▲                                  │
│                              │                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  Bare-Metal Layer                                        │  │
│  │  (Direct Register Access, Low-Level Control)             │  │
│  └──────────────────────────────────────────────────────────┘  │
│                              ▲                                  │
│                              │                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  Hardware (STM32 MCU)                                    │  │
│  │  (Registers, Peripherals, Memory)                        │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Key Features:

✅ **Direct Register Access** - Understand how hardware really works  
✅ **HAL Setup Tutorials** - Properly configure microcontroller peripherals  
✅ **Interrupt Handling** - Master interrupt controllers and handlers  
✅ **Bare-Metal Drivers** - Design efficient drivers from scratch  
✅ **Advanced Debugging** - Use debugging tools effectively  
✅ **Virtual Simulation** - Test without hardware using software simulation  
✅ **Multi-Board Support** - F0, F1, F4 series with automatic peripheral mapping  
✅ **CI/CD Pipeline** - Automated testing and validation  
✅ **Real-World Examples** - Practical implementations you can learn from  

### 🎯 Ultra-Novelty Features

This repository includes **industry-first features** not found in standard STM32 tutorials:

1. **🎮 Virtual Board Simulator** - Complete software simulation layer
   - Test GPIO, NVIC, ADC without hardware
   - Interrupt simulation framework
   - Error injection for robust testing

2. **🔄 Multi-Board Selector Pattern** - Seamless board switching
   - Support for F0/F1/F4 variants
   - Automatic clock configuration
   - Peripheral availability detection

3. **🛠️ Advanced Driver Abstraction** - Production-ready patterns
   - Universal GPIO driver interface
   - Complete error handling framework
   - Interrupt priority management

4. **🐛 Development Tools Suite** - Professional debugging
   - Advanced macro logging system
   - Memory inspection utilities
   - Performance profiling helpers
   - Register dump utilities

---

## 🎯 Board Selection Guide

### Supported Boards

| Series | Example Board | Clock | GPIO Ports | Key Features |
|--------|---------------|-------|------------|--------------|
| **STM32F0** | Nucleo-F030R8 | 48 MHz | A-F (6) | Entry-level, USB support |
| **STM32F1** | Nucleo-F103RB | 72 MHz | A-G (7) | Classic ARM MCU |
| **STM32F4** | Nucleo-F446RE | 180 MHz | A-I (9) | High performance, FPU |

### Quick Board Selection

**Method 1: Compile-time definition**
```c
#define STM32F4XX  // or STM32F1XX or STM32F0XX
#include "board_support/board_config.h"
```

**Method 2: Compiler flag**
```bash
arm-none-eabi-gcc -DSTM32F1XX -o output.elf source.c
```

**Method 3: Makefile**
```makefile
# Select board
BOARD_SERIES = STM32F4XX
CFLAGS += -D$(BOARD_SERIES)
```

### Automatic Features

When you select a board, the system automatically provides:
- ✅ Correct peripheral base addresses
- ✅ Clock tree configuration
- ✅ GPIO port availability
- ✅ Peripheral detection (UART, SPI, I2C, ADC)
- ✅ Memory map alignment

**Example:**
```c
#define STM32F1XX
#include "board_support/board_config.h"

// Get board information
const char* series = Board_GetSeriesName();    // "STM32F1"
uint32_t clock = Board_GetSystemClock();       // 72000000

// Check peripherals
BoardPeripherals_t p = Board_GetPeripherals();
if (p.has_usart3) {
    // USART3 is available on this board
}
```  

---

## 🏗️ Repository Structure

```
STM32_HAL_Development/
│
├── 📁 01_Register_Access/
│   ├── direct_register_read.c       ← Read GPIO registers
│   ├── direct_register_write.c      ← Modify GPIO registers
│   ├── register_bitfield.c          ← Bitfield operations
│   ├── memory_mapped_io.c           ← Memory-mapped I/O concepts
│   └── examples.md                  ← Detailed register examples
│
├── 📁 02_HAL_Setup/
│   ├── clock_configuration.c        ← System clock setup
│   ├── gpio_hal_setup.c             ← GPIO HAL initialization
│   ├── uart_hal_setup.c             ← UART peripheral setup
│   ├── spi_hal_setup.c              ← SPI peripheral setup
│   ├── i2c_hal_setup.c              ← I2C peripheral setup
│   ├── timer_hal_setup.c            ← Timer configuration
│   ├── adc_hal_setup.c              ← ADC setup
│   └── hal_utils.c                  ← Common HAL utilities
│
├── 📁 03_Interrupt_Handling/
│   ├── interrupt_basics.c           ← Interrupt concepts & examples
│   ├── nvic_configuration.c         ← NVIC setup
│   ├── gpio_interrupt_handler.c     ← GPIO interrupt implementation
│   ├── timer_interrupt_handler.c    ← Timer interrupt ISR
│   ├── uart_interrupt_handler.c     ← UART RX/TX interrupts
│   ├── isr_context_switch.c         ← Context management in ISR
│   └── interrupt_priorities.c       ← Priority configuration
│
├── 📁 04_Bare_Metal_Drivers/
│   ├── gpio_driver.c                ← GPIO driver from scratch
│   ├── uart_driver.c                ← UART driver implementation
│   ├── i2c_driver.c                 ← I2C driver with bit-banging
│   ├── spi_driver.c                 ← SPI driver
│   ├── timer_driver.c               ← Timer driver
│   ├── driver_interface.h           ← Standard driver interface
│   └── driver_testing.c             ← Driver test suite
│
├── 📁 05_Debugging_Advanced/
│   ├── debug_basics.md              ← Debugging fundamentals
│   ├── breakpoint_usage.c           ← Breakpoint examples
│   ├── watch_variables.c            ← Watch variable setup
│   ├── semihosting.c                ← Semihosting implementation
│   ├── uart_debug_output.c          ← Debug via UART
│   ├── memory_inspection.c          ← Memory dump utilities
│   ├── performance_profiling.c      ← Performance measurement
│   └── gdb_scripting.py             ← GDB automation scripts
│
├── 📁 06_Real_World_Projects/
│   ├── 01_GPIO_LED_Blink/
│   │   ├── main.c
│   │   ├── Makefile
│   │   └── README.md
│   ├── 02_UART_Communication/
│   │   ├── main.c
│   │   ├── uart_handler.c
│   │   └── README.md
│   ├── 03_ADC_Sensor_Reading/
│   │   ├── main.c
│   │   ├── adc_handler.c
│   │   └── README.md
│   ├── 04_SPI_SD_Card/
│   │   ├── main.c
│   │   ├── spi_driver.c
│   │   └── README.md
│   ├── 05_I2C_OLED_Display/
│   │   ├── main.c
│   │   ├── i2c_driver.c
│   │   └── README.md
│   └── 06_Interrupts_Button_LED/
│       ├── main.c
│       ├── interrupt_handler.c
│       └── README.md
│
├── 📁 07_Virtual_Simulation/           ← 🆕 Virtual driver testing framework
│   ├── sim_gpio.c                     ← Virtual GPIO driver with pin-mux
│   ├── sim_nvic.c                     ← Virtual NVIC interrupt controller
│   ├── sim_hal_wrapper.c              ← HAL-compatible API wrapper
│   ├── sim_adc.c                      ← Virtual ADC simulation
│   ├── Makefile                       ← Build system for tests
│   └── README.md                      ← Virtual simulation guide
│
├── 📁 Tools_Config/
│   ├── STM32_Datasheet_Reference.md ← Pin configurations
│   ├── Memory_Map_Guide.md          ← Memory layout reference
│   ├── Peripheral_Addresses.c       ← Peripheral base addresses
│   └── CubeMX_Export_Guide.md       ← CubeMX workflow
│
├── 📁 Documentation/
│   ├── INTERRUPT_TUTORIAL.md        ← Complete interrupt guide
│   ├── REGISTER_GUIDE.md            ← Register reference
│   ├── HAL_BEST_PRACTICES.md        ← Best practices
│   ├── DEBUGGING_GUIDE.md           ← Advanced debugging
│   ├── PERFORMANCE_OPTIMIZATION.md  ← Optimization tips
│   ├── SIMULATION_GUIDE.md          ← 🆕 Virtual & QEMU simulation guide
│   └── TROUBLESHOOTING.md           ← Common issues & fixes
│
├── Makefile                          ← Build configuration
├── CMakeLists.txt                    ← CMake alternative
├── .gitignore                        ← Git ignore rules
├── LICENSE                           ← License (MIT)
└── README.md                         ← This file!
```

---

## ⚙️ Core Concepts

### 1️⃣ Direct Register Access

```c
// ═══════════════════════════════════════════════════════════════
// CONCEPT: Direct register access = hardware control at lowest level
// ═══════════════════════════════════════════════════════════════

// Example: Toggle GPIO Pin via Direct Register Access
#include "stm32f4xx.h"

// Method 1: Using Base Address + Offset
#define GPIOA_BASE  0x40020000
#define GPIO_ODR    0x14

int main(void) {
    // Create pointer to GPIO A output data register
    volatile uint32_t *GPIO_A_ODR = (volatile uint32_t *)(GPIOA_BASE + GPIO_ODR);
    
    // Toggle pin 5
    *GPIO_A_ODR ^= (1 << 5);  // Toggle PA5
    
    return 0;
}

// Method 2: Using Device Header Definitions (Better Practice)
int main(void) {
    // GPIOA->ODR structure already defined in stm32f4xx.h
    GPIOA->ODR ^= GPIO_PIN_5;  // Toggle PA5
    
    return 0;
}
```

**Key Concepts:**
- 📍 **Memory-Mapped I/O**: Registers are at fixed memory addresses
- 🔲 **Volatile Keyword**: Prevents compiler optimization on register reads
- 📝 **Bit Manipulation**: Control individual pins using bit operations

---

### 2️⃣ HAL Setup Process

```
┌────────────────────────────────────────────────────────────┐
│               HAL Initialization Flow                       │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  Step 1: Reset System                                     │
│  └─> HAL_Init()                                           │
│      └─ Sets up SysTick timer (1ms interrupt)             │
│      └─ Initializes HAL variables                         │
│                                                            │
│  Step 2: Configure System Clock                           │
│  └─> SystemClock_Config()                                 │
│      └─ Configure PLL                                     │
│      └─ Set CPU frequency (typically 84-180 MHz)          │
│      └─ Configure peripheral clocks                       │
│                                                            │
│  Step 3: Initialize Peripherals                           │
│  └─> HAL_GPIO_Init() / HAL_UART_Init() / etc.             │
│      └─ Enable peripheral clock                           │
│      └─ Configure GPIO pins / peripheral registers        │
│      └─ Setup interrupts if needed                        │
│                                                            │
│  Step 4: Enable Interrupts (Optional)                     │
│  └─> HAL_NVIC_SetPriority() + HAL_NVIC_EnableIRQ()        │
│      └─ Setup interrupt priorities                        │
│      └─ Enable specific interrupts                        │
│                                                            │
│  Step 5: Run Application                                  │
│  └─> Main application loop                                │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

### 3️⃣ Interrupt Handling

```
┌──────────────────────────────────────────────────────────────┐
│              Interrupt Execution Timeline                     │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─ Main Code Running ─┐                                    │
│  │ void main() {       │                                    │
│  │   // Normal Code    │──┐                                 │
│  │ }                   │  │                                 │
│  └─────────────────────┘  │                                 │
│                           │ Interrupt Triggered!            │
│                           │ (e.g., GPIO Pin Change)        │
│                           ▼                                  │
│  ┌──────────────────────────────────────────┐               │
│  │ 1. SAVE CONTEXT (CPU registers)          │               │
│  │    - Save R0-R3, R12, LR, PC, PSR        │               │
│  └──────────────────────────────────────────┘               │
│                           ▼                                  │
│  ┌──────────────────────────────────────────┐               │
│  │ 2. EXECUTE ISR (Interrupt Service Rtn)  │               │
│  │    void EXTI0_IRQHandler(void) { ... }  │               │
│  └──────────────────────────────────────────┘               │
│                           ▼                                  │
│  ┌──────────────────────────────────────────┐               │
│  │ 3. RESTORE CONTEXT                       │               │
│  │    - Restore all saved registers         │               │
│  └──────────────────────────────────────────┘               │
│                           ▼                                  │
│  ┌─ Resume Main Code ──┐                                    │
│  │ // Continue from    │                                    │
│  │ // where interrupted│                                    │
│  └─────────────────────┘                                    │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

### 4️⃣ Bare-Metal Driver Design

```c
// ═══════════════════════════════════════════════════════════════
// DRIVER DESIGN PATTERN
// ═══════════════════════════════════════════════════════════════

// 1. Define Driver Structure
typedef struct {
    uint32_t base_address;
    uint8_t is_initialized;
    void (*callback)(void);
} GPIO_Driver_t;

// 2. Implement Initialization
void GPIO_Driver_Init(GPIO_Driver_t *driver, uint32_t base) {
    driver->base_address = base;
    driver->is_initialized = 1;
    // Configure registers...
}

// 3. Implement Operations
void GPIO_Driver_SetPin(GPIO_Driver_t *driver, uint8_t pin) {
    volatile uint32_t *reg = (volatile uint32_t *)driver->base_address;
    *reg |= (1 << pin);
}

// 4. Implement Error Handling
typedef enum {
    GPIO_OK,
    GPIO_ERROR,
    GPIO_NOT_INITIALIZED
} GPIO_Status_t;
```

---

## 🔧 Getting Started

### Prerequisites

```bash
# Essential Tools
✓ ARM Embedded Toolchain (arm-none-eabi-gcc)
✓ STM32CubeMX (for code generation)
✓ ST-Link Utility or OpenOCD (for programming)
✓ IDE: STM32CubeIDE / VSCode / Keil uVision
✓ Debugger: ST-Link V2 or compatible

# Optional Tools
✓ GDB (GNU Debugger)
✓ Serial Terminal (PuTTY, Minicom, etc.)
✓ Logic Analyzer or Oscilloscope
```

### Installation Steps

```bash
# 1. Clone Repository
git clone https://github.com/yourusername/STM32_HAL_Development.git
cd STM32_HAL_Development

# 2. Install ARM Toolchain
# Ubuntu/Debian:
sudo apt-get install arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib

# 3. Build Example Project
cd 06_Real_World_Projects/01_GPIO_LED_Blink
make clean
make

# 4. Flash to Board
make flash

# 5. Monitor Serial Output
make monitor
```

---

## 📚 Module Breakdown

### 📍 Module 1: Register Access

| Topic | Description | Key Files |
|-------|-------------|-----------|
| **Bit Manipulation** | AND, OR, XOR, SHIFT operations | `register_bitfield.c` |
| **Memory-Mapped I/O** | Understanding peripheral addressing | `memory_mapped_io.c` |
| **Register Read/Write** | Safe register access patterns | `direct_register_*.c` |
| **Bit Fields** | Using C bitfields vs manual operations | Examples included |

**Example: Bit Manipulation Operations**

```c
// Set bit
#define SET_BIT(reg, bit)     ((reg) |= (1U << (bit)))

// Clear bit
#define CLEAR_BIT(reg, bit)   ((reg) &= ~(1U << (bit)))

// Toggle bit
#define TOGGLE_BIT(reg, bit)  ((reg) ^= (1U << (bit)))

// Read bit
#define READ_BIT(reg, bit)    (((reg) >> (bit)) & 1U)

// Set multiple bits
#define SET_BITS(reg, mask)   ((reg) |= (mask))

// Clear multiple bits
#define CLEAR_BITS(reg, mask) ((reg) &= ~(mask))

// Check if bits are set
#define ARE_BITS_SET(reg, mask) (((reg) & (mask)) == (mask))
```

---

### ⚙️ Module 2: HAL Setup

**Clock Configuration Flow:**

```
┌──────────────────────────┐
│  External Oscillator     │ (8 MHz Crystal)
│  or Internal RC Oscillator  │ (16 MHz)
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│  PLL (Phase-Locked Loop) │ Multiply frequency
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│  System Clock Selector   │ Choose clock source
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│  Bus Dividers (AHB, APB) │ Distribute clocks
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│  Peripheral Clocks       │ Enable specific peripherals
└──────────────────────────┘
```

---

### 🎯 Module 3: Interrupt Handling

**NVIC (Nested Vectored Interrupt Controller) Structure:**

```
┌─────────────────────────────────────────────────────────┐
│                   STM32 Interrupts                       │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  System Exceptions (0-15):                             │
│  • Reset, NMI, HardFault, SVC, PendSV, SysTick        │
│                                                         │
│  External Interrupts (16-239):                         │
│  • GPIO, UART, SPI, I2C, Timer, ADC, etc.             │
│                                                         │
│  ┌─────────────────────────────────────┐              │
│  │ Priority Levels: 0 (Highest)         │              │
│  │              to 255 (Lowest)         │              │
│  │                                     │              │
│  │ Preemption: Can interrupt others    │              │
│  │ Sub-priority: Order within same     │              │
│  └─────────────────────────────────────┘              │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

---

### 🔧 Module 4: Bare-Metal Drivers

**Driver Architecture:**

```
┌─────────────────────────────────────────────────────┐
│            Bare-Metal Driver Structure              │
├─────────────────────────────────────────────────────┤
│                                                     │
│  1. HARDWARE ABSTRACTION LAYER (HAL)               │
│     └─ Encapsulates register details               │
│                                                     │
│  2. STATE MANAGEMENT                               │
│     └─ Track device state, open/closed, busy       │
│                                                     │
│  3. ERROR HANDLING                                 │
│     └─ Return status codes, set error flags        │
│                                                     │
│  4. INTERRUPT HANDLING                             │
│     └─ ISR callbacks, context preservation         │
│                                                     │
│  5. RESOURCE MANAGEMENT                            │
│     └─ Clock gating, power management              │
│                                                     │
└─────────────────────────────────────────────────────┘
```

---

### 🐛 Module 5: Debugging

**Debugging Workflow:**

```
┌─────────────────┐
│  Start Debugging│
└────────┬────────┘
         │
         ▼
┌────────────────────────┐
│  1. Set Breakpoints    │ Program halts at line
│     - Line break       │ - Check variables
│     - Conditional      │ - Watch expressions
│     - Function entry   │
└────────┬───────────────┘
         │
         ▼
┌────────────────────────┐
│  2. Run to Breakpoint  │ Inspect state
└────────┬───────────────┘
         │
         ▼
┌────────────────────────┐
│  3. Step Through Code  │ Step-over (skip functions)
│     - Step Into        │ Step-into (enter functions)
│     - Step Out         │ Step-out (return from function)
└────────┬───────────────┘
         │
         ▼
┌────────────────────────┐
│  4. Inspect Memory     │ View registers, variables
│     - Watch Variables  │ Memory dump
│     - Memory View      │
└────────┬───────────────┘
         │
         ▼
┌────────────────────────┐
│  5. Modify & Continue  │ Change variable value
│     - Hot-Fix          │ Resume execution
└────────┬───────────────┘
         │
         ▼
┌─────────────────┐
│  Bug Found!     │
└─────────────────┘
```

---

### 🧪 Module 7: Virtual Simulation Framework

**Virtual Driver Testing Architecture:**

```
┌─────────────────────────────────────────────────────────┐
│           Virtual Simulation Framework                   │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌────────────────────────────────────┐                │
│  │  Application/Test Code             │                │
│  └───────────────┬────────────────────┘                │
│                  │                                      │
│  ┌───────────────▼────────────────────┐                │
│  │  HAL Abstraction Layer             │                │
│  │  • HAL_GPIO_Init()                 │                │
│  │  • HAL_GPIO_WritePin()             │                │
│  │  • HAL_NVIC_EnableIRQ()            │                │
│  └───────────────┬────────────────────┘                │
│                  │                                      │
│  ┌───────────────▼────────────────────┐                │
│  │  Virtual Drivers                   │                │
│  │  • GPIO: Pin config, read/write    │                │
│  │  • NVIC: Interrupt priority        │                │
│  │  • ADC: Channel simulation         │                │
│  └───────────────┬────────────────────┘                │
│                  │                                      │
│  ┌───────────────▼────────────────────┐                │
│  │  Software Simulation               │                │
│  │  • No hardware required            │                │
│  │  • Fast iteration cycles           │                │
│  │  • Error injection testing         │                │
│  └────────────────────────────────────┘                │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

**Key Features:**

| Feature | Description |
|---------|-------------|
| **GPIO Simulation** | Full pin configuration, interrupts, pin multiplexing (AF0-AF15) |
| **NVIC Simulation** | 240 IRQ lines, priority handling, interrupt processing |
| **HAL Compatible** | Drop-in replacement for STM32 HAL functions |
| **Error Injection** | Configurable fault injection for robustness testing |
| **Cross-Platform** | Runs on Linux, macOS, Windows without hardware |
| **CI/CD Ready** | Easy integration into automated testing pipelines |

**Quick Start:**

```bash
cd 07_Virtual_Simulation
make test          # Build and run all tests
make test-gpio     # Test GPIO driver
make test-nvic     # Test interrupt controller
make test-hal      # Test HAL wrapper
```

**Example Usage:**

```c
// Forward declarations (in production, use header files)
extern HAL_StatusTypeDef HAL_Init(void);
extern HAL_StatusTypeDef HAL_GPIO_Init(uint8_t port, GPIO_InitTypeDef *GPIO_Init);
extern void HAL_GPIO_WritePin(uint8_t port, uint16_t pin, GPIO_PinState state);
extern void HAL_GPIO_TogglePin(uint8_t port, uint16_t pin);
extern void HAL_Delay(uint32_t ms);

// Note: For standalone tests, link with:
// gcc -o test test.c sim_hal_wrapper.c sim_gpio.c sim_nvic.c

int main(void) {
    // Initialize HAL
    HAL_Init();
    
    // Configure GPIO pin
    GPIO_InitTypeDef gpio;
    gpio.Pin = 5;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOA_PORT, &gpio);
    
    // Use standard HAL functions
    HAL_GPIO_WritePin(GPIOA_PORT, 5, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(GPIOA_PORT, 5);
    
    return 0;
}
```

**Benefits:**
- ✅ Test without hardware
- ✅ Fast compile-test cycles
- ✅ Error injection capabilities
- ✅ Learn STM32 concepts risk-free
- ✅ Debug with standard tools (gdb, valgrind)

See `07_Virtual_Simulation/README.md` and `Documentation/SIMULATION_GUIDE.md` for complete documentation.

---

## 🎓 Learning Path

### Beginner (Week 1-2)

```
Day 1-2:   ├─ Understand STM32 Architecture
           ├─ Learn register concepts
           └─ Set up development environment

Day 3-4:   ├─ Direct register read/write
           ├─ GPIO basic operations
           └─ First LED blink project

Day 5-7:   ├─ HAL initialization
           ├─ Clock configuration
           └─ Multiple GPIO control

Day 8-14:  ├─ UART communication
           ├─ Serial debugging
           └─ Real-time data transmission
```

### Intermediate (Week 3-4)

```
├─ Interrupts & NVIC configuration
├─ Timer operations
├─ ADC fundamentals
├─ SPI/I2C communication protocols
└─ Sensor integration projects
```

### Advanced (Week 5-6)

```
├─ Bare-metal driver design
├─ Advanced interrupt handling
├─ DMA (Direct Memory Access)
├─ RTOS concepts
└─ Optimization & debugging techniques
```

### Expert (Week 7+)

```
├─ Custom bootloader development
├─ Firmware security
├─ Power management
├─ CAN bus protocols
└─ Real-time processing systems
```

---

## 🐛 Debugging Guide

### Common Issues & Solutions

#### ❌ Issue 1: Code Hangs After Flash

```c
// PROBLEM: Missing clock initialization
// SOLUTION: Add system clock setup before peripherals

void main(void) {
    // ✓ CORRECT: Initialize HAL first
    HAL_Init();
    
    // ✓ CORRECT: Configure clocks
    SystemClock_Config();
    
    // ✓ NOW you can initialize peripherals
    GPIO_Init();
    
    while(1) {
        // Application code
    }
}
```

#### ❌ Issue 2: Interrupt Never Fires

```c
// CHECKLIST:
// ✓ Is interrupt source actually happening?
//   └─ Use oscilloscope or logic analyzer
//
// ✓ Is NVIC interrupt enabled?
//   └─ HAL_NVIC_EnableIRQ(IRQn);
//
// ✓ Is priority set correctly?
//   └─ HAL_NVIC_SetPriority(IRQn, priority, sub_priority);
//
// ✓ Is ISR handler name EXACTLY correct?
//   └─ void EXTI0_IRQHandler(void) { ... }
//
// ✓ Did you clear interrupt flags in ISR?
//   └─ __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
```

#### ❌ Issue 3: UART Not Receiving Data

```c
// DEBUGGING STEPS:

// 1. Verify physical connection
// ├─ TX connected to RX (crossed)
// └─ GND connected

// 2. Check baud rate matches
// ├─ Terminal speed = HAL config
// └─ Common: 9600, 115200

// 3. Verify GPIO pin configuration
UART_Init_t config = {
    .baud_rate = 115200,
    .word_length = UART_WORDLENGTH_8B,
    .stop_bits = UART_STOPBITS_1,
    .parity = UART_PARITY_NONE
};

// 4. Check UART interrupt is enabled
HAL_NVIC_EnableIRQ(USART1_IRQn);

// 5. Monitor with oscilloscope
// └─ Check signal levels and timing
```

#### ❌ Issue 4: Memory Access Violation

```c
// PROBLEM: Accessing invalid memory
// SOLUTION: Add bounds checking

void write_register(uint32_t address, uint32_t value) {
    // ✓ Validate address range
    if (address < PERIPH_BASE || address > PERIPH_END) {
        DEBUG_PRINTF("Invalid address: 0x%08x\n", address);
        return;
    }
    
    // ✓ Ensure address is aligned
    if (address & 0x03) {
        DEBUG_PRINTF("Unaligned address\n");
        return;
    }
    
    // ✓ Now write safely
    volatile uint32_t *reg = (volatile uint32_t *)address;
    *reg = value;
}
```

---

## ⚡ Performance Tips

### 1. Optimize Interrupt Response Time

```c
// ✗ SLOW: Doing too much in ISR
void GPIO_IRQHandler(void) {
    for(int i = 0; i < 1000000; i++) {
        some_calculation();  // Too slow!
    }
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

// ✓ FAST: Minimal work in ISR
volatile uint8_t interrupt_flag = 0;

void GPIO_IRQHandler(void) {
    interrupt_flag = 1;  // Set flag
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
}

// Do heavy work in main loop
while(1) {
    if (interrupt_flag) {
        interrupt_flag = 0;
        some_calculation();  // Now it's safe
    }
}
```

### 2. Use DMA for Data Transfers

```c
// ✗ SLOW: CPU-driven data transfer
for(int i = 0; i < 1000; i++) {
    SPI->DR = data[i];  // CPU waits for each byte
    while(!(SPI->SR & SPI_SR_TXE));
}

// ✓ FAST: DMA-driven transfer
DMA_Config_t dma = {
    .source = (uint32_t)data,
    .destination = (uint32_t)&SPI->DR,
    .size = 1000,
    .mode = DMA_MODE_CIRCULAR
};
HAL_DMA_Start(&dma);
// CPU is free to do other work!
```

### 3. Optimize Clock Speed

```c
// STM32F4: Maximum CPU frequency is 180 MHz
// Each extra MHz = faster execution but more power

void SystemClock_Config(void) {
    // PLL Configuration for 180 MHz
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
}
```

### 4. Power Management

```c
// Sleep when idle - reduces power consumption
while(1) {
    // Do important work
    process_data();
    
    // Enter sleep mode when waiting
    __WFE();  // Wait for Event
    // or
    __WFI();  // Wait for Interrupt
    // CPU power consumption drops 100x!
}
```

---

## 📋 Code Examples

### Example 1: GPIO Toggle with Timing

```c
#include "stm32f4xx_hal.h"

GPIO_InitTypeDef GPIO_InitStruct;

void GPIO_Setup(void) {
    // Enable GPIOA clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure PA5 as output
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void main(void) {
    HAL_Init();
    GPIO_Setup();
    
    while(1) {
        // Toggle LED every 500ms
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);
    }
}
```

### Example 2: UART Receive with Interrupt

```c
UART_HandleTypeDef huart1;
volatile uint8_t rx_buffer[100];
volatile uint16_t rx_size = 0;

void UART_Setup(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    
    HAL_UART_Init(&huart1);
    
    // Enable RX interrupt
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_buffer, sizeof(rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        rx_size = huart->RxXferSize;
        // Process received data
        process_command((char *)rx_buffer);
    }
    // Re-enable interrupt for next reception
    HAL_UART_Receive_IT(huart, (uint8_t *)rx_buffer, sizeof(rx_buffer));
}
```

### Example 3: ADC Temperature Sensor

```c
ADC_HandleTypeDef hadc1;
volatile uint32_t adc_value = 0;

void ADC_Setup(void) {
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    
    HAL_ADC_Init(&hadc1);
    
    // Configure temperature sensor channel
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
}

uint32_t ReadTemperature(void) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    adc_value = HAL_ADC_GetValue(&hadc1);
    
    // Convert to Celsius
    // Temp = (V25 - Vsense) / AvgSlope + 25
    float temp = (0.76f - (adc_value * 3.3f / 4096.0f)) / 0.0025f + 25.0f;
    
    return (uint32_t)temp;
}
```

---

## 🤝 Contributing

We welcome contributions! Here's how to help:

```
1. Fork the repository
2. Create feature branch (git checkout -b feature/AmazingFeature)
3. Commit changes (git commit -m 'Add AmazingFeature')
4. Push to branch (git push origin feature/AmazingFeature)
5. Open Pull Request

Code Style Guidelines:
├─ Follow MISRA C standards where possible
├─ Use meaningful variable names
├─ Comment complex register operations
├─ Test on actual hardware
└─ Update documentation
```

---

## 📚 Resources

### Official Documentation

| Resource | Link | Purpose |
|----------|------|---------|
| STM32F4 Reference Manual | st.com/resource/stm32f4 | Register definitions |
| Datasheet | st.com/content/stm32f4xx-ds | Pin configurations, specs |
| HAL User Manual | st.com/resource/hal-user-manual | HAL API reference |

### External Learning Resources

```
📖 Books:
  • "Embedded Systems with ARM Cortex-M Microcontrollers"
  • "The Definitive Guide to ARM Cortex-M0 and Cortex-M0+"
  • "Mastering Microcontroller with Embedded Driver Development"

🎥 Video Courses:
  • STM32 Embedded Systems (Udemy)
  • Bare-Metal ARM Programming (Various YouTube channels)
  • Interrupt Handling Deep Dive

🌐 Websites:
  • https://www.st.com
  • https://github.com/STMicroelectronics/STM32CubeF4
  • Community forums and Stack Overflow
```

---

## 📊 Quick Reference

### GPIO Operations
```c
// Initialize
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// Output
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);      // Set high
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);    // Set low
HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);                   // Toggle

// Input
GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
```

### Timing Operations
```c
HAL_Delay(1000);                           // Delay 1 second
uint32_t ticks = HAL_GetTick();           // Get system ticks
__HAL_TIM_SET_COUNTER(&htim2, 0);        // Reset timer
```

### Interrupt Operations
```c
HAL_NVIC_SetPriority(GPIO_IRQn, 5, 0);   // Set priority
HAL_NVIC_EnableIRQ(GPIO_IRQn);           // Enable interrupt
HAL_NVIC_DisableIRQ(GPIO_IRQn);          // Disable interrupt
```

---

## 📝 License

This project is licensed under the **MIT License** - see the LICENSE file for details.

---

## 🎉 Acknowledgments

```
This repository draws from:
  ✓ STMicroelectronics official documentation
  ✓ Community contributions and feedback
  ✓ Real-world embedded systems experience
  ✓ Best practices from industry professionals
```

---

## 📧 Contact & Support

```
📧 Email: your.email@example.com
🐙 GitHub: https://github.com/yourusername
💬 Discussions: GitHub Discussions enabled
🆘 Issues: Report bugs via GitHub Issues
```

---

## 🚀 Quick Start Commands

```bash
# Clone and setup
git clone https://github.com/yourusername/STM32_HAL_Development.git
cd STM32_HAL_Development

# Build first project
cd 06_Real_World_Projects/01_GPIO_LED_Blink
make clean && make

# Flash to STM32 board
make flash

# Monitor serial output
make monitor

# Clean build artifacts
make distclean
```

---

<div align="center">

### ⭐ If this repository helped you, please star it! ⭐

**Made with ❤️ for the embedded systems community**

```
   _____  ____  __  ___________
  / ___/ / __ \/  |/  /_  __/ /
  \__ \ / /_/ / /  /  / / / /  
 ___/ // _, _/ /  /  / / / /   
/____//_/ |_/_/__/  /_/ /_/    
                               
STM32 HAL Development Guide
```

Last Updated: January 2026  
Version: 2.0  
Maintained by: Daksh Patel

</div>

---

**🎯 Happy Embedded Systems Development! 🎯**

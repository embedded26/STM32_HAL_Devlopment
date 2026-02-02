# 01_Register_Access

## Overview
This directory contains examples demonstrating direct register access in STM32 microcontrollers without using HAL abstractions. These examples teach fundamental concepts of embedded systems programming and hardware interaction.

## Learning Objectives
- Understand memory-mapped I/O architecture
- Learn direct register manipulation techniques
- Master bit and bitfield operations
- Comprehend the importance of volatile keyword
- Gain insight into STM32 register structure

## Files in This Directory

### 1. direct_register_read.c
**Purpose:** Demonstrates reading GPIO registers directly

**Key Concepts:**
- Reading GPIO Input Data Register (IDR)
- Clock enable via RCC register
- Pin configuration via MODER register
- Extracting individual pin states
- Reading multiple GPIO ports

**Usage:**
```c
// Compile and run to see register reading examples
gcc direct_register_read.c -I../drivers/inc -o read_example
./read_example
```

### 2. direct_register_write.c
**Purpose:** Shows how to write to GPIO registers

**Key Concepts:**
- Writing to Output Data Register (ODR)
- Atomic operations using BSRR (Bit Set/Reset Register)
- Configuring output mode and speed
- Difference between ODR and BSRR methods
- Controlling multiple pins simultaneously

**Usage:**
```c
// Demonstrates LED blinking at register level
gcc direct_register_write.c -I../drivers/inc -o write_example
./write_example
```

### 3. register_bitfield.c
**Purpose:** Comprehensive bitfield manipulation examples

**Key Concepts:**
- Single-bit operations (SET, CLEAR, TOGGLE, READ)
- Multi-bit field manipulation
- Pin mode configuration (2 bits per pin)
- Speed and pull-up/down configuration
- Alternate function selection
- Binary visualization of register contents

**Usage:**
```c
// Shows all bitfield operation techniques
gcc register_bitfield.c -I../drivers/inc -o bitfield_example
./bitfield_example
```

### 4. memory_mapped_io.c
**Purpose:** Explains memory-mapped I/O concepts

**Key Concepts:**
- STM32 memory map architecture
- Pointer-based peripheral access
- Structure-based register access
- Importance of volatile keyword
- Peripheral address calculation
- Bus architecture (APB1, APB2, AHB1, AHB2)

**Usage:**
```c
// Educational demonstration of memory mapping
gcc memory_mapped_io.c -I../drivers/inc -o mmio_example
./mmio_example
```

## Register Access Methods

### Method 1: Direct Pointer Access
```c
volatile uint32_t *gpio_odr = (volatile uint32_t*)0x40020014;
*gpio_odr |= (1 << 5);  // Set PA5
```

### Method 2: Structure-Based Access (Recommended)
```c
GPIOA->ODR |= (1 << 5);  // Set PA5
```

### Method 3: Using BSRR for Atomic Operations (Best)
```c
GPIOA->BSRRL = (1 << 5);  // Set PA5 atomically
GPIOA->BSRRH = (1 << 5);  // Reset PA5 atomically
```

## Important Register Concepts

### GPIO Register Map
| Offset | Register | Description |
|--------|----------|-------------|
| 0x00 | MODER | Mode register (2 bits/pin) |
| 0x04 | OTYPER | Output type register |
| 0x08 | OSPEEDR | Output speed register |
| 0x0C | PUPDR | Pull-up/pull-down register |
| 0x10 | IDR | Input data register (read-only) |
| 0x14 | ODR | Output data register |
| 0x18 | BSRR | Bit set/reset register |
| 0x20-24 | AFR | Alternate function registers |

### Bit Meanings

**MODER (2 bits per pin):**
- 00: Input mode
- 01: Output mode
- 10: Alternate function
- 11: Analog mode

**OSPEEDR (2 bits per pin):**
- 00: Low speed
- 01: Medium speed
- 10: Fast speed
- 11: High speed

**PUPDR (2 bits per pin):**
- 00: No pull-up/down
- 01: Pull-up
- 10: Pull-down
- 11: Reserved

## Why Direct Register Access?

1. **Understanding:** Deepens comprehension of hardware operation
2. **Performance:** Can be more efficient than HAL in time-critical code
3. **Debugging:** Essential skill for troubleshooting HAL issues
4. **Flexibility:** Allows access to features not exposed by HAL
5. **Portability:** Easier to port code to different HAL implementations

## Best Practices

1. **Always use volatile:** Prevents compiler optimization issues
2. **Prefer BSRR over ODR:** Atomic operations avoid race conditions
3. **Read-modify-write carefully:** Can cause issues in interrupt contexts
4. **Enable clocks first:** Always enable peripheral clock before access
5. **Use macros for bit manipulation:** Improves code readability

## Common Pitfalls

❌ **Don't:**
```c
GPIOA->ODR |= (1 << 5);  // In interrupt context (not atomic)
uint32_t *ptr = 0x40020000;  // Missing volatile
```

✅ **Do:**
```c
GPIOA->BSRRL = (1 << 5);  // Atomic operation
volatile uint32_t *ptr = (volatile uint32_t*)0x40020000;
```

## Hardware Requirements
- STM32F446RE Nucleo board (or compatible)
- USB cable for programming/debugging
- Optional: LED connected to PA5 for testing

## Compilation Notes
These examples are designed to be educational and may require actual hardware for full functionality. When compiling for host system (x86/ARM):
- Register reads/writes will simulate the operations
- printf output shows what would happen on real hardware
- For actual hardware testing, use an embedded toolchain (arm-none-eabi-gcc)

## Next Steps
After mastering direct register access, proceed to:
- **02_HAL_Setup**: Learn HAL abstraction layer
- **03_Interrupt_Handling**: Understand interrupt-safe operations
- **04_Bare_Metal_Drivers**: Build complete drivers using these concepts

## References
- STM32F446RE Reference Manual (RM0390)
- STM32F446RE Datasheet
- [Main Project README](../README.md)
- Cortex-M4 Generic User Guide

## Additional Resources
- Understanding the volatile keyword
- ARM Cortex-M memory map
- Read-modify-write hazards in embedded systems
- Atomic operations importance

---
*Part of the STM32 HAL Development Repository*

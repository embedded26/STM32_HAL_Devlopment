# 05_Debugging_Advanced

## Overview
This directory contains advanced debugging examples and techniques for STM32 development. Master professional debugging methodologies, memory inspection, and performance profiling to create robust embedded applications.

## Learning Objectives
- Use debug utilities effectively
- Inspect and analyze memory
- Profile code performance
- Detect memory corruption
- Optimize critical code paths
- Debug hardware issues systematically

## Files in This Directory

### 1. debug_example.c
**Purpose:** Comprehensive debug utilities usage

**Features:**
- Multi-level logging (ERROR, WARN, INFO, DEBUG, TRACE)
- Register dumping
- Memory inspection
- Bit manipulation helpers
- Error tracking
- Assertion macros

**Debug Levels:**
```c
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4
#define DEBUG_LEVEL_TRACE   5
```

**Usage:**
```c
DEBUG_ERROR("Critical error: %d", error_code);
DEBUG_WARN("Warning: Temperature high");
DEBUG_INFO("System initialized");
DEBUG_LOG("Variable x = %d", x);
DEBUG_TRACE("Entering function");
```

### 2. memory_dump_example.c
**Purpose:** Memory inspection and analysis techniques

**Capabilities:**
- Dump memory regions (hex + ASCII)
- Inspect structure layouts
- Check peripheral registers
- Detect memory corruption
- Analyze stack usage
- Verify pointer validity
- Compare memory regions

**Memory Dump Format:**
```
=== Memory Dump: Buffer ===
Address: 0x20000100, Length: 64 bytes
Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
-------  -----------------------------------------------
00000000 48 65 6C 6C 6F 20 53 54 4D 33 32 21 00 00 00 00  |Hello STM32!....|
```

### 3. profiling_example.c
**Purpose:** Performance profiling and optimization

**Profiling Capabilities:**
- Cycle-accurate timing with DWT
- Function call overhead measurement
- Register access timing
- Arithmetic operation comparison
- Array operation benchmarks
- Implementation comparison

**DWT Cycle Counter:**
```c
uint32_t start = DWT_CYCCNT;
// Code to profile
uint32_t cycles = DWT_CYCCNT - start;
```

## Debug Utilities API

### Logging Macros
```c
DEBUG_ERROR(fmt, ...)    // Errors only
DEBUG_WARN(fmt, ...)     // Warnings and above
DEBUG_INFO(fmt, ...)     // Info and above
DEBUG_LOG(fmt, ...)      // Debug messages
DEBUG_TRACE(fmt, ...)    // All messages + function name
```

### Assertion
```c
DEBUG_ASSERT(condition);  // Halts if false
```

### Register Dump
```c
Debug_DumpRegister(&(GPIOA->MODER), "MODER");
Debug_DumpRegisters(base_addr, names, count);
```

### Memory Dump
```c
Debug_DumpMemory(addr, len, label);
```

### Bit Manipulation
```c
BIT_SET(reg, bit);
BIT_CLEAR(reg, bit);
BIT_TOGGLE(reg, bit);
BIT_READ(reg, bit);
```

## DWT (Data Watchpoint and Trace)

### Overview
- Cortex-M debug unit
- Cycle-accurate counter
- No interrupts or overhead
- Always counts (when enabled)

### Registers
```c
#define DWT_CTRL    0xE0001000  // Control
#define DWT_CYCCNT  0xE0001004  // Cycle count
#define DEM_CR      0xE000EDFC  // Debug enable
```

### Initialization
```c
void DWT_Init(void) {
    DEM_CR |= (1 << 24);       // Enable trace
    DWT_CYCCNT = 0;            // Reset counter
    DWT_CTRL |= (1 << 0);      // Enable counter
}
```

### Usage Patterns
```c
// Simple timing
uint32_t start = DWT_CYCCNT;
my_function();
uint32_t cycles = DWT_CYCCNT - start;

// Precise delays
void delay_us(uint32_t us) {
    uint32_t start = DWT_CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT_CYCCNT - start) < ticks);
}
```

## Memory Debugging Techniques

### Stack Canaries
```c
uint32_t canary_before = 0xDEADBEEF;
uint8_t buffer[128];
uint32_t canary_after = 0xCAFEBABE;

// After operations, check:
if (canary_before != 0xDEADBEEF ||
    canary_after != 0xCAFEBABE) {
    // Buffer overflow detected!
}
```

### Pointer Validation
```c
bool is_valid_sram_ptr(void *ptr) {
    uint32_t addr = (uint32_t)ptr;
    return (addr >= 0x20000000 && addr < 0x20020000);
}

bool is_valid_flash_ptr(void *ptr) {
    uint32_t addr = (uint32_t)ptr;
    return (addr >= 0x08000000 && addr < 0x08080000);
}
```

### Structure Padding
```c
typedef struct {
    uint8_t  byte1;    // 1 byte
    // 3 bytes padding
    uint32_t word;     // 4 bytes (aligned)
    uint8_t  byte2;    // 1 byte
    // 3 bytes padding
} MyStruct_t;  // Total: 12 bytes (not 6!)

// Use sizeof() and offsetof() to inspect
```

## Performance Profiling

### Typical Operation Costs (at 84 MHz)
| Operation | Cycles | Time (ns) |
|-----------|--------|-----------|
| NOP | 1 | 12 |
| Register read | 1-2 | 12-24 |
| Register write | 1-2 | 12-24 |
| Addition | 1 | 12 |
| Multiplication | 1 | 12 |
| Division | 12+ | 140+ |
| Function call | 4-8 | 48-96 |

### Optimization Checklist
- [ ] Use appropriate optimization level (-O2 typical)
- [ ] Inline small functions
- [ ] Use const for read-only data
- [ ] Prefer 32-bit operations on ARM
- [ ] Minimize volatile access
- [ ] Reduce function call overhead
- [ ] Use DMA for bulk transfers
- [ ] Consider lookup tables vs calculation

## Common Debug Scenarios

### Scenario 1: GPIO Not Working
```c
// Check peripheral clock
if (!(RCC->AHB1ENR & (1 << 0))) {
    DEBUG_ERROR("GPIOA clock not enabled");
}

// Check pin mode
uint32_t mode = (GPIOA->MODER >> (pin * 2)) & 0x3;
DEBUG_LOG("PA%d mode: %d", pin, mode);

// Dump all GPIO registers
Debug_DumpRegisters((uint32_t*)GPIOA, gpio_names, 8);
```

### Scenario 2: Interrupt Not Firing
```c
// Check NVIC enabled
if (!(NVIC_ISER0 & (1 << irq_num))) {
    DEBUG_ERROR("IRQ %d not enabled in NVIC", irq_num);
}

// Check pending flag
if (NVIC_ISPR0 & (1 << irq_num)) {
    DEBUG_WARN("IRQ %d is pending", irq_num);
}

// Check EXTI configuration (for GPIO interrupts)
DEBUG_LOG("EXTI->IMR = 0x%08X", EXTI->IMR);
DEBUG_LOG("EXTI->PR = 0x%08X", EXTI->PR);
```

### Scenario 3: Timer Not Counting
```c
// Check timer enabled
if (!(TIM2->CR1 & 1)) {
    DEBUG_ERROR("Timer not enabled");
}

// Check clock source
if (!(RCC->APB1ENR & (1 << 0))) {
    DEBUG_ERROR("TIM2 clock not enabled");
}

// Dump timer registers
DEBUG_LOG("CNT: %lu", TIM2->CNT);
DEBUG_LOG("PSC: %u", TIM2->PSC);
DEBUG_LOG("ARR: %lu", TIM2->ARR);
```

## Debugging Workflow

### 1. Reproduce the Issue
- Create minimal test case
- Document exact steps
- Note environmental conditions

### 2. Gather Information
- Enable debug logging
- Dump relevant registers
- Check memory contents
- Review error counters

### 3. Form Hypothesis
- Based on symptoms
- Consider recent changes
- Review documentation

### 4. Test Hypothesis
- Add targeted debug output
- Try specific fixes
- Use assertions to verify assumptions

### 5. Validate Fix
- Test original scenario
- Test edge cases
- Check for side effects
- Profile performance impact

## Production vs Debug Builds

### Debug Build
```c
#define DEBUG_LEVEL DEBUG_LEVEL_DEBUG
#define DEBUG_ENABLE_ASSERT
#define ENABLE_PROFILING
```

### Production Build
```c
#define DEBUG_LEVEL DEBUG_LEVEL_ERROR
// #define DEBUG_ENABLE_ASSERT  // Disabled
// #define ENABLE_PROFILING     // Disabled
```

### Conditional Compilation
```c
#ifdef DEBUG_BUILD
    Debug_DumpMemory(buffer, len, "Buffer");
    DEBUG_LOG("State: %d", state);
#endif
```

## Compilation

```bash
# Debug build
gcc debug_example.c -I../drivers/inc -DDEBUG_LEVEL=5 \
    -DDEBUG_ENABLE_ASSERT -o debug_ex
./debug_ex

# Memory dump example
gcc memory_dump_example.c -I../drivers/inc -o mem_dump
./mem_dump

# Profiling example
gcc profiling_example.c -I../drivers/inc -o profile
./profile
```

## Tools Integration

### GDB Debugging
```bash
# Start GDB
arm-none-eabi-gdb firmware.elf

# Common commands
(gdb) break main
(gdb) run
(gdb) print variable
(gdb) x/16x 0x20000000  # Examine memory
(gdb) info registers
```

### OpenOCD
```bash
# Connect to target
openocd -f board/st_nucleo_f4.cfg

# In another terminal
telnet localhost 4444
> reset halt
> mdw 0x40020000 16  # Memory display word
```

## Advanced Techniques

### Watchpoints
- Monitor variable changes
- Break on memory access
- Useful for corruption detection

### Trace
- Use SWO for printf debugging
- ITM for instrumentation
- Real-time variable monitoring

### Logic Analyzer
- Capture GPIO signals
- Verify timing
- Analyze protocols (SPI, I2C)

## Next Steps
- Apply debugging to real projects (06_Real_World_Projects)
- Learn RTOS debugging techniques
- Master advanced GDB features
- Use static analysis tools

## References
- ARM CoreSight Architecture
- STM32 Debug Support
- GDB Manual
- [Main Project README](../README.md)

---
*Part of the STM32 HAL Development Repository*

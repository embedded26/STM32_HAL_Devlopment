# Virtual Simulation Framework

This directory contains virtual driver implementations for STM32 peripherals, enabling software-based testing without hardware or QEMU.

## Overview

The virtual simulation framework provides:

- **Virtual GPIO Driver** (`sim_gpio.c`): Complete GPIO peripheral simulation with pin configuration, reading/writing, interrupts, and pin multiplexing
- **Virtual NVIC** (`sim_nvic.c`): Interrupt controller simulation with 240 IRQ lines, priority handling, and interrupt processing
- **HAL Wrapper** (`sim_hal_wrapper.c`): HAL-compatible API that works with virtual drivers
- **ADC Simulation** (`sim_adc.c`): Basic ADC peripheral with 16 channels

## Quick Start

### Build All Tests

```bash
make all
```

This compiles four test executables:
- `build/test_adc`: ADC simulation test
- `build/test_gpio`: GPIO driver test
- `build/test_nvic`: NVIC controller test
- `build/test_hal_wrapper`: HAL wrapper integration test

### Run All Tests

```bash
make test
```

This builds and runs all test executables sequentially.

### Run Individual Tests

```bash
make test-gpio    # GPIO tests only
make test-nvic    # NVIC tests only
make test-hal     # HAL wrapper tests
make test-adc     # ADC tests
```

## Features

### Virtual GPIO Driver

✅ **Full GPIO Configuration**
- Input/Output/Alternate/Analog modes
- Push-pull and open-drain output types
- Speed settings (Low/Medium/Fast/High)
- Pull-up/Pull-down configuration

✅ **Pin Multiplexing**
- Alternate function configuration (AF0-AF15)
- Runtime pin-mux switching

✅ **Interrupt Support**
- Rising edge, falling edge, or both
- Callback-based interrupt handling
- Interrupt simulation for testing

✅ **Error Injection**
- Configurable error injection for robustness testing
- Multiple error types (invalid port/pin, configuration errors)
- Error code reporting

### Virtual NVIC

✅ **Interrupt Management**
- 240 IRQ lines (typical STM32 configuration)
- 16 priority levels (0-15)
- Enable/disable individual IRQ lines
- Global interrupt enable/disable

✅ **Priority-Based Execution**
- Automatic priority sorting
- Higher priority interrupts preempt lower priority
- Pending and active state tracking

✅ **Interrupt Handlers**
- Callback registration for each IRQ
- Named interrupts for easy debugging

### HAL Wrapper

✅ **HAL-Compatible API**
- Drop-in replacement for STM32 HAL GPIO functions
- Compatible with existing HAL-based code
- Easy migration between virtual and real hardware

✅ **Supported HAL Functions**
- `HAL_Init()`: Initialize HAL and virtual drivers
- `HAL_GPIO_Init()`: Configure GPIO pins
- `HAL_GPIO_ReadPin()`: Read pin state
- `HAL_GPIO_WritePin()`: Write pin state
- `HAL_GPIO_TogglePin()`: Toggle pin
- `HAL_NVIC_EnableIRQ()`: Enable interrupt
- `HAL_NVIC_DisableIRQ()`: Disable interrupt
- `HAL_NVIC_SetPriority()`: Set interrupt priority

## Usage Examples

### GPIO Basic Example

```c
// Forward declarations (or use header files in production)
extern void VirtualGPIO_Init(void);
extern uint8_t VirtualGPIO_EnableClock(uint8_t port);
extern uint8_t VirtualGPIO_ConfigurePin(uint8_t port, uint8_t pin, uint8_t mode,
                                        uint8_t output_type, uint8_t speed, uint8_t pupd);
extern uint8_t VirtualGPIO_WritePin(uint8_t port, uint8_t pin, uint8_t value);
extern uint8_t VirtualGPIO_TogglePin(uint8_t port, uint8_t pin);

int main(void) {
    // Initialize
    VirtualGPIO_Init();
    
    // Enable clock for GPIOA
    VirtualGPIO_EnableClock(0);
    
    // Configure PA5 as output
    VirtualGPIO_ConfigurePin(0, 5, 1, 0, 3, 0);
    
    // Write and toggle
    VirtualGPIO_WritePin(0, 5, 1);
    VirtualGPIO_TogglePin(0, 5);
    
    return 0;
}
```

### Interrupt Example

```c
// Forward declarations
extern void VirtualGPIO_Init(void);
extern uint8_t VirtualGPIO_EnableClock(uint8_t port);
extern uint8_t VirtualGPIO_ConfigureInterrupt(uint8_t port, uint8_t pin, uint8_t mode,
                                               void (*handler)(uint8_t, uint8_t));
extern void VirtualGPIO_SimulateInterrupt(uint8_t port, uint8_t pin, uint8_t edge);

void button_handler(uint8_t port, uint8_t pin) {
    printf("Button pressed!\n");
}

int main(void) {
    VirtualGPIO_Init();
    VirtualGPIO_EnableClock(2);  // GPIOC
    
    // Configure PC13 for falling edge interrupt
    VirtualGPIO_ConfigureInterrupt(2, 13, 5, button_handler);
    
    // Simulate button press
    VirtualGPIO_SimulateInterrupt(2, 13, 0);  // Falling edge
    
    return 0;
}
```

### HAL Example

```c
// Forward declarations (or use header files in production)
// When linking: gcc -o test test.c sim_hal_wrapper.c sim_gpio.c sim_nvic.c
extern HAL_StatusTypeDef HAL_Init(void);
extern HAL_StatusTypeDef HAL_GPIO_Init(uint8_t port, GPIO_InitTypeDef *GPIO_Init);
extern void HAL_GPIO_TogglePin(uint8_t port, uint16_t pin);
extern void HAL_Delay(uint32_t ms);

int main(void) {
    HAL_Init();
    
    GPIO_InitTypeDef gpio;
    gpio.Pin = 5;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOA_PORT, &gpio);
    
    // Blink LED
    for (int i = 0; i < 10; i++) {
        HAL_GPIO_TogglePin(GPIOA_PORT, 5);
        HAL_Delay(500);
    }
    
    return 0;
}
```

## Error Injection Testing

Enable error injection to test error handling:

```c
// Enable 10% error injection rate
VirtualGPIO_SetErrorInjection(1);

// Operations may now fail randomly
if (!VirtualGPIO_WritePin(0, 5, 1)) {
    uint8_t error = VirtualGPIO_GetLastError();
    printf("Error: %d\n", error);
}

// Disable error injection
VirtualGPIO_SetErrorInjection(0);
```

## Building Your Own Tests

### Link with Virtual Drivers

```bash
gcc -o my_test my_test.c sim_gpio.c sim_nvic.c -Wall -std=c99
```

### Include in Your Code

```c
// Option 1: Forward declare functions you need
extern void VirtualGPIO_Init(void);
extern uint8_t VirtualGPIO_EnableClock(uint8_t port);
// ... etc

// Option 2: Link at compile time
// gcc -o my_test my_test.c sim_gpio.c sim_nvic.c -Wall -std=c99

// Option 3: Use conditional compilation with proper linking
#ifdef USE_VIRTUAL_DRIVERS
  // Forward declarations or header includes here
  extern void VirtualGPIO_Init(void);
  // ...
#endif
```

## Makefile Targets

| Target | Description |
|--------|-------------|
| `all` | Build all test executables |
| `test` | Build and run all tests |
| `test-adc` | Run ADC test only |
| `test-gpio` | Run GPIO test only |
| `test-nvic` | Run NVIC test only |
| `test-hal` | Run HAL wrapper test only |
| `clean` | Remove build artifacts |
| `help` | Show help message |

## Architecture

```
Application Code
       ↓
HAL Wrapper (optional)
       ↓
Virtual Drivers (GPIO, NVIC, ADC)
       ↓
Software Simulation
```

## Benefits

1. **No Hardware Required**: Test code without physical STM32 boards
2. **Fast Iteration**: Compile and run tests in seconds
3. **Error Injection**: Test error handling and edge cases
4. **CI/CD Friendly**: Easy integration into automated pipelines
5. **Cross-Platform**: Runs on Linux, macOS, Windows
6. **Debugging**: Use standard debugging tools (gdb, valgrind)
7. **Educational**: Learn STM32 concepts without hardware investment

## Limitations

- Timing is simulated (not real-time)
- No actual hardware interaction
- Limited peripheral support (GPIO, NVIC, ADC currently)
- Simplified interrupt model

For full system emulation, consider using QEMU (see `../Documentation/SIMULATION_GUIDE.md`).

## Extending the Framework

To add new peripherals:

1. Create `sim_<peripheral>.c` with virtual implementation
2. Add main() wrapped in `#ifdef RUN_STANDALONE_TEST`
3. Add build rules to Makefile
4. Update this README

## Documentation

See `../Documentation/SIMULATION_GUIDE.md` for comprehensive documentation including:
- Detailed API reference
- Advanced usage examples
- QEMU integration
- Testing strategies

## License

Part of the STM32_HAL_Development educational repository.

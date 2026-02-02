# 🚀 Quick Start Guide - STM32 HAL Development

Get up and running with STM32 bare-metal development in minutes!

---

## 📋 Prerequisites

### Hardware
- **Recommended**: STM32 Nucleo board (F0/F1/F4 series)
  - Nucleo-F030R8 (STM32F0)
  - Nucleo-F103RB (STM32F1)
  - Nucleo-F446RE (STM32F4) ← Default target
- **Alternative**: Any STM32 development board
- **For learning**: No hardware required! Use virtual simulation

### Software
- **ARM GCC Toolchain** (for cross-compilation)
  ```bash
  # Ubuntu/Debian
  sudo apt-get install gcc-arm-none-eabi
  
  # macOS (Homebrew)
  brew install --cask gcc-arm-embedded
  
  # Windows
  # Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
  ```

- **Build Tools**
  ```bash
  sudo apt-get install make git
  ```

- **Optional Tools**
  - OpenOCD (for debugging)
  - ST-Link utilities
  - QEMU for ARM (for simulation)

---

## 🎯 30-Second Start (No Hardware)

### Test Virtual Simulation

```bash
# Clone repository
git clone https://github.com/embedded26/STM32_HAL_Devlopment.git
cd STM32_HAL_Devlopment

# Build and test virtual simulation
cd 07_Virtual_Simulation
make clean && make

# Run tests
./build/test_gpio
./build/test_adc
./build/test_nvic
./build/test_hal_wrapper

# Success! ✅ You just ran STM32 code without hardware
```

---

## 🏗️ 5-Minute Start (With Hardware)

### Step 1: Select Your Board

Edit your project files to define your board:

```c
// In your main.c or project config
#define STM32F4XX  // For F4 series (default)
// OR
#define STM32F1XX  // For F1 series
// OR
#define STM32F0XX  // For F0 series

#include "board_support/board_config.h"
```

### Step 2: Build Your First Project

```bash
cd 06_Real_World_Projects

# Compile LED blink example
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
  -I../drivers/inc \
  -I../drivers/inc/board_support \
  led_blink.c ../drivers/src/stm32f446re_gpio_drivers.c \
  -o led_blink.elf

# Convert to binary
arm-none-eabi-objcopy -O binary led_blink.elf led_blink.bin
```

### Step 3: Flash to Board

```bash
# Using ST-Link
st-flash write led_blink.bin 0x8000000

# OR using OpenOCD
openocd -f interface/stlink.cfg \
        -f target/stm32f4x.cfg \
        -c "program led_blink.elf verify reset exit"
```

### Step 4: Verify

- LED should start blinking on your board! 🎉
- Default LED pin: PA5 (user LED on Nucleo boards)

---

## 📚 Learning Path

### Beginner (Week 1-2)
Start here if you're new to STM32 or embedded development:

1. **Virtual Simulation** (No hardware needed)
   ```bash
   cd 07_Virtual_Simulation
   cat README.md  # Read the guide
   make && ./build/test_gpio
   ```

2. **Register Access** (Understanding hardware)
   ```bash
   cd 01_Register_Access
   cat README.md
   # Study: direct_register_read.c, direct_register_write.c
   ```

3. **First LED Blink** (Your first project!)
   ```bash
   cd 06_Real_World_Projects
   # Study and compile: led_blink.c
   ```

### Intermediate (Week 3-4)
4. **GPIO Drivers** (Using HAL layer)
   ```bash
   cd 04_Bare_Metal_Drivers
   # Study: gpio_driver_example.c
   ```

5. **Interrupts** (Async event handling)
   ```bash
   cd 03_Interrupt_Handling
   # Study: interrupt_basics.c, exti_gpio_interrupt.c
   ```

6. **Button + Interrupt Project**
   ```bash
   cd 06_Real_World_Projects
   # Study: button_interrupt.c
   ```

### Advanced (Week 5-6)
7. **HAL Setup** (Peripheral configuration)
   ```bash
   cd 02_HAL_Setup
   # Study: clock_configuration.c, uart_hal_setup.c
   ```

8. **Debugging** (Professional techniques)
   ```bash
   cd 05_Debugging_Advanced
   # Study: debug_example.c, profiling_example.c
   ```

9. **UART Communication**
   ```bash
   cd 06_Real_World_Projects
   # Study: uart_echo.c
   ```

---

## 🔧 Common Build Patterns

### Pattern 1: Single File Build
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
  -I../drivers/inc \
  your_file.c \
  -o output.elf
```

### Pattern 2: With Driver Libraries
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
  -I../drivers/inc \
  -I../drivers/inc/board_support \
  your_file.c \
  ../drivers/src/stm32f446re_gpio_drivers.c \
  -o output.elf
```

### Pattern 3: With Linker Script
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
  -T stm32f446re.ld \
  -I../drivers/inc \
  your_file.c ../drivers/src/*.c \
  --specs=nosys.specs \
  -o output.elf
```

---

## 🐛 Troubleshooting

### Problem: "arm-none-eabi-gcc: command not found"
**Solution**: Install ARM GCC toolchain (see Prerequisites)

### Problem: "stm32f446re.h: No such file or directory"
**Solution**: Add include path: `-I../drivers/inc`

### Problem: Board not detected
**Solution**: 
1. Check USB connection
2. Install ST-Link drivers
3. Try: `st-info --probe`

### Problem: Flash fails with "Cannot connect to target"
**Solution**:
1. Reconnect board
2. Press RESET button
3. Check ST-Link firmware: `st-flash --version`

### Problem: LED not blinking
**Solution**:
1. Verify pin assignment (PA5 for Nucleo)
2. Check clock configuration
3. Add delay between toggles

---

## 📖 Next Steps

### Documentation
- 📘 [Module Guide](MODULE_GUIDE.md) - Detailed module documentation
- 🔍 [Troubleshooting](TROUBLESHOOTING.md) - Common issues and solutions
- 🧪 [Simulation Guide](Documentation/SIMULATION_GUIDE.md) - Virtual testing

### Code Exploration
- `drivers/inc/` - HAL driver headers
- `drivers/src/` - Driver implementations
- `01_Register_Access/` - Low-level register access
- `06_Real_World_Projects/` - Complete working projects

### External Resources
- [STM32 Reference Manual](https://www.st.com/resource/en/reference_manual/)
- [ARM Cortex-M4 TRM](https://developer.arm.com/documentation/)
- [STM32 Community](https://community.st.com/)

---

## 💡 Pro Tips

1. **Start with simulation** - Test logic without hardware
2. **Use debug macros** - Include `debug_utils.h` for logging
3. **Read datasheets** - Understand your specific MCU
4. **Oscilloscope is your friend** - Verify signals when debugging
5. **Git branching** - Try experiments in separate branches

---

## 🤝 Getting Help

- **Issues**: Open a GitHub issue
- **Questions**: Check README.md and MODULE_GUIDE.md
- **Examples**: Look in 06_Real_World_Projects/
- **Debugging**: See TROUBLESHOOTING.md

---

## ✅ Quick Verification Checklist

After setup, verify you can:
- [ ] Build virtual simulation (`cd 07_Virtual_Simulation && make`)
- [ ] Run virtual tests (`./build/test_gpio`)
- [ ] Compile example code (`arm-none-eabi-gcc --version`)
- [ ] (With hardware) Flash a binary (`st-flash write ...`)
- [ ] (With hardware) See LED blinking

**All checked?** You're ready to develop! 🚀

---

*For detailed documentation, see [README.md](README.md) and [MODULE_GUIDE.md](MODULE_GUIDE.md)*

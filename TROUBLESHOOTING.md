# 🔧 Troubleshooting Guide - STM32 HAL Development

Common issues and their solutions.

---

## 📋 Table of Contents

1. [Build Issues](#build-issues)
2. [Flashing Problems](#flashing-problems)
3. [Runtime Issues](#runtime-issues)
4. [Hardware Issues](#hardware-issues)
5. [Debugging Issues](#debugging-issues)
6. [Virtual Simulation Issues](#virtual-simulation-issues)

---

## 🛠️ Build Issues

### Issue: `arm-none-eabi-gcc: command not found`

**Cause**: ARM GCC toolchain not installed or not in PATH.

**Solution**:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi

# Verify installation
arm-none-eabi-gcc --version

# If installed but not found, add to PATH
export PATH=$PATH:/usr/local/gcc-arm-none-eabi/bin
```

**Alternative**: Download from [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

---

### Issue: `fatal error: stm32f446re.h: No such file or directory`

**Cause**: Include path not specified.

**Solution**:
```bash
# Add include path
arm-none-eabi-gcc -I../drivers/inc -I../drivers/inc/board_support ...

# Or use absolute path
arm-none-eabi-gcc -I/path/to/STM32_HAL_Devlopment/drivers/inc ...
```

**Makefile Solution**:
```makefile
INCLUDES = -I../drivers/inc -I../drivers/inc/board_support
CFLAGS = $(INCLUDES) -mcpu=cortex-m4 -mthumb
```

---

### Issue: `undefined reference to 'GPIO_Init'`

**Cause**: Implementation file not linked.

**Solution**:
```bash
# Include source file in compilation
arm-none-eabi-gcc \
  your_file.c \
  ../drivers/src/stm32f446re_gpio_drivers.c \
  -o output.elf
```

---

### Issue: `multiple definition of 'main'`

**Cause**: Multiple files with `main()` function.

**Solution**:
```bash
# Compile only one file with main()
arm-none-eabi-gcc main_file.c helper.c -o output.elf

# Or comment out main() in one file
```

---

### Issue: Compilation warnings about implicit declarations

**Cause**: Missing includes or function prototypes.

**Solution**:
```c
// Add proper includes
#include <stdint.h>
#include <stdio.h>
#include "stm32f446re.h"
#include "stm32f446re_gpio_drivers.h"

// Or add function prototypes
void my_function(void);
```

---

## 📥 Flashing Problems

### Issue: `st-flash: command not found`

**Cause**: ST-Link tools not installed.

**Solution**:
```bash
# Ubuntu/Debian
sudo apt-get install stlink-tools

# Or build from source
git clone https://github.com/stlink-org/stlink.git
cd stlink
make
sudo make install
```

---

### Issue: `Error: libusb_open() failed with LIBUSB_ERROR_ACCESS`

**Cause**: USB permissions issue.

**Solution**:
```bash
# Add udev rules
sudo cp /usr/share/stlink/49-stlinkv*.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger

# Or run with sudo (not recommended)
sudo st-flash write firmware.bin 0x8000000
```

---

### Issue: `Cannot connect to target`

**Symptoms**: st-flash or OpenOCD can't connect to board.

**Solutions**:

1. **Check USB connection**:
   ```bash
   lsusb | grep -i st-link
   # Should see: STMicroelectronics ST-LINK
   ```

2. **Reset board**:
   - Press RESET button on board
   - Disconnect and reconnect USB

3. **Check ST-Link firmware**:
   ```bash
   st-flash --version
   st-info --probe
   ```

4. **Try different USB port/cable**:
   - Use USB 2.0 port (not 3.0)
   - Try a different USB cable

5. **Update ST-Link firmware**:
   - Download STSW-LINK007 from ST website
   - Run ST-Link Upgrade utility

---

### Issue: `Flash write failed at address 0x08000000`

**Cause**: Flash protection enabled or invalid binary.

**Solutions**:

1. **Disable read protection**:
   ```bash
   st-flash unlock
   ```

2. **Verify binary size**:
   ```bash
   ls -lh firmware.bin
   # Should be reasonable (< 512KB for F446RE)
   ```

3. **Mass erase**:
   ```bash
   st-flash erase
   st-flash write firmware.bin 0x8000000
   ```

---

## 🏃 Runtime Issues

### Issue: Program doesn't start / LED doesn't blink

**Diagnosis Steps**:

1. **Verify clock configuration**:
   ```c
   // Check if clock is enabled
   RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA clock
   ```

2. **Check GPIO configuration**:
   ```c
   // Verify pin mode
   GPIOA->MODER |= (1 << 10);  // PA5 output
   ```

3. **Add delays**:
   ```c
   // Increase delay for visibility
   for(volatile int i = 0; i < 1000000; i++);
   ```

4. **Verify pin assignment**:
   - Nucleo F4: User LED is PA5
   - Nucleo F1: User LED is PA5
   - Check your board schematic

---

### Issue: Hard fault / System crash

**Symptoms**: Program stops, debug LED blinks rapidly, no response.

**Common Causes**:

1. **Stack overflow**:
   ```c
   // Reduce stack usage
   uint8_t big_array[10000];  // ← Too big for stack!
   
   // Solution: Use static or heap
   static uint8_t big_array[10000];
   ```

2. **Null pointer dereference**:
   ```c
   GPIO_RegDef_t *gpio = NULL;
   gpio->ODR = 0x00;  // ← Crash!
   
   // Solution: Check pointers
   if (gpio != NULL) {
       gpio->ODR = 0x00;
   }
   ```

3. **Unaligned memory access**:
   ```c
   // ARM requires aligned access
   uint32_t *ptr = (uint32_t*)0x20000001;  // ← Misaligned!
   *ptr = 0x12345678;  // Crash!
   ```

4. **Invalid peripheral access**:
   ```c
   // Enable clock before accessing peripheral
   GPIO_PeriClockControl(GPIOA, ENABLE);  // ← Must do this first!
   GPIOA->ODR = 0x00;
   ```

**Debugging**:
```c
// Add hard fault handler
void HardFault_Handler(void) {
    while(1) {
        // Examine registers in debugger
        // Check CFSR, HFSR, MMFAR, BFAR
    }
}
```

---

### Issue: Interrupts not working

**Checklist**:

1. **Enable NVIC**:
   ```c
   NVIC_EnableIRQ(EXTI0_IRQn);
   ```

2. **Set priority** (optional):
   ```c
   NVIC_SetPriority(EXTI0_IRQn, 5);
   ```

3. **Enable interrupt source**:
   ```c
   // For EXTI
   EXTI->IMR |= (1 << 0);  // Unmask line 0
   ```

4. **Implement ISR**:
   ```c
   void EXTI0_IRQHandler(void) {
       // Clear pending flag
       EXTI->PR |= (1 << 0);
       
       // Handle interrupt
   }
   ```

5. **Enable global interrupts**:
   ```c
   __enable_irq();  // Or: asm("CPSIE I");
   ```

---

## 🔌 Hardware Issues

### Issue: GPIO pin not responding

**Diagnosis**:

1. **Check with multimeter**:
   - Measure voltage at pin
   - Should be ~3.3V (HIGH) or 0V (LOW)

2. **Verify with oscilloscope**:
   - Connect to GPIO pin
   - Toggle pin in code
   - Should see square wave

3. **Check schematics**:
   - Verify pin is connected
   - Check for pull-ups/downs
   - Look for conflicting peripherals

**Solutions**:

```c
// 1. Verify clock enabled
GPIOA_PCLK_EN();

// 2. Configure as output
GPIOA->MODER &= ~(3 << (2*5));  // Clear mode bits
GPIOA->MODER |= (1 << (2*5));   // Set output mode

// 3. Test with direct register access
GPIOA->ODR |= (1 << 5);   // Set HIGH
for(volatile int i=0; i<1000000; i++);
GPIOA->ODR &= ~(1 << 5);  // Set LOW
```

---

### Issue: UART not transmitting

**Common Problems**:

1. **Clock not enabled**:
   ```c
   RCC->APB1ENR |= (1 << 17);  // USART2 clock
   RCC->AHB1ENR |= (1 << 0);   // GPIOA clock
   ```

2. **Pins not configured**:
   ```c
   // PA2 = TX, PA3 = RX (USART2)
   GPIOA->MODER |= (2 << (2*2));  // AF mode for PA2
   GPIOA->AFR[0] |= (7 << (4*2)); // AF7 (USART2)
   ```

3. **Baud rate incorrect**:
   ```c
   // For 9600 baud @ 16MHz APB1
   USART2->BRR = 0x683;  // See datasheet calculation
   ```

4. **UART not enabled**:
   ```c
   USART2->CR1 |= (1 << 13);  // UE: UART enable
   USART2->CR1 |= (1 << 3);   // TE: Transmitter enable
   ```

**Testing**:
```c
// Send character
while(!(USART2->SR & (1 << 7)));  // Wait for TXE
USART2->DR = 'A';
```

---

## 🐛 Debugging Issues

### Issue: OpenOCD won't connect

**Solution**:
```bash
# Check interface file
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# Try different debug speed
openocd -f interface/stlink.cfg \
        -c "adapter speed 1000" \
        -f target/stm32f4x.cfg

# Reset halt
openocd -f interface/stlink.cfg \
        -f target/stm32f4x.cfg \
        -c "init; reset halt"
```

---

### Issue: GDB connection refused

**Solution**:
```bash
# 1. Start OpenOCD in terminal 1
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# 2. In terminal 2
arm-none-eabi-gdb firmware.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

---

### Issue: Can't see debug output

**Solutions**:

1. **Use semihosting**:
   ```c
   extern void initialise_monitor_handles(void);
   
   int main(void) {
       initialise_monitor_handles();
       printf("Hello World\n");
   }
   ```

2. **Use SWO/ITM**:
   - Configure ITM in debugger
   - Use ITM_SendChar()

3. **Use UART**:
   ```c
   // Redirect printf to UART
   int _write(int file, char *ptr, int len) {
       for(int i=0; i<len; i++) {
           while(!(USART2->SR & (1<<7)));
           USART2->DR = ptr[i];
       }
       return len;
   }
   ```

---

## 🎮 Virtual Simulation Issues

### Issue: `make: *** No rule to make target 'test'`

**Solution**:
```bash
# Ensure you're in correct directory
cd 07_Virtual_Simulation

# Check Makefile exists
ls -la Makefile

# Use available targets
make help
make all
```

---

### Issue: Simulation tests fail

**Solutions**:

1. **Clean and rebuild**:
   ```bash
   make clean
   make
   ```

2. **Check test output**:
   ```bash
   ./build/test_gpio -v  # Verbose mode if available
   ```

3. **Verify GCC version**:
   ```bash
   gcc --version
   # Should be 7.0 or newer
   ```

---

### Issue: `Segmentation fault` in simulation

**Cause**: Invalid memory access in simulation code.

**Debug**:
```bash
# Run with debugger
gdb ./build/test_gpio
(gdb) run
(gdb) bt  # Backtrace when crash occurs
```

---

## 📞 Getting Additional Help

### Before Asking for Help

Create a minimal reproducible example:

1. **Simplify code** to smallest failing case
2. **Document** what you expected vs. what happened
3. **List** your environment:
   - Board model
   - Compiler version
   - OS
   - Tool versions

4. **Show** relevant code snippets
5. **Include** error messages (full text)

### Resources

- **Documentation**: README.md, MODULE_GUIDE.md
- **Examples**: 06_Real_World_Projects/
- **Community**: [STM32 Community](https://community.st.com/)
- **GitHub Issues**: Open an issue with [bug] tag

---

## 🔍 Diagnostic Commands

### System Information
```bash
# Check tools
arm-none-eabi-gcc --version
st-flash --version
openocd --version

# Check USB devices
lsusb | grep -i st-link

# Check board connection
st-info --probe

# Check permissions
groups  # Should include dialout or plugdev
```

### Test Compilation
```bash
# Minimal test
echo 'int main(void) { while(1); }' > test.c
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb test.c -o test.elf
echo $?  # Should be 0 (success)
```

---

## ✅ Quick Fixes Checklist

- [ ] Correct include paths (-I flags)
- [ ] Source files linked in build
- [ ] Peripheral clocks enabled
- [ ] GPIO configured correctly
- [ ] Adequate delays added
- [ ] Board connected properly
- [ ] ST-Link firmware updated
- [ ] Correct pin assignments used
- [ ] USB permissions set
- [ ] Proper reset after flash

---

*Still having issues? Check [MODULE_GUIDE.md](MODULE_GUIDE.md) or open a GitHub issue.*

/*
 * gpio_hal_setup.c
 *
 * Demonstrates GPIO HAL driver initialization and usage
 * Learning objectives:
 * - Using GPIO driver API
 * - Proper peripheral initialization sequence
 * - Configuration structures
 * - HAL vs direct register access
 */

#include "../drivers/inc/stm32f446re_gpio_drivers.h"
#include <stdio.h>

void simple_delay(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++);
}

void example_basic_output(void)
{
    printf("\n=== Example 1: Basic GPIO Output ===\n");
    
    /* Configure PA5 as output (LED on Nucleo board) */
    GPIO_Handle_t GpioLed;
    
    printf("1. Configuring PA5 as output for LED...\n");
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    printf("   Pin: PA5\n");
    printf("   Mode: Output\n");
    printf("   Speed: Fast\n");
    printf("   Output Type: Push-Pull\n");
    printf("   Pull-up/down: None\n");
    
    GPIO_Init(&GpioLed);
    printf("2. GPIO initialized successfully\n\n");
    
    /* Blink LED */
    printf("3. Blinking LED 3 times...\n");
    for (int i = 0; i < 3; i++) {
        printf("   Blink %d: ON\n", i + 1);
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
        simple_delay(500000);
        
        printf("   Blink %d: OFF\n", i + 1);
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, RESET);
        simple_delay(500000);
    }
}

void example_input_with_pullup(void)
{
    printf("\n=== Example 2: GPIO Input with Pull-up ===\n");
    
    /* Configure PC13 as input with pull-up (User button on Nucleo) */
    GPIO_Handle_t GpioBtn;
    
    printf("1. Configuring PC13 as input (User Button)...\n");
    GpioBtn.pGPIOx = GPIOC;
    GpioBtn.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    GpioBtn.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_IN;
    GpioBtn.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioBtn.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_PIN_PU;
    
    printf("   Pin: PC13\n");
    printf("   Mode: Input\n");
    printf("   Pull-up: Enabled\n");
    
    GPIO_Init(&GpioBtn);
    printf("2. GPIO initialized successfully\n\n");
    
    /* Read button state */
    printf("3. Reading button state...\n");
    for (int i = 0; i < 5; i++) {
        uint8_t button_state = GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13);
        printf("   Sample %d: Button = %s\n", i + 1, 
               button_state ? "Released" : "Pressed");
        simple_delay(1000000);
    }
}

void example_multiple_pins(void)
{
    printf("\n=== Example 3: Multiple GPIO Pins ===\n");
    
    /* Configure multiple LEDs on Port B */
    printf("1. Configuring multiple output pins (PB0, PB1, PB2)...\n");
    
    GPIO_Handle_t GpioLeds[3];
    
    for (int i = 0; i < 3; i++) {
        GpioLeds[i].pGPIOx = GPIOB;
        GpioLeds[i].GPIO_PINConfig.GPIO_PinNumber = i;
        GpioLeds[i].GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
        GpioLeds[i].GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
        GpioLeds[i].GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
        GpioLeds[i].GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
        
        GPIO_Init(&GpioLeds[i]);
        printf("   PB%d configured\n", i);
    }
    
    printf("\n2. Running LED pattern...\n");
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int i = 0; i < 3; i++) {
            printf("   LED %d ON\n", i);
            GPIO_WriteToOutputPin(GPIOB, i, SET);
            simple_delay(300000);
            GPIO_WriteToOutputPin(GPIOB, i, RESET);
        }
    }
    
    printf("   Pattern complete\n");
}

void example_toggle_pin(void)
{
    printf("\n=== Example 4: GPIO Toggle Operation ===\n");
    
    GPIO_Handle_t GpioLed;
    
    printf("1. Configuring PA5 for toggle demonstration...\n");
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GpioLed.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    GPIO_Init(&GpioLed);
    
    printf("\n2. Toggling LED 10 times...\n");
    for (int i = 0; i < 10; i++) {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        printf("   Toggle %d\n", i + 1);
        simple_delay(200000);
    }
}

void example_port_operations(void)
{
    printf("\n=== Example 5: Port-Level Operations ===\n");
    
    /* Configure entire port */
    printf("1. Configuring Port A pins 0-3 as output...\n");
    
    for (int i = 0; i < 4; i++) {
        GPIO_Handle_t GpioPin;
        GpioPin.pGPIOx = GPIOA;
        GpioPin.GPIO_PINConfig.GPIO_PinNumber = i;
        GpioPin.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
        GpioPin.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
        GpioPin.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
        GpioPin.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
        
        GPIO_Init(&GpioPin);
    }
    
    printf("\n2. Writing patterns to port...\n");
    uint16_t patterns[] = {0x0001, 0x0003, 0x0007, 0x000F, 0x0000};
    
    for (int i = 0; i < 5; i++) {
        printf("   Pattern 0x%04X\n", patterns[i]);
        GPIO_WriteToOutputPort(GPIOA, patterns[i]);
        simple_delay(500000);
    }
}

void demonstrate_clock_control(void)
{
    printf("\n=== Example 6: Peripheral Clock Control ===\n");
    
    printf("1. Initial clock state:\n");
    printf("   GPIOA clock: %s\n", 
           (RCC->AHB1ENR & (1 << 0)) ? "Enabled" : "Disabled");
    printf("   GPIOB clock: %s\n",
           (RCC->AHB1ENR & (1 << 1)) ? "Enabled" : "Disabled");
    
    printf("\n2. Enabling peripheral clocks via API...\n");
    GPIO_PeriClockControl(GPIOA, ENABLE);
    GPIO_PeriClockControl(GPIOB, ENABLE);
    
    printf("   GPIOA clock: %s\n",
           (RCC->AHB1ENR & (1 << 0)) ? "Enabled" : "Disabled");
    printf("   GPIOB clock: %s\n",
           (RCC->AHB1ENR & (1 << 1)) ? "Enabled" : "Disabled");
}

int main(void)
{
    printf("=== GPIO HAL Setup and Usage Examples ===\n");
    
    // Show clock control
    demonstrate_clock_control();
    
    // Basic output example
    example_basic_output();
    
    // Input with pull-up
    example_input_with_pullup();
    
    // Multiple pins
    example_multiple_pins();
    
    // Toggle operation
    example_toggle_pin();
    
    // Port operations
    example_port_operations();
    
    printf("\n=== All GPIO Examples Complete ===\n");
    printf("\nKey Takeaways:\n");
    printf("1. Always enable peripheral clock before use\n");
    printf("2. Use configuration structures for clean setup\n");
    printf("3. HAL provides higher-level abstraction\n");
    printf("4. Toggle function is efficient for LED blinking\n");
    printf("5. Port operations allow multi-pin control\n");
    
    return 0;
}

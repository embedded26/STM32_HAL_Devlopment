/*
 * led_blink.c
 *
 * Simple LED blink project - The "Hello World" of embedded systems
 * Learning objectives:
 * - Complete project structure
 * - GPIO initialization and control
 * - Software delays
 * - Clean code organization
 */

#include "../drivers/inc/stm32f446re_gpio_drivers.h"
#include "../drivers/inc/debug_utils.h"
#include <stdio.h>

/* Global error tracker */
DebugErrorTracker_t g_debug_error_tracker = {0};

/* Configuration */
#define LED_PIN         GPIO_PIN_NO_5
#define LED_PORT        GPIOA
#define BLINK_DELAY_MS  500

/* Simple delay function */
void delay_ms(uint32_t milliseconds)
{
    /* Rough delay - assumes 16 MHz */
    for (volatile uint32_t i = 0; i < milliseconds * 4000; i++);
}

/* LED initialization */
void LED_Init(void)
{
    DEBUG_INFO("Initializing LED on PA5");
    
    GPIO_Handle_t led_handle;
    
    /* Configure LED pin */
    led_handle.pGPIOx = LED_PORT;
    led_handle.GPIO_PINConfig.GPIO_PinNumber = LED_PIN;
    led_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led_handle.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    /* Initialize GPIO */
    GPIO_Init(&led_handle);
    
    DEBUG_INFO("LED initialized successfully");
}

/* LED control functions */
void LED_On(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_PIN, SET);
}

void LED_Off(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_PIN, RESET);
}

void LED_Toggle(void)
{
    GPIO_ToggleOutputPin(LED_PORT, LED_PIN);
}

/* Main application */
int main(void)
{
    printf("=== LED Blink Project ===\n\n");
    
    DEBUG_INFO("System starting...");
    
    /* Initialize hardware */
    LED_Init();
    
    printf("LED blinking on PA5\n");
    printf("Blink period: %d ms\n\n", BLINK_DELAY_MS * 2);
    
    /* Main loop */
    uint32_t blink_count = 0;
    
    while (1)
    {
        LED_Toggle();
        
        blink_count++;
        if (blink_count % 10 == 0) {
            DEBUG_INFO("Blink count: %lu", (unsigned long)blink_count);
        }
        
        delay_ms(BLINK_DELAY_MS);
    }
    
    return 0;
}

/* 
 * Project Structure:
 * 
 * led_blink/
 * ├── main.c (this file)
 * ├── Makefile
 * ├── startup_stm32f446re.s
 * ├── STM32F446RE.ld
 * └── drivers/ (from repository)
 * 
 * Compilation:
 * arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb \
 *     -O2 -Wall -g \
 *     -I../drivers/inc \
 *     led_blink.c ../drivers/src/stm32f446re_gpio_drivers.c \
 *     -o led_blink.elf
 * 
 * Flash to board:
 * st-flash write led_blink.bin 0x08000000
 * 
 * Expected Behavior:
 * - LED on PA5 (green LED on Nucleo board) blinks
 * - Blink period: 1 second (500ms ON, 500ms OFF)
 * - Debug messages every 10 blinks (if connected to debugger)
 * 
 * Troubleshooting:
 * 1. LED not blinking?
 *    - Check power supply
 *    - Verify clock configuration
 *    - Check GPIO initialization
 * 
 * 2. Wrong blink rate?
 *    - Adjust delay multiplier for your clock frequency
 *    - Use DWT for precise timing
 * 
 * 3. LED dim or flickering?
 *    - Check output type (should be push-pull)
 *    - Verify drive strength
 * 
 * Extensions:
 * 1. Add different blink patterns (SOS, heartbeat)
 * 2. Control blink rate with potentiometer (ADC)
 * 3. Use timer interrupt instead of delay
 * 4. Add multiple LEDs with different patterns
 * 5. Implement breathing effect using PWM
 */

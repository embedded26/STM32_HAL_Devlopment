/*
 * gpio_driver_example.c
 *
 * Demonstrates usage of the custom GPIO driver created in drivers/
 * Learning objectives:
 * - Using custom HAL drivers
 * - Driver API best practices
 * - Proper initialization sequences
 * - Combining driver functions
 */

#include "../drivers/inc/stm32f446re_gpio_drivers.h"
#include <stdio.h>

void simple_delay(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++);
}

void example_led_control(void)
{
    printf("=== Example 1: LED Control Using GPIO Driver ===\n\n");
    
    GPIO_Handle_t led_handle;
    
    /* Configure LED on PA5 */
    printf("1. Configuring PA5 as output for LED...\n");
    led_handle.pGPIOx = GPIOA;
    led_handle.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    led_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led_handle.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    GPIO_Init(&led_handle);
    printf("   LED configured successfully\n\n");
    
    /* Blink LED */
    printf("2. Blinking LED 5 times...\n");
    for (int i = 0; i < 5; i++) {
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
        printf("   LED ON\n");
        simple_delay(500000);
        
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, RESET);
        printf("   LED OFF\n");
        simple_delay(500000);
    }
    
    printf("LED blink complete\n\n");
}

void example_button_led(void)
{
    printf("=== Example 2: Button Controlled LED ===\n\n");
    
    GPIO_Handle_t button_handle, led_handle;
    
    /* Configure button on PC13 (User button on Nucleo) */
    printf("1. Configuring PC13 as input (Button)...\n");
    button_handle.pGPIOx = GPIOC;
    button_handle.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
    button_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_IN;
    button_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    button_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_PIN_PU;
    
    GPIO_Init(&button_handle);
    printf("   Button configured with pull-up\n");
    
    /* Configure LED on PA5 */
    printf("2. Configuring PA5 as output (LED)...\n");
    led_handle.pGPIOx = GPIOA;
    led_handle.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    led_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led_handle.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    GPIO_Init(&led_handle);
    printf("   LED configured\n\n");
    
    /* Read button and control LED */
    printf("3. Reading button and controlling LED (10 samples)...\n");
    for (int i = 0; i < 10; i++) {
        uint8_t button_state = GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13);
        
        if (button_state == 0) {  // Button pressed (active low)
            GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, SET);
            printf("   Button PRESSED - LED ON\n");
        } else {
            GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, RESET);
            printf("   Button RELEASED - LED OFF\n");
        }
        
        simple_delay(500000);
    }
    
    printf("Button-LED demo complete\n\n");
}

void example_traffic_light(void)
{
    printf("=== Example 3: Traffic Light Simulation ===\n\n");
    
    GPIO_Handle_t red_led, yellow_led, green_led;
    
    /* Configure Red LED on PB0 */
    printf("1. Configuring traffic lights...\n");
    red_led.pGPIOx = GPIOB;
    red_led.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
    red_led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    red_led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
    red_led.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    red_led.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    GPIO_Init(&red_led);
    printf("   Red LED (PB0) configured\n");
    
    /* Configure Yellow LED on PB1 */
    yellow_led.pGPIOx = GPIOB;
    yellow_led.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
    yellow_led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    yellow_led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
    yellow_led.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    yellow_led.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    GPIO_Init(&yellow_led);
    printf("   Yellow LED (PB1) configured\n");
    
    /* Configure Green LED on PB2 */
    green_led.pGPIOx = GPIOB;
    green_led.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    green_led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    green_led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_MEDIUM;
    green_led.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    green_led.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    GPIO_Init(&green_led);
    printf("   Green LED (PB2) configured\n\n");
    
    /* Run traffic light sequence */
    printf("2. Running traffic light sequence (2 cycles)...\n\n");
    
    for (int cycle = 0; cycle < 2; cycle++) {
        printf("   Cycle %d:\n", cycle + 1);
        
        /* Red */
        printf("     RED\n");
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, SET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, RESET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, RESET);
        simple_delay(2000000);
        
        /* Yellow */
        printf("     YELLOW\n");
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, RESET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, SET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, RESET);
        simple_delay(1000000);
        
        /* Green */
        printf("     GREEN\n");
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_0, RESET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_1, RESET);
        GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_2, SET);
        simple_delay(2000000);
    }
    
    /* Turn off all LEDs */
    GPIO_WriteToOutputPort(GPIOB, 0x0000);
    printf("\nTraffic light simulation complete\n\n");
}

void example_port_operations(void)
{
    printf("=== Example 4: Port-Level Operations ===\n\n");
    
    GPIO_Handle_t gpio_handle;
    
    /* Configure pins PA0-PA3 as outputs */
    printf("1. Configuring PA0-PA3 as outputs...\n");
    for (int i = 0; i < 4; i++) {
        gpio_handle.pGPIOx = GPIOA;
        gpio_handle.GPIO_PINConfig.GPIO_PinNumber = i;
        gpio_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
        gpio_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
        gpio_handle.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
        gpio_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
        
        GPIO_Init(&gpio_handle);
    }
    printf("   PA0-PA3 configured\n\n");
    
    /* Binary counter on port */
    printf("2. Running 4-bit binary counter...\n");
    for (uint16_t count = 0; count < 16; count++) {
        GPIO_WriteToOutputPort(GPIOA, count);
        printf("   Count: %2d (0x%04X)\n", count, count);
        simple_delay(300000);
    }
    
    GPIO_WriteToOutputPort(GPIOA, 0x0000);
    printf("Counter complete\n\n");
}

void example_toggle_pin(void)
{
    printf("=== Example 5: Using Toggle Function ===\n\n");
    
    GPIO_Handle_t led_handle;
    
    /* Configure PA5 */
    led_handle.pGPIOx = GPIOA;
    led_handle.GPIO_PINConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    led_handle.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_handle.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    led_handle.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_handle.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    GPIO_Init(&led_handle);
    
    printf("Toggling PA5 LED 10 times...\n");
    for (int i = 0; i < 10; i++) {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        printf("  Toggle %d\n", i + 1);
        simple_delay(200000);
    }
    
    printf("Toggle demo complete\n\n");
}

int main(void)
{
    printf("=== GPIO Driver Usage Examples ===\n\n");
    
    printf("These examples demonstrate the GPIO driver API from\n");
    printf("drivers/src/stm32f446re_gpio_drivers.c\n\n");
    
    example_led_control();
    
    example_button_led();
    
    example_traffic_light();
    
    example_port_operations();
    
    example_toggle_pin();
    
    printf("=== All Examples Complete ===\n\n");
    
    printf("Driver Functions Used:\n");
    printf("- GPIO_PeriClockControl(): Enable peripheral clock\n");
    printf("- GPIO_Init(): Initialize GPIO pin\n");
    printf("- GPIO_ReadFromInputPin(): Read single pin\n");
    printf("- GPIO_WriteToOutputPin(): Write to single pin\n");
    printf("- GPIO_WriteToOutputPort(): Write to entire port\n");
    printf("- GPIO_ToggleOutputPin(): Toggle pin state\n");
    
    return 0;
}

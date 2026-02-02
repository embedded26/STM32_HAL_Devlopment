/*
 * sim_hal_wrapper.c - HAL Abstraction Layer for Virtual Drivers
 * Provides HAL-like API for testing without hardware
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// HAL Status codes
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// GPIO Configuration Structure (HAL-compatible)
typedef struct {
    uint32_t Pin;        // Pin number (0-15)
    uint32_t Mode;       // GPIO mode
    uint32_t Pull;       // Pull-up/Pull-down
    uint32_t Speed;      // GPIO speed
    uint32_t Alternate;  // Alternate function
} GPIO_InitTypeDef;

// GPIO Pin States
typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

// Forward declarations for virtual driver functions
extern void VirtualGPIO_Init(void);
extern uint8_t VirtualGPIO_EnableClock(uint8_t port);
extern uint8_t VirtualGPIO_ConfigurePin(uint8_t port, uint8_t pin, uint8_t mode, 
                                        uint8_t output_type, uint8_t speed, uint8_t pupd);
extern uint8_t VirtualGPIO_WritePin(uint8_t port, uint8_t pin, uint8_t value);
extern uint8_t VirtualGPIO_ReadPin(uint8_t port, uint8_t pin, uint8_t *value);
extern uint8_t VirtualGPIO_TogglePin(uint8_t port, uint8_t pin);
extern uint8_t VirtualGPIO_SetAltFunction(uint8_t port, uint8_t pin, uint8_t alt_func);
extern uint8_t VirtualGPIO_ConfigureInterrupt(uint8_t port, uint8_t pin, uint8_t mode,
                                               void (*handler)(uint8_t, uint8_t));

extern void VirtualNVIC_Init(void);
extern uint8_t VirtualNVIC_EnableIRQ(uint8_t irq_num);
extern uint8_t VirtualNVIC_DisableIRQ(uint8_t irq_num);
extern uint8_t VirtualNVIC_SetPriority(uint8_t irq_num, uint8_t priority);

// GPIO Port mapping (A=0, B=1, etc.)
#define GPIOA_PORT 0
#define GPIOB_PORT 1
#define GPIOC_PORT 2
#define GPIOD_PORT 3
#define GPIOE_PORT 4
#define GPIOF_PORT 5
#define GPIOG_PORT 6
#define GPIOH_PORT 7
#define GPIOI_PORT 8

// GPIO Modes (HAL compatible)
#define GPIO_MODE_INPUT         0x00000000U
#define GPIO_MODE_OUTPUT_PP     0x00000001U
#define GPIO_MODE_OUTPUT_OD     0x00000011U
#define GPIO_MODE_AF_PP         0x00000002U
#define GPIO_MODE_AF_OD         0x00000012U
#define GPIO_MODE_ANALOG        0x00000003U
#define GPIO_MODE_IT_RISING     0x10110000U
#define GPIO_MODE_IT_FALLING    0x10210000U
#define GPIO_MODE_IT_RISING_FALLING 0x10310000U

// GPIO Pull-up/Pull-down
#define GPIO_NOPULL             0x00000000U
#define GPIO_PULLUP             0x00000001U
#define GPIO_PULLDOWN           0x00000002U

// GPIO Speed
#define GPIO_SPEED_FREQ_LOW     0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM  0x00000001U
#define GPIO_SPEED_FREQ_HIGH    0x00000002U
#define GPIO_SPEED_FREQ_VERY_HIGH 0x00000003U

// Helper function to convert HAL mode to virtual mode
static uint8_t hal_mode_to_virtual(uint32_t hal_mode) {
    switch (hal_mode & 0xFF) {
        case 0x00: return 0;  // INPUT
        case 0x01: return 1;  // OUTPUT_PP
        case 0x02: return 2;  // ALTERNATE
        case 0x03: return 3;  // ANALOG
        case 0x11: return 1;  // OUTPUT_OD (treat as output)
        case 0x12: return 2;  // AF_OD (treat as alternate)
        default: return 0;
    }
}

// Helper function to check if interrupt mode
static uint8_t is_interrupt_mode(uint32_t hal_mode) {
    return (hal_mode & 0x10000000U) != 0;
}

// Helper function to get interrupt type from HAL mode
static uint8_t get_interrupt_type(uint32_t hal_mode) {
    if (hal_mode & 0x00100000U) return 4;  // RISING
    if (hal_mode & 0x00200000U) return 5;  // FALLING
    if (hal_mode & 0x00300000U) return 6;  // BOTH
    return 4;  // Default to rising
}

// HAL GPIO Initialization
HAL_StatusTypeDef HAL_GPIO_Init(uint8_t port, GPIO_InitTypeDef *GPIO_Init) {
    if (GPIO_Init == NULL) {
        printf("[HAL] ERROR: NULL GPIO_Init structure\n");
        return HAL_ERROR;
    }
    
    printf("[HAL] Initializing GPIO port %d, pin %d\n", port, GPIO_Init->Pin);
    
    // Enable clock
    if (!VirtualGPIO_EnableClock(port)) {
        return HAL_ERROR;
    }
    
    // Check if interrupt mode
    if (is_interrupt_mode(GPIO_Init->Mode)) {
        uint8_t irq_type = get_interrupt_type(GPIO_Init->Mode);
        if (!VirtualGPIO_ConfigureInterrupt(port, GPIO_Init->Pin, irq_type, NULL)) {
            return HAL_ERROR;
        }
    } else {
        // Configure pin
        uint8_t mode = hal_mode_to_virtual(GPIO_Init->Mode);
        uint8_t output_type = (GPIO_Init->Mode & 0x10) ? 1 : 0;
        
        if (!VirtualGPIO_ConfigurePin(port, GPIO_Init->Pin, mode, output_type,
                                       GPIO_Init->Speed, GPIO_Init->Pull)) {
            return HAL_ERROR;
        }
        
        // Set alternate function if needed
        if (mode == 2) {  // Alternate function
            if (!VirtualGPIO_SetAltFunction(port, GPIO_Init->Pin, GPIO_Init->Alternate)) {
                return HAL_ERROR;
            }
        }
    }
    
    printf("[HAL] GPIO initialization complete\n");
    return HAL_OK;
}

// HAL GPIO Read Pin
GPIO_PinState HAL_GPIO_ReadPin(uint8_t port, uint16_t pin) {
    uint8_t value = 0;
    
    if (VirtualGPIO_ReadPin(port, pin, &value)) {
        return value ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
    
    return GPIO_PIN_RESET;
}

// HAL GPIO Write Pin
void HAL_GPIO_WritePin(uint8_t port, uint16_t pin, GPIO_PinState state) {
    VirtualGPIO_WritePin(port, pin, state == GPIO_PIN_SET ? 1 : 0);
}

// HAL GPIO Toggle Pin
void HAL_GPIO_TogglePin(uint8_t port, uint16_t pin) {
    VirtualGPIO_TogglePin(port, pin);
}

// HAL NVIC Enable IRQ
void HAL_NVIC_EnableIRQ(uint8_t irq_num) {
    VirtualNVIC_EnableIRQ(irq_num);
}

// HAL NVIC Disable IRQ
void HAL_NVIC_DisableIRQ(uint8_t irq_num) {
    VirtualNVIC_DisableIRQ(irq_num);
}

// HAL NVIC Set Priority
void HAL_NVIC_SetPriority(uint8_t irq_num, uint32_t preempt_priority, uint32_t sub_priority) {
    // Combine priorities using 4-bit scheme:
    // Bits [3:2] = preemption priority (0-3)
    // Bits [1:0] = sub priority (0-3)
    // This simulates the typical STM32 priority grouping
    uint8_t priority = (preempt_priority << 2) | (sub_priority & 0x3);
    if (priority > 15) priority = 15;
    
    VirtualNVIC_SetPriority(irq_num, priority);
}

// HAL Delay (simulated)
void HAL_Delay(uint32_t ms) {
    printf("[HAL] Delay %d ms (simulated)\n", ms);
}

// HAL Init
HAL_StatusTypeDef HAL_Init(void) {
    printf("[HAL] HAL Initialization\n");
    VirtualGPIO_Init();
    VirtualNVIC_Init();
    return HAL_OK;
}

// Example: Blink LED using HAL API
void example_blink_led(void) {
    printf("\n=== Example: Blink LED using HAL API ===\n");
    
    HAL_Init();
    
    // Configure PA5 as output (LED)
    GPIO_InitTypeDef gpio;
    gpio.Pin = 5;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = 0;
    
    if (HAL_GPIO_Init(GPIOA_PORT, &gpio) != HAL_OK) {
        printf("ERROR: Failed to initialize GPIO\n");
        return;
    }
    
    // Blink LED
    for (int i = 0; i < 5; i++) {
        printf("\nBlink cycle %d:\n", i + 1);
        HAL_GPIO_WritePin(GPIOA_PORT, 5, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(GPIOA_PORT, 5, GPIO_PIN_RESET);
        HAL_Delay(500);
    }
    
    printf("\n=== Blink Complete ===\n");
}

// Example: Button with interrupt
void button_irq_callback(uint8_t port, uint8_t pin) {
    printf("[Callback] Button pressed on GPIO%c.%d!\n", 'A' + port, pin);
}

void example_button_interrupt(void) {
    printf("\n=== Example: Button with Interrupt ===\n");
    
    HAL_Init();
    
    // Configure PC13 as input with pull-up and interrupt on falling edge
    GPIO_InitTypeDef gpio;
    gpio.Pin = 13;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = 0;
    
    if (HAL_GPIO_Init(GPIOC_PORT, &gpio) != HAL_OK) {
        printf("ERROR: Failed to initialize GPIO\n");
        return;
    }
    
    // Enable EXTI interrupt
    HAL_NVIC_SetPriority(40, 0, 0);  // EXTI15_10 IRQ
    HAL_NVIC_EnableIRQ(40);
    
    printf("Button configured. Simulate interrupt:\n");
    // In real code, interrupt would be triggered by hardware
    // Here we simulate it
    extern void VirtualGPIO_SimulateInterrupt(uint8_t port, uint8_t pin, uint8_t edge);
    VirtualGPIO_SimulateInterrupt(GPIOC_PORT, 13, 0);  // Falling edge
    
    printf("\n=== Example Complete ===\n");
}

// Test main function
int main(void) {
    printf("=== HAL Wrapper Test Suite ===\n");
    
    // Test 1: LED Blink
    example_blink_led();
    
    // Test 2: Button with interrupt
    example_button_interrupt();
    
    // Test 3: Multiple GPIO configuration
    printf("\n=== Test: Multiple GPIO Pins ===\n");
    HAL_Init();
    
    GPIO_InitTypeDef gpio;
    
    // Configure multiple pins
    for (int pin = 0; pin < 8; pin++) {
        gpio.Pin = pin;
        gpio.Mode = GPIO_MODE_OUTPUT_PP;
        gpio.Pull = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
        gpio.Alternate = 0;
        
        HAL_GPIO_Init(GPIOB_PORT, &gpio);
    }
    
    // Toggle all pins
    printf("\nToggling all configured pins:\n");
    for (int pin = 0; pin < 8; pin++) {
        HAL_GPIO_TogglePin(GPIOB_PORT, pin);
    }
    
    // Test 4: Alternate function
    printf("\n=== Test: Alternate Function (USART) ===\n");
    
    // Configure PA9 as USART1_TX (AF7)
    gpio.Pin = 9;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = 7;
    
    HAL_GPIO_Init(GPIOA_PORT, &gpio);
    
    // Configure PA10 as USART1_RX (AF7)
    gpio.Pin = 10;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = 7;
    
    HAL_GPIO_Init(GPIOA_PORT, &gpio);
    
    printf("\n=== All HAL Tests Complete ===\n");
    return 0;
}

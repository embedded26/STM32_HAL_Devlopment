/*
 * sim_gpio.c - Virtual GPIO Driver for QEMU/Simulation Testing
 * Provides GPIO pin multiplexing, interrupt handling, and error simulation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define MAX_GPIO_PORTS 9  // GPIOA to GPIOI
#define MAX_GPIO_PINS 16  // 0-15 pins per port

// GPIO Pin Modes
#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_ALTERNATE 2
#define GPIO_MODE_ANALOG    3
#define GPIO_MODE_IT_RISING 4
#define GPIO_MODE_IT_FALLING 5
#define GPIO_MODE_IT_BOTH   6

// GPIO Output Types
#define GPIO_OTYPE_PP 0  // Push-Pull
#define GPIO_OTYPE_OD 1  // Open-Drain

// GPIO Speed
#define GPIO_SPEED_LOW    0
#define GPIO_SPEED_MEDIUM 1
#define GPIO_SPEED_FAST   2
#define GPIO_SPEED_HIGH   3

// GPIO Pull-up/Pull-down
#define GPIO_PUPD_NONE 0
#define GPIO_PUPD_UP   1
#define GPIO_PUPD_DOWN 2

// Error Codes
#define GPIO_ERROR_NONE         0
#define GPIO_ERROR_INVALID_PORT 1
#define GPIO_ERROR_INVALID_PIN  2
#define GPIO_ERROR_CONFIG       3
#define GPIO_ERROR_INTERRUPT    4
#define GPIO_ERROR_PINMUX       5

// Virtual GPIO Pin State
typedef struct {
    uint8_t mode;           // Pin mode
    uint8_t output_type;    // Push-pull or open-drain
    uint8_t speed;          // Speed setting
    uint8_t pupd;           // Pull-up/pull-down
    uint8_t alt_function;   // Alternate function (0-15)
    uint8_t value;          // Current pin value (0 or 1)
    uint8_t irq_enabled;    // Interrupt enabled flag
    void (*irq_handler)(uint8_t port, uint8_t pin); // Interrupt callback
} VirtualGPIOPin;

// Virtual GPIO Port
typedef struct {
    VirtualGPIOPin pins[MAX_GPIO_PINS];
    uint8_t clock_enabled;
    char name;  // 'A' to 'I'
} VirtualGPIOPort;

// Global GPIO state
static VirtualGPIOPort gpio_ports[MAX_GPIO_PORTS];
static uint8_t gpio_initialized = 0;
static uint8_t error_injection_enabled = 0;
static uint8_t last_error = GPIO_ERROR_NONE;

// Function to initialize virtual GPIO system
void VirtualGPIO_Init(void) {
    if (gpio_initialized) return;
    
    srand(time(NULL));
    
    // Initialize all ports
    for (int port = 0; port < MAX_GPIO_PORTS; port++) {
        gpio_ports[port].name = 'A' + port;
        gpio_ports[port].clock_enabled = 0;
        
        // Initialize all pins to default state
        for (int pin = 0; pin < MAX_GPIO_PINS; pin++) {
            gpio_ports[port].pins[pin].mode = GPIO_MODE_INPUT;
            gpio_ports[port].pins[pin].output_type = GPIO_OTYPE_PP;
            gpio_ports[port].pins[pin].speed = GPIO_SPEED_LOW;
            gpio_ports[port].pins[pin].pupd = GPIO_PUPD_NONE;
            gpio_ports[port].pins[pin].alt_function = 0;
            gpio_ports[port].pins[pin].value = 0;
            gpio_ports[port].pins[pin].irq_enabled = 0;
            gpio_ports[port].pins[pin].irq_handler = NULL;
        }
    }
    
    gpio_initialized = 1;
    printf("[VirtualGPIO] Initialized %d GPIO ports with %d pins each\n", 
           MAX_GPIO_PORTS, MAX_GPIO_PINS);
}

// Enable/disable error injection for testing
void VirtualGPIO_SetErrorInjection(uint8_t enable) {
    error_injection_enabled = enable;
    printf("[VirtualGPIO] Error injection %s\n", enable ? "ENABLED" : "DISABLED");
}

// Get last error code
uint8_t VirtualGPIO_GetLastError(void) {
    return last_error;
}

// Simulate random errors when error injection is enabled
static uint8_t inject_error(void) {
    if (!error_injection_enabled) return 0;
    
    // 10% chance of error when injection is enabled
    if (rand() % 10 == 0) {
        last_error = (rand() % 5) + 1;  // Errors 1-5
        printf("[VirtualGPIO] ERROR INJECTED: Code %d\n", last_error);
        return 1;
    }
    return 0;
}

// Enable clock for GPIO port
uint8_t VirtualGPIO_EnableClock(uint8_t port) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS) {
        last_error = GPIO_ERROR_INVALID_PORT;
        return 0;
    }
    
    if (inject_error()) return 0;
    
    gpio_ports[port].clock_enabled = 1;
    printf("[VirtualGPIO] Clock enabled for GPIO%c\n", gpio_ports[port].name);
    return 1;
}

// Configure GPIO pin
uint8_t VirtualGPIO_ConfigurePin(uint8_t port, uint8_t pin, uint8_t mode, 
                                  uint8_t output_type, uint8_t speed, uint8_t pupd) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS) {
        last_error = GPIO_ERROR_INVALID_PORT;
        printf("[VirtualGPIO] ERROR: Invalid port %d\n", port);
        return 0;
    }
    
    if (pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_INVALID_PIN;
        printf("[VirtualGPIO] ERROR: Invalid pin %d\n", pin);
        return 0;
    }
    
    if (!gpio_ports[port].clock_enabled) {
        last_error = GPIO_ERROR_CONFIG;
        printf("[VirtualGPIO] ERROR: Clock not enabled for GPIO%c\n", 
               gpio_ports[port].name);
        return 0;
    }
    
    if (inject_error()) return 0;
    
    // Configure the pin
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    p->mode = mode;
    p->output_type = output_type;
    p->speed = speed;
    p->pupd = pupd;
    
    printf("[VirtualGPIO] Configured GPIO%c.%d: Mode=%d, Type=%d, Speed=%d, PUPD=%d\n",
           gpio_ports[port].name, pin, mode, output_type, speed, pupd);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Set alternate function for pin (Pin Multiplexing)
uint8_t VirtualGPIO_SetAltFunction(uint8_t port, uint8_t pin, uint8_t alt_func) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_PINMUX;
        printf("[VirtualGPIO] ERROR: Invalid port/pin for alternate function\n");
        return 0;
    }
    
    if (inject_error()) return 0;
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    if (p->mode != GPIO_MODE_ALTERNATE) {
        last_error = GPIO_ERROR_PINMUX;
        printf("[VirtualGPIO] WARNING: Pin not in alternate mode\n");
    }
    
    p->alt_function = alt_func;
    printf("[VirtualGPIO] GPIO%c.%d alternate function set to AF%d\n",
           gpio_ports[port].name, pin, alt_func);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Write to GPIO pin
uint8_t VirtualGPIO_WritePin(uint8_t port, uint8_t pin, uint8_t value) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_INVALID_PORT;
        return 0;
    }
    
    if (inject_error()) return 0;
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    if (p->mode != GPIO_MODE_OUTPUT) {
        printf("[VirtualGPIO] WARNING: Writing to non-output pin GPIO%c.%d\n",
               gpio_ports[port].name, pin);
    }
    
    p->value = value ? 1 : 0;
    printf("[VirtualGPIO] GPIO%c.%d <- %d\n", gpio_ports[port].name, pin, p->value);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Read from GPIO pin
uint8_t VirtualGPIO_ReadPin(uint8_t port, uint8_t pin, uint8_t *value) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_INVALID_PIN;
        return 0;
    }
    
    if (inject_error()) return 0;
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    
    // Simulate input based on pull-up/pull-down or random for floating
    if (p->mode == GPIO_MODE_INPUT) {
        if (p->pupd == GPIO_PUPD_UP) {
            *value = 1;
        } else if (p->pupd == GPIO_PUPD_DOWN) {
            *value = 0;
        } else {
            *value = rand() % 2;  // Simulate floating input
        }
        p->value = *value;
    } else {
        *value = p->value;
    }
    
    printf("[VirtualGPIO] GPIO%c.%d -> %d\n", gpio_ports[port].name, pin, *value);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Toggle GPIO pin
uint8_t VirtualGPIO_TogglePin(uint8_t port, uint8_t pin) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_INVALID_PIN;
        return 0;
    }
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    p->value = !p->value;
    
    printf("[VirtualGPIO] GPIO%c.%d toggled to %d\n", 
           gpio_ports[port].name, pin, p->value);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Configure interrupt for pin
uint8_t VirtualGPIO_ConfigureInterrupt(uint8_t port, uint8_t pin, uint8_t mode,
                                        void (*handler)(uint8_t, uint8_t)) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        last_error = GPIO_ERROR_INTERRUPT;
        return 0;
    }
    
    if (inject_error()) return 0;
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    p->mode = mode;
    p->irq_enabled = 1;
    p->irq_handler = handler;
    
    printf("[VirtualGPIO] Interrupt configured for GPIO%c.%d (Mode: %d)\n",
           gpio_ports[port].name, pin, mode);
    
    last_error = GPIO_ERROR_NONE;
    return 1;
}

// Simulate interrupt trigger
void VirtualGPIO_SimulateInterrupt(uint8_t port, uint8_t pin, uint8_t edge) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS || pin >= MAX_GPIO_PINS) {
        printf("[VirtualGPIO] ERROR: Cannot simulate interrupt on invalid pin\n");
        return;
    }
    
    VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
    
    if (!p->irq_enabled) {
        printf("[VirtualGPIO] WARNING: Interrupt not enabled for GPIO%c.%d\n",
               gpio_ports[port].name, pin);
        return;
    }
    
    // Check if edge matches configured mode
    uint8_t should_trigger = 0;
    if ((p->mode == GPIO_MODE_IT_RISING && edge == 1) ||
        (p->mode == GPIO_MODE_IT_FALLING && edge == 0) ||
        (p->mode == GPIO_MODE_IT_BOTH)) {
        should_trigger = 1;
    }
    
    if (should_trigger) {
        printf("[VirtualGPIO] INTERRUPT triggered on GPIO%c.%d (Edge: %s)\n",
               gpio_ports[port].name, pin, edge ? "RISING" : "FALLING");
        
        if (p->irq_handler != NULL) {
            p->irq_handler(port, pin);
        } else {
            printf("[VirtualGPIO] WARNING: No interrupt handler registered\n");
        }
    }
}

// Print GPIO port state
void VirtualGPIO_PrintPortState(uint8_t port) {
    if (!gpio_initialized) VirtualGPIO_Init();
    
    if (port >= MAX_GPIO_PORTS) {
        printf("[VirtualGPIO] ERROR: Invalid port\n");
        return;
    }
    
    printf("\n=== GPIO%c State ===\n", gpio_ports[port].name);
    printf("Clock: %s\n", gpio_ports[port].clock_enabled ? "ENABLED" : "DISABLED");
    printf("Pin | Mode | Type | Speed | PUPD | AF | Value | IRQ\n");
    printf("----+------+------+-------+------+----+-------+----\n");
    
    for (int pin = 0; pin < MAX_GPIO_PINS; pin++) {
        VirtualGPIOPin *p = &gpio_ports[port].pins[pin];
        printf("%2d  |  %d   |  %d   |   %d   |  %d   | %2d |   %d   | %s\n",
               pin, p->mode, p->output_type, p->speed, p->pupd, 
               p->alt_function, p->value, p->irq_enabled ? "Y" : "N");
    }
    printf("==================\n\n");
}

// Example interrupt handler
void example_irq_handler(uint8_t port, uint8_t pin) {
    printf("[IRQ Handler] Interrupt handled for GPIO%c.%d\n", 'A' + port, pin);
}

#ifdef RUN_STANDALONE_TEST
// Test function - only compiled when RUN_STANDALONE_TEST is defined
int main(void) {
    printf("=== Virtual GPIO Driver Test ===\n\n");
    
    // Initialize
    VirtualGPIO_Init();
    
    // Test 1: Basic GPIO configuration
    printf("\n--- Test 1: Basic GPIO Configuration ---\n");
    VirtualGPIO_EnableClock(0);  // GPIOA
    VirtualGPIO_ConfigurePin(0, 5, GPIO_MODE_OUTPUT, GPIO_OTYPE_PP, 
                             GPIO_SPEED_HIGH, GPIO_PUPD_NONE);
    VirtualGPIO_WritePin(0, 5, 1);
    VirtualGPIO_TogglePin(0, 5);
    
    // Test 2: Input pin with pull-up
    printf("\n--- Test 2: Input Pin Configuration ---\n");
    VirtualGPIO_EnableClock(1);  // GPIOB
    VirtualGPIO_ConfigurePin(1, 3, GPIO_MODE_INPUT, GPIO_OTYPE_PP,
                             GPIO_SPEED_LOW, GPIO_PUPD_UP);
    uint8_t value;
    VirtualGPIO_ReadPin(1, 3, &value);
    
    // Test 3: Alternate function (Pin Mux)
    printf("\n--- Test 3: Alternate Function ---\n");
    VirtualGPIO_ConfigurePin(0, 9, GPIO_MODE_ALTERNATE, GPIO_OTYPE_PP,
                             GPIO_SPEED_FAST, GPIO_PUPD_NONE);
    VirtualGPIO_SetAltFunction(0, 9, 7);  // AF7 for USART
    
    // Test 4: Interrupt configuration
    printf("\n--- Test 4: Interrupt Configuration ---\n");
    VirtualGPIO_EnableClock(2);  // GPIOC
    VirtualGPIO_ConfigureInterrupt(2, 13, GPIO_MODE_IT_RISING, example_irq_handler);
    VirtualGPIO_SimulateInterrupt(2, 13, 1);  // Rising edge
    VirtualGPIO_SimulateInterrupt(2, 13, 0);  // Falling edge (should not trigger)
    
    // Test 5: Error injection
    printf("\n--- Test 5: Error Injection ---\n");
    VirtualGPIO_SetErrorInjection(1);
    for (int i = 0; i < 5; i++) {
        if (!VirtualGPIO_WritePin(0, 5, 1)) {
            printf("Operation failed with error code: %d\n", 
                   VirtualGPIO_GetLastError());
        }
    }
    VirtualGPIO_SetErrorInjection(0);
    
    // Test 6: Display port state
    printf("\n--- Test 6: Port State Display ---\n");
    VirtualGPIO_PrintPortState(0);
    
    printf("\n=== All Tests Complete ===\n");
    return 0;
}
#endif  // RUN_STANDALONE_TEST

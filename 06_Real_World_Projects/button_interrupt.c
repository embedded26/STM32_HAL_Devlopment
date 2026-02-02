/*
 * button_interrupt.c
 *
 * Button with interrupt-driven LED control
 * Learning objectives:
 * - External interrupt configuration
 * - Debouncing in software
 * - ISR best practices
 * - State management
 */

#include "../drivers/inc/stm32f446re_gpio_drivers.h"
#include "../drivers/inc/stm32f446re.h"
#include "../drivers/inc/debug_utils.h"
#include <stdio.h>

/* Global error tracker */
DebugErrorTracker_t g_debug_error_tracker = {0};

/* Configuration */
#define BUTTON_PIN      GPIO_PIN_NO_13
#define BUTTON_PORT     GPIOC
#define LED_PIN         GPIO_PIN_NO_5
#define LED_PORT        GPIOA
#define DEBOUNCE_MS     50

/* EXTI and SYSCFG */
typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} EXTI_RegDef_t;

typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t PMC;
    volatile uint32_t EXTICR[4];
} SYSCFG_RegDef_t;

#define EXTI ((EXTI_RegDef_t*)EXT1_BASEADDR)
#define SYSCFG ((SYSCFG_RegDef_t*)SYSCFC_BASEADDR)
#define NVIC_ISER1 ((volatile uint32_t*)0xE000E104)

/* Global state */
volatile uint8_t led_state = 0;
volatile uint32_t button_presses = 0;
volatile uint32_t last_press_time = 0;

/* Delay function */
void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 4000; i++);
}

/* Get tick count (simplified - in real project use SysTick) */
uint32_t get_tick(void)
{
    static uint32_t tick = 0;
    return tick++;
}

/* Initialize LED */
void LED_Init(void)
{
    GPIO_Handle_t led;
    
    led.pGPIOx = LED_PORT;
    led.GPIO_PINConfig.GPIO_PinNumber = LED_PIN;
    led.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PINConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_NO_PUPD;
    
    GPIO_Init(&led);
    
    DEBUG_INFO("LED initialized");
}

/* Initialize button with interrupt */
void Button_Init(void)
{
    GPIO_Handle_t button;
    
    /* Configure button GPIO */
    button.pGPIOx = BUTTON_PORT;
    button.GPIO_PINConfig.GPIO_PinNumber = BUTTON_PIN;
    button.GPIO_PINConfig.GPIO_PinMode = GPIO_MODE_IN;
    button.GPIO_PINConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    button.GPIO_PINConfig.GPIO_PinPupdCpntrol = GPIO_PIN_PU;
    
    GPIO_Init(&button);
    
    DEBUG_INFO("Button GPIO configured");
    
    /* Configure EXTI */
    RCC->APB2ENR |= (1 << 14);  // Enable SYSCFG clock
    
    /* Map PC13 to EXTI13 */
    SYSCFG->EXTICR[3] &= ~(0xF << 4);
    SYSCFG->EXTICR[3] |= (0x2 << 4);  // Port C
    
    /* Configure falling edge trigger */
    EXTI->FTSR |= (1 << 13);
    
    /* Enable EXTI13 interrupt */
    EXTI->IMR |= (1 << 13);
    
    /* Enable in NVIC (EXTI15_10 is IRQ 40) */
    *NVIC_ISER1 |= (1 << (40 - 32));
    
    DEBUG_INFO("Button interrupt configured");
}

/* LED control */
void LED_On(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_PIN, SET);
    led_state = 1;
}

void LED_Off(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_PIN, RESET);
    led_state = 0;
}

void LED_Toggle(void)
{
    if (led_state) {
        LED_Off();
    } else {
        LED_On();
    }
}

/* Interrupt handler */
void EXTI15_10_IRQHandler(void)
{
    /* Check if EXTI13 triggered */
    if (EXTI->PR & (1 << 13))
    {
        /* Debouncing */
        uint32_t current_time = get_tick();
        if (current_time - last_press_time > DEBOUNCE_MS)
        {
            /* Valid button press */
            button_presses++;
            LED_Toggle();
            
            last_press_time = current_time;
        }
        
        /* Clear pending flag */
        EXTI->PR |= (1 << 13);
    }
}

/* Main application */
int main(void)
{
    printf("=== Button Interrupt Project ===\n\n");
    
    DEBUG_INFO("System starting...");
    
    /* Initialize hardware */
    LED_Init();
    Button_Init();
    
    printf("Button-controlled LED\n");
    printf("Press user button (PC13) to toggle LED\n\n");
    
    LED_Off();  // Start with LED off
    
    /* Main loop */
    uint32_t last_count = 0;
    
    while (1)
    {
        /* Check if button was pressed */
        if (button_presses != last_count)
        {
            printf("Button pressed! Count: %lu, LED: %s\n",
                   (unsigned long)button_presses,
                   led_state ? "ON" : "OFF");
            
            last_count = button_presses;
        }
        
        /* Main loop can do other things */
        delay_ms(100);
    }
    
    return 0;
}

/*
 * Project Features:
 * 
 * 1. Interrupt-driven button handling
 *    - Doesn't waste CPU polling
 *    - Responds quickly to button press
 * 
 * 2. Software debouncing
 *    - Prevents multiple triggers from mechanical bounce
 *    - Uses time-based filtering
 * 
 * 3. State tracking
 *    - Maintains LED state
 *    - Counts button presses
 * 
 * 4. Clean ISR implementation
 *    - Checks which interrupt fired
 *    - Clears pending flag
 *    - Minimal processing in ISR
 * 
 * Expected Behavior:
 * - LED toggles on each button press
 * - Debouncing prevents multiple toggles from single press
 * - Press count displayed in console
 * 
 * Troubleshooting:
 * 1. Multiple toggles per press?
 *    - Increase debounce time
 *    - Check bounce duration with oscilloscope
 * 
 * 2. Interrupt not firing?
 *    - Verify EXTI configuration
 *    - Check NVIC enabled
 *    - Confirm SYSCFG mapping
 * 
 * 3. LED state incorrect?
 *    - Ensure ISR clears pending flag
 *    - Check for race conditions
 * 
 * Extensions:
 * 1. Add long press detection
 * 2. Implement double-click detection
 * 3. Control different LEDs with multiple buttons
 * 4. Add button press patterns (e.g., Morse code)
 * 5. Implement hold-and-repeat functionality
 * 6. Use state machine for complex button behaviors
 */

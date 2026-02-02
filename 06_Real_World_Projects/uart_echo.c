/*
 * uart_echo.c
 *
 * UART echo program - receives characters and echoes them back
 * Learning objectives:
 * - UART peripheral setup
 * - Transmit and receive operations
 * - Interrupt-driven I/O
 * - Circular buffer implementation
 */

#include "../drivers/inc/stm32f446re.h"
#include "../drivers/inc/debug_utils.h"
#include <stdio.h>
#include <string.h>

/* Global error tracker */
DebugErrorTracker_t g_debug_error_tracker = {0};

/* UART Register Structure */
typedef struct {
    volatile uint32_t SR;    // Status register
    volatile uint32_t DR;    // Data register
    volatile uint32_t BRR;   // Baud rate register
    volatile uint32_t CR1;   // Control register 1
    volatile uint32_t CR2;   // Control register 2
    volatile uint32_t CR3;   // Control register 3
} USART_RegDef_t;

#define USART2 ((USART_RegDef_t*)USART2_BASEADDR)

/* Configuration */
#define BAUD_RATE       115200
#define APB1_CLOCK      42000000UL

/* Circular buffer for RX */
#define RX_BUFFER_SIZE  128

typedef struct {
    uint8_t buffer[RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} CircularBuffer_t;

CircularBuffer_t rx_buffer = {0};

/* Delay function */
void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 4000; i++);
}

/* Buffer functions */
uint8_t buffer_is_empty(CircularBuffer_t *buf)
{
    return (buf->count == 0);
}

uint8_t buffer_is_full(CircularBuffer_t *buf)
{
    return (buf->count >= RX_BUFFER_SIZE);
}

void buffer_put(CircularBuffer_t *buf, uint8_t data)
{
    if (!buffer_is_full(buf)) {
        buf->buffer[buf->head] = data;
        buf->head = (buf->head + 1) % RX_BUFFER_SIZE;
        buf->count++;
    }
}

uint8_t buffer_get(CircularBuffer_t *buf)
{
    uint8_t data = 0;
    
    if (!buffer_is_empty(buf)) {
        data = buf->buffer[buf->tail];
        buf->tail = (buf->tail + 1) % RX_BUFFER_SIZE;
        buf->count--;
    }
    
    return data;
}

/* UART GPIO configuration */
void UART_GPIO_Init(void)
{
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= (1 << 0);
    
    /* PA2 (TX) and PA3 (RX) - AF7 */
    GPIOA->MODER &= ~((0x3 << 4) | (0x3 << 6));
    GPIOA->MODER |= ((0x2 << 4) | (0x2 << 6));  // AF mode
    
    GPIOA->OSPEEDR |= ((0x3 << 4) | (0x3 << 6));  // High speed
    
    GPIOA->PUPDR &= ~((0x3 << 4) | (0x3 << 6));
    GPIOA->PUPDR |= ((0x1 << 4) | (0x1 << 6));  // Pull-up
    
    /* AF7 for USART2 */
    GPIOA->AFR[0] &= ~((0xF << 8) | (0xF << 12));
    GPIOA->AFR[0] |= ((0x7 << 8) | (0x7 << 12));
    
    DEBUG_INFO("UART GPIO configured");
}

/* UART peripheral initialization */
void UART_Init(void)
{
    /* Enable USART2 clock */
    RCC->APB1ENR |= (1 << 17);
    
    /* Baud rate calculation */
    uint32_t usartdiv = APB1_CLOCK / (16 * BAUD_RATE);
    uint32_t fraction = ((APB1_CLOCK % (16 * BAUD_RATE)) * 16) / (16 * BAUD_RATE);
    USART2->BRR = (usartdiv << 4) | (fraction & 0xF);
    
    /* Configure: 8-bit, no parity, 1 stop bit */
    USART2->CR1 = 0;
    USART2->CR1 |= (1 << 3);   // TE - Transmit enable
    USART2->CR1 |= (1 << 2);   // RE - Receive enable
    USART2->CR1 |= (1 << 5);   // RXNE interrupt enable
    
    USART2->CR2 = 0;           // 1 stop bit
    
    /* Enable USART */
    USART2->CR1 |= (1 << 13);  // UE - USART enable
    
    /* Enable USART2 interrupt in NVIC */
    volatile uint32_t *NVIC_ISER1 = (volatile uint32_t*)0xE000E104;
    *NVIC_ISER1 |= (1 << (38 - 32));  // USART2 is IRQ 38
    
    DEBUG_INFO("UART initialized at %lu baud", (unsigned long)BAUD_RATE);
}

/* UART transmit functions */
void UART_TransmitChar(char c)
{
    /* Wait for TXE */
    while (!(USART2->SR & (1 << 7)));
    
    /* Write data */
    USART2->DR = c;
}

void UART_TransmitString(const char *str)
{
    while (*str) {
        UART_TransmitChar(*str++);
    }
}

/* UART interrupt handler */
void USART2_IRQHandler(void)
{
    /* Check RXNE flag */
    if (USART2->SR & (1 << 5))
    {
        /* Read data (clears RXNE flag) */
        uint8_t data = USART2->DR & 0xFF;
        
        /* Put in buffer */
        buffer_put(&rx_buffer, data);
    }
}

/* Process received data */
void process_rx_data(void)
{
    while (!buffer_is_empty(&rx_buffer))
    {
        uint8_t c = buffer_get(&rx_buffer);
        
        /* Echo character */
        UART_TransmitChar(c);
        
        /* Handle special characters */
        if (c == '\r') {
            UART_TransmitChar('\n');
        }
        else if (c == '\n') {
            // Already sent
        }
        else if (c == 0x7F || c == 0x08) {  // Backspace/DEL
            UART_TransmitChar(' ');
            UART_TransmitChar(c);
        }
    }
}

/* Main application */
int main(void)
{
    printf("=== UART Echo Project ===\n\n");
    
    DEBUG_INFO("System starting...");
    
    /* Initialize UART */
    UART_GPIO_Init();
    UART_Init();
    
    /* Send welcome message */
    const char *welcome = "\r\n=== STM32 UART Echo ===\r\n";
    UART_TransmitString(welcome);
    UART_TransmitString("Type characters - they will be echoed back\r\n");
    UART_TransmitString("Baud: 115200, 8N1\r\n\r\n");
    
    printf("UART echo active\n");
    printf("Connect serial terminal to USART2 (PA2/PA3)\n");
    printf("Settings: 115200 baud, 8N1\n\n");
    
    /* Main loop */
    uint32_t loop_count = 0;
    
    while (1)
    {
        /* Process received data */
        process_rx_data();
        
        /* Periodic status */
        if (++loop_count >= 1000000) {
            loop_count = 0;
            DEBUG_LOG("Buffer: %d/%d", rx_buffer.count, RX_BUFFER_SIZE);
        }
    }
    
    return 0;
}

/*
 * Project Features:
 * 
 * 1. UART Configuration
 *    - 115200 baud, 8 data bits, no parity, 1 stop bit
 *    - GPIO alternate function setup
 *    - Proper baud rate calculation
 * 
 * 2. Interrupt-driven Reception
 *    - RXNE interrupt for efficient RX
 *    - Circular buffer for data storage
 *    - No polling required
 * 
 * 3. Echo Functionality
 *    - Echoes received characters back
 *    - Handles special characters (CR, LF, backspace)
 *    - Buffer overflow protection
 * 
 * 4. Clean Code Organization
 *    - Separate initialization functions
 *    - Buffer management abstraction
 *    - ISR kept minimal
 * 
 * Expected Behavior:
 * - Characters typed in terminal appear back
 * - Enter key produces CR+LF
 * - Backspace works correctly
 * - No data loss up to buffer size
 * 
 * Troubleshooting:
 * 1. No characters received?
 *    - Check baud rate settings
 *    - Verify GPIO configuration
 *    - Confirm TX/RX not swapped
 * 
 * 2. Garbled characters?
 *    - Check baud rate calculation
 *    - Verify APB1 clock frequency
 *    - Check for framing errors
 * 
 * 3. Missing characters?
 *    - Increase buffer size
 *    - Process data faster
 *    - Check interrupt priority
 * 
 * Extensions:
 * 1. Implement command line interface
 * 2. Add AT command parser
 * 3. Implement DMA for transfers
 * 4. Add flow control (RTS/CTS)
 * 5. Create line buffer with editing
 * 6. Implement printf redirection to UART
 * 7. Add error detection and reporting
 * 8. Support different baud rates dynamically
 */

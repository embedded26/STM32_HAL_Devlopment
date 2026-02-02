/*
 * uart_hal_setup.c
 *
 * Demonstrates UART peripheral HAL setup skeleton
 * Learning objectives:
 * - UART peripheral configuration
 * - Baud rate calculation
 * - Frame format setup
 * - Basic transmit/receive setup
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* UART Configuration Structure (simplified) */
typedef struct {
    uint32_t BaudRate;
    uint8_t WordLength;    // 8 or 9 bits
    uint8_t StopBits;      // 1 or 2
    uint8_t Parity;        // None, Even, Odd
    uint8_t Mode;          // TX, RX, or TX_RX
} UART_Config_t;

/* UART register structure (simplified) */
typedef struct {
    volatile uint32_t SR;    // Status register
    volatile uint32_t DR;    // Data register
    volatile uint32_t BRR;   // Baud rate register
    volatile uint32_t CR1;   // Control register 1
    volatile uint32_t CR2;   // Control register 2
    volatile uint32_t CR3;   // Control register 3
} USART_RegDef_t;

#define USART2 ((USART_RegDef_t*)USART2_BASEADDR)

void UART_ClockEnable(void)
{
    printf("=== UART Clock Configuration ===\n");
    
    /* Enable USART2 clock (APB1 bus) */
    printf("1. Enabling USART2 peripheral clock...\n");
    RCC->APB1ENR |= (1 << 17);  // USART2EN bit
    printf("   USART2 clock enabled\n");
    printf("   APB1ENR = 0x%08lX\n\n", (unsigned long)RCC->APB1ENR);
}

void UART_GPIOSetup(void)
{
    printf("=== UART GPIO Configuration ===\n");
    
    /*
     * USART2 Pins on STM32F446RE:
     * PA2 - USART2_TX (AF7)
     * PA3 - USART2_RX (AF7)
     */
    
    printf("1. Enabling GPIOA clock...\n");
    RCC->AHB1ENR |= (1 << 0);
    
    printf("2. Configuring PA2 (TX) and PA3 (RX)...\n");
    
    /* Set PA2 and PA3 to Alternate Function mode */
    GPIOA->MODER &= ~((0x3 << 4) | (0x3 << 6));  // Clear bits
    GPIOA->MODER |= ((0x2 << 4) | (0x2 << 6));   // AF mode (10)
    printf("   Mode: Alternate Function\n");
    
    /* Set speed to high */
    GPIOA->OSPEEDR &= ~((0x3 << 4) | (0x3 << 6));
    GPIOA->OSPEEDR |= ((0x3 << 4) | (0x3 << 6));
    printf("   Speed: High\n");
    
    /* Set output type to push-pull */
    GPIOA->OTYPER &= ~((1 << 2) | (1 << 3));
    printf("   Output Type: Push-Pull\n");
    
    /* Set pull-up for both pins */
    GPIOA->PUPDR &= ~((0x3 << 4) | (0x3 << 6));
    GPIOA->PUPDR |= ((0x1 << 4) | (0x1 << 6));
    printf("   Pull-up/down: Pull-up\n");
    
    /* Configure alternate function AF7 for USART2 */
    GPIOA->AFR[0] &= ~((0xF << 8) | (0xF << 12));  // Clear AFR2 and AFR3
    GPIOA->AFR[0] |= ((0x7 << 8) | (0x7 << 12));   // AF7
    printf("   Alternate Function: AF7 (USART2)\n\n");
}

void UART_BaudRateConfig(uint32_t baud_rate)
{
    printf("=== UART Baud Rate Configuration ===\n");
    
    /*
     * Baud rate calculation:
     * BRR = (f_PCLK) / (8 * (2 - OVER8) * baud_rate)
     * 
     * Assuming:
     * - APB1 clock = 42 MHz (typical after PLL config)
     * - OVER8 = 0 (16x oversampling)
     * - Baud rate = 115200
     * 
     * BRR = 42000000 / (16 * 115200) = 22.786
     * Mantissa = 22, Fraction = 0.786 * 16 = 12.576 ≈ 13
     * BRR = (22 << 4) | 13 = 0x016D
     */
    
    printf("1. Calculating BRR value...\n");
    uint32_t apb1_clock = 42000000;  // Assuming 42 MHz
    uint32_t usartdiv = apb1_clock / (16 * baud_rate);
    uint32_t fraction = ((apb1_clock % (16 * baud_rate)) * 16) / (16 * baud_rate);
    
    uint32_t brr_value = (usartdiv << 4) | (fraction & 0xF);
    
    printf("   APB1 Clock: %lu Hz\n", (unsigned long)apb1_clock);
    printf("   Baud Rate: %lu\n", (unsigned long)baud_rate);
    printf("   Mantissa: %lu\n", (unsigned long)usartdiv);
    printf("   Fraction: %lu\n", (unsigned long)fraction);
    printf("   BRR Value: 0x%04lX\n\n", (unsigned long)brr_value);
    
    USART2->BRR = brr_value;
}

void UART_FrameConfig(UART_Config_t *config)
{
    printf("=== UART Frame Configuration ===\n");
    
    uint32_t cr1_temp = 0;
    
    /* Word length */
    printf("1. Word Length: %d bits\n", config->WordLength);
    if (config->WordLength == 9) {
        cr1_temp |= (1 << 12);  // M bit
    }
    
    /* Parity */
    printf("2. Parity: ");
    switch (config->Parity) {
        case 0:
            printf("None\n");
            break;
        case 1:
            printf("Even\n");
            cr1_temp |= (1 << 10);  // PCE bit
            break;
        case 2:
            printf("Odd\n");
            cr1_temp |= (1 << 10) | (1 << 9);  // PCE + PS
            break;
    }
    
    /* Enable TX and RX */
    printf("3. Mode: ");
    if (config->Mode & 0x1) {
        printf("TX ");
        cr1_temp |= (1 << 3);  // TE bit
    }
    if (config->Mode & 0x2) {
        printf("RX ");
        cr1_temp |= (1 << 2);  // RE bit
    }
    printf("\n");
    
    /* Apply CR1 configuration */
    USART2->CR1 = cr1_temp;
    
    /* Stop bits (CR2) */
    printf("4. Stop Bits: %d\n", config->StopBits);
    uint32_t cr2_temp = 0;
    if (config->StopBits == 2) {
        cr2_temp |= (0x2 << 12);  // STOP[1:0] = 10
    }
    USART2->CR2 = cr2_temp;
    
    printf("\n   CR1 = 0x%08lX\n", (unsigned long)USART2->CR1);
    printf("   CR2 = 0x%08lX\n\n", (unsigned long)USART2->CR2);
}

void UART_Enable(void)
{
    printf("=== Enabling UART Peripheral ===\n");
    
    /* Set UE bit in CR1 to enable USART */
    USART2->CR1 |= (1 << 13);
    
    printf("USART2 enabled (UE bit set)\n");
    printf("CR1 = 0x%08lX\n\n", (unsigned long)USART2->CR1);
}

void UART_TransmitChar(char data)
{
    /* Wait for TXE (Transmit Data Register Empty) */
    while (!(USART2->SR & (1 << 7)));
    
    /* Write data to DR */
    USART2->DR = data;
}

void UART_TransmitString(const char *str)
{
    while (*str) {
        UART_TransmitChar(*str++);
    }
}

char UART_ReceiveChar(void)
{
    /* Wait for RXNE (Read Data Register Not Empty) */
    while (!(USART2->SR & (1 << 5)));
    
    /* Read data from DR */
    return (char)(USART2->DR & 0xFF);
}

int main(void)
{
    printf("=== UART HAL Setup Example ===\n\n");
    
    /* Step 1: Enable clocks */
    UART_ClockEnable();
    
    /* Step 2: Configure GPIO pins for UART */
    UART_GPIOSetup();
    
    /* Step 3: Configure baud rate */
    UART_BaudRateConfig(115200);
    
    /* Step 4: Configure frame format */
    UART_Config_t uart_config = {
        .BaudRate = 115200,
        .WordLength = 8,
        .StopBits = 1,
        .Parity = 0,  // None
        .Mode = 0x3   // TX and RX
    };
    UART_FrameConfig(&uart_config);
    
    /* Step 5: Enable UART peripheral */
    UART_Enable();
    
    printf("=== UART Initialization Complete ===\n\n");
    
    /* Demonstrate usage */
    printf("=== UART Usage Example ===\n");
    printf("Transmitting: \"Hello UART!\\n\"\n\n");
    
    printf("Note: Actual transmission would occur here\n");
    printf("      Connect a serial terminal to USART2 (PA2/PA3)\n");
    printf("      Baud: 115200, 8N1\n\n");
    
    /* Pseudo-code for actual transmission */
    printf("Example code:\n");
    printf("  UART_TransmitString(\"Hello UART!\\n\");\n");
    printf("  char received = UART_ReceiveChar();\n\n");
    
    printf("=== UART Setup Example Complete ===\n");
    
    return 0;
}

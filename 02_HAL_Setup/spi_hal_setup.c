/*
 * spi_hal_setup.c
 *
 * Demonstrates SPI peripheral HAL setup skeleton
 * Learning objectives:
 * - SPI peripheral configuration
 * - Master/Slave mode setup
 * - Clock polarity and phase
 * - Data frame format
 */

#include "../drivers/inc/stm32f446re.h"
#include <stdio.h>

/* SPI Configuration Structure */
typedef struct {
    uint8_t DeviceMode;      // Master or Slave
    uint8_t BusConfig;       // Full-duplex, Half-duplex, Simplex
    uint8_t ClockSpeed;      // Baud rate prescaler
    uint8_t DataSize;        // 8-bit or 16-bit
    uint8_t CPOL;           // Clock polarity
    uint8_t CPHA;           // Clock phase
    uint8_t SSM;            // Software slave management
} SPI_Config_t;

/* SPI Register Structure (simplified) */
typedef struct {
    volatile uint32_t CR1;    // Control register 1
    volatile uint32_t CR2;    // Control register 2
    volatile uint32_t SR;     // Status register
    volatile uint32_t DR;     // Data register
} SPI_RegDef_t;

#define SPI1_REG ((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2_REG ((SPI_RegDef_t*)SPI2_BASEADDR)

/* SPI Mode definitions */
#define SPI_MODE_MASTER     1
#define SPI_MODE_SLAVE      0

/* Bus config */
#define SPI_BUS_FULL_DUPLEX     0
#define SPI_BUS_HALF_DUPLEX     1
#define SPI_BUS_SIMPLEX_RX      2

/* Clock speed (prescaler) */
#define SPI_SPEED_DIV2      0
#define SPI_SPEED_DIV4      1
#define SPI_SPEED_DIV8      2
#define SPI_SPEED_DIV16     3
#define SPI_SPEED_DIV32     4
#define SPI_SPEED_DIV64     5
#define SPI_SPEED_DIV128    6
#define SPI_SPEED_DIV256    7

void SPI_ClockEnable(void)
{
    printf("=== SPI Clock Configuration ===\n");
    
    /* Enable SPI1 clock (APB2 bus) */
    printf("1. Enabling SPI1 peripheral clock...\n");
    RCC->APB2ENR |= (1 << 12);  // SPI1EN bit
    printf("   SPI1 clock enabled (APB2)\n");
    printf("   APB2ENR = 0x%08lX\n\n", (unsigned long)RCC->APB2ENR);
}

void SPI_GPIOSetup(void)
{
    printf("=== SPI GPIO Configuration ===\n");
    
    /*
     * SPI1 Pins on STM32F446RE:
     * PA5 - SPI1_SCK  (AF5)
     * PA6 - SPI1_MISO (AF5)
     * PA7 - SPI1_MOSI (AF5)
     * PA4 - SPI1_NSS  (AF5) - optional, can use GPIO for CS
     */
    
    printf("1. Enabling GPIOA clock...\n");
    RCC->AHB1ENR |= (1 << 0);
    
    printf("2. Configuring PA5 (SCK), PA6 (MISO), PA7 (MOSI)...\n");
    
    /* Set PA5, PA6, PA7 to Alternate Function mode */
    GPIOA->MODER &= ~((0x3 << 10) | (0x3 << 12) | (0x3 << 14));
    GPIOA->MODER |= ((0x2 << 10) | (0x2 << 12) | (0x2 << 14));
    printf("   Mode: Alternate Function\n");
    
    /* Set speed to very high */
    GPIOA->OSPEEDR &= ~((0x3 << 10) | (0x3 << 12) | (0x3 << 14));
    GPIOA->OSPEEDR |= ((0x3 << 10) | (0x3 << 12) | (0x3 << 14));
    printf("   Speed: Very High\n");
    
    /* Set output type to push-pull */
    GPIOA->OTYPER &= ~((1 << 5) | (1 << 6) | (1 << 7));
    printf("   Output Type: Push-Pull\n");
    
    /* No pull-up/down for SPI pins */
    GPIOA->PUPDR &= ~((0x3 << 10) | (0x3 << 12) | (0x3 << 14));
    printf("   Pull-up/down: None\n");
    
    /* Configure alternate function AF5 for SPI1 */
    GPIOA->AFR[0] &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOA->AFR[0] |= ((0x5 << 20) | (0x5 << 24) | (0x5 << 28));
    printf("   Alternate Function: AF5 (SPI1)\n\n");
}

void SPI_Configure(SPI_Config_t *config)
{
    printf("=== SPI Configuration ===\n");
    
    uint32_t cr1_temp = 0;
    
    /* 1. Device mode (Master/Slave) */
    printf("1. Device Mode: ");
    if (config->DeviceMode == SPI_MODE_MASTER) {
        printf("Master\n");
        cr1_temp |= (1 << 2);  // MSTR bit
    } else {
        printf("Slave\n");
    }
    
    /* 2. Bus configuration */
    printf("2. Bus Config: ");
    switch (config->BusConfig) {
        case SPI_BUS_FULL_DUPLEX:
            printf("Full-Duplex\n");
            break;
        case SPI_BUS_HALF_DUPLEX:
            printf("Half-Duplex\n");
            cr1_temp |= (1 << 15);  // BIDIMODE
            break;
        case SPI_BUS_SIMPLEX_RX:
            printf("Simplex RX-only\n");
            cr1_temp |= (1 << 10);  // RXONLY
            break;
    }
    
    /* 3. Clock speed (baud rate prescaler) */
    printf("3. Clock Speed: DIV_%d\n", 1 << (config->ClockSpeed + 1));
    cr1_temp |= (config->ClockSpeed << 3);  // BR[2:0]
    
    /* 4. Data size */
    printf("4. Data Size: %d-bit\n", config->DataSize);
    if (config->DataSize == 16) {
        cr1_temp |= (1 << 11);  // DFF bit
    }
    
    /* 5. Clock polarity (CPOL) */
    printf("5. Clock Polarity (CPOL): %d\n", config->CPOL);
    if (config->CPOL) {
        cr1_temp |= (1 << 1);
    }
    
    /* 6. Clock phase (CPHA) */
    printf("6. Clock Phase (CPHA): %d\n", config->CPHA);
    if (config->CPHA) {
        cr1_temp |= (1 << 0);
    }
    
    /* 7. Software slave management */
    printf("7. Software Slave Management: %s\n", config->SSM ? "Enabled" : "Disabled");
    if (config->SSM) {
        cr1_temp |= (1 << 9);   // SSM
        cr1_temp |= (1 << 8);   // SSI (internal slave select)
    }
    
    /* Write configuration to CR1 */
    SPI1_REG->CR1 = cr1_temp;
    
    printf("\n   CR1 = 0x%08lX\n\n", (unsigned long)SPI1_REG->CR1);
}

void SPI_Enable(void)
{
    printf("=== Enabling SPI Peripheral ===\n");
    
    /* Set SPE bit in CR1 to enable SPI */
    SPI1_REG->CR1 |= (1 << 6);
    
    printf("SPI1 enabled (SPE bit set)\n");
    printf("CR1 = 0x%08lX\n\n", (unsigned long)SPI1_REG->CR1);
}

void SPI_TransmitByte(uint8_t data)
{
    /* Wait for TXE (Transmit buffer Empty) */
    while (!(SPI1_REG->SR & (1 << 1)));
    
    /* Write data to DR */
    SPI1_REG->DR = data;
}

uint8_t SPI_ReceiveByte(void)
{
    /* Wait for RXNE (Receive buffer Not Empty) */
    while (!(SPI1_REG->SR & (1 << 0)));
    
    /* Read data from DR */
    return (uint8_t)(SPI1_REG->DR & 0xFF);
}

uint8_t SPI_TransferByte(uint8_t data)
{
    /* Transmit byte */
    SPI_TransmitByte(data);
    
    /* Receive byte (in full-duplex, we always receive) */
    return SPI_ReceiveByte();
}

void demonstrate_spi_modes(void)
{
    printf("=== SPI Communication Modes ===\n\n");
    
    printf("1. Full-Duplex:\n");
    printf("   - Simultaneous TX and RX\n");
    printf("   - Uses MOSI, MISO, and SCK\n");
    printf("   - Most common mode\n\n");
    
    printf("2. Half-Duplex:\n");
    printf("   - Either TX or RX, not both\n");
    printf("   - Uses single data line\n");
    printf("   - Direction controlled by BIDIOE bit\n\n");
    
    printf("3. Simplex RX-only:\n");
    printf("   - Receive only, continuous clock\n");
    printf("   - Master receives from slave\n");
    printf("   - Uses MISO and SCK\n\n");
}

void demonstrate_spi_clock_modes(void)
{
    printf("=== SPI Clock Modes (CPOL/CPHA) ===\n\n");
    
    printf("Mode 0: CPOL=0, CPHA=0\n");
    printf("  - Clock idle low\n");
    printf("  - Data sampled on first edge (rising)\n\n");
    
    printf("Mode 1: CPOL=0, CPHA=1\n");
    printf("  - Clock idle low\n");
    printf("  - Data sampled on second edge (falling)\n\n");
    
    printf("Mode 2: CPOL=1, CPHA=0\n");
    printf("  - Clock idle high\n");
    printf("  - Data sampled on first edge (falling)\n\n");
    
    printf("Mode 3: CPOL=1, CPHA=1\n");
    printf("  - Clock idle high\n");
    printf("  - Data sampled on second edge (rising)\n\n");
}

int main(void)
{
    printf("=== SPI HAL Setup Example ===\n\n");
    
    /* Educational information */
    demonstrate_spi_modes();
    demonstrate_spi_clock_modes();
    
    /* Step 1: Enable clocks */
    SPI_ClockEnable();
    
    /* Step 2: Configure GPIO pins for SPI */
    SPI_GPIOSetup();
    
    /* Step 3: Configure SPI peripheral */
    SPI_Config_t spi_config = {
        .DeviceMode = SPI_MODE_MASTER,
        .BusConfig = SPI_BUS_FULL_DUPLEX,
        .ClockSpeed = SPI_SPEED_DIV8,  // APB2/8 = 84MHz/8 = 10.5 MHz
        .DataSize = 8,
        .CPOL = 0,
        .CPHA = 0,
        .SSM = 1  // Software slave management enabled
    };
    SPI_Configure(&spi_config);
    
    /* Step 4: Enable SPI peripheral */
    SPI_Enable();
    
    printf("=== SPI Initialization Complete ===\n\n");
    
    /* Demonstrate usage */
    printf("=== SPI Usage Example ===\n");
    printf("Example: Transmitting 0xA5 and receiving response\n\n");
    
    printf("Note: Actual transmission would occur here\n");
    printf("      Connect SPI device to SPI1 pins:\n");
    printf("      PA5 - SCK, PA6 - MISO, PA7 - MOSI\n\n");
    
    printf("Example code:\n");
    printf("  uint8_t tx_data = 0xA5;\n");
    printf("  SPI_TransmitByte(tx_data);\n");
    printf("  uint8_t rx_data = SPI_ReceiveByte();\n\n");
    
    printf("  // Or use transfer for full-duplex:\n");
    printf("  uint8_t response = SPI_TransferByte(0xA5);\n\n");
    
    printf("=== SPI Setup Example Complete ===\n");
    
    return 0;
}

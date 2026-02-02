/*
 * board_config.h
 *
 * Multi-Board Configuration System for STM32 Development
 * Supports automatic board selection and peripheral mapping
 *
 * Usage:
 *   Define one of the following before including this header:
 *   - STM32F0XX for STM32F0 series
 *   - STM32F1XX for STM32F1 series  
 *   - STM32F4XX for STM32F4 series (default: F446RE)
 */

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include <stdint.h>

/*********************************************************************
 * Board Selection
 *********************************************************************/

// Auto-detect if not specified
#if !defined(STM32F0XX) && !defined(STM32F1XX) && !defined(STM32F4XX)
    #define STM32F4XX  // Default to F4 series
    #warning "No STM32 series defined, defaulting to STM32F4XX"
#endif

/*********************************************************************
 * Board-Specific Includes
 *********************************************************************/

#ifdef STM32F0XX
    #include "stm32f0xx.h"
    #define BOARD_SERIES "STM32F0"
    #define MCU_FAMILY STM32F0
#elif defined(STM32F1XX)
    #include "stm32f1xx.h"
    #define BOARD_SERIES "STM32F1"
    #define MCU_FAMILY STM32F1
#elif defined(STM32F4XX)
    #include "stm32f446re.h"
    #define BOARD_SERIES "STM32F4"
    #define MCU_FAMILY STM32F4
#else
    #error "Unsupported STM32 series"
#endif

/*********************************************************************
 * Clock Configuration Defaults
 *********************************************************************/

#ifdef STM32F0XX
    #define SYSTEM_CLOCK_HZ     48000000UL  // 48 MHz for F0
    #define APB_CLOCK_HZ        48000000UL
#elif defined(STM32F1XX)
    #define SYSTEM_CLOCK_HZ     72000000UL  // 72 MHz for F1
    #define APB_CLOCK_HZ        36000000UL
#elif defined(STM32F4XX)
    #define SYSTEM_CLOCK_HZ     180000000UL // 180 MHz for F4
    #define APB1_CLOCK_HZ       45000000UL
    #define APB2_CLOCK_HZ       90000000UL
#endif

/*********************************************************************
 * GPIO Configuration
 *********************************************************************/

// Number of GPIO ports available
#ifdef STM32F0XX
    #define GPIO_PORT_COUNT     6  // A-F
#elif defined(STM32F1XX)
    #define GPIO_PORT_COUNT     7  // A-G
#elif defined(STM32F4XX)
    #define GPIO_PORT_COUNT     9  // A-I
#endif

/*********************************************************************
 * Peripheral Availability
 *********************************************************************/

typedef struct {
    uint8_t has_usart1  : 1;
    uint8_t has_usart2  : 1;
    uint8_t has_usart3  : 1;
    uint8_t has_uart4   : 1;
    uint8_t has_uart5   : 1;
    uint8_t has_usart6  : 1;
    uint8_t has_spi1    : 1;
    uint8_t has_spi2    : 1;
    uint8_t has_spi3    : 1;
    uint8_t has_i2c1    : 1;
    uint8_t has_i2c2    : 1;
    uint8_t has_i2c3    : 1;
    uint8_t has_adc1    : 1;
    uint8_t has_adc2    : 1;
    uint8_t has_adc3    : 1;
    uint8_t has_dma1    : 1;
    uint8_t has_dma2    : 1;
} BoardPeripherals_t;

// Board capabilities detection
static inline BoardPeripherals_t Board_GetPeripherals(void)
{
    BoardPeripherals_t peripherals = {0};
    
#ifdef STM32F0XX
    peripherals.has_usart1 = 1;
    peripherals.has_usart2 = 1;
    peripherals.has_spi1 = 1;
    peripherals.has_spi2 = 1;
    peripherals.has_i2c1 = 1;
    peripherals.has_i2c2 = 1;
    peripherals.has_adc1 = 1;
    peripherals.has_dma1 = 1;
    
#elif defined(STM32F1XX)
    peripherals.has_usart1 = 1;
    peripherals.has_usart2 = 1;
    peripherals.has_usart3 = 1;
    peripherals.has_spi1 = 1;
    peripherals.has_spi2 = 1;
    peripherals.has_i2c1 = 1;
    peripherals.has_i2c2 = 1;
    peripherals.has_adc1 = 1;
    peripherals.has_adc2 = 1;
    peripherals.has_dma1 = 1;
    
#elif defined(STM32F4XX)
    peripherals.has_usart1 = 1;
    peripherals.has_usart2 = 1;
    peripherals.has_usart3 = 1;
    peripherals.has_uart4 = 1;
    peripherals.has_uart5 = 1;
    peripherals.has_usart6 = 1;
    peripherals.has_spi1 = 1;
    peripherals.has_spi2 = 1;
    peripherals.has_spi3 = 1;
    peripherals.has_i2c1 = 1;
    peripherals.has_i2c2 = 1;
    peripherals.has_i2c3 = 1;
    peripherals.has_adc1 = 1;
    peripherals.has_adc2 = 1;
    peripherals.has_adc3 = 1;
    peripherals.has_dma1 = 1;
    peripherals.has_dma2 = 1;
#endif
    
    return peripherals;
}

/*********************************************************************
 * Utility Functions
 *********************************************************************/

// Get board information string
static inline const char* Board_GetSeriesName(void)
{
    return BOARD_SERIES;
}

// Get system clock frequency
static inline uint32_t Board_GetSystemClock(void)
{
    return SYSTEM_CLOCK_HZ;
}

#endif /* BOARD_CONFIG_H_ */

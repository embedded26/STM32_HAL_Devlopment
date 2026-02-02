/*
 * stm32f0xx.h
 *
 * STM32F0 Series Register Definitions
 * Compatible with STM32F0 Nucleo boards (F030, F070, F091, etc.)
 */

#ifndef INC_STM32F0XX_H_
#define INC_STM32F0XX_H_

#include <stdint.h>

#define __VO volatile

/*********************************************************************
 * Memory Base Addresses
 *********************************************************************/
#define FLASH_BASEADDR              0x08000000U
#define SRAM_BASEADDR               0x20000000U
#define ROM_BASEADDR                0x1FFFF000U

/*********************************************************************
 * Peripheral Base Addresses
 *********************************************************************/
#define PERIPH_BASEADDR             0x40000000U
#define APB_PERIPH_BASEADDR         PERIPH_BASEADDR
#define AHB1_PERIPH_BASEADDR        0x40020000U
#define AHB2_PERIPH_BASEADDR        0x48000000U

/*********************************************************************
 * GPIO Base Addresses (F0 Series)
 *********************************************************************/
#define GPIOA_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR              (AHB2_PERIPH_BASEADDR + 0x1400)

/*********************************************************************
 * RCC Base Address
 *********************************************************************/
#define RCC_BASEADDR                (AHB1_PERIPH_BASEADDR + 0x1000)

/*********************************************************************
 * Peripheral Base Addresses
 *********************************************************************/
#define USART1_BASEADDR             (APB_PERIPH_BASEADDR + 0x13800)
#define USART2_BASEADDR             (APB_PERIPH_BASEADDR + 0x4400)
#define SPI1_BASEADDR               (APB_PERIPH_BASEADDR + 0x13000)
#define SPI2_BASEADDR               (APB_PERIPH_BASEADDR + 0x3800)
#define I2C1_BASEADDR               (APB_PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR               (APB_PERIPH_BASEADDR + 0x5800)

/*********************************************************************
 * GPIO Register Definition
 *********************************************************************/
typedef struct
{
    __VO uint32_t MODER;        // Mode register
    __VO uint32_t OTYPER;       // Output type register
    __VO uint32_t OSPEEDR;      // Output speed register
    __VO uint32_t PUPDR;        // Pull-up/pull-down register
    __VO uint32_t IDR;          // Input data register
    __VO uint32_t ODR;          // Output data register
    __VO uint32_t BSRR;         // Bit set/reset register
    __VO uint32_t LCKR;         // Configuration lock register
    __VO uint32_t AFR[2];       // Alternate function registers
    __VO uint32_t BRR;          // Bit reset register
} GPIO_RegDef_t;

/*********************************************************************
 * RCC Register Definition
 *********************************************************************/
typedef struct
{
    __VO uint32_t CR;           // Clock control register
    __VO uint32_t CFGR;         // Clock configuration register
    __VO uint32_t CIR;          // Clock interrupt register
    __VO uint32_t APB2RSTR;     // APB2 peripheral reset register
    __VO uint32_t APB1RSTR;     // APB1 peripheral reset register
    __VO uint32_t AHBENR;       // AHB peripheral clock enable register
    __VO uint32_t APB2ENR;      // APB2 peripheral clock enable register
    __VO uint32_t APB1ENR;      // APB1 peripheral clock enable register
    __VO uint32_t BDCR;         // Backup domain control register
    __VO uint32_t CSR;          // Control/status register
    __VO uint32_t AHBRSTR;      // AHB peripheral reset register
    __VO uint32_t CFGR2;        // Clock configuration register 2
    __VO uint32_t CFGR3;        // Clock configuration register 3
    __VO uint32_t CR2;          // Clock control register 2
} RCC_RegDef_t;

/*********************************************************************
 * Peripheral Definitions
 *********************************************************************/
#define GPIOA                       ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB                       ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC                       ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD                       ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE                       ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF                       ((GPIO_RegDef_t*)GPIOF_BASEADDR)

#define RCC                         ((RCC_RegDef_t*)RCC_BASEADDR)

/*********************************************************************
 * Clock Enable Macros
 *********************************************************************/
#define GPIOA_PCLK_EN()             (RCC->AHBENR |= (1 << 17))
#define GPIOB_PCLK_EN()             (RCC->AHBENR |= (1 << 18))
#define GPIOC_PCLK_EN()             (RCC->AHBENR |= (1 << 19))
#define GPIOD_PCLK_EN()             (RCC->AHBENR |= (1 << 20))
#define GPIOE_PCLK_EN()             (RCC->AHBENR |= (1 << 21))
#define GPIOF_PCLK_EN()             (RCC->AHBENR |= (1 << 22))

/*********************************************************************
 * Common Macros
 *********************************************************************/
#define ENABLE                      1
#define DISABLE                     0
#define SET                         ENABLE
#define RESET                       DISABLE
#define GPIO_PIN_SET                SET
#define GPIO_PIN_RESET              RESET

#endif /* INC_STM32F0XX_H_ */

/*
 * stm32f1xx.h
 *
 * STM32F1 Series Register Definitions
 * Compatible with STM32F1 Nucleo boards (F103, etc.)
 */

#ifndef INC_STM32F1XX_H_
#define INC_STM32F1XX_H_

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
#define APB1_PERIPH_BASEADDR        PERIPH_BASEADDR
#define APB2_PERIPH_BASEADDR        0x40010000U
#define AHB_PERIPH_BASEADDR         0x40018000U

/*********************************************************************
 * GPIO Base Addresses (F1 Series)
 *********************************************************************/
#define GPIOA_BASEADDR              (APB2_PERIPH_BASEADDR + 0x0800)
#define GPIOB_BASEADDR              (APB2_PERIPH_BASEADDR + 0x0C00)
#define GPIOC_BASEADDR              (APB2_PERIPH_BASEADDR + 0x1000)
#define GPIOD_BASEADDR              (APB2_PERIPH_BASEADDR + 0x1400)
#define GPIOE_BASEADDR              (APB2_PERIPH_BASEADDR + 0x1800)
#define GPIOF_BASEADDR              (APB2_PERIPH_BASEADDR + 0x1C00)
#define GPIOG_BASEADDR              (APB2_PERIPH_BASEADDR + 0x2000)

/*********************************************************************
 * RCC Base Address
 *********************************************************************/
#define RCC_BASEADDR                (AHB_PERIPH_BASEADDR + 0x3000)

/*********************************************************************
 * Peripheral Base Addresses
 *********************************************************************/
#define USART1_BASEADDR             (APB2_PERIPH_BASEADDR + 0x3800)
#define USART2_BASEADDR             (APB1_PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR             (APB1_PERIPH_BASEADDR + 0x4800)
#define SPI1_BASEADDR               (APB2_PERIPH_BASEADDR + 0x3000)
#define SPI2_BASEADDR               (APB1_PERIPH_BASEADDR + 0x3800)
#define I2C1_BASEADDR               (APB1_PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR               (APB1_PERIPH_BASEADDR + 0x5800)

/*********************************************************************
 * GPIO Register Definition (F1 has different structure)
 *********************************************************************/
typedef struct
{
    __VO uint32_t CRL;          // Port configuration register low
    __VO uint32_t CRH;          // Port configuration register high
    __VO uint32_t IDR;          // Port input data register
    __VO uint32_t ODR;          // Port output data register
    __VO uint32_t BSRR;         // Port bit set/reset register
    __VO uint32_t BRR;          // Port bit reset register
    __VO uint32_t LCKR;         // Port configuration lock register
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
#define GPIOG                       ((GPIO_RegDef_t*)GPIOG_BASEADDR)

#define RCC                         ((RCC_RegDef_t*)RCC_BASEADDR)

/*********************************************************************
 * Clock Enable Macros
 *********************************************************************/
#define GPIOA_PCLK_EN()             (RCC->APB2ENR |= (1 << 2))
#define GPIOB_PCLK_EN()             (RCC->APB2ENR |= (1 << 3))
#define GPIOC_PCLK_EN()             (RCC->APB2ENR |= (1 << 4))
#define GPIOD_PCLK_EN()             (RCC->APB2ENR |= (1 << 5))
#define GPIOE_PCLK_EN()             (RCC->APB2ENR |= (1 << 6))
#define GPIOF_PCLK_EN()             (RCC->APB2ENR |= (1 << 7))
#define GPIOG_PCLK_EN()             (RCC->APB2ENR |= (1 << 8))

/*********************************************************************
 * Common Macros
 *********************************************************************/
#define ENABLE                      1
#define DISABLE                     0
#define SET                         ENABLE
#define RESET                       DISABLE
#define GPIO_PIN_SET                SET
#define GPIO_PIN_RESET              RESET

#endif /* INC_STM32F1XX_H_ */

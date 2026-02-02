/*
 * stm32f446re.h
 *
 *  Created on: Jun 20, 2025
 *      Author: DELL
 */

// Date 24-06-2025

#ifndef INC_STM32F446RE_H_
#define INC_STM32F446RE_H_

// MACROS FOR MEMORY AND PERIPHERALS
#include <stdint.h>
#define __VO volatile


#define SRAM1_BASEADDR                0x20000000U
#define SRAM2_BASEADDR                0x2001C000U
#define ROM_BASEADDR  				  0x1FFF0000U
#define SRAM 						  SRAM1_BASEADDR
#define PERIPH_BASEADDR               0x40000000U

#define APB1_PERIPH_BASEADDR          PERIPH_BASEADDR
#define APB2_PERIPH_BASEADDR          0x40010000U
#define AHB1_PERIPH_BASEADDR          0x40020000U
#define AHB2_PERIPH_BASEADDR          0x50000000U


#define GPIOA_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x0000 )
#define GPIOB_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x4000 )
#define GPIOC_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x8000 )
#define GPIOD_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0xC000 )
#define GPIOE_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x1000 )
#define GPIOF_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x1400 )
#define GPIOG_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x1800 )
#define GPIOH_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x1C00 )
#define GPIOI_BASEADDR  ( AHB1_PERIPH_BASEADDR + 0x2000 )
#define RCC_BASEADDR    ( AHB1_PERIPH_BASEADDR + 0x3800 )




#define I2C1_BASEADDR   ( APB1_PERIPH_BASEADDR + 0x5400 )
#define I2C2_BASEADDR   ( APB1_PERIPH_BASEADDR + 0x5800 )
#define I2C3_BASEADDR   ( APB1_PERIPH_BASEADDR + 0x5C00 )

#define SPI2_BASEADDR   ( APB1_PERIPH_BASEADDR + 0x3800 )
#define SPI3_BASEADDR   ( APB1_PERIPH_BASEADDR + 0x3C00 )

#define USART2_BASEADDR ( APB1_PERIPH_BASEADDR + 0x4400 )
#define USART3_BASEADDR ( APB1_PERIPH_BASEADDR + 0x4800 )
#define UART4_BASEADDR  ( APB1_PERIPH_BASEADDR + 0x4C00 )
#define UART5_BASEADDR  ( APB1_PERIPH_BASEADDR + 0x5000 )


#define SPI1_BASEADDR   ( APB2_PERIPH_BASEADDR + 0x3000 )
#define USART1_BASEADDR ( APB2_PERIPH_BASEADDR + 0x1000 )
#define USART6_BASEADDR ( APB2_PERIPH_BASEADDR + 0x1400 )
#define EXT1_BASEADDR   ( APB2_PERIPH_BASEADDR + 0x3C00 )
#define SYSCFC_BASEADDR ( APB2_PERIPH_BASEADDR + 0x3800 )

// #define 	   			( GPIOA_BASEADDR + 0x00 )

typedef struct
 {
	__VO uint32_t MODER;
	__VO uint32_t OTYPER;
	__VO uint32_t OSPEEDR;
	__VO uint32_t PUPDR;
	__VO uint32_t IDR;
	__VO uint32_t ODR;
	__VO uint32_t BSRRL;
	__VO uint32_t BSRRH;
	__VO uint32_t LCKR;
	__VO uint32_t AFR[2];

 } GPIO_RegDef_t;

 typedef struct
 {
	 __VO uint32_t CR;
	 __VO uint32_t PLLCFGR;
	 __VO uint32_t CFGR;
	 __VO uint32_t CIR;
	 __VO uint32_t AHB1RSTR;
	 __VO uint32_t AHB2RSTR;
	 __VO uint32_t AHB3RSTR;
	 __VO uint32_t RESERVED1;
	 __VO uint32_t APB1RSTR;
	 __VO uint32_t APB2RSTR;
	 __VO uint32_t RESERVED2;
	 __VO uint32_t RESERVED3;
	 __VO uint32_t AHB1ENR;
	 __VO uint32_t AHB2ENR;
	 __VO uint32_t AHB3ENR;
	 __VO uint32_t RESERVED4;
	 __VO uint32_t APB1ENR;
	 __VO uint32_t APB2ENR;
	 __VO uint32_t RESERVED5;
	 __VO uint32_t RESERVED6;
	 __VO uint32_t AHB1LPENR;
	 __VO uint32_t AHB2LPENR;
	 __VO uint32_t AHB3LPENR;
	 __VO uint32_t RESERVED7;
	 __VO uint32_t PB1LPENR;
	 __VO uint32_t PB2LPENR;
	 __VO uint32_t RESERVED8;
	 __VO uint32_t RESERVED9;
	 __VO uint32_t BDCR;
	 __VO uint32_t CSR;
	 __VO uint32_t RESERVED10;
	 __VO uint32_t RESERVED11;
	 __VO uint32_t SSCGR;
	 __VO uint32_t PLLI2SCFGR;
	 __VO uint32_t PLLSAICFGR;
	 __VO uint32_t DCKCFGR;
	 __VO uint32_t CKGATENR;
	 __VO uint32_t DCKCFGR2;

 } RCC__RegDef_t;



 // GPIO_RegDef_t *pGPIOA = GPIOA;

#define GPIOA ((GPIO_RegDef_t*)GPIOA_BASEADDR )
#define GPIOB ((GPIO_RegDef_t*)GPIOB_BASEADDR )
#define GPIOC ((GPIO_RegDef_t*)GPIOC_BASEADDR )
#define GPIOD ((GPIO_RegDef_t*)GPIOD_BASEADDR )
#define GPIOE ((GPIO_RegDef_t*)GPIOE_BASEADDR )
#define GPIOF ((GPIO_RegDef_t*)GPIOF_BASEADDR )
#define GPIOG ((GPIO_RegDef_t*)GPIOG_BASEADDR )
#define GPIOH ((GPIO_RegDef_t*)GPIOH_BASEADDR )
#define GPIOI ((GPIO_RegDef_t*)GPIOI_BASEADDR )


#define RCC ((RCC__RegDef_t*)RCC_BASEADDR )


#define GPIOA_PCLK_EN()  (RCC->AHB1ENR |= (1 << 0) )
#define GPIOB_PCLK_EN()  (RCC->AHB1ENR |= (1 << 1) )
#define GPIOC_PCLK_EN()  (RCC->AHB1ENR |= (1 << 2) )
#define GPIOD_PCLK_EN()  (RCC->AHB1ENR |= (1 << 3) )
#define GPIOE_PCLK_EN()  (RCC->AHB1ENR |= (1 << 4) )
#define GPIOF_PCLK_EN()  (RCC->AHB1ENR |= (1 << 5) )
#define GPIOG_PCLK_EN()  (RCC->AHB1ENR |= (1 << 6) )
#define GPIOH_PCLK_EN()  (RCC->AHB1ENR |= (1 << 7) )
#define GPIOI_PCLK_EN()  (RCC->AHB1ENR |= (1 << 8) )


 // SET PERIPHERALS

#define I2C1_PCLK_EN ()  (RCC->AHB1ENR |= (1 << 21) )
#define I2C2_PCLK_EN ()  (RCC->AHB1ENR |= (1 << 22) )
#define I2C3_PCLK_EN ()  (RCC->AHB1ENR |= (1 << 23) )


#define SPI1_PCLK_EN ()  (RCC->AHB2ENR   |= (1 << 12) )
#define SPI2_PCLK_EN ()  (RCC->APB1LPENR |= (1 << 14) )
#define SPI3_PCLK_EN ()  (RCC->APB1LPENR |= (1 << 15) )

#define USART1_PCLK_EN() (RCC->APB2LPENR |= (1 << 4 ) )
#define USART2_PCLK_EN() (RCC->APB1LPENR |= (1 << 17) )
#define USART3_PCLK_EN() (RCC->APB1LPENR |= (1 << 18) )
#define UART4_PCLK_EN()  (RCC->APB1LPENR |= (1 << 19) )
#define UART5_PCLK_EN()  (RCC->APB1LPENR |= (1 << 20) )
#define USART6_PCLK_EN() (RCC->APB2LPENR |= (1 << 5 ) )


#define SYSCFG_PCLK_EN() (RCC->APB2LPENR |= (1 << 14) )



// CLEAR PERIPHERALS


//#define GPIOA_PCLK_DI()  (RCC->AHB1ENR &= (1 << 0) )
//#define GPIOB_PCLK_DI()  (RCC->AHB1ENR &= (1 << 1) )
//#define GPIOC_PCLK_DI()  (RCC->AHB1ENR &= (1 << 2) )
//#define GPIOD_PCLK_DI()  (RCC->AHB1ENR &= (1 << 3) )
//#define GPIOE_PCLK_DI()  (RCC->AHB1ENR &= (1 << 4) )
//#define GPIOF_PCLK_DI()  (RCC->AHB1ENR &= (1 << 5) )
//#define GPIOG_PCLK_DI()  (RCC->AHB1ENR &= (1 << 6) )
//#define GPIOH_PCLK_DI()  (RCC->AHB1ENR &= (1 << 7) )
//#define GPIOI_PCLK_DI()  (RCC->AHB1ENR &= (1 << 8) )




#define I2C1_PCLK_DE ()  	(RCC->AHB1ENR &= (1 << 21) )
#define I2C2_PCLK_DE ()  	(RCC->AHB1ENR &= (1 << 22) )
#define I2C3_PCLK_DE ()  	(RCC->AHB1ENR &= (1 << 23) )


#define SPI1_PCLK_DE ()  	(RCC->AHB2ENR   &= (1 << 12) )
#define SPI2_PCLK_DE ()  	(RCC->APB1LPENR &= (1 << 14) )
#define SPI3_PCLK_DE ()  	(RCC->APB1LPENR &= (1 << 15) )

#define USART1_PCLK_DE() 	(RCC->APB2LPENR &= (1 << 4 ) )
#define USART2_PCLK_DE() 	(RCC->APB1LPENR &= (1 << 17) )
#define USART3_PCLK_DE() 	(RCC->APB1LPENR &= (1 << 18) )
#define UART4_PCLK_DE () 	(RCC->APB1LPENR &= (1 << 19) )
#define UART5_PCLK_DE () 	(RCC->APB1LPENR &= (1 << 20) )
#define USART6_PCLK_DE() 	(RCC->APB2LPENR &= (1 << 5 ) )


#define SYSCFG_PCLK_DE() 	(RCC->APB2LPENR &= (1 << 14) )

#define GPIOI_PCLK_DI()
#define GPIOA_REG_RESET()	do { (RCC->AHB1RSTR   |= (1 << 0) );(RCC->AHB1RSTR   &= ~(1 << 0) );}while(0)
#define GPIOB_REG_RESET()	do { (RCC->AHB1RSTR   |= (1 << 1) );(RCC->AHB1RSTR   &= ~(1 << 1) );}while(0)
#define GPIOC_REG_RESET()	do { (RCC->AHB1RSTR   |= (1 << 2) );(RCC->AHB1RSTR   &= ~(1 << 2) );}while(0)


#define ENABLE 			1
#define DISABLE 		0
#define SET 			ENABLE
#define RESET 			DISABLE
#define GPIO_PIN_SET 	SET
#define GPIO_PIN_RESET	RESET




#endif /* INC_STM32F446RE_H_ */

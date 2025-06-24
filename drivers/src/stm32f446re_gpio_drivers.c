/*
 * stm32f446re_gpio_drivers.c
 *
 *  Created on: Jun 23, 2025
 *      Author: DELL
 */

#include "stm32f446re_gpio_drivers.h"

// peripheral clock

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOC_PCLK_DI();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_DI();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_DI();
		}
	}
}

//Init and De-int

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0;
	// MODE , SPEED , PULL UP PULL DOWN SETTING , OUTPUT TYPE ,ALTERNATE FUNCTIONALITY

	if(pGPIOHandle->GPIO_PinConfig.GPIO_Pinmode <= GPIO_MODE_ANALOG)
	{
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_Pinmode << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
		pGPIOHandle->pGPIOx->MODER |= temp ;
		temp = 0;
	}
	else
	{
		// interrupt mode
	}

	temp= 0;

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->OSPEEDR |= temp ;

	temp = 0;

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 *pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->PUPDR |= temp ;

	temp = 0;

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );
	pGPIOHandle->pGPIOx->OTYPER |= temp ;

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType == GPIO_MODE_ALTFN)
	{




		// CONFIGURER THE ALT FUNCTION REGISTERS
	}




}
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{



}
// Data read ad write

uint8_t  GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber)
{




}
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{




}
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber ,uint8_t Value)
{



}
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx ,uint16_t Value)
{




}
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx ,uint8_t Value)
{



}

void GPIO_IRQConfig(uint8_t IRQNumber ,uint8_t IRQPriority ,uint8_t EnorDi )
{



}
void GPIO_IRQHandling(uint8_t PinNumber)
{




}


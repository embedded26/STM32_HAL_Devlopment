/*
 * stm32f446re_gpio_drivers.h
 *
 *  Created on: Jun 23, 2025
 *      Author: DELL
 */

#ifndef INC_STM32F446RE_GPIO_DRIVERS_H_
#define INC_STM32F446RE_GPIO_DRIVERS_H_


#include "stm32f446re.h"

typedef struct
{
uint8_t GPIO_PinNumber ;
uint8_t GPIO_PinMode ;
uint8_t GPIO_PinSpeed ;
uint8_t GPIO_PinPupdCpntrol ;
uint8_t GPIO_PinOPType ;
uint8_t GPIO_PinAltFunMode ;

} GPIO_pinconfig_t;

typedef struct
{

GPIO_RegDef_t *pGPIOx ;
GPIO_pinconfig_t GPIO_PINConfig;

} GPIO_Handle_t;

#define GPIO_MODE_IN     	0
#define GPIO_MODE_OUT  		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG 	3
#define GPIO_MODE_IT_FT 	4
#define GPIO_MODE_IT_RT 	5
#define GPIO_MODE_IT_RFT 	6

#define GPIO_OP_TYPE_PP		0
#define GPIO_OP_TYPE_OD		1



#define GPIO_SPEED_LOW 		O
#define GPIO_SPEED_MEDIUM	1
#define GPIO_SPEED_FAST		2
#define GPIO_SPEED_HIGH		3


#define GPIO_NO_PUPD		0
#define GPIO_PIN_PU			1
#define GPIO_PIN_PD			2

#define GPIO_PIN_NO_0		0
#define GPIO_PIN_NO_1		1
#define GPIO_PIN_NO_2		2
#define GPIO_PIN_NO_3		3
#define GPIO_PIN_NO_4		4
#define GPIO_PIN_NO_5		5
#define GPIO_PIN_NO_6		6
#define GPIO_PIN_NO_7		7
#define GPIO_PIN_NO_8		8
#define GPIO_PIN_NO_9		9
#define GPIO_PIN_NO_10		10
#define GPIO_PIN_NO_11		11
#define GPIO_PIN_NO_12		12
#define GPIO_PIN_NO_13		13
#define GPIO_PIN_NO_14		14
#define GPIO_PIN_NO_15		15



// Peripheral Clock Set-up

void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{

    if (EnorDi == ENABLE)
    {
        if (pGPIOx == GPIOA) RCC->AHB1ENR |= (1 << 0);
        else if (pGPIOx == GPIOB) RCC->AHB1ENR |= (1 << 1);
        else if (pGPIOx == GPIOC) RCC->AHB1ENR |= (1 << 2);
        // ... add others
    }
    else
    {
        // Disable Clock (optional)
    }



}

//Init and De-int

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{





}
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);
// Data read ad write

uint8_t  GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx , uint8_t PinNumber ,uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx ,uint16_t Value);
//void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
    pGPIOx->ODR ^= (1 << PinNumber);  //  Now PinNumber is declared
}

void GPIO_IRQConfig(uint8_t IRQNumber ,uint8_t IRQPriority ,uint8_t EnorDi );
void GPIO_IRQHandling(uint8_t PinNumber);





#endif /* INC_STM32F446RE_GPIO_DRIVERS_H_ */

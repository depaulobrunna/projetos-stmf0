#include "stm32f0xx.h"                  // Device header


int main()
{
#if 0
	RCC->AHBENR |= _VAL2FLD(RCC_AHBENR_GPIOCEN, 1);
	GPIOC->MODER |= _VAL2FLD(GPIO_MODER_MODER8, 1);
	GPIOC->ODR &= ~GPIO_ODR_8;
#endif
	
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER10_0|GPIO_MODER_MODER11_0;
	GPIOB->ODR &= ~(GPIO_ODR_10);
	
	while(1)
	{
		//GPIOB->ODR &= ~(GPIO_ODR_10);
	
	}
}

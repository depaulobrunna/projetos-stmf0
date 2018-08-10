#include "stm32f0xx.h"

#define SIZE 1024

ADC_TypeDef *adc = ADC1;
TIM_TypeDef *tim = TIM1;
DMA_Channel_TypeDef *dma = DMA1_Channel1;

volatile uint16_t vector[SIZE];

void DMA1_Channel1_IRQHandler(void);
void ADC1_COMP_IRQHandler(void);

static void dmaInit(void);
static void adcInit(void);
static void timInit(void);
static inline void startProcess(void);
static inline void stopProcess(void);

int main(void)
{
	timInit();
	adcInit();
	dmaInit();
    
	startProcess();
	while(1);
}

static void dmaInit(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	dma->CCR &= ~DMA_CCR_EN;
	dma->CPAR = (uint32_t)((uint32_t *)&adc->DR);
	dma->CMAR = (uint32_t)((uint32_t *)&vector[0]);
	dma->CNDTR = SIZE;
	dma->CCR |= DMA_CCR_MINC|DMA_CCR_TCIE|
							DMA_CCR_MSIZE_0|DMA_CCR_PSIZE_0;
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

static void adcInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	adc->CR |= ADC_CR_ADEN;
	adc->CFGR1 |= ADC_CFGR1_DISCEN|     // ADC IN DISCONTINUOUS MODE
								ADC_CFGR1_RES_0|      // 10-bit ADC
								ADC_CFGR1_EXTEN_0|    // RISING EDGE EXTERNAL TRIGGER
								ADC_CFGR1_EXTSEL_0|   // TIM1_CC4 AS EXTERNAL TRIGGR
								ADC_CFGR1_DMAEN;      // ADC WITH DMA OPERATION
	adc->SMPR = ADC_SMPR1_SMPR;
	adc->CHSELR = ADC_CHSELR_CHSEL0;
	
	adc->IER |= ADC_IER_EOCIE|ADC_IER_EOSEQIE;
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	ADC->CCR |= ADC_CCR_VREFEN;

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER0;
}

static void timInit(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	tim->CR1 |= TIM_CR1_ARPE;
	tim->EGR |= TIM_EGR_CC4G;
	tim->PSC = 1000 - 1;
	tim->ARR = 20 - 1;
	tim->CCR4 = 10 - 1;
	tim->CCMR2 |= (TIM_CCMR2_OC4CE|
								 TIM_CCMR2_OC4PE|
								 TIM_CCMR2_OC4M_2|    // TIM1 CH4 CAPTURE/COMPARE PWM MODE
								 TIM_CCMR2_OC4M_1);   // CH4 IS UP IF TIM1CNT < TIM1CCR4
	tim->CCER = TIM_CCER_CC4E;         // CH4 ENABLED AS PWM OUTPUT FROM TIM1
	tim->BDTR |= TIM_BDTR_MOE;

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER11_1;
	GPIOA->AFR[1] = 2 << 12;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11;
}

static inline void startProcess(void)
{
	dma->CCR |= DMA_CCR_EN;
	adc->CR |= ADC_CR_ADSTART;
	tim->CR1 |= TIM_CR1_CEN;
}

static inline void stopProcess(void)
{
	dma->CCR &= ~DMA_CCR_EN;
	adc->CR |= ADC_CR_ADDIS;
	tim->CR1 &= ~TIM_CR1_CEN;
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF1)
	{
		DMA1->IFCR |= DMA_IFCR_CTCIF1;
		stopProcess();
	}
}

void ADC1_COMP_IRQHandler(void)
{
	if(adc->ISR & ADC_ISR_EOC)
	{ // NÃO ENTRA

	}
	if(adc->ISR & ADC_ISR_EOS)
	{ // ENTRA
		adc->ISR |= ADC_ISR_EOS;
	}
}

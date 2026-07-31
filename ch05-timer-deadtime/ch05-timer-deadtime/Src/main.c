#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    (void)RCC->AHB1ENR;

    GPIOA->MODER &= ~(3U << (9*2));
    GPIOA->MODER |=  (2U << (9*2));
    GPIOA->AFR[1] &= ~(0xFU << ((9-8)*4));
    GPIOA->AFR[1] |=  (0x1U << ((9-8)*4));

    GPIOB->MODER &= ~(3U << (0*2));
    GPIOB->MODER |=  (2U << (0*2));
    GPIOB->AFR[0] &= ~(0xFU << (0*4));
    GPIOB->AFR[0] |=  (0x1U << (0*4));

    TIM1->PSC = 0;
    TIM1->ARR = 1600 - 1;
    TIM1->CCR2 = 800;

    TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM1->CCMR1 |= (6U << TIM_CCMR1_OC2M_Pos);
    TIM1->CCMR1 |= TIM_CCMR1_OC2PE;

    TIM1->CCER = 0;
    TIM1->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2NE;

    /* ★ 데드타임 설정 — DTG=0x20(32) → 예상 32×62.5ns = 2000ns */
    TIM1->BDTR = 0;
    TIM1->BDTR |= (0x20U << TIM_BDTR_DTG_Pos);
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_CEN;

    for (;;) { }
}

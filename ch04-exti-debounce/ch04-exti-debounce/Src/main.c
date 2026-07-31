#include "stm32f4xx.h"

volatile uint32_t edge_count = 0;
volatile uint32_t edge_count_debounced = 0;
volatile uint32_t systick_ms = 0;

#define DEBOUNCE_MS 20

void SysTick_Handler(void)
{
    systick_ms++;
}

void EXTI15_10_IRQHandler(void)
{
    static uint32_t last_tick = 0;

    if (EXTI->PR & (1U << 13)) {
        EXTI->PR = (1U << 13);
        edge_count++;   /* 디바운싱 없는 원시 카운트 */

        if ((systick_ms - last_tick) > DEBOUNCE_MS) {
            edge_count_debounced++;   /* 디바운싱 적용된 카운트 */
            GPIOA->ODR ^= (1U << 5);  /* LED는 디바운싱된 신호로만 토글 */
        }
        last_tick = systick_ms;
    }
}

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;

    /* PA5 출력 (LD2) */
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    /* PC13 내부 풀업 */
    GPIOC->PUPDR &= ~(3U << (13 * 2));
    GPIOC->PUPDR |=  (1U << (13 * 2));

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    (void)RCC->APB2ENR;

    SYSCFG->EXTICR[3] &= ~(0xFU << 4);
    SYSCFG->EXTICR[3] |=  (0x2U << 4);

    EXTI->IMR  |= (1U << 13);
    EXTI->FTSR |= (1U << 13);

    NVIC_SetPriority(EXTI15_10_IRQn, 5);
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    /* SysTick 1ms 인터벌 — HSI 16MHz 기준 */
    SysTick_Config(16000000 / 1000);

    for (;;) { }
}

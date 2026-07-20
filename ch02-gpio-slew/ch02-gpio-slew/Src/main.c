#include "stm32f4xx.h"
#include "my_gpio.h"
#include "dwt.h"

#define N 10000

volatile uint32_t cycles_register;
volatile uint32_t cycles_driver;

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;

    my_gpio_init_output(GPIOC, 8, GPIO_SPEED_LOW);
    dwt_init();

    /* 방식 1: 레지스터 직접 (BSRR) */
    uint32_t start = DWT->CYCCNT;
    for (int i = 0; i < N; i++) {
        GPIOC->BSRR = (1U << 8);
        GPIOC->BSRR = (1U << (8 + 16));
    }
    cycles_register = DWT->CYCCNT - start;

    /* 방식 2: 자체 드라이버 함수 */
    start = DWT->CYCCNT;
    for (int i = 0; i < N; i++) {
        my_gpio_set(GPIOC, 8);
        my_gpio_clear(GPIOC, 8);
    }
    cycles_driver = DWT->CYCCNT - start;

    for (;;) { }   /* 여기서 멈춰서 디버거로 값 확인 */
}

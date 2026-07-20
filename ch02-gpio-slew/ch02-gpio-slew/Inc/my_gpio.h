#ifndef MY_GPIO_H
#define MY_GPIO_H

#include "stm32f4xx.h"

typedef enum {
    GPIO_SPEED_LOW    = 0,
    GPIO_SPEED_MEDIUM = 1,
    GPIO_SPEED_FAST   = 2,
    GPIO_SPEED_HIGH   = 3
} gpio_speed_t;

/* 초기화 함수는 그대로 .c에 둔다 — 자주 안 불리는 함수라 인라인 필요없음 */
void my_gpio_init_output(GPIO_TypeDef *port, uint8_t pin, gpio_speed_t speed);

/* set/clear/toggle은 헤더에 static inline으로 직접 구현 */
static inline void my_gpio_set(GPIO_TypeDef *port, uint8_t pin)
{
    port->BSRR = (1U << pin);
}

static inline void my_gpio_clear(GPIO_TypeDef *port, uint8_t pin)
{
    port->BSRR = (1U << (pin + 16));
}

static inline void my_gpio_toggle(GPIO_TypeDef *port, uint8_t pin)
{
    if (port->ODR & (1U << pin)) {
        my_gpio_clear(port, pin);
    } else {
        my_gpio_set(port, pin);
    }
}

#endif

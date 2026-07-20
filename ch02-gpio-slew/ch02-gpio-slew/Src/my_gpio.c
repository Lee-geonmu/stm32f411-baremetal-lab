#include "my_gpio.h"

void my_gpio_init_output(GPIO_TypeDef *port, uint8_t pin, gpio_speed_t speed)
{
    port->MODER &= ~(3U << (pin * 2));
    port->MODER |=  (1U << (pin * 2));

    port->OSPEEDR &= ~(3U << (pin * 2));
    port->OSPEEDR |=  ((uint32_t)speed << (pin * 2));
}

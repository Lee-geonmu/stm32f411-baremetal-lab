#include "stm32f4xx.h"

#define ADC_CHANNEL_NTC   0
#define ADC_CHANNEL_LM35  1
#define R_FIXED           9860.0f   /* 실측 고정저항값 (Ω) */

static void adc_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    GPIOA->MODER |= (3U << (0*2)) | (3U << (1*2));

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    (void)RCC->APB2ENR;

    ADC1->CR2 = 0;
    ADC1->SQR1 = 0;

    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (7U << (0 * 3));
    ADC1->SMPR2 |= (7U << (1 * 3));

    ADC1->CR2 |= ADC_CR2_ADON;

    for (volatile int i = 0; i < 10000; i++) { }
}

static uint16_t adc_read(uint8_t channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) { }
    return (uint16_t)(ADC1->DR & 0xFFF);
}

volatile uint16_t adc_ntc_raw;
volatile uint16_t adc_lm35_raw;

volatile float ntc_voltage;
volatile float lm35_voltage;
volatile float lm35_temp_c;
volatile float ntc_resistance;   /* 역산된 NTC 저항값 (Ω) */

int main(void)
{
    SCB->CPACR |= ((3U << 20) | (3U << 22));
    __asm volatile ("dsb");
    __asm volatile ("isb");

    adc_init();

    for (;;)
    {
        adc_ntc_raw  = adc_read(ADC_CHANNEL_NTC);
        adc_lm35_raw = adc_read(ADC_CHANNEL_LM35);

        ntc_voltage  = ((float)adc_ntc_raw  / 4095.0f) * 3.3f;
        lm35_voltage = ((float)adc_lm35_raw / 4095.0f) * 3.3f;
        lm35_temp_c  = lm35_voltage * 100.0f;

        /* NTC 저항 역산:
           R_ntc = (VCC - V_adc) * R_fixed / V_adc */
        if (ntc_voltage > 0.001f) {
            ntc_resistance = ((3.3f - ntc_voltage) * R_FIXED) / ntc_voltage;
        }

        for (volatile int i = 0; i < 1000000; i++) { }
    }
}

#include "stm32f4xx.h"

static void clock_init_100mhz(void)
{
    /* 1) PWR 클럭 인에이블 + VOS Scale 1 */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;
    PWR->CR |= PWR_CR_VOS;

    /* 2) Flash 대기상태 — PLL 전환 "전에" 반드시 먼저 */
    FLASH->ACR = FLASH_ACR_LATENCY_3WS
               | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_3WS) { }

    /* 3) HSI 확인 */
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) { }

    /* 4) PLL 정지 후 재설정 */
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY) { }

    /* HSI 16MHz / M=8 = 2MHz(VCO입력) * N=100 = 200MHz(VCO출력) / P=2 = 100MHz */
    RCC->PLLCFGR = (8U   << RCC_PLLCFGR_PLLM_Pos)
                 | (100U << RCC_PLLCFGR_PLLN_Pos)
                 | (0U   << RCC_PLLCFGR_PLLP_Pos)      /* 00 = /2 */
                 | (0U   << RCC_PLLCFGR_PLLSRC_Pos)    /* 0  = HSI */
                 | (4U   << RCC_PLLCFGR_PLLQ_Pos);

    /* 5) PLL 기동 */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) { }

    /* 6) 버스 프리스케일러 — SW 전환 "전에" */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1
              |  RCC_CFGR_PPRE1_DIV2    /* APB1 최대 50MHz */
              |  RCC_CFGR_PPRE2_DIV1;

    /* 7) SYSCLK 소스를 PLL로 전환 */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }
}

static void mco2_output_init(void)
{
    /* PC9를 MCO2로 출력해서 스코프로 SYSCLK 실측 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    (void)RCC->AHB1ENR;

    GPIOC->MODER &= ~(3U << (9 * 2));
    GPIOC->MODER |=  (2U << (9 * 2));      /* 10 = AF 모드 */

    /* 슬루레이트를 Medium으로 낮춤 — VeryHigh는 엣지가 너무 날카로워
       그라운드 리드 인덕턴스와 만나면 링잉이 커져서 자동측정이 오작동한다 */
    GPIOC->OSPEEDR &= ~(3U << (9 * 2));
    GPIOC->OSPEEDR |=  (1U << (9 * 2));    /* 01 = Medium Speed */

    GPIOC->AFR[1] &= ~(0xFU << ((9 - 8) * 4));
    GPIOC->AFR[1] |=  (0x0U << ((9 - 8) * 4)); /* AF0 = MCO2 */

    RCC->CFGR &= ~(RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE);
    RCC->CFGR |= (0U << RCC_CFGR_MCO2_Pos)      /* 00 = SYSCLK */
              |  (7U << RCC_CFGR_MCO2PRE_Pos);  /* 111 = /5 */
}

int main(void)
{
    clock_init_100mhz();
    mco2_output_init();

    /* LED로 클럭 설정이 살아있는지 육안 확인용 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));
    GPIOA->BSRR = (1U << 5);

    for (;;) { }
}

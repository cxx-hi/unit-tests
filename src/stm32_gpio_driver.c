#include "gpio_interface.h"
#include "stm32f1xx_hal.h"

static void stm32_init_output(uint8_t pin)
{
    // 实际STM32的GPIO初始化代码
    GPIO_InitTypeDef init = {0};
    init.Pin = (1 << pin);
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &init);
}

static void stm32_set_high(uint8_t pin)
{
    HAL_GPIO_WritePin(GPIOA, (1 << pin), GPIO_PIN_SET);
}

static void stm32_set_low(uint8_t pin)
{
    HAL_GPIO_WritePin(GPIOA, (1 << pin), GPIO_PIN_RESET);
}

static bool stm32_read(uint8_t pin)
{
    return HAL_GPIO_ReadPin(GPIOA, (1 << pin)) == GPIO_PIN_SET;
}

// 真实的STM32驱动
static const gpio_ops_t stm32_driver = {
    .init_output = stm32_init_output,
    .set_high = stm32_set_high,
    .set_low = stm32_set_low,
    .read = stm32_read
};

// 在系统初始化时调用
void stm32_gpio_init(void)
{
    gpio_set_driver(&stm32_driver);
}
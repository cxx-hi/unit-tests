#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

// 抽象的GPIO操作接口
typedef struct {
    void (*init_output)(uint8_t pin); //初始化引脚为输出模式
    void (*set_high)(uint8_t pin); //设置高电平
    void (*set_low)(uint8_t pin); //设置低电平
    bool (*read)(uint8_t pin);  //读取引脚电平
} gpio_ops_t;

// 设置要使用的GPIO驱动（依赖注入）
void gpio_set_driver(const gpio_ops_t *driver);

// 应用程序使用的API
void gpio_init_output(uint8_t pin);
void gpio_write(uint8_t pin, bool value);

#endif
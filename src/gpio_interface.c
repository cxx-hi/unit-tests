#include "gpio_interface.h"

static const gpio_ops_t *current_driver = NULL;

void gpio_set_driver(const gpio_ops_t *driver)
{
    current_driver = driver;  //注入具体的驱动
}

void gpio_init_output(uint8_t pin)
{
    if (current_driver && current_driver->init_output) {
        current_driver->init_output(pin);
    }
}

void gpio_write(uint8_t pin, bool value)
{
    if (!current_driver) return;
    
    if (value) {
        if (current_driver->set_high) {
            current_driver->set_high(pin);  //调用实际驱动的函数
        }
    } else {
        if (current_driver->set_low) {
            current_driver->set_low(pin);
        }
    }
}
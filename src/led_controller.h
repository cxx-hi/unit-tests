
#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdbool.h>

// LED状态
typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINKING
} led_state_t;

// LED控制器初始化
void led_controller_init(void);

// 设置LED状态
void led_set_state(led_state_t state);

// 获取当前LED状态
led_state_t led_get_state(void);

// 定时器中断调用（每100ms）
void led_timer_tick(void);

#endif

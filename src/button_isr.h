
#ifndef BUTTON_ISR_H
#define BUTTON_ISR_H

#include <stdint.h>
#include <stdbool.h>

// 按键事件类型
typedef enum {
    BUTTON_EVENT_NONE, //无事件
    BUTTON_EVENT_PRESSED, //按下
    BUTTON_EVENT_RELEASED, //释放
    BUTTON_EVENT_LONG_PRESS //长按
} button_event_t;

// 初始化
void button_isr_init(void);

// 处理中断（这个函数会在ISR中被调用）
void button_isr_handler(void);

// 获取最新的事件（主循环中调用）
button_event_t button_get_event(void);

// 获取最后按下时长（毫秒）
uint32_t button_get_press_duration(void);

#endif
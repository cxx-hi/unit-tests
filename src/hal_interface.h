// hal_interface.h - 纯函数声明，没有结构体
#ifndef HAL_INTERFACE_H
#define HAL_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * 这个文件只声明函数，不定义任何结构体
 * CMock会为每个函数自动生成Mock
 */

// LED操作
void led_on(uint8_t led_index);
void led_off(uint8_t led_index);

// 音效操作
void sound_tone(uint16_t frequency, uint32_t duration_ms);
void sound_noTone(void);

// 时间操作
uint32_t timer_millis(void);
void timer_delay(uint32_t ms);

// 显示操作
void display_number(uint8_t number);
void display_dash(void);

// 随机数
uint32_t random_value(uint32_t max);

// 调试输出
void debug_print(const char* str);
void debug_print_number(uint32_t num);

#endif
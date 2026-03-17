#ifndef HAL_WRAPPER_H
#define HAL_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>

// 硬件操作接口
bool hal_get_button_status(void); //读取按键状态（真实硬件可能是GPIO引脚）
void hal_clear_interrupt_flag(void); //清除中断标志（真实硬件可能是写寄存器）

// 日志接口（可能是输出到串口、存储等）
void log_button_event(uint32_t timestamp); //记录事件

// 获取系统时间（可以是滴答计数器）
uint32_t hal_get_tick(void);

#endif
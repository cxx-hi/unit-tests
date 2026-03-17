
#include "button_isr.h"
#include "hal_wrapper.h"

#define LONG_PRESS_THRESHOLD 3000

static button_event_t pending_event = BUTTON_EVENT_NONE; //待处理事件
static uint32_t press_start_time = 0; //按下开始时间
static bool is_pressed = false; //当前是否处于按下状态

void button_isr_init(void)
{
    pending_event = BUTTON_EVENT_NONE;
    press_start_time = 0;
    is_pressed = false;
}

void button_isr_handler(void)
{
    //读取硬件状态（通过抽象接口）
    bool current_reading = hal_get_button_status();
    uint32_t current_tick = hal_get_tick();
    
    //状态机逻辑
    if (current_reading && !is_pressed) {
        // 按下
        is_pressed = true;
        press_start_time = current_tick;
        pending_event = BUTTON_EVENT_PRESSED;
    } else if (!current_reading && is_pressed) {
        // 释放
        is_pressed = false;
        uint32_t press_duration = current_tick - press_start_time;
        
        //判断是长按还是短按
        if (press_duration >= LONG_PRESS_THRESHOLD) {
            pending_event = BUTTON_EVENT_LONG_PRESS;
        } else {
            pending_event = BUTTON_EVENT_RELEASED;
        }
    }
    //其他情况：状态未变化，什么都不做
    
    //记录事件（调试用）
    log_button_event(current_tick);

    //清除硬件中断标志
    hal_clear_interrupt_flag();
}

//事件获取
button_event_t button_get_event(void)
{
    button_event_t event = pending_event;
    pending_event = BUTTON_EVENT_NONE; //读取后清除
    return event;
}

//获取按下时长
uint32_t button_get_press_duration(void)
{
    if (!is_pressed) {
        return 0;  //没按下返回0
    }
    return hal_get_tick() - press_start_time;
}

#include "unity.h"
#include "button_isr.h"
#include "mock_hal_wrapper.h"

void setUp(void)
{
    button_isr_init(); //每个测试前重置状态
}

void tearDown(void)
{
    //每个测试后清理（CMock验证）
}

// 测试1：第一次按下
void test_first_press_should_generate_pressed_event(void)
{
    // 需要连续3次相同状态
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(true);
        hal_get_tick_ExpectAndReturn(1000 + i);
        log_button_event_Expect(1000 + i);
        hal_clear_interrupt_flag_Expect();
        //执行被测试函数
        button_isr_handler();
    }
    
    //验证结果
    button_event_t event = button_get_event();
    TEST_ASSERT_EQUAL_INT(BUTTON_EVENT_PRESSED, event);
}

// 测试2：短按释放
void test_short_release_should_generate_released_event(void)
{
    // 按下（3次）
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(true);
        hal_get_tick_ExpectAndReturn(1000 + i);
        log_button_event_Expect(1000 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    button_get_event();  // 清除PRESSED
    
    // 释放（3次，模拟500ms后释放）
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(false);
        hal_get_tick_ExpectAndReturn(1500 + i);
        log_button_event_Expect(1500 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    
    button_event_t event = button_get_event();
    TEST_ASSERT_EQUAL_INT(BUTTON_EVENT_RELEASED, event);
}

// 测试3：长按释放
void test_long_press_should_generate_long_press_event(void)
{
    // 按下（3次）
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(true);
        hal_get_tick_ExpectAndReturn(1000 + i);
        log_button_event_Expect(1000 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    button_get_event();  // 清除PRESSED
    
    // 释放（3次），时间超过3000ms，4000ms后释放
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(false);
        hal_get_tick_ExpectAndReturn(5000 + i);
        log_button_event_Expect(5000 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    
    button_event_t event = button_get_event();
    TEST_ASSERT_EQUAL_INT(BUTTON_EVENT_LONG_PRESS, event);
}

// 测试4：获取按下时长
void test_get_press_duration_should_return_correct_time(void)
{
    // 按下（3次）
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(true);
        hal_get_tick_ExpectAndReturn(1000 + i);
        log_button_event_Expect(1000 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    
    // 由于防抖用了3次调用，实际按下时间是1002（第3次调用的时间）
    // 所以500ms后查询，应该是1502
    hal_get_tick_ExpectAndReturn(1502);
    uint32_t duration = button_get_press_duration();
    TEST_ASSERT_UINT32_WITHIN(2, 500, duration);  // 允许±2ms误差
}

// 测试5：未按下返回0
void test_get_press_duration_should_return_zero_when_not_pressed(void)
{
    uint32_t duration = button_get_press_duration();
    TEST_ASSERT_EQUAL_UINT32(0, duration);
}

// 测试6：事件清除
void test_event_should_be_cleared_after_read(void)
{
    // 按下（3次）
    for (int i = 0; i < 3; i++) {
        hal_get_button_status_ExpectAndReturn(true);
        hal_get_tick_ExpectAndReturn(1000 + i);
        log_button_event_Expect(1000 + i);
        hal_clear_interrupt_flag_Expect();
        button_isr_handler();
    }
    
    button_event_t event1 = button_get_event();
    TEST_ASSERT_EQUAL_INT(BUTTON_EVENT_PRESSED, event1);
    
    button_event_t event2 = button_get_event();
    TEST_ASSERT_EQUAL_INT(BUTTON_EVENT_NONE, event2);
}

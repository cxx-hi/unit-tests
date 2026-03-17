
#include "unity.h"
#include "led_controller.h"
#include "mock_gpio_interface.h"

void setUp(void)
{
    // setUp中不调用led_controller_init，因为每个测试会自己调用
}

void tearDown(void)
{
}

// 测试初始化
void test_led_controller_init_should_configure_gpio(void)
{
    // 设置期望：init_output被调用一次，write被调用一次
    gpio_init_output_Expect(5);
    gpio_write_Expect(5, false);
    
    led_controller_init();  // 测试中调用init
}

// 测试打开LED
void test_led_set_state_on_should_turn_led_on(void)
{
    // 先初始化（设置期望）
    gpio_init_output_Expect(5);
    gpio_write_Expect(5, false);
    led_controller_init();
    
    // 测试打开LED
    gpio_write_Expect(5, true);
    led_set_state(LED_ON);
    
    TEST_ASSERT_EQUAL_INT(LED_ON, led_get_state());
}

// 测试关闭LED
void test_led_set_state_off_should_turn_led_off(void)
{
    // 初始化
    gpio_init_output_Expect(5);
    gpio_write_Expect(5, false);
    led_controller_init();
    
    // 先打开
    gpio_write_Expect(5, true);
    led_set_state(LED_ON);
    
    // 再关闭
    gpio_write_Expect(5, false);
    led_set_state(LED_OFF);
    
    TEST_ASSERT_EQUAL_INT(LED_OFF, led_get_state());
}

// 测试闪烁
void test_led_blinking_should_toggle_after_5_ticks(void)
{
    // 初始化
    gpio_init_output_Expect(5);
    gpio_write_Expect(5, false);
    led_controller_init();
    
    // 设置为闪烁状态
    led_set_state(LED_BLINKING);
    
    // 前4次tick不应该有gpio_write
    for (int i = 0; i < 4; i++) {
        led_timer_tick();
    }
    
    // 第5次tick应该切换一次
    gpio_write_Expect(5, true);
    led_timer_tick();
    
    // 再5次tick，应该再切换一次
    for (int i = 0; i < 4; i++) {
        led_timer_tick();
    }
    gpio_write_Expect(5, false);
    led_timer_tick();
}

// 新增：测试保持ON状态时tick无操作
void test_led_timer_tick_should_do_nothing_when_not_blinking(void)
{
    // 初始化
    gpio_init_output_Expect(5);
    gpio_write_Expect(5, false);
    led_controller_init();
    
    // 设置为ON
    gpio_write_Expect(5, true);
    led_set_state(LED_ON);
    
    // tick不应该有任何操作
    led_timer_tick();
}
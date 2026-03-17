
#include "led_controller.h"
#include "gpio_interface.h"

#define LED_PIN 5  // 使用PA5

static led_state_t current_state = LED_OFF;
static bool led_physical_state = false;
static uint8_t blink_counter = 0;

void led_controller_init(void)
{
    gpio_init_output(LED_PIN);  //调用接口层
    gpio_write(LED_PIN, false);  //初始关闭LED
    current_state = LED_OFF;
    led_physical_state = false;
    blink_counter = 0;
}

void led_set_state(led_state_t state)
{
    current_state = state;
    blink_counter = 0;  // 重置闪烁计数器
    
    // 立即更新LED
    if (state == LED_ON) {
        gpio_write(LED_PIN, true);
        led_physical_state = true;
    } else if (state == LED_OFF) {
        gpio_write(LED_PIN, false);
        led_physical_state = false;
    }
    // LED_BLINKING 状态在定时器中断中处理
}

led_state_t led_get_state(void)
{
    return current_state;
}

void led_timer_tick(void)
{
    if (current_state != LED_BLINKING) {
        return;
    }
    
    blink_counter++;
    if (blink_counter >= 5) {  // 500ms 切换一次（每100ms tick一次）5个tick切换一次
        blink_counter = 0;
        led_physical_state = !led_physical_state;
        gpio_write(LED_PIN, led_physical_state);
    }
}

#include "unity.h"
#include "simon_game.h"
#include "mock_hal_interface.h"

/*=============================================================================
 * 全局变量
 *============================================================================*/
static uint32_t mock_time = 0;

// LED调用记录
static int led_on_call_count = 0;
static uint8_t led_on_params[20];

// 音效调用记录
static int sound_tone_call_count = 0;
static uint16_t sound_tone_freqs[20];

/*=============================================================================
 * Stub函数 - 只用于测试3（播放序列）
 *============================================================================*/
void my_led_on_stub(uint8_t led_index, int cmock_num_calls)
{
    led_on_call_count++;
    led_on_params[led_on_call_count - 1] = led_index;
}

void my_sound_tone_stub(uint16_t frequency, uint32_t duration_ms, int cmock_num_calls)
{
    sound_tone_call_count++;
    sound_tone_freqs[sound_tone_call_count - 1] = frequency;
}

uint32_t my_timer_millis_stub(int cmock_num_calls)
{
    return mock_time;
}

/*=============================================================================
 * 测试夹具
 *============================================================================*/
static simon_game_t game;

void setUp(void)
{
    mock_hal_interface_Init();
    simon_game_init(&game);
    mock_time = 0;
    led_on_call_count = 0;
    sound_tone_call_count = 0;
}

void tearDown(void)
{
    mock_hal_interface_Verify();
    mock_hal_interface_Destroy();
}

/*=============================================================================
 * 辅助函数
 *============================================================================*/
void setup_sequence(uint8_t* seq, uint8_t len)
{
    for (int i = 0; i < len; i++) {
        game.sequence[i] = seq[i];
    }
    game.sequence_length = len;
}

/*=============================================================================
 * 测试用例
 *============================================================================*/

/**
 * 测试1：初始化状态
 */
void test_init_should_reset_game_state(void)
{
    TEST_ASSERT_EQUAL_INT(0, simon_game_get_score(&game));
    TEST_ASSERT_EQUAL_INT(GAME_STATE_IDLE, simon_game_get_state(&game));
}

/**
 * 测试2：新回合添加随机颜色
 */
void test_new_round_should_add_random_color(void)
{
    random_value_ExpectAndReturn(NUM_COLORS, 2);
    display_number_Expect(0);
    timer_millis_ExpectAndReturn(1000);
    
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(GAME_STATE_PLAYING_SEQUENCE, game.state);
    TEST_ASSERT_EQUAL_INT(1, game.sequence_length);
    TEST_ASSERT_EQUAL_INT(2, game.sequence[0]);
}

/**
 * 测试3：播放序列 - 使用 Stub
 */
void test_play_sequence_should_light_leds_in_order(void)
{
    // 为这个测试设置 Stub
    led_on_Stub(my_led_on_stub);
    sound_tone_Stub(my_sound_tone_stub);
    timer_millis_Stub(my_timer_millis_stub);
    
    //预设序列
    uint8_t seq[] = {1, 2, 0};
    setup_sequence(seq, 3);
    
    game.state = GAME_STATE_PLAYING_SEQUENCE;
    game.play_index = 0;
    game.last_action_time = 0;
    
    // 第1次调用
    mock_time = 400;
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(1, game.play_index);
    TEST_ASSERT_EQUAL_INT(1, led_on_call_count);
    TEST_ASSERT_EQUAL_INT(1, led_on_params[0]);
    
    // 第2次调用
    mock_time = 800;
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(2, game.play_index);
    TEST_ASSERT_EQUAL_INT(2, led_on_call_count);
    TEST_ASSERT_EQUAL_INT(2, led_on_params[1]);
    
    // 第3次调用
    mock_time = 1200;
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(3, game.play_index);
    TEST_ASSERT_EQUAL_INT(3, led_on_call_count);
    TEST_ASSERT_EQUAL_INT(0, led_on_params[2]);
    
    // 第4次调用
    mock_time = 1600;
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(GAME_STATE_WAITING_INPUT, game.state);
}

/**
 * 测试4：正确输入 - 使用 Expect
 */
void test_correct_input_should_advance_sequence(void)
{
    uint8_t seq[] = {1, 2};
    setup_sequence(seq, 2);
    
    game.state = GAME_STATE_WAITING_INPUT;
    game.current_input_index = 0;
    
    // 按第一个按钮
    led_on_Expect(1);
    timer_delay_Expect(100);
    led_off_Expect(1);
    
    simon_game_button_pressed(&game, 1);
    
    TEST_ASSERT_EQUAL_INT(1, game.current_input_index);
    TEST_ASSERT_EQUAL_INT(GAME_STATE_WAITING_INPUT, game.state);
    
    // 按第二个按钮
    led_on_Expect(2);
    timer_delay_Expect(100);
    led_off_Expect(2);
    
    simon_game_button_pressed(&game, 2);
    
    TEST_ASSERT_EQUAL_INT(GAME_STATE_LEVEL_UP, game.state);
    TEST_ASSERT_TRUE(game.player_correct);
}

/**
 * 测试5：错误输入 - 使用 Expect
 */
void test_wrong_input_should_end_game(void)
{
    uint8_t seq[] = {1};
    setup_sequence(seq, 1);
    
    game.state = GAME_STATE_WAITING_INPUT;
    game.current_input_index = 0;
    
    led_on_Expect(2);
    timer_delay_Expect(100);
    led_off_Expect(2);
    
    simon_game_button_pressed(&game, 2);
    
    TEST_ASSERT_EQUAL_INT(GAME_STATE_GAME_OVER, game.state);
    TEST_ASSERT_FALSE(game.player_correct);
}

/**
 * 测试6：输入超时 - 使用 Expect
 */
void test_input_timeout_should_end_game(void)
{
    game.state = GAME_STATE_WAITING_INPUT;
    game.state_start_time = 1000;
    
    mock_time = 5000;
    timer_millis_ExpectAndReturn(5000);
    simon_game_update(&game);
    TEST_ASSERT_EQUAL_INT(GAME_STATE_WAITING_INPUT, game.state);
    
    mock_time = 6001;
    timer_millis_ExpectAndReturn(6001);
    simon_game_update(&game);
    TEST_ASSERT_EQUAL_INT(GAME_STATE_GAME_OVER, game.state);
}

/**
 * 测试7：过关处理 - 使用 Expect
 */
void test_level_up_should_play_melody(void)
{
    game.sequence_length = 3;
    game.state = GAME_STATE_LEVEL_UP;
    
    sound_tone_Expect(262, 150);
    timer_delay_Expect(200);
    sound_tone_Expect(330, 150);
    timer_delay_Expect(200);
    sound_tone_Expect(392, 150);
    timer_delay_Expect(200);
    sound_tone_Expect(523, 150);
    timer_delay_Expect(200);
    
    display_number_Expect(2);
    
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(GAME_STATE_IDLE, game.state);
}

/**
 * 测试8：游戏结束 - 使用 Expect
 */
void test_game_over_should_print_score_and_reset(void)
{
    game.sequence_length = 5;
    game.state = GAME_STATE_GAME_OVER;
    
    debug_print_Expect("Game over! Score: ");
    debug_print_number_Expect(4);
    
    sound_tone_Expect(262, 300);
    timer_delay_Expect(300);
    sound_tone_Expect(196, 500);
    
    display_dash_Expect();
    timer_delay_Expect(2000);
    
    display_number_Expect(0);
    
    simon_game_update(&game);
    
    TEST_ASSERT_EQUAL_INT(0, game.sequence_length);
    TEST_ASSERT_EQUAL_INT(GAME_STATE_IDLE, game.state);
}
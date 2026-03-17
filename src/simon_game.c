#include "simon_game.h"
#include "hal_interface.h"  // 直接包含硬件接口

#define LED_DURATION 300
#define LED_INTERVAL 400
#define INPUT_TIMEOUT 5000
#define NOTE_BASE 440

// 私有函数（不变）
static uint16_t get_tone_for_color(uint8_t color)
{
    switch (color) {
        case 0: return NOTE_BASE;
        case 1: return NOTE_BASE + 100;
        case 2: return NOTE_BASE + 200;
        case 3: return NOTE_BASE + 300;
        default: return NOTE_BASE;
    }
}

// 修改：不再通过hal指针调用，直接调用全局函数
static void start_new_round(simon_game_t* game)
{
    // 1. 生成新的随机颜色 - 直接调用random_value()
    uint8_t new_color = random_value(NUM_COLORS);
    
    // 2. 添加到序列末尾
    if (game->sequence_length < MAX_SEQUENCE_LENGTH) {
        game->sequence[game->sequence_length] = new_color;
        game->sequence_length++;
    }
    
    // 3. 更新分数显示 - 直接调用display_number()
    display_number(simon_game_get_score(game));

    // 4. 进入播放序列状态
    game->state = GAME_STATE_PLAYING_SEQUENCE;
    game->play_index = 0;
    game->last_action_time = timer_millis();  // 直接调用
}

static void play_sequence(simon_game_t* game)
{
    uint32_t now = timer_millis();  // 直接调用
    
    if (game->play_index < game->sequence_length) {
        if (now - game->last_action_time >= LED_INTERVAL) {
            uint8_t color = game->sequence[game->play_index];
            
            // 直接调用led_on()和sound_tone()
            led_on(color);
            sound_tone(get_tone_for_color(color), LED_DURATION);
            
            game->play_index++;
            game->last_action_time = now;
        }
    } else {
        game->state = GAME_STATE_WAITING_INPUT;
        game->current_input_index = 0;
        game->state_start_time = now;
    }
}

static void wait_for_input(simon_game_t* game)
{
    uint32_t now = timer_millis();  // 直接调用
    
    if (now - game->state_start_time > INPUT_TIMEOUT) {
        game->state = GAME_STATE_GAME_OVER;
    }
}

static void handle_level_up(simon_game_t* game)
{
    const uint16_t melody[] = {262, 330, 392, 523};
    
    for (int i = 0; i < 4; i++) {
        // 直接调用sound_tone()和timer_delay()
        sound_tone(melody[i], 150);
        timer_delay(200);
    }
    
    display_number(simon_game_get_score(game));  // 直接调用
    game->state = GAME_STATE_IDLE;
}

static void handle_game_over(simon_game_t* game)
{
    // 直接调用debug函数
    debug_print("Game over! Score: ");
    debug_print_number(simon_game_get_score(game));
    
    // 播放失败音效
    sound_tone(262, 300);
    timer_delay(300);
    sound_tone(196, 500);
    
    // 显示"--"
    display_dash();
    timer_delay(2000);
    
    // 重置游戏
    simon_game_reset(game);
    display_number(0);
}

// 公开API实现（修改：去掉hal参数）
void simon_game_init(simon_game_t* game)
{
    game->sequence_length = 0;
    game->current_input_index = 0;
    game->state = GAME_STATE_IDLE;
    game->player_correct = false;
    game->state_start_time = 0;
    game->last_action_time = 0;
    game->play_index = 0;
}

void simon_game_reset(simon_game_t* game)
{
    game->sequence_length = 0;
    game->current_input_index = 0;
    game->state = GAME_STATE_IDLE;
    game->player_correct = false;
    game->play_index = 0;
}

uint8_t simon_game_get_score(const simon_game_t* game)
{
    return (game->sequence_length > 0) ? (game->sequence_length - 1) : 0;
}

game_state_t simon_game_get_state(const simon_game_t* game)
{
    return game->state;
}

void simon_game_button_pressed(simon_game_t* game, uint8_t button_index)
{
    if (game->state != GAME_STATE_WAITING_INPUT) return;
    if (game->current_input_index >= game->sequence_length) return;
    
    uint8_t expected = game->sequence[game->current_input_index];
    
    // 反馈 - 直接调用
    led_on(button_index);
    timer_delay(100);
    led_off(button_index);
    
    if (button_index == expected) {
        game->current_input_index++;
        
        if (game->current_input_index >= game->sequence_length) {
            game->player_correct = true;
            game->state = GAME_STATE_LEVEL_UP;
        }
    } else {
        game->player_correct = false;
        game->state = GAME_STATE_GAME_OVER;
    }
}

void simon_game_update(simon_game_t* game)
{
    switch (game->state) {
        case GAME_STATE_IDLE:
            start_new_round(game);
            break;
        case GAME_STATE_PLAYING_SEQUENCE:
            play_sequence(game);
            break;
        case GAME_STATE_WAITING_INPUT:
            wait_for_input(game);
            break;
        case GAME_STATE_LEVEL_UP:
            handle_level_up(game);
            break;
        case GAME_STATE_GAME_OVER:
            handle_game_over(game);
            break;
        default:
            break;
    }
}
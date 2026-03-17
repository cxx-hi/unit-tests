#ifndef SIMON_GAME_H
#define SIMON_GAME_H

#include <stdint.h>
#include <stdbool.h>

// 不再需要包含hal_interface.h，但需要声明这些函数存在
// 这些函数会在链接时提供（真实硬件或Mock）

#define MAX_SEQUENCE_LENGTH 100
#define NUM_COLORS 4

// 游戏状态枚举（不变）
typedef enum {
    GAME_STATE_IDLE,
    GAME_STATE_PLAYING_SEQUENCE,
    GAME_STATE_WAITING_INPUT,
    GAME_STATE_LEVEL_UP,
    GAME_STATE_GAME_OVER
} game_state_t;

// 游戏上下文（去掉了hal指针）
typedef struct {
    uint8_t sequence[MAX_SEQUENCE_LENGTH];
    uint8_t sequence_length;
    uint8_t current_input_index;
    
    game_state_t state;
    bool player_correct;
    
    uint32_t state_start_time;
    uint32_t last_action_time;
    uint8_t play_index;
    
    // 不再需要 const hal_t* hal，直接调用全局函数
} simon_game_t;

// API函数（不变）
void simon_game_init(simon_game_t* game);
void simon_game_update(simon_game_t* game);
void simon_game_button_pressed(simon_game_t* game, uint8_t button_index);
uint8_t simon_game_get_score(const simon_game_t* game);
game_state_t simon_game_get_state(const simon_game_t* game);
void simon_game_reset(simon_game_t* game);

#endif
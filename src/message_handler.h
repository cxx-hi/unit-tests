
#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

// 消息结构
typedef struct {
    uint8_t id;
    uint16_t data;
    uint32_t timestamp;
} message_t;

// 初始化消息处理器
void message_handler_init(void);

void message_handler_reset(void);

// 发送消息
bool message_send(uint8_t id, uint16_t data);

// 接收消息（阻塞，等待指定ticks）
bool message_receive(message_t *msg, uint32_t timeout_ticks);

// 获取队列中等待的消息数量
uint32_t message_get_pending_count(void);

// 处理所有待处理消息（假设被周期调用）
void message_process_all(void);

#endif

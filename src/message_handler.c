#include <stddef.h>
#include "message_handler.h"
#include "freertos_queue.h"

#define MESSAGE_QUEUE_LENGTH 10
#define MESSAGE_QUEUE_ITEM_SIZE sizeof(message_t)

static QueueHandle_t message_queue = NULL;  //内部静态，所有测试共享，添加message_handle_reset()函数，每个测试前重置

void message_handler_init(void)
{
    if (message_queue == NULL) {
        message_queue = xQueueCreate(MESSAGE_QUEUE_LENGTH, MESSAGE_QUEUE_ITEM_SIZE);
    }
}

void message_handler_reset(void)
{
    message_queue = NULL;
}

bool message_send(uint8_t id, uint16_t data)
{
    if (message_queue == NULL) {
        return false;
    }
    
    message_t msg = {
        .id = id,
        .data = data,
        .timestamp = 0  // 实际项目中可能是系统滴答计数
    };
    
    // 发送到队列，等待最大100ms
    return xQueueSend(message_queue, &msg, 100);
}

bool message_receive(message_t *msg, uint32_t timeout_ticks)
{
    if (message_queue == NULL || msg == NULL) {
        return false;
    }
    
    return xQueueReceive(message_queue, msg, timeout_ticks);
}

uint32_t message_get_pending_count(void)
{
    if (message_queue == NULL) {
        return 0;
    }
    
    return uxQueueMessagesWaiting(message_queue);
}

// 处理所有待处理消息
void message_process_all(void)
{
    if (message_queue == NULL) {
        return;
    }
    
    message_t msg;
    // 非阻塞方式接收所有消息
    while (xQueueReceive(message_queue, &msg, 0)) {
        // 根据消息ID做不同处理
        switch (msg.id) {
            case 1:
                // 处理类型1的消息
                break;
            case 2:
                // 处理类型2的消息
                break;
            default:
                // 忽略未知类型
                break;
        }
    }
}
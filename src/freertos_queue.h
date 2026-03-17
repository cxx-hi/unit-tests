#ifndef FREERTOS_QUEUE_H
#define FREERTOS_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

// FreeRTOS队列句柄类型（简化）
typedef void* QueueHandle_t;

// 队列操作API（这些就是我们要mock的函数）
QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);
bool xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, uint32_t xTicksToWait);
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, uint32_t xTicksToWait);
uint32_t uxQueueMessagesWaiting(QueueHandle_t xQueue);

#endif
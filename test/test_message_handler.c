
#include "unity.h"
#include "message_handler.h"
#include "mock_freertos_queue.h"
#include <string.h>

static QueueHandle_t fake_queue_handle = (QueueHandle_t)0x12345678;

// 测试4用的回调函数和全局变量
static message_t g_expected_msg;
static bool g_receive_return;

// 测试6用的全局变量
static message_t g_msg1;
static message_t g_msg2;
static int g_call_count;

// 测试7用的回调函数
bool timeout_callback(QueueHandle_t xQueue, void* pvBuffer, uint32_t xTicksToWait, int cmock_num_calls)
{
    return false;
}

// 测试4的回调函数
bool receive_callback(QueueHandle_t xQueue, void* pvBuffer, uint32_t xTicksToWait, int cmock_num_calls)
{
    if (pvBuffer != NULL) {
        memcpy(pvBuffer, &g_expected_msg, sizeof(message_t));
    }
    return g_receive_return;
}

// 测试6的回调函数
bool process_callback(QueueHandle_t xQueue, void* pvBuffer, uint32_t xTicksToWait, int cmock_num_calls)
{
    g_call_count++;
    if (g_call_count == 1) {
        if (pvBuffer != NULL) memcpy(pvBuffer, &g_msg1, sizeof(message_t));
        return true;
    } else if (g_call_count == 2) {
        if (pvBuffer != NULL) memcpy(pvBuffer, &g_msg2, sizeof(message_t));
        return true;
    } else {
        return false;
    }
}

void setUp(void)
{
    mock_freertos_queue_Init();
    message_handler_reset();
    g_call_count = 0;  // 重置调用计数
}

void tearDown(void)
{
    mock_freertos_queue_Verify();
    mock_freertos_queue_Destroy();
}

// 测试1：初始化应该创建队列
void test_message_handler_init_should_create_queue(void)
{
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
}

// 测试2：发送消息应该调用xQueueSend
void test_message_send_should_call_xQueueSend(void)
{
    // 先初始化
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
    
    // 使用IgnoreAndReturn，不检查参数
    xQueueSend_IgnoreAndReturn(true);
    
    bool result = message_send(5, 1234);
    TEST_ASSERT_TRUE(result);
}

// 测试3：发送消息时如果队列未初始化，应返回false，发送失败且不调用任何RTOS函数
void test_message_send_should_return_false_when_queue_not_initialized(void)
{
    bool result = message_send(5, 1234);
    TEST_ASSERT_FALSE(result);
}

// 测试4：接收消息应该调用xQueueReceive - 使用回调函数
void test_message_receive_should_call_xQueueReceive(void)
{
    // 先初始化
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
    
    // 准备测试数据
    message_t expected_msg = {.id = 3, .data = 5678, .timestamp = 0};
    message_t actual_msg;
    
    // 设置回调函数
    g_expected_msg = expected_msg;
    g_receive_return = true;
    xQueueReceive_Stub(receive_callback);
    
    // 执行接收
    bool result = message_receive(&actual_msg, 50);
    
    // 验证结果
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT8(3, actual_msg.id);
    TEST_ASSERT_EQUAL_UINT16(5678, actual_msg.data);
}

// 测试5：获取等待消息数量，获取队列长度
void test_message_get_pending_count_should_call_uxQueueMessagesWaiting(void)
{
    // 先初始化
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
    
    uxQueueMessagesWaiting_ExpectAndReturn(fake_queue_handle, 5);
    
    uint32_t count = message_get_pending_count();
    TEST_ASSERT_EQUAL_UINT32(5, count);
}

// 测试6：处理所有消息 - 使用回调函数
void test_message_process_all_should_process_until_queue_empty(void)
{
    // 先初始化
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
    
    // 准备测试数据
    g_msg1 = (message_t){.id = 1, .data = 111, .timestamp = 0};
    g_msg2 = (message_t){.id = 2, .data = 222, .timestamp = 0};
    g_call_count = 0;
    
    // 设置回调函数
    xQueueReceive_Stub(process_callback);
    
    // 执行
    message_process_all();
    
    // 验证被调用了3次（2次成功，1次失败）
    TEST_ASSERT_EQUAL_INT(3, g_call_count);
}

// 测试7：超时场景
void test_message_receive_should_handle_timeout(void)
{
    // 先初始化
    xQueueCreate_ExpectAndReturn(10, sizeof(message_t), fake_queue_handle);
    message_handler_init();
    
    // 设置回调函数返回false
    xQueueReceive_Stub(timeout_callback);
    
    message_t msg;
    bool result = message_receive(&msg, 100);
    
    TEST_ASSERT_FALSE(result);
}
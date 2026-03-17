
#define UNITY_INCLUDE_FLOAT
#include "unity.h"
#include "mock_i2c_driver.h"
#include "sensor_reader.h"

// 定义多个回调函数以测试不同场景

// 回调1：模拟成功读取，返回25°C
i2c_status_t success_callback(uint8_t dev_addr, uint8_t* buffer, uint8_t len, int cmock_num_calls)
{
    // 检查参数
    if (dev_addr != 0x48) return I2C_ERROR;
    if (buffer == NULL || len < 2) return I2C_ERROR;
    
    // 返回模拟数据 (25°C)
    buffer[0] = 0x01;
    buffer[1] = 0x90;
    return I2C_OK;
}

// 回调2：模拟成功读取，返回不同的温度值 (比如 30°C)
i2c_status_t another_success_callback(uint8_t dev_addr, uint8_t* buffer, uint8_t len, int cmock_num_calls)
{
    if (buffer != NULL && len >= 2) {
        buffer[0] = 0x01;  // 30°C = 0x01E0 * 0.0625
        buffer[1] = 0xE0;
    }
    return I2C_OK;
}

// 回调3：模拟读取失败
i2c_status_t failure_callback(uint8_t dev_addr, uint8_t* buffer, uint8_t len, int cmock_num_calls)
{
    return I2C_ERROR;
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_read_temperature_should_return_25_degrees_on_success(void)
{
    i2c_read_Stub(success_callback);
    
    float temp = read_temperature();
    TEST_ASSERT_EQUAL_INT(2500, (int)(temp * 100 + 0.5));
}

void test_read_temperature_should_return_30_degrees_on_another_success(void)
{
    i2c_read_Stub(another_success_callback);
    
    float temp = read_temperature();
     TEST_ASSERT_EQUAL_INT(3000, (int)(temp * 100 + 0.5));
}

void test_read_temperature_should_return_error_value_on_failure(void)
{
    i2c_read_Stub(failure_callback);
    
    float temp = read_temperature();
    TEST_ASSERT_EQUAL_INT(-10000, (int)(temp * 100));
}

#include "sensor_reader.h"
#include "i2c_driver.h"

float read_temperature(void) {
    uint8_t data[2];
    if (i2c_read(0x48, data, 2) != I2C_OK) {
        return -100.0f;  // 错误标志
    }
    // 假设传感器返回的是16位温度值，每LSB代表0.0625°C
    int16_t raw = (data[0] << 8) | data[1];
    return raw * 0.0625f;
}
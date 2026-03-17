#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

typedef enum {
    I2C_OK,
    I2C_ERROR
} i2c_status_t;

i2c_status_t i2c_read(uint8_t dev_addr, uint8_t* buffer, uint8_t len);

#endif
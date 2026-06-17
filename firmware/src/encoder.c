#include <math.h>
#include "hardware/i2c.h"
#include "encoder.h"

#define AS5600_ADDR      0x36
#define AS5600_REG_ANGLE 0x0E
#define I2C_PORT         i2c0
#define PIN_SDA          8
#define PIN_SCL          9

void encoder_init(void) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
}

float encoder_read_angle(void) {
    uint8_t reg = AS5600_REG_ANGLE;
    uint8_t buf[2];
    i2c_write_blocking(I2C_PORT, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, AS5600_ADDR, buf, 2, false);
    uint16_t raw = ((uint16_t)buf[0] << 8) | buf[1];
    return (raw & 0x0FFF) * (2.0f * M_PI / 4096.0f);
}

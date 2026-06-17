#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

/* ---------- pin assignments ---------- */
#define PIN_PWM_A_HI  0
#define PIN_PWM_A_LO  1
#define PIN_PWM_B_HI  2
#define PIN_PWM_B_LO  3
#define PIN_PWM_C_HI  4
#define PIN_PWM_C_LO  5
#define PIN_SDA       8
#define PIN_SCL       9

/* ---------- constants ---------- */
#define PWM_WRAP         2499   /* 125 MHz / (2499+1) / 2 = 25 kHz centre-aligned */
#define COUNTS_TO_AMPS   (3.3f / 4096.0f / 0.1f)  /* INA180A3 gain=100, shunt=1mΩ */
#define AS5600_ADDR      0x36
#define AS5600_REG_ANGLE 0x0E

/* ---------- PWM ---------- */
static void pwm_init_phases(void) {
    uint pins[] = {PIN_PWM_A_HI, PIN_PWM_A_LO, PIN_PWM_B_HI, PIN_PWM_B_LO, PIN_PWM_C_HI, PIN_PWM_C_LO};
    for (int i = 0; i < 6; i++) {
        gpio_set_function(pins[i], GPIO_FUNC_PWM);
        uint slice = pwm_gpio_to_slice_num(pins[i]);
        pwm_set_wrap(slice, PWM_WRAP);
        pwm_set_enabled(slice, true);
    }
}

static void pwm_set_duties(float da, float db, float dc) {
    pwm_set_gpio_level(PIN_PWM_A_HI, (uint16_t)(da * PWM_WRAP));
    pwm_set_gpio_level(PIN_PWM_B_HI, (uint16_t)(db * PWM_WRAP));
    pwm_set_gpio_level(PIN_PWM_C_HI, (uint16_t)(dc * PWM_WRAP));
}

/* ---------- current sensing ---------- */
static void adc_init_current(void) {
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
}

static void adc_read_currents(float *ia, float *ib, float *ic) {
    adc_select_input(0); *ia = adc_read() * COUNTS_TO_AMPS;
    adc_select_input(1); *ib = adc_read() * COUNTS_TO_AMPS;
    adc_select_input(2); *ic = adc_read() * COUNTS_TO_AMPS;
}

/* ---------- encoder ---------- */
static void encoder_init(void) {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
}

static float encoder_read_angle(void) {
    uint8_t reg = AS5600_REG_ANGLE;
    uint8_t buf[2];
    i2c_write_blocking(i2c0, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, AS5600_ADDR, buf, 2, false);
    uint16_t raw = ((uint16_t)buf[0] << 8) | buf[1];
    return (raw & 0x0FFF) * (2.0f * M_PI / 4096.0f);
}

/* ---------- FOC ---------- */
typedef struct {
    float id, iq;
    float vd, vq;
    float duty_a, duty_b, duty_c;
} foc_state_t;

static void foc_update(foc_state_t *s, float angle, float ia, float ib, float ic) {
    float alpha = ia;
    float beta  = (ia + 2.0f * ib) / sqrtf(3.0f);

    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    s->id =  alpha * cos_a + beta * sin_a;
    s->iq = -alpha * sin_a + beta * cos_a;

    /* PI controllers for id/iq go here */

    float out_alpha = s->vd * cos_a - s->vq * sin_a;
    float out_beta  = s->vd * sin_a + s->vq * cos_a;

    s->duty_a = 0.5f + out_alpha * 0.5f;
    s->duty_b = 0.5f + (-0.5f * out_alpha + (sqrtf(3.0f) / 2.0f) * out_beta) * 0.5f;
    s->duty_c = 0.5f + (-0.5f * out_alpha - (sqrtf(3.0f) / 2.0f) * out_beta) * 0.5f;
}

/* ---------- main ---------- */
int main(void) {
    stdio_init_all();

    pwm_init_phases();
    adc_init_current();
    encoder_init();

    foc_state_t foc = {0};

    while (true) {
        float angle = encoder_read_angle();
        float ia, ib, ic;
        adc_read_currents(&ia, &ib, &ic);
        foc_update(&foc, angle, ia, ib, ic);
        pwm_set_duties(foc.duty_a, foc.duty_b, foc.duty_c);
    }
}

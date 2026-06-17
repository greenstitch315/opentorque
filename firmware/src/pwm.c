#include "hardware/pwm.h"
#include "pwm.h"

/* Assign GPIO pins once schematic pin mapping is confirmed */
#define PIN_PWM_A_HI  0
#define PIN_PWM_A_LO  1
#define PIN_PWM_B_HI  2
#define PIN_PWM_B_LO  3
#define PIN_PWM_C_HI  4
#define PIN_PWM_C_LO  5

#define PWM_WRAP 2499  /* 125 MHz / (2499+1) / 2 = 25 kHz centre-aligned */

static void init_pin(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_enabled(slice, true);
}

void pwm_init_phases(void) {
    init_pin(PIN_PWM_A_HI); init_pin(PIN_PWM_A_LO);
    init_pin(PIN_PWM_B_HI); init_pin(PIN_PWM_B_LO);
    init_pin(PIN_PWM_C_HI); init_pin(PIN_PWM_C_LO);
}

void pwm_set_duties(float da, float db, float dc) {
    pwm_set_gpio_level(PIN_PWM_A_HI, (uint16_t)(da * PWM_WRAP));
    pwm_set_gpio_level(PIN_PWM_B_HI, (uint16_t)(db * PWM_WRAP));
    pwm_set_gpio_level(PIN_PWM_C_HI, (uint16_t)(dc * PWM_WRAP));
}

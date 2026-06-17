#include "pico/stdlib.h"
#include "foc.h"
#include "pwm.h"
#include "adc.h"
#include "encoder.h"

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
